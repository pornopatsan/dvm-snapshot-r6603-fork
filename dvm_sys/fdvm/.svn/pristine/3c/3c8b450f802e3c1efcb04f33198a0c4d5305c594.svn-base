#include "dvm.h"

void __convert_args(SgExpression *expr, SgExpression *&Arg, SgExpression *&Arg1, SgExpression *&Arg2)
{
    SgExpression *currArgs = ((SgFunctionCallExp *)expr)->args();
    Arg = currArgs->lhs();
    Arg1 = currArgs->rhs()->lhs();
    Arg2 = currArgs->rhs()->rhs()->lhs();
    convertExpr(Arg, Arg);
    convertExpr(Arg1, Arg1);
    convertExpr(Arg2, Arg2);
}

void __convert_args(SgExpression *expr, SgExpression *&Arg, SgExpression *&Arg1)
{
    SgExpression *currArgs = ((SgFunctionCallExp *)expr)->args();
    Arg = currArgs->lhs();
    Arg1 = currArgs->rhs()->lhs();
    convertExpr(Arg, Arg);
    convertExpr(Arg1, Arg1);
}

void __convert_args(SgExpression *expr, SgExpression *&Arg)
{
    SgExpression *currArgs = ((SgFunctionCallExp *)expr)->args();
    Arg = currArgs->lhs();
    convertExpr(Arg, Arg);
}

void __cmplx_handler(SgExpression *expr, SgExpression *&retExp, const char *name, int nArgs)
{
    SgExpression *currArgs = ((SgFunctionCallExp *)expr)->args();
    int countArgs = 0;
    bool kind = false;
    int kind_val = -1;
    int kind_pos = -1;

    while (currArgs)
    {
        if (currArgs->lhs()->variant() == KEYWORD_ARG)
        {
            kind = true;
            kind_val = currArgs->lhs()->rhs()->valueInteger();
            kind_pos = countArgs;
        }
        countArgs++;        
        currArgs = currArgs->rhs();
    }
    if (kind == false)
    {
        if (countArgs == 1)
            createNewFCall(expr, retExp, name, 1);
        else if (countArgs == 2)
            createNewFCall(expr, retExp, name, 2);
        else if (countArgs == 3) // with KIND
        {
            kind_val = ((SgFunctionCallExp *)expr)->args()->rhs()->rhs()->lhs()->valueInteger();
            if (kind_val == 4)
                createNewFCall(expr, retExp, "cmplx2", 2);
            else if (kind_val == 8)
                createNewFCall(expr, retExp, "dcmplx2", 2);
            else
                createNewFCall(expr, retExp, name, 2);
        }
    }
    else // with key word KIND
    {
        const char *name_kind;
        if (kind_val == 4)
            name_kind = "cmplx2";
        else if (kind_val == 8)
            name_kind = "dcmplx2";
        else
            name_kind = name;

        if (countArgs == 2)
            createNewFCall(expr, retExp, name_kind, 1);
        else if (countArgs == 3)
        {
            if (kind_pos == 2)
                createNewFCall(expr, retExp, name_kind, 2);
            else if (kind_pos == 0)
            {            
                SgFunctionCallExp *tmp = new SgFunctionCallExp(*createNewFunctionSymbol(NULL));
                tmp->addArg(*((SgFunctionCallExp *)expr)->args()->rhs()->lhs());
                tmp->addArg(*((SgFunctionCallExp *)expr)->args()->rhs()->rhs()->lhs());

                createNewFCall(tmp, retExp, name_kind, 2);
            }
            else
                createNewFCall(expr, retExp, "ERROR", 1);
        }
    }
}

void __minmax_handler(SgExpression *expr, SgExpression *&retExp, const char *name, int nArgs)
{
    SgExpression *currArgs = ((SgFunctionCallExp *)expr)->args();
    SgFunctionCallExp *retFunc = createNewFCall(name);
    //set first 2 agrs
    SgExpression *Arg = currArgs->lhs();
    convertExpr(Arg, Arg);
    retFunc->addArg(*Arg);

    currArgs = currArgs->rhs();
    Arg = currArgs->lhs();
    convertExpr(Arg, Arg);
    retFunc->addArg(*Arg);

    currArgs = currArgs->rhs();
    //create nested MAX/MIN functions 
    while (currArgs)
    {
        SgFunctionCallExp *tmp = createNewFCall(name);
        tmp->addArg(*retFunc);
        Arg = currArgs->lhs();
        convertExpr(Arg, Arg);
        tmp->addArg(*Arg);
        currArgs = currArgs->rhs();
        retFunc = tmp;
    }
    retExp = retFunc;
}

static bool isArgIntType(SgExpression *Arg)
{
    bool res = true;
    if (Arg->variant() == VAR_REF)
    {
        SgType *tmp = Arg->symbol()->type();
        if (tmp->equivalentToType(C_Type(SgTypeDouble())) ||
            tmp->equivalentToType(C_Type(SgTypeFloat())))
            res = false;
    }
    else
    {
        if (Arg->lhs())
            res = res && isArgIntType(Arg->lhs());
        if (Arg->rhs())
            res = res && isArgIntType(Arg->rhs());
    }
    return res;
}
//TODO: add more complex analysis above
void __mod_handler(SgExpression *expr, SgExpression *&retExp, const char *name, int nArgs)
{
    SgExpression *Arg, *Arg1;
    __convert_args(expr, Arg, Arg1);
    if (isArgIntType(Arg) && isArgIntType(Arg1))
        retExp = &(*Arg % *Arg1);
    else
    {
        retExp = createNewFCall("fmod");
        ((SgFunctionCallExp*) retExp)->addArg(*Arg);
        ((SgFunctionCallExp*) retExp)->addArg(*Arg1);
    }
}

void __iand_handler(SgExpression *expr, SgExpression *&retExp, const char *name, int nArgs)
{
    SgExpression *Arg, *Arg1;
    __convert_args(expr, Arg, Arg1);
    retExp = &(*Arg & *Arg1);
}

void __ior_handler(SgExpression *expr, SgExpression *&retExp, const char *name, int nArgs)
{
    SgExpression *Arg, *Arg1;
    __convert_args(expr, Arg, Arg1);
    retExp = &(*Arg | *Arg1);
}

void __ieor_handler(SgExpression *expr, SgExpression *&retExp, const char *name, int nArgs)
{
    SgExpression *Arg, *Arg1;
    __convert_args(expr, Arg, Arg1);

    SgExpression *xor_op = new SgExpression(XOR_OP);
    xor_op->setLhs(*Arg);
    xor_op->setRhs(*Arg1);
    retExp = xor_op;
}

void __arc_sincostan_d_handler(SgExpression *expr, SgExpression *&retExp, const char *name, int nArgs)
{
    SgExpression *Arg;
    __convert_args(expr, Arg);

    SgFunctionCallExp *retFunc = createNewFCall(name);
    retFunc->addArg(*Arg);

    retExp = &(*retFunc * *new SgValueExp(180.0) / *new SgVarRefExp(new SgSymbol(VARIABLE_NAME, "CUDART_PI")));
}

void __atan2d_handler(SgExpression *expr, SgExpression *&retExp, const char *name, int nArgs)
{
    SgExpression *Arg, *Arg1;
    __convert_args(expr, Arg, Arg1);

    SgFunctionCallExp *retFunc = createNewFCall(name);
    retFunc->addArg(*Arg);
    retFunc->addArg(*Arg1);

    retExp = &(*retFunc * *new SgValueExp(180.0) / *new SgVarRefExp(new SgSymbol(VARIABLE_NAME, "CUDART_PI")));
}

void __sindcosdtand_handler(SgExpression *expr, SgExpression *&retExp, const char *name, int nArgs)
{
    SgExpression *Arg;
    __convert_args(expr, Arg);

    SgFunctionCallExp *retFunc = createNewFCall(name);
    retFunc->addArg(*Arg * *new SgVarRefExp(new SgSymbol(VARIABLE_NAME, "CUDART_PI")) / *new SgValueExp(180.0));

    retExp = retFunc;
}

void __cotan_handler(SgExpression *expr, SgExpression *&retExp, const char *name, int nArgs)
{
    SgExpression *Arg;
    __convert_args(expr, Arg);

    SgFunctionCallExp *retFunc = createNewFCall(name);
    retFunc->addArg(*Arg);

    retExp = &(*new SgValueExp(1.0) / *retFunc);
}

void __cotand_handler(SgExpression *expr, SgExpression *&retExp, const char *name, int nArgs)
{
    SgExpression *Arg;
    __convert_args(expr, Arg);
    
    SgFunctionCallExp *retFunc = createNewFCall(name);
    retFunc->addArg(*Arg * *new SgVarRefExp(new SgSymbol(VARIABLE_NAME, "CUDART_PI")) / *new SgValueExp(180.0));
    
    retExp = &(*new SgValueExp(1.0) / *retFunc);
}

void __ishftc_handler(SgExpression *expr, SgExpression *&retExp, const char *name, int nArgs)
{
    SgExpression *currArgs = ((SgFunctionCallExp *)expr)->args();
    int countArgs = 0;

    while (currArgs)
    {
        countArgs++;
        currArgs = currArgs->rhs();
    }
    switch (countArgs)
    {
        case 2:
            createNewFCall(expr, retExp, "ishc", 2);
            break;
        case 3:
            createNewFCall(expr, retExp, name, 3);
            break;
        default:
            //printf("this function takes 2 or 3 arguments");
            break;
    }
}

void __merge_bits_handler(SgExpression *expr, SgExpression *&retExp, const char *name, int nArgs)
{
    SgExpression* Arg, * Arg1, * Arg2;
    __convert_args(expr, Arg, Arg1, Arg2);
    SgExpression *xor_op = new SgExpression(XOR_OP);
    xor_op->setLhs(*Arg2);
    xor_op->setRhs(*new SgValueExp(-1));
    retExp = &((*Arg & *Arg2) | (*Arg1 & *xor_op));
}

void __not_handler(SgExpression *expr, SgExpression *&retExp, const char *name, int nArgs)
{
    SgExpression* Arg;
    __convert_args(expr, Arg);
    SgExpression* xor_op = new SgExpression(XOR_OP);
    xor_op->setLhs(*Arg);
    xor_op->setRhs(*new SgValueExp(-1));
    retExp = xor_op;
}

void __poppar_handler(SgExpression *expr, SgExpression *&retExp, const char *name, int nArgs)
{
    SgExpression* Arg;
    __convert_args(expr, Arg);
    SgFunctionCallExp* func = createNewFCall(name);
    func->addArg(*Arg);
    retExp = &(*func & *new SgValueExp(1));
}

void __modulo_handler(SgExpression* expr, SgExpression*& retExp, const char* name, int nArgs)
{
    SgExpression* Arg, * Arg1;
    __convert_args(expr, Arg, Arg1);
    SgFunctionCallExp* floor = createNewFCall("floor");
    SgFunctionCallExp* doubleA = createNewFCall("double");
    doubleA->addArg(*Arg);
    SgFunctionCallExp* doubleB = createNewFCall("double");
    doubleB->addArg(*Arg1);
    floor->addArg(*doubleA / *doubleB);
    retExp = &(*Arg - *Arg1 * *floor);
}

