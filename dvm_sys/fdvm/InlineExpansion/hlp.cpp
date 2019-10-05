/**************************************************************\
* Inline Expansion                                             *
*                                                              *
*            Miscellaneous help routines                       *
\**************************************************************/

#include "inline.h"
#include <ctype.h>
#include <stdlib.h>
#ifdef __SPF
#include <string>
#endif

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
    char *buff = new char[strlen(s) + strlen(t) + 32];
    sprintf(buff, s, t);
    err(buff, num, stmt);
    delete[]buff;
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
    char *buff = new char[strlen(s) + strlen(t) + 32];
    char num3s[16];
    sprintf(buff, s, t);
    format_num(num, num3s);
    errcnt++;
    (void)fprintf(stderr, "Error %s in   %s  of %s: %s\n", num3s, cur_func->symbol()->identifier(), cur_func->fileName(), buff);
    delete[]buff;
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
    char num3s[16];
    format_num(num, num3s);
    errcnt++;
    //  printf( "Error on line %d : %s\n", stmt->lineNumber(),  s);
#ifdef __SPF
    char message[256];
    sprintf(message, "Error %d: %s", num, s);

    std::string toPrint = "|";
    toPrint += std::to_string(1) + " "; // ERROR
    toPrint += std::string(stmt->fileName()) + " ";
    toPrint += std::to_string(stmt->lineNumber()) + " ";
    toPrint += std::to_string(0);
    toPrint += "|" + std::string(message);

    printf("@%s@\n", toPrint.c_str());
#else
    (void)fprintf(stderr, "Error %s on line %d of %s: %s\n", num3s, stmt->lineNumber(), stmt->fileName(), s);
#endif
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
    char *buff = new char[strlen(s) + strlen(t) + 32];
    sprintf(buff, s, t);
    warn(buff, num, stmt);
    delete[]buff;
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
    char num3s[16];
    format_num(num, num3s);
    // printf( "Warning on line %d: %s\n", stmt->lineNumber(), s);
    (void)fprintf(stderr, "Warning %s on line %d of %s: %s\n", num3s, stmt->lineNumber(), stmt->fileName(), s);
}

void Warn_g(const char *s, const char *t, int num)
{
    char *buff = new char[strlen(s) + strlen(t) + 32];
    char num3s[16];
    format_num(num, num3s);
    sprintf(buff, s, t);
    (void)fprintf(stderr, "Warning %s in   %s  of %s: %s\n", num3s, cur_func->symbol()->identifier(), cur_func->fileName(), buff);
    delete[]buff;
}
//*********************************************************************
void printVariantName(int i) {
    if ((i >= 0 && i < MAXTAGS) && tag[i]) printf("%s", tag[i]);
    else printf("not a known node variant");
}
//***********************************

char *UnparseExpr(SgExpression *e)
{
    char *buf;
    int l;
    Init_Unparser();
    buf = Tool_Unparse2_LLnode(e->thellnd);
    l = strlen(buf);
    char *ustr = new char[l + 1];
    strcpy(ustr, buf);
    //ustr[l]   = ' ';
    //ustr[l+1] = '\0';
    return(ustr);
}
//************************************

const char* header(int i) {
    switch (i) {
    case(PROG_HEDR):
        return("program");
    case(PROC_HEDR):
        return("subroutine");
    case(FUNC_HEDR):
        return("function");
    default:
        return("error");
    }
}

SgLabel* firstLabel(SgFile *f)
{
    SetCurrentFileTo(f->filept);
    SwitchToFile(GetFileNumWithPt(f->filept));
    return LabelMapping(PROJ_FIRST_LABEL());
}

int isLabel(int num) {
    PTR_LABEL lab;
    for (lab = PROJ_FIRST_LABEL(); lab; lab = LABEL_NEXT(lab))
        if (num == LABEL_STMTNO(lab))
            return 1;
    return 0;
}

SgLabel *isLabelWithScope(int num, SgStatement *stmt) {
    PTR_LABEL lab;
    for (lab = PROJ_FIRST_LABEL(); lab; lab = LABEL_NEXT(lab))
        //if( num == LABEL_STMTNO(lab) && LABEL_BODY(lab)->scope == stmt->thebif)
        if (num == LABEL_STMTNO(lab) && LABEL_SCOPE(lab) == stmt->thebif)
            return LabelMapping(lab);
    return NULL;
}


SgLabel * GetLabel()
{
    static int lnum = 90000;
    if (lnum > max_lab)
        return (new SgLabel(lnum--));
    while (isLabel(lnum))
        lnum--;
    return (new SgLabel(lnum--));
}

SgLabel * GetNewLabel()
{
    static int lnum = 99999;
    if (lnum > max_lab) /*  for current file must be set before first call GetNewLabel() :max_lab = getLastLabelId(); */
        return (new SgLabel(lnum--));
    while (isLabel(lnum))
        lnum--;
    return (new SgLabel(lnum--));
    /*
      int lnum;
      if(max_lab <99999)
         return(new SgLabel(++max_lab));
      lnum = 1;
      while(isLabel(lnum))
        lnum++;
      return(new SgLabel(lnum));
    */
}

SgLabel * NewLabel()
{
    if (max_lab < 99999)
        return(new SgLabel(++max_lab));
    ++num_lab;
    while (isLabel(num_lab))
        ++num_lab;
    return(new SgLabel(num_lab));
}

void SetScopeOfLabel(SgLabel *lab, SgStatement *scope)
{
    LABEL_SCOPE(lab->thelabel) = scope->thebif;
}

/*
SgLabel * NewLabel(int lnum)
{
  if(max_lab <99999)
     return(new SgLabel(++max_lab));

  while(isLabel(lnum))
    ++lnum;
  return(new SgLabel(lnum));
}
*/

int isSymbolName(char *name)
//
{
    SgSymbol *s;
    for (s = current_file->firstSymbol(); s; s = s->next())
        if (!strcmp(name, s->identifier()))
            return 1;
    return 0;
}

int isSymbolNameInScope(char *name, SgStatement *scope)
{
    SgSymbol *s;
    for (s = current_file->firstSymbol(); s; s = s->next())
        if (scope == s->scope() && !strcmp(name, s->identifier()))
            return 1;
    return 0;
}
/*
{
  PTR_SYMB sym;
  for(sym=PROJ_FIRST_SYMB(); sym; sym=SYMB_NEXT(sym))
     if( SYMB_SCOPE(sym) == scope->thebif && (!strcmp(name,SYMB_IDENT(sym)) ) )
       return 1;
  return 0;
}
*/

void format_num(int num, char num3s[])
{
    if (num > 99)
        num3s[sprintf(num3s, "%3d", num)] = 0;
    else if (num > 9)
        num3s[sprintf(num3s, "0%2d", num)] = 0;
    else
        num3s[sprintf(num3s, "00%1d", num)] = 0;
}

SgExpression *ConnectList(SgExpression *el1, SgExpression *el2)
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

void PrintSymbolTable(SgFile *f)
{
    SgSymbol *s;
    printf("\nS Y M B O L   T A B L E \n");
    for (s = f->firstSymbol(); s; s = s->next())
        //printf(" %s/%d/     ", s->identifier(), s->id() );
        printSymb(s);
}

void printSymb(SgSymbol *s)
{
    const char *head;
    head = isHeaderStmtSymbol(s) ? "HEADER  " : "        ";
    printf("SYMB[%3d]  scope=STMT[%3d] : %s    %s", s->id(), (s->scope()) ? (s->scope())->id() : -1, s->identifier(), head);
    printType(s->type());
    printf("\n");
}

void printType(SgType *t)
{
    SgArrayType *arrayt;
    /*SgExpression *e = new SgExpression(TYPE_RANGES(t->thetype));*/
    int i, n;
    if (!t) { printf("no type "); return; }
    else   printf("TYPE[%d]:", t->id());
    if ((arrayt = isSgArrayType(t)) != 0)
    {
        printf("dimension(");
        n = arrayt->dimension();
        for (i = 0; i < n; i++)
        {
            (arrayt->sizeInDim(i))->unparsestdout();
            if (i < n - 1)  printf(", ");
        }
        printf(") ");
    }
    else
    {
        switch (t->variant())
        {
        case  T_INT:      printf("integer "); break;
        case  T_FLOAT:    printf("real "); break;
        case  T_DOUBLE:   printf("double precision "); break;
        case  T_CHAR:     printf("character "); break;
        case  T_STRING:   printf("Character ");
            UnparseLLND(TYPE_RANGES(t->thetype));
            /*if(t->length()) printf("[%d]",t->length()->variant());*/
            /*((SgArrayType *) t)->getDimList()->unparsestdout();*/
            break;
        case  T_BOOL:     printf("logical "); break;
        case  T_COMPLEX:  printf("complex "); break;
        case  T_DCOMPLEX:  printf("double complex "); break;

        default: break;
        }
    }
    /*  if(e) e->unparsestdout();*/
    if (t->hasBaseType())
    {
        printf("of ");
        printType(t->baseType());
    }
}

void PrintTypeTable(SgFile *f)
{
    SgType *t;
    printf("\nT Y P E   T A B L E \n");
    for (t = f->firstType(); t; t = t->next())
    {
        printType(t); printf("\n");
    }

}

SgExpression *ReplaceParameter(SgExpression *e)
{
    if (!e)
        return(e);
    if (e->variant() == CONST_REF) {
        SgConstantSymb * sc = isSgConstantSymb(e->symbol());
        return(ReplaceParameter(&(sc->constantValue()->copy())));
    }
    e->setLhs(ReplaceParameter(e->lhs()));
    e->setRhs(ReplaceParameter(e->rhs()));
    return(e);
}

SgExpression *ReplaceIntegerParameter(SgExpression *e)
{
    if (!e)
        return(e);
    if (e->variant() == CONST_REF && e->type()->variant() == T_INT) {
        SgConstantSymb * sc = isSgConstantSymb(e->symbol());
        return(ReplaceIntegerParameter(&(sc->constantValue()->copy())));
    }
    e->setLhs(ReplaceIntegerParameter(e->lhs()));
    e->setRhs(ReplaceIntegerParameter(e->rhs()));
    return(e);
}

/*
SgExpression *ReplaceFuncCall(SgExpression *e)
{
  if(!e)
    return(e);
  if(isSgFunctionCallExp(e) && e->symbol()) {//function call
     if( !e->lhs()  && (!strcmp(e->symbol()->identifier(),"number_of_processors") || !strcmp(e->symbol()->identifier(),"actual_num_procs"))) {             //NUMBER_OF_PROCESSORS() or
                                                              // ACTUAL_NUM_PROCS()
    SgExprListExp *el1,*el2;
    if(!strcmp(e->symbol()->identifier(),"number_of_processors"))
      el1 = new SgExprListExp(*ParentPS());
    else
      el1 = new SgExprListExp(*CurrentPS());
    el2 = new SgExprListExp(*ConstRef(0));
    e->setSymbol(fdvm[GETSIZ]);
    fmask[GETSIZ] = 1;
    el1->setRhs(el2);
    e->setLhs(el1);
    return(e);
    }

   if( !e->lhs() && (!strcmp(e->symbol()->identifier(),"processors_rank"))) {
                                                                //PROCESSORS_RANK()
    SgExprListExp *el1;
    el1 = new SgExprListExp(*ParentPS());
    e->setSymbol(fdvm[GETRNK]);
    fmask[GETRNK] = 1;
    e->setLhs(el1);
    return(e);
    }

   if(!strcmp(e->symbol()->identifier(),"processors_size")) {
                                                               //PROCESSORS_SIZE()
    SgExprListExp *el1;
    el1 = new SgExprListExp(*ParentPS());
    e->setSymbol(fdvm[GETSIZ]);
    fmask[GETSIZ] = 1;
    el1->setRhs(*(e->lhs())+(*ConstRef(0)));  //el1->setRhs(e->lhs());
    e->setLhs(el1);
    return(e);
   }
  }
  e->setLhs(ReplaceFuncCall(e->lhs()));
  e->setRhs(ReplaceFuncCall(e->rhs()));
  return(e);
}
*/

/* version from dvm.cpp
SgExpression *Calculate(SgExpression *e)
{ SgExpression *er;
   er  = ReplaceParameter( &(e->copy()));
   if(er->isInteger())
      return( new SgValueExp(er->valueInteger()));
    else
      return(e);
}
*/

/* new version */
SgExpression *Calculate(SgExpression *e)
{
    if (e->isInteger())
        return(new SgValueExp(e->valueInteger()));
    else
        return(e);
}


SgExpression *Calculate_List(SgExpression *e)
{
    SgExpression *el;
    for (el = e; el; el = el->rhs())
        el->setLhs(Calculate(el->lhs()));
    return(e);
}


int ExpCompare(SgExpression *e1, SgExpression *e2)
{//compares two expressions
// returns 1 if they are textually identical
    if (!e1 && !e2) // both expressions are null
        return(1);
    if (!e1 || !e2) // one of them is null
        return(0);
    if (e1->variant() != e2->variant()) // variants are not equal
        return(0);
    switch (e1->variant()) {
    case INT_VAL:
        return(NODE_IV(e1->thellnd) == NODE_IV(e2->thellnd));
    case FLOAT_VAL:
    case DOUBLE_VAL:
    case BOOL_VAL:
    case CHAR_VAL:
    case STRING_VAL:
        return(!strcmp(NODE_STR(e1->thellnd), NODE_STR(e2->thellnd)));
    case COMPLEX_VAL:
        return(ExpCompare(e1->lhs(), e2->lhs()) && ExpCompare(e1->rhs(), e2->rhs()));
    case CONST_REF:
    case VAR_REF:
        return(e1->symbol() == e2->symbol());
    case ARRAY_REF:
    case FUNC_CALL:
        if (e1->symbol() == e2->symbol())
            return(ExpCompare(e1->lhs(), e2->lhs())); // compares subscript/argument lists
        else
            return(0);
    case EXPR_LIST:
    {SgExpression *el1, *el2;
    for (el1 = e1, el2 = e2; el1&&el2; el1 = el1->rhs(), el2 = el2->rhs())
        if (!ExpCompare(el1->lhs(), el2->lhs()))  // the corresponding elements of lists are not identical
            return(0);
    if (el1 || el2) //one list is shorter than other
        return(0);
    else
        return(1);
    }
    case MINUS_OP:  //unary operations
    case NOT_OP:
        return(ExpCompare(e1->lhs(), e2->lhs())); // compares operands    
    default:
        return(ExpCompare(e1->lhs(), e2->lhs()) && ExpCompare(e1->rhs(), e2->rhs()));
    }
}


SgExpression *LowerBound(SgSymbol *ar, int i)
// lower bound of i-nd dimension of array ar (i= 0,...,Rank(ar)-1)
{
    SgArrayType *artype;
    SgExpression *e;
    SgSubscriptExp *sbe;
    //if(IS_POINTER(ar))
     // return(new SgValueExp(1));
    artype = isSgArrayType(ar->type());
    if (!artype)
        return(NULL);
    e = artype->sizeInDim(i);
    if (!e)
        return(NULL);
    if ((sbe = isSgSubscriptExp(e)) != NULL) {
        if (sbe->lbound())
            return(sbe->lbound());

        //else if(IS_ALLOCATABLE_POINTER(ar)){
        //  if(HEADER(ar))
        //    return(header_ref(ar,Rank(ar)+3+i));
        //  else
        //    return(LBOUNDFunction(ar,i+1));
        //}

        else
            return(new SgValueExp(1));
    }
    else
        return(new SgValueExp(1));  // by default lower bound = 1      
}

int Rank(SgSymbol *s)
{
    SgArrayType *artype;
    //if(IS_POINTER(s))
     // return(PointerRank(s));
    artype = isSgArrayType(s->type());
    if (artype)
        return (artype->dimension());
    else
        return (0);
}

SgExpression *UpperBound(SgSymbol *ar, int i)
// upper bound of i-nd dimension of array ar (i= 0,...,Rank(ar)-1)
{
    SgArrayType *artype;
    SgExpression *e;
    SgSubscriptExp *sbe;


    artype = isSgArrayType(ar->type());
    if (!artype)
        return(NULL);
    e = artype->sizeInDim(i);
    if (!e)
        return(NULL);
    if ((sbe = isSgSubscriptExp(e)) != NULL) {
        if (sbe->ubound())
            return(sbe->ubound());

        //else if(HEADER(ar))
        //  return(&(*GetSize(HeaderRefInd(ar,1),i+1)-*HeaderRefInd(ar,Rank(ar)+3+i)+*new SgValueExp(1)));
        //else
        //  return(UBOUNDFunction(ar,i+1));

    }
    else
        return(e);
    // !!!! test case "*"
    return(e);
}

symb_list  *AddToSymbList(symb_list *ls, SgSymbol *s)
{
    symb_list *l;
    //adding the symbol 's' to symb_list 'ls'
    if (!ls) {
        ls = new symb_list;
        ls->symb = s;
        ls->next = NULL;
    }
    else {
        l = new symb_list;
        l->symb = s;
        l->next = ls;
        ls = l;
    }
    return(ls);
}
