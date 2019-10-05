/*********************************************************************/
/*                  pC++/Sage++  Copyright (C) 1993                  */
/*  Indiana University  University of Oregon  University of Rennes   */
/*********************************************************************/


/*------------------------------------------------------*
 *							*
 *	      Routines to read in BIF graph		*
 *							*
 *------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#ifdef SYS5
#include <string.h>
#else
#include <strings.h>
#endif

#ifdef __SPF
extern void addToCollection(const int line, const char *file, void *pointer, int type);
#endif

/*typedef unsigned int u_short;*/
#include "db.h"
#include "dep_str.h"
/*extern int strncmp(); */
#define NULL_CHECK(BASE,VALUE) ((VALUE) ? (BASE + (VALUE-1)): 0)

/*
 * External variables/functions referenced
 */
extern int debug;

int   language;			/* type of language of this dep file */

/*	       
 * Local variables
 */
static struct locs	floc;	/* used to read in preamble "floc" */
static struct preamble	head;	/* used to read in preamble "head" */
static struct bf_nd	bf;	/* used to read in bif nodes */
static struct ll_nd	ll;	/* used to read in ll nodes */
static struct sym_nd	sym;	/* used to read in symbol nodes */
static struct typ_nd	typ;	/* used to read in type nodes */
static struct lab_nd	lab;	/* used to read in label nodes */
static struct fil_nd	fil;	/* used to read in file nodes */
static struct cmt_nd	cmt;	/* used to read in comment nodes */
static struct dep_nd	dpd;	/* used to read in dep nodes */

static PTR_BLOB head_blob, cur_blob;
static PTR_BFND head_bfnd, cur_bfnd;
static PTR_LLND head_llnd, cur_llnd;
static PTR_SYMB head_symb, cur_symb;
static PTR_TYPE head_type, cur_type;
static PTR_DEP	head_dep,  cur_dep;
static PTR_LABEL head_lab, cur_lab;
static PTR_FNAME head_file;
static PTR_CMNT head_cmnt, cur_cmnt;
static PTR_BFND global_bfnd;

static char **strtbl;		/* starting address of string table */
static u_shrt tmp[10000];	/* temp working area */
static FILE *fd;		/* local copy of file id for the dep file */
static PTR_FILE lfi;
static int need_swap = 0;	/* set to 1 if we need to swap bytes */

void swab();
/********************************************************
 *			swap_w				*
 *							*
 *	     Swap bytes of one word (2 bytes)		*
 ********************************************************/
static void
swap_w(p)
	char *p;
{
    char c;

    c = *(p+1);
    *(p+1) = *p;
    *p = c;
}


/********************************************************
 *			swap_i				*
 *							*
 *	     Swap bytes of an integer (4 bytes)		*
 ********************************************************/
static void
swap_i(p)
	char *p;
{
    char c;

    c = *(p+3);			/* swap the 1st and 4th bytes */
    *(p+3) = *p;
    *p++ = c;
    c = *p;			/* swap the 2nd and 3rd bytes */
    *p = *(p+1);
    *(p+1) = c;
}


/********************************************************
 *			swap_l (phb)                    *
 *							*
 *	     Swap bytes of an 64bit long (8 bytes)      *
 ********************************************************/
/* UNDER CONSTRUCTION, FIXME */
/*static void
swap_l(p)
	char *p;
{
    char c;
    c = *(p+3);			// swap the 1st and 4th bytes 
    *(p+3) = *p;
    *p++ = c;
    c = *p;			// swap the 2nd and 3rd bytes 
    *p = *(p+1);
    *(p+1) = c;
}*/


/*------------------------------------------------------*
 *		   read_str_tbl				*
 *							*
 *	Read in the string table in dep file		*
 *------------------------------------------------------*/
static int
read_str_tbl()
{
    int	     i, n, sz;
    u_shrt  u;
    char    *s;
    char   **cp;

    /*
     * Fast forward to where the string table starts
     */
    if (fseek(fd, floc.strs, 0) < 0)
	return -1;

    /*
     * The first word is the total number of strings in the dep file
     */

    /* get size of string table */
    if ((int)fread( (char *) &u, sizeof(u_shrt), 1, fd) < 0) 
	return -1;

    if (need_swap)
	swap_w((char *)&u);
    sz = (int) u;
    if ((cp = strtbl = (char **)malloc(sz * sizeof(char *))) == NULL)
    {
        fprintf(stderr, "read_str_tbl: No more space\n");
        exit(1);
    }
#ifdef __SPF
    addToCollection(__LINE__, __FILE__,cp, 0);
#endif

    /*
     * Then followed by strings in the form of
     *		-------------------------
     *		| str length | contents	|
     *		-------------------------
     */
    for (i = 0; i < sz; i++) {
        /* get string length */
	if ((int)fread( (char *) &u, sizeof(u_shrt), 1, fd) < 0) 

	    return -1;
	if (need_swap)
	    swap_w((char *)&u);
	n = (int) u;
	if ((s = malloc(n+1)) == NULL) 
    {
	    fprintf(stderr, "read_str_tbl: No more space\n");
	    exit(1);
	}
#ifdef __SPF
    addToCollection(__LINE__, __FILE__,s, 0);
#endif
	if ((int)fread(s, sizeof(char), n, fd) < 0) /* now the content */
	    return -1;
	*(s+n) = '\0';
	*cp++ = s;
    }
    return 0;
}


/*--------------------------------------------------------------*
 *			read_preamble				*
 *	    Read in the preamble part of the dep file		*
 *--------------------------------------------------------------*/
static int
read_preamble()
{
    int i;
    char filemagic[10];

    /* The first 8 bytes is the file magic (see /etc/magic) PHB */
    if ((int)fread(filemagic, sizeof(char), 8, fd) < 0)
        return -1;
    if (strncmp("sage.dep",filemagic,8) != 0) {
      fprintf(stderr, "This is not a legal .dep file\n");
      return -2;
    }

    /* First word (2 bytes) in the dep file is a pre-selected magic number */
    if ((int)fread( (char *) tmp, sizeof(u_shrt), 1, fd) < 0)
	return -1;
    if (*tmp != D_MAGIC) {	/* Is this a dep file? */
	need_swap = 1;		/* No... */
	swap_w((char *)tmp);		/*  ... Maybe we need to swap bytes */
	if(*tmp != D_MAGIC) {	/* Try again */
	  fprintf(stderr, "Are you sure this is a legal dep file? %x\n",*tmp);
	  return -2;
	}
    }
    
    /*
     * The second part is for double checking machanism.  Here we have
     * the starting locations (offsets) of low level nodes, symbol nodes,
     * type nodes, label nodes, comment nodes, file nodes, dep nodes and
     * string table (relative to the beginning of file).
     */

    /* Some more data */
    if ((int)fread( (char *) &floc, sizeof(struct locs), 1, fd) < 0)
	return -1;

    if (need_swap) {
	swap_i((char *)&floc.llnd);	/* !! long !! 64bit? (phb) */
	swap_i((char *)&floc.symb);	/* !! long !! 64bit? (phb) */
	swap_i((char *)&floc.type);	/* !! long !! 64bit? (phb) */
	swap_i((char *)&floc.labs);	/* !! long !! 64bit? (phb) */
	swap_i((char *)&floc.cmnt);	/* !! long !! 64bit? (phb) */
	swap_i((char *)&floc.file);	/* !! long !! 64bit? (phb) */
	swap_i((char *)&floc.deps);	/* !! long !! 64bit? (phb) */
	swap_i((char *)&floc.strs);	/* !! long !! 64bit? (phb) */
    }
    
    /* Reconstruct the string table first */
    if (read_str_tbl() < 0)
	return -1;

    /* rewind back to the point after "locs" information (8 is filemagic) */
    if (fseek(fd, sizeof(u_shrt)+sizeof(struct locs)+8, 0) < 0)
	return -1;

    /*
     * Read in the second part of preamble.  Here we have numbers of
     * all nodes (bif, low level, etc.) for this dep file
     */
    if ((int)fread( (char *) &head, sizeof(struct preamble), 1, fd) < 0)
	return -1;
    if (need_swap)
	swab((char *)&head, (char *)&head, sizeof(struct preamble));

    language = lfi->lang = (int)head.language;

    if ((sizeof(void *) * 8) != (int) head.ptrsize) {
	fprintf(stderr, "WARNING: .dep file created on a %d bit machine\n",
            head.ptrsize);
	return -2;
      }
	
    lfi->num_blobs = (int) head.num_blobs;
    lfi->num_bfnds = (int) head.num_bfnds;
    lfi->num_llnds = (int) head.num_llnds;
    lfi->num_symbs = (int) head.num_symbs;
    lfi->num_types = (int) head.num_types;
    lfi->num_label = (int) head.num_label;
    lfi->num_dep   = (int) head.num_dep;
    lfi->num_cmnt  = (int) head.num_cmnts;
    lfi->num_files = (int) head.num_files;

    /*
     * Now use those numbers to allocate all nodes for this dep file
     */
    lfi->head_blob = head_blob = (PTR_BLOB)(lfi->num_blobs>0? calloc(lfi->num_blobs, sizeof(struct blob)): NULL);
    lfi->head_bfnd = head_bfnd = (PTR_BFND)(lfi->num_bfnds>0? calloc(lfi->num_bfnds, sizeof(struct bfnd)): NULL);
    lfi->head_llnd = head_llnd = (PTR_LLND)(lfi->num_llnds>0? calloc(lfi->num_llnds, sizeof(struct llnd)): NULL);
    lfi->head_symb = head_symb = (PTR_SYMB)(lfi->num_symbs>0? calloc(lfi->num_symbs, sizeof(struct symb)): NULL);
    lfi->head_type = head_type = (PTR_TYPE)(lfi->num_types>0? calloc(lfi->num_types, sizeof(struct data_type)): NULL);
    lfi->head_dep  = head_dep  = (PTR_DEP)(lfi->num_dep >0 ?  calloc(lfi->num_dep,	 sizeof(struct dep)) : NULL);
    lfi->head_lab  = head_lab  = (PTR_LABEL)(lfi->num_label>0? calloc(lfi->num_label, sizeof(struct Label)): NULL);
    lfi->head_cmnt = head_cmnt = (PTR_CMNT)(lfi->num_cmnt>0 ? calloc(lfi->num_cmnt,	 sizeof(struct cmnt)): NULL);
    lfi->head_file = head_file = (PTR_FNAME)(lfi->num_files>0? calloc(lfi->num_files, sizeof(struct file_name)): NULL);
     
#ifdef __SPF
    if (lfi->head_blob) addToCollection(__LINE__, __FILE__,lfi->head_blob, 0);
    if (lfi->head_bfnd) addToCollection(__LINE__, __FILE__,lfi->head_bfnd, 0);
    if (lfi->head_llnd) addToCollection(__LINE__, __FILE__,lfi->head_llnd, 0);
    if (lfi->head_symb) addToCollection(__LINE__, __FILE__,lfi->head_symb, 0);
    if (lfi->head_type) addToCollection(__LINE__, __FILE__,lfi->head_type, 0);
    if (lfi->head_dep) addToCollection(__LINE__, __FILE__,lfi->head_dep, 0);
    if (lfi->head_lab) addToCollection(__LINE__, __FILE__,lfi->head_lab, 0);
    if (lfi->head_cmnt) addToCollection(__LINE__, __FILE__,lfi->head_cmnt, 0);
    if (lfi->head_file) addToCollection(__LINE__, __FILE__,lfi->head_file, 0);
#endif

    lfi->global_bfnd = global_bfnd = head_bfnd + ((int)head.global_bfnd - 1);

    cur_blob   = head_blob;
    cur_bfnd   = lfi->num_bfnds>0 ? head_bfnd + (lfi->num_bfnds - 1) : NULL;
    cur_llnd   = lfi->num_llnds>0 ? head_llnd + (lfi->num_llnds - 1) : NULL;
    cur_symb   = lfi->num_symbs>0 ? head_symb + (lfi->num_symbs - 1) : NULL;
    cur_type   = lfi->num_types>0 ? head_type + (lfi->num_types - 1) : NULL;
    cur_dep    = lfi->num_dep  >0 ? head_dep  + (lfi->num_dep	- 1) : NULL;
    cur_lab    = lfi->num_label>0 ? head_lab  + (lfi->num_label - 1) : NULL;
    cur_cmnt   = lfi->num_cmnt >0 ? head_cmnt + (lfi->num_cmnt	- 1) : NULL;

    for (i = 0; i < lfi->num_bfnds; i++) {
	(head_bfnd + i)->id	= i + 1;
	(head_bfnd + i)->thread = head_bfnd + (i + 1);
    }
    if (lfi->num_bfnds > 0)	/* the thread field of the last entry was... */
	cur_bfnd->thread = NULL; /* ...changed in the previous loop */

    for (i = 0; i < lfi->num_llnds; i++) {
	(head_llnd + i)->id	= i + 1;
	(head_llnd + i)->thread = head_llnd + (i + 1);
    }
    if (lfi->num_llnds > 0)
	cur_llnd->thread = NULL;

    for (i = 0; i < lfi->num_symbs; i++) {
	(head_symb + i)->id	= i + 1;
	(head_symb + i)->thread = head_symb + (i + 1);
    }
    if (lfi->num_symbs > 0)
	cur_symb->thread = NULL;

    for (i = 0; i < lfi->num_types; i++) {
	(head_type + i)->id	= i + 1;
	(head_type + i)->thread = head_type + (i + 1);
    }
    if (lfi->num_types > 0)
	cur_type->thread = NULL;

    for (i = 0; i < lfi->num_files; i++){
	(head_file + i)->id = i + 1;
	(head_file + i)->next = head_file + (i + 1);
    }
    if (lfi->num_files > 0)
	(head_file+(lfi->num_files-1))->next = NULL;

    for (i = 0; i < lfi->num_dep; i++) {
	(head_dep + i)->id     = i + 1;
	(head_dep + i)->thread = head_dep + (i + 1);
    }
    if (lfi->num_dep > 0)
	cur_dep->thread = NULL;

    for (i = 0; i < lfi->num_label; i++) {
	(head_lab + i)->id     = i + 1;
	(head_lab + i)->next = head_lab + (i + 1);
    }
    if (lfi->num_label > 0)
	cur_lab->next = NULL;

    for (i = 0; i < lfi->num_cmnt; i++) {
	(head_cmnt + i)->id	= i + 1;
	(head_cmnt + i)->thread = head_cmnt + (i + 1);
    }
    if (lfi->num_cmnt > 0)
	cur_cmnt->thread = NULL;
    return 0;
}


/*------------------------------------------------------*
 *		    read_blob_nodes			*
 *							*
 *		Reads in a blob list			*
 *------------------------------------------------------*/
static PTR_BLOB
read_blob_nodes() 
{
    int i, n;
    PTR_BLOB head, blnd_ptr = NULL;

    /* read in the count */
    if ((int)fread( (char *) tmp, sizeof(u_shrt), 1, fd) < 0) { 
	perror("read_blob_nodes:");
	return NULL;
    }
    if (need_swap)
	    swap_w((char *)tmp);
    if (!(n = (int)(*tmp)))
	return NULL;		/* count = 0; empty list */

    head = cur_blob;

    /* read in blob list */
    if ((int)fread( (char *) tmp, sizeof(u_shrt), n, fd) < 0) { 
	perror("read_blob_nodes:");
	return NULL;
    }
    if (need_swap)
	    swab((char *)tmp, (char*)tmp, n*sizeof(u_shrt));

    for (i = 0; i < n; i++) {	/* re-contruct the blob nodes */
	blnd_ptr	= cur_blob++;
	blnd_ptr->next	= cur_blob;
	blnd_ptr->ref	= head_bfnd + (tmp[i] - 1);
    }
    blnd_ptr->next = NULL;

    return head;
}


/*--------------------------------------------------------------*
 *			read_bif_nodes				*
 *								*
 *		routines to read in bif nodes			*
 *--------------------------------------------------------------*/
static int
read_bif_nodes() 
{
    PTR_BFND bfnd_ptr;
    int i;

    for (i = 0; i < lfi->num_bfnds; i++) {
        /* read in a bif node */
	if ((int)fread( (char *) &bf, sizeof(struct bf_nd), 1, fd) < 0) 
	    return -1;
	if (need_swap)
	    swab((char *)&bf, (char *)&bf, sizeof(struct bf_nd));
	if (debug)
	    fprintf(stderr,"Processing bif %d\n",i);
	bfnd_ptr		 = head_bfnd + i;
	bfnd_ptr->variant	 = (int) bf.variant;
	bfnd_ptr->filename	 = NULL_CHECK(head_file, bf.filename);
	bfnd_ptr->control_parent = NULL_CHECK(head_bfnd, bf.cp);
	bfnd_ptr->label		 = NULL_CHECK(head_lab, bf.label);
	bfnd_ptr->entry.Template.bf_ptr1  = NULL_CHECK(head_bfnd,bf.bf_ptr1);
	bfnd_ptr->entry.Template.cmnt_ptr = NULL_CHECK(head_cmnt,bf.cmnt_ptr);
	bfnd_ptr->entry.Template.symbol	  = NULL_CHECK(head_symb,bf.symbol);
	bfnd_ptr->entry.Template.ll_ptr1  = NULL_CHECK(head_llnd,bf.ll_ptr1);
	bfnd_ptr->entry.Template.ll_ptr2  = NULL_CHECK(head_llnd,bf.ll_ptr2);
	bfnd_ptr->entry.Template.ll_ptr3  = NULL_CHECK(head_llnd,bf.ll_ptr3);
	bfnd_ptr->entry.Template.dep_ptr1 = NULL_CHECK(head_dep, bf.dep_ptr1);
	bfnd_ptr->entry.Template.dep_ptr2 = NULL_CHECK(head_dep, bf.dep_ptr2);
	bfnd_ptr->entry.Template.lbl_ptr  = NULL_CHECK(head_lab, bf.lbl_ptr);
	bfnd_ptr->g_line		  = (int) bf.g_line;
	bfnd_ptr->l_line		  = (int) bf.l_line;
	bfnd_ptr->decl_specs		  = (int) bf.decl_specs;
	bfnd_ptr->entry.Template.bl_ptr1  = read_blob_nodes();
	bfnd_ptr->entry.Template.bl_ptr2  = read_blob_nodes();
    }
    return 0;
}


/*--------------------------------------------------------------*
 *			read_ll_nodes				*
 *								*
 *		      routines to read ll_nodes			*
 *--------------------------------------------------------------*/
static int
read_ll_nodes()
{
    PTR_LLND llnd_ptr;
    int i;

    for(i = 0; i < lfi->num_llnds; i++) {
	if ((int)fread( (char *) &ll, sizeof(struct ll_nd), 1, fd) < 0)
	    return -1;
	if (need_swap)
	    swab((char *)&ll, (char *)&ll, sizeof(struct ll_nd));

	llnd_ptr	  = head_llnd + i;
	llnd_ptr->variant = (int) ll.variant; 
	llnd_ptr->type	  = NULL_CHECK(head_type, ll.type);

	switch(llnd_ptr->variant) {
	case INT_VAL :
	    if ((int)fread( (char *) &llnd_ptr->entry.ival, sizeof(int), 1, fd) < 0)
		return -1;
	    if (need_swap)
		swap_i((char *)&llnd_ptr->entry.ival);
	    break;
	case BOOL_VAL :
	    if ((int)fread( (char *) tmp, sizeof(u_shrt), 1, fd) < 0)
		return -1;
	    if (need_swap)
		swap_w((char *)tmp);
	    llnd_ptr->entry.bval = (int)(*tmp);
	    break;
	case CHAR_VAL :
	    if ((int)fread( (char *) tmp, sizeof(u_shrt), 1, fd) < 0)
		return -1;
	    if (need_swap)
		swap_w((char *)tmp);
	    llnd_ptr->entry.cval = (char)(*tmp);
	    break;
	case DOUBLE_VAL:
	case FLOAT_VAL :
	case STMT_STR  :
	case STRING_VAL:
	case KEYWORD_VAL:
	    if ((int)fread( (char *) tmp, sizeof(u_shrt), 1, fd) < 0)
		return -1;
	    if (need_swap)
		swap_w((char *)tmp);
	    llnd_ptr->entry.string_val = *(strtbl+(*tmp));
	    break;
	case RANGE_OP  :
	case UPPER_OP  :
	case LOWER_OP  :
	    if ((int)fread( (char *) tmp, sizeof(u_shrt), 2, fd) < 0)
		return -1;
	    if (need_swap)
		swab((char *)tmp, (char *)tmp, 2*sizeof(u_shrt));
	    llnd_ptr->entry.array_op.symbol= NULL_CHECK(head_symb,(*tmp));
	    llnd_ptr->entry.array_op.dim   = (int)tmp[1];
	    break;
	case LABEL_REF :
	    if ((int)fread( (char *) tmp, sizeof(u_shrt), 1, fd) < 0)
		return -1;
	    if (need_swap)
		swap_w((char *)tmp);
	    llnd_ptr->entry.label_list.lab_ptr= NULL_CHECK(head_lab,(*tmp));
	    break;
/*	case ARITH_ASSGN_OP:*/	/* New added for VPC++	*/
/*	    if ((int)fread( (char *) tmp, sizeof(u_shrt), 3, fd) < 0)
		return -1;
	    if (need_swap)
		swab((char *)tmp, (char *)tmp, 3*sizeof(u_shrt));
*/		
/* The next line is a _REAL_ hack, I added the cast (PHB) */
/*	    llnd_ptr->entry.Template.symbol = (PTR_SYMB) ((int) tmp[0]);
	    llnd_ptr->entry.Template.ll_ptr1 = NULL_CHECK(head_llnd,tmp[1]);
	    llnd_ptr->entry.Template.ll_ptr2 = NULL_CHECK(head_llnd,tmp[2]);
	    break;
*/	    
	default:
	    if ((int)fread( (char *) tmp, sizeof(u_shrt), 3, fd) < 0)
		return -1;
	    if (need_swap)
		swab((char *)tmp, (char *)tmp, 3*sizeof(u_shrt));
	    llnd_ptr->entry.Template.symbol =NULL_CHECK(head_symb,(*tmp));
	    llnd_ptr->entry.Template.ll_ptr1=NULL_CHECK(head_llnd,tmp[1]);
	    llnd_ptr->entry.Template.ll_ptr2=NULL_CHECK(head_llnd,tmp[2]);
	}
    }
    return 0;
}


/*--------------------------------------------------------------*
 *								*
 *		  routines to read symbol table			*
 *								*
 *--------------------------------------------------------------*/
static int
read_symb_nodes()
{
    PTR_SYMB symb_ptr;
    int i;

    for(i = 0; i < lfi->num_symbs; i++) {
	if ((int)fread( (char *) &sym, sizeof(struct sym_nd), 1, fd) < 0)
	    return -1;
	if (need_swap)
	    swab((char *)&sym, (char *)&sym, sizeof(struct sym_nd));

	symb_ptr	    = head_symb + i;
	symb_ptr->variant   = (int) sym.variant;
	symb_ptr->type	    = NULL_CHECK(head_type, sym.type);
	symb_ptr->attr	    = (int) sym.attr;
	symb_ptr->next_symb = NULL_CHECK(head_symb, sym.next);
	symb_ptr->scope	    = NULL_CHECK(head_bfnd, sym.scope);
	symb_ptr->ident	    = *(strtbl + sym.ident);

	switch (symb_ptr->variant) {
	case DEFAULT	:
	case TYPE_NAME	:
	    if ((int)fread( (char *) tmp, sizeof(u_shrt), 1, fd) < 0)
		return -1;
            if (need_swap)
		swab((char *)tmp, (char *)tmp, 1*sizeof(u_shrt));
            symb_ptr->entry.Template.base_name = NULL_CHECK(head_symb,tmp[0]);
	    break;
	case CONST_NAME	:
	    if ((int)fread( (char *) tmp, sizeof(u_shrt), 2, fd) < 0)
		return -1;
	    if (need_swap)
		/*swap_w((char *)tmp);*/
                swab((char *)tmp, (char *)tmp, (2)*sizeof(u_shrt));
	    symb_ptr->entry.const_value = NULL_CHECK(head_llnd,(*tmp));
            symb_ptr->entry.Template.base_name = NULL_CHECK(head_symb,tmp[1]);
	    break;
	case ENUM_NAME	:
	case FIELD_NAME	:
	    if ((int)fread( (char *) tmp, sizeof(u_shrt), 5, fd) < 0)
		return -1;
	    if (need_swap)
		swab((char *)tmp, (char *)tmp, 5*sizeof(u_shrt));
	    symb_ptr->entry.field.tag	= (int)(*tmp);
	    symb_ptr->entry.field.next	= NULL_CHECK(head_symb,tmp[1]);
	    symb_ptr->entry.field.base_name= NULL_CHECK(head_symb,tmp[2]);
	    symb_ptr->entry.field.declared_name = NULL_CHECK(head_symb,tmp[3]);
	    symb_ptr->entry.field.restricted_bit= NULL_CHECK(head_llnd,tmp[4]);
	    break;
	case VARIABLE_NAME:
	    if ((int)fread( (char *) tmp, sizeof(u_shrt), 3+1, fd) < 0)
		return -1;
	    if (need_swap)
		swab((char *)tmp, (char *)tmp, (3+1)*sizeof(u_shrt));
	    symb_ptr->entry.var_decl.local  = (int)(*tmp);
	    symb_ptr->entry.var_decl.next_in= NULL_CHECK(head_symb,tmp[1]);
	    symb_ptr->entry.var_decl.next_out=NULL_CHECK(head_symb,tmp[2]);
            symb_ptr->entry.Template.base_name = NULL_CHECK(head_symb,tmp[3]);
	    break;
	case PROGRAM_NAME:
	    if ((int)fread( (char *) tmp, sizeof(u_shrt), 2+1, fd) < 0)
		return -1;
	    if (need_swap)
		swab((char *)tmp, (char *)tmp, (2+1)*sizeof(u_shrt));

	    symb_ptr->entry.prog_decl.symb_list = NULL_CHECK(head_symb,(*tmp));
	    symb_ptr->entry.prog_decl.prog_hedr = NULL_CHECK(head_bfnd,tmp[1]);
            symb_ptr->entry.Template.base_name = NULL_CHECK(head_symb,tmp[2]);
	    break;
	    break;
	case PROCEDURE_NAME :
        case PROCESS_NAME:
	case FUNCTION_NAME:	 
	case INTERFACE_NAME:
	    if ((int)fread( (char *) tmp, sizeof(u_shrt), 8+1, fd) < 0)
		return -1;
	    if (need_swap)
		swab((char *)tmp, (char *)tmp, (8+1)*sizeof(u_shrt));

	    symb_ptr->entry.proc_decl.num_input	 = (int)(*tmp);
	    symb_ptr->entry.proc_decl.num_output = (int)tmp[1];
	    symb_ptr->entry.proc_decl.num_io	 = (int)tmp[2];
	    symb_ptr->entry.proc_decl.in_list =NULL_CHECK(head_symb,tmp[3]);
	    symb_ptr->entry.proc_decl.out_list =NULL_CHECK(head_symb,tmp[4]);
	    symb_ptr->entry.proc_decl.symb_list=NULL_CHECK(head_symb,tmp[5]);
	    symb_ptr->entry.proc_decl.proc_hedr=NULL_CHECK(head_bfnd,tmp[6]);
            symb_ptr->entry.proc_decl.local_size = (int)tmp[7];
            symb_ptr->entry.Template.base_name = NULL_CHECK(head_symb,tmp[8]);
	    break;
	case MEMBER_FUNC:	/*  NEW ADDED FOR VPC */
	    if ((int)fread( (char *) tmp, sizeof(u_shrt), 11, fd) < 0)
		return -1;
	    if (need_swap)
		swab((char *)tmp, (char *)tmp, 11*sizeof(u_shrt));
	    symb_ptr->entry.member_func.num_input  = (int)(*tmp);
	    symb_ptr->entry.member_func.num_output = (int)tmp[1];
	    symb_ptr->entry.member_func.num_io	   = (int)tmp[2];
	    symb_ptr->entry.member_func.in_list =NULL_CHECK(head_symb,tmp[3]);
	    symb_ptr->entry.member_func.out_list =NULL_CHECK(head_symb,tmp[4]);
	    symb_ptr->entry.member_func.symb_list  =NULL_CHECK(head_symb,tmp[5]);
	    symb_ptr->entry.member_func.func_hedr  =NULL_CHECK(head_bfnd,tmp[6]);
	    symb_ptr->entry.member_func.next	   =NULL_CHECK(head_symb,tmp[7]);
	    symb_ptr->entry.member_func.base_name  =NULL_CHECK(head_symb,tmp[8]);
	    symb_ptr->entry.member_func.declared_name =NULL_CHECK(head_symb,tmp[9]);
            symb_ptr->entry.member_func.local_size = (int)tmp[10];
	
	    break;
	case VAR_FIELD :
	    if ((int)fread( (char *) tmp, sizeof(u_shrt), 4, fd) < 0)
		return -1;
	    if (need_swap)
		swab((char *)tmp, (char *)tmp, 4*sizeof(u_shrt));
	    symb_ptr->entry.variant_field.tag  = tmp[0];
	    symb_ptr->entry.variant_field.next = NULL_CHECK(head_symb, tmp[1]);
	    symb_ptr->entry.variant_field.base_name = NULL_CHECK(head_symb, tmp[2]);
	    symb_ptr->entry.variant_field.variant_list = NULL_CHECK(head_llnd, tmp[3]);
	    break;
	default:
	    if ((int)fread( (char *) tmp, sizeof(u_shrt), 1, fd) < 0)
		return -1;
            if (need_swap)
		swab((char *)tmp, (char *)tmp, 1*sizeof(u_shrt));
            symb_ptr->entry.Template.base_name = NULL_CHECK(head_symb,tmp[0]);
	    break;
	}
    }
    return 0;
}


/*----------------------------------------------------------------------*
 *									*
 *		    routines to read type table				*
 *									*
 *----------------------------------------------------------------------*/
static int
read_type_nodes()
{
    PTR_TYPE type_ptr;
    int i, uss1, uss2;

    for(i = 0; i < lfi->num_types; i++) {
	if ((int)fread( (char *) &typ, sizeof(struct typ_nd), 1, fd) < 0)
	    return -1;
	if (need_swap)
	    swab((char *)&typ, (char *)&typ, sizeof(struct typ_nd));

	type_ptr		= head_type + i;
	type_ptr->variant	= (int)typ.variant;
	type_ptr->name		= NULL_CHECK(head_symb,typ.name);

	switch (type_ptr->variant) {
	case T_INT	:
	case T_FLOAT	:
	case T_DOUBLE	:
	case T_CHAR	:
	case T_BOOL	:
        case T_COMPLEX  :
        case T_DCOMPLEX  :
	    if ((int)fread( (char *) tmp, sizeof(u_shrt), 2, fd) < 0)
		return -1;
	    if (need_swap)
		swap_w((char *)tmp); 
		/* swab((char *)tmp, (char *)tmp, sizeof(u_shrt)); */
	    type_ptr->entry.Template.ranges    = NULL_CHECK(head_llnd,tmp[0]);
	    type_ptr->entry.Template.kind_len  = NULL_CHECK(head_llnd,tmp[1]);
	    break;
	case T_STRING	:
	    if ((int)fread( (char *) tmp, sizeof(u_shrt), 3, fd) < 0)
		return -1;
	    if (need_swap)
		swap_w((char *)tmp); 
	    type_ptr->entry.Template.ranges    = NULL_CHECK(head_llnd,tmp[0]);
	    type_ptr->entry.Template.kind_len  = NULL_CHECK(head_llnd,tmp[1]);
	    type_ptr->entry.Template.dummy1    = (int)tmp[2];
	    break;
	case DEFAULT	:
	case T_VOID	:	/* NEW ADDED FOR VPC */
        case T_UNKNOWN  :
	case T_ENUM_FIELD:
	    break;
	case T_SUBRANGE	:
	    if ((int)fread( (char *) tmp, sizeof(u_shrt), 3, fd) < 0)
		return -1;
	    if (need_swap)
		swab((char *)tmp, (char *)tmp, 3*sizeof(u_shrt));
	    type_ptr->entry.subrange.base_type = NULL_CHECK(head_type,tmp[0]);
	    type_ptr->entry.subrange.lower     = NULL_CHECK(head_llnd,tmp[1]);
	    type_ptr->entry.subrange.upper     = NULL_CHECK(head_llnd,tmp[2]);
	    break;
	case T_ARRAY	:
	    if ((int)fread( (char *) tmp, sizeof(u_shrt), 3, fd) < 0)
		return -1;
	    if (need_swap)
		swab((char *)tmp, (char *)tmp, 3*sizeof(u_shrt));
	    type_ptr->entry.ar_decl.num_dimensions = (int)tmp[0];
	    type_ptr->entry.ar_decl.base_type = NULL_CHECK(head_type,tmp[1]);
	    type_ptr->entry.ar_decl.ranges    = NULL_CHECK(head_llnd,tmp[2]);
	    break;
	case T_LIST	:
	    if ((int)fread( (char *) tmp, sizeof(u_shrt), 1, fd) < 0)
		return -1;
	    if (need_swap)
		swap_w((char *)tmp);
	    type_ptr->entry.base_type = NULL_CHECK(head_type,(*tmp));
	    break;

	case T_RECORD	:
	    if ((int)fread( (char *) tmp, sizeof(u_shrt), 2, fd) < 0)
		return -1;
	    if (need_swap)
		swab((char *)tmp, (char *)tmp, 2*sizeof(u_shrt));
	    type_ptr->entry.re_decl.num_fields = (int)(*tmp);
	    type_ptr->entry.re_decl.first      = NULL_CHECK(head_symb,tmp[1]);
	    break;
	case T_DESCRIPT:	/*  NEW ADDED FOR VPC */
	    if ((int)fread( (char *) tmp, sizeof(u_shrt), 7, fd) < 0)
		return -1;
	    if (need_swap)
		swab((char *)tmp, (char *)tmp, 7*sizeof(u_shrt));
	    type_ptr->entry.descriptive.signed_flag	= (int)tmp[0] ;
					           uss1  = (int)tmp[1];
					           uss2  = (int)tmp[2];
	    type_ptr->entry.descriptive.long_short_flag = (int) ((uss1 << 16) | uss2);
	    type_ptr->entry.descriptive.mod_flag	= (int)tmp[3] ;
	    type_ptr->entry.descriptive.storage_flag	= (int)tmp[4] ;
	    type_ptr->entry.descriptive.access_flag	= (int)tmp[5] ;
	    type_ptr->entry.descriptive.base_type	= NULL_CHECK(head_type,tmp[6]);
	    break;
        case T_REFERENCE:	/*  NEW ADDED FOR VPC */
	case T_POINTER: {	/*  NEW ADDED FOR VPC */
	    short int s;
	    if ((int)fread( (char *) tmp, sizeof(u_shrt), 4, fd) < 0)
		return -1;
	    if (need_swap)
		swab((char *)tmp, (char *)tmp, 4*sizeof(u_shrt));
	    type_ptr->entry.Template.base_type = NULL_CHECK(head_type,tmp[0]);
	    s = tmp[1];		/* hack!! since this is a singed short */
	    type_ptr->entry.Template.dummy1    = (int) s;
					 uss1  = (int)tmp[2];
					 uss2  = (int)tmp[3];
	    type_ptr->entry.Template.dummy5    = (int) ((uss1 << 16) | uss2);
	    }
	    break;
	case T_FUNCTION:	/*  NEW ADDED FOR VPC */
	    if ((int)fread( (char *) tmp, sizeof(u_shrt), 1, fd) < 0)
		return -1;
	    if (need_swap)
		swap_w((char *)tmp);
	    type_ptr->entry.Template.base_type = NULL_CHECK(head_type,(*tmp));
	    break;
	case T_DERIVED_TYPE :	/*  NEW ADDED FOR VPC */
	    if ((int)fread( (char *) tmp, sizeof(u_shrt), 2, fd) < 0)
		return -1;
	    if (need_swap)
		swab((char *)tmp, (char *)tmp, 2*sizeof(u_shrt));
	    type_ptr->entry.derived_type.symbol = NULL_CHECK(head_symb,tmp[0]);
	    type_ptr->entry.derived_type.scope_symbol = NULL_CHECK(head_symb,tmp[1]);
	    break;
        case T_MEMBER_POINTER:  /* for C::*  same as derived collection in structure */
	case T_DERIVED_COLLECTION:	/*  NEW ADDED FOR PC++ */
	    if ((int)fread( (char *) tmp, sizeof(u_shrt), 2, fd) < 0)
		return -1;
	    if (need_swap)
		swab((char *)tmp, (char *)tmp, 2*sizeof(u_shrt));
	    type_ptr->entry.col_decl.collection_name = NULL_CHECK(head_symb,tmp[0]);
	    type_ptr->entry.col_decl.base_type = NULL_CHECK(head_type,tmp[1]);
	    break;
	case T_DERIVED_TEMPLATE:	/*  NEW ADDED FOR PC++ */
	    if ((int)fread( (char *) tmp, sizeof(u_shrt), 2, fd) < 0)
		return -1;
	    if (need_swap)
		swab((char *)tmp, (char *)tmp, 2*sizeof(u_shrt));
	    type_ptr->entry.templ_decl.templ_name = NULL_CHECK(head_symb,tmp[0]);
	    type_ptr->entry.templ_decl.args = NULL_CHECK(head_llnd,tmp[1]);
	    break;

	case T_ENUM  :
	case T_UNION :		/*  NEW ADDED FOR VPC */
	case T_CLASS :		/*  NEW ADDED FOR VPC */
	case T_STRUCT :		/*  NEW ADDED FOR VPC */
	case T_DERIVED_CLASS :	/*  NEW ADDED FOR VPC */
	case T_COLLECTION:	/*  NEW ADDED FOR PC++ */
	    if ((int)fread( (char *) tmp, sizeof(u_shrt), 4, fd) < 0)
		return -1;
	    if (need_swap)
		swab((char *)tmp, (char *)tmp, 4*sizeof(u_shrt));
	    type_ptr->entry.derived_class.num_fields = (int)tmp[0] ;
	    type_ptr->entry.derived_class.first	     = NULL_CHECK(head_symb,tmp[1]);
	    type_ptr->entry.derived_class.original_class = NULL_CHECK(head_bfnd,tmp[2]);
	    type_ptr->entry.derived_class.base_type = NULL_CHECK(head_type,tmp[3]);
	    break;

	default :
	    break;
	}
    }
    return 0;
}


/*----------------------------------------------------------------------*
 *			    read_label_nodes				*
 *									*
 *			  Reads the label nodes				*
 *----------------------------------------------------------------------*/
static int
read_label_nodes()
{
    PTR_LABEL lab_ptr;
    int i;

    for (i=0; i < lfi->num_label; i++) {
	if ((int)fread( (char *) &lab, sizeof(struct lab_nd), 1, fd) < 0)
	    return -1;
	if (need_swap) {
	    swab((char *)&lab, (char *)&lab, sizeof(struct lab_nd)-sizeof(long));
	    swap_i((char *)&lab.stat_no);
	}
	
	lab_ptr	= head_lab +i;
	lab_ptr->stateno = lab.stat_no;
	lab_ptr->labtype = lab.labtype;
	lab_ptr->statbody= NULL_CHECK(head_bfnd, lab.body);
	lab_ptr->label_name= NULL_CHECK(head_symb,lab.name); /* for VPC */
    }
    return 0;
}


/*----------------------------------------------------------------------*
 *			  read_dep_nodes				*
 *									*
 *			Reads the dep nodes				*
 *----------------------------------------------------------------------*/
static int
read_dep_nodes()
{
    PTR_DEP dep;
    int i, j;

    for ( i=0; i < lfi->num_dep; i++ ) {
	if ((int)fread( (char *) &dpd, sizeof(struct dep_nd), 1, fd) < 0)
	    return -1;
	if (need_swap)
	    swab((char *)&dpd, (char *)&dpd, sizeof(struct dep_nd));

	dep = head_dep + (--dpd.id);
	dep->type	= (int)dpd.type;
	dep->symbol	= NULL_CHECK(head_symb,dpd.sym);
	dep->from.stmt	= NULL_CHECK(head_bfnd,dpd.from_stmt);
	dep->from.refer = NULL_CHECK(head_llnd,dpd.from_ref);
	dep->to.stmt	= NULL_CHECK(head_bfnd,dpd.to_stmt);
	dep->to.refer	= NULL_CHECK(head_llnd,dpd.to_ref);
      /* i dont know what these are!!!
	dep->from_hook	= NULL_CHECK(head_bfnd,dpd.from_hook);
	dep->to_hook	= NULL_CHECK(head_bfnd,dpd.to_hook);
       */
	dep->from_fwd	= NULL_CHECK(head_dep,dpd.from_fwd);
	dep->from_back	= NULL_CHECK(head_dep,dpd.from_back);
	dep->to_fwd	= NULL_CHECK(head_dep,dpd.to_fwd);
	dep->to_back	= NULL_CHECK(head_dep,dpd.to_back);

	for (j=0; j<MAX_DEP; j++ ) {
	    dep->direct[j] = (char)dpd.dire[j];
	}
    }
    return 0;
}


/*----------------------------------------------------------------------*
 *			  read_cmnt_nodes				*
 *									*
 *			Reads the comment nodes				*
 *----------------------------------------------------------------------*/
static int
read_cmnt_nodes()
{
    PTR_CMNT cmnt = lfi->head_cmnt;
    int i;

    for (i = 0; i < lfi->num_cmnt; i++) {
	if ((int)fread( (char *) &cmt, sizeof(struct cmt_nd), 1, fd) < 0)
	    return -1;
	if (need_swap)
	    swab((char *)&cmt, (char *)&cmt, sizeof(struct cmt_nd));

	cmnt->type   = (int) cmt.type;
	cmnt->next   = NULL_CHECK(head_cmnt, cmt.next);
	cmnt->string = *(strtbl + cmt.str);
	cmnt++;
    }
    return 0;
}


/*
 * strip_dot_slash tries to strip "./" from the filename
 */
static
void strip_dot_slash(s)
    char *s;
{
    char *p, *q, ch;

    while ((ch = *s++))
	if (ch == '.') {
	    if (*s == '/') {
		p = q = s++ - 1;
		while ((*p++ = *s++));
		s = q;
	    } else if (*s == '.')
		s++;
	}
}


/*----------------------------------------------------------------------*
 *			  read_filename_nodes				*
 *									*
 *			Reads the filename nodes			*
 *----------------------------------------------------------------------*/
static int
read_filename_nodes()
{
    int i;
    PTR_FNAME fp = head_file;

    for (i = 0; i < lfi->num_files; i++) {
	if ((int)fread( (char *) &fil, sizeof(struct fil_nd), 1, fd) < 0)
	    return -1;
	if (need_swap)
	    swab((char *)&fil, (char *)&fil, sizeof(struct fil_nd));

	strip_dot_slash(fp->name = *(strtbl + fil.name));
	fp++;
    }
    lfi->filename  = head_file->name;
    return 0;
}


/*------------------------------------------------------*
 *		    read_nodes				*
 *							*
 *		Drives the read routines		*
 *------------------------------------------------------*/
int
read_nodes(fi)
	PTR_FILE fi;
{
    need_swap = 0;
    lfi = fi;
    fd = fi->fid;
    if (read_preamble() < 0)
	return -1;

    if (read_bif_nodes() < 0)
	return -1;
    if (debug)
	fprintf(stderr,"bif nodes loaded\n");

    if (ftell(fd) != floc.llnd) {
	fprintf (stderr,"read_nodes: wrong location of low level nodes\n");
	if (fseek(fd, floc.llnd, 0) < 0)
	    return -1;
    }
    if (read_ll_nodes() < 0) {
	perror("read_ll_nodes:");
	return -1;
    }

    if (debug)
	fprintf(stderr,"low level nodes loaded\n");

    if (ftell(fd) != floc.symb) {
	fprintf(stderr,"read_nodes: wrong location of symbol nodes\n");
	if(fseek(fd, floc.symb, 0) < 0)
	    return -1;
    }
    if (read_symb_nodes() < 0)
	return -1;
    if (debug)
	fprintf(stderr,"symbol table loaded \n");

    if (ftell(fd) != floc.type) {
	fprintf(stderr,"read_nodes: wrong location of type nodes\n");
	if(fseek(fd, floc.type, 0) < 0)
	    return -1;
    }
    if (read_type_nodes() < 0)
	return -1;
    if (debug)
	fprintf(stderr,"type table loaded \n");

    if (ftell(fd) != floc.labs) {
	fprintf(stderr,"read_nodes: wrong location of label nodes\n");
	if(fseek(fd, floc.labs, 0) < 0)
	    return -1;
    }
    if (read_label_nodes() < 0)
	return -1;
    if (debug)
	fprintf(stderr,"label table loaded\n");

    if (ftell(fd) != floc.cmnt) {
	fprintf(stderr,"read_nodes: wrong location of comment nodes\n");
	if(fseek(fd, floc.cmnt, 0) < 0)
	    return -1;
    }
    if (read_cmnt_nodes() < 0)
	return -1;
    if (debug)
	fprintf(stderr,"comment strings loaded \n");

    if (ftell(fd) != floc.file) {
	fprintf(stderr,"read_nodes: wrong location of filename nodes\n");
	if(fseek(fd, floc.file, 0) < 0)
	    return -1;
    }
    if (read_filename_nodes() < 0)
	return -1;
    if (debug)
	fprintf(stderr,"filename table loaded\n");

    if (ftell(fd) != floc.deps) {
	fprintf(stderr,"read_nodes: wrong location of dependence arc nodes\n");
	if(fseek(fd, floc.deps, 0) < 0)
	    return -1;
    }
    if (read_dep_nodes() < 0)
	return -1;
    if (debug)
	fprintf(stderr,"dependence arcs loaded \n");

    /* Now set up the returned values */
    global_bfnd->control_parent = (PTR_BFND) fi;
    fi->cur_blob  = cur_blob;
    fi->cur_bfnd  = cur_bfnd;
    fi->cur_llnd  = cur_llnd;
    fi->cur_symb  = cur_symb;
    fi->cur_type  = cur_type;
    fi->cur_dep	  = cur_dep;
    fi->cur_lab	  = cur_lab;
    fi->cur_cmnt  = cur_cmnt;
    return 0;
}

