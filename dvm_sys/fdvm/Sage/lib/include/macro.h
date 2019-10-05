/*********************************************************************/
/*                  pC++/Sage++  Copyright (C) 1993                  */
/*  Indiana University  University of Oregon  University of Rennes   */
/*********************************************************************/


/* declaration pour la toolbox 19/12/91 */

/* The following include files are sigma  include files */
#include "defs.h"
#include "bif.h"
#include "ll.h"
#include "symb.h"
#include "sets.h"
#include "db.h"
#include "vparse.h"

#ifdef CPLUS_
extern "C" PTR_FILE pointer_on_file_proj;   
#else
extern PTR_FILE pointer_on_file_proj;
#endif
/* the following are names of constants used by the C parser to */
/* add attributed to symbol table entries.                      */
/* For symbptr->attr   access with SYMB_ATTR(..) */
/* note these are ALSO IN FILE vpc.h and we should find a single spot for them!! */
#define ATT_CLUSTER     0
#define ATT_GLOBAL      1
#define PURE            8
#define PRIVATE_FIELD   16
#define PROTECTED_FIELD 32
#define PUBLIC_FIELD    64
#define ELEMENT_FIELD   128
#define COLLECTION_FIELD 256
#define CONSTRUCTOR      512
#define DESTRUCTOR       1024
#define PCPLUSPLUS_DOSUBSET 2048
#define INVALID             4096
#define SUBCOLLECTION       4096*2
#define OVOPERATOR      4096*4


/*  
 * There are 3 types  of macros:
 *       the first type deals with bif nodes and are named BIF_XXX
 *       the second type deals with symbol nodes and are named SYMB_XXX 
 *       the last type deasl with  low level nodes and are named NODE_XXX
 */

/* Macros for BIF NODE */
#define DECL_SOURCE_LINE(FUNC) ((FUNC)->g_line)
#define DECL_SOURCE_FILE(FUNC) (default_filename)
/* give the code of a node */
#define BIF_CODE(NODE) ((NODE)->variant)
#define BIF_LINE(NODE) ((NODE)->g_line)
#define BIF_LOCAL_LINE(NODE) ((NODE)->l_line)
#define BIF_DECL_SPECS(NODE) ((NODE)->decl_specs)
#define BIF_INDEX(NODE) ((NODE)->index)
/* give the identifier */
#define BIF_ID(NODE)   ((NODE)->id)
#define BIF_NEXT(NODE)   ((NODE)->thread)
#define BIF_CP(NODE)   ((NODE)->control_parent)
#define BIF_LABEL(NODE) ((NODE)->label)
#define BIF_LL1(NODE)  ((NODE)->entry.Template.ll_ptr1)
#define BIF_LL2(NODE)  ((NODE)->entry.Template.ll_ptr2)
#define BIF_LL3(NODE)  ((NODE)->entry.Template.ll_ptr3)
#define BIF_SYMB(NODE)  ((NODE)->entry.Template.symbol)
#define BIF_BLOB1(NODE)  ((NODE)->entry.Template.bl_ptr1)
#define BIF_BLOB2(NODE)  ((NODE)->entry.Template.bl_ptr2)
#define BIF_FLOW(NODE)  ((NODE)->entry.Template.bl_ptr1->ref)
#define BIF_FLOW_TRUE(NODE)  ((NODE)->entry.Template.bl_ptr1->ref)
#define BIF_FLOW_FALSE_EXIST(NODE)  ((NODE)->entry.Template.bl_ptr2)
#define BIF_FLOW_FALSE(NODE)  ((NODE)->entry.Template.bl_ptr2->ref)
#define BIF_FILE_NAME(NODE) ((NODE)->filename)
#define BIF_CMNT(NODE) ((NODE)->entry.Template.cmnt_ptr)
#define BIF_LABEL_USE(NODE) ((NODE)->entry.Template.lbl_ptr)
#define BIF_SETS(NODE)       ((NODE)->entry.Template.sets)
#define BIF_PROPLIST(NODE)  ((NODE)->prop_list)
/* seems to be useless not used that way???????*/
#define BIF_PROPLIST_NAME(NODE)  ((NODE)->prop_list.prop_name)
#define BIF_PROPLIST_VAL(NODE)  ((NODE)->prop_list.prop_val)
#define BIF_PROPLIST_NEXT(NODE)  ((NODE)->prop_list.next)

/* Macros for LOW LEVEL NODE*/

/* Give the code of the node */
#define NODE_CODE(NODE) ((NODE)->variant)
/* give the identifier */
#define NODE_ID(NODE)   ((NODE)->id)
#define NODE_NEXT(NODE) ((NODE)->thread)
#define NODE_CHAIN(NODE) ((NODE)->thread)
#define NODE_TYPE(NODE) ((NODE)->type)
#define NODE_STR(NODE)  ((NODE)->entry.string_val)
#define NODE_STRING_POINTER(NODE)  ((NODE)->entry.string_val)
#define NODE_IV(NODE)   ((NODE)->entry.ival)

/* use for integer constant 
   the boolean value is use if the constante is big
   (two integers) */
#define NODE_INT_CST_LOW(NODE)   ((NODE)->entry.ival)
#define NODE_DOUBLE_CST(NODE)  ((NODE)->entry.string_val)
#define NODE_FLOAT_CST(NODE)  ((NODE)->entry.string_val)
#define NODE_CHAR_CST(NODE)  ((NODE)->entry.cval)
#define NODE_BOOL_CST(NODE)  ((NODE)->entry.bval)
/* la partie haute est dans les noeuds info 
   A modifier par la suite */


#define NODE_CV(NODE)   ((NODE)->entry.cval)
#define NODE_DV(NODE)   ((NODE)->entry.dval)
#define NODE_REAL_CST(NODE)   ((NODE)->entry.dval)
#define NODE_BV(NODE)   ((NODE)->entry.bval)
#define NODE_ARRAY_OP(NODE) ((NODE)->entry.array_op)
#define NODE_TEMPLATE(NODE) ((NODE)->entry.Template)
#define NODE_SYMB(NODE) ((NODE)->entry.Template.symbol)
#define NODE_TEMPLATE_LL1(NODE) ((NODE)->entry.Template.ll_ptr1)
#define NODE_TEMPLATE_LL2(NODE) ((NODE)->entry.Template.ll_ptr2)
#define NODE_OPERAND0(NODE) ((NODE)->entry.Template.ll_ptr1)
#define NODE_PURPOSE(NODE) ((NODE)->entry.Template.ll_ptr1)
#define NODE_OPERAND1(NODE) ((NODE)->entry.Template.ll_ptr2)
#define NODE_OPERAND2(NODE) bif_sorry("OPERAND2")
#define NODE_VALUE(NODE) ((NODE)->entry.Template.ll_ptr2)
#define NODE_STRING_LENGTH(NODE) (strlen((NODE)->entry.string_val))
#define NODE_LABEL(NODE)          ((NODE)->entry.label_list.lab_ptr)
#define NODE_LIST_ITEM(NODE)       ((NODE)->entry.list.item)
#define NODE_LIST_NEXT(NODE)       ((NODE)->entry.list.next)

/* For symbole NODE */
#define SYMB_VAL(NODE)   ((NODE)->entry.const_value)
#define SYMB_DECLARED_NAME(NODE)   ((NODE)->entry.member_func.declared_name)
#define SYMB_CODE(NODE)   ((NODE)->variant)
#define SYMB_ID(NODE)   ((NODE)->id)
#define SYMB_IDENT(NODE)   ((NODE)->ident)
#define SYMB_PARENT(NODE)   ((NODE)->parent)
#define SYMB_DECL(NODE)   ((NODE)->decl)
#define SYMB_ATTR(NODE)   ((NODE)->attr)
#define SYMB_DOVAR(NODE)   ((NODE)->dovar)
#define SYMB_BLOC_NEXT(NODE)   ((NODE)->next_symb)
#define SYMB_NEXT(NODE)   ((NODE)->thread)
#define SYMB_LIST(NODE)   ((NODE)->id_list)
#define SYMB_TYPE(NODE)   ((NODE)->type)
#define SYMB_SCOPE(NODE)   ((NODE)->scope)
#define SYMB_UD_CHAIN(NODE) ((NODE)->ud_chain)
#define SYMB_ENTRY(NODE)   ((NODE)->entry)
#define SYMB_NEXT_DECL(NODE)   ((NODE)->entry.var_decl.next_in)
#define SYMB_NEXT_FIELD(NODE)   ((NODE)->entry.field.next)
#define SYMB_RESTRICTED_BIT(NODE)   ((NODE)->entry.field.restricted_bit)
#define SYMB_BASE_NAME(NODE)   ((NODE)->entry.Template.base_name)
#define SYMB_FUNC_HEDR(NODE)   ((NODE)->entry.func_decl.func_hedr)
#define SYMB_FUNC_PARAM(NODE)   ((NODE)->entry.proc_decl.in_list)
#define SYMB_FUNC_NB_PARAM(NODE) ((NODE)->entry.proc_decl.num_input)
#define SYMB_FUNC_OUTPUT(NODE)   ((NODE)->entry.proc_decl.num_output)
#define SYMB_FIELD_BASENAME(NODE)   ((NODE)->entry.field.base_name)
#define SYMB_FIELD_TAG(NODE)   ((NODE)->entry.field.tag)
#define SYMB_FIELD_DECLARED_NAME(NODE)   ((NODE)->entry.field.declared_name)
#define SYMB_FIELD_OFFSET(NODE)   ((NODE)->entry.field.offset)
#define SYMB_MEMBER_BASENAME(NODE)   ((NODE)->entry.member_func.base_name)
#define SYMB_MEMBER_NEXT(NODE)   ((NODE)->entry.member_func.next)
#define SYMB_MEMBER_HEADER(NODE)   ((NODE)->entry.member_func.func_hedr)
#define SYMB_MEMBER_LIST(NODE)   ((NODE)->entry.member_func.symb_list)
#define SYMB_MEMBER_PARAM(NODE)   ((NODE)->entry.member_func.in_list)
#define SYMB_MEMBER_TAG(NODE)   ((NODE)->entry.member_func.tag)
#define SYMB_MEMBER_OFFSET(NODE)   ((NODE)->entry.member_func.offset)
#define SYMB_MEMBER_DECLARED_NAME(NODE)   ((NODE)->entry.member_func.declared_name)
#define SYMB_MEMBER_OUTLIST(NODE)   ((NODE)->entry.member_func.out_list)
#define SYMB_MEMBER_NB_OUTPUT(NODE)   ((NODE)->entry.member_func.num_output)
#define SYMB_MEMBER_NB_IO(NODE)   ((NODE)->entry.member_func.num_io)

/* for Template */
#define SYMB_TEMPLATE_DUMMY1(NODE)   ((NODE)->entry.Template.seen)
#define SYMB_TEMPLATE_DUMMY2(NODE) ((NODE)->entry.Template.num_input)
#define SYMB_TEMPLATE_DUMMY3(NODE) ((NODE)->entry.Template.num_output)
#define SYMB_TEMPLATE_DUMMY4(NODE) ((NODE)->entry.Template.num_io)
#define SYMB_TEMPLATE_DUMMY5(NODE) ((NODE)->entry.Template.in_list)
#define SYMB_TEMPLATE_DUMMY6(NODE) ((NODE)->entry.Template.out_list)
#define SYMB_TEMPLATE_DUMMY7(NODE) ((NODE)->entry.Template.symb_list)
#define SYMB_TEMPLATE_DUMMY8(NODE) ((NODE)->entry.Template.local_size)
#define SYMB_TEMPLATE_DUMMY9(NODE) ((NODE)->entry.Template.label_list)
#define SYMB_TEMPLATE_DUMMY10(NODE) ((NODE)->entry.Template.func_hedr)
#define SYMB_TEMPLATE_DUMMY11(NODE) ((NODE)->entry.Template.call_list)
#define SYMB_TEMPLATE_DUMMY12(NODE) ((NODE)->entry.Template.tag)
#define SYMB_TEMPLATE_DUMMY13(NODE) ((NODE)->entry.Template.offset)
#define SYMB_TEMPLATE_DUMMY14(NODE) ((NODE)->entry.Template.declared_name)
#define SYMB_TEMPLATE_DUMMY15(NODE) ((NODE)->entry.Template.next)
#define SYMB_TEMPLATE_DUMMY16(NODE) ((NODE)->entry.Template.base_name)


/* for BLOB NODE */

#define  BLOB_NEXT(NODE) ((NODE)->next)
#define  BLOB_VALUE(NODE) ((NODE)->ref)
#define  HEAD_BLOB(NODE)  ((NODE)->head_blob)

/* for type node */
#define TYPE_CODE(NODE)  ((NODE)->variant)
#define TYPE_ID(NODE)  ((NODE)->id)
#define TYPE_SYMB(NODE)  ((NODE)->name)
#define TYPE_UD_CHAIN(NODE) ((NODE)->ud_chain)
#define TYPE_LENGTH(NODE) ((NODE)->length)
#define TYPE_BASE(NODE)   ((NODE)->entry.Template.base_type)
#define TYPE_RANGES(NODE) ((NODE)->entry.Template.ranges)
#define TYPE_KIND_LEN(NODE) ((NODE)->entry.Template.kind_len)
#define TYPE_QUOTE(NODE) ((NODE)->entry.Template.dummy1)
#define TYPE_DIM(NODE) ((NODE)->entry.ar_decl.num_dimensions)
#define TYPE_DECL_BASE(NODE) ((NODE)->entry.ar_decl.base_type)
#define TYPE_DECL_RANGES(NODE) ((NODE)->entry.ar_decl.ranges)
#define TYPE_NEXT(NODE) ((NODE)->thread)
#define TYPE_DESCRIP(NODE) ((NODE)->entry.descriptive)
#define TYPE_DESCRIP_BASE_TYPE(NODE) ((NODE)->entry.descriptive.base_type)
#define TYPE_FIRST_FIELD(NODE) ((NODE)->entry.re_decl.first)
#define TYPE_UNSIGNED(NODE) ((NODE)->entry.descriptive.signed_flag)
#define TYPE_LONG_SHORT(NODE) ((NODE)->entry.descriptive.long_short_flag)
#define TYPE_MODE_FLAG(NODE) ((NODE)->entry.descriptive.mod_flag)
#define TYPE_STORAGE_FLAG(NODE) ((NODE)->entry.descriptive.storage_flag)
#define TYPE_ACCESS_FLAG(NODE) ((NODE)->entry.descriptive.access_flag)
#define TYPE_SYMB_DERIVE(NODE) ((NODE)->entry.derived_type.symbol)
#define TYPE_SCOPE_SYMB_DERIVE(NODE) ((NODE)->entry.derived_type.scope_symbol)
#define TYPE_COLL_BASE(NODE) ((NODE)->entry.col_decl.base_type)
#define TYPE_COLL_ORI_CLASS(NODE) ((NODE)->entry.derived_class.original_class)
#define TYPE_COLL_NUM_FIELDS(NODE) ((NODE)->entry.derived_class.num_fields)
#define TYPE_COLL_RECORD_SIZE(NODE) ((NODE)->entry.derived_class.record_size)
#define TYPE_COLL_FIRST_FIELD(NODE) ((NODE)->entry.derived_class.first)
#define TYPE_COLL_NAME(NODE)     ((NODE)->entry.col_decl.collection_name)
#define TYPE_TEMPL_NAME(NODE)     ((NODE)->entry.templ_decl.templ_name)
#define TYPE_TEMPL_ARGS(NODE)     ((NODE)->entry.templ_decl.args)
/* sepcial case for enumeral type */
#define TYPE_VALUES(NODE) ((NODE)->entry.Template.ranges) /* wrong, to verify */

/* To allow copies of type */
#define TYPE_TEMPLATE_BASE(NODE)   ((NODE)->entry.Template.base_type)
#define TYPE_TEMPLATE_DUMMY1(NODE) ((NODE)->entry.Template.dummy1)
#define TYPE_TEMPLATE_RANGES(NODE) ((NODE)->entry.Template.ranges)
#define TYPE_TEMPLATE_DUMMY2(NODE) ((NODE)->entry.Template.dummy2)
#define TYPE_TEMPLATE_DUMMY3(NODE) ((NODE)->entry.Template.dummy3)
#define TYPE_TEMPLATE_DUMMY4(NODE) ((NODE)->entry.Template.dummy4)
#define TYPE_TEMPLATE_DUMMY5(NODE) ((NODE)->entry.Template.dummy5)
/* Other */
#define FILE_OF_CURRENT_PROJ(PROJ) ((PROJ)->proj_name)
#define FUNCT_NAME(FUNC) ((FUNC)->entry.Template.symbol->ident)
#define FUNCT_SYMB(FUNC) ((FUNC)->entry.Template.symbol)
#define FUNCT_FIRST_PAR(FUNC) ((FUNC)->entry.Template.symbol->entry.func_decl.in_list)


#define MIN(X,Y) ((X) < (Y) ? (X) : (Y))
#define MAX(X,Y) ((X) > (Y) ? (X) : (Y))
#define CEIL(x,y) (((x) + (y) - 1) / (y))

/* extern pour Bif */

/* other type of low level node and decl */
#define CEIL_DIV_EXPR 1000
#define MAX_OP  1001
#define BIF_PARM_DECL 1002 
#define BIF_SAVE_EXPR 1003
#define MIN_OP 1004
#define BIF_ADDR_EXPR 1005
#define BIF_NOP_EXPR 1006
#define BIF_RTL_EXPR 1007
/* #define TRUNC_MOD_EXPR 1008  killed by dbg because in rid enum*/
/* #define TRUNC_DIV_EXPR 1009 killed by dbg because in rid enum*/
#define FLOOR_DIV_EXPR 1010
#define FLOOR_MOD_EXPR 1011
#define CEIL_MOD_EXPR 1012
#define ROUND_DIV_EXPR 1013
#define ROUND_MOD_EXPR 1014
#define RDIV_EXPR 1015
#define EXACT_DIV_EXPR 1016
#define COND_EXPR EXPR_IF 
#define CONVERT_EXPR 1017
/*#define MINUS_EXPR SUBT_OP  removed by Beckman*/
#define CONST_DECL 1018 /* to be modify */
#define ABS_EXPR 1019
#define BIT_NOT_EXPR BIT_COMPLEMENT_OP
#define NEGATE_EXPR MINUS_OP
#define TRUTH_ANDIF_EXPR 1020
#define TRUTH_AND_EXPR 1021
#define TRUTH_NOT_EXPR 1022
#define TRUTH_ORIF_EXPR 1023
#define POSTINCREMENT_EXPR PLUSPLUS_OP
#define PREINCREMENT_EXPR 1024
#define PREDECREMENT_EXPR 1025
#define COMPOUND_EXPR 1026
#define ENUMERAL_TYPE T_ENUM
#define FLOAT_EXPR 1027
/*#define RSHIFT_EXPR RSHIFT_OP
  #define LSHIFT_EXPR  LSHIFT_OP   removed by Pete Beckman*/
/* #define BIT_IOR_EXPR 1028 killed by dbg because in rid enum*/
/* #define BIT_XOR_EXPR 1029 killed by dbg because in rid enum*/
#define BIT_ANDTC_EXPR 1030
#define ERROR_MARK NULL
#define TRUTH_OR_EXPR 1031
#define FIX_TRUNC_EXPR 1032
#define RROTATE_EXPR 1033
#define LROTATE_EXPR 1034
#define RANGE_EXPR 1035
#define POSTDECREMENT_EXPR 1036
#define COMPONENT_REF RECORD_REF /* NODE SYMB define for this node */
#define INDIRECT_REF DEREF_OP
#define REFERENCE_TYPE 1037
/* #define CONSTRUCTOR 1038*/
#define FIX_FLOOR_EXPR 1039
#define FIX_ROUND_EXPR 1040
#define FIX_CEIL_EXPR 1041
#define FUNCTION_DECL 1042
#define MODIFY_EXPR 1043
#define REFERENCE_EXPR  1044
#define RESULT_DECL 1045
#define PARM_DECL 1046 /* not used */
#define CALL_EXPR 1047
#define INIT_EXPR 1048


/* other type for type node */
#define T_LITERAL 1100 /* not use */
#define T_SIZE 1101
#define LAST_CODE T_SIZE
/* end other type of node */

/* definition for project */

#define PROJ_FIRST_SYMB() (pointer_on_file_proj->head_symb)
#define PROJ_FIRST_TYPE() (pointer_on_file_proj->head_type)
#define PROJ_FIRST_LLND() (pointer_on_file_proj->head_llnd)
#define PROJ_FIRST_BIF() (pointer_on_file_proj->head_bfnd)
#define PROJ_FIRST_CMNT() (pointer_on_file_proj->head_cmnt)
#define PROJ_FIRST_LABEL() (pointer_on_file_proj->head_lab)

#define CUR_FILE_NUM_BIFS() (pointer_on_file_proj->num_bfnds)
#define CUR_FILE_NUM_LLNDS() (pointer_on_file_proj->num_llnds)
#define CUR_FILE_NUM_SYMBS() (pointer_on_file_proj->num_symbs)
#define CUR_FILE_NUM_TYPES() (pointer_on_file_proj->num_types)
#define CUR_FILE_NUM_LABEL() (pointer_on_file_proj->num_label)
#define CUR_FILE_NUM_BLOBS() (pointer_on_file_proj->num_blobs)
#define CUR_FILE_NUM_CMNT() (pointer_on_file_proj->num_cmnt)
#define CUR_FILE_CUR_BFND() (pointer_on_file_proj->cur_bfnd)
#define CUR_FILE_CUR_LLND() (pointer_on_file_proj->cur_llnd)
#define CUR_FILE_CUR_SYMB() (pointer_on_file_proj->cur_symb)
#define CUR_FILE_CUR_TYPE() (pointer_on_file_proj->cur_type)
#define CUR_FILE_GLOBAL_BFND() (pointer_on_file_proj->global_bfnd)
#define CUR_FILE_NAME() (pointer_on_file_proj->filename)
#define CUR_FILE_HEAD_FILE() (pointer_on_file_proj->head_file)


#define FILE_GLOBAL_BFND(FIL) ((FIL)->global_bfnd)
#define FILE_FILENAME(FIL)  ((FIL)->filename)
#define FILE_LANGUAGE(FIL) ((FIL)->lang)


#define CUR_PROJ_FILE_CHAIN() (cur_proj->file_chain)   /* modified by Pete */
#define CUR_PROJ_NAME() (cur_proj->proj_name) /* modified by Pete */

#define PROJ_FILE_CHAIN(PROJ) ((PROJ)->file_chain)
                                
/* use as a general pointer */

typedef  char *POINTER;
enum typenode { BIFNODE, LLNODE, SYMBNODE, TYPENODE, BLOBNODE,
                  BLOB1NODE, LABEL, FILENODE}; //add LABEL (Kataev 21.03.2013), FILE (Kataev 15.07.2013


#define MAXTILE 10 /* nombre maximum de boucle que l'on peut tiler */
#define MAX_STMT 100 /* nombre d'instruction d'une boucle */


/**************** For Comment Nodes *****************************/


#define CMNT_ID(NODE)      ((NODE)->id)
#define CMNT_TYPE(NODE)    ((NODE)->type)
#define CMNT_STRING(NODE)  ((NODE)->string)
#define CMNT_NEXT(NODE)    ((NODE)->thread)
#define CMNT_NEXT_ATTACH(NODE)    ((NODE)->next)


/**************** For LABEL NODES *****************************/

#define LABEL_ID(NODE)             ((NODE)->id)
#define LABEL_NEXT(NODE)           ((NODE)->next)
#define LABEL_UD_CHAIN(NODE)       ((NODE)->ud_chain)
#define LABEL_USED(NODE)           ((NODE)->labused)
#define LABEL_ILLEGAL(NODE)        ((NODE)->labinacc)
#define LABEL_DEFINED(NODE)        ((NODE)->labdefined)
#define LABEL_SCOPE(NODE)          ((NODE)->scope)
#define LABEL_BODY(NODE)           ((NODE)->statbody)
#define LABEL_SYMB(NODE)           ((NODE)->label_name)
#define LABEL_TYPE(NODE)           ((NODE)->labtype)
#define LABEL_STMTNO(NODE)         ((NODE)->stateno)


/**************** Misceallous ***********************************/

#define LABEL_KIND   100000  /* bigger than the variant of all kind of node*/
#define BLOB_KIND    100001
#define CMNT_KIND    100002

/************** For Sets Node ********************************/

#define SETS_GEN(NODE)       ((NODE)->gen)
#define SETS_INDEF(NODE)     ((NODE)->in_def)
#define SETS_USE(NODE)       ((NODE)->use)
#define SETS_INUSE(NODE)     ((NODE)->in_use)
#define SETS_OUTDEF(NODE)    ((NODE)->out_def)
#define SETS_OUTUSE(NODE)    ((NODE)->out_use)
#define SETS_ARRAYEF(NODE)   ((NODE)->arefl)

#define SETS_REFL_SYMB(NODE) ((NODE)->id)
#define SETS_REFL_NEXT(NODE) ((NODE)->next)
#define SETS_REFL_NODE(NODE) ((NODE)->node)
#define SETS_REFL_REF(NODE) ((NODE)->node->refer)
#define SETS_REFL_STMT(NODE) ((NODE)->node->stmt)

/************** For HASH NODE     ********************************/
#define HASH_IDENT(NODE)  ((NODE)->ident)

/************** For Special malloc ********************************/

 
/* pour la gestion memoire */
struct chaining
{
  char *zone;
  struct chaining *list;
};
 
typedef struct chaining *ptchaining;
struct stack_chaining
{
   ptchaining first;
   ptchaining last;
   struct stack_chaining *prev;
   struct stack_chaining *next;
   int level;
};
typedef struct stack_chaining *ptstack_chaining;
