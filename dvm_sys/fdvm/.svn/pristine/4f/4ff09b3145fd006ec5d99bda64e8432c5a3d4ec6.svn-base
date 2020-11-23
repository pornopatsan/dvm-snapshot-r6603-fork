/*********************************************************************/
/*                  pC++/Sage++  Copyright (C) 1993                  */
/*  Indiana University  University of Oregon  University of Rennes   */
/*********************************************************************/


/*------------------------------------------------------*
 *							*
 *	      Routines to write BIF graph out		*
 *							*
 *------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>

#include "compatible.h"
#ifdef SYS5
#include <string.h>
#else
#include <strings.h>
#endif
 
#ifdef __SPF
extern void addToCollection(const int line, const char *file, void *pointer, int type);
extern void removeFromCollection(void *pointer);
#endif

/*typedef unsigned int u_short;*/
#include "db.h"
#include "dep_str.h"
/*extern char* strncpy(); */
 
#define FOLLOW_BIF_POINTER_TO_ID(VAR) \
	(bf_ptr->entry.Template.VAR? bf_ptr-> entry.Template.VAR->id: 0)

#define FOLLOW_LL_POINTER_TO_ID(VAR) \
	(ll_ptr-> entry.Template.VAR? ll_ptr-> entry.Template.VAR->id: 0)

#define FOLLOW_SYMB_POINTER_1_TO_ID(VAR) \
	(sy_ptr->VAR? sy_ptr->VAR->id: 0)

#define FOLLOW_SYMB_POINTER_2_TO_ID(VAR) \
	(sy_ptr->entry.VAR? sy_ptr->entry.VAR->id: 0)

#define FOLLOW_TYPE_POINTER_TO_ID(VAR) \
	(ty_ptr->entry.VAR? ty_ptr->entry.VAR->id: 0)

#define FOLLOW_DEP_TO_ID(VAR) \
	(dep->VAR? dep->VAR->id: 0)

/*
 * External variables/functions referenced
 */

static PTR_BFND head_bfnd, cur_bfnd;
static PTR_LLND head_llnd, cur_llnd;
static PTR_SYMB head_symb, cur_symb;
static PTR_TYPE head_type, cur_type;
static PTR_DEP head_dep, cur_dep;
static PTR_LABEL head_label, cur_label;
static PTR_CMNT head_cmnt, cur_cmnt;
static PTR_FNAME head_file;
static PTR_BFND global_bfnd;

static int num_blobs;
static int num_bfnds;
static int num_llnds;
static int num_symbs;
static int num_types;
static int num_label;
static int num_cmnt;
static int num_files;
static int num_dep;

extern int language;
extern int debug;

/*
 * Local variables
 */
static struct preamble	head;
static struct bf_nd	bf;
static struct ll_nd	ll;
static struct sym_nd	sym;
static struct typ_nd	typ;
static struct lab_nd	lab;
static struct fil_nd	fil;
static struct cmt_nd	cmt;
static struct dep_nd	dpd;
static struct locs	loc;

static FILE  *fd;		/* file pointer of the dep file */
static char **strtbl,		/* start of string table */
	    **endtbl,		/* end of string table */
	    **cp;		/* current pointer */
static int nstr = 0;		/* no of string stored so far */
static int tblsz = 2000;	/* initial string table size */

static u_shrt tmp[100000];	/* some work space */

/*------------------------------------------------------*
 *			store_str			*
 *							*
 *	 put the given string into string table		*
 *------------------------------------------------------*/
static u_shrt
store_str(str)
	char *str;
{
    if (nstr >= tblsz) {
        tblsz += 1000;
#ifdef __SPF
        removeFromCollection(strtbl);        
#endif
        if (!(strtbl = (char **)realloc(strtbl, tblsz * sizeof(char **))))
        {
            fprintf(stderr, "store_str: No more space\n");
            exit(1);
        }
#ifdef __SPF
        addToCollection(__LINE__, __FILE__,strtbl, 0);
#endif
        endtbl = strtbl + tblsz;
        cp = strtbl + nstr;
    }
	*cp++ = str;
	return (u_shrt)nstr++;
}


/*------------------------------------------------------*
 *		find_global_bif_node			*
 *							*
 *    Find the global bif node (there is only one)	*
 *------------------------------------------------------*/
PTR_BFND 
find_global_bif_node()
{
	register PTR_BFND bf_node;

	bf_node = head_bfnd;
	while (bf_node->variant != GLOBAL)
		bf_node = bf_node->thread;

	return (bf_node);
}


/*------------------------------------------------------*
 *		    write_preamble			*
 *							*
 *	    Write the preamble of the dep file		*
 *------------------------------------------------------*/
static int
write_preamble()
{
  u_shrt magic_no = D_MAGIC;
  char filemagic[10];

  strncpy(filemagic,"sage.dep",8);
  /* The first 8 bytes is the file magic (see /etc/magic) PHB */
  if ((int)fwrite(filemagic, sizeof(char), 8, fd) < 0)
    return -1;

	if ((int)fwrite( (char *) &magic_no, sizeof(u_shrt), 1, fd) < 0)
		return -1;

	if ((int)fwrite( (char *) &loc, sizeof(struct locs), 1, fd) < 0)
		return -1;

        head.ptrsize    = (u_shrt) ( sizeof(void *) * 8 );
	head.language	= (u_shrt) language;
	head.num_blobs	= (u_shrt) num_blobs;
	head.num_bfnds	= (u_shrt) num_bfnds;
	head.num_llnds	= (u_shrt) num_llnds;
	head.num_symbs	= (u_shrt) num_symbs;
	head.num_types	= (u_shrt) num_types;
	head.num_label	= (u_shrt) num_label;
	head.global_bfnd= (u_shrt) global_bfnd->id;
	head.num_dep	= (u_shrt) num_dep;
	head.num_cmnts	= (u_shrt) num_cmnt;
	head.num_files	= (u_shrt) num_files;

	return (int)fwrite( (char *) &head, sizeof(struct preamble), 1, fd);
}


/*------------------------------------------------------*
 *		   write_blob_list			*
 *							*
 *	dump the blob list with the given head		*
 *------------------------------------------------------*/
static int 
write_blob_list(head)
	PTR_BLOB head;
{
	register PTR_BLOB bl_ptr;
	u_shrt *p;
	int n;

	for (bl_ptr = head, p = tmp+1; bl_ptr; bl_ptr = bl_ptr->next)
             if( bl_ptr->ref)
		*p++ = (u_shrt) bl_ptr->ref->id;

	n = p - tmp;	/* calculate the no of blob nodes in the list */
	tmp[0] = (u_shrt) n - 1;
	return (int)fwrite( (char *) tmp, sizeof(u_shrt), n, fd);
}


/*------------------------------------------------------*
 *		    write_bif_node			*
 *							*
 *	    routines to write out one bif node		*
 *------------------------------------------------------*/
static int 
write_bif_node(bf_ptr)
	PTR_BFND bf_ptr;
{
	bf.id	   = (u_shrt) bf_ptr->id;
	bf.variant = (u_shrt) bf_ptr->variant;
	bf.cp	   = (u_shrt) (bf_ptr->control_parent? bf_ptr->control_parent->id :0);
	bf.bf_ptr1 = (u_shrt) FOLLOW_BIF_POINTER_TO_ID(bf_ptr1);
	bf.cmnt_ptr= (u_shrt) FOLLOW_BIF_POINTER_TO_ID(cmnt_ptr);
	bf.symbol  = (u_shrt) FOLLOW_BIF_POINTER_TO_ID(symbol);
	bf.ll_ptr1 = (u_shrt) FOLLOW_BIF_POINTER_TO_ID(ll_ptr1);
	bf.ll_ptr2 = (u_shrt) FOLLOW_BIF_POINTER_TO_ID(ll_ptr2);
	bf.ll_ptr3 = (u_shrt) FOLLOW_BIF_POINTER_TO_ID(ll_ptr3);
	bf.dep_ptr1= (u_shrt) FOLLOW_BIF_POINTER_TO_ID(dep_ptr1);
	bf.dep_ptr2= (u_shrt) FOLLOW_BIF_POINTER_TO_ID(dep_ptr2);
	bf.label   = (u_shrt) (bf_ptr->label? bf_ptr->label->id: 0);
	bf.lbl_ptr = (u_shrt) FOLLOW_BIF_POINTER_TO_ID(lbl_ptr);
	bf.g_line  = (u_shrt) bf_ptr->g_line;
	bf.l_line  = (u_shrt) bf_ptr->l_line;
        bf.decl_specs = (u_shrt) bf_ptr->decl_specs;
	bf.filename= (u_shrt) (bf_ptr->filename? bf_ptr->filename->id: 0);

	if ((int)fwrite( (char *) &bf, sizeof(struct bf_nd), 1, fd) < 0)
		return -1;
	if (write_blob_list(bf_ptr->entry.Template.bl_ptr1) < 0)
		return -1;
	return write_blob_list(bf_ptr->entry.Template.bl_ptr2);
}


/*------------------------------------------------------*
 *		  write_bif_nodes			*
 *							*
 *	     routines to print bif nodes		*
 *------------------------------------------------------*/
static int 
write_bif_nodes()
{
	register PTR_BFND bf_ptr;

	for (bf_ptr = head_bfnd; bf_ptr; bf_ptr = bf_ptr->thread)
		if (write_bif_node(bf_ptr) < 0) {
			perror("write_bif_nodes:");
			return -1;
		}
	return 0;
}


/*------------------------------------------------------*
 *		     write_ll_node			*
 *							*
 *	       print out one low level node		*
 *------------------------------------------------------*/
static int 
write_ll_node(ll_ptr)
	PTR_LLND ll_ptr;
{
	int n = 0;

	ll.id	   = (u_shrt) ll_ptr->id;
	ll.variant = (u_shrt) ll_ptr->variant;
	ll.type	   = (u_shrt) (ll_ptr->type ? ll_ptr->type->id : 0);
	if ((int)fwrite( (char *) &ll, sizeof(struct ll_nd), 1, fd) < 0)
		return -1;

	switch (ll_ptr->variant) {
	    case INT_VAL:
		return (int)fwrite( (char *) &ll_ptr->entry.ival, sizeof(int), 1, fd);
	    case BOOL_VAL:
		tmp[0] = (u_shrt) ll_ptr->entry.bval;
		n = 1;
		break;
	    case CHAR_VAL:
		tmp[0] = (u_shrt) ll_ptr->entry.cval;
		n = 1;
		break;
	    case DOUBLE_VAL:
	    case FLOAT_VAL:
	    case STMT_STR:
	    case STRING_VAL:
	    case KEYWORD_VAL:
		tmp[0] = store_str(ll_ptr->entry.string_val);
		n = 1;
		break;
	    case RANGE_OP:
	    case UPPER_OP:
	    case LOWER_OP:
		tmp[0] = (u_shrt) (ll_ptr->entry.array_op.symbol ?
				    ll_ptr->entry.array_op.symbol->id :
				    0);
		tmp[1] = (u_shrt) ll_ptr->entry.array_op.dim;
		n = 2;
		break;
	    case LABEL_REF:
		tmp[0] = (u_shrt) ll_ptr->entry.label_list.lab_ptr->id;
		n = 1;
		break;
/*	    case ARITH_ASSGN_OP: */	/* New added for VPC++ */
/* The next line is a _REAL_ hack, I added the cast (PHB) */
/*		tmp[0] = (u_shrt) ((int) ll_ptr->entry.Template.symbol);
		tmp[1] = (u_shrt) FOLLOW_LL_POINTER_TO_ID(ll_ptr1);
		tmp[2] = (u_shrt) FOLLOW_LL_POINTER_TO_ID(ll_ptr2);
		n = 3;
		break;
*/		
	   default:
		tmp[0] = (u_shrt) FOLLOW_LL_POINTER_TO_ID(symbol);
		tmp[1] = (u_shrt) FOLLOW_LL_POINTER_TO_ID(ll_ptr1);
		tmp[2] = (u_shrt) FOLLOW_LL_POINTER_TO_ID(ll_ptr2);
		n = 3;
		break;
	}
	return (n? (int)fwrite( (char *) tmp, sizeof(u_shrt), n, fd): 0);
}


/*------------------------------------------------------*
 *		  write_ll_nodes			*
 *							*
 *		dump low level nodes			*
 *------------------------------------------------------*/
static int 
write_ll_nodes()
{
	register PTR_LLND ll_ptr;

	for (ll_ptr = head_llnd; ll_ptr; ll_ptr = ll_ptr->thread)
		if (write_ll_node(ll_ptr) < 0) {
			perror("write_ll_nodes:");
			return -1;
		}
	return 0;
}


/*------------------------------------------------------*
 *		    write_symb_node			*
 *							*
 *		print out one symbol node		*
 *------------------------------------------------------*/
static int 
write_symb_node(sy_ptr)
	PTR_SYMB sy_ptr;
{
	int n = 0;

	sym.id	    = (u_shrt) sy_ptr->id;
	sym.variant = (u_shrt) sy_ptr->variant;
	sym.type    = (u_shrt) FOLLOW_SYMB_POINTER_1_TO_ID(type);
	sym.attr    = (u_shrt) sy_ptr->attr;
	sym.next    = (u_shrt) FOLLOW_SYMB_POINTER_1_TO_ID(next_symb);
	sym.scope   = (u_shrt) (sy_ptr->scope? sy_ptr->scope->id: 0);
	sym.ident   = store_str(sy_ptr->ident);

	if ((int)fwrite( (char *) &sym, sizeof(struct sym_nd), 1, fd) < 0)
		return -1;

	switch (sy_ptr->variant) {
	    case CONST_NAME:
		tmp[0] = (u_shrt) FOLLOW_SYMB_POINTER_2_TO_ID(const_value);
                tmp[1] = (u_shrt) FOLLOW_SYMB_POINTER_2_TO_ID(Template.base_name);
		n = 2;
		break;
	    case ENUM_NAME:
	    case FIELD_NAME:
		tmp[0] = (u_shrt)sy_ptr->entry.field.tag;
		tmp[1] = (u_shrt)FOLLOW_SYMB_POINTER_2_TO_ID(field.next);
		tmp[2] = (u_shrt)FOLLOW_SYMB_POINTER_2_TO_ID(field.base_name);
		tmp[3] = (u_shrt)FOLLOW_SYMB_POINTER_2_TO_ID(field.declared_name); /* VPC++ */
		tmp[4] = (u_shrt)FOLLOW_SYMB_POINTER_2_TO_ID(field.restricted_bit); /* VPC++ */
		n = 5;
		break;
	    case VARIABLE_NAME:
		tmp[0] = (u_shrt)sy_ptr->entry.var_decl.local;
		tmp[1] = (u_shrt)FOLLOW_SYMB_POINTER_2_TO_ID(var_decl.next_in);
		tmp[2] = (u_shrt)FOLLOW_SYMB_POINTER_2_TO_ID(var_decl.next_out);
		n = 3;
                tmp[n] = (u_shrt) FOLLOW_SYMB_POINTER_2_TO_ID(Template.base_name);
                n++;
		break;
	    case PROGRAM_NAME:
		tmp[0] = (u_shrt)FOLLOW_SYMB_POINTER_2_TO_ID(prog_decl.symb_list);
		tmp[1] = (u_shrt)FOLLOW_SYMB_POINTER_2_TO_ID(prog_decl.prog_hedr);
		n = 2;
                tmp[n] = (u_shrt) FOLLOW_SYMB_POINTER_2_TO_ID(Template.base_name);
                n++;
		break;
	    case PROCEDURE_NAME:
            case PROCESS_NAME:
	    case FUNCTION_NAME:
	    case INTERFACE_NAME:
		tmp[0] = (u_shrt) sy_ptr->entry.proc_decl.num_input;
		tmp[1] = (u_shrt) sy_ptr->entry.proc_decl.num_output;
		tmp[2] = (u_shrt) sy_ptr->entry.proc_decl.num_io;
		tmp[3] = (u_shrt) FOLLOW_SYMB_POINTER_2_TO_ID(proc_decl.in_list);
		tmp[4] = (u_shrt) FOLLOW_SYMB_POINTER_2_TO_ID(proc_decl.out_list);
		tmp[5] = (u_shrt) FOLLOW_SYMB_POINTER_2_TO_ID(proc_decl.symb_list);
		tmp[6] = (u_shrt) FOLLOW_SYMB_POINTER_2_TO_ID(proc_decl.proc_hedr);
                tmp[7] = (u_shrt) sy_ptr->entry.func_decl.local_size;
		n = 8;
                tmp[n] = (u_shrt) FOLLOW_SYMB_POINTER_2_TO_ID(Template.base_name);
                n++;
		break;
	    case MEMBER_FUNC:	 /*  NEW ADDED FOR VPC */
		tmp[0] = (u_shrt) sy_ptr->entry.member_func.num_input;
		tmp[1] = (u_shrt) sy_ptr->entry.member_func.num_output;
		tmp[2] = (u_shrt) sy_ptr->entry.member_func.num_io;
		tmp[3] = (u_shrt) FOLLOW_SYMB_POINTER_2_TO_ID(member_func.in_list);
		tmp[4] = (u_shrt) FOLLOW_SYMB_POINTER_2_TO_ID(member_func.out_list);
		tmp[5] = (u_shrt) FOLLOW_SYMB_POINTER_2_TO_ID(member_func.symb_list);
		tmp[6] = (u_shrt) FOLLOW_SYMB_POINTER_2_TO_ID(member_func.func_hedr);
		tmp[7] = (u_shrt) FOLLOW_SYMB_POINTER_2_TO_ID(member_func.next);
		tmp[8] = (u_shrt) FOLLOW_SYMB_POINTER_2_TO_ID(member_func.base_name);
		tmp[9] = (u_shrt) FOLLOW_SYMB_POINTER_2_TO_ID(member_func.declared_name);
                tmp[10] = (u_shrt) sy_ptr->entry.member_func.local_size;
		n = 11;
		break;
	    default:
                tmp[n] = (u_shrt) FOLLOW_SYMB_POINTER_2_TO_ID(Template.base_name);
                n++;
		break;
	}
       
	return (n? (int)fwrite( (char *) tmp, sizeof(u_shrt), n, fd): 0);
}


/*------------------------------------------------------*
 *		write_symb_nodes			*
 *							*
 *		dump symbol table			*
 *------------------------------------------------------*/
static int 
write_symb_nodes()
{
	register PTR_SYMB sy_ptr;

	for (sy_ptr = head_symb; sy_ptr; sy_ptr = sy_ptr->thread)
		if (write_symb_node(sy_ptr) < 0) {
			perror("write_symb_nodes:");
			return -1;
		}
	return 0;
}


/*------------------------------------------------------*
 *		  write_type_node			*
 *							*
 *	      print out one type node			*
 *------------------------------------------------------*/
static int 
write_type_node(ty_ptr)
	PTR_TYPE ty_ptr;
{       
	int	n = 0;
        int uss1;
	typ.id	    = (u_shrt) ty_ptr->id;
	typ.variant = (u_shrt) ty_ptr->variant;
	typ.name    = (u_shrt) (ty_ptr->name ? ty_ptr->name->id : 0);

	if ((int)fwrite( (char *) &typ, sizeof(struct typ_nd), 1, fd) < 0)
		return -1;

	switch (ty_ptr->variant) {
	    case T_INT:
	    case T_FLOAT:
	    case T_DOUBLE:
	    case T_CHAR:
	    case T_BOOL:
	    case T_COMPLEX:
	    case T_DCOMPLEX:
		tmp[0] = (u_shrt) FOLLOW_TYPE_POINTER_TO_ID(Template.ranges);
	        tmp[1] = (u_shrt) FOLLOW_TYPE_POINTER_TO_ID(Template.kind_len);
                n = 2;
		break;
	    case T_STRING:
		tmp[0] = (u_shrt) FOLLOW_TYPE_POINTER_TO_ID(Template.ranges);
	        tmp[1] = (u_shrt) FOLLOW_TYPE_POINTER_TO_ID(Template.kind_len);
		tmp[2] = (u_shrt) ty_ptr->entry.Template.dummy1;
                n = 3;
		break;                
	    case T_SUBRANGE:
		tmp[0] = (u_shrt) FOLLOW_TYPE_POINTER_TO_ID(subrange.base_type);
		tmp[1] = (u_shrt) FOLLOW_TYPE_POINTER_TO_ID(subrange.lower);
		tmp[2] = (u_shrt) FOLLOW_TYPE_POINTER_TO_ID(subrange.upper);
		n = 3;
		break;
	    case T_ARRAY:
		tmp[0] = (u_shrt) ty_ptr->entry.ar_decl.num_dimensions;
		tmp[1] = (u_shrt) FOLLOW_TYPE_POINTER_TO_ID(ar_decl.base_type);
		tmp[2] = (u_shrt) FOLLOW_TYPE_POINTER_TO_ID(ar_decl.ranges);		
		n = 3;
		break;
	    case T_LIST:
		tmp[0] = (u_shrt) FOLLOW_TYPE_POINTER_TO_ID(base_type);
		n = 1;
		break;
	    case T_RECORD:
		tmp[0] = (u_shrt) ty_ptr->entry.re_decl.num_fields;
		tmp[1] = (u_shrt) FOLLOW_TYPE_POINTER_TO_ID(re_decl.first);
		n = 2;
		break;
	    case T_DESCRIPT:   /*  NEW ADDED FOR VPC */
		tmp[0] = (u_shrt) ty_ptr->entry.descriptive.signed_flag ;
                uss1 = ty_ptr->entry.descriptive.long_short_flag;
                tmp[2] = (u_shrt) uss1;
                tmp[1] = (u_shrt) (uss1 >> 16);
		tmp[3] = (u_shrt) ty_ptr->entry.descriptive.mod_flag ;
		tmp[4] = (u_shrt) ty_ptr->entry.descriptive.storage_flag ;
		tmp[5] = (u_shrt) ty_ptr->entry.descriptive.access_flag ;
		tmp[6] = (u_shrt) FOLLOW_TYPE_POINTER_TO_ID(descriptive.base_type);
		n = 7;
		break;
	    case T_POINTER:	/*  NEW ADDED FOR VPC */
	    case T_REFERENCE:	/*  NEW ADDED FOR VPC */
		tmp[0] = (u_shrt) FOLLOW_TYPE_POINTER_TO_ID(Template.base_type);
		tmp[1] = (u_shrt) ty_ptr->entry.Template.dummy1 ; /* indirect level */
		uss1   = ty_ptr->entry.Template.dummy5 ; /* for const etc.  */
                tmp[3] = (u_shrt) uss1;
                tmp[2] = (u_shrt) (uss1 >> 16);
		n = 4;
		break;
		
	    case T_FUNCTION:   /*  NEW ADDED FOR VPC */
		tmp[0] = (u_shrt) FOLLOW_TYPE_POINTER_TO_ID(Template.base_type);
		n = 1;
		break;

	    case T_DERIVED_TYPE :   /*	NEW ADDED FOR VPC */
		tmp[0] = (u_shrt) FOLLOW_TYPE_POINTER_TO_ID(derived_type.symbol);
		tmp[1] = (u_shrt) FOLLOW_TYPE_POINTER_TO_ID(derived_type.scope_symbol);
		n = 2;
		break;
	    case T_MEMBER_POINTER:
	    case T_DERIVED_COLLECTION :	  /*  NEW ADDED FOR PC++ */
		tmp[0] = (u_shrt) FOLLOW_TYPE_POINTER_TO_ID(col_decl.collection_name);
		tmp[1] = (u_shrt) FOLLOW_TYPE_POINTER_TO_ID(col_decl.base_type);
		n = 2;
		break;
	    case T_DERIVED_TEMPLATE :	  /*  NEW ADDED FOR PC++ */
		tmp[0] = (u_shrt) FOLLOW_TYPE_POINTER_TO_ID(templ_decl.templ_name);
		tmp[1] = (u_shrt) FOLLOW_TYPE_POINTER_TO_ID(templ_decl.args);
		n = 2;
		break;
	    case T_ENUM:
	    case T_UNION:   /*	NEW ADDED FOR VPC */
	    case T_STRUCT:  /*	NEW ADDED FOR VPC */
	    case T_CLASS :  /*	NEW ADDED FOR VPC */
	    case T_DERIVED_CLASS :   /*	 NEW ADDED FOR VPC */
	    case T_COLLECTION:	     /*	 NEW ADDED FOR PC++ */
		tmp[0] = (u_shrt) ty_ptr->entry.derived_class.num_fields;
		tmp[1] = (u_shrt) FOLLOW_TYPE_POINTER_TO_ID(derived_class.first);
		tmp[2] = (u_shrt) FOLLOW_TYPE_POINTER_TO_ID(derived_class.original_class);
		tmp[3] = (u_shrt) FOLLOW_TYPE_POINTER_TO_ID(derived_class.base_type);
		n = 4;
		break;
		
	    default:
		break;
	}
	return (n? (int)fwrite( (char *) tmp, sizeof(u_shrt), n, fd): 0);
}


/*------------------------------------------------------*
 *		    write_type_nodes			*
 *------------------------------------------------------*/
static int 
write_type_nodes()
{
	register PTR_TYPE ty_ptr;

	for (ty_ptr = head_type; ty_ptr; ty_ptr = ty_ptr->thread)
		if (write_type_node(ty_ptr) < 0) {
			perror("write_type_nodes:");
			return -1;
		}
	return 0;
}


/*------------------------------------------------------*
 *		    write_label_node			*
 *------------------------------------------------------*/
static int 
write_label_node(lb_ptr)
	register PTR_LABEL lb_ptr;
{
	lab.id	    = (u_shrt) lb_ptr->id;
	lab.labtype = (u_shrt) lb_ptr->labtype;
	lab.body    = (u_shrt) (lb_ptr->statbody ? lb_ptr->statbody->id : 0);
	lab.name    = (u_shrt) (lb_ptr->label_name ? lb_ptr->label_name->id: 0);
	lab.stat_no = lb_ptr->stateno;
	return (int)fwrite( (char *) &lab, sizeof(struct lab_nd), 1, fd);
}


/*------------------------------------------------------*
 *		    write_label_nodes			*
 *------------------------------------------------------*/
static int 
write_label_nodes()
{
	register PTR_LABEL lb_ptr;

	for (lb_ptr = head_label; lb_ptr; lb_ptr = lb_ptr->next)
		if (write_label_node(lb_ptr) < 0) {
			perror("write_label_nodes:");
			return -1;
		}
	return 0;
}


/*------------------------------------------------------*
 *		  write_filename_nodes			*
 *------------------------------------------------------*/
static int 
write_filename_nodes()
{
	register PTR_FNAME filep;

	for (filep = head_file; filep; filep = filep->next) {
		fil.id	 = (u_shrt) filep->id;
		fil.name = store_str(filep->name);
		if ((int)fwrite( (char *) &fil, sizeof(struct fil_nd), 1, fd) < 0) {
			perror("write_filename_nodes:");
			return -1;
		}
	}
	return 0;
}


/*------------------------------------------------------*
 *		  write_comment_node			*
 *							*
 *	      print out one comment node		*
 *------------------------------------------------------*/
static int 
write_comment_node(cm_ptr)
	PTR_CMNT cm_ptr;
{
	cmt.id	 = (u_shrt) cm_ptr->id;
	cmt.type = (u_shrt) cm_ptr->type;
	cmt.next = (u_shrt) (cm_ptr->next ? cm_ptr->next->id : 0);
	cmt.str	 = store_str(cm_ptr->string);
	return (int)fwrite( (char *) &cmt, sizeof(struct cmt_nd), 1, fd);
}


/*------------------------------------------------------*
 *		    write_comment_nodes			*
 *------------------------------------------------------*/
static int
write_comment_nodes()
{
	register PTR_CMNT cm_ptr;

	for (cm_ptr = head_cmnt; cm_ptr; cm_ptr = cm_ptr->thread)
		if (write_comment_node(cm_ptr) < 0) {
			perror("write_comment_nodes:");
			return -1;
		}
	return 0;
}


/*------------------------------------------------------*
 *		   write_dep_node			*
 *							*
 *	    print out one dependence node		*
 *------------------------------------------------------*/
static int 
write_dep_node(dep)
	PTR_DEP dep;
{
	register int j;

	dpd.id		= (u_shrt) dep->id;
	dpd.type	= (u_shrt) dep->type;
	dpd.sym		= (u_shrt) FOLLOW_DEP_TO_ID(symbol);
	dpd.from_stmt	= (u_shrt) FOLLOW_DEP_TO_ID(from.stmt);
	dpd.from_ref	= (u_shrt) FOLLOW_DEP_TO_ID(from.refer);
	dpd.to_stmt	= (u_shrt) FOLLOW_DEP_TO_ID(to.stmt);
	dpd.to_ref	= (u_shrt) FOLLOW_DEP_TO_ID(to.refer);
	dpd.from_hook	= (u_shrt) 0; /* FOLLOW_DEP_TO_ID(from_hook); */
	dpd.to_hook	= (u_shrt) 0; /* FOLLOW_DEP_TO_ID(to_hook);   */
	dpd.from_fwd	= (u_shrt) FOLLOW_DEP_TO_ID(from_fwd);
	dpd.from_back	= (u_shrt) FOLLOW_DEP_TO_ID(from_back);
	dpd.to_fwd	= (u_shrt) FOLLOW_DEP_TO_ID(to_fwd);
	dpd.to_back	= (u_shrt) FOLLOW_DEP_TO_ID(to_back);

	for (j = 0; j < MAX_DEP; j++)
		dpd.dire[j] = (u_shrt) dep->direct[j];

	return (int)fwrite( (char *) &dpd, sizeof(struct dep_nd), 1, fd);
}



/*------------------------------------------------------*
 *		    write_dep_nodes			*
 *------------------------------------------------------*/
static int 
write_dep_nodes()
{
	register PTR_DEP dep;

	if (!num_dep)
		return 0;
	for (dep = head_dep; dep && dep->id != -1; dep = dep->thread)
		if (write_dep_node(dep) < 0) {
			perror("write_dep_nodes:");
			return -1;
		}
	return 0;
}


/*------------------------------------------------------*
 *		write_string				*
 *------------------------------------------------------*/
static int
write_string(str)
	char *str;
{
	int l1;

        if(!str) l1 = 0;
	else l1 = strlen(str);
	tmp[0] = (u_shrt) l1;
	if ((int)fwrite( (char *) tmp, sizeof(u_shrt), 1, fd) >= 0)
		if ((int)fwrite( (char *) str, sizeof(char), l1, fd) >= 0)
			return 0;
	return -1;
}


/*------------------------------------------------------*
 *		write_str_tbl				*
 *------------------------------------------------------*/
static int
write_str_tbl(str, n)
	char **str;
	int    n;
{
	register char **p = str;
	register int i;
	u_shrt u;

	u = (u_shrt) n;
	if ((int)fwrite( (char *) &u, sizeof(u_shrt), 1, fd) < 0) /* output no of strings */
		return -1;
	for (i = 0; i < n; i++)
		if (write_string(*p++) < 0) {
			perror("write_str_tbl:");
			return -1;
		}
	return 0;
}


/****************************************************************
 *								*
 *  fix_next_symb -- Try to fix the "next_symb" field in the	*
 *		     symbol table field so that they point to	*
 *		     the next symbol declared in the same scope	*
 ****************************************************************/
static void
  fix_next_symb()
{
  register int no = 0, i, max=0;
  register PTR_SYMB s;
  int *id;		/* table to store ids of difference scope */
  PTR_SYMB *pt;         /* point to the last symbol in that scope */
  
  /* This is a hack to find out how much memory we need to malloc (PHB) */
  for (s = head_symb; s; s = s->thread) max++;

  /* malloc the memory (PHB) */
  id = (int *)      malloc(sizeof(     int) * (max+100));
  pt = (PTR_SYMB *) malloc(sizeof(PTR_SYMB) * (max+100));
  if ((pt == 0) || (id == 0))
     { fprintf(stderr,"Out of memory in fix_next_symb\n"); exit(1); }

  for (s = head_symb; s; s = s->thread) {
    for (i = no - 1 ; i >= 0; --i)
      if ((s->scope != NULL) && (id[i] == s->scope->id)) 
        /* found one on the table */
	break;
    if (i >= 0) {	/* if already in table */
      if (i > max) 
	{ fprintf(stderr,"index out of range in fix_next_symb\n"); exit(1);}
      pt[i]->next_symb = s; /* add to the end in this scope */
      pt[i] = s; /* this one becomes the tail */
    } else
      if (s->scope) {	/* A new one -- add to the table */
	if (no > max) 
	  { fprintf(stderr,"index out of range in fix_next_symb\n"); exit(1);}
	id[no]	 = s->scope->id; /* id of new scope */
	pt[no++] = s; /* tail pointer */
      }
  }
  free(id);
  free(pt);
}


/*------------------------------------------------------*
 *							*
 *	     driver routines to print nodes		*
 *							*
 *------------------------------------------------------*/
int
write_nodes(fi, name)
	PTR_FILE  fi;
	char	 *name;
{
	if ((fd = fopen (name, "wb")) == NULL) {
		fprintf(stderr, "Could not open %s for write\n", name);
		return (-1);
	}

	head_bfnd = fi->head_bfnd;
	cur_bfnd = fi->cur_bfnd;
	head_llnd = fi->head_llnd;
	cur_llnd = fi->cur_llnd;
	head_symb = fi->head_symb;
	cur_symb = fi->cur_symb;
	head_type = fi->head_type;
	cur_type = fi->cur_type;
	head_dep = fi->head_dep;
	cur_dep = fi->cur_dep;
	head_label = fi->head_lab;
	cur_label = fi->cur_lab;
	head_cmnt = fi->head_cmnt;
	cur_cmnt = fi->cur_cmnt;
	head_file = fi->head_file;
	global_bfnd = fi->global_bfnd;

	num_blobs = fi->num_blobs;
	num_bfnds = fi->num_bfnds;
	num_llnds = fi->num_llnds;
	num_symbs = fi->num_symbs;
	num_types = fi->num_types;
	num_label = fi->num_label;
	num_cmnt = fi->num_cmnt;
	num_files = fi->num_files;
	num_dep = fi->num_dep;

	nstr = 0;
    if (strtbl == NULL)
    {
        if (!(strtbl = (char **)calloc(tblsz, sizeof(char *))))
        {
            perror("write_nodes(): calloc() error");
            return (-1);
        }
#ifdef __SPF
        addToCollection(__LINE__, __FILE__,strtbl, 0);
#endif
    }
	cp = strtbl;
	endtbl = strtbl + tblsz;

	if (!global_bfnd)
	  global_bfnd = find_global_bif_node();
	
	fix_next_symb();
	if (write_preamble() < 0) {
	  perror("write_nodes(): write_preamble() failed");
	  return (-1);
	}
	
	if (write_bif_nodes() < 0) {
	  perror("write_nodes(): write_bif_nodes() failed");
	  return (-1);
	}
	
	if ((loc.llnd = ftell(fd)) < 0)	{ 
	  perror("write_nodes(): ftell() failed (0)");
	  return (-1);
	}

	if (write_ll_nodes() < 0) { 
	  perror("write_nodes(): write_ll_nodes() failed");
	  return (-1);
	}

	if ((loc.symb = ftell(fd)) < 0) { 
	  perror("write_nodes(): ftell() failed (1)");
	  return (-1);
	}

	if (write_symb_nodes() < 0) { 
	  perror("write_nodes(): write_symb_nodes() failed");
	  return (-1);
	}

	if ((loc.type = ftell(fd)) < 0)	{ 
	  perror("write_nodes(): ftell() failed (2)");
	  return (-1);
	}

	if (write_type_nodes() < 0) { 
	  perror("write_nodes(): write_type_nodes() failed");
	  return (-1);
	}

	if ((loc.labs = ftell(fd)) < 0)	{ 
	  perror("write_nodes(): ftell() failed (3)");
	  return (-1);
	}

	if (write_label_nodes() < 0) { 
	  perror("write_nodes(): write_label_nodes() failed");
	  return (-1);
	}

	if ((loc.cmnt = ftell(fd)) < 0) { 
	  perror("write_nodes(): ftell() failed (4)");
	  return (-1);
	}

	if (write_comment_nodes() < 0) { 
	  perror("write_nodes(): write_comment_nodes() failed");
	  return (-1);
	}

	if ((loc.file = ftell(fd)) < 0)	{ 
	  perror("write_nodes(): ftell() failed (5)");
	  return (-1);
	}

	if (write_filename_nodes() < 0) { 
	  perror("write_nodes(): write_filename_nodes() failed");
	  return (-1);
	}

	if ((loc.deps = ftell(fd)) < 0)	{ 
	  perror("write_nodes(): ftell() failed (6)");
	  return (-1);
	}

	if (write_dep_nodes() < 0) {
	  perror("write_nodes(): write_dep_nodes() failed");
	  return (-1);
	}

	if ((loc.strs = ftell(fd)) < 0) { 
	  perror("write_nodes(): ftell() failed (7)");
	  return (-1);
	}

	if (write_str_tbl(strtbl, nstr) < 0) { 
	  perror("write_nodes(): write_str_tbl() failed");
	  return (-1);
	}

        /* Rewind to beginning of data segment (Magic + sage.dep) PHB */
	if (fseek(fd, (long)sizeof(u_shrt)+(long)8, 0) < 0) { 
	  perror("write_nodes(): fseek");
	  return -1;
	}
        /* write out the offsets */
	if ((int)fwrite( (char *) &loc, sizeof(struct locs), 1, fd) < 0) {
	  perror("write_nodes(): Could not write out offsets");
	  return -1;
	}

	if (fclose(fd) < 0) {
	  perror("write_nodes(): Could not close dep file");
	  return -1;
	}

	return 0;
}


int
rewrite_depfile (fi, name)
	PTR_FILE fi;
	char   *name;
{
	int	 i;
	PTR_BFND tmp;

	tmp = fi->global_bfnd->control_parent;
	fi->global_bfnd->control_parent = NULL;
	i = write_nodes (fi, name);
	fi->global_bfnd->control_parent = tmp;
	return i;
}

