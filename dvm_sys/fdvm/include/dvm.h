#pragma once
#include "user.h"
#include "aks_structs.h"

#define MAXTAGS 1000
#include "dvm_tag.h"

#define FORTRAN_LANG 0
#define C_LANG 1

#define RTS1  1
#define RTS2  2

#define MAX_DIMS 15  // 15 - Fortran 2008, 7 - Fortran 95, Fortran 2003
#define MAX_FUN_F90 20
#define ASYNCID_NUMB 8000
#define MAX_RED_VAR_SIZE 20
#define MAX_BLOCKS 65535
#define NO_ERR_MSG 0
#define WITH_ERR_MSG 1
#define SG_FILE_ATTR 99999

/*ACC*/
#define Ndev 3
enum{NON,HOST,CUDA};

#define HOST_DEVICE 1
#define CUDA_DEVICE 2

#define Nregim 3

enum {_ZERO_,REGION_ASYNC,REGION_COMPARE_DEBUG};

const int Ntp = 13;   //number of dvm-base types (RTS1)

#include "libnum.h"

#ifdef IN_DVM_
#define EXTERN
#else
#define EXTERN extern
#endif

struct distribute_list {
       distribute_list *next;
       SgStatement *stdis;
};
struct stmt_list {
       stmt_list *next;
       SgStatement *st;
};
struct dist_symb_list {
       dist_symb_list *next;
       SgSymbol *symb;
};
struct align {
       SgSymbol * symb;
       align * next;
       align * alignees;
       SgStatement * align_stmt;
};
struct mod_attr{
       SgSymbol *symb;
       SgSymbol *symb_list;
};
struct algn_attr {
       int type;
       align *ref;
};
struct rem_var {
       int index;
       int amv;
       int ncolon;
       SgSymbol *buffer;  /*ACC*/
};
struct rem_acc {
       SgExpression *rml;
       SgStatement *rmout;
       int rmbuf_use[Ntp];
       rem_acc *next;
};
struct group_name_list {
       group_name_list *next;
       SgSymbol *symb;
};
struct symb_list {
       symb_list *next;
       SgSymbol *symb;
};

struct base_list {
       base_list *next;
       SgSymbol *type_symbol;
       SgSymbol *base_symbol;
       SgSymbol *gpu_symbol;
};
struct D_do_list {
       D_do_list *next;
       int No;
       int num_line;
       SgLabel *end_lab;
       SgSymbol *do_var;
};
struct interval_list {
       interval_list *prev;
       int No;
       SgStatement *begin_st;
};
struct D_fragment {
       D_fragment *next;
       int No;
};

struct fragment_list {
       int No;
       SgStatement *begin_st;
       int dlevel;
       int elevel;
       int dlevel_spec;
       int elevel_spec;
       fragment_list *next;
};
struct fragment_list_in {
       int N1;
       int N2;
       int level;
       fragment_list_in *next;
};
struct reduction_list {
       reduction_list *next;
       int red_op;
       SgExpression *red_var;
       int ind;
};
struct IND_ref_list {
       IND_ref_list *next;
       SgExpression *rmref;
       SgExpression *axis[7];
       SgExpression *coef[7];
       SgExpression *cons[7];
       int nc;
       int ind;
};

struct coeffs {
       SgSymbol *sc[MAX_DIMS+2];
       int use;
};

struct heap_pointer_list {
       heap_pointer_list *next;
       SgSymbol *symb_heap;
       SgSymbol *symb_p;
};

struct filename_list {
       filename_list *next;
       char *name;
       SgSymbol *fns;
};
struct region_list {
       int is_data;
       int No;
       SgStatement *region_dir;
       SgStatement *cur_do_dir;
       int Lnums;
       int targets;
       region_list *next;
};  /*ACC*/

struct reduction_operation_list {
       SgSymbol *redvar;
       SgSymbol *locvar;
       SgExpression *formal_arg;
       SgExpression *actual_arg;
       SgExpression *value_arg;
       SgExpression *dimSize_arg;
       SgExpression *lowBound_arg;
       SgSymbol *red_grid;
       SgSymbol *loc_grid;
       SgSymbol *red_init;
       SgSymbol *red_host;
      /* SgSymbol *red_offset;
       SgSymbol *loc_offset;
       SgSymbol *red_base;
       SgSymbol *loc_base;
      */
       int number;
       int redvar_size;
       int array_red_size;
       reduction_operation_list *next;
};  /*ACC*/

struct use_mode {
       use_mode *next;
       int in;
       int out;
       int mark;
}; /*ACC*/

struct local_part_list {
       SgSymbol *dvm_array;
       SgSymbol *local_part;
       local_part_list *next;
}; /*ACC*/

const int ROOT = 1;
const int NODE = 2;
const int ALIGN_TREE      = 1000;
const int ARRAY_HEADER    = 1001;
//const int SHADOW_GROUP_IND = 1002;
//const int RED_GROUP_DIR = 1003;
const int SHADOW_WIDTH    = 1004;
const int REMOTE_VARIABLE = 1005;
const int POINTER_        = 1006;
const int LOOP_NUMBER     = 1007;
const int LOOP_INTERVAL_NUMBER = 1008;
const int ALIGN_RULE      = 1009;
const int LOC_ARR         = 1010;
const int DO_VARIABLE_USE = 1011;
const int INDIRECT_SUBSCRIPT   = 1012;
const int BUFFER_COUNT    = 1013;
const int HEAP_INDEX      = 1014;
const int ARRAY_COEF      = 1015;
const int RED_GROUP_VAR   = 1016;
const int TASK_INDEX      = 1017;
const int CONSISTENT_ARRAY_HEADER = 1018;
const int ARRAY_BASE      = 1019;
const int INDEX_DELTA     = 1020;
const int INIT_LOOP       = 1021;
const int MODULE_STR      = 1022;
const int DISTRIBUTE_     = 1023;
const int TSK_SYMBOL      = 1024;
const int DEBUG_AR_INDEX  = 1025;
const int DEBUG_GOTO      = 1026;
const int GPU_HEADER        = 1027; /*ACC*/
const int REDVAR_INDEX      = 1028; /*ACC*/
const int INTENT_OF_VAR     = 1029; /*ACC*/
const int STATEMENT_GROUP   = 1030; /*ACC*/
const int REPLICATED_ARRAY  = 1031; /*ACC*/
const int DUMMY_ARRAY       = 1032; /*ACC*/
const int INSERTED_STATEMENT= 1033; /*ACC*/
const int INDEX_LIST        = 1034; /*ACC*/
const int ACROSS_GROUP_IND  = 1035; /*ACC*/
const int DUMMY_ARGUMENT    = 1036; /*ACC*/
const int TSK_IND_VAR       = 1037;
const int TSK_RENUM_ARRAY   = 1038;
const int TSK_LPS_ARRAY     = 1039;
const int TSK_HPS_ARRAY     = 1040;
const int TSK_AUTO          = 1041;
const int GRAPH_NODE        = 1042;
const int LAST_STATEMENT    = 1043; 
const int RTC_NOT_REPLACE   = 1044; /*ACC*/
const int RTC_CALLS         = 1045; /*ACC*/
const int RTS2_CREATED      = 1046; /*RTS2*/
const int HANDLER_HEADER    = 1047; /*ACC*/
const int MODULE_USE        = 1048; /*ACC*/
const int DEFERRED_SHAPE    = 1049; 
const int END_OF_USE_LIST   = 1050; /*ACC*/
const int ROUTINE_ATTR      = 1051; /*ACC*/

const int MAX_LOOP_LEVEL = 10; // 7 - maximal number of loops in parallel loop nest 
const int MAX_LOOP_NEST = 25;  // maximal number of nested loops
const int MAX_FILE_NUM = 100;  // maximal number of file reference in procedure
const int SIZE_IO_BUF = 262144; //4185600;   // IO buffer size in elements
const int ANTIDEP = 0;
const int FLOWDEP = 1;
#define FICT_INT  2000000000            /* -2147483648  0x7FFFFFFFL*/

//enum{ Integer, Real, Double, Complex, Logical, DoubleComplex};
enum {UNIT_,FMT_,REC_,ERR_,IOSTAT_,END_,NML_,EOR_,SIZE_,ADVANCE_,POS_,IOMSG_, NUM__R};
enum {U_,FILE_,STATUS_,ER_,IOST_,ACCESS_,FORM_,RECL_,BLANK_,EXIST_,
OPENED_,NUMBER_,NAMED_,NAME_,SEQUENTIAL_,DIRECT_,NEXTREC_,FORMATTED_,
UNFORMATTED_,POSITION_,ACTION_,READWRITE_,READ_,WRITE_,DELIM_,PAD_,CONVERT_, NUM__O};

enum {SIZE,LBOUND,UBOUND,LEN,CHAR,KIND,F_INT,F_REAL,F_CHAR,F_LOGICAL,F_CMPLX,MAX_,MIN_,IAND_,IOR_,ALLOCATED_,ASSOCIATED_}; //intrinsic functions of Fortran 90

enum {NEW_,REDUCTION_,SHADOW_RENEW_,SHADOW_START_,SHADOW_WAIT_,SHADOW_COMPUTE_,REMOTE_ACCESS_,CONSISTENT_,STAGE_,PRIVATE_,CUDA_BLOCK_,ACROSS_,TIE_}; //clauses of PARALLEL directive


const int Integer   = 0;
const int Real      = 1;
const int Double    = 2;
const int Complex   = 3;
const int Logical   = 4;
const int DComplex  = 5;
const int Character = 6;
const int Integer_1 = 7;
const int Integer_2 = 8;
const int Integer_8 = 9;
const int Logical_1 = 10;
const int Logical_2 = 11;
const int Logical_8 = 12;

#define HEADER(A)  ((int*)(ORIGINAL_SYMBOL(A))->attributeValue(0,ARRAY_HEADER))
#define INDEX(A) (*((int*)(ORIGINAL_SYMBOL(A))->attributeValue(0,ARRAY_HEADER)))
#define DVM000(N)   (new SgArrayRefExp(*dvmbuf, *new SgValueExp(N)))  
#define SH_GROUP(S)  (*((int *) (S) -> attributeValue(0, SHADOW_GROUP_IND)))
#define RED_GROUP(S)  (*((int *) (S) -> attributeValue(0, RED_GROUP_IND)))
#define SHADOW_(A) ((SgExpression **)(ORIGINAL_SYMBOL(A))->attributeValue(0,SHADOW_WIDTH))
#define POINTER_DIR(A) ((SgStatement **)(ORIGINAL_SYMBOL(A))->attributeValue(0,POINTER_))
#define DISTRIBUTE_DIRECTIVE(A) ((SgStatement **)(ORIGINAL_SYMBOL(A))->attributeValue(0,DISTRIBUTE_))
#define ARRAY_BASE_SYMBOL(A) ((SgSymbol **)(ORIGINAL_SYMBOL(A))->attributeValue(0,ARRAY_BASE))
#define INDEX_SYMBOL(A) ((SgSymbol **)(A)->attributeValue(0,INDEX_DELTA))
#define INIT_LOOP_VAR(A) ((SgSymbol **)(A)->attributeValue(0,INIT_LOOP))
#define CONSISTENT_HEADER(A) (*((SgSymbol **)(ORIGINAL_SYMBOL(A))->attributeValue(0,CONSISTENT_ARRAY_HEADER)))
#define POINTER_INDEX(A) (*((int *)(A)->attributeValue(0,HEAP_INDEX)))
#define BUFFER_INDEX(A) (*((int*)(ORIGINAL_SYMBOL(A))->attributeValue(0,BUFFER_COUNT)))
#define BUFFER_COUNT_PLUS_1(A) (*((int*)(ORIGINAL_SYMBOL(A))->attributeValue(0,BUFFER_COUNT))) =  (*((int*)(ORIGINAL_SYMBOL(A))->attributeValue(0,BUFFER_COUNT)))+1;
#define PS_INDEX(A) (*((int *)(A)->attributeValue(0,TASK_INDEX)))
#define DEBUG_INDEX(A) (*((int*)(ORIGINAL_SYMBOL(A))->attributeValue(0,DEBUG_AR_INDEX)))
#define TASK_SYMBOL(A) (*((SgSymbol **)(ORIGINAL_SYMBOL(A))->attributeValue(0,TSK_SYMBOL)))
#define TASK_IND_VAR(A) (*((SgSymbol **)(ORIGINAL_SYMBOL(A))->attributeValue(0,TSK_IND_VAR)))
#define TASK_RENUM_ARRAY(A) (*((SgSymbol **)(ORIGINAL_SYMBOL(A))->attributeValue(0,TSK_RENUM_ARRAY)))
#define TASK_LPS_ARRAY(A) (*((SgSymbol **)(ORIGINAL_SYMBOL(A))->attributeValue(0,TSK_LPS_ARRAY)))
#define TASK_HPS_ARRAY(A) (*((SgSymbol **)(ORIGINAL_SYMBOL(A))->attributeValue(0,TSK_HPS_ARRAY)))
#define TASK_AUTO(A) ((A)->attributeValue(0,TSK_AUTO))
#define RTS2_OBJECT(A) ((A)->attributeValue(0,RTS2_CREATED))
#define IS_LIST_END(A) ((A)->attributeValue(0,END_OF_USE_LIST))
/*#define AR_COEFFICIENTS(A)  ( (A)->attributeValue(0,ARRAY_COEF) ?  (coeffs *) (A)->attributeValue(0,ARRAY_COEF) : (coeffs *) (ORIGINAL_SYMBOL(A))->attributeValue(0,ARRAY_COEF))*/
#define AR_COEFFICIENTS(A)  (DvmArrayCoefficients(A))
#define MAX_DVM   maxdvm = (maxdvm < ndvm) ? ndvm-1 : maxdvm  
#define FREE_DVM(A)  maxdvm = (maxdvm < ndvm) ? ndvm-1 : maxdvm;  ndvm-=A  
#define SET_DVM(A)   maxdvm = (maxdvm < ndvm) ? ndvm-1 : maxdvm;  ndvm=A  
#define FREE_HPF(A)  maxhpf = (maxhpf < nhpf) ? nhpf-1 : maxhpf;  nhpf-=A  
#define SET_HPF(A)   maxhpf = (maxhpf < nhpf) ? nhpf-1 : maxhpf;  nhpf=A  
#define HPF000(N)   (new SgArrayRefExp(*hpfbuf, *new SgValueExp(N)))  
#define IS_DUMMY(A)  ((A)->thesymb->entry.var_decl.local == IO) 
#define IS_TEMPLATE(A)  ((A)->attributes() & TEMPLATE_BIT) 
#define IN_COMMON(A)  ((A)->attributes() & COMMON_BIT) 
#define IN_DATA(A)  ((A)->attributes() & DATA_BIT) 
#define IN_EQUIVALENCE(A)  ((A)->attributes() & EQUIVALENCE_BIT) 
#define IS_ARRAY(A)  ((A)->attributes() & DIMENSION_BIT)
#define IS_ALLOCATABLE(A)  ((A)->attributes() & ALLOCATABLE_BIT) 
#define IS_ALLOCATABLE_POINTER(A)  (((A)->attributes() & ALLOCATABLE_BIT) || ((A)->attributes() & POINTER_BIT))
#define IS_POINTER_F90(A)  ((A)->attributes() & POINTER_BIT) 
#define CURRENT_SCOPE(A)  (((A)->scope() == cur_func) && ((A)->thesymb->entry.var_decl.local != BY_USE) )
#define IS_BY_USE(A)  ((A)->thesymb->entry.Template.base_name != 0) 
/*#define ORIGINAL_SYMBOL(A) (OriginalSymbol(A)) */
#define ORIGINAL_SYMBOL(A) (IS_BY_USE(A) ? (A)->moduleSymbol() : (A))
#define IS_SAVE(A) (((A)->attributes() & SAVE_BIT) || (saveall && !IS_TEMPLATE(A) && !IN_COMMON(A) && !IS_DUMMY(A)) )
#define HAS_SAVE_ATTR(A) (((A)->attributes() & SAVE_BIT ) || saveall )
#define IS_POINTER(A)  ((A)->attributes() & DVM_POINTER_BIT) 
#define IS_SH_GROUP_NAME(A) ((A)->variant() == SHADOW_GROUP_NAME)
#define IS_RED_GROUP_NAME(A) ((A)->variant() == REDUCTION_GROUP_NAME)
#define IS_GROUP_NAME(A) (((A)->variant() == SHADOW_GROUP_NAME) || ((A)->variant() == REDUCTION_GROUP_NAME) || ((A)->variant() == REF_GROUP_NAME)) 
#define IS_DVM_ARRAY(A)   (((A)->attributes() & DISTRIBUTE_BIT) || ((A)->attributes() & ALIGN_BIT) || ((A)->attributes() & INHERIT_BIT))
#define IS_DISTR_ARRAY(A) (((A)->attributes() & DISTRIBUTE_BIT) || ((A)->attributes() & ALIGN_BIT) || ((A)->attributes() & INHERIT_BIT))
#define IN_MODULE (cur_func->variant() == MODULE_STMT)
#define IN_MAIN_PROGRAM (cur_func->variant() == PROG_HEDR)
#define DVM_PROC_IN_MODULE(A) ((mod_attr *)(A)->attributeValue(0,MODULE_STR))
/*
#define LINE_NUMBER_BEFORE(ST,WHERE)     doAssignStmtBefore(new SgValueExp((ST)->lineNumber()),WHERE); ndvm--; InsertNewStatementBefore(D_Lnumb(ndvm),WHERE)
#define LINE_NUMBER_STL_BEFORE(STL,ST,WHERE)     doAssignStmtBefore(new SgValueExp((ST)->lineNumber()),WHERE); ndvm--; InsertNewStatementBefore(STL=D_Lnumb(ndvm),WHERE)
#define LINE_NUMBER_AFTER(ST,WHERE)   InsertNewStatementAfter (D_Lnumb(ndvm),WHERE,(WHERE)->controlParent());  doAssignStmtBefore(new SgValueExp((ST)->lineNumber()),cur_st); ndvm--
#define LINE_NUMBER_N_AFTER(N,WHERE,CP)  InsertNewStatementAfter(D_Lnumb(ndvm),WHERE,CP);  doAssignStmtBefore(new SgValueExp(N),cur_st); ndvm--
*/
/*
#define LINE_NUMBER_BEFORE(ST,WHERE)     doAssignStmtBefore(new SgValueExp((ST)->lineNumber()),WHERE); ndvm--; InsertNewStatementBefore((many_files ? D_FileLine(ndvm,ST) : D_Lnumb(ndvm)) ,WHERE)
#define LINE_NUMBER_STL_BEFORE(STL,ST,WHERE)     doAssignStmtBefore(new SgValueExp((ST)->lineNumber()),WHERE); ndvm--; InsertNewStatementBefore(STL= (many_files ? D_FileLine(ndvm,ST) :  D_Lnumb(ndvm)),WHERE)
#define LINE_NUMBER_AFTER(ST,WHERE)   InsertNewStatementAfter ((many_files ? D_FileLine(ndvm,ST) : D_Lnumb(ndvm)),WHERE,(WHERE)->controlParent());  doAssignStmtBefore(new SgValueExp((ST)->lineNumber()),cur_st); ndvm--
#define LINE_NUMBER_N_AFTER(N,WHERE,CP)  InsertNewStatementAfter((many_files ? D_FileLine(ndvm,CP): D_Lnumb(ndvm)),WHERE,CP);  doAssignStmtBefore(new SgValueExp(N),cur_st); ndvm--
*/
/*
#define LINE_NUMBER_BEFORE(ST,WHERE)  InsertNewStatementBefore(D_FileLineConst((ST)->lineNumber(),ST),WHERE)
#define LINE_NUMBER_STL_BEFORE(STL,ST,WHERE)  InsertNewStatementBefore(STL= D_FileLineConst((ST)->lineNumber(),ST),WHERE)
#define LINE_NUMBER_AFTER(ST,WHERE)             InsertNewStatementAfter (D_FileLineConst((ST)->lineNumber(),ST),WHERE,(WHERE)->controlParent())
#define LINE_NUMBER_AFTER_WITH_CP(ST,WHERE,CP)  InsertNewStatementAfter (D_FileLineConst((ST)->lineNumber(),ST),WHERE,CP)
#define LINE_NUMBER_N_AFTER(N,WHERE,CP)  InsertNewStatementAfter(D_FileLineConst(N,CP),WHERE,CP)
*/
#define LINE_NUMBER_BEFORE(ST,WHERE)  InsertNewStatementBefore(Dvmh_Line((ST)->lineNumber(),ST),WHERE)
#define LINE_NUMBER_STL_BEFORE(STL,ST,WHERE)  InsertNewStatementBefore(STL= Dvmh_Line((ST)->lineNumber(),ST),WHERE)
#define LINE_NUMBER_AFTER(ST,WHERE)             InsertNewStatementAfter (Dvmh_Line((ST)->lineNumber(),ST),WHERE,(WHERE)->controlParent())
#define LINE_NUMBER_AFTER_WITH_CP(ST,WHERE,CP)  InsertNewStatementAfter (Dvmh_Line((ST)->lineNumber(),ST),WHERE,CP)
#define LINE_NUMBER_N_AFTER(N,WHERE,CP)  InsertNewStatementAfter(Dvmh_Line(N,CP),WHERE,CP)

#define LINE_NUMBER_NEXP_AFTER(NE,WHERE,CP)  InsertNewStatementAfter(D_DummyFileLine(ndvm,"dvm_check"),WHERE,CP);  doAssignStmtBefore((NE),cur_st); ndvm--
#define ALIGN_RULE_INDEX(A) ((int*)(A)->attributeValue(0,ALIGN_RULE))
#define INTERVAL_LINE  (St_frag->begin_st->lineNumber())
#define INTERVAL_NUMBER  (St_frag->No)
#define GROUP_REF(S,I) (new SgArrayRefExp(*(S),*new SgValueExp(I)))
#define IS_DO_VARIABLE_USE(E) ((SgExpression **)(E)->attributeValue(0,DO_VARIABLE_USE))
#define HEADER_SIZE(A) (2+(maxbuf+1)*2*(Rank(A)+1))
#define HSIZE(R) (2*R + 2)
#define ARRAY_ELEMENT(A,I)  (new SgArrayRefExp(*A, *new SgValueExp(I)))
#define INTEGER_VALUE(E,C) ((E)->variant() == INT_VAL && (E)->valueInteger() == (C))
#define IS_INTRINSIC_TYPE(T) (!TYPE_RANGES((T)->thetype) && !TYPE_KIND_LEN((T)->thetype) && ((T)->variant() != T_DERIVED_TYPE))
#define DEBUG_STMTS_FOR_GOTO(ST) ((SgStatement **)(ST)->attributeValue(0,DEBUG_GOTO))
/* ACC */
#define HEADER_FOR_GPU(A)  ((SgSymbol**)(A)->attributeValue(0,GPU_HEADER))
#define FREE_GPU(A)  maxgpu = (maxgpu < ngpu) ? ngpu-1 : maxgpu;  ngpu-=A  
#define SET_GPU(A)   maxgpu = (maxgpu < ngpu) ? ngpu-1 : maxgpu;  ngpu=A  
#define GPU000(N)   (new SgArrayRefExp(*gpubuf, *new SgValueExp(N)))  
#define IN_COMPUTE_REGION  (cur_region && !cur_region->is_data && !in_checksection)
#define BY_HANDLER  (ACC_program)
#define IND_REDVAR(A) (*((int *)(A)->attributeValue(0,REDVAR_INDEX)))
#define SET_NRED(A)   maxred_gpu = (maxred_gpu < nred_gpu) ? nred_gpu-1 : maxred_gpu;  nred_gpu=A
#define DECL(A)  ((A)->thesymb->decl)
#define VAR_INTENT(A) (((int *)(A)->attributeValue(0,INTENT_OF_VAR)))
#define IN_STATEMENT_GROUP(A) ((A)->attributeValue(0,STATEMENT_GROUP))
#define HEADER_OF_REPLICATED(A) (((int *)(A)->attributeValue(0,REPLICATED_ARRAY)))
#define DUMMY_FOR_ARRAY(A) ( (SgSymbol **)(A)->attributeValue(0,DUMMY_ARRAY) )
#define DUMMY_ARG(A)       ( (SgSymbol **)(A)->attributeValue(0,DUMMY_ARGUMENT) )
#define IS_CONSISTENT(A)  ((A)->attributes() & CONSISTENT_BIT)
#define IS_INSERTED(A) ((A)->attributeValue(0,INSERTED_STATEMENT))
#define VECTOR_REF(V,N)   (new SgArrayRefExp(*V, *new SgValueExp(N)))  
#define INDEX_VECTOR(A) ( (SgSymbol **)(A)->attributeValue(0,INDEX_LIST) )
#define GROUP_INDEX(A) (((int *)(A)->attributeValue(0,ACROSS_GROUP_IND)))
#define GRAPHNODE(A) (*((graph_node **)(A)->attributeValue(0,GRAPH_NODE)))
#define ATTR_NODE(A) ((graph_node **)(A)->attributeValue(0,GRAPH_NODE))
#define HEDR(A)  ((A)->thesymb->entry.Template.func_hedr)
#define FILE_LAST_STATEMENT(ST) ((SgStatement **)(ST)->attributeValue(0,LAST_STATEMENT))
#define CALLED_FUNCTIONS(ST) ((symb_list **)(ST)->attributeValue(0,RTC_CALLS)) 
#define INTERFACE_RTS2  (parloop_by_handler == 2)
#define HEADER_FOR_HANDLER(A)  ( (SgSymbol **)(A)->attributeValue(0,HANDLER_HEADER) )
#define USE_STATEMENTS_ARE_REQUIRED ( (int *) first_do_par->attributeValue(0,MODULE_USE) )
#define DEFERRED_SHAPE_TEMPLATE(A) ( (ORIGINAL_SYMBOL(A))->attributeValue(0,DEFERRED_SHAPE) )
#define HAS_ROUTINE_ATTR(A) ((A)->attributeValue(0,ROUTINE_ATTR))

EXTERN
SgFunctionSymb * fdvm [MAX_LIBFUN_NUM];
EXTERN
SgFunctionSymb * f90 [MAX_FUN_F90];
EXTERN
const char * name_dvm [MAX_LIBFUN_NUM];
EXTERN
short fmask [MAX_LIBFUN_NUM];
EXTERN
SgVariableSymb * dvmbuf,
               * hpfbuf,
               * Imem,
               * Rmem,
               * Dmem, 
               * Lmem,
               * Cmem,
               * Chmem,
               * DCmem,
               * heapdvm,
               * Pipe;

/*ACC*/
EXTERN
SgVariableSymb * gpubuf,
               * Imem_gpu,
               * Rmem_gpu,
               * Dmem_gpu, 
               * Lmem_gpu,
               * Cmem_gpu,
               * Chmem_gpu,
               * DCmem_gpu;  /*ACC*/

         
EXTERN SgConstantSymb *Iconst[10];
EXTERN const char *tag[MAXTAGS];
EXTERN int max_lab;  // maximal  label in file
EXTERN int ndvm;  // index for buffer array 'dvm000'	
EXTERN int maxdvm;  //  size of array 'dvm000' 
EXTERN int loc_distr;
EXTERN int send;  //set to 1 if I/O statement require 'send' operation
EXTERN char *fin_name; //input file name
EXTERN SgStatement *cur_st;  // current statement  (for inserting)
EXTERN SgFile *current_file;    //current file
EXTERN SgStatement *where;//used in doAssignStmt: new statement is inserted before 'where' statement
EXTERN int nio;
EXTERN SgSymbol *bufIO[Ntp],*IOstat;
EXTERN int buf_use[Ntp];
EXTERN SgSymbol *loop_var[MAX_DIMS+1]; // for generatig DO statements
EXTERN SgStatement *cur_func;  // current function 
EXTERN int errcnt;  // counter of errors in file
EXTERN int saveall; //= 1 if there is SAVE without name-list in current function(procedure) 
EXTERN SgStatement *par_do;  // first DO statement of current parallel loop  
EXTERN int iplp; //dvm000 element number for storing ParLoopRef
EXTERN int irg;  //dvm000 element number for storing RedGroupRef
EXTERN int irgts;  //dvm000 element number for storing RedGroupRef(task_region)
EXTERN int idebrg;  //dvm000 element number for storing DebRedGroupRef
EXTERN SgExpression *redgref; // reduction group reference
EXTERN SgExpression *redgrefts; // reduction group reference for TASK_REGION
EXTERN SgExpression *debredgref; // debug reduction group reference
EXTERN SgExpression *red_list; // reduction operation list in FDVM program
EXTERN SgExpression *task_red_list; // reduction operation list (in TASK_REGION directive)
EXTERN rem_acc *rma; // remote_access directive/clause list
EXTERN int iconsg;  //dvm000 element number for storing ConsistGroupRef
EXTERN int iconsgts;  //dvm000 element number for storing ConsistGroupRef(task_region)
EXTERN int idebcg;  //dvm000 element number for storing DebRedGroupRef
EXTERN SgExpression *consgref; // consistent group reference
EXTERN SgExpression *consgrefts; // consistent group reference for TASK_REGION
EXTERN SgExpression *debconsgref; // debug reduction(consistent) group reference
EXTERN SgExpression *cons_list; // consistent array list in FDVM program
EXTERN SgExpression *task_cons_list; // consistent array list (in TASK_REGION directive)

EXTERN SgLabel *end_lab, *begin_lab; //labels for parallel loop nest
EXTERN D_do_list *cur_do;
EXTERN D_do_list *free_list;
EXTERN int Dloop_No;
EXTERN int pardo_No;
EXTERN int taskreg_No;
EXTERN int pardo_line;
EXTERN int D_end_do;
EXTERN int nfrag ; //counter of intervals for performance analizer 
EXTERN interval_list *St_frag ;
EXTERN interval_list *St_loop_first;
EXTERN interval_list *St_loop_last;
EXTERN int perf_analysis ; //set to 1 by -e1
EXTERN int close_loop_interval;
EXTERN stmt_list *goto_list, *acc_return_list;
EXTERN int len_int; //set by option -bind
EXTERN int len_DvmType;//set by option -t and -bind
EXTERN int bind;//set by option -bind
/*EXTERN D_fragment *deb[5]; //set by option -d and -e*/
EXTERN int dvm_debug ;   //set to 1 by -d1  or -d2 or -d3 or -d4 flag
EXTERN int only_debug ;  //set to 1 by -s flag
EXTERN int level_debug ; //set to 1 by -d1, to 2 by -d2, ...
EXTERN fragment_list_in *debug_fragment; //set by option -d
EXTERN fragment_list_in *perf_fragment; //set by option -e
EXTERN int debug_regim; //set by option -d
EXTERN int check_regim; //set by option -dc
EXTERN int dbg_if_regim; //set by option -dbif
EXTERN int deb_mpi; //set by option -dmpi
EXTERN int d_no_index; //set by option -dnoind
EXTERN  int IOBufSize; //set by option -bufio
EXTERN  int UnparserBufSize; //set by option -bufUnparser
EXTERN  int collapse_loop_count; //set by option -collapse
EXTERN SgSymbol *dbg_var;
EXTERN int HPF_program;
EXTERN int ACC_program;
EXTERN int rmbuf_size[Ntp];
EXTERN int first_time;
EXTERN SgStatement *indep_st; //first INDEPENDENT directive of loop nest
EXTERN SgStatement *ins_st1, *ins_st2; // for INDEPENDENT loop
EXTERN SgSymbol *DoVar[MAX_LOOP_NEST], **IND_var, **IEX_var;
EXTERN int iarg; // for INDEPENDENT loop
EXTERN int nIND, nIEX; //the number of INDEPENDENT loops and enclosing loops;
EXTERN SgExpression *IND_target;
EXTERN SgExpression *IND_target_R;
EXTERN IND_ref_list *IND_refs;
EXTERN reduction_list *redl; // reduction operation list in HPF program
EXTERN int nhpf;    // index for work array 'hpf000'	
EXTERN int maxhpf;  //  size of array 'hpf000' 
EXTERN int all_sh_width, no_rma, one_inquiry, only_local; // //set by option -Hflag
EXTERN SgStatement *first_hpf_exec;
EXTERN int hpf_ind;
EXTERN symb_list *dvm_ar,*registration;
EXTERN int many_files;
EXTERN int seq_loop_nest;
EXTERN int pipeline;
EXTERN symb_list *imp_loop; //input element list, that are appear in implicit loop
EXTERN  heap_pointer_list *heap_point;
EXTERN  filename_list *fnlist;
EXTERN  int filename_num;
EXTERN  SgFunctionSymb *SIZE_function;
EXTERN  int in_interface; //inside the interface block > 0
EXTERN int inparloop;    //set to 1 in the range of parallel loop
EXTERN SgSymbol *registration_array;
EXTERN int count_reg;
EXTERN SgSymbol *check_sum;
EXTERN  int default_integer_size,default_real_size ;
EXTERN  int all_replicated;  //all arrays in procedure are replicated
EXTERN symb_list *if_goto;
EXTERN int nifvar;
EXTERN stmt_list *entry_list;
EXTERN SgExpression *dbif_cond, *dbif_not_cond;
EXTERN SgStatement *st_adr, *st_dstv;
EXTERN base_list *mem_use_structure;
EXTERN symb_list *sym_gpu; /* list of dvm_array with gpu-copies */ /*ACC*/
EXTERN region_list *cur_region;    //current region in ACC program /*ACC*/ 
EXTERN int gpu_mem_use[Ntp];                                       /*ACC*/
//EXTERN SgSymbol *mem_symb[6],*gpu_mem_symb[6], *k_mem_symb[6];   /*ACC*/
EXTERN int ngpu;    // index for work array 'gpu000'               /*ACC*/
EXTERN int nkernel; // counter of kernels                          /*ACC*/	
EXTERN int maxgpu;  //  size of work  array 'gpu000'               /*ACC*/ 
//EXTERN SgSymbol *s_blocks,*s_threads,*s_blocks_off;              /*ACC*/
EXTERN SgSymbol *mod_gpu_symb,*loop_ref_symb;                      /*ACC*/
EXTERN SgSymbol*index_array_symb;                                  /*ACC*/
EXTERN SgStatement *if_gpu,*mod_gpu,*created_unit;                 /*ACC*/
//EXTERN char *fname_gpu[80];                                      /*ACC*/
EXTERN symb_list *acc_array_list;                                  /*ACC*/
EXTERN int nred_gpu,maxred_gpu, for_kernel, for_host;              /*ACC*/
EXTERN int device_flag[Ndev];                                      /*ACC*/
EXTERN int region_debug; //set by option -dgpu ,-dreg              /*ACC*/
EXTERN int region_compare; //set by option -dgpu                   /*ACC*/
EXTERN SgConstantSymb *region_const[Nregim];                       /*ACC*/
EXTERN SgExpression *cuda_block;                                   /*ACC*/
EXTERN SgExpression *allocated_list;                               /*ACC*/
EXTERN SgStatement *first_do_par;                                  /*ACC*/
EXTERN int in_checksection,undefined_Tcuda, cuda_functions;        /*ACC*/
EXTERN symb_list *RGname_list;                                     /*ACC*/
EXTERN int parloop_by_handler; //set to 1 by option -Opl and       /*ACC*/
                               //    to 2 by option -Opl2
//---------------------------------------------------------------------
/*  dvm.cpp   */
void TranslateFileDVM(SgFile *f);
void TransFunc(SgStatement *func,SgStatement* &end_of_unit) ;
void InsertDebugStat(SgStatement *func,SgStatement* &end_of_unit) ;
void DVMFileUnparse(SgFile *f) ;
void DeclareVarDVM(SgStatement *lstat, SgStatement *lstat2);
void doDistFormat(SgExpression *e);
//void GenDistArray (SgSymbol *das, int idisars, SgExpression **distr_list, SgExpression *ps, SgStatement *stdis);
void GenDistArray (SgSymbol *das, int idisars, SgExpression *distr_rule_list, SgExpression *ps, SgStatement *stdis);
SgExpression * doSizeArray(SgSymbol *ar, SgStatement *st);
SgExpression * doSizeFunctionArray(SgSymbol *ar, SgStatement *st);
SgExpression *doSizeArrayD(SgSymbol *ar, SgStatement *st);
SgSymbol * baseMemory(SgType *t);
int TypeSize(SgType *t);
int CharLength(SgType *t);
void TypeMemory(SgType *t);
int TypeIndex(SgType *t);
int doDisRuleArrays (SgStatement *stdist, int aster, SgExpression **distr_list);
SgExpression *doDisRules(SgStatement *stdis, int aster, int &idis);
void Align_Tree(align *root);
void AlignTree( align *root);
void GenAlignArray(align *node, align *root, int nr, SgExpression *align_rule_list, int iaxis);
void doAlignRule_1 (int rank);
int doAlignRule (SgSymbol *alignee,SgStatement *algn_st, int iaxis);
SgExpression *doAlignRules (SgSymbol *alignee, SgStatement *algn_st, int iaxis, int &nt);
int AxisNumOfDummyInExpr (SgExpression *e, SgSymbol *dim_ident[], int ni,                             SgExpression **eref, int use[], SgStatement *st);
void CoeffConst(SgExpression *e, SgExpression *ei, SgExpression **pcoef, SgExpression **pcons);
//void RealignArray(SgSymbol *als, SgSymbol *tgs, int iaxis, int nr, int new_sign, SgStatement *stal);  
void RealignArray(SgSymbol *als, SgSymbol *tgs, int iaxis, int nr, SgExpression *align_rule_list, int new_sign, SgStatement *stal) ;
void ArrayHeader(SgSymbol *ar, int ind);
int Rank(SgSymbol *s);
SgExpression *dvm_array_ref();
SgExpression *dvm_ref(int n);
int DeleteDArFromList(SgStatement *stmt);
void ChangeArg_DistArrayRef(SgExpression *e);
void ChangeDistArrayRef(SgExpression *e);
void ChangeDistArrayRef_Left(SgExpression *e);
SgExpression *ReplaceParameter(SgExpression *e);
SgExpression *LinearForm (SgSymbol *ar, SgExpression *el);
SgExpression *LinearFormB (SgSymbol *ar, int ihead, int n, SgExpression *el);
SgExpression * head_ref (SgSymbol *ar, int n);
SgExpression * header_ref (SgSymbol *ar, int n);
SgExpression * header_rf (SgSymbol *ar, int ihead, int n);
SgExpression * HeaderRef (SgSymbol *ar);
SgExpression * HeaderRefInd(SgSymbol *ar, int n);
SgType* SgTypeComplex(SgFile *f);
SgType* SgTypeDoubleComplex(SgFile *f);
SgExpression *HeaderNplus1(SgSymbol * ar);   
SgExpression *BufferHeaderNplus1(SgExpression * rme, int n, int ihead,SgSymbol *ar); 
SgExpression *LowerBound(SgSymbol *ar, int i);
SgExpression *UpperBound(SgSymbol *ar, int i);
int ExpCompare(SgExpression *e1, SgExpression *e2);
int RedFuncNumber(SgExpression *kwe);
int RedFuncNumber_2(int num);
int VarType(SgSymbol *var);
void ShadowList (SgExpression *el, SgStatement *st,SgExpression *gref );
int doShadSizeArrayM1(SgSymbol *ar,SgExpression **shlist);
int doShadSizeArrays(SgExpression *shl, SgSymbol *ar, SgStatement *st, SgExpression **shlist);
int BoundSizeArrays(SgSymbol *das);
int NameIndex(SgType *type);
SgType *Base_Type(SgType *type);
int doAlignIteration(SgStatement *stat, SgExpression *aref);
int Alignment(SgStatement *stat, SgExpression *aref, SgExpression *axis[], SgExpression *coef[], SgExpression *cons[],int interface);
void doLoopStmt(SgStatement *st);
stmt_list  *addToStmtList(stmt_list *pstmt, SgStatement *stat);
stmt_list  *delFromStmtList(stmt_list *pstmt);
SgExpression * ArrayDimSize(SgSymbol *ar, int i);
void RedistributeArray(SgSymbol *das, int idisars, SgExpression *distr_rule_list, SgExpression *ps, int sign,SgExpression *dasref, SgStatement *stdis); 
SgExpression * DistObjectRef (SgSymbol *ar); 
SgStatement *doIfThenConstr(SgSymbol *ar);
SgExpression *Calculate(SgExpression *e);
int RemAccessRefCompare(SgExpression *e1, SgExpression *e2);
void ChangeRemAccRef(SgExpression *e, SgExpression *rve);
SgExpression *isRemAccessRef(SgExpression *e);
int CreateBufferArray(int rank, SgExpression *rme, int *amview, SgStatement *stmt);
void CopyToBuffer(int rank,  int ibuf, SgExpression *rme);
void RemoteVariableList(SgSymbol *group,SgExpression *rml, SgStatement *stmt);
void RemoteVariableList1(SgSymbol *group,SgExpression *rml, SgStatement *stmt);
void DeleteBuffers(SgExpression *rml);
void AddRemoteAccess(SgExpression *rml, SgStatement *rmout);
void DelRemoteAccess();
void RemoteAccessEnd();
SgExpression *isSpecialFormExp(SgExpression *e,int i,int ind,SgExpression *vpart[],SgSymbol *do_var[]);
int   isInvariantPart(SgExpression *e);
int   isDependentPart(SgExpression *e,SgSymbol *do_var[]);
SgExpression *RenewSpecExp(SgExpression *e, int cnst, int ind);
int   isDistObject(SgExpression *e);
SgExpression *Exprn(SgExpression *e);
int   isListOfArrays(SgExpression *e, SgStatement *st);
char * AttrName(int i);
int   TestShapeSpec(SgExpression *e);
void  AddToGroupNameList (SgSymbol *s);
symb_list *AddToSymbList( symb_list *ls, SgSymbol *s);
symb_list *AddNewToSymbList ( symb_list *ls, SgSymbol *s);
symb_list *AddNewToSymbListEnd ( symb_list *ls, SgSymbol *s);
symb_list *MergeSymbList(symb_list *ls1, symb_list *ls2);
symb_list *CopySymbList(symb_list *ls);
void  DistArrayRef(SgExpression *e, int modified, SgStatement *st);
void  GoRoundEntry(SgStatement *stmt);
void  BeginBlockForEntry(SgStatement *stmt);
int   isInSymbList(symb_list *ls,SgSymbol *s);
void  NewVarList(SgExpression *nl,SgStatement *stmt);
void  TestReverse(SgExpression *e,SgStatement *st);
void  TestShadowWidths(SgSymbol *ar, SgExpression * lbound[], SgExpression * ubound[], int nw, SgStatement *st);
int   PointerRank(SgSymbol *p);
SgType * PointerType(SgSymbol *p);
void  AssignPointer(SgStatement *ass);
void  AllocateArray(SgStatement *stmt, distribute_list *distr);
void  AllocateDistArray(SgSymbol *p, SgExpression *desc, SgStatement *stdis, SgStatement *stmt);
void  AlignTreeAlloc( align *root,SgStatement *stmt);
void  AllocateAlignArray(SgSymbol *p, SgExpression *desc, SgStatement *stmt);
void  AlignAllocArray(align *node, align *root, int nr, int iaxis,SgExpression *desc, SgStatement *stmt);
void  AddFirstSubscript(SgExpression *ea, SgExpression *ei);
SgExpression * PointerHeaderRef(SgExpression *pe, int ind);
void  CopyHeader(SgExpression *ple, SgExpression *pre, int rank);
int   TestArrayRef(SgExpression *e, SgStatement *stmt);
void  AddDistSymbList(SgSymbol *s);
void  initialize();
void  initVariantNames();
void  initLibNames();
void  initMask();
void  InitDVM( SgFile *f);
void  StoreLowerBoundsPlus(SgSymbol *ar, SgExpression *arref);
void  StoreLowerBoundsPlusFromAllocate(SgSymbol *ar,SgExpression *arref,SgExpression *lbound);
void  ReplaceLowerBound(SgSymbol *ar, int i);
void  ReplaceArrayBounds(SgSymbol *ar) ;
SgExpression *ConstRef(int ic);
SgExpression *SignConstRef(int ic);
int DVMTypeLength();
void ReconfPS( stmt_list *pstmt);
SgExpression *CountOfTasks(SgStatement *st);
void TestParamType(SgStatement *stmt);
SgExpression *CurrentPS ();
SgExpression *ParentPS ();
SgExpression *ReplaceFuncCall(SgExpression *e);
SgExpression *PSReference(SgStatement *st);
SgExpression *TaskPS(SgExpression *target,SgStatement *st);
SgExpression *hasOntoClause(SgStatement *stdis);
SgExpression *hasNewValueClause(SgStatement *stdis);
int RankOfSection(SgExpression *are);
void CreateTaskArray(SgSymbol *ts);
int LoopVarType(SgSymbol *var,SgStatement *st);
int LocVarType(SgSymbol *var,SgStatement *st);
void StartTask(SgStatement *stmt);
void InitGroups();
//void InsertRedVarsInGroup(SgExpression *redgref,int irv,int nred);
SgExpression * isDoVarUse (SgExpression *e, int use[], SgSymbol *ident[], int ni, int *num, SgStatement *st);
SgSymbol* isIndirectSubscript (SgExpression *e, SgSymbol *ident, SgStatement *st);
void IndirectList(SgSymbol *group, SgExpression *rml, SgStatement *stmt);
SgExpression *BufferHeader4(SgExpression * rme, int ihead);
void BeginDebugFragment(int num, SgStatement *stmt);
void EndDebugFragment();
void InitRemoteGroups();
int MinLevel(int level, int max, int is_max);
int MaxLevels(SgStatement *stmt,int *max_dlevel,int *max_elevel);
SgExpression *PointerArrElem(SgSymbol *p,SgStatement *stdis);
SgExpression *ReverseDim(SgExpression *desc,int rank);
SgExpression *DoSubscriptList(SgExpression *are,int ind);
SgExpression *doSizeArrayQuery(SgExpression *headref,int rank);
int DVMType();
int NumberOfElements(SgSymbol *ar, SgStatement *stmt, int err);
void InitHeap(SgSymbol *heap);
int isHPFprogram(char *filename);
SgExpression * HeapIndex(SgStatement *st);
SgExpression * LowerBoundOfDimension(SgArrayType *artype, int i);
int DeleteHeapFromList(SgStatement *stmt);
void InitAsyncid();
void AsyncCopyWait(SgExpression *asc);
void Triplet(SgExpression *e,SgSymbol *ar,int i, SgExpression *einit[],SgExpression *elast[],SgExpression *estep[]);
void AsynchronousCopy(SgStatement *stmt);
void CreateCoeffs(coeffs* scoef,SgSymbol *ar);
SgExpression * coef_ref (SgSymbol *ar, int n);
coeffs *DvmArrayCoefficients(SgSymbol *ar);
void DeleteShadowGroups(SgStatement *stmt);
void InitShadowGroups();
int doSectionIndex(SgExpression *esec, SgSymbol *ar, SgStatement *st, int idv[], int ileft, SgExpression *lrec[], SgExpression *rrec[]);
void ShadowSectionTriplet(SgExpression *e, int i, SgExpression *einit[], SgExpression *elast[], SgExpression *estep[], SgExpression *lrec[], SgExpression *rrec[],  int flag);
void AddToAligneeList(align *root, align* node);
void RegistrationList(SgStatement *stmt);
void  RegistrateArg(SgExpression *ele);
int TestType(SgType *type);
filename_list  *AddToFileNameList ( char *s);
filename_list  *AddToFileNameList(const char *s);
char* FileNameVar(int i);
void InitFileNameVariables();
char* RedGroupVarName(SgSymbol *gr);
void CreateRedGroupVars();
void InitRedGroupVariables();
void WaitDirList();
int DefineLoopNumberForDimension(SgStatement * stat,int loop_num[]);
SgExpression *DebReductionGroup(SgSymbol *gs);
void Reduction_Task_Region(SgStatement *stmt);
void EndReduction_Task_Region(SgStatement *stmt);
void DeleteLocTemplate(SgStatement *stmt);
void ShadowComp (SgExpression *ear, SgStatement *st, int ilh);
SgSymbol* CreateConsistentHeaderSymb(SgSymbol *ar);
void EndOfProgramUnit(SgStatement *stmt, SgStatement *func, int begin_block);
SgSymbol* BaseSymbol(SgSymbol *ar);
void InitBaseCoeffs();
void CreateIndexVariables(SgExpression *dol);
SgSymbol* IndexSymbol(SgSymbol *si);
void  doAssignIndexVar(SgExpression *dol,int iout, SgExpression *init[]);
void ChangeIndexRefBySum(SgExpression *ve);
SgExpression *TestDVMArrayRef(SgExpression *e);
void ChangeArrayCoeff(SgSymbol *ar);
SgSymbol *CreateInitLoopVar(SgSymbol *dovar, SgSymbol *init);
SgSymbol* InitLoopSymbol(SgSymbol *si,SgType *t);
void DerivedTypeMemory(SgType *t);
SgSymbol* DerivedTypeBaseSymbol(SgSymbol *stype,SgType *t);
SgSymbol* CommonSymbol(SgSymbol *stype);
int StructureSize(SgSymbol *s);
void ConsistentArrayList  (SgExpression *el,SgExpression *gref, SgStatement *st, SgStatement *stmt1, SgStatement *stmt2);
void ConsistentArraysStart  (SgExpression *el);
void EndConsistent_Task_Region(SgStatement *stmt);
void doAxisTask(SgStatement *st, SgExpression *eref);
void Consistent_Task_Region(SgStatement *stmt);
//void TransModule(SgStatement *hedr);
void TransBlockData(SgStatement *hedr,SgStatement* &end_of_unit);
void VarDeclaration(SgStatement *stmt);
void initF90Names();
void ALLOCATEf90_arrays(SgStatement *stmt, distribute_list *distr);
void DEALLOCATEf90_arrays(SgStatement *stmt);
void ALLOCATEf90DistArray(SgSymbol *p, SgExpression *desc, SgStatement *stdis, SgStatement *stmt);
SgExpression * doSizeAllocArray(SgSymbol *ar, SgExpression *desc, SgStatement *st, int RTS_flag);
void StoreLowerBoundsPlusOfAllocatable(SgSymbol *ar,SgExpression *desc);
SgSymbol *FirstTypeField(SgType *t);
SgSymbol *baseMemoryOfDerivedType(SgType *t);
void NewSpecificationStatement(SgExpression *op, SgExpression *dvm_list, SgStatement *stmt);
void AllocatePointerHeader(SgSymbol *ar,SgStatement *stmt);
SgExpression *LeftMostField (SgExpression *e);
SgExpression *RightMostField(SgExpression *e);
int NumericTypeLength(SgType *t);
SgExpression *StringLengthExpr(SgType *t, SgSymbol *s);
SgExpression *LengthOfKindExpr(SgType *t, SgExpression *se, SgExpression *le);
SgExpression * TypeLengthExpr(SgType *t);
int IntrinsicTypeSize(SgType *t);
void DeclareVarDVMForInterface(SgStatement *lstat, symb_list *distsymb);
void DeleteShapeSpecDAr(SgStatement *stmt);
SgStatement *InterfaceBody(SgStatement *hedr);
SgStatement *InterfaceBlock(SgStatement *hedr);
SgExpression  *DVMVarInitialization(SgExpression *es);
SgExpression  *FileNameInitialization(SgExpression *es,char *name);
SgStatement *CreateModuleProcedure(SgStatement *mod_hedr, SgStatement *lst, SgStatement* &has_contains);
char* ModuleProcName(SgSymbol *smod);
void GenForUseStmts(SgStatement *hedr,SgStatement *where_st);
void GenForUseList(SgExpression *ul,SgStatement *stmt, SgStatement *where_st);
void GenDVMArray(SgSymbol *ar, SgStatement *stmt, SgStatement *where_st);
void GenCallForUSE(SgStatement *hedr,SgStatement *where_st);
SgStatement *MayBeDeleteModuleProc(SgStatement *mod_proc,SgStatement *end_mod);
int TestDVMDirectivesInModule(stmt_list *pstmt);
int TestDVMDirectivesInProcedure(stmt_list *pstmt);
int TestUseStmts();
int ArrayAssignment(SgStatement *stmt);
void MakeSection(SgExpression *are);
SgExpression *AsyncArrayElement(SgExpression *asc, SgExpression *ei);
SgSymbol *OriginalSymbol(SgSymbol *s);
void DistributeArrayList(SgStatement *stdis);
SgExpression *CurrentAM();
SgSymbol *TaskAMVSymbol(SgSymbol *s);
SgSymbol * CreateRegistrationArraySymbol();
SgSymbol *Rename(SgSymbol *ar, SgStatement *stmt);
SgExpression *ArraySection(SgExpression *are, SgSymbol *ar, int rank, SgStatement *stmt, int &init);
int DistrArrayAssign(SgStatement *stmt);
int AssignDistrArray(SgStatement *stmt);
SgSymbol *CheckSummaSymbol();
SgExpression *DebugIfCondition();
SgExpression *DebugIfNotCondition();
void InitDebugVar();
SgStatement *LastStatementOfDoNest(SgStatement *first_do);
void TranslateBlock (SgStatement *stat);
SgStatement *CreateCopyOfExecPartOfProcedure();
void  InsertCopyOfExecPartOfProcedure(SgStatement *stc);
int lookForDVMdirectivesInBlock(SgStatement *first,SgStatement *last,int contains[] );
SgSymbol *DebugGoToSymbol(SgType *t);
int IsGoToStatement(SgStatement *stmt);
int VarType_RTS(SgSymbol *var);
int TestType_RTS(SgType *type);
void CopyDvmBegin(SgStatement *entry,SgStatement *first_dvm_exec,SgStatement *last_dvm_entry);
void DoStmtsForENTRY(SgStatement *first_dvm_exec,SgStatement *last_dvm_entry);
void StructureProcessing(SgStatement *stmt);
SgStatement *ProcessVarDecl(SgStatement *vd);
void   ALLOCATEStructureComponent(SgSymbol *p, SgExpression *struct_e, SgExpression *desc, SgStatement *stmt);
void StoreLowerBoundsPlusOfAllocatableComponent(SgSymbol *ar,SgExpression *desc, SgExpression *struct_);
SgExpression * header_ref_in_structure (SgSymbol *ar, int n, SgExpression *struct_);
void MarkCoeffsAsUsed();
void LowerBoundInTriplet(SgExpression *e,SgSymbol *ar,int i, SgExpression *einit[]);
void UpperBoundInTriplet(SgExpression *e,SgSymbol *ar,int i, SgExpression *einit[]);
void DeleteSymbList(symb_list *ls);
void TranslateFromTo(SgStatement *first, SgStatement *last, int error_msg);
int isInternalOrModuleProcedure(SgStatement *header_st);
SgSymbol *LastSymbolOfFile(SgFile *f);
int TestType_DVMH(SgType *type);
int TestType_RTS2(SgType *type);
int CompareTypes(SgType *t1,SgType *t2);
int isInterfaceRTS2(SgStatement *stdis);
SgExpression *doDvmShapeList(SgSymbol *ar, SgStatement *st);
SgExpression *doShapeList(SgSymbol *ar, SgStatement *st);
SgExpression *AddElementToList(SgExpression *list, SgExpression *e);
SgExpression *ListUnion(SgExpression *list1, SgExpression *list2);
SgExpression *TypeSize_RTS2(SgType *type);
SgExpression *DeclaredShadowWidths(SgSymbol *ar);
void DerivedSpecification(SgExpression *edrv, SgStatement *stmt, SgExpression *eFunc[]);
void Shadow_Add_Directive(SgStatement *stmt);
SgExpression *CalcLinearForm(SgSymbol *ar, SgExpression *el);
SgSymbol *IOstatSymbol();
void ShadowNames(SgSymbol *ar, int axis, SgExpression *shadow_name_list);
int TestMaxDims(SgExpression *list, SgSymbol *ar, SgStatement *stmt);
void TemplateDeclarationTest(SgStatement *stmt);
int DeferredShape(SgExpression *eShape);
void Template_Create(SgStatement *stmt);
void Template_Delete(SgStatement *stmt);
//void RenamingDvmArraysByUse(SgStatement *stmt);
void RemovingDifferentNamesOfVar(SgStatement *first);
void UpdateUseListWithDvmArrays(SgStatement *use_stmt);
char *doOutFileName(const char *fdeb_name);

/*  parloop.cpp */
int ParallelLoop(SgStatement *stmt);
int ParallelLoop_Debug(SgStatement *stmt);
void EndOfParallelLoopNest(SgStatement *stmt, SgStatement *end_stmt, SgStatement *par_do,SgStatement *func);
int TestParallelDirective(SgStatement *stmt, int nloop, int ndo, SgStatement *first_do);
void CheckClauses(SgStatement *stmt, SgExpression *clause[]);
void StoreLoopPar(SgExpression *par[], int n, int ind, SgStatement*stl);
void  NewVarList(SgExpression *nl,SgStatement *stmt);
void Interface_1(SgStatement *stmt,SgExpression *clause[],SgSymbol *do_var[],SgExpression *init[],SgExpression *last[],SgExpression *step[],int nloop,int ndo,SgStatement *first_do,int iplp,int iout,SgStatement *stl,SgSymbol *newj,int ub);
void Interface_2(SgStatement *stmt,SgExpression *clause[],SgExpression *init[],SgExpression *last[],SgExpression *step[],int nloop,int ndo,SgStatement *first_do);
int doParallelLoopByHandler(int iplp, SgStatement *first, SgExpression *clause[], SgExpression *oldGroup, SgExpression *newGroup,SgExpression *oldGroup2, SgExpression *newGroup2);
int CreateParallelLoopByHandler_H2(SgExpression *init[], SgExpression *last[], SgExpression *step[], int nloop);
void MappingParallelLoop(SgStatement *stmt, int ilh );
int WhatInterface(SgStatement *stmt);
void CopyHeaderElems(SgStatement *st_after);
void ChangeLoopInitPar(SgStatement*stl,int nloop, SgExpression *do_init[],SgStatement*after);
int PositiveDoStep(SgExpression *step[], int i);
int Analyze_DO_steps(SgExpression *step[], int step_mask[],int ndo);
void CreateShadowGroupsForAccross(SgExpression *in_spec,SgExpression *out_spec,SgStatement * stmt,SgExpression *gleft,SgExpression *g,SgExpression *gright,int ag[],int all_positive_step,int loop_num[]);
int doRecurLengthArrays(SgExpression *shl, SgSymbol *ar, SgStatement *st, int rtype,int all_positive_step,int loop_num[]);
int RecurList (SgExpression *el, SgStatement *st, SgExpression *gref, int *ag,int gnum,int all_positive_step,int loop_num[]);
void DefineLoopNumberForNegStep(int step_mask[], int n,int loop_num[]);
void CreateShadowGroupsForAccrossNeg(SgExpression *in_spec,SgStatement * stmt,SgExpression *gleft,SgExpression *gright,int ag[],int all_positive_step,int loop_num[]);
int Recurrences(SgExpression *shl, SgExpression *lrec[], SgExpression *rrec[],int n);
int DepList (SgExpression *el, SgStatement *st, SgExpression *gref, int dep);
int doDepLengthArrays(SgExpression *shl, SgSymbol *ar, SgStatement *st, int dep);
void AcrossList(int ilh, int isOut, SgExpression *el, SgStatement *st, SgExpression *tie_clause);
SgExpression *doLowHighList(SgExpression *shl, SgSymbol *ar, SgStatement *st);
void ReceiveArray(SgExpression *spec_accr,SgStatement *parst);
void SendArray(SgExpression *spec_accr);
void ReductionList  (SgExpression *el,SgExpression *gref, SgStatement *st, SgStatement *stmt1, SgStatement *stmt2, int ilh2);
void ReductionVarsStart (SgExpression *el);
void ReductionVarsWait  (SgExpression *el);
void    InsertReductions_H(SgExpression *red_op_list, int ilh);
int LocElemNumber(SgExpression *en);
int Reduction_Debug(SgStatement *stmt);
int TestReductionClause(SgExpression *e);
align *CopyAlignTreeNode(SgSymbol *ar);
void InOutAcross(SgExpression *e, SgExpression* e_spec[], SgStatement *stmt);
void InOutSpecification(SgExpression *ea, SgExpression* e_spec[]);
SgExpression *AxisList(SgStatement *stmt, SgExpression *tied_array_ref);
int isInTieList(SgSymbol *ar, SgExpression *tie_list);

/*  acc.cpp */
SgStatement *RegistrateDVMArray(SgSymbol *ar,int ireg,int inflag,int outflag);
void RegisterVariablesInRegion(SgExpression *evl, int intent, int irgn);
int TargetsList(SgExpression *tgs);
void ACC_ROUTINE_Directive(SgStatement *stmt);
SgStatement *ACC_REGION_Directive(SgStatement *stmt);
SgStatement *ACC_END_REGION_Directive(SgStatement *stmt);
SgStatement *ACC_DATA_REGION_Directive(SgStatement *stmt);
SgStatement *ACC_END_DATA_REGION_Directive(SgStatement *stmt);
SgStatement *ACC_DO_Directive(SgStatement *stmt);
SgStatement *ACC_Directive(SgStatement *stmt);
int isACCdirective(SgStatement *stmt);
void NewRegion(SgStatement *stmt, int n, int data_flag);
void TempVarACC(SgStatement * func );
SgSymbol* DerivedTypeGpuBaseSymbol(SgSymbol *stype,SgType *t);
void Gpu_ArrayHeader (SgSymbol *ar,int ind);
void TypeGpuMemory(SgType *t);
void DerivedTypeGpuMemory(SgType *t);
SgSymbol *baseGpuMemory(SgType *t);
SgSymbol* GpuHeaderSymbol(SgSymbol *ar);
SgExpression * GpuHeaderRef(SgSymbol *ar) ;
SgExpression * GpuHeaderRefWithInd(SgSymbol *ar,int i);
SgSymbol *baseGpuMemoryOfDerivedType(SgType *t);
void DeclareVarGPU(SgStatement *lstat, SgType *tlen);
void BlocksThreadsSymbols();
void AllocateGPUBases(SgStatement *stat);
void ACC_ParallelLoopEnd(SgStatement *pardo);
void ACC_CreateParallelLoop(int iplp,SgStatement *first_do,int nloop,SgStatement *par_dir,SgExpression *clause[],int interface);
SgStatement *doIfThenConstrForLoop_GPU(SgExpression *ref,SgStatement *endhost,SgStatement *dowhile);
SgSymbol *KernelSymbol(SgStatement *st_do);
void Blocks_Off_Symbol();
void InitializeACC();
void InitializeInFuncACC();
SgSymbol *GPUModuleSymb(SgStatement *global_st);
void CreateGPUModule();
char *filenameACC();
SgSymbol *CudaforSymb(SgStatement *global_st);
void InsertUseStatementForGpuModule();
SgStatement *CreateKernelProcedure(SgSymbol *skernel);
SgStatement *CopyBodyLoopToKernel(SgStatement *first_do);
SgType *Type_dim3();
void CudaVars();
SgExpression *CreateBlocksThreadsSpec(int size, SgSymbol *s_blocks,SgSymbol *s_threads,SgSymbol *s_stream,SgSymbol *s_shared_mem);
SgExpression *KernelCondition(SgSymbol *sind,SgSymbol *sblock,int level);
SgExpression *KernelConditionWithDoStep(SgStatement *stdo,SgSymbol *sblock, int level);
void KernelWorkSymbols();
SgSymbol *ArraySymbol(char *name,SgType *basetype, SgExpression *range,SgStatement *scope);
SgSymbol *ArraySymbol(const char *name, SgType *basetype, SgExpression *range, SgStatement *scope);
SgStatement *Assign_To_ibof(int rank);
SgExpression *ExpressionForIbof(int rank);
SgStatement *DoStmt(SgStatement *first_do,int i);
SgStatement *Assign_To_IndVar(SgStatement *dost, int il, int nloop,SgSymbol *sblock);
SgExpression *IbaseRef(SgSymbol *base, int ind);
void CreatePrivateAndUsesVarList();
SgExpression * AddToVarRefList( SgExpression *list, SgExpression *e);
SgExpression * AddListToList( SgExpression *list, SgExpression *el);
void CreateDvmArrayList();
void CreateRegionVarList();
void CreateArgumentList();
int KindOfIndexType();
void MakeDeclarationsForKernel(SgSymbol *red_count_symb, SgType *idxTypeInKernel);
void MakeDeclarationsForKernel_On_C(SgType *idxTypeInKernel);
SgSymbol *K_baseMemory(SgType *t);
char *ChangeFtoCuf(const char *fout_name);
char *ChangeFto_C_Cu(const char *fout_name);
char *ChangeFto_info_C(const char *fout_name);
char *ChangeFto_cpp(const char *fout_name);
void UpdateOnHost(SgExpression *el, SgStatement *stmt);
SgStatement *ACC_UPDATE_Directive(SgStatement *stmt);
void InsertReductions_GPU(SgExpression *red_op_list,SgStatement *do_while, int loop_ind);
SgSymbol *Var_Offset_Symbol(SgSymbol *var);
int    MaxRedVarSize(SgExpression *red_op_list);
SgExpression *ThreadsGreedSize();
void InsertDoWhileForRedCount(SgStatement *cp);
void InsertDoWhileForRedCount_C(SgStatement *cp,SgSymbol *s_threads,SgSymbol *red_count_symb);
SgSymbol *RedCountSymbol(SgStatement *scope);
SgSymbol *OverallBlocksSymbol();
void AddBasesOfReductionVars();
void AddToRedVarList(SgExpression *ev,int i);
SgExpression *CreateRedOffsetVarList();
SgExpression *FindUsesInFormalArgumentList();
SgSymbol *RedOffsetSymbolInKernel(SgSymbol *s);
SgSymbol *RedBlockSymbolInKernel(SgSymbol *s,SgType *type);
void CreateReductionBlocks(SgStatement *stat,int nloop,SgExpression *red_op_list,SgSymbol *red_count_symb);
SgSymbol *IndVarInKernel(SgSymbol *s);
void ReductionBlockInKernel(SgStatement *stat,int nloop,SgSymbol *i_var,SgSymbol *j_var,SgExpression *ered, reduction_operation_list *rsl,SgSymbol *red_count_symb,int n);
void ReductionBlockInKernel_On_C_Cuda(SgStatement*, SgSymbol*, SgExpression*, reduction_operation_list*, SgIfStmt*, SgIfStmt*&, SgIfStmt*&, int&, bool withGridRed = false, bool across = false);
SgSymbol *SyncthreadsSymbol();
const char* RedFunctionInKernelC(const int num_red, const unsigned num_E, const unsigned num_IE);
SgStatement *RedOp_Assign(SgSymbol *i_var, SgSymbol *s_block, SgExpression *ered, SgSymbol *d, int k);
SgExpression *RedVar_Block_Ref(SgSymbol *sblock, SgSymbol *sind);
SgExpression *RedVar_Block_2D_Ref(SgSymbol *sblock, SgSymbol *sind, SgExpression *redind);
SgSymbol *RedOffsetSymbolInKernel_ToList(SgSymbol *s);
SgType *Type_For_Red_Loc(SgSymbol *redsym, SgSymbol *locsym, SgType *redtype, SgType *loctype);
SgType *TypeOfRedBlockSymbol(SgExpression *ered);
SgStatement * MakeStructDecl(SgSymbol *strc);
void TypeGpuMemoryForArray(SgType *t, int n);
SgSymbol*GpuBaseSymbolForLocArray(int n);
SgSymbol*K_GpuBaseSymbolForLocArray(int n);
SgSymbol *LocArrayBaseGpuMemory(SgType *t,int n);
SgSymbol *FormalLocationSymbol(SgSymbol *locvar, int i);
SgExpression *CreateFormalLocationList(SgSymbol *locvar, int numb);
void Do_Assign_For_Loc_Arrays();
SgSymbol *RedVarFieldSymb(SgSymbol *s_block);
SgExpression *RedLocVar_Block_Ref(SgSymbol *sblock, SgSymbol *sind, SgSymbol *d, SgExpression *field);
SgExpression *LocVarIndex(SgSymbol *sl, int i);
SgExpression *RedVarIndex(SgSymbol *sl, int i);
SgStatement *RedOp_If(SgSymbol *i_var, SgSymbol *s_block, SgExpression *ered, SgSymbol *d,int num);
SgExpression *ConditionForRedBlack(SgExpression *erb);
void AddFormalArg_For_LocArrays();
void AddActualArg_For_LocArrays();
SgExpression *CreateActualLocationList(SgSymbol *locvar, int numb);
int Create_New_File(char *file_name, SgFile *file, char *fout_name);
void UnparseTo_CufAndCu_Files(SgFile *f,FILE *fout_cuf, FILE *fout_C_cu, FILE *fout_info);
void UnparseForDynamicCompilation(FILE *fout_cpp);
void Create_C_extern_block();
void Create_info_block();
SgStatement *Create_C_Adapter_Function(SgSymbol *sadapter);
SgStatement *Create_C_Adapter_Function(SgSymbol *sadapter, int pos);
SgStatement *Create_C_Adapter_Function_For_Sequence(SgSymbol *sadapter,SgStatement *first_st);
void Prototypes();
void Typedef_Stmts(SgStatement *end_bl);
SgType *Cuda_Index_Type();
SgType *DvmhLoopRef_Type();
SgType *Dvmh_Type();
SgType * C_VoidType();
SgType * C_LongType();
SgType *C_PointerType(SgType *type);
SgType *C_ReferenceType(SgType *type);
SgSymbol *AdapterSymbol(SgStatement *st_do);
SgType *C_Derived_Type(SgSymbol *styp);
SgStatement *Create_Init_Cuda_Function();
SgStatement * makeSymbolDeclaration(SgSymbol *s);
SgStatement * makeSymbolDeclarationWithInit(SgSymbol *s,SgExpression *einit);
SgStatement * makeExternSymbolDeclaration(SgSymbol *s);
SgExpression * addDeclExpList(SgSymbol *s,SgExpression *el);
char *GpuHeaderName(SgSymbol *s) ;
SgExpression *ThreadsGridSize(SgSymbol *s_threads);
char *filename_short(SgStatement *st);
void ChangeAdapterName(SgSymbol *s);
SgSymbol *HostProcSymbol(SgStatement *st_do);
SgSymbol *IndirectFunctionSymbol(SgStatement *stmt, char *name);
void DeviceTypeConsts();
SgSymbol *DeviceTypeConst(int i);
char *ParallelLoopComment(int line);
char *SequenceComment(int line);
const char *CudaIndexTypeComment();
SgExpression *ArrayArgumentList();
SgExpression *BaseArgumentList();
void  CudaBlockSize(SgExpression *cuda_block_list);
void  CudaBlockSize(SgExpression *cuda_block_list,SgExpression *esize[]);
int ListElemNumber(SgExpression *list);
SgStatement *Create_Host_Across_Loop_Subroutine(SgSymbol *sHostProc);
SgStatement *Create_Host_Loop_Subroutine(SgSymbol *sHostProc, int type);
char * BoundName(SgSymbol *s, int i, int isLower);
SgSymbol *DummyBoundSymbol(SgSymbol *rv, int i, int isLower, SgStatement *st_hedr);
SgExpression *CreateDummyBoundListOfArray(SgSymbol *ar, reduction_operation_list *rl, SgStatement *st_hedr);
SgExpression * DummyListForReductionArrays(SgStatement *st_hedr);
SgExpression *CreateBaseMemoryList();
SgExpression *ConstRef_F95(int ic);
SgExpression *DvmType_Ref(SgExpression *e);
int ParLoopRank();
void DeclareArrayCoefficients(SgStatement *after);
void ReplaceLoopBounds(SgStatement *first_do, int lrank,SgSymbol *s_low_bound, SgSymbol *s_high_bound); 
int NumberOfCoeffs(SgSymbol *sg);
void    CreateStructuresForReductionInKernel();
void CompleteStructuresForReductionInKernel();
SgStatement *CreateLoopKernelProcedure(SgSymbol *skernel);
int OneSteps(int nl,SgStatement *nest);
int IConstStep(SgStatement *stdo);
SgExpression *blocksRef(SgSymbol *sblock, int ind);
SgType *CudaIndexType();
int KindOfCudaIndexType();
SgType *CudaOffsetType();
void SymbolOfCudaOffsetType();
void KernelBloksSymbol();
int IConstStep(SgStatement *stdo);
SgSymbol *RedGridSymbolInKernel(SgSymbol *s,int n,SgExpression *dimSizeArgs,int is_red_or_loc_var);
SgStatement *CopyBodyLoopForCudaKernel(SgStatement *first_do, int nloop);
SgStatement *CreateIfForRedBlack(SgStatement *loop_body, int nloop);
SgExpression *CreateKernelDummyList(SgSymbol *s_red_count_k, SgType *idxTypeInKernel);
SgExpression *CreateRedDummyList();
SgExpression *CreateArrayDummyList();
SgExpression *CoefficientList();
SgExpression *ArrayRefList();
SgExpression *UsedValueRef(SgSymbol *susg,SgSymbol *s);
SgType *C_Type(SgType *type);
void UsesInPrivateArrayDeclarations(SgExpression *privates);
SgExpression *UsesList(SgStatement *first,SgStatement *last);
void RefInExpr(SgExpression *e, int mode);
SgStatement *InnerMostLoop(SgStatement *dost,int nloop);
int isPrivate(SgSymbol *s);
int isReductionVar(SgSymbol *s);
void    CreateStructuresForReductions(SgExpression *red_op_list); 
SgExpression *isInUsesList(SgSymbol *s);
SgExpression *isInUsesListByChar(const char *symb);
//int isIntrinsicFunction(SgSymbol *sf);
//int IntrinsicInd(SgSymbol *sf);

void Call(SgSymbol *s, SgExpression *e);
void Argument(SgExpression *e, int i, SgSymbol *s);
SgSymbol *FunctionResultVar(SgStatement *func);
int isParDoIndexVar(SgSymbol *s);
SgSymbol *IntentConst(int intent);
int SectionBounds(SgExpression *are);
SgExpression *SectionBoundsList(SgExpression *are);
char *RegionComment(int line);
char *EndRegionComment(int line);
//SgExpression *DevicesExpr(int targets[]);
SgExpression *DevicesExpr(int targets);
SgStatement *ACC_ACTUAL_Directive(SgStatement *stmt);
SgStatement *ACC_GET_ACTUAL_Directive(SgStatement *stmt);
void DoPrivateList(SgStatement *par);
int AnalyzeRegion(SgStatement *reg_dir);
void MarkArraySymbol(SgSymbol *ar,int mode);
void RegisterUses(int irgn);
void RegisterDvmArrays(int irgn);
int IntentMode(SgSymbol *s);
int WhatMode(int mode,int mode_new);
void MarkAsRegistered(SgSymbol *s);
void HandlerTypeConsts();
SgSymbol *HandlerTypeConst(int i);
SgSymbol *RegionRegimConst(int regim);
int IntStepForHostHandler(SgExpression *dostep );
SgExpression *IsRedBlack(int nloop);
void FlagStatement(SgStatement *st);
SgStatement *ACC_CreateStatementGroup(SgStatement *first_st);
SgStatement *CopyBlockToKernel(SgStatement *first_st,SgStatement *last_st);
SgSymbol *isSymbolWithSameNameInTable(SgSymbol *first_in,char *name);
void ACC_RenewParLoopHeaderVars(SgStatement *first_do, int nloop);
void RefIn_LoopHeaderExpr(SgExpression *e,SgStatement *dost);
SgSymbol *RemoteAccessBufferInKernel(SgSymbol *ar,int rank);
SgSymbol *isSameNameBuffer(char *name,SgExpression *rml);
SgExpression *RemoteAccessHeaderList();
void CreateRemoteAccessBuffers();
coeffs *BufferCoeffs(SgSymbol *sbuf,SgSymbol *ar);
void AddRemoteAccessBufferList_ToArrayList();
SgExpression * ExpressionListsUnion(SgExpression *list, SgExpression *alist);
SgExpression *isInExprList(SgExpression *e,SgExpression *list);
symb_list *isInSymbList(SgSymbol *s, symb_list *slist);
symb_list *SymbolListsUnion(symb_list *slist1, symb_list *slist2);
void UnregisterVariables(int begin_block);
int isDestroyable(SgSymbol *s);
int isLocal(SgSymbol *s);
//void InsertDestroyBlock(SgStatement *st);
void ACC_UnregisterDvmBuffers();
void ACC_RegisterDvmBuffer(SgExpression *bufref, int buffer_rank);
SgSymbol *isSameNameShared(char *name);
void Doublet(SgExpression *e,SgSymbol *ar,int i, SgExpression *einit[],SgExpression *elast[]);
void ACC_StoreLowerBoundsOfDvmBuffer(SgSymbol *s, SgExpression *dim[], int dim_num[], int rank, int ibuf, SgStatement *stmt);
void ACC_Before_Loadrb(SgExpression *bufref);
void ACC_Region_After_Waitrb(SgExpression *bufref);
SgExpression *BufferLowerBound(SgExpression *ei);
SgExpression *DoStart(SgSymbol *dovar);
SgExpression *ReplaceIndexRefByLoopLowerBound(SgExpression *e,SgSymbol *dovar,SgExpression *estart);
SgSymbol *DummyReplicatedArray(SgSymbol *ar,int rank);
void  doStatementsToPerformByHandler(int ilh, SgSymbol *adapter_symb, SgSymbol *hostproc_symb,int is_parloop,int interface);
int CreateLoopForSequence(SgStatement *first);
void ACC_ReductionVarsAreActual();
SgExpression *DoReductionOperationList(SgStatement *par);
SgStatement *Create_Empty_Stat();
void DoHeadersForNonDvmArrays();
int HeaderForNonDvmArray(SgSymbol *s, SgStatement *stat);
SgExpression *HeaderForArrayInParallelDir(SgSymbol *ar, SgStatement *st);
SgSymbol *CreateReplicatedArray(SgSymbol *s);
void StoreLowerBoundsOfNonDvmArray(SgSymbol *ar);
void DeleteNonDvmArrays();
int isIn_acc_array_list(SgSymbol *s);
void TransferBlockToHostSubroutine(SgStatement *first_st,SgStatement *last_st,SgStatement *st_end);
SgStatement *Create_Host_Sequence_Subroutine(SgSymbol *sHostProc,SgStatement *first_st,SgStatement *last_st);
void MarkAsInsertedStatement(SgStatement *st);
void TestDvmObjectAssign(SgStatement *st);
void ReplaceAssignByIfForRegion(SgStatement *stmt);
SgSymbol *indexArraySymbol(SgSymbol *ar);
int MaxArrayRank();
SgSymbol *LocalPartSymbolInKernel(SgSymbol *ar);
SgSymbol *LocalPartArray(SgSymbol *ar);
SgExpression *LocalityConditionInKernel(SgSymbol *ar, SgExpression *ei[]);
void MakeDeclarationsInKernel_ForSequence(SgType*);
SgExpression *CreateKernelDummyList_ForSequence(SgType *idxTypeInKernel);
SgExpression *CreateLocalPartList();
SgExpression *CreateLocalPartList(SgType *indeTypeInKernel);
int isByValue(SgSymbol *s);
int isInByValueList(SgSymbol *s);
SgSymbol *GpuScalarAdrSymbolInAdapter(SgSymbol *s, SgStatement *st_hedr);
int CorrectIntent(SgExpression *e);
char *TestAndCorrectName(const char *name);
char *TestAndCorrectName(char *name);
SgSymbol *isSameNameInLoop(char *name);
SgSymbol *isSamePrivateVar(char *name);
SgSymbol *isSameUsedVar(char *name);
SgSymbol *isSameRedVar(char *name);
SgSymbol *isSameArray(char *name);
SgSymbol *isSameIndexVar(char *name);
SgType * C_LongLongType();
SgType * C_DvmType();
SgType * C_CudaIndexType();
char *OpenMpComment_HandlerType(int idvm);
char *OpenMpComment_InitFlags(int idvm);
void SymbolOfCudaIndexType();
SgType * C_BaseDvmType();
SgStatement *CreateLoopKernelFunction_On_C (SgSymbol *skernel);
SgStatement *Create_C_Kernel_Function(SgSymbol *sF);
void  CreateBlockForCalculationThreadLoopVariables();
SgStatement *FunctionCallStatement(SgSymbol *sf );
SgStatement *AssignStatement(SgExpression *le, SgExpression *re);
SgStatement *Declaration_Statement(SgSymbol *s );
char *LoopKernelComment();
char *SequenceKernelComment(int lineno);
char *Cuda_LoopHandlerComment();
char *Cuda_SequenceHandlerComment(int lineno);
char *Host_LoopHandlerComment();
char *Host_SequenceHandlerComment(int lineno);
char *Indirect_ProcedureComment(int lineno);
void SymbolChange_InBlock(SgSymbol *snew,SgSymbol *sold, SgStatement *first_st,SgStatement *last_st);
void SymbolChange_InExpr(SgSymbol *snew, SgSymbol *sold, SgExpression *e);
//void ACC_ShadowCompute(SgExpression *shadow_compute_list, SgStatement *st_shcmp);
int isOutArray(SgSymbol *s) ;
void doStatementsForShadowCompute(int ilh, int interface);
SgSymbol *doDeviceNumVar(SgStatement *st_hedr, SgStatement *st_exec, SgSymbol *s_dev_num,SgSymbol *s_loop_ref);
char *CommentLine(const char *txt);
int  WithAcrossClause();
SgExpression *CreateUsesDummyList();
SgExpression *ThreadIdxRefExpr(char *xyz);
SgExpression *ThreadIdxRefExpr(const char *xyz);
SgExpression *BlockIdxRefExpr(char *xyz);
SgExpression *BlockIdxRefExpr(const char *xyz);
SgStatement * CreateKernel_ForSequence(SgSymbol *kernel_symb, SgStatement *first_st, SgStatement *last_st, SgType *idxTypeInKernel);
SgSymbol *KernelDummyLocalPart(SgSymbol *s);
SgSymbol *KernelDummyPointerVar(SgSymbol *s);
SgStatement *CreateLoopKernel(SgSymbol *skernel, SgType *idxTypeInKernel);
SgStatement *CreateLoopKernel(SgSymbol *skernel, AnalyzeReturnGpuO1 &infoGpuO1, SgType *idxTypeInKernel);
char * DimSizeName(SgSymbol *s,int i);
SgExpression *MallocExpr(SgSymbol *var,SgExpression *eldim);
SgSymbol *FormalDimSizeSymbol(SgSymbol *var, int i);
SgSymbol *FormalLowBoundSymbol(SgSymbol *var, int i);
SgExpression *CreateFormalDimSizeList(SgSymbol *var);
SgExpression *CreateFormalLowBoundList(SgSymbol *var);
SgSymbol *RedInitValSymbolInKernel(SgSymbol *s,SgExpression *dimSizeArgs);
SgSymbol *InitValSymbolForRedInAdapter(SgSymbol *s, SgStatement *st_hedr);
SgSymbol *IndexSymbolForRedVarInKernel(int i);
SgExpression *RedVarUpperBound(SgExpression *el,int i);
SgExpression *CreateIndexVarList(int N);
SgStatement *doLoopNestForReductionArray(reduction_operation_list *rl, SgStatement *ass);
SgSymbol *IndexLoopVar(int i);
SgExpression *SubscriptListOfRedArray(SgSymbol *ar);
SgSymbol *RedVariableSymbolInKernel(SgSymbol *s,SgExpression *dimSizeArgs);
SgExpression *DimSizeListOfReductionArrays();
SgExpression *isConstantBound(SgSymbol *rv, int i, int isLower);
SgExpression *CreateBoundListOfArray(SgSymbol *ar);
SgExpression * BoundListOfReductionArrays();
SgStatement * makeSymbolDeclaration_T(SgStatement *st_hedr);
void CreateComplexTypeSymbols(SgStatement *st_bl);
void SaveLineNumbers(SgStatement *stat_copy);
SgSymbol *SymbolInKernel(SgSymbol *s);
SgExpression *ToInt(SgExpression *e);
void ParallelOnList(SgStatement *par);
SgExpression *ACC_GroupRef(int ind);
SgExpression *dim3FunctionCall(int i);
SgType *CudaOffsetTypeRef_Type();
int IsInLabelList(SgLabel *lab, stmt_list *labeled_list);
void ReplaceExitCycleGoto(SgStatement *block, SgStatement *stk);
int IsParDoLabel(SgLabel *lab, int pl_rank);
int isInLoop(SgStatement *stmt);
void  ReplaceCaseStatement(SgStatement *first);
void FormatAndDataStatementExport(SgStatement *par_dir, SgStatement *first_do);
void AddExternStmtToBlock_C();
void  GenerateStmtsForInfoFile();
char *Up_regs_Symbol_Name(SgSymbol *s_regs);
SgStatement *AssignBlocksSElement(int i,int pl_rank, SgSymbol *s_blocksS,SgSymbol *s_idxL,SgSymbol *s_idxH,SgSymbol *s_threads);
char *IncludeComment(const char *txt);
char *DefineComment(char *txt);
void TypeSymbols(SgStatement *end_bl);
void GenerateEndIfDir();
void GenerateDeclarationDir();
SgStatement *Assign_To_cur_blocks(int i, int nloop);
SgStatement *Assign_To_rest_blocks(int i);
SgStatement *Assign_To_IndVar2(SgStatement *dost, int i, int nloop);
SgExpression *KernelCondition2(SgStatement *dost, int level);
void InsertAssignForReduction(SgStatement *st_where,SgSymbol *s_num_of_red_blocks,SgSymbol *s_fill_flag,SgSymbol *s_overallBlocks, SgSymbol *s_threads);
void  InsertPrepareReductionCalls(SgStatement *st_where,SgSymbol *s_loop_ref,SgSymbol *s_num_of_red_blocks,SgSymbol *s_fill_flag,SgSymbol *s_red_num);
void InsertFinishReductionCalls(SgStatement *st_where,SgSymbol *s_loop_ref,SgSymbol *s_red_num);
SgStatement *IfForHeader(SgSymbol *s_restBlocks, SgSymbol *s_blocks, SgSymbol *s_max_blocks);
int testLabelUse(SgLabel *lb, int pl_rank, SgStatement *stmt);
SgExpression * LinearIndex(int ind, int L);
int TestGroupStatement(SgStatement *first,SgStatement *last);
int TestOneGroupStatement(SgStatement *stmt);
void DeclareUsedVars();
void DeclareInternalPrivateVars();
void DeclarePrivateVars();
void DeclareArrayBases();
void DeclareArrayCoeffsInKernel(SgType*);
void DeclareLocalPartVars();
void DeclareLocalPartVars(SgType*);
void DeclareDummyArgumentsForReductions(SgSymbol *red_count_symb, SgType*);
int isPrivateInRegion(SgSymbol *s);
void DeclareDoVars();
int is_acc_array(SgSymbol *s);
SgExpression *CreateArrayAdrList(SgSymbol *header_symb, SgStatement *st_host);
char *Header_DummyArgName(SgSymbol *s);
SgExpression *Dimension(SgSymbol *hs, int i, int rank);
SgSymbol *DummyDvmHeaderSymbol(SgSymbol *ar, SgStatement *st_hedr);
SgSymbol *DummyDvmArraySymbol(SgSymbol *ar,SgSymbol *header_symb);
SgExpression *ElementOfAddrArgumentList(SgSymbol *s);
SgExpression *AddrArgumentList();
void CompareReductionAndPrivateList();
void TestPrivateList();
void doNotForCuda();
char * DevicesString(int targets);
int GeneratedForCuda();
void RefInControlList(SgExpression *eoc[],int n);
void RefInControlList_Inquire(SgExpression *eoc[],int n);
void RefInIOList(SgExpression *iol, int mode);
void RefInImplicitLoop(SgExpression *eim, int mode);
SgSymbol *dvm000SymbolForHost(int host_dvm, SgStatement *hedr);
SgExpression *Red_grid_index(SgSymbol *sind);
SgExpression *BlockDimsProduct();
SgExpression *LowerShiftForArrays (SgSymbol *ar, int i);
SgExpression *UpperShiftForArrays (SgSymbol *ar, int i);
SgExpression *coefProd(int i, SgExpression *ec);
SgExpression *LinearFormForRedArray (SgSymbol *ar,  SgExpression *el, reduction_operation_list *rsl);
void CreateCalledFunctionDeclarations(SgStatement *st_hedr);
void CreateUseStatementsForCalledProcedures(SgStatement *st_hedr);
void CreateUseStatementsForDerivedTypes(SgStatement *st_hedr);
void CreateUseStatements(SgStatement *st_hedr);
SgStatement *CreateHostProcedure(SgSymbol *sHostProc);
void DeclareCalledFunctions();
int isForCudaRegion();
SgSymbol *CudaIndexConst();
char *CalledProcedureComment(const char *txt, SgSymbol *symb);
SgSymbol *KernelDummyArray(SgSymbol *s);
void ConstantSubstitutionInTypeSpec(SgExpression *e);
int TestLocal(SgExpression *list);
void EnterDataRegionForLocalVariables(SgStatement *st);
void EnterDataRegionForAllocated(SgStatement *stmt);
void EnterDataRegion(SgExpression *ale,SgStatement *stmt);
void ExitDataRegion(SgExpression *ale,SgStatement *stmt);
int  ExitDataRegionForAllocated(SgStatement *st,int begin_block);
void ExitDataRegionForLocalVariables(SgStatement *st, int is);
void ExtractCopy(SgExpression *elist);
void CleanAllocatedList();
SgStatement *CreateIndirectDistributionProcedure(SgSymbol *sProc,symb_list *paramList,symb_list *dummy_index_list,SgExpression *derived_elem_list,int flag);
SgExpression *FirstArrayElementSubscriptsForHandler(SgSymbol *ar);
SgSymbol *HeaderSymbolForHandler(SgSymbol *ar);
void TestRoutineAttribute(SgSymbol *s, SgStatement *routine_interface);
int LookForRoutineDir(SgStatement *interfaceFunc);
SgStatement *Interface(SgSymbol *s);
SgStatement *getInterface(SgSymbol *s);
/*   acc_analyzer.cpp   */
//void Private_Vars_Analyzer(SgStatement *firstSt, SgStatement *lastSt);
//void Private_Vars_Function_Analyzer(SgStatement* start);
void Private_Vars_Project_Analyzer();
void TieList(SgStatement *par);

/*  hpf.cpp    */ 
int SearchDistArrayRef(SgExpression *e, SgStatement *stmt);
void BufferDistArrayRef(SgExpression *e, SgStatement *stmt);
int IndependentLoop_Debug(SgStatement *stmt);
int IndependentLoop(SgStatement *stmt);
void SkipIndepLoopNest(SgStatement *stmt);
SgExpression *ConnectNewList(SgExpression *el1, SgExpression *el2);
void IEXLoopAnalyse(SgStatement *func);
void IEXLoopBegin(SgStatement *st);
void INDLoopBegin();
int doAlignIterationIND();
void ReductionListIND1();
void ReductionListIND2(SgExpression *gref);
void ReductionListIND_Err();
void OffDoVarsOfNest(SgStatement *end_stmt);
void  IND_UsedDistArrayRef(SgExpression *e, SgStatement *st);
SgExpression *IND_ModifiedDistArrayRef(SgExpression *e, SgStatement *st);
void  RemoteVariableListIND();
void INDReductionDebug();
int AxisNumOfDoVarInExpr (SgExpression *e, SgSymbol *dovar_ident[], int ni, SgExpression **eref, int use[], int *pINuse, SgStatement *st);
int isNewVar(SgSymbol *s);
int isINDtarget(SgExpression *re);
IND_ref_list *isInINDrefList(SgExpression *re);
SgExpression *INDBufferHeaderNplus1(IND_ref_list *rme, SgSymbol *ar, int ni, int ihead);
void IND_DistArrayRef(SgExpression *e, SgStatement *st, IND_ref_list *el);
void BufferHeaderCopy(SgSymbol *b, int ibuf, int n, IND_ref_list *el);
void  ArrayHeaderCopy(int n, IND_ref_list *el); 
void InitInquiryVar(int iq);
int CompareIfReduction(SgExpression *e1, SgExpression *e2);
int ReductionFuncNumber(SgExpression *e,int expr_ind);
int IsInNewList(SgExpression *pos_red, SgExpression *newl);
int IsInReductionList(SgExpression *pos_red);
int IsReductionVariable(SgExpression *pos_red, SgExpression *newl);
int IsError(SgExpression *pos_red, SgExpression *newl, int variant);
int FindInExpr(SgExpression *red, SgExpression *expr);
int IsReductionOp(SgStatement *st, SgExpression *newl);
int IsLIFReductionOp(SgStatement *st, SgExpression *newl);

/*  stmt.cpp    */  
void doAssignStmt (SgExpression *re);
void doAssignTo (SgExpression *le, SgExpression *re);
void doAssignTo_After (SgExpression *le, SgExpression *re);
SgExpression * LeftPart_AssignStmt (SgExpression *re);
void doAssignStmtBefore (SgExpression *re, SgStatement *current);
void doAssignStmtAfter (SgExpression *re);
void Extract_Stmt(SgStatement *st);
void InsertNewStatementBefore (SgStatement *stat, SgStatement *current); 
void InsertNewStatementAfter (SgStatement *stat, SgStatement *current, SgStatement *cp);
void ReplaceByIfStmt(SgStatement *stmt);
void ReplaceAssignByIf(SgStatement *stmt);
int  isDoEndStmt(SgStatement *stmt);
void ReplaceDoNestLabel(SgStatement *last_st, SgLabel *new_lab);
void ReplaceDoNestLabel_Above(SgStatement *last_st, SgStatement *from_st,SgLabel *new_lab);
void ReplaceParDoNestLabel(SgStatement *last_st, SgStatement *from_st,SgLabel *new_lab);
void ReplaceContext(SgStatement *stmt);
void LogIf_to_IfThen(SgStatement *stmt);
SgStatement *ReplaceDoLabel(SgStatement *last_st, SgLabel *new_lab);
SgStatement *ReplaceLabelOfDoStmt(SgStatement *first,SgStatement *last_st, SgLabel *new_lab);
int  isParallelLoopEndStmt(SgStatement *stmt,SgStatement *first_do);
SgStatement * lastStmtOfDo(SgStatement *stdo);
SgStatement *ContinueWithLabel(SgLabel *lab);
SgStatement *doIfThenConstrForRedis(SgExpression *headref, SgStatement *stmt, int index);
SgStatement *doIfThenConstrForRealign(int iamv, SgStatement *stmt, int cond);
SgStatement *doIfThenConstrForRealign(SgExpression *headref, SgStatement *stmt, int cond);
SgStatement *doIfThenConstrForPrefetch(SgStatement *stmt);
SgStatement *doIfThenConstrForRemAcc(SgSymbol *group, SgStatement *stmt);
void  doIfForReduction(SgExpression *redgref, int deb);
void  doLogIfForHeap(SgSymbol *heap, int size);
SgStatement *doIfThenConstrForIND(SgExpression *e, int cnst, int cond, int has_else, SgStatement *stmt, SgStatement *cp);
SgStatement *PrintStat(SgExpression *item);
void doIfForDelete(SgSymbol *sg, SgStatement *stmt);
SgStatement *doIfForFileVariables(SgSymbol *s);
SgStatement *doIfForCreateReduction(SgSymbol *gs, int i, int flag);
void  doIfForConsistent(SgExpression *gref);
SgStatement *doIfThenConstrWithArElem(SgSymbol *ar, int ind);
SgStatement *ReplaceStmt_By_IfThenConstr(SgStatement *stmt,SgExpression *econd);
void TransferStmtAfter(SgStatement *stmt, SgStatement *where);
SgStatement *CreateIfThenConstr(SgExpression *cond, SgStatement *st);
void TransferBlockIntoIfConstr(SgStatement *ifst, SgStatement *stmt1, SgStatement *stmt2);
void TransferStatementGroup(SgStatement *first_st, SgStatement *last_st, SgStatement *st_end);
void ReplaceArithIF(stmt_list *gol);
void ReplaceComputedGoTo(stmt_list *gol);
void UnparseFunctionsOfFile(SgFile *f,FILE *fout);
int isDoEndStmt_f90(SgStatement *stmt);
SgLabel * LabelOfDoStmt(SgStatement *stmt);
SgStatement * lastStmtOfIf(SgStatement *stif);
void doCallAfter(SgStatement *call);
void doCallStmt(SgStatement *call);
SgStatement *ReplaceBy_DO_ENDDO(SgStatement *first,SgStatement *last_st);
SgStatement *IncludeLine(char *str);
SgStatement *PreprocessorDirective(char *str);
SgStatement *PreprocessorDirective(const char *str);
SgStatement *ifdef_dir(char *str);
SgStatement *ifndef_dir(char *str);
SgStatement *endif_dir();
SgStatement *else_dir();
SgExpression *CalculateArrayBound(SgExpression *edim,SgSymbol *ar, int flag_private);
void ReplaceArrayBoundsInDeclaration(SgExpression *e);
int ExplicitShape(SgExpression *eShape);
SgSymbol *ArraySymbolInHostHandler(SgSymbol *ar,SgStatement *scope);
SgSymbol *DeclareSymbolInHostHandler(SgSymbol *var, SgStatement *st_hedr, SgSymbol *loc_var);
char *RegisterConstName();
int TightlyNestedLoops_Test(SgStatement *prev_do, SgStatement *dost);
SgStatement *NextExecStat(SgStatement *st);
int isExecutableDVMHdirective(SgStatement *stmt);
SgStatement * lastStmtOf(SgStatement *st);
SgStatement *lastStmtOfFile(SgFile *f);
void DeleteSaveAttribute(SgStatement *stmt);
SgStatement *doIfThenConstrForOnDir(SgStatement *stmt);
SgStatement *doIfForOnDir(SgStatement *stmt, SgLabel *t_lab);
SgStatement *ReplaceOnByIf(SgStatement *stmt,SgStatement *end_stmt);
SgStatement *TestEndOn(SgStatement *stmt);
void doLogIfForAllocated(SgExpression *objref, SgStatement *stmt);
void doLogIfForIOstat(SgSymbol *s, SgExpression *espec, SgStatement *stmt);

/*  funcall.cpp */
void Get_AM();
void GetVM();
SgExpression *GetAM();
SgExpression *Reconf(SgExpression *size_array, int rank, int sign);
int  BeginBlock ();
void  BeginBlock_H();
void EndBlock (SgStatement *st);
SgStatement *EndBlock_H(SgStatement *st);
SgExpression * EndBl (int n);
void RTLInit ();
void RTLExit (SgStatement *st);
SgExpression * CreateAMView(SgExpression *size_array, int rank, int sign);
SgExpression * DistributeAM (SgExpression *amv, SgExpression *psref,int count, int idisars, int iparam); 
SgStatement *RedistributeAM(SgExpression *ref, SgExpression *psref, int count, int idisars,int sign); 
SgExpression *CreateDistArray(SgSymbol *das, SgExpression *array_header,                             SgExpression *size_array, int rank, int ileft, int iright, int sign, int re_sign) ;
SgExpression *AlignArray (SgExpression *array_handle,
                          SgExpression *template_handle,
                          int iaxis, 
                          int icoeff,  
                          int iconst); 
SgStatement *RealignArr (SgExpression *array_header,
                          SgExpression *pattern_ref,
                          int iaxis, 
                          int icoeff,  
                          int iconst,
                          int new_sign ); 
SgStatement *StartBound(SgExpression *gref);
SgStatement *WaitBound(SgExpression *gref);
SgStatement *SendBound(SgExpression *gref);
SgStatement *ReceiveBound(SgExpression *gref);
SgExpression *DelBG(SgExpression *gref);
SgStatement *BoundFirst(int iloopref,SgExpression *gref);
SgStatement *BoundLast (int iloopref, SgExpression *gref);
SgExpression *CreateReductionGroup();
SgExpression *ReductionVar(int num_red, SgExpression *red_array, int ntype, int length, SgExpression *loc_array, int loc_length, int sign);
SgStatement *LoopReduction(int ilh, int num_red, SgExpression *red_array, int ntype, SgExpression *length, SgExpression *loc_array, SgExpression *loc_length);
SgStatement *InsertRedVar(SgExpression *gref, int irv, int iplp);
SgExpression *SaveRedVars(SgExpression *gref);
SgStatement  *StartRed(SgExpression *gref);
SgStatement  *WaitRed (SgExpression *gref);
SgExpression *DelRG(SgExpression *gref);
SgStatement * BeginParLoop (int iloopref, SgExpression *header, int rank, int iaxis, int nr, int iinp, int iout);
SgExpression *CreateParLoop(int rank);
SgStatement  *EndParLoop(int iloopref);
SgExpression *doLoop(int iloopref);
SgExpression *GetAddres(SgSymbol * var);
SgExpression *GetAddresMem(SgExpression * em);
SgStatement *Addres(SgExpression * em);
SgExpression *GetAddresDVM(SgExpression * em);
void  CreateBoundGroup(SgExpression *gref);
SgStatement *InsertArrayBound(SgExpression *gref, SgExpression *head, int ileft, int iright, int corner) ;
SgExpression *TestIOProcessor();
SgExpression *GetRank(int iref);
SgExpression *GetSize(SgExpression *ref,int axis);
SgExpression *DA_CopyTo_A(SgExpression *head, SgExpression *toar, int init_ind,                                 int last_ind, int step_ind, int regim);
SgExpression *A_CopyTo_DA( SgExpression *fromar, SgExpression *head, int init_ind,                                 int last_ind, int step_ind, int regim);
SgExpression *ArrayCopy(SgExpression *from_are, int from_init, int from_last, int from_step, 
  SgExpression *to_are, int to_init, int to_last, int to_step, int regim);
SgExpression *AsyncArrayCopy(SgExpression *from_are,   int from_init,   int from_last,         int from_step, SgExpression *to_are,  int to_init,  int to_last,  int to_step,  int regim,   SgExpression *flag);
SgExpression *WaitCopy(SgExpression *flag);
SgExpression *ReadWriteElement(SgExpression *from, SgExpression *to, int ind);  
SgStatement *SendMemory(int icount, int inda, int indl);
SgStatement *CloseFiles(); 
SgExpression *AddHeader(SgExpression *head_new,SgExpression *head );
SgExpression *DeleteObject(SgExpression *objref); 
SgExpression *TestElement(SgExpression *head, int ind);
SgExpression *HasElement(SgExpression *ar_header, int n, SgExpression *index_list);
SgExpression *CalculateLinear(SgExpression *ar_header, int n, SgExpression *index_list);  
SgStatement *D_LoadVar(SgExpression *vref,int type,SgExpression *headref,SgExpression *opref); 
SgStatement *D_LoadVar2(SgExpression *vref, int type, SgExpression *headref, SgExpression *opref) ;
SgStatement *D_StorVar();
SgStatement *D_PrStorVar(SgExpression *vref,int type,SgExpression *headref,SgExpression *opref); 
SgStatement *D_InOutVar(SgExpression *vref, int type, SgExpression *headref);
SgStatement *D_Lnumb(int num_line);
SgStatement *D_Fname();
SgStatement *D_Begpl(int num_loop,int rank,int iinit);
//SgStatement *D_Begpl(int num_loop);
SgStatement *D_Begsl(int num_loop);
SgStatement *D_Begtr(int num_treg);
SgStatement *D_Skpbl();
SgStatement *D_Endl(int num_loop,int begin_line);
SgStatement *D_Iter(SgSymbol *do_var, int type);
SgStatement *D_Iter_I(int ind, int indtp);
SgStatement *D_Iter_ON(int ind, int type);
SgStatement *D_RmBuf(SgExpression *source_headref, SgExpression *buf_headref, int rank, int index) ;
SgStatement *St_Binter(int num_fragment, SgExpression *valvar);
SgStatement *St_Einter(int num_fragment,int begin_line);
SgStatement *St_Bsloop(int num_fragment);
SgStatement *St_Bploop(int num_fragment);
SgStatement *St_Enloop(int num_fragment,int begin_line);
SgStatement *St_Biof();
SgStatement *St_Eiof();
SgExpression *CrtPS(SgExpression *psref, int ii, int il, int sign);
SgExpression *GetAMView(SgExpression *headref);
SgExpression *GetAMR(SgExpression *amvref, SgExpression *index);
SgStatement  *MapAM(SgExpression *am, SgExpression *ps);
SgExpression *RunAM(SgExpression *am);
SgStatement  *StopAM();
SgStatement  *MapTasks(SgExpression *taskCount,SgExpression *procCount,SgExpression *params,SgExpression *low_proc,SgExpression *high_proc,SgExpression *renum);
SgExpression *LoadBG(SgExpression *gref);
SgExpression *WaitBG(SgExpression *gref);
SgExpression *CreateBG(int st_sign,int del_sign);
SgExpression *InsertRemBuf(SgExpression *gref, SgExpression *buf);
SgStatement *CreateRemBuf(SgExpression *header,SgExpression *buffer,int st_sign,int iplp, int iaxis,int icoeff,int iconst);
SgExpression *RemoteAccessKind(SgExpression *header,SgExpression *buffer,int st_sign,int iplp,int iaxis,int icoeff,int iconst,int ilsh,int ihsh);
SgStatement *CreateRemBufP(SgExpression *header,SgExpression *buffer,int st_sign,SgExpression *psref,int icoord);
SgStatement *LoadRemBuf(SgExpression *buf);
SgStatement *WaitRemBuf(SgExpression *buf);
SgExpression *LoadIG(SgSymbol *group);
SgExpression *WaitIG(SgSymbol *group);
SgExpression *CreateIG(int st_sign,int del_sign);
SgExpression *InsertIndBuf(SgSymbol *group, SgExpression *buf);
SgExpression *CreateIndBuf(SgExpression *header,SgExpression *buffer,int st_sign,SgExpression *mehead,int iconst);
SgExpression *LoadIndBuf(SgExpression *buf);
SgExpression *WaitIndBuf(SgExpression *buf);
SgStatement *InsertArrayBoundDep(SgExpression *gref, SgExpression *head, int ileft, int iright, int max, int ishsign);
SgExpression *GetProcSys(SgExpression *amref);
SgExpression * GenBlock   (SgExpression *psref, SgExpression *amv, int iweight, int icount);
SgExpression * WeightBlock(SgExpression *psref, SgExpression *amv, int iweight, int iwnumb, int icount);
SgExpression * MultBlock (SgExpression *amv, int iaxisdiv, int n);
void TypeControl();
void TypeControl_New();
SgStatement *D_Read(SgExpression *adr) ;
SgStatement *D_ReadA(SgExpression *adr,int indel, int icount) ;
SgExpression *LocIndType(int irv, int type);
SgExpression *DVM_Receive(int iplp,SgExpression *mem,int t,int is);
SgExpression *DVM_Send(int iplp,SgExpression *mem,int t,int is);
SgStatement *InitAcross(int acrtype,SgExpression *oldg, SgExpression *newg);
SgStatement *D_FileLine(int num_line, SgStatement *stmt);
SgStatement *D_DummyFileLine(int num_line, const char *fname);
SgStatement *AddBound( );
SgStatement *InsertArrayBoundSec(SgExpression *gref, SgExpression *head, int ilsec, int irsec, int iilowshs, int illowshs, int iihishs,int ilhishs, int max, int ishsign);
SgStatement *D_RegistrateArray(int rank, int type, SgExpression *headref,  SgExpression *size_array,SgExpression *arref);
SgExpression *D_CreateDebRedGroup();
SgStatement *D_InsRedVar(SgExpression *dgref,int num_red, SgExpression *red_array, int ntype, int length, SgExpression *loc_array, int loc_length, int locindtype);
SgExpression *D_SaveRG(SgExpression *dgref);
SgStatement *D_CalcRG(SgExpression *dgref);
SgStatement *D_DelRG(SgExpression *dgref);
SgStatement *AddBoundShadow(SgExpression *head,int ileft,int iright );
SgExpression *CreateConsistArray(SgSymbol *cas, SgExpression *array_header, SgExpression *size_array, int rank,  int sign, int re_sign);
SgExpression *FreeConsistent(SgExpression *header);
SgExpression *StartConsistent(SgExpression *header,int iplp,int iaxis,int icoeff,int iconst,int re_sign);
SgExpression *WaitConsistent(SgExpression *header);
SgExpression *CreateConsGroup(int st_sign,int del_sign);
SgExpression *WaitConsGroup(SgExpression *gref);
SgExpression *StartConsGroup(SgExpression *gref);
SgExpression *InsertConsGroup(SgExpression *gref,SgExpression *header,int iplp,int iaxis,int icoeff,int iconst,int re_sign);
SgExpression *TaskConsistent(SgExpression *header,SgExpression *amvref, int iaxis);
SgExpression *IncludeConsistentTask(SgExpression *gref,SgExpression *header,SgExpression *amvref, int iaxis,int re_sign);
SgExpression *ExstractConsGroup(SgExpression *gref, int del_sign);

SgExpression *SizeFunction(SgSymbol *ar, int i);
SgExpression *SizeFunctionWithKind(SgSymbol *ar, int i, int kind);
SgExpression *LBOUNDFunction(SgSymbol *ar, int i);
SgExpression *UBOUNDFunction(SgSymbol *ar, int i);
SgExpression *LENFunction(SgSymbol *string);
SgExpression *CHARFunction(int i);
SgExpression *KINDFunction(SgExpression *arg);
SgExpression *MaxFunction(SgExpression *arg1,SgExpression *arg2);
SgExpression *MinFunction(SgExpression *arg1,SgExpression *arg2);
SgExpression *IandFunction(SgExpression *arg1,SgExpression *arg2);
SgExpression *IorFunction(SgExpression *arg1,SgExpression *arg2);
SgExpression *AllocatedFunction(SgExpression *arg);
SgExpression *AssociatedFunction(SgExpression *arg);
SgExpression *TypeFunction(SgType *t, SgExpression *e, SgExpression *ke);
SgExpression *SummaOfDistrArray(SgExpression *headref, SgExpression *sumvarref);
SgExpression *SummaOfArray(SgExpression *are, int rank, SgExpression *size, int ntype,SgExpression *sumvarref);
SgExpression *doPLmb(int iloopref,int ino);
SgExpression *doSL(int num_loop,int iout);
SgExpression *doPLmbSEQ(int ino, int rank, int iout);
SgStatement *D_PutDebugVarAdr(SgSymbol *dbg_var, int flag);
SgExpression *RegionCreate(int flag);
SgExpression *RegistrateDataRegion();
SgStatement *RegisterScalar(int irgn,SgSymbol *c_intent,SgSymbol *s);
SgStatement *RegionRegisterScalar(int irgn,SgSymbol *c_intent,SgSymbol *s);
SgStatement *RegisterSubArray(int irgn, SgSymbol *c_intent, SgSymbol *ar, int ilow, int ihigh);
SgStatement *RegionRegisterSubArray(int irgn, SgSymbol *c_intent, SgSymbol *ar, SgExpression *index_list);
SgStatement *RegisterArray(int irgn, SgSymbol *c_intent, SgSymbol *ar);
SgStatement *RegionRegisterArray(int irgn, SgSymbol *c_intent, SgSymbol *ar);
SgStatement *RegisterBufferArray(int irgn, SgSymbol *c_intent, SgExpression *bufref, int ilow, int ihigh);
SgStatement *ActualScalar(SgSymbol *s);
SgStatement *ActualSubArray(SgSymbol *ar, int ilow, int ihigh);
SgStatement *ActualSubArray_2(SgSymbol *ar, int rank, SgExpression *index_list);
SgStatement *ActualSubVariable(SgSymbol *s, int ilow, int ihigh);
SgStatement *ActualSubVariable_2(SgSymbol *s, int rank, SgExpression *index_list);
SgStatement *GetActualScalar(SgSymbol *s);
SgStatement *GetActualSubArray(SgSymbol *ar, int ilow, int ihigh);
SgStatement *GetActualSubArray_2(SgSymbol *ar, int rank, SgExpression *index_list);
SgStatement *DestroyArray(SgExpression *objref);
SgStatement *DestroyScalar(SgExpression *objref);
SgStatement *RegistrateDVMArray(int ireg,SgExpression *header,SgExpression *gpuheader, SgExpression *gpubase,int inflag,int outflag);
SgStatement *EndRegion(int n);
SgStatement *UnRegistrateDataRegion(int n);
SgStatement *RTL_GPU_Init();
SgStatement *RTL_GPU_Finish();
SgStatement *Init_Cuda();
//SgExpression *RegistrateLoop_GPU(int irgn,int iplp,int flag_first,int flag_last);
//SgExpression *StartLoop_GPU(int il);
SgExpression *LoopCreate_H(int irgn,int iplp);
SgExpression *LoopCreate_H2(int nloop, SgExpression *paramList);
SgExpression *LoopCreate_H2(SgExpression &paramList);
SgStatement *LoopMap(int ilh, SgExpression *desc, int rank, SgExpression *paramList);  
SgStatement *LoopMap(SgExpression &paramList);
SgExpression *AlignmentLinear(SgExpression *axis,SgExpression *multiplier,SgExpression *summand);
SgStatement *LoopStart_H(int il);
SgStatement *LoopEnd_H(int il);
SgStatement *LoopPerform_H(int il);
SgStatement *LoopPerform_H2(int il);
SgExpression *Loop_GPU(int il);
SgType *IndexType();
SgStatement *CallKernel_GPU(SgSymbol *skernel, SgExpression *blosks_threads);
//SgExpression *StartShadow_GPU(int irgn,SgExpression *gref);
SgExpression *GetActualEdges_H(SgExpression *gref);
//SgStatement *DoneShadow_GPU(int ish);
SgStatement *ShadowRenew_H(SgExpression *gref);
SgStatement *ShadowRenew_H2(SgExpression *head,int corner,int rank,SgExpression *shlist);
SgStatement *IndirectShadowRenew(SgExpression *head, int axis, SgExpression *shadow_name);
SgStatement *EndHostExec_GPU(int il);
SgStatement *UpdateDVMArrayOnHost(SgSymbol *s);
SgStatement *InsertRed_GPU(int il,int irv,SgExpression *base,SgExpression *loc_base,SgExpression *offset,SgExpression *loc_offset);
SgExpression *GetNaturalBase(SgSymbol *s_cur_dev,SgSymbol *shead); /* C */
SgExpression *FillHeader(SgSymbol *s_cur_dev,SgSymbol *sbase,SgSymbol *shead,SgSymbol *sgpuhead);/* C */
SgExpression *FillHeader_Ex(SgSymbol *s_cur_dev,SgSymbol *sbase,SgSymbol *shead,SgSymbol *sgpuhead,SgSymbol *soutType,SgSymbol *sParams);
SgExpression *LoopDoCuda(SgSymbol *s_loop_ref,SgSymbol *s_blocks,SgSymbol *s_threads,SgSymbol *s_stream, SgSymbol *s_blocks_info,SgSymbol *s_const);
SgFunctionCallExp *CallKernel(SgSymbol *skernel, SgExpression *blosks_threads);
SgExpression *RegisterReduction(SgSymbol *s_loop_ref, SgSymbol *s_var_num, SgSymbol *s_red,SgSymbol *s_loc);
SgExpression *InitReduction(SgSymbol *s_loop_ref, SgSymbol *s_var_num, SgSymbol *s_red,SgSymbol *s_loc);
SgExpression *LoopSharedNeeded(SgSymbol *s_loop_ref, SgExpression *ecount);
SgExpression *GetDeviceAddr(SgSymbol *s_cur_dev,SgSymbol *s_var);
SgStatement *RegisterHandler_H(int il,SgSymbol *dev_const, SgExpression *flag, SgSymbol *sfun,int bcount,int parcount); /* OpenMP */
SgStatement *RegisterHandler_H2(int il,SgSymbol *dev_const, SgExpression *flag, SgExpression *efun);
SgStatement *SetCudaBlock_H(int il, int ib);
SgStatement *SetCudaBlock_H2(int il, SgExpression *X, SgExpression *Y, SgExpression *Z );
SgStatement *LoopInsertReduction_H(int ilh,int irv);
SgStatement *LoopRedInit_HH(SgSymbol *loop_s, int nred, SgSymbol *sRed,SgSymbol *sLoc);
SgStatement *LoopRedPost_HH(SgSymbol *loop_s, int nred, SgSymbol *sRed,SgSymbol *sLoc);
SgStatement *LoopFillBounds_HH(SgSymbol *loop_s, SgSymbol *sBlow,SgSymbol *sBhigh,SgSymbol *sBstep);
SgExpression *LoopGetSlotCount_HH(SgSymbol *loop_s);
SgStatement *RegionForDevices(int irgn,SgExpression *devices);
SgStatement *StartRegion(int irgn);
SgStatement *RegionDestroyRb(int irgn, SgExpression *bufref);
SgStatement *RegionAfterWaitrb(int irgn, SgExpression *bufref);
SgStatement *RegionBeforeLoadrb(SgExpression *bufref);
SgStatement *ActualArray(SgSymbol *ar);
SgStatement *GetActualArray(SgExpression *objref);
SgStatement *GetActualSubVariable(SgSymbol *s, int ilow, int ihigh);
SgStatement *GetActualSubVariable_2(SgSymbol *s, int rank, SgExpression *index_list);
SgStatement *CreateDvmArrayHeader(SgSymbol *cas, SgExpression *array_header, SgExpression *size_array, int rank,  int sign, int re_sign) ;
SgStatement *HandleConsistent(SgExpression *gref);
SgExpression *HasLocalElement(SgSymbol *s_loop_ref,SgSymbol *ar, SgSymbol *IndAr);
SgExpression *HasLocalElement_H2(SgSymbol *s_loop_ref, SgSymbol*ar, int n, SgExpression *index_list);
SgExpression *GetLocalPart(SgSymbol *s_loop_ref, SgSymbol *shead, SgSymbol *s_const);
SgStatement *FillLocalPart_HH(SgSymbol *loop_s, SgSymbol *shead, SgSymbol *spart);
SgStatement *SetVariableName(int irgn, SgSymbol *var);
SgStatement *SetArrayName(int irgn, SgSymbol *ar);
SgStatement *LoopShadowCompute_H(int il,SgExpression *headref);
SgStatement *LoopShadowCompute_Array(int il,SgExpression *headref);
SgStatement *ShadowCompute(int ilh,SgExpression *head,int rank,SgExpression *shlist);
SgStatement *ActualAll();
SgStatement *GetActualAll();
SgStatement *Redistribute_H(SgExpression *objref,int new_sign);
SgStatement *Realign_H(SgExpression *objref, int new_sign);
SgExpression *GetOverallStep(SgSymbol *s_loop_ref);
SgExpression *GetDeviceNum(SgSymbol *s_loop_ref);
SgExpression *FillBounds(SgSymbol *loop_s, SgSymbol *sBlow,SgSymbol *sBhigh,SgSymbol *sBstep);
SgExpression *mallocFunction(SgExpression *arg, SgStatement *scope);
SgExpression *freeFunction(SgExpression *arg, SgStatement *scope);
SgExpression *RedPost(SgSymbol *loop_s, SgSymbol *s_var_num, SgSymbol *sRed,SgSymbol *sLoc);
SgExpression *CudaInitReduction(SgSymbol *s_loop_ref,  SgSymbol *s_var_num, SgSymbol *s_dev_red, SgSymbol *s_dev_loc);
SgExpression *CudaReplicate(SgSymbol *Addr, SgSymbol *recordSize, SgSymbol *quantity, SgSymbol *devPtr);
SgStatement *LoopAcross_H(int il,SgExpression *oldGroup,SgExpression *newGroup);
SgStatement *LoopAcross_H2(int il, int isOut, SgExpression *headref, int rank, SgExpression *shlist);
SgExpression *GetDependencyMask(SgSymbol *s_loop_ref) ;
SgExpression *CudaTransform(SgSymbol *s_loop_ref, SgSymbol *s_head, SgSymbol *s_BackFlag, SgSymbol *s_headH, SgSymbol *s_addrParam); 
SgExpression *CudaAutoTransform(SgSymbol *s_loop_ref, SgSymbol *s_head);
SgExpression *ApplyOffset(SgSymbol *s_head, SgSymbol *s_base, SgSymbol *s_headH) ;
SgExpression *GetConfig(SgSymbol *s_loop_ref,SgSymbol *s_shared_perThread,SgSymbol *s_regs_perThread,SgSymbol *s_threads,SgSymbol *s_stream, SgSymbol *s_shared_perBlock);
SgExpression *PrepareReduction(SgSymbol *s_loop_ref,  SgSymbol *s_var_num, SgSymbol *s_count, SgSymbol *s_fill_flag, int fixedCount = 0, int fillFlag = -1);
SgExpression *FinishReduction(SgSymbol *s_loop_ref,  SgSymbol *s_var_num);
SgExpression *Register_Red(SgSymbol *s_loop_ref, SgSymbol *s_var_num, SgSymbol *s_red_array, SgSymbol *s_loc_array,SgSymbol *s_offset,SgSymbol *s_loc_offset);
SgExpression *ChangeFilledBounds(SgSymbol *s_low,SgSymbol *s_high,SgSymbol *s_idx, SgSymbol *s_n,SgSymbol *s_dep,SgSymbol *s_type,SgSymbol *s_idxs);
SgStatement *D_FileLineConst(int line, SgStatement *stmt);
SgExpression *GetStage(SgStatement *first_do, int iplp);
SgExpression *GuessIndexType(SgSymbol *s_loop_ref);
SgExpression *RtcSetLang(SgSymbol *s_loop_ref, const int lang);
SgStatement *DeleteObject_H(SgExpression *objref);
SgStatement *DataEnter(SgExpression *objref,SgExpression *esize);
SgStatement *DataExit(SgExpression *objref, int saveFlag);
SgStatement *ScopeInsert(SgExpression *objref);
SgStatement *ScopeStart();
SgStatement *ScopeEnd();
SgExpression *HandlerFunc(SgSymbol *sfun, int paramCount, SgExpression *arg_list);
SgExpression *Register_Array_H2(SgExpression *ehead);
SgExpression *DvmhString(SgExpression *s);
SgExpression *DvmhConnected(SgExpression *unit, SgExpression *failIfYes);
SgExpression *DvmhStringVariable(SgExpression *v); 
SgExpression *DvmhVariable(SgExpression *v);
SgExpression *VarGenHeader(SgExpression *item); 
SgStatement *SaveCheckpointFilenames(SgExpression *cpName, std::vector<SgExpression *> filenames);
SgStatement *CheckFilename(SgExpression *cpName, SgExpression *filename);
SgStatement *GetNextFilename(SgExpression *cpName, SgExpression *lastFile, SgExpression *currentFile);
SgStatement *CpWait(SgExpression *cpName, SgExpression *statusVar);
SgStatement *CpSaveAsyncUnit(SgExpression *cpName, SgExpression *file, SgExpression *unit);
SgStatement *Dvmh_Line(int line, SgStatement *stmt);
SgStatement *DvmhArrayCreate(SgSymbol *das, SgExpression *array_header, int rank, SgExpression *arglist);
SgStatement *DvmhTemplateCreate(SgSymbol *das, SgExpression *array_header, int rank, SgExpression *arglist);
SgExpression *DvmhReplicated();
SgExpression *DvmhBlock(int axis);
SgExpression *DvmhWgtBlock(int axis, SgSymbol *sw, SgExpression *en);
SgExpression *DvmhGenBlock(int axis, SgSymbol *sg);
SgExpression *DvmhMultBlock(int axis, SgExpression *em);
SgExpression *DvmhIndirect(int axis, SgSymbol *smap);
SgExpression *DvmhDerived(int axis, SgExpression *derived_rhs, SgExpression *counter_func, SgExpression *filler_func);
SgStatement *DvmhDistribute(SgSymbol *das, int rank, SgExpression *distr_list);
SgStatement *DvmhRedistribute(SgSymbol *das, int rank, SgExpression *distr_list);
SgStatement *DvmhAlign(SgSymbol *als, SgSymbol *align_base, int nr, SgExpression *alignment_list);
SgStatement *DvmhRealign(SgExpression *objref, int new_sign, SgExpression *pattern_ref, int nr, SgExpression *align_list);
SgStatement *IndirectLocalize(SgExpression *ref_array, SgExpression *target_array, int iaxis);
SgExpression *DvmhExprScan(SgExpression *edummy);
SgExpression *DvmhExprConstant(SgExpression *e);
SgExpression *DvmhExprIgnore();
SgExpression *DvmhDerivedRhs(SgExpression *erhs);
SgStatement *ShadowAdd(SgExpression *templ, int iaxis, SgExpression *derived_rhs, SgExpression *counter_func, SgExpression *filler_func, SgExpression *shadow_name, int nl, SgExpression *array_list);
SgStatement *CreateDvmArrayHeader_2(SgSymbol *ar, SgExpression *array_header,  int rank,  SgExpression *shape_list);
SgStatement *ForgetHeader(SgExpression *objref);
SgExpression *DvmhArraySlice(int rank, SgExpression *slice_list);
SgStatement *DvmhArrayCopy( SgExpression *array_header_right, int rank_right, SgExpression *slice_list_right, SgExpression *array_header_left, int rank_left, SgExpression *slice_list_left );
SgStatement *DvmhArrayCopyWhole( SgExpression *array_header_right, SgExpression *array_header_left );
SgStatement *Correspondence_H (int il, SgExpression *hedr, SgExpression *axis_list);

/*  io.cpp      */
void IO_ThroughBuffer(SgSymbol *ar, SgStatement *stmt);
int  IOcontrol(SgExpression *e, SgExpression *ioc[],int type);
int  control_list1(SgExpression *e, SgExpression *ioc[]);
int  control_list_open(SgExpression *e, SgExpression *ioc[]);
int  control_list_inquire (SgExpression *e, SgExpression *ioc[]);
int  control_list_rw(SgExpression *e, SgExpression *ioc[]);
void InsertSendInputList(SgExpression *input_list, SgExpression * io_stat,SgStatement *stmt);
void InsertSendIOSTAT(SgExpression * eios);
void InsertSendInquire(SgExpression * eioc[]);
void ImplicitLoop(SgExpression *ein, int *pj, SgExpression *iisize[], SgExpression *iielem[],SgExpression *iinumb[], SgStatement *stmt);
SgExpression * InputItemLength (SgExpression *e, SgStatement *stmt);
SgExpression *SubstringLength(SgExpression *sub);
SgExpression *ArrayLength(SgSymbol *ar, SgStatement *stmt, int err);
SgExpression *ArrayLengthInElems(SgSymbol *ar, SgStatement *stmt, int err);
SgExpression *ElemLength(SgSymbol *ar);
SgExpression *NumbOfElem(SgExpression *es,SgExpression *el);
int  TestIOList(SgExpression *iol,SgStatement *stmt,int error_msg);
int  ImplicitLoopTest(SgExpression *eim,SgStatement *stmt,int error_msg);
int  IOitemTest(SgExpression *e,SgStatement *stmt,int error_msg);
SgExpression *CorrectLastOpnd(SgExpression *len, SgSymbol *ar, SgExpression *bounds,SgStatement *stmt);
int   hasAsterOrOneInLastDim(SgSymbol *ar);
SgSymbol *lastDimInd(SgExpression *el);
SgExpression *FirstArrayElement(SgSymbol *ar);
SgExpression *Barrier();
int SpecialKindImplicitLoop(SgExpression *el, SgExpression *ein, int *pj, SgExpression *iisize[], SgExpression *iielem[],SgExpression *iinumb[],SgStatement *stmt);
SgExpression *FirstArrayElementOfSection(SgSymbol *ar,SgExpression *einit[]);
SgExpression *FirstElementOfSection(SgExpression *ea);
int ArraySectionRank(SgExpression *ea);
int ContinuousSection(SgExpression *ea);
SgExpression *SectionLength(SgExpression *ea, SgStatement *stmt, int err);
int isColon(SgExpression *e);
int ContinuousSection(SgExpression *ea);
SgStatement *Any_IO_Statement(SgStatement *stmt);
void IoModeDirective(SgStatement *stmt, char io_modes_str[], int error_msg);
void Open_Statement(SgStatement *stmt, char io_modes_str[], int error_msg);
void Close_Statement(SgStatement *stmt, int error_msg);
void Inquiry_Statement(SgStatement *stmt, int error_msg);
void FilePosition_Statement(SgStatement *stmt, int error_msg);
void ReadWritePrint_Statement(SgStatement *stmt, int error_msg);
void ReadWrite_Statement(SgStatement *stmt, int error_msg);
void Close_RTS(SgStatement *stmt,int error_msg);
void Open_RTS(SgStatement* stmt, char* io_modes_str, int error_msg);
void FilePosition_RTS(SgStatement* stmt, int error_msg);
void ReadWrite_RTS(SgStatement *stmt, int error_msg);
void OpenClose(SgStatement *stmt, int error_msg);
void NewOpenClose(SgStatement *stmt);
void NewFilePosition(SgStatement *stmt);
void Replace_IO_Statement(SgExpression *ioc[],SgStatement *stmt);
void ReplaceByStop(int io_err, SgStatement *stmt);
int Check_Control_IO_Statement(int io_err, SgExpression *ioc[], SgStatement *stmt, int error_msg);
void FilePosition(SgStatement *stmt, int error_msg);
void NewFilePosition(SgStatement *stmt);
void Inquiry(SgStatement *stmt, int error_msg);
SgStatement *IfConnected(SgStatement *stmt, SgExpression *unit, bool suitableForNewIO);
int control_list_oc_new(SgExpression *e, SgExpression *ioc[]);
void Dvmh_Close(SgExpression *ioc[]);
void Dvmh_Open(SgExpression *ioc[], const char *io_modes_str);
void Dvmh_FilePosition(SgExpression *ioc[], int variant);
void Dvmh_ReadWrite(SgExpression **ioc, SgStatement *stmt);
void InsertGotoStmt(SgExpression *err, int index);
void OccupyDvm000Elem(SgExpression *cond, int index);
int control_list_close_new(SgExpression *e, SgExpression *ioc[]);
int control_list_open_new(SgExpression *e, SgExpression *ioc[]);
bool checkArgsClose(SgExpression **ioc, SgStatement *stmt, int error_msg);
bool checkArgsOpen(SgExpression **ioc, SgStatement *stmt, int error_msg, char const *io_modes_str);
bool checkArgsEnfileRewind(SgExpression **ioc, SgStatement *stmt, int error_msg);
bool checkArgsRW(SgExpression **ioc, SgStatement *stmt, int error_msg);
bool checkVarRefIntArg(SgExpression *arg, int i, SgStatement *stmt, int error_msg);
bool checkStringVarArg(SgExpression *arg, int i, SgStatement *stmt, int error_msg);
bool checkStringArg(SgExpression *arg, int i, SgStatement *stmt, int error_msg);
bool checkIntArg(SgExpression *arg, int i, SgStatement *stmt, int error_msg);
bool checkLabelRefArg(SgExpression *arg, SgStatement *stmt, int error_msg);
bool checkDefaultStringArg(SgExpression *arg, const char **possible_values, int count, int i, SgStatement *stmt, int error_msg);
bool checkAccessArg(SgExpression **ioc, SgStatement *stmt, int error_msg);
bool checkFormArg(SgExpression **ioc, SgStatement *stmt, int error_msg);
bool checkFormattedArgs(SgExpression **ioc, SgStatement *stmt, int error_msg);
//bool checkFileArg(SgExpression **ioc, SgStatement *stmt, int error_msg);
bool checkPosArg(SgExpression **ioc, SgStatement *stmt, int error_msg);
bool checkUnitAndNewunitArg(SgExpression **ioc, SgStatement *stmt, int error_msg);
bool checkNewunitArg(SgExpression **ioc, SgStatement *stmt, int error_msg);
bool checkReclArg(SgExpression **ioc, SgStatement *stmt, int error_msg);
bool checkStatusArg(SgExpression **ioc, SgStatement *stmt, int error_msg);
bool checkDvmModeArg(char const *io_modes_str, SgStatement *stmt, int error_msg);
const char *stringValuesOfArgs(int argNumber, SgStatement *stmt);
int FixError(const char *str, int ierr, SgSymbol *s, SgStatement *stmt, int error_msg);
SgExpression *ArrayFieldLast(SgExpression *e);
SgExpression *FirstElementOfField(SgExpression *e_RecRef);
int hasEndErrControlSpecifier(SgStatement *stmt, SgExpression *ioEnd[] );
void ChangeSpecifierByIOSTAT(SgExpression *e);
void ChangeControlList(SgStatement *stmt, SgExpression *ioEnd[] );
void ReplaceStatementWithEndErrSpecifier(SgStatement *stmt, SgExpression *ioEnd[] );

/* checkpoint.cpp */
void CP_Create_Statement(SgStatement *st, int error_msg);
void CP_Save_Statement(SgStatement *st, int error_msg);
void CP_Load_Statement(SgStatement *st, int error_msg);
void CP_Wait(SgStatement *stmt, int error_msg);

/*  debug.cpp   */
void D_AddToDoList (int Nloop, int Nline, SgLabel *lab, SgSymbol *var);
void D_DelFromDoList ();
SgStatement *CloseLoop(SgStatement *stmt);
int  isDoVar(SgSymbol *s);
void SetDoVar(SgSymbol *s);
void OffDoVar(SgSymbol *s);
void FreeDoList();  
void OpenParLoop(SgStatement *dost);
void OpenParLoop_Inter(SgStatement *dost,int ind, int indtp,SgSymbol *do_var[],int ndo);
void CloseParLoop(SgStatement *dostmt,SgStatement *stmt,SgStatement *end_stmt);
void CloseDoInParLoop(SgStatement *end_stmt);
void AddAttrLoopNumber(int No,SgStatement *stmt);
int  LoopNumber(SgStatement *stmt);
SgExpression *Value(SgExpression *e);
SgExpression *Value_F95(SgExpression *e);
void LineNumber(SgStatement *st);
int  OpenInterval (SgStatement *stmt);
int  CloseInterval();
void ExitInterval(SgStatement *stmt);
void OverLoopAnalyse(SgStatement *func);
void FormLoopIntList(SgStatement *st);
int  IntervalNumber(SgStatement *stmt);
void SeqLoopBegin(SgStatement *st);
void AddAttrIntervalNumber(SgStatement *stmt);
SgStatement *SeqLoopEnd(SgStatement *end_stmt,SgStatement *stmt);
void SeqLoopEndInParLoop(SgStatement *end_stmt,SgStatement *stmt);
void SkipParLoopNest(SgStatement *stmt);
int hasGoToIn(SgStatement *parent,SgLabel *lab_after);
int ToThisLabel(SgStatement *gost, SgLabel *lab_after);
void  ReplaceGoToLabelInsideLoop(SgStatement *parent,SgStatement *lst, SgLabel *lab_after);
void  ReplaceGoToInsideLoop(SgStatement *dost,SgStatement *endst, SgStatement *dst, SgStatement *est);
void DeleteGoToFromList(SgStatement *stmt);
void BeginDebugFragment(int num);
void EndDebugFragment(int num);
void  DebugTaskRegion(SgStatement *stmt);
void CloseTaskRegion(SgStatement *tr_st,SgStatement *stmt);
int TypeDVM();
void ArrayRegistration ();
heap_pointer_list *HeapList(heap_pointer_list *heap_point, SgSymbol *sheap,SgSymbol *sp);
SgSymbol *HeapForPointer(SgSymbol *p);
void RegistrateAllocArray( stmt_list *alloc_st);
void AllocArrayRegistration( SgStatement *stmt);
SgStatement *Check(SgStatement *stmt);
void Registrate_Ar(SgSymbol *ar);
void Registrate_Allocatable(SgExpression *alce, SgStatement *stmt);
void AllocatableArrayRegistration (SgStatement *stmt);
void InsertStmtsBeforeGoTo(SgStatement *gotost, SgStatement *dst, SgStatement *est);
SgStatement *StmtWithLabel(SgLabel *lab);
int LineNumberOfStmtWithLabel(SgLabel *lab);
void AddDebugGotoAttribute(SgStatement *gotost,SgStatement *lnumst);
void  DebugVarArrayRef(SgExpression *e,SgStatement *stmt);
void  DebugVarArrayRef_Left(SgExpression *e,SgStatement *stmt,SgStatement *stcur);
void  DebugArg_VarArrayRef(SgExpression *e,SgStatement *stmt);
void CheckVarArrayRef(SgExpression *e, SgStatement *stmt, SgExpression *epr);
void  DebugLoop(SgStatement *stmt);
void  DebugParLoop(SgStatement *stmt,int rank, int iinp);
void DebugExpression(SgExpression *e, SgStatement *stmt);
void DebugAssignStatement(SgStatement *stmt);

/*  help.cpp */
SgLabel *  firstLabel(SgFile *f);
int isLabel(int num) ;
SgLabel * GetLabel();
const char * header (int i);
char *UnparseExpr(SgExpression *e) ;
void printVariantName(int i);
int FragmentList(char *l, int dlevel, int elevel);
void AddToFragmentList(int num1, int num2,int dlevel,int elevel);
void Error(const char *s, const char *t, int num, SgStatement *stmt);
void err(const char *s, int num, SgStatement *stmt);
void Err_g(const char *s, const char *t, int num);
void err_p(const char *s, const char *name, int num);
void Warning(const char *s, const char *t, int num, SgStatement *stmt);
void warn(const char *s, int num, SgStatement *stmt);
void Warn_g(const char *s, const char *t, int num);
void errN(const char *s, int num, SgStatement *stmt);
void format_num (int num, char num3s[]);
SgExpression *ConnectList(SgExpression *el1, SgExpression *el2);
int is_integer_value(char *str);
char *SymbListString(symb_list *symbl);
char *baseFileName(char *name);

/* acc_f2c.cpp */
void Translate_Fortran_To_C(SgStatement *stat, SgStatement *last, std::vector <std::stack <SgStatement*> > &, int);
void Translate_Fortran_To_C(SgStatement *Stmt);

SgSymbol* createNewFunctionSymbol(const char *name);
void swapDimentionsInprivateList(void);
void createNewFCall(SgExpression*, SgExpression*&, const char*, int);
SgFunctionCallExp* createNewFCall(const char *name);
void convertExpr(SgExpression*, SgExpression*&);
void initSupportedVars(void);
void initF2C_FunctionCalls(void);
void initIntrinsicFunctionNames();

/* acc_across.cpp */
ArgsForKernel *Create_C_Adapter_Function_Across(SgSymbol *sadapter);
SgStatement *CreateLoopKernelAcross(SgSymbol*, ArgsForKernel*, SgType*);
SgStatement *CreateLoopKernelAcross(SgSymbol*, ArgsForKernel*, int, SgType*);

/* acc_index_analyzer.cpp */
SgExpression* analyzeArrayIndxs(SgSymbol *array, SgExpression *listIdx);

/* aks_analyzeLoops.cpp */
AnalyzeReturnGpuO1 analyzeLoopBody(int type);

/* acc_rtc.cpp */
void ACC_RTC_AddCalledProcedureComment(SgSymbol *symbK);
void ACC_RTC_CompleteAllParams();
void ACC_RTC_ConvertCudaKernel(SgStatement *cuda_kernel, const char *kernelName);
char* _RTC_convertUnparse(const char* inBuf);
symb_list *ACC_RTC_ExpandCallList(symb_list *call_list);
char *_RTC_PrototypesForKernel(symb_list *call_list);
void ACC_RTC_AddFunctionsToKernelConsts(SgStatement *first_kernel_const);
void _RTC_UnparsedFunctionsToKernelConst(SgStatement *stmt);


/* acc_utilities.cpp */
char* copyOfUnparse(const char *strUp);
char* aks_strupr(const char *str);
char* aks_strlowr(const char *str);
void correctPrivateList(int flag);
SgType *indexTypeInKernel(int rt_Type);
SgFunctionCallExp *cudaKernelCall(SgSymbol *skernel, SgExpression *specs, SgExpression *args);
void DeclareDoVars(SgType *indexType);
SgExpression *CreateArrayDummyList(SgType *indexType);
SgStatement* createKernelCallsInCudaHandler(SgFunctionCallExp *baseFunc, SgSymbol *s_loop_ref, SgSymbol *idxTypeInKernel, SgSymbol *s_blocks);
int isIntrinsicFunctionName(const char *name);
void addNumberOfFileToAttribute(SgProject *project);
int getIntrinsicFunctionTypeSize(const char* name);
void recExpressionPrintFdvm(SgExpression* exp);

/* calls.cpp */
void ProjectStructure(SgProject &project);
void FileStructure(SgFile *file);
void doCallGraph(SgFile *file);
SgStatement *ProgramUnit(SgStatement *first);
SgStatement *Subprogram(SgStatement *func);
void FunctionCallSearch(SgExpression *e);
void Arg_FunctionCallSearch(SgExpression *e) ;
void FunctionCallSearch_Left(SgExpression *e);
void Call_Site (SgSymbol *s, int inlined);
SgSymbol * GetProcedureHeaderSymbol(SgSymbol *s);
void MarkAsRoutine(SgSymbol *s);
void MarkAsCalled(SgSymbol *s);
void MarkAsUserProcedure(SgSymbol *s);
void MakeFunctionCopy(SgSymbol *s);
SgStatement *HeaderStatement(SgSymbol *s);
void InsertCalledProcedureCopies();
SgStatement *InsertProcedureCopy(SgStatement *st_header, SgSymbol *sproc, int is_routine, SgStatement *after);
int FromOtherFile(SgSymbol *s);
int findParameterNumber(SgSymbol *s, char *name); 
int isInParameter(SgSymbol *s, int i);
SgSymbol *ProcedureSymbol(SgSymbol *s);
int IsPureProcedure(SgSymbol *s);
int IsElementalProcedure(SgSymbol *s);
int IsRecursiveProcedure(SgSymbol *s);
int IsNoBodyProcedure(SgSymbol *s);
int isUserFunction(SgSymbol *s);
int IsInternalProcedure(SgSymbol *s);
SgExpression *FunctionDummyList(SgSymbol *s);
char *FunctionResultIdentifier(SgSymbol *sfun);
SgSymbol *isSameNameInProcedure(char *name, SgSymbol *sfun);
char *NameCheck(char *name, SgSymbol *sfun);
void InsertReturnBeforeEnd(SgStatement *new_header, SgStatement *end_st);
void ChangeReturnStmts(SgStatement *new_header, SgStatement *end_st, SgSymbol *sres);
void ExtractDeclarationStatements(SgStatement *header);
void MakeFunctionDeclarations(SgStatement *header, SgSymbol *s_last);
SgSymbol *LastSymbolOfFunction(SgStatement *header);
void ConvertArrayReferences(SgStatement *first, SgStatement *last);
void doPrototype(SgStatement *func_hedr, SgStatement *block_header, int static_flag);
SgStatement *FunctionPrototype(SgSymbol *sf);
bool CreateIntefacePrototype(SgStatement *header);
SgStatement *hasInterface(SgSymbol *s);
void SaveInterface(SgSymbol *s, SgStatement *interface);
SgStatement  *TranslateProcedureHeader_To_C(SgStatement *new_header);
//-----------------------------------------------------------------------
extern "C" char* funparse_bfnd(...);
extern "C" char* Tool_Unparse2_LLnode(...);
extern "C" void Init_Unparser(...);
extern "C" char* UnparseBif_Char(...);
//extern "C" void UnparseProgram_ThroughAllocBuf(...);
extern "C" PTR_BFND duplicateStmtsFromTo(...);

/*OMP*/
const int COMMON_VAR = 1027; /*OMP*/
const int SAVE_VAR = 1028; /*OMP*/
const int STATIC_CONTEXT = 1029; /*OMP*/
const int DEBUG_STAT = 1030; /*OMP*/
const int FIRST_ELEM = 1031; /*OMP*/
const int FORMAL_PARAM = 1032; /*OMP*/
const int DECLARED_FUNC = 1033; /*OMP*/
const int DEBUG_LINE = 1034; /*OMP*/
EXTERN int OMP_program; /*OMP*/
EXTERN int omp_debug;   //set to 1 by -d1  or -d2 or -d3 or -d4 flag
EXTERN int omp_perf;   //set to 1 by -emp flag
#define D1 1
#define D2 2
#define DPERF 4
#define D3 8
#define D4 16
#define D5 32
/*Attributes for omp statements*/
const int OMP_STMT_BEFORE = 2000; /*OMP*/
const int OMP_STMT_AFTER = 2001; /*OMP*/
const int OMP_MARK = 2002; /*OMP*/
const int OMP_NEXT = 2003; /*OMP*/
const int OMP_CRITICAL = 2004; /*OMP*/
int isOmpGetNumThreads(SgExpression *); /*OMP*/
SgStatement * GetLexNextIgnoreOMP(SgStatement *st); /*OMP*/
SgSymbol *ChangeParallelDir (SgStatement *stmt); /*OMP*/
void ChangeAccrossOpenMPParam (SgStatement *stmt, SgSymbol *newj, int ub); /*OMP*/
void MarkOriginalStmt (SgStatement *func); /*OMP*/
void TranslateFileOpenMPDVM(SgFile *f); /*OMP*/
void DelAttributeFromStmt (int type, SgStatement *st); /*OMP*/
int isOmpDir (SgStatement * st); /*OMP*/
void ConvertLoopWithLabelToEnddoLoop (SgStatement *stat); /*OMP*/
#define BIT_OPENMP    1024*128*128     /* OpenMP Fortran */

// options on FDVM converter
enum OPTIONS {
    AUTO_TFM = 0, ONE_THREAD, SPEED_TEST_L0, SPEED_TEST_L1, GPU_O0, GPU_O1, RTC, C_CUDA, OPT_EXP_COMP,
    O_HOST, NO_CUDA, NO_BL_INFO, LOOP_ANALYSIS, PRIVATE_ANALYSIS, IO_RTS, READ_ALL, NO_REMOTE, NO_PURE_FUNC, 
    GPU_IRR_ACC, NUM_OPT};
// ONE_THREAD - compile one thread CUDA-kernels only for across (TODO for all CUDA-kernels)
// SPEED_TEST_L0, SPEED_TEST_L1 - debug options for speed testof CUDA-kernels for across
// RTC - enable CUDA run-time compilation of all CUDA-kernels
// C_CUDA - enable Fortran to C convertation on CUDA-kernels
// OPT_EXP_COMP - compute expressions on loop analysis, it enables LOOP_ANALYSIS
// O_HOST - enable procedure optimizations for CPU
// NO_CUDA - dont generate CUDA-kernels
// NO_BL_INFO (depricated) - dont generate block information for CUDA-kernels's grid 
// LOOP_ANALYSIS - enable loop analysis for all parallel loops (default: only for across parallel loops)
// PRIVATE_ANALYSIS - enable private analysis for all parallel loops
// IO_RTS - enable new interface for parallel IO operations on DVMH programs
// READ_ALL - READ statement execution by all processes
// NO_REMOTE - ignore REMOTE_ACCESS specifications (compilation mode for single processor execution)
// NUM_OPT - it is not an option, it is a maximum value of enum

class Options
{
private:
    bool states[NUM_OPT];
    int values[NUM_OPT];
    bool freezed;
public:
    Options()
    {
        for (int i = 0; i < NUM_OPT; ++i)
        {
            states[i] = false;
            values[i] = -1;
        }
        freezed = false;
    }
    void setOn(const OPTIONS opt) 
    { 
        if (!freezed)
            states[opt] = true; 
    }
    void setOff(const OPTIONS opt) 
    {
        if (!freezed) {
            states[opt] = false;
            values[opt] = -1;
        }
    }
    void setValue(const OPTIONS opt, const int val) 
    { 
        if (!freezed) {
            states[opt] = true;
            values[opt] = val;
        }
    }

    bool isOn(const OPTIONS opt) const    { return states[opt]; }
    int getValue(const OPTIONS opt) const { return values[opt]; }

    void checkCombinations()
    {
        if (states[SPEED_TEST_L1])
            states[SPEED_TEST_L0] = true;
        // TODO: add warning
        if (states[ONE_THREAD])
            states[AUTO_TFM] = false;

        if (states[GPU_O0]) {
            fprintf(stderr, "switch off -gpuO0 option, this is not implemented yet\n");
            states[GPU_O0] = false;
        }

        if (states[GPU_IRR_ACC])
        {
            if (states[NO_CUDA] || !states[NO_BL_INFO])
            {
                states[LOOP_ANALYSIS] = false;
                states[OPT_EXP_COMP] = false;
                states[GPU_IRR_ACC] = false;
                if (states[NO_CUDA])
                    fprintf(stderr, "switch off -dvmIrregAnalysis option because -noCuda option is on\n");
                else
                    fprintf(stderr, "switch off -dvmIrregAnalysis option because -noBI option is off\n");
            }
            else
            {
                states[LOOP_ANALYSIS] = true;
                states[OPT_EXP_COMP] = true;
            }
        }
        //freeze all changes after initialization
        freezed = true;
    }
};

extern Options options;