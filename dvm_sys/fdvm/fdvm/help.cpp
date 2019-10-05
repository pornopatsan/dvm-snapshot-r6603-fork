/**************************************************************\
* Fortran DVM                                                  *
*                                                              *
*            Miscellaneous help routines                       *
\**************************************************************/

#include "dvm.h"
#include <ctype.h>
#include <stdlib.h>
extern "C" PTR_SYMB last_file_symbol;
//*************************************************************
/*
* Error - formats the error message then call "err" to print it
*
* input:
*	  s - string that specifies the conversion format
*	  t - string that to be formated according to s
*        num  - error message number
*        stmt - pointer to the statement
*/
//*************************************************************
void Error(const char *s, const char *t, int num, SgStatement *stmt)

{
    char *buff = new char[strlen(t) + strlen(s) + 8];
    sprintf(buff, s, t);
    err(buff, num, stmt);

    delete []buff;
}

/*
* Err_g - formats and prints the special kind error message (without statement reference)
*
* input:
*	  s - string that specifies the conversion format
*	  t - string that to be formated according to s
*        num  - error message number
*/

void Err_g(const char *s, const char *t, int num)

{
    char *buff = new char[strlen(t) + strlen(s) + 8];
    char num3s[4];
    sprintf(buff, s, t);
    format_num(num, num3s);
    errcnt++;
    (void)fprintf(stderr, "Error %s in   %s  of %s: %s\n", num3s, cur_func->symbol()->identifier(), cur_func->fileName(), buff);
    delete []buff;
}

/*
* err_p -- prints the special kind error message (with procedure reference)
*
* input:
*	  s - string to be printed out
*        num  - error message number
*        name - procedure identifier
*/
void err_p(const char *s, const char *name, int num)

{
    char num3s[4];
    format_num(num, num3s);
    errcnt++;
    
    (void)fprintf(stderr, "Error %s in procedure %s: %s \n", num3s, name, s);
}

/*
* err -- prints the error message
*
* input:
*	  s - string to be printed out
*        num  - error message number
*        stmt - pointer to the statement
*/
void err(const char *s, int num, SgStatement *stmt)

{
    char num3s[4];
    format_num(num, num3s);
    errcnt++;
    //  printf( "Error on line %d : %s\n", stmt->lineNumber(),  s);
    (void)fprintf(stderr, "Error %s on line %d of %s: %s\n", num3s, stmt->lineNumber(), stmt->fileName(), s);
}

/*
* Warning -- formats a warning message then call "warn" to print it out
*
* input:
*	  s - string that specifies the conversion format
*	  t - string that to be converted according to s
*        num  - warning message number
*        stmt - pointer to the statement
*/
void Warning(const char *s, const char *t, int num, SgStatement *stmt)
{
    char *buff = new char[strlen(t) + strlen(s) + 8];
    sprintf(buff, s, t);
    warn(buff, num, stmt);

    delete []buff;
}

/*
* warn -- print the warning message if specified
*
* input:
*	  s - string to be printed
*        num  - warning message number
*        stmt - pointer to the statement
*/
void warn(const char *s, int num, SgStatement *stmt)
{
    char num3s[4];
    format_num(num, num3s);
    // printf( "Warning on line %d: %s\n", stmt->lineNumber(), s);
    (void)fprintf(stderr, "Warning %s on line %d of %s: %s\n", num3s, stmt->lineNumber(), stmt->fileName(), s);

}

void Warn_g(const char *s, const char *t, int num)
{
    char *buff = new char[strlen(t) + strlen(s) + 8];
    char num3s[4];
    format_num(num, num3s);
    sprintf(buff, s, t);
    (void)fprintf(stderr, "Warning %s in   %s  of %s: %s\n", num3s, cur_func->symbol()->identifier(), cur_func->fileName(), buff);
    delete []buff;
}

//*********************************************************************
void printVariantName(int i)
{
    if ((i >= 0 && i < MAXTAGS) && tag[i]) 
        printf("%s", tag[i]);
    else 
        printf("not a known node variant");
}
//***********************************

#define BUFLEN 5000
char buffer[BUFLEN], *bp;
#define binop(n)	(n >= EQ_OP && n <= NEQV_OP)

static const char *fop_name[] = {
    " .eq. ",
    " .lt. ",
    " .gt. ",
    " .ne. ",
    " .le. ",
    " .ge. ",
    " + ",
    " - ",
    " .or. ",
    " * ",
    " / ",
    "",
    " .and. ",
    "**",
    "",
    " // ",
    " .xor. ",
    " .eqv. ",
    " .neqv. "
};


/*
* Precedence table of operators for Fortran
*/
static char  precedence[] = {	/* precedence table of the operators */
    5,		/* .eq. */
    5,		/* .lt. */
    5,		/* .gt. */
    5,		/* .ne. */
    5,		/* .le. */
    5,		/* .ge. */
    3,		/*  +   */
    3,		/*  -   */
    8,		/* .or. */
    2,		/*  *   */
    2,		/*  /   */
    0,		/* none */
    7,		/* .and. */
    1,		/*  **  */
    0,		/* none */
    4,		/*  //  */
    8,		/* .xor. */
    9,		/* .eqv. */
    9		/* .neqv. */
};


/*
* Type names in ascii form
*/
/*static const char *ftype_name[] = {
    "integer",
    "real",
    "double precision",
    "character",
    "logical",
    "character",
    "gate",
    "event",
    "sequence",
    "",
    "",
    "",
    "",
    "complex",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "double complex"
};*/

/****************************************************************
*								*
*  addstr -- add the string "s" to output buffer		*
*								*
*  Input:							*
*	   s  - the string to be appended to the buffer		*
*								*
*  Side effect:						*
*	   bp - points to where next character will go		*
*								*
****************************************************************/
void addstr(const char *s)
{
    while ((*bp = *s++) != 0)
        bp++;
}

/****************************************************************
*								*
*  unp_llnd -- unparse the given low level node to source	*
*		string						*
*								*
*  Input:							*
*	   pllnd - low level node to be unparsed		*
*	   bp (implicitely) - where the output string to be	*
*		   placed					*
*								*
*  Output:							*
*	   the unparse string where "bp" was pointed to		*
*								*
*  Side Effect:						*
*	   "bp" will be updated to the next character behind	*
*	   the end of the unparsed string (by "addstr")		*
*								*
****************************************************************/
void unp_llnd(PTR_LLND pllnd)
{
    if (pllnd == NULL) 
        return;

    switch (pllnd->variant)
    {
    case INT_VAL:
    { char sb[64];

    sprintf(sb, "%d", pllnd->entry.ival);
    addstr(sb);
    break;
    }
    case LABEL_REF:
    { char sb[64];

    sprintf(sb, "%d", (int)pllnd->entry.label_list.lab_ptr->stateno);
    addstr(sb);
    break;
    }
    case FLOAT_VAL:
    case DOUBLE_VAL:
    case STMT_STR:
        addstr(pllnd->entry.string_val);
        break;
    case STRING_VAL:
        *bp++ = '\'';
        addstr(pllnd->entry.string_val);
        *bp++ = '\'';
        break;
    case COMPLEX_VAL:
        *bp++ = '(';
        unp_llnd(pllnd->entry.Template.ll_ptr1);
        *bp++ = ',';
        unp_llnd(pllnd->entry.Template.ll_ptr2);
        *bp++ = ')';
        break;
    case KEYWORD_VAL:
        addstr(pllnd->entry.string_val);
        break;
    case KEYWORD_ARG:
        unp_llnd(pllnd->entry.Template.ll_ptr1);
        addstr("=");
        unp_llnd(pllnd->entry.Template.ll_ptr2);
        break;
    case BOOL_VAL:
        if (pllnd->entry.bval)
            addstr(".TRUE.");
        else
            addstr(".FALSE.");
        break;
    case CHAR_VAL:
        /* if (! in_impli)  */
        *bp++ = '\'';
        *bp++ = pllnd->entry.cval;
        /* if (! in_impli)  */
        *bp++ = '\'';
        break;
    case CONST_REF:
    case VAR_REF:
    case ENUM_REF:
    case TYPE_REF:
    case INTERFACE_REF:
        addstr(pllnd->entry.Template.symbol->ident);
        /* Look out !!!! */
        /* Purpose unknown. Commented out. */
        /*
        if (pllnd->entry.Template.symbol->type->entry.Template.ranges != LLNULL)
        unp_llnd(pllnd->entry.Template.symbol->type->entry.Template.ranges);
        */
        break;
    case ARRAY_REF:
        addstr(pllnd->entry.array_ref.symbol->ident);
        if (pllnd->entry.array_ref.index) {
            *bp++ = '(';
            unp_llnd(pllnd->entry.array_ref.index);
            *bp++ = ')';
        }
        break;
    case ARRAY_OP:
        unp_llnd(pllnd->entry.Template.ll_ptr1);
        *bp++ = '(';
        unp_llnd(pllnd->entry.Template.ll_ptr2);
        *bp++ = ')';
        break;
    case RECORD_REF:
        unp_llnd(pllnd->entry.Template.ll_ptr1);
        addstr("%");
        unp_llnd(pllnd->entry.Template.ll_ptr2);
        break;
    case STRUCTURE_CONSTRUCTOR:
        addstr(pllnd->entry.Template.symbol->ident);
        *bp++ = '(';
        unp_llnd(pllnd->entry.Template.ll_ptr1);
        *bp++ = ')';
        break;
    case CONSTRUCTOR_REF:
        addstr("(/");
        unp_llnd(pllnd->entry.Template.ll_ptr1);
        addstr("/)");
        break;
    case ACCESS_REF:
        unp_llnd(pllnd->entry.access_ref.access);
        if (pllnd->entry.access_ref.index != NULL) {
            *bp++ = '(';
            unp_llnd(pllnd->entry.access_ref.index);
            *bp++ = ')';
        }
        break;
    case OVERLOADED_CALL:
        break;
    case CONS:
        unp_llnd(pllnd->entry.Template.ll_ptr1);
        addstr(",");
        unp_llnd(pllnd->entry.Template.ll_ptr2);
        break;
    case ACCESS:
        unp_llnd(pllnd->entry.access.array);
        addstr(", FORALL=(");
        addstr(pllnd->entry.access.control_var->ident);
        *bp++ = '=';
        unp_llnd(pllnd->entry.access.range);
        *bp++ = ')';
        break;
    case IOACCESS:
        *bp++ = '(';
        unp_llnd(pllnd->entry.ioaccess.array);
        addstr(", ");
        addstr(pllnd->entry.ioaccess.control_var->ident);
        *bp++ = '=';
        unp_llnd(pllnd->entry.ioaccess.range);
        *bp++ = ')';
        break;
    case PROC_CALL:
    case FUNC_CALL:
        addstr(pllnd->entry.proc.symbol->ident);
        *bp++ = '(';
        unp_llnd(pllnd->entry.proc.param_list);
        *bp++ = ')';
        break;
    case EXPR_LIST:
        unp_llnd(pllnd->entry.list.item);
        /* if (in_param) {
        addstr("=");
        unp_llnd(pllnd->entry.list.item->entry.const_ref.symbol->entry.const_value);
        }
        */
        if (pllnd->entry.list.next) {
            addstr(",");
            unp_llnd(pllnd->entry.list.next);
        }
        break;
    case EQUI_LIST:
        *bp++ = '(';
        unp_llnd(pllnd->entry.list.item);
        *bp++ = ')';
        if (pllnd->entry.list.next) {
            addstr(", ");
            unp_llnd(pllnd->entry.list.next);
        }
        break;
    case COMM_LIST:
    case NAMELIST_LIST:
        if (pllnd->entry.Template.symbol) {
            *bp++ = '/';
            addstr(pllnd->entry.Template.symbol->ident);
            *bp++ = '/';
        }
        unp_llnd(pllnd->entry.list.item);
        if (pllnd->entry.list.next) {
            addstr(", ");
            unp_llnd(pllnd->entry.list.next);
        }
        break;
    case VAR_LIST:
    case RANGE_LIST:
    case CONTROL_LIST:
        unp_llnd(pllnd->entry.list.item);
        if (pllnd->entry.list.next) {
            addstr(",");
            unp_llnd(pllnd->entry.list.next);
        }
        break;
    case DDOT:
        if (pllnd->entry.binary_op.l_operand)
            unp_llnd(pllnd->entry.binary_op.l_operand);
        *bp++ = ':';
        if (pllnd->entry.binary_op.r_operand)
            unp_llnd(pllnd->entry.binary_op.r_operand);
        break;
    case DEFAULT:
        addstr("default");
        break;
    case DEF_CHOICE:
    case SEQ:
        unp_llnd(pllnd->entry.seq.ddot);
        if (pllnd->entry.seq.stride) {
            *bp++ = ':';
            unp_llnd(pllnd->entry.seq.stride);
        }
        break;
    case SPEC_PAIR:
        unp_llnd(pllnd->entry.spec_pair.sp_label);
        *bp++ = '=';
        unp_llnd(pllnd->entry.spec_pair.sp_value);
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
    {
                      int i = pllnd->variant - EQ_OP, j;
                      PTR_LLND p;
                      int num_paren = 0;

                      p = pllnd->entry.binary_op.l_operand;
                      j = p->variant;
                      if (binop(j) && precedence[i] < precedence[j - EQ_OP]) {
                          num_paren++;
                          *bp++ = '(';
                      }
                      unp_llnd(p);
                      if (num_paren) {
                          *bp++ = ')';
                          num_paren--;
                      }
                      addstr(fop_name[i]); /* print the op name */
                      p = pllnd->entry.binary_op.r_operand;
                      j = p->variant;
                      if (binop(j) && precedence[i] <= precedence[j - EQ_OP]) {
                          num_paren++;
                          *bp++ = '(';
                      }
                      unp_llnd(p);
                      if (num_paren) {
                          *bp++ = ')';
                          num_paren--;
                      }
                      break;
    }
    case MINUS_OP:
        addstr(" -(");
        unp_llnd(pllnd->entry.unary_op.operand);
        *bp++ = ')';
        break;
    case UNARY_ADD_OP:
        addstr(" +(");
        unp_llnd(pllnd->entry.unary_op.operand);
        *bp++ = ')';
        break;
    case NOT_OP:
        addstr(" .not. (");
        unp_llnd(pllnd->entry.unary_op.operand);
        *bp++ = ')';
        break;
    case PAREN_OP:
        addstr("(");
        unp_llnd(pllnd->entry.Template.ll_ptr1);
        addstr(")");
    case ASSGN_OP:
        addstr("=");
        unp_llnd(pllnd->entry.Template.ll_ptr1);
    case STAR_RANGE:
        addstr(" : ");
        break;
    case OMP_THREADPRIVATE: /*OMP*/
        addstr(" / "); /*OMP*/
        unp_llnd(pllnd->entry.Template.ll_ptr1); /*OMP*/
        addstr(" / "); /*OMP*/
        break; /*OMP*/
        /*	case IMPL_TYPE:
        pr_ftype_name(pllnd->type, 1);
        if (pllnd->entry.Template.ll_ptr1 != LLNULL)
        {
        addstr("(");
        unp_llnd(pllnd->entry.Template.ll_ptr1);
        addstr(")");
        }
        break;
        */
        /*
        case ORDERED_OP :
        addstr("ordered ");
        break;
        case EXTEND_OP :
        addstr("extended ");
        break;
        case MAXPARALLEL_OP:
        addstr("max parallel = ");
        unp_llnd(pllnd->entry.Template.ll_ptr1);
        break;
        case PARAMETER_OP :
        addstr("parameter ");
        break;
        case PUBLIC_OP :
        addstr("public ");
        break;
        case PRIVATE_OP :
        addstr("private ");
        break;
        case ALLOCATABLE_OP :
        addstr("allocatable ");
        break;
        case DIMENSION_OP :
        addstr("dimension (");
        unp_llnd(pllnd->entry.Template.ll_ptr1);
        addstr(")");
        break;
        case EXTERNAL_OP :
        addstr("external ");
        break;
        case OPTIONAL_OP :
        addstr("optional ");
        break;
        case IN_OP :
        addstr("intent (in) ");
        break;
        case OUT_OP :
        addstr("intent (out) ");
        break;
        case INOUT_OP :
        addstr("intent (inout) ");
        break;
        case INTRINSIC_OP :
        addstr("intrinsic ");
        break;
        case POINTER_OP :
        addstr("pointer ");
        break;
        case SAVE_OP :
        addstr("save ");
        break;
        case TARGET_OP :
        addstr("target ");
        break;
        */
    case LEN_OP:
        addstr("*");
        unp_llnd(pllnd->entry.Template.ll_ptr1);
        break;
        /*	case TYPE_OP :
        pr_ftype_name(pllnd->type, 1);
        unp_llnd(pllnd->type->entry.Template.ranges);
        break;
        */
        /*
        case ONLY_NODE :
        addstr("only: ");
        if (pllnd->entry.Template.ll_ptr1)
        unp_llnd(pllnd->entry.Template.ll_ptr1);
        break;
        case DEREF_OP :
        unp_llnd(pllnd->entry.Template.ll_ptr1);
        break;
        case RENAME_NODE :
        unp_llnd(pllnd->entry.Template.ll_ptr1);
        addstr("=>");
        unp_llnd(pllnd->entry.Template.ll_ptr2);
        break;
        case VARIABLE_NAME :
        addstr(pllnd->entry.Template.symbol->ident);
        break;
        */
    default:
        fprintf(stderr, "Error: unp_llnd -- bad llnd_ptr %d!\n", pllnd->variant);
        break;
    }
}

/****************************************************************
*								*
*  funparse_llnd -- unparse the low level node for Fortran	*
*								*
*  input:							*
*	   llnd -- the node to be unparsed			*
*								*
*  output:							*
*	   the unparsed string					*
*								*
****************************************************************/
char* funparse_llnd(PTR_LLND llnd)
{
    int len;
    char *p;

    bp = buffer;	/* reset the buffer pointer */
    unp_llnd(llnd);
    /*  *bp++ = '\n'; */
    *bp++ = '\0';
    len = (bp - buffer) + 1; /* calculate the string length */
    p = (char *)malloc(len);	/* allocate space for returned value */
    strcpy(p, buffer); 	/* copy the buffer for output */
    *buffer = '\0';
    return p;
}

char *UnparseExpr(SgExpression *e)
{
    char *buf;

    if (isSgVarRefExp(e) || (isSgArrayRefExp(e) && (!(e->lhs()) || d_no_index)))
        return (e->symbol()->identifier());

    buf = funparse_llnd(e->thellnd);
    return buf;
}
/*
char *UnparseExpr(SgExpression *e)
{char *buf;

int l;
if(isSgVarRefExp(e) || (isSgArrayRefExp(e) && !(e->lhs())))
return (e->symbol()->identifier());
Init_Unparser();
buf = Tool_Unparse2_LLnode(e->thellnd);
l = strlen(buf);
char *ustr = new char[l+1];
strcpy(ustr,buf);
//ustr[l]   = ' ';
//ustr[l+1] = '\0';
return(ustr);
}
*/
//************************************

const char* header(int i)
{
    switch (i) 
    {
    case(PROG_HEDR) :
        return("program");
    case(PROC_HEDR) :
        return("subroutine");
    case(FUNC_HEDR) :
        return("function");
    default:
        return("error");
    }
}

SgLabel*  firstLabel(SgFile *f)
{
    SetCurrentFileTo(f->filept);
    SwitchToFile(GetFileNumWithPt(f->filept));
    return LabelMapping(PROJ_FIRST_LABEL());
}

int isLabel(int num) 
{
    PTR_LABEL lab;
    for (lab = PROJ_FIRST_LABEL(); lab; lab = LABEL_NEXT(lab))
    if (num == LABEL_STMTNO(lab))
        return 1;
    return 0;
}

SgLabel* GetLabel()
{
    static int lnum = 90000;
    if (lnum>max_lab)
        return (new SgLabel(lnum--));
    while (isLabel(lnum))
        lnum--;
    return (new SgLabel(lnum--));
}
/*
int FragmentList(char *l, int level)
{char ch[10],*str,*p;
int num;
D_fragment *fr;
str = l;
p = ch;
cur_num:
for(; (*str != '\0' &&  *str != ','); str++)
if(isdigit(*str))
*p++ = *str;
else
return(0);
*p = '\0';
num = atoi(p);
fr = new D_fragment;
fr->next = NULL;
fr->No = num;
if(num == 0) {
fr->next =  deb[level];
deb[level] = fr;
} else
if(!deb[level]){
fr->next = NULL;
deb[level] = fr;
} else {
fr->next =  deb[level]->next;
deb[level] ->next = fr;
}

if(*str == '\0')
return(1);

str = str+1;
goto cur_num;

return(1);
}


int FragmentList(char *l, int dlevel, int elevel)
{char ch[10],*str,*p;
int num,num1;
str = l;
num1 =0;
cur_num:
p = ch;
if(!isdigit(*str)) return(0);
for(; (*str != '\0' &&  *str != ',' &&  *str != '-'); str++)
if(isdigit(*str))
*p++ = *str;
else
//return(0);
break;
*p = '\0';
num = atoi(ch);
if(*str == '-')
num1 = num;
else
if(num1){
AddToFragmentList(num1,num,dlevel,elevel);
num1 =0;
}
else
AddToFragmentList(num,num,dlevel,elevel);

if(*str == '\0')
return(1);
if(*str != ',' && *str != '-')
return(0);
str = str+1;
goto cur_num;

}
*/

int FragmentList(char *l, int dlevel, int elevel)
{
    char ch[10], *str, *p;
    int num, num1;
    str = l;
    num1 = 0;
cur_num:
    p = ch;
    if (!isdigit(*str)) return(0);
    for (; (*str != '\0' && *str != ',' && *str != '-'); str++)
    if (isdigit(*str))
        *p++ = *str;
    else
        //return(0);
        break;
    *p = '\0';
    num = atoi(ch);
    if (*str == '-')
        num1 = num;
    else
    if (num1){
        AddToFragmentList(num1, num, dlevel, elevel);
        num1 = 0;
    }
    else
        AddToFragmentList(num, num, dlevel, elevel);

    if (*str == '\0')
        return(1);
    if (*str != ',' && *str != '-')
        return(0);
    str = str + 1;
    goto cur_num;

}
/*
void AddToFragmentList(int num,int dlevel,int elevel)
{ fragment_list *fr;
if(dlevel == 0 && elevel == 0)
return;
if(!debug_fragment) {
debug_fragment = new fragment_list;
debug_fragment->No = num;
debug_fragment->next = NULL;
debug_fragment->dlevel = dlevel;
debug_fragment->elevel = elevel;
} else {
for(fr= debug_fragment; fr; fr=fr->next)
if(fr->No == num) {
if(dlevel != 0)
fr->dlevel = dlevel;
if(elevel != 0)
fr->elevel = elevel;
return;
}
fr = new fragment_list;
fr->No = num;
fr->dlevel = dlevel;
fr->elevel = elevel;
fr->next =  debug_fragment;
debug_fragment = fr;
}
return;
}

void AddToFragmentList(int num1, int num2, int dlevel, int elevel)
{ fragment_list_in *fr;
if(dlevel == 0 && elevel == 0)
return;
fr = new fragment_list_in;
fr->N1 = num1;
fr->N2 = num2;
fr->dlevel = dlevel;
fr->elevel = elevel;
fr->next =  debug_fragment;
debug_fragment = fr;
return;
}
*/

void AddToFragmentList(int num1, int num2, int dlevel, int elevel)
{
    fragment_list_in *fr;
    if (dlevel == -1 && elevel == -1)
        return;
    fr = new fragment_list_in;
    fr->N1 = num1;
    fr->N2 = num2;
    if (elevel == -1) {
        fr->level = dlevel;
        fr->next = debug_fragment;
        debug_fragment = fr;
    }
    else {
        fr->level = elevel;
        fr->next = perf_fragment;
        perf_fragment = fr;
    }
    return;
}

/*
fragment_list_in *AddToFragmentList(int num1, int num2, int level, fragment_list_in *frlist)
{ fragment_list_in *fr;
if(level == 0)
return;
fr = new fragment_list_in;
fr->N1 = num1;
fr->N2 = num2;
fr->level = level;
fr->next =  frlist;
return(fr);
}
*/


void format_num(int num, char num3s[])
{
    if (num>99)
        sprintf(num3s, "%3d", num);
    else if (num>9)
        sprintf(num3s, "0%2d", num);
    else
        sprintf(num3s, "00%1d", num);
}

SgExpression* ConnectList(SgExpression *el1, SgExpression *el2)
{
    SgExpression *el;
    if (!el1)
        return(el2);
    if (!el2)
        return(el1);
    for (el = el1; el->rhs(); el = el->rhs())
        ;
    el->setRhs(el2);
    return(el1);
}

int is_integer_value(char *str)
{
    char *p;
    p = str;
    for (; *str != '\0'; str++)
    if (!isdigit(*str))
        return 0;
    return (atoi(p));
}

char* SymbListString(symb_list *symbl)
{
    symb_list *sl;
    int len;
    char *p;

    bp = buffer;	/* reset the buffer pointer */
    for (sl = symbl; sl; sl = sl->next)
    {
        if (sl != symbl)
            addstr(", ");
        addstr(sl->symb->identifier());
    }
    *bp++ = '\0';
    len = (bp - buffer) + 1; /* calculate the string length */
    p = (char *)malloc(len);	/* allocate space for returned value */
    strcpy(p, buffer); 	/* copy the buffer for output */
    *buffer = '\0';

    return p;
}

char * baseFileName(char *name)
{//removal the path from the filename 'name' 
   char *p=strrchr(name,'/');
   if(p)
      return (p+1);
   else if(p=strrchr(name,'\\'))
      return (p+1);
   else
      return(name);   
}

char *to_C_ident(char *name, bool allowFirstDigit)
{
  int l = strlen(name);
  for (int i = 0; i < l; i++) 
  {
        char c = name[i];
        if (!((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == '_' || ((i > 0 || allowFirstDigit) && c >= '0' && c <= '9')))
            name[i] = '_';
  }
  return name;
}

SgSymbol *isSameNameInProgramUnit(const char *name,SgStatement *func)
{
    SgSymbol *s, *until;
    SgStatement *last = func->lastNodeOfStmt();
    if(last->lexNext() && last->lexNext()->symbol())
       until = last->lexNext()->symbol();
    else
       until = SymbMapping(last_file_symbol)->next();
    for (s= func->symbol(); s && s!=until; s = s->next())
    {
        if (s && !strcmp(s->identifier(), name))
            return(s);
    }
    return(NULL);
}

char *Check_Correct_Name(const char *name)
{
    SgSymbol *s = NULL;
    char *ret = new char[strlen(name) + 1];
    strcpy(ret,name);
    while ((s = isSameNameInProgramUnit(ret,cur_func)))
    {
        ret = (char *)malloc((unsigned)(strlen(name) + 2));
        sprintf(ret, "%s_", s->identifier());
    }
    return ret;
}

