/*********************************************************************/
/*                  pC++/Sage++  Copyright (C) 1993                  */
/*  Indiana University  University of Oregon  University of Rennes   */
/*********************************************************************/


/****************************************************************
 *								*
 *  db.c:							*
 *								*
 *  contains miscellaneous routines to handle inquiries to the	*
 *  program date base.  Supposed to be a higher level interface	*
 *								*
 ****************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include "compatible.h"
#ifdef SYS5
#include <string.h>
#else
#include <strings.h>
#endif
 
#include "db.h"

#ifdef __SPF
extern void addToCollection(const int line, const char *file, void *pointer, int type);
extern void removeFromCollection(void *pointer);
#endif

/*
 * external references
 */
extern int debug;
extern int language;

int	 read_nodes();
int	 test_mod_ref();	/* in "mod_ref.c" */
int	 check_ref();
void	 build_ref(),
	 visit_llnd();

char	*(* unparse_bfnd)();	/* routine to unparse BIF nodes */
char	*(* unparse_llnd)();	/* routine to unparse Low level nodes */
char	*(* unparse_symb)();	/* routine to unparse Symbol nodes */
char	*(* unparse_type)();	/* routine to unparse Type nodes */
void	 readnodes();
void	 gen_udchain();
void	 dump_udchain();
PTR_BLOB  alloc_blob();
PTR_BLOB1 make_blob1();
PTR_INFO  make_obj_info();

PTR_BFND make_bfnd();
PTR_TYPE make_type();
PTR_SYMB make_symb();

char *funparse_bfnd(),		/* bif nodes unparser for Fortran */
     *funparse_blck(),		/* unparse the whole block for Fortran */
     *funparse_llnd(),		/* ll nodes unparser for Fortran */
     *funparse_symb(),		/* symbol nodes unparser for Fortran */
     *funparse_type(),		/* type nodes unparser for Fortran */
     *cunparse_bfnd(),          /* bif nodes unparser for C */ 
     *cunparse_blck(),		/* unparse the whole block for C */
     *cunparse_llnd(),          /* ll nodes unparser for C */  
     *cunparse_symb(),          /* symbol nodes unparser for C */
     *cunparse_type();          /* type nodes unparser for C */

/*
 * Global variables to be shared by other routines
 */

/*
 * Here we put unparsers of various kind of nodes into an array
 * indexed by the language type:
 *
 *   (*UnparseBfnd[ForSrc])();   calls the bif node unparser for Fortran
 *   (*UnparseBfnd[CSrc])();	  calls the bif node unparser for C
 */

/* typedef char *(*PCF)(); */

PCF UnparseBfnd[] = {
	funparse_bfnd,
	cunparse_bfnd
};

PCF UnparseBlock[] = {
	funparse_blck,
	cunparse_blck
};

PCF UnparseLlnd[] = {
	funparse_llnd,
	cunparse_llnd
};

PCF UnparseSymb[] = {
	funparse_symb,
	cunparse_symb
};

PCF UnparseType[] = {
	funparse_type,
	cunparse_type
};


/*
 * global variables
 */
PTR_BLOB head_proj;		/* pointer to the project header */
PTR_PROJ cur_proj = NULL;	/* point to the current active project */
PTR_FILE cur_file = NULL;	/* point to the current active file */
char db_err_msg[100];


/*
 * local variables
 */
static PTR_HASH hash_table[hashMax];
static PTR_BLOB1 obj, tail;
static int skip_rest = 0;	/* set to 1 if one proc/func ref found in llnd */

/*
 * last_char returns the last character of the given NON-EMPTY string
 */
static char
last_char(s)
    register char *s;
{
    while (*s++);
    return *(s-2);
}


/****************************************************************
 *								*
 *   init_hash -- initialize the hash table			*
 *								*
 *   Input:							*
 *	hash_tbl - pointer to the hash table to be initializes	*
 *								*
 ****************************************************************/
/*static void
init_hash(hash_tbl)
	PTR_HASH hash_tbl[];
{
    register int i = hashMax;
    register PTR_HASH *p = hash_tbl;

    for (; i; --i)
	*p++ = (PTR_HASH) NULL;
}*/


/****************************************************************
 *								*
 *   hash -- computes the hash value of a given string		*
 *								*
 *   Input:							*
 *		str - a character string			*
 *								*
 *   Output:							*
 *		an integer representing the hash value of the	*
 *		given string					*
 *								*
 ****************************************************************/
static int 
hash(str)
	register char *str;
{
    register int i;

    for (i = 0; *str;)
	i += *str++;
    return (i % hashMax);
}


/****************************************************************
 *								*
 *  insert_hash -- insert the given symbol table entry into	*
 *		   the hash table				*
 *  input:							*
 *	   symb - the symbol entry to be inserted		*
 *	   head_hash - start of hash table			*
 *								*
 ****************************************************************/
static void
insert_hash(symb, head_hash)
	register PTR_SYMB symb;
	PTR_HASH head_hash[];
{
    int    index;
    PTR_HASH entry;

    index = hash(symb->ident);
    if ((entry = (PTR_HASH)calloc(1, sizeof(struct hash_entry))) != 0)
    {
#ifdef __SPF
        addToCollection(__LINE__, __FILE__,entry, 0);
#endif
        entry->id_attr = symb;
        entry->next_entry = head_hash[index];
        head_hash[index] = entry;
    }
    else
        (void)strcpy(db_err_msg, "No more space");
}


/****************************************************************
 *								*
 *  build_hash -- build the hash table for all symbols in the	*
 *		  project					*
 *								*
 *  Inputs:							*
 *	   head_symb - starting point of the symbol entries	*
 *	   head_hash - starting point of the hash table 	*
 *								*
 ****************************************************************/
static void
build_hash(head_symb, head_hash)
	PTR_SYMB head_symb;
	PTR_HASH head_hash[];
{
    register PTR_SYMB s;

    for (s = head_symb; s; s = s->thread)
	insert_hash(s, head_hash);
}


/****************************************************************
 *								*
 *  append_blob1_nd -- append b2 to the end of b1		*
 *								*
 *  Inputs:							*
 *	   b1 - head of the blob1 list				*
 *	   b2 - second list to be appended to b1		*
 *								*
 *  Output:							*
 *	   a blob1 list with b2 appended to end of b1		*
 *								*
 ****************************************************************/
static PTR_BLOB1
append_blob1_nd(b1, b2)
	PTR_BLOB1 b1, b2;
{
    if (b1) {
	register PTR_BLOB1 p, q;

	for (p=b1; p; p = p->next) /* skip to the end of b1 */
	    q = p;
	q->next = b2;
    } else
	b1 = b2;
    return b1;
}


/****************************************************************
 *								*
 *  insert_info_nd -- insert an info node to the return list	*
 *								*
 *  Input:							*
 *	   new - new info node to be added to the list		*
 *								*
 *  Side Effects:						*
 *	   The new node was added to the end of list pointed	*
 *	   to by the global variable "tail".  It changes the	*
 *	   global variable "obj", too, if the list was empty	*
 *								*
 ****************************************************************/
static void
insert_info_nd(new)
	PTR_BLOB1 new;
{
    if (obj == NULL)
	obj = tail = new;
    else {
	tail->next = new;
	tail = new;
    }
}


/****************************************************************
 *								*
 *   check_llnd -- traverse the given low level node "llnd"	*
 *		   for the USE or MOD information about the	*
 *		   symbol "var_name"				*
 *								*
 *   Inputs:							*
 *	   bf   - bif node					*
 *	   llnd - the low level node to be searched		*
 *	   type - type of information wanted			*
 *	   var_name - the given variable name			*
 *								*
 *   Side effect:						*
 *	   add a new obj_info node to the reference list	*
 *								*
 ****************************************************************/
static void
check_llnd(bf, llnd, type, var_name)
	PTR_BFND  bf;
	PTR_LLND llnd;
	int	 type;
	char	*var_name;
{
    if (llnd == NULL) return;

    switch (llnd->variant) {
    case LABEL_REF:
	break;
    case CONST_REF:
    case VAR_REF	:
    case ARRAY_REF:
	if(check_ref(llnd->entry.Template.symbol->id) == 0)
	    ;
	build_ref(llnd->entry.Template.symbol, bf);
	break;
    case CONSTRUCTOR_REF:
	break;
    case ACCESS_REF:
	break;
    case CONS:
	break;
    case ACCESS:
	break;
    case IOACCESS	:
	break;
    case PROC_CALL:
    case FUNC_CALL:
	visit_llnd(bf,llnd->entry.proc.param_list);
	break;
    case EXPR_LIST:
	visit_llnd(bf,llnd->entry.list.item);
	if (llnd->entry.list.next)
	    visit_llnd(bf,llnd->entry.list.next);
	break;
    case EQUI_LIST:
	visit_llnd(bf,llnd->entry.list.item);
	if (llnd->entry.list.next)
	    visit_llnd(bf,llnd->entry.list.next);
	break;
    case COMM_LIST:
	if (llnd->entry.Template.symbol) {
	    /*	addstr(llnd->entry.Template.symbol->ident);
	     */
	}
	visit_llnd(bf,llnd->entry.list.item);
	if (llnd->entry.list.next)
	    visit_llnd(bf,llnd->entry.list.next);
	break;
    case VAR_LIST	:
    case RANGE_LIST:
    case CONTROL_LIST:
	visit_llnd(bf,llnd->entry.list.item);
	if (llnd->entry.list.next)
	    visit_llnd(bf,llnd->entry.list.next);
	break;
    case DDOT:
	visit_llnd(bf,llnd->entry.binary_op.l_operand);
	if (llnd->entry.binary_op.r_operand)
	    visit_llnd(bf,llnd->entry.binary_op.r_operand);
	break;
    case DEF_CHOICE:
    case SEQ:
	visit_llnd(bf,llnd->entry.seq.ddot);
	if (llnd->entry.seq.stride)
	    visit_llnd(bf,llnd->entry.seq.stride);
	break;
    case SPEC_PAIR:
	visit_llnd(bf,llnd->entry.spec_pair.sp_label);
	visit_llnd(bf,llnd->entry.spec_pair.sp_value);
	break;
    case EQ_OP:
    case LT_OP:
    case GT_OP:
    case NOTEQL_OP:
    case LTEQL_OP:
    case GTEQL_OP:
    case ADD_OP:
    case SUBT_OP:
    case OR_OP:
    case MULT_OP:
    case DIV_OP:
    case MOD_OP:
    case AND_OP:
    case EXP_OP:
    case CONCAT_OP:
	visit_llnd(bf,llnd->entry.binary_op.l_operand);
	visit_llnd(bf,llnd->entry.binary_op.r_operand);
	break;
    case MINUS_OP:
    case NOT_OP:
	visit_llnd(bf,llnd->entry.unary_op.operand);
	break;
    case STAR_RANGE:
	break;
    default:
	break;
    }
}


/****************************************************************
 *								*
 * proc_ref_in_llnd -- recursively traverses the given low level*
 *		       node to find all procedures or functions *
 *		       references in it 			*
 *								*
 *  Input:							*
 *	   fi  - the file obj where this bif node belongs to	*
 *	   bif - the bif node where the llnd belongs		*
 *	   ll  - the low level node to be checked		*
 *								*
 *  Side Effect:						*
 *	   a blob1 list that contains all the call sites under	*
 *	   the node "ll" is put on the GLOBAL variable "obj".	*
 *								*
 ****************************************************************/
static void
proc_ref_in_llnd(fi, bif, ll)
	PTR_FILE fi;
	PTR_BFND bif;
	PTR_LLND ll;
{
    if (ll == NULL)
	return;

    if (ll->variant == FUNC_CALL || ll->variant == PROC_CALL || ll->variant == FUNCTION_REF) {
	PTR_INFO inf;
	char	*bp, *t;

	t = (UnparseBfnd[language])(bif);
	skip_rest = 1;
	bp = malloc(strlen(t) + 1);
#ifdef __SPF
    addToCollection(__LINE__, __FILE__,bp, 0);
#endif
	(void) strcpy(bp, t);
	inf = make_obj_info(fi->filename, bif->g_line, bif->l_line, bp);
	insert_info_nd(make_blob1(IsObj, inf, NULL));
	return;
    }

    /* NOTE: the following code is "tag" dependent */
    if (ll->variant >= VAR_LIST && ll->variant < CONST_NAME) {
	if (! skip_rest)
	    proc_ref_in_llnd(fi, bif, ll->entry.Template.ll_ptr1);
	if (! skip_rest)
	    proc_ref_in_llnd(fi, bif, ll->entry.Template.ll_ptr2);
    }
}


/****************************************************************
 *								*
 *  find_proc_call -- recursively traverses the given bif node	*
 *		      to find all procedures or functions calls *
 *		      in it.					*
 *								*
 *  Inputs:							*
 *	   fi  - the file obj where this bif node belongs to	*
 *	   bif - the bif node to be checked			*
 *								*
 *  Side effect:						*
 *	   a blob1 list that contains all the call sites under	*
 *	   the node " bif", i.e. itself and all its subtree is	*
 *	   put on the "global" variable "obj"			* 
 *								*
 ****************************************************************/
static void
find_proc_call(fi, bif)
	PTR_FILE fi;
	PTR_BFND bif;
{
    char	 buf[200], *bp, *tmp, *t;
    PTR_INFO inf;
    PTR_BLOB bl;

    if (bif == NULL)
	return;

    bp = buf;
    switch (bif->variant) {
    case GLOBAL:
    case PROG_HEDR:
    case PROC_HEDR:
    case FUNC_HEDR:
    case BASIC_BLOCK:
    case ARITHIF_NODE:
    case LOGIF_NODE:
    case LOOP_NODE:
    case FOR_NODE:
    case WHILE_NODE:
    case CDOALL_NODE:
    case SDOALL_NODE:
	if (!skip_rest)
	    proc_ref_in_llnd(fi, bif, bif->entry.Template.ll_ptr1);
	if (!skip_rest)
	    proc_ref_in_llnd(fi, bif, bif->entry.Template.ll_ptr2);
	if (!skip_rest)
	    proc_ref_in_llnd(fi, bif, bif->entry.Template.ll_ptr3);
	for (bl = bif->entry.Template.bl_ptr1; bl; bl = bl->next) {
	    skip_rest = 0;
	    find_proc_call(fi, bl->ref);
	}
	break;
    case IF_NODE:
    case ELSEIF_NODE:
	proc_ref_in_llnd(fi, bif, bif->entry.Template.ll_ptr1);
	for (bl = bif->entry.Template.bl_ptr1; bl; bl = bl->next) {
	    skip_rest = 0;
	    find_proc_call(fi, bl->ref);
	}
	for (bl = bif->entry.Template.bl_ptr2; bl; bl = bl->next) {
	    skip_rest = 0;
	    find_proc_call(fi, bl->ref);
	}
	break;
    case PROC_STAT:		/* this is a procedure call */
    case FUNC_CALL:		/* this is a function call */
	t = tmp = (UnparseBfnd[language])(bif);
	bp = malloc(strlen(t) + 1);
#ifdef __SPF
    addToCollection(__LINE__, __FILE__,bp, 0);
#endif
	(void) strcpy(bp, t);
#ifdef __SPF
    removeFromCollection(tmp);
#endif
	free(tmp);
	inf = make_obj_info(fi->filename, bif->g_line, bif->l_line, bp);
	insert_info_nd(make_blob1(IsObj, inf, NULL));
	break;
    default:
	if (!skip_rest)
	    proc_ref_in_llnd(fi, bif, bif->entry.Template.ll_ptr1);
	if (!skip_rest)
	    proc_ref_in_llnd(fi, bif, bif->entry.Template.ll_ptr2);
	if (!skip_rest)
	    proc_ref_in_llnd(fi, bif, bif->entry.Template.ll_ptr3);
	skip_rest = 0;
	break;
    }
}


/****************************************************************
 *								*
 * proc_ref_llnd -- recursively traverses the given low level	*
 *		    node to find all procedures or functions	*
 *		    references in it				*
 *								*
 *  Input:							*
 *	   fi  - the file obj where this bif node belongs to	*
 *	   bif - the bif node where the llnd belongs		*
 *	   ll  - the low level node to be checked		*
 *								*
 *  Output:							*
 *	   a blob1 list that contains all the call sites under	*
 *	   the node "ll"					*
 *								*
 ****************************************************************/
static PTR_BLOB1
proc_ref_llnd(fi, bif, ll)
	PTR_FILE fi;
	PTR_BFND bif;
	PTR_LLND ll;
{
    PTR_BLOB1 bl = NULL;

    if (ll) {
	if (ll->variant == FUNC_CALL || ll->variant == PROC_CALL || ll->variant == FUNCTION_REF) {
	    char    *bp, *t;
	    PTR_INFO inf;

	    t = ll->entry.Template.symbol->ident;
	    bp = malloc(strlen(t) + 1);
#ifdef __SPF
        addToCollection(__LINE__, __FILE__,bp, 0);
#endif
	    (void) strcpy(bp, t);
	    inf = make_obj_info(fi->filename, bif->g_line, bif->l_line, bp);
	    bl = make_blob1(IsObj, inf, NULL);
	}

	/* NOTE: the following code is "tag" dependent */
	if (ll->variant >= VAR_LIST && ll->variant < CONST_NAME) {
	    PTR_BLOB1 n;

	    n = proc_ref_llnd(fi, bif, ll->entry.Template.ll_ptr1);
        if (n)		/* there are proc references in llnd1 */
        {
            if (bl)
                bl->next = n;
            else
                bl = n;
        }
	    n = proc_ref_llnd(fi, bif, ll->entry.Template.ll_ptr2);
        if (n)		/* there are proc references in llnd2 */
        {
            if (bl) 
            {
                register PTR_BLOB1 p, q;

                for (p = bl; p; p = p->next) /* skip to the end of list */
                    q = p;
                q->next = n;
            }
            else
                bl = n;
        }
	}
    }
    return bl;
}


/****************************************************************
 *								*
 *  ext_proc_call -- recursively traverse the given bif node to *
 *		     find all procedure or functions calls	*
 *		     inside a block (basic, loop, if-then-else) *
 *								*
 *  Inputs:							*
 *	   fi  - the file obj where this bif node belongs to	*
 *	   bl  - the blob chain to be checked			*
 *								*
 *  Output:							*
 *	   a blob1 list that contains all the call sites inside *
 *	   loops in the node "bif", i.e. itself and all its	*
 *	   subtree						*
 *								*
 ****************************************************************/
static PTR_BLOB1
ext_proc_call(fi, bl)
	PTR_FILE fi;
	PTR_BLOB bl;
{
    char	   *t;
    PTR_INFO  inf;
    PTR_BLOB  b;
    PTR_BFND  bf;
    PTR_BLOB1 obj, tail, new, n1, n2;

    obj = tail = NULL;
    for (b = bl; b; b = b->next) {
	bf = b->ref;
	switch(bf->variant) {
	case PROC_STAT:
	case FUNC_CALL:
	    t = malloc(strlen(bf->entry.Template.symbol->ident) + 1);
#ifdef __SPF
        addToCollection(__LINE__, __FILE__,t, 0);
#endif
	    (void) strcpy(t, bf->entry.Template.symbol->ident);
	    inf = make_obj_info(fi->filename, bf->g_line, bf->l_line, t);
	    new = make_blob1(IsObj, inf, NULL);
	    if (obj == NULL)
		obj = tail = new;
	    else {
		tail->next = new;
		tail = new;
	    }
	    break;
	case LOOP_NODE:
	case FOR_NODE:
	case WHILE_NODE:
	case PARFOR_NODE:
	case PAR_NODE:
	    n1 = proc_ref_llnd(fi, bf, bf->entry.Template.ll_ptr1);
	    if ((n2 = proc_ref_llnd(fi, bf, bf->entry.Template.ll_ptr2)))
		n1 = append_blob1_nd(n1, n2);
	    if ((n2 = proc_ref_llnd(fi, bf, bf->entry.Template.ll_ptr3)))
		n1 = append_blob1_nd(n1,n2);
	    if ((n2 = ext_proc_call(fi, bf->entry.Template.bl_ptr1)))
		n1 = append_blob1_nd(n1, n2);

	    if (n1) {
		PTR_INFO inf1;

		inf1 = make_obj_info(fi->filename, bf->g_line, bf->l_line, "loop");
		n2 = make_blob1(IsObj, inf1, n1);
		new = make_blob1(IsLnk, (PTR_INFO)n2, NULL);
		if (obj == NULL)
		    obj = tail = new;
		else {
		    tail->next = new;
		    tail = new;
		}
	    }
	    break;
	case CDOALL_NODE:
	case SDOALL_NODE:
	    n1 = proc_ref_llnd(fi, bf, bf->entry.Template.ll_ptr1);
	    if ((n2 = proc_ref_llnd(fi, bf, bf->entry.Template.ll_ptr2)))
		n1 = append_blob1_nd(n1, n2);
	    if ((n2 = proc_ref_llnd(fi, bf, bf->entry.Template.ll_ptr3)))
		n1 = append_blob1_nd(n1,n2);
	    if ((n2 = ext_proc_call(fi, bf->entry.Template.bl_ptr2)))
		n1 = append_blob1_nd(n1, n2);
	    if (n1) {
		PTR_INFO inf1;

		inf1 = make_obj_info(fi->filename, bf->g_line, bf->l_line, "loop");
		n2 = make_blob1(IsObj, inf1, n1);
		new = make_blob1(IsLnk, (PTR_INFO)n2, NULL);
		if (obj == NULL)
		    obj = tail = new;
		else {
		    tail->next = new;
		    tail = new;
		}
	    }
	    break;
	case IF_NODE:
	case ELSEIF_NODE:
	    n1 = proc_ref_llnd(fi, bf, bf->entry.Template.ll_ptr1);
	    if ((n2 = ext_proc_call(fi, bf->entry.Template.bl_ptr1)))
		n1 = append_blob1_nd(n1, n2);
	    n2 = ext_proc_call(fi, bf->entry.Template.bl_ptr2);
	    if (n1)	{	/* if the true branch has proc call */
		n1 =append_blob1_nd(n1, n2);
	    } else {		/* if no proc call in true branch */
		if (n2) 	/* but some in false branch */
		    n1 = n2;
	    }
	    if (n1) {
		PTR_INFO inf1;

		inf1 = make_obj_info(fi->filename, bf->g_line, bf->l_line, "if");
		n2 = make_blob1(IsObj, inf1, n1);
		new = make_blob1(IsLnk, (PTR_INFO)n2, NULL);
		if (obj == NULL)
		    obj = tail = new;
		else {
		    tail->next = new;
		    tail = new;
		}
	    }
	    break;
	default:
	    new = proc_ref_llnd(fi, bf, bf->entry.Template.ll_ptr1);
	    if ((n2 = proc_ref_llnd(fi, bf, bf->entry.Template.ll_ptr2)))
		new = append_blob1_nd(new, n2);
	    if ((n2 = proc_ref_llnd(fi, bf, bf->entry.Template.ll_ptr3)))
		new = append_blob1_nd(new, n2);
        if (new)
        {
            if (obj == NULL)
                obj = tail = new;
            else
            {
                tail->next = new;
                tail = new;
            }
        }
	    break;
	}
    }
    return (obj);
}

/****************************************************************
 *								*
 *  open_file -- open the dep file "filename"			*
 *								*
 *  Input:							*
 *	   filename -- the name of the dep file to be read in	*
 *								*
 *  Output:							*
 *	   NON-NULL : a pointer to file_obj so as to be able	*
 *		      to access the information.		*
 *	   NULL : open failure					*
 *								*
 ****************************************************************/
static PTR_FILE
open_file(filename)
	char	*filename;
{
    PTR_FILE f;
    FILE	  *fid;
    char	  *temp;
    int    l;

    l = strlen(filename);
    temp = malloc(l + 5);
#ifdef __SPF
    addToCollection(__LINE__, __FILE__,temp, 0);
#endif
    (void)strcpy(temp, filename);
    if ((fid = fopen(temp, "rb")) == NULL) {
        register char *t = temp + l;

        *t++ = '.';
        *t++ = 'd';
        *t++ = 'e';
        *t++ = 'p';
        *t = '\0';
        if ((fid = fopen(temp, "rb")) == NULL) {
            sprintf(db_err_msg, "OpenProj -- Cannot open file \"%s\"", filename);
            return(NULL);
        }
    }
    f = (PTR_FILE)calloc(1, sizeof(struct file_obj));
#ifdef __SPF
    addToCollection(__LINE__, __FILE__,f, 0);
#endif
    if (f == NULL) {
        (void)strcpy(db_err_msg, "open_file -- No more space");
        return(NULL);
    }

    f->fid = fid;
    if (read_nodes(f) < 0)
        return NULL;
    fclose(fid);
    f->hash_tbl = (PTR_HASH *)calloc(hashMax, sizeof(PTR_HASH));
    if (f->hash_tbl == NULL) 
    {
        (void)strcpy(db_err_msg, "open_file -- No more space");
        return(NULL);
    }
#ifdef __SPF
    addToCollection(__LINE__, __FILE__,f->hash_tbl, 0);
#endif
    build_hash(f->head_symb, f->hash_tbl);
    /* the following line is for special testing routine
    if (language == CSrc)
    test_mod_ref(f->global_bfnd);
    */
    gen_udchain(f);
    if (debug)
        dump_udchain(f);
    return(f);
}


static void
dealloc(f)
    PTR_FILE f;
{
    PTR_BLOB b, b1, b2;

    /* Delete all function entries from project's hash table */
    for (b = f->global_bfnd->entry.Template.bl_ptr1; b; b = b->next)
        if (language == ForSrc || (language == CSrc && b->ref->variant == FUNC_HEDR))
            for (b1 = b2 = *(cur_proj->hash_tbl + hash(b->ref->entry.Template.symbol->ident)); b1; b1 = b1->next)
                if (b1->ref == b->ref) {
                    b2 = b1->next;
                    break;
                }
                else
                    b2 = b1;

    /* clean up a little bit.  This is by no means a thorough one */
    if (f->num_blobs)
    {
#ifdef __SPF
        removeFromCollection(f->head_blob);
#endif
        free(f->head_blob);
    }

    if (f->num_bfnds)
    {
#ifdef __SPF
        removeFromCollection(f->head_bfnd);
#endif
        free(f->head_bfnd);
    }

    if (f->num_llnds)
    {
#ifdef __SPF
        removeFromCollection(f->head_llnd);
#endif
        free(f->head_llnd);
    }

    if (f->num_symbs)
    {
#ifdef __SPF
        removeFromCollection(f->head_symb);
#endif
        free(f->head_symb);
    }

    if (f->num_types)
    {
#ifdef __SPF
        removeFromCollection(f->head_type);
#endif
        free(f->head_type);
    }

    if (f->num_dep)
    {
#ifdef __SPF
        removeFromCollection(f->head_dep);
#endif
        free(f->head_dep);
    }

    if (f->num_label)
    {
#ifdef __SPF
        removeFromCollection(f->head_lab);
#endif
        free(f->head_lab);
    }

    if (f->num_cmnt)
    {
#ifdef __SPF
        removeFromCollection(f->head_cmnt);
#endif
        free(f->head_cmnt);
    }

    if (f->num_files)
    {
#ifdef __SPF
        removeFromCollection(f->head_file);
#endif
        free(f->head_file);
    }

#ifdef __SPF
    removeFromCollection(f->hash_tbl);
    removeFromCollection(f);
#endif
    free(f->hash_tbl);
    free(f);
}


/* this creates a new empty file with the given dep file name
   and the given Language type.  It tries to open the file and
   returns 0 if it fails.  If it finds a similar file in the
   project it deletes it.  It enters the file in the project.
   returns 1 if it worked.
   note this file has a global node, the standard types are defined,
   and the default symbol is defined.
*/

int
new_empty_file(Language, filename) 
        int Language; /* 1 = CSrc or C++  and 0 = ForSrc */
	char	*filename;
{
    PTR_FILE f;
    /* FILE	  *fid; */
    char	  *temp;
    int    l;
    /* PTR_SYMB star_symb; */
    PTR_BLOB b, b1;
    /* PTR_BFND global_bfnd; */
    PTR_FNAME fname;

    l = strlen(filename);
    temp = malloc(l+5);
#ifdef __SPF
    addToCollection(__LINE__, __FILE__,temp, 0);
#endif
    (void) strcpy(temp, filename);
    /*
    if ((fid=fopen(temp, "w")) == NULL) {
	register char *t = temp+l;

	*t++ = '.';
	*t++ = 'd';
	*t++ = 'e';
	*t++ = 'p';
	*t   = '\0';
	if ((fid=fopen(temp, "w")) == NULL) {
	    sprintf(db_err_msg, "OpenProj -- Cannot create file \"%s\"", filename);
	    return(NULL);
	}
    }
    */
    f = (PTR_FILE) calloc(1, sizeof(struct file_obj));
    if (f == NULL) {
        (void)strcpy(db_err_msg, "open_file -- No more space");
        return(0);
    }
#ifdef __SPF
    addToCollection(__LINE__, __FILE__,f, 0);
#endif
    fname = (PTR_FNAME) calloc(1, sizeof(struct file_name));
    if (f == NULL) {
        (void)strcpy(db_err_msg, "open_empty_file -- no more space");
        return 0;
    };
#ifdef __SPF
    addToCollection(__LINE__, __FILE__,fname, 0);
#endif
    f->num_files = 1;
    f->head_file = fname;
    fname->name = temp;
    fname->id = 1;

    f->fid = NULL;
    f->lang = Language;
/*    fclose(fid);  */
    f->hash_tbl = (PTR_HASH *) calloc(hashMax, sizeof(PTR_HASH));
    if (f->hash_tbl == NULL) {
        (void)strcpy(db_err_msg, "open_file -- No more space");
        return(0);
    }
#ifdef __SPF
    addToCollection(__LINE__, __FILE__,f->hash_tbl, 0);
#endif
    build_hash(f->head_symb, f->hash_tbl);
    /* global_int    = (PTR_TYPE)*/ make_type(f, T_INT);
    /* global_float  = (PTR_TYPE)*/ make_type(f, T_FLOAT);
    /* global_double = (PTR_TYPE)*/ make_type(f, T_DOUBLE);
    /* global_char   = (PTR_TYPE)*/ make_type(f, T_CHAR);
    /* global_string = (PTR_TYPE)*/ make_type(f, T_STRING);
    /* global_bool   = (PTR_TYPE)*/ make_type(f, T_BOOL);
    /* global_complex= (PTR_TYPE)*/ make_type(f, T_COMPLEX);
    /* global_default= (PTR_TYPE)*/ make_type(f, DEFAULT);
    /* global_void   = (PTR_TYPE)*/ make_type(f, T_VOID);   
    /* global_void   = (PTR_TYPE)*/ make_type(f, T_UNKNOWN);   
    /* DEFAULT is used for type */
    make_symb(f, DEFAULT, "*");
    f->global_bfnd =  make_bfnd(f,GLOBAL, SMNULL, LLNULL, LLNULL, LLNULL);
    f->global_bfnd->filename=fname;
    f->filename = temp;
    /* add it to the project */
    for (b = b1 = cur_proj->file_chain; b; b1 = b, b = b->next)
	if (! strcmp(temp, ((PTR_FILE)b->ref)->filename))
	    break;
    if (b)			/* if non-NULL, then already in the project */
	dealloc((PTR_FILE)b->ref);
    if (b == NULL) {		/* it's not in the project before */
	if ((b = alloc_blob()) == NULL)
	    return 0;
	b1->next = b;		/* add it to the end of the list */
    }
    b->ref = (PTR_BFND) f;
    return 1; 
}


/****************************************************************
 *								*
 *  AddToProj -- Add another file to the current project	*
 *								*
 *  Input:							*
 *	   file -- file name to be added to the project 	*
 *								*
 *  Output:							*
 *	   1 if everything ok					*
 *	   0 if something wrong 				*
 *								*
 ****************************************************************/
int
AddToProj(file)
	char *file;
{
    char tmp[50], *p = tmp, *q = file;
    PTR_BLOB b, b1, new;
    PTR_FILE f;
    int index;

    while ((*p++ = *q++) != '.'); /* simple-minded copy*/
    *p++ = 'd';
    *p++ = 'e';
    *p++ = 'p';
    *p++ = '\0';
    for (b = b1 = cur_proj->file_chain; b; b1 = b, b = b->next)
        if (!strcmp(file, ((PTR_FILE)b->ref)->filename))
            break;
    if (b)			/* if non-NULL, then already in the project */
        dealloc((PTR_FILE)b->ref);
    if ((f = open_file(tmp)) == NULL)
        return 0;
    if (b == NULL) {		/* it's not in the project before */
        if ((b = alloc_blob()) == NULL)
            return 0;
        b1->next = b;		/* add it to the end of the list */
    }
    b->ref = (PTR_BFND)f;

    /* Insert all procedures in this file into current project's hash table */
    for (b = f->global_bfnd->entry.Template.bl_ptr1; b; b = b->next) {
        if (language == ForSrc ||
            (language == CSrc && b->ref->variant == FUNC_HEDR)) {
            index = hash(b->ref->entry.Template.symbol->ident);
            if ((new = (PTR_BLOB)calloc(1, sizeof(struct blob))) != 0)
            {
                new->ref = b->ref; /* point to the procedure's bif node */
                new->next = *(cur_proj->hash_tbl + index);
                *(cur_proj->hash_tbl + index) = new;

#ifdef __SPF
                addToCollection(__LINE__, __FILE__,new, 0);
#endif
            }
            else 
            {
                (void)strcpy(db_err_msg, "open_proj_file -- No more space");
                return 0;
            }
        }
    }
    return 1;
}


/****************************************************************
 *								*
 *  DelFromProj -- Delte the file from the current project	*
 *								*
 *  Input:							*
 *	   file -- file name to be deleted			*
 *								*
 *  Output:							*
 *	   1 if everything ok					*
 *	   0 if something wrong 				*
 *								*
 ****************************************************************/
int
DelFromProj(file)
	char *file;
{
    PTR_BLOB b, b1;

    for (b = b1 = cur_proj->file_chain; b; b1 = b, b = b->next)
	if (! strcmp(file, ((PTR_FILE)b->ref)->filename))
	    break;
    if (b) {			/* if non-NULL, then it's in the project */
	dealloc((PTR_FILE)b->ref);
	b1->next = b->next;
	return 1;
    } else
	return 0;
}


/****************************************************************
 *								*
 *  open_proj_files -- open all the files in a given project	*
 *								*
 *  Input:							*
 *	   proj -- pointer to the project object		*
 *	   no	-- number of files in the project		*
 *	   file_list -- list of file names in the project	*
 *								*
 *  Output:							*
 *	   1 if everything ok					*
 *	   0 if something wrong 				*
 *								*
 ****************************************************************/
static int
open_proj_file(proj, no, file_list)
	PTR_PROJ  proj;
	int	  no;
	char	**file_list;
{
    int	       i, index;
    PTR_BLOB   b, new;
    PTR_FILE   f;
    char     **fp;

    fp = file_list;		/* points to start of the list */
    for (i = 1; i <= no; i++) {
        if ((f = open_file(*fp++)) != NULL) 
        {
            b = alloc_blob();
            if (b == NULL) 
            {
                (void)strcpy(db_err_msg, "open_proj_file: alloc_blob failed");
                return 0;
            }
            b->ref = (PTR_BFND)f; /* NOT a bif node, but ... */
            b->next = proj->file_chain;
            proj->file_chain = b;

            /* Insert all procedures into the project's hash table */
            for (b = f->global_bfnd->entry.Template.bl_ptr1; b; b = b->next)
            {
                if (language == ForSrc || (language == CSrc && b->ref->variant == FUNC_HEDR))
                {
                    index = hash(b->ref->entry.Template.symbol->ident);
                    if ((new = (PTR_BLOB)calloc(1, sizeof(struct blob))) != 0) 
                    {
                        new->ref = b->ref; /* point to the procedure's bif node */
                        new->next = *(proj->hash_tbl + index);
                        *(proj->hash_tbl + index) = new;
#ifdef __SPF
                        addToCollection(__LINE__, __FILE__,new, 0);
#endif
                    }
                    else 
                    {
                        (void)strcpy(db_err_msg, "open_proj_file -- No more space");
                        return 0;
                    }
                }
            }
        }
        else 
        {
            (void)sprintf(db_err_msg, "OpenProj -- No such file \"%s\"\n", *(--fp));
            return 0;
        }
    }
    return 1;
}



/****************************************************************
 *								*
 *  OpenProj -- open the project with list of files as		*
 *		specified in the "file_list"			*
 *								*
 *  Inputs:							*
 *	   pname     -- the project name			*
 *	   no	     -- number of files in the project		*
 *	   file_list -- list of .dep files to be read in	*
 *								*
 *  Output:							*
 *	   NON-NULL : a pointer to the project object so as to	*
 *		      be able to access the information.	*
 *	   NULL : open failure					*
 *								*
 ****************************************************************/
PTR_PROJ
OpenProj(pname, no, file_list)
	char	 *pname;
	int	  no;
	char	**file_list;
{
    PTR_BLOB b;
    PTR_PROJ p;

    /* First allocate a project structure to it */
    if ((p = (PTR_PROJ)calloc(1, sizeof(struct proj_obj))) == NULL)
        return NULL;

    p->proj_name = malloc(strlen(pname) + 1);
#ifdef __SPF
    addToCollection(__LINE__, __FILE__, p->proj_name, 0);
    addToCollection(__LINE__, __FILE__, p, 0);
#endif
    (void)strcpy(p->proj_name, pname);

    /* Then insert it to the project chain */
    b = alloc_blob();
    b->ref = (PTR_BFND)p;	/* NOT a bif node, but ... */
    b->next = head_proj;	/* insert this project to */
    head_proj = b;		/*	...	the list */

    cur_proj = p;		/* Make it the current active project */
    p->hash_tbl = (PTR_BLOB *)calloc(hashMax, sizeof(PTR_BLOB));
    if (p->hash_tbl == NULL)
        return NULL;
#ifdef __SPF
    addToCollection(__LINE__, __FILE__, p->hash_tbl, 0);
#endif

    if (open_proj_file(p, no, file_list))
        return (p);
    else
        return NULL;
}


/****************************************************************
 *								*
 *  SelectProj -- Select the project "proj_name" as active	*
 *		  project					*
 *								*
 *  Inputs:							*
 *	   proj_name - the project's filename			*
 *								*
 *  Output:							*
 *	   A PTR_PROJ that points to the selected project	*
 *	   object.  Returns a NULL if the project didn't exit	*
 *								*
 ****************************************************************/
PTR_PROJ
SelectProj(proj_name)
	char	*proj_name;
{
    PTR_BLOB  b;
    PTR_PROJ  p;

    /* First search the project chain to find the one specified */
    for (b = head_proj; b; b = b->next) {
	p = (PTR_PROJ) b->ref;
	if(!strcmp(proj_name, p->proj_name))
	    break;
    }

    if (b == NULL) {
	(void) sprintf(db_err_msg, "SelectProj -- no such project \"%s\"", proj_name);
	return NULL;
    }

    return (cur_proj = p);
}


/****************************************************************
 *								*
 *  GetProjInfo -- get info about a given project from the data *
 *		   base 					*
 *								*
 *  Inputs:							*
 *	   proj_name - the project's filename			*
 *	   info      - type of info wanted.  Could be one of	*
 *		       the followings:				*
 *			 ProjFiles,  ProjNames, ProjGlobals,	*
 *			 ProjSrc or UnsolvRef			*
 *  Output:							*
 *	   A blob1 list that contains the info inquired 	*
 *								*
 *  Side Effects:						*
 *	   It changes the global variables "obj" and "tail"	*
 *	   (by calling insert_info_nd)				*
 *								*
 ****************************************************************/
PTR_BLOB1
GetProjInfo(proj_name, info)
	char	*proj_name;
	int	 info;
{
    PTR_BLOB  b, bl;
    PTR_INFO  inf;
    PTR_FILE  f;
    PTR_PROJ  p;

    /* First search the project chain to find the one specified */
    for (b = head_proj; b; b = b->next) {
	p = (PTR_PROJ) b->ref;
	if(!strcmp(proj_name, p->proj_name))
	    break;
    }

    if (b == NULL) {
	(void) sprintf(db_err_msg, "GetProjInfo -- no such project \"%s\"", proj_name);
	return NULL;
    }

    obj = tail = NULL;

    /* Then search the file chain inside the project */
    switch(info) {
    case ProjFiles:
	for (b = p->file_chain; b; b = b->next) {
	    f = (PTR_FILE) b->ref;
	    inf = make_obj_info(f->filename, 0, 0, NULL);
	    insert_info_nd(make_blob1(IsObj, inf, NULL));
	}
	break;
    case ProjSrc:
	{
	    char *c_tab[100],	/* for .c files */
	    *h_tab[100],	/* for .h files */
	    *u_tab[100];	/* for .f and other unknow type files */
	    char **c1, **c2, **h1, **h2, **u1, **u2, ch;
	    PTR_FNAME fp;

	    c1 = c2 = c_tab;
	    u1 = u2 = u_tab;
	    h1 = h_tab;

	    /* Scan through the file chain to gather all filenames */
	    for (b = p->file_chain; b; b = b->next)
		for (fp = ((PTR_FILE)b->ref)->head_file; fp; fp = fp->next) {
		    if ((ch =last_char(fp->name)) == 'c')
			*c1++ = fp->name;
		    else if (ch == 'h') {
			for (h2 = h_tab; h2 < h1; h2++)
			    if (!strcmp(fp->name, *h2))
				break;
			if (h2 == h1)
			    *h1++ = fp->name;
		    }
		    else
			*u1++ = fp->name;
		}

	    /* Now link them all together */
	    while (c2 < c1)
		insert_info_nd(make_blob1(IsObj, make_obj_info(*c2++, 0, 0, NULL), NULL));

	    h2 = h_tab;
	    while (h2 < h1)
		insert_info_nd(make_blob1(IsObj, make_obj_info(*h2++, 0, 0, NULL), NULL));

	    while (u2 < u1)
		insert_info_nd(make_blob1(IsObj, make_obj_info(*u2++, 0, 0, NULL), NULL));
	}
	break;
    case ProjNames:
	for (b = p->file_chain; b; b = b->next) {
	    f = (PTR_FILE) b->ref;
	    for(bl = f->global_bfnd->entry.Template.bl_ptr1; bl; bl = bl->next) {
		PTR_BFND bf;
		char * ch;
		if (language == ForSrc ||
		    (language == CSrc && bl->ref->variant==FUNC_HEDR)) {
		    bf = bl->ref;
		    ch = (UnparseBfnd[language])(bf);
		    inf = make_obj_info(bf->filename->name, bf->g_line, bf->l_line, ch);
		    insert_info_nd(make_blob1(IsObj, inf, NULL));
		}
	    }
	}
	break;
    case ProjGlobals:		/* WARNING -- C languag specific */
	if (language == CSrc)
	    for (b = p->file_chain; b; b = b->next) {
		f = (PTR_FILE) b->ref;
		for(bl = f->global_bfnd->entry.Template.bl_ptr1; bl; bl = bl->next) {
		    PTR_BFND bf;

		    if (bl->ref->variant != FUNC_HEDR) {
			bf = bl->ref;
			inf = make_obj_info(bf->filename->name, bf->g_line, bf->l_line,
					    (UnparseBfnd[language])(bf));
			insert_info_nd(make_blob1(IsObj, inf, NULL));
		    }
		}
	    }
	break;
    case UnsolvRef:
	obj = NULL;
	for (b = p->file_chain; b; b = b->next) {
	    f = (PTR_FILE) b->ref;
	}
	break;
    }
    return obj;
}


/****************************************************************
 *								*
 *  GetProcInfo -- get info about a given procedure from the	*
 *		   data base					*
 *								*
 *  Input:							*
 *	  proc_name - the procedure's filename			*
 *	  info	    - type of info wanted.  Could be one of	*
 *		      the followings:				*
 *		      ProcDef, Mod, Use, Alias, CallSite,	*
 *		      ExternProc, or CallSiteE			*
 *  Output:							*
 *	   A blob1 list that contains the info inquired 	*
 *								*
 ****************************************************************/
PTR_BLOB1
GetProcInfo(proc_name, info)
	char	*proc_name;
	int	 info;
{
    int    i;
    char	   buf[1000], *bp, *tmp, *t;
    PTR_PROJ proj;
    PTR_FILE fi;
    PTR_INFO inf;
    PTR_BLOB bl;
    PTR_BFND bf, bf1;
    PTR_SYMB s;
    PTR_LLND tp;

    /* First search for the hash table to find the procedure bif node */
    proj = cur_proj;
    i = hash(proc_name);
    for (bl = *(proj->hash_tbl + i); bl; bl = bl->next)
        if (!strcmp(bl->ref->entry.Template.symbol->ident, proc_name))
            break;		/* find it */

    if (bl == NULL)		/* no such procedures or functions */
        return NULL;

    bf = bl->ref;		/* get the procedure header */
    bf1 = bf->control_parent;	/* should get the global_bfnd */
    fi = (PTR_FILE)bf1->control_parent; /* the file_info node */
    obj = tail = NULL;
    switch (info) {
    case ProcDef:
        bp = buf;		/* reset the pointer */
        bf1 = bf->control_parent; /* should get the global_bfnd */
        fi = (PTR_FILE)bf1->control_parent; /* the file_info node */
        t = tmp = (UnparseBfnd[language])(bf); /* unparse the proc node */
        while ((*bp = *t++) != 0)	/* save to the output area */
            bp++;
#ifdef __SPF
        removeFromCollection(tmp);
#endif
        free(tmp);
        s = bf->entry.Template.symbol; /* symbol node of the proc */

        /* Now trace down its parameter declaration */
        for (s = s->entry.proc_decl.in_list; s; s = s->entry.var_decl.next_in) {
            tmp = t = (UnparseSymb[language])(s);
            while ((*bp = *t++) != 0)
                bp++;
#ifdef __SPF
            removeFromCollection(tmp);
#endif
            free(tmp);
        }
        *bp = '\0';		/* Mark end of string */
        bp = malloc(strlen(buf) + 1);
#ifdef __SPF
        addToCollection(__LINE__, __FILE__,bp, 0);
#endif
        (void)strcpy(bp, buf);
        inf = make_obj_info(fi->filename, bf->g_line, bf->l_line, bp);
        return(make_blob1(IsObj, inf, NULL));
    case Mod:
        tp = bf->entry.Template.ll_ptr2;
        if (tp->entry.Template.ll_ptr2 != NULL)
            tp = tp->entry.Template.ll_ptr2;
        inf = make_obj_info(fi->filename, bf->g_line, bf->l_line,
            (UnparseLlnd[language])(tp));
        return(make_blob1(IsObj, inf, NULL));
    case Use:
        tp = bf->entry.Template.ll_ptr3;
        if (tp->entry.Template.ll_ptr2 != NULL)
            tp = tp->entry.Template.ll_ptr2;
        inf = make_obj_info(fi->filename, bf->g_line, bf->l_line,
            (UnparseLlnd[language])(tp));
        return(make_blob1(IsObj, inf, NULL));
    case Alias:
        break;
    case CallSite:
        bf = bl->ref;
        for (bl = bf->entry.Template.bl_ptr1; bl; bl = bl->next)
            find_proc_call(fi, bl->ref);
        skip_rest = 0;
        return obj;
    case ExternProc:
        break;
    case CallSiteE:
        bp = malloc(strlen(bf->entry.Template.symbol->ident) + 1);
#ifdef __SPF
        addToCollection(__LINE__, __FILE__,bp, 0);
#endif
        (void)strcpy(bp, bf->entry.Template.symbol->ident);
        inf = make_obj_info(fi->filename, bf->g_line, bf->l_line, bp);
        return (make_blob1(IsObj, inf, ext_proc_call(fi, bf->entry.Template.bl_ptr1)));
    default:
        (void)strcpy(db_err_msg, "GetProcInfo -- No such info available");
        break;
    }
    return NULL;
}


/****************************************************************
 *								*
 *  GetVarInfo -- get info about a given variable from the data *
 *		  base						*
 *								*
 *  Inputs:							*
 *	   var_name - the variable's name			*
 *	   info     - type of info wanted.  Could be one of the *
 *		      following: Use, Mod, UseMod and Alias	*
 *	   proc_name - specifies the procedure you want to	*
 *		      check.  If it's NULL, then all instances	*
 *		      of the "var_name" will be returned	*
 *  Output:							*
 *	   A blob1 list that contains the info inquired 	*
 *								*
 ****************************************************************/
PTR_BLOB1
GetVarInfo(var_name, info, proc_name)
	char	*var_name;
	int	 info;
	char	*proc_name;
{
    int   i;
    PTR_HASH  p;
    PTR_BFND  bif;
    PTR_BLOB  bl;
	
    /* First, get the symbol table entry */
    i = hash(var_name);
    for (p = hash_table[i]; p ; p = p->next_entry)
	if(!strcmp(var_name, p->id_attr->ident))
	    break;
    if (p == NULL)		/* no such variable */
	return(NULL);

    /* Then for its ud_chain */
    for (bl = p->id_attr->ud_chain; bl; bl = bl->next) {
	bif = bl->ref;
	switch(bif->variant) {
	case PROG_HEDR:
	case PROC_HEDR:
	case FUNC_HEDR:
	    break;
	case CDOALL_NODE:
	case FOR_NODE:
	    check_llnd(bif, bif->entry.Template.ll_ptr1, Use, var_name); /* check range */
	    check_llnd(bif, bif->entry.Template.ll_ptr2, Use, var_name); /* check incr */
	    check_llnd(bif, bif->entry.Template.ll_ptr3, Use, var_name); /* where cond */
	    break;
	case WHILE_NODE:
	case WHERE_NODE:
	    check_llnd(bif, bif->entry.Template.ll_ptr1, Use, var_name); /* check cond */
	    break;
	case IF_NODE:
	case ELSEIF_NODE:
	    check_llnd(bif, bif->entry.Template.ll_ptr1, Use, var_name); /* check cond */
	    break;
	case LOGIF_NODE:
	    check_llnd(bif, bif->entry.Template.ll_ptr1, Use, var_name); /* check cond */
	    break;
	case ARITHIF_NODE:
	    check_llnd(bif, bif->entry.Template.ll_ptr1, Use, var_name); /* check cond */
	    break;
	case ASSIGN_STAT:
	case IDENTIFY:
	    check_llnd(bif, bif->entry.Template.ll_ptr1, Use, var_name); /* check l_val */
	    check_llnd(bif, bif->entry.Template.ll_ptr2, Use, var_name); /* check r_val */
	    break;
	case PROC_STAT:
	    check_llnd(bif, bif->entry.Template.ll_ptr1, Use, var_name); /* check l_val */
	    break;
	case ASSGOTO_NODE:
	case COMGOTO_NODE:
	    check_llnd(bif, bif->entry.Template.ll_ptr1, Use, var_name); /* check l_val */
	    break;
	case VAR_DECL:
	case PARAM_DECL:
	case DIM_STAT:
	case EQUI_STAT:
	case DATA_DECL:
	case IMPL_DECL:
	    /* for type decl chain
	       check_llnd(bif, bif->entry.Template.ll_ptr1, Use, var_name);
	       break;
	       */
	case READ_STAT:
	case WRITE_STAT:
	    break;
	case STOP_STAT:
	case OTHERIO_STAT:
	case COMM_STAT:
	case CONT_STAT:
	case FORMAT_STAT:
	case GOTO_NODE:
	case CONTROL_END:
	    break;
	default:
	    break;
	}
    }
 return(NULL);
}


/****************************************************************
 *								*
 *  GetTypeInfo -- get a list of variables of a given type from *
 *		   the data base				*
 *								*
 *  Input:							*
 *	  type_name - the type's name				*
 *	  proc_name - specifies the procedure you want to	*
 *		      check.  If it's NULL, then all instances	*
 *		      of the "var_name" will be returned	*
 *  Output:							*
 *	   A blob1 list that contains the info inquired 	*
 *								*
 ****************************************************************/
PTR_BLOB1
GetTypeInfo(type_name, proc_name)
	char	*type_name;
	char	*proc_name;
{
    return NULL;
}


/****************************************************************
 *								*
 *  GetTypeDef -- Get definition about a given type from	*
 *		  the data base 				*
 *								*
 *  Input:							*
 *	  type_name - the type's name				*
 *	  proc_name - specifies the procedure you want to	*
 *		      check.  If it's NULL, then all instances	*
 *		      of the "var_name" will be returned	*
 *  Output:							*
 *	   A blob1 list that contains the info inquired 	*
 *								*
 ****************************************************************/
PTR_BLOB1
GetTypeDef(type_name, proc_name)
	char	*type_name;
	char	*proc_name;
{
    int i;
    char *c;
    PTR_BLOB bl;
    PTR_BLOB1 bl1 = NULL, bl2;
    PTR_BFND bf;
    PTR_FILE f;
    PTR_HASH p;

    if (proc_name) {		/* if procedure name was specified */
	i = hash(proc_name);
	for (bl = *(cur_proj->hash_tbl + i); bl; bl = bl->next)
	    if (!strcmp(proc_name, bl->ref->entry.Template.symbol->ident))
		break;		/* find it */
	if (bl == NULL) {
	    (void) sprintf(db_err_msg,"GetTypeDef -- no such procedure \"%s\"",proc_name);
	    return NULL;
	}
	bf = bl->ref->control_parent; /* should get the global bif node */
	f = (PTR_FILE)bf->control_parent; /* get the file info node */
	i = hash(type_name);
	for (p = *(f->hash_tbl + i); p; p = p->next_entry)
	    if( 		/* p->id_attr->variant == TYPE_NAME && */
	       !strcmp(type_name, p->id_attr->ident)) {
		c = (*unparse_type)(p->id_attr->type);
		return (make_blob1(IsObj, make_obj_info(proc_name, 0, 0, c), NULL));
	    }
	(void) sprintf(db_err_msg, "GetTypeDef -- No such type \"%s\"",type_name);
	return NULL;
    } else {			/* procedure name not specified */
	for (bl = cur_proj->file_chain; bl; bl = bl->next) {
	    f = (PTR_FILE)bl->ref;
	    i = hash(type_name);
	    for (p = *(f->hash_tbl + i); p; p = p->next_entry)
		if(		/* p->id_attr->variant == TYPE_NAME && */
		   !strcmp(type_name, p->id_attr->ident)) {
		    c = (*unparse_type)(p->id_attr->type);
		    bl2 = make_blob1(IsObj,
				     make_obj_info(p->id_attr->scope->entry.Template.symbol->ident, 0, 0, c),
				     NULL);
		    if (bl1) {
			bl2->next = bl1;
			bl1 = bl2;
		    } else
			bl1 = bl2;
		}
	}
	return bl1;
    }
}

/****************************************************************
 *								*
 *  rec_num_search -- recursively search for the bif node that	*
 *		      corresponds to the num'th line in the	*
 *		      file fname				*
 *								*
 *  Inputs:							*
 *	bf    - the bif node that will be searched		*
 *	num   - line number					*
 *	fname - filename to be checked against			*
 *								*
 *  Output:							*
 *	The bif node pointer if one exists for the given line	*
 *	in the given file					*
 *								*
 ****************************************************************/
PTR_BFND
rec_num_search(bf,num,fname)
	PTR_BFND bf;
	int	 num;
	char 	*fname;
{
    if (!strcmp(bf->filename->name, fname) && bf->g_line == num)
	return(bf);
    else{
	PTR_BLOB b;
	PTR_BFND rv;

	for (b = bf->entry.Template.bl_ptr1; b; b = b->next)
	    if( (rv = rec_num_search(b->ref,num,fname)) != NULL)
		return(rv);

	for (b = bf->entry.Template.bl_ptr2; b; b = b->next)
	    if( (rv = rec_num_search(b->ref,num,fname)) != NULL)
		return(rv);
    }
    return(NULL);
}


/****************************************************************
 *								*
 *   FindBifNode -- find the corresponding BIF node given a	*
 *		    filename and line number			*
 *								*
 *   Input:							*
 *	    filename - name of the file to be looked upon	*
 *	    line     - line number to be checked		*
 *								*
 *   Output:							*
 *	    A bif pointer (PTR_BFND) points to the bif node	*
 *	    corresponds to the given line number		*
 *	    NULL if error occured				*
 *								*
 ****************************************************************/
PTR_BFND
FindBifNode(filename, line)
	char	*filename;
	int	 line;
	
{
    PTR_PROJ p = cur_proj;
    PTR_BFND bf = NULL;
    PTR_BFND rec_num_search();
    PTR_BLOB b;

    for (b=p->file_chain; b; b = b->next) {
	if(!strcmp(((PTR_FILE)b->ref)->filename, filename)) {
	    bf = ((PTR_FILE)b->ref)->head_bfnd;
	    break;
	}
    }

    if (!b) {
	(void) sprintf(db_err_msg, "No such file \"%s\" in this project",filename);
	return NULL;
    }
    return(rec_num_search(bf,line,filename));
}


/****************************************************************
 *								*
 *  bget_prop -- Get property named "pname" from the property	*
 *		 of a given bif node				*
 *								*
 *  Inputs:							*
 *	  bf	- bif pointer from which the property is to be	*
 *		  extracted					*
 *	  pname - property name in string			*
 *								*
 *  Output:							*
 *	  value of the specified property			*
 *	  NULL if not found					*
 *								*
 ****************************************************************/
char *
bget_prop(bf, pname)
	PTR_BFND bf;
	char	*pname;
{
    register PTR_PLNK prop;

    for (prop = bf->prop_list; prop; prop = prop->next)
	if (! strcmp(prop->prop_name, pname))
	    return (prop->prop_val);
    return (NULL);
}


/****************************************************************
 *								*
 *  get_prop -- Get property named "pname" from a given 	*
 *	       statement's property list			*
 *								*
 *  Inputs:							*

 *	  fname   - name of the source file			*
 *	  line_no - line number of the statement		*
 *	  pname   - property name in string			*
 *								*
 *  Output:							*
 *	  value of the specified property			*
 *								*
 ****************************************************************/
char *
get_prop(fname, line_no, pname)
	char	*fname;
	int	 line_no;
	char	*pname;
{
    PTR_BFND bf;

    bf = FindBifNode(fname, line_no);
    return (bf? bget_prop(bf, pname): NULL);
}


/****************************************************************
 *								*
 *  put_prop -- Put property "prop" about a given statement to	*
 *		  the data base 				*
 *								*
 *  Inputs:							*
 *	  fname   - name of the source file			*
 *	  line_no - line number of the statement		*
 *	  pname   - property name in string			*
 *	  value   - property value				*
 *								*
 *  Output:							*
 *	  0 - if no error occured				*
 *	  1 - if error occured					*
 *								*
 ****************************************************************/
int
put_prop(fname, line_no, pname, value)
	char	*fname;
	int	 line_no;
	char	*pname;
	char	*value;
{
    PTR_BFND bf;
    PTR_PLNK pr;

    bf = FindBifNode(fname, line_no);
    if (bf)
    {
        if ((pr = (PTR_PLNK)malloc(sizeof(struct prop_link))) != 0) 
        {
            pr->prop_name = pname;
            pr->prop_val = value;
            pr->next = bf->prop_list;
            bf->prop_list = pr;
#ifdef __SPF
            addToCollection(__LINE__, __FILE__,pr, 0);
#endif
            return 0;
        }
        else
            (void)strcpy(db_err_msg, "put_prop -- No more space");
    }
    return 1;
}


static char *depstrs[] = { "flow","anti","output","huh??","got me?"};
static char *dirstrs[] = { "  ", "= ", "- ", "0-", "+ ", "0+", ". ", "+-"};

static PTR_BFND current_par_loop = NULL;

static int
same_loop(from, to)
	PTR_BFND from, to;
{
    PTR_BFND c;
    c = from;
    while(c != NULL && c->variant != GLOBAL && c != current_par_loop)
	c = c->control_parent;
    if(c != current_par_loop) return(0);
    c = to;
    while(c != NULL && c->variant != GLOBAL && c != current_par_loop)
	c = c->control_parent;
    if(c != current_par_loop) return(0);
    return(1);
}

static PTR_BLOB1
search_deps(nb,q,depth)
    PTR_BLOB1	nb;
    PTR_BLOB	q;
    int 	depth;
{
    PTR_BFND  bchild;
    PTR_DEP   d;
    char	   *s;
    PTR_BLOB1 lb = NULL, btmp;

    if (nb != NULL) lb = nb;
    while (q != NULL) {
        bchild = q->ref;
        q = q->next;
        d = bchild->entry.Template.dep_ptr1;
        while (d != NULL) {
            if ((d->symbol->type->variant == T_ARRAY && d->direct[depth] > 1) ||
                (d->type == 0 && d->direct[depth] > 1))
                if (same_loop(d->from.stmt, d->to.stmt)) {
                    btmp = (PTR_BLOB1)malloc(sizeof(struct blob1));
                    if (nb == NULL) { nb = btmp; lb = btmp; }
                    else { lb->next = btmp; lb = btmp; }
                    s = malloc(256);
#ifdef __SPF
                    addToCollection(__LINE__, __FILE__,s, 0);
#endif
                    sprintf(s, "id:%s type:%s to line %d dir_vect =(%s,%s,%s)\n",
                        d->symbol->ident, depstrs[(int)(d->type)],
                        d->to.stmt->g_line,
                        dirstrs[(int)(d->direct[1])], dirstrs[(int)(d->direct[2])],
                        dirstrs[(int)(d->direct[3])]);
                    btmp->ref = s;
                    btmp->next = NULL;
                }
            d = d->from_fwd;
        }
        if (bchild->entry.Template.bl_ptr1 != NULL) {
            nb = search_deps(nb, bchild->entry.Template.bl_ptr1, depth);
            lb = nb; while (lb != NULL && lb->next != NULL) lb = lb->next;
        }
        if (bchild->entry.Template.bl_ptr2 != NULL) {
            nb = search_deps(nb, bchild->entry.Template.bl_ptr2, depth);
            lb = nb; while (lb != NULL && lb->next != NULL) lb = lb->next;
        }
    }
    return(nb);
}


PTR_BLOB1
GetDepInfo(filename, line)
	char *filename;
	int line;
{
    PTR_BFND b, bpar;
    PTR_DEP d;
    int depth;
    char * s;
    PTR_BLOB1 nb, lb, btmp;
    PTR_BLOB q;

    b = FindBifNode(filename, line);
    if (b == NULL) return(NULL);
    /* if b is a loop, we look for all loop carried deps for */
    /* this loop.  otherwise just list dependence going out */
    if (b->variant == FOR_NODE) {
        depth = 0;
        bpar = b;
        current_par_loop = b;
        while (bpar != NULL && bpar->variant != GLOBAL) {
            if (bpar->variant == FOR_NODE ||
                bpar->variant == CDOALL_NODE ||
                bpar->variant == WHILE_NODE ||
                bpar->variant == FORALL_NODE) depth++;
            bpar = bpar->control_parent;
        }
        q = b->entry.Template.bl_ptr1;
        nb = (PTR_BLOB1)malloc(sizeof(struct blob1));
#ifdef __SPF
        addToCollection(__LINE__, __FILE__,nb, 0);
#endif
        s = malloc(256);
#ifdef __SPF
        addToCollection(__LINE__, __FILE__,s, 0);
#endif
        sprintf(s, "Essential dependences inhibiting parallelization of loop are:\n");
        nb->ref = s;
        nb->next = NULL;
        nb = search_deps(nb, q, depth);
        return(nb);
    }				/* if loop case */
    d = b->entry.Template.dep_ptr1;
    nb = NULL;
    while (d != NULL) {
        btmp = (PTR_BLOB1)malloc(sizeof(struct blob1));
#ifdef __SPF
        addToCollection(__LINE__, __FILE__,btmp, 0);
#endif
        if (nb == NULL) { nb = btmp; lb = btmp; }
        else { lb->next = btmp; lb = btmp; }
        s = malloc(256);
#ifdef __SPF
        addToCollection(__LINE__, __FILE__,s, 0);
#endif
        sprintf(s, "id:%s type:%s to line %d dir_vect =(%s,%s,%s)\n",
            d->symbol->ident, depstrs[(int)(d->type)],
            d->to.stmt->g_line,
            dirstrs[(int)(d->direct[1])], dirstrs[(int)(d->direct[2])],
            dirstrs[(int)(d->direct[3])]);
        btmp->ref = s;
        btmp->next = NULL;
        d = d->from_fwd;
    }
    return(nb);
}


/****************************************************************
 *								*
 *   FindRef -- find the reference of the given symbol in the	*
 *		low level node					*
 *								*
 *   Inputs:							*
 *		ll - the low level node to be searched		*
 *		name - the symbol name to be looked up		*
 *								*
 *   Output:							*
 *		an integer indicating the type of the "name":	*
 *								*
 *			0 -- program				*
 *			1 -- procedure				*
 *			2 -- function				*
 *			3 -- constant (or parmameter in Fortran)*
 *			4 -- scalar variable			*
 *			5 -- array variable			*
 *			6 -- record variable			*
 *			7 -- enumerated type			*
 *			8 -- label variable			*
 *			9 -- name of common block		*
 *								*
 ****************************************************************/
static int
FindRef(ll, name)
	PTR_LLND  ll;
	char	 *name;
{
    int val;

    if (!ll)
	return -1;
	
    switch (ll->variant) {
    case CONST_REF:
	if (!strcmp(name, ll->entry.Template.symbol->ident))
	    return 3;
	break;
    case VAR_REF:
	if (!strcmp(name, ll->entry.Template.symbol->ident))
	    return 4;
	break;
    case ARRAY_REF:
	if (!strcmp(name, ll->entry.Template.symbol->ident))
	    return 5;
	break;
    case RECORD_REF:
	if (!strcmp(name, ll->entry.Template.symbol->ident))
	    return 6;
	break;
    case ENUM_REF:
	if (!strcmp(name, ll->entry.Template.symbol->ident))
	    return 7;
	break;
    case LABEL_REF:
	if (!strcmp(name, ll->entry.Template.symbol->ident))
	    return 8;
	break;
    case COMM_LIST:
	if (ll->entry.Template.symbol && /* could be blank common */
	    !strcmp(name, ll->entry.Template.symbol->ident))
	    return 9;
	break;
    case FUNC_CALL:
	if (!strcmp(name, ll->entry.Template.symbol->ident))
	    return 2;
	break;
    default:
	break;
    }

    if ((val=FindRef(ll->entry.Template.ll_ptr1,name)) != -1)
	return val;

    if ((val=FindRef(ll->entry.Template.ll_ptr2,name)) != -1)
	return val;
    return -1;
}


/****************************************************************
 *								*
 *   SymbType -- find the type of the given symbol		*
 *								*
 *   Input:							*
 *	    filename - name of the file to be looked upon	*
 *	    line     - line number of the symbol reference	*
 *	    name     - varaible name				*
 *								*
 *   Output:							*
 *	    an integer representing the variable type (take a	*
 *	    look at "../h/tag" for possible returned values	*
 *	    return a -1 if error occured			*
 *								*
 ****************************************************************/
int
SymbType(filename, line, name)
	char	*filename;
	int	 line;
	char	*name;
{
    int val;
    PTR_BFND bf;

    if ((bf = FindBifNode(filename, line)) == NULL)
	return -1;

    switch (bf->variant) {
    case PROG_HEDR:
	if (!strcmp(name, bf->entry.Template.symbol->ident))
	    return 0;
	break;
    case PROC_HEDR:
	if (!strcmp(name, bf->entry.Template.symbol->ident))
	    return 1;
	break;
    case FUNC_HEDR:
    case PROC_STAT:
	if (!strcmp(name, bf->entry.Template.symbol->ident))
	    return 2;
	break;
    }
    if ((val=FindRef(bf->entry.Template.ll_ptr1,name)) != -1)
	return val;

    if ((val=FindRef(bf->entry.Template.ll_ptr2,name)) != -1)
	return val;

    if ((val=FindRef(bf->entry.Template.ll_ptr3,name)) != -1)
	return val;
    (void) sprintf(db_err_msg, "No such symbol \"%s\" in line %d",name, line);
    return -1;
}


/****************************************************************
 *								*
 *   EndOfLoop -- find line number of end of loop statement	*
 *								*
 *   Input:							*
 *	    filename - name of the file to be looked upon	*
 *	    line     - line number of the lopp statement	*
 *								*
 *   Output:							*
 *	    return the line number of the end-of-loop statement *
 *	    return -1 if error occured				*
 *								*
 ****************************************************************/
int
EndOfLoop(filename, line)
	char	*filename;
	int	 line;
{
    PTR_BFND bf;
    PTR_BLOB bl, bl1;

    if ( (bf = FindBifNode(filename, line)) != NULL) {
	bl1 = NULL;
	for (bl=bf->entry.for_node.control; bl; bl = bl->next)
	    bl1 = bl;
	if (bl1)
	    return bl1->ref->g_line;
    }
    return -1;
}


/****************************************************************
 *								*
 *    ProgName -- get the main program's name from data base	*
 *								*
 *    Input:							*
 *		proj -- poniter of project object		*
 *								*
 *    Output:							*
 *		A string that contains the program's name	*
 *		A NULL point if no main program exists		*
 *								*
 ****************************************************************/
char *
ProjName(proj)
	PTR_PROJ proj;
{
    PTR_BLOB b, bl;
    PTR_FILE f;

    for (b = proj->file_chain; b; b = b->next) {
	f = (PTR_FILE) b->ref;
	for (bl = f->global_bfnd->entry.Template.bl_ptr1; bl; bl = bl->next)
	    if (bl->ref->variant == PROG_HEDR)
		return (bl->ref->entry.Template.symbol->ident);
    }
    return NULL;
}


/****************************************************************
 *								*
 * GetLangType -- get the type of language of a file		*
 *								*
 * Input:							*
 *	bf - a bif node pointer (to represent a file)		*
 *								*
 * Output:							*
 *	An integer of value CSrc, ForSrc etc.  with the CSrc	*
 *	means this is a C program and ForSrc, a Fortran one.	*
 *	A -1 indicates something wrong.				*
 *								*
 ****************************************************************/
int
GetLangType(bf)
    PTR_BFND bf;
{
    PTR_BFND b;

    /* First, find the global bif node of this dep file */
    for(b = bf; b && b->variant == GLOBAL ; b = b->control_parent)
	;
    
    /* Its control_parent is set to the file object that contains it */
    return(b? ((PTR_FILE)b->control_parent)->lang: -1);
}
