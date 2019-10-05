#include "dvm.h"
#include "acc_data.h"
#include "aks_structs.h"
#include "aks_loopStructure.h"

extern SgStatement *dvm_parallel_dir;
extern SgStatement* AssignStatement(SgExpression &lhs, SgExpression &rhs);

using namespace std;

// ---------------------------------------------------------------------- // Access

Access::Access(SgExpression *_exp, Array *_parent)
{
    exp = _exp;
    expAcc = copyOfUnparse(exp->unparse());
    operation[0] = operation[1] = 0;
    parentArray = _parent;
}

// only one idx  in one dimention in exp
void Access::matchLoopIdxs(vector<SgSymbol*> &symbols)
{
    SgExpression *tmp = exp;
    int idx = 0;
    
    if (alignOnLoop.size() == 0)
        alignOnLoop = vector<int>(parentArray->getDimNum());

    while (tmp)
    {
        for (unsigned i = 0; i < symbols.size(); ++i)
        {
            alignOnLoop[idx] = -1;
            if (matchRecursion(tmp->lhs(), symbols[i]))
            {
                alignOnLoop[idx] = i;
                break;
            }
        }
        idx++;
        tmp = tmp->rhs();
    }
}

bool Access::matchRecursion(SgExpression *_exp, SgSymbol *symb)
{
    bool retVal = false;

    SgExpression *left = _exp->lhs();
    SgExpression *right = _exp->rhs();

    if (_exp->variant() != VAR_REF)
    {
        if (left)
            retVal = retVal || matchRecursion(left, symb);
        if (right)
            retVal = retVal || matchRecursion(right, symb);
    }
    else
    {
        SgSymbol *s = _exp->symbol();
        if (strcmp(s->identifier(), symb->identifier()) == 0)
            retVal = true;
    }
    return retVal;
}

void Access::setExp(char* _exp)             { expAcc = _exp; }
void Access::setExp(SgExpression *_exp)     { exp = _exp; }
char* Access::getExpChar()                  { return expAcc; }
SgExpression* Access::getExp()              { return exp; }
void Access::incOperW()                     { operation[1]++; }
void Access::incOperR()                     { operation[0]++; }
Array* Access::getParentArray()             { return parentArray; }
void Access::setParentArray(Array *_parent) { parentArray = _parent; }
std::vector<int>* Access::getAlignOnLoop()  { return &alignOnLoop; }

// ---------------------------------------------------------------------- // Array

Array::Array(int _dim, char *_name, Loop *_parent)
{
    dimNum = _dim;
    name = _name;
    parentLoop = _parent;
    acrossType = 0;
}

Array::Array(char *_name, Loop *_parent)
{
    name = _name;
    parentLoop = _parent;
    acrossType = 0;
}

Access* Array::getAccess(char* _expAcc)
{
    int idx = -1;
    for (unsigned i = 0; i < accesses.size(); ++i)
    {
        if (strcmp(_expAcc, accesses[i]->getExpChar()) == 0)
        {
            idx = i;
            break;
        }
    }
    if (idx == -1)
        return NULL;
    else
        return accesses[idx];
}

void Array::analyzeAcrDims()
{
    SgExpression *tmp = dvm_parallel_dir->expr(1);
    bool fieled = false;
    while (tmp)
    {
        SgExpression *t = tmp->lhs();
        unsigned numberOfAcr = 0;
        if (t->variant() == ACROSS_OP)
        {
            t = t->lhs();
            while (t)
            {
                if (strcmp(name, t->lhs()->symbol()->identifier()) == 0)
                {
                    fieled = true;
                    SgExpression *tt = t->lhs()->lhs();
                    while (tt)
                    {
                        bool acrossYes = false;
                        if (tt->lhs()->lhs()->valueInteger() != 0)
                            acrossYes = true;
                        if (tt->lhs()->rhs()->valueInteger() != 0)
                            acrossYes = true;

                        if (acrossYes)
                        {
                            acrossDims.push_back(1);
                            numberOfAcr++;
                        }
                        else
                            acrossDims.push_back(0);
                        tt = tt->rhs();
                    }
                }
                t = t->rhs();
            }
        }
        if (numberOfAcr != 0)
            acrossType = (1 << numberOfAcr) - 1;
        tmp = tmp->rhs();
    }

    if (fieled == false)
    {
        for (int i = 0; i < dimNum; ++i)
            acrossDims.push_back(-1);
    }

    if (abs(dimNum - parentLoop->getLoopDim()))
    {
        for (int i = 0; i < abs(dimNum - parentLoop->getLoopDim()); i++)
            acrossDims.push_back(-1);
    }

}

void Array::analyzeAlignOnLoop()
{
    alignOnLoop = std::vector<int>(dimNum);
    for (int i = 0; i < dimNum; ++i)
        alignOnLoop[i] = -1;

    if (accesses.size() > 0)
    {

        for (unsigned i = 0; i < accesses.size(); ++i)
        {
            if (accesses[i]->getAlignOnLoop()->size() == 0)
                accesses[i]->matchLoopIdxs(*parentLoop->getSymbols());
        }

        int *tmp = new int[dimNum];
        for (int i = 0; i < dimNum; ++i)
            tmp[i] = (*(accesses[0]->getAlignOnLoop()))[i];

        bool eq = true;
        for (unsigned i = 1; i < accesses.size(); ++i)
        {
            bool ok = true;
            for (int k = 0; k < dimNum; ++k)
            {
                if (tmp[k] != (*(accesses[i]->getAlignOnLoop()))[k])
                {
                    ok = false;
                    break;
                }
            }

            if (!ok)
            {
                eq = false;
                break;
            }
        }

        if (eq)
        {
            for (int i = 0; i < dimNum; ++i)
                alignOnLoop[i] = tmp[i];
        }
    }
}

void Array::analyzeTrDims()
{
    int dimParLoop = parentLoop->getLoopDim();
    
    int idxAcrossSymb1 = -1;
    int idxAcrossSymb2 = -1;

    // all for's of Loop with across
    if (dimParLoop > 1 && parentLoop->getAcrType() > 1)
    {
        if (parentLoop->getAcrType() == dimParLoop)
        {
            idxAcrossSymb1 = dimParLoop - 1;
            idxAcrossSymb2 = dimParLoop - 2;
        }
        else
        {
            int t = 0;
            for (int p = (int)(acrossDims.size() - 1); p >= 0 && t != 2; --p)
            {
                if (acrossDims[p] == 1)
                {
                    idxAcrossSymb1 = p;
                    t++;
                }
            }
        }

        int idxInArray1 = -1;
        int idxInArray2 = -1;
        for (unsigned i = 0; i < alignOnLoop.size(); ++i)
        {
            if (alignOnLoop[i] == idxAcrossSymb1)
                idxInArray1 = i;
            else if (alignOnLoop[i] == idxAcrossSymb2)
                idxInArray2 = i;
        }

        if (idxInArray1 != -1 && idxInArray2 != -1)
        {
            // inverse idxInArray and count from "1"
            idxInArray1 = dimNum - idxInArray1;
            idxInArray2 = dimNum - idxInArray2;
        }

        addTfmDim(idxInArray1);
        addTfmDim(idxInArray2);
    }
}

SgSymbol* Array::findAccess(SgExpression *_exp, char *&_charEx)
{
    SgSymbol *retVal = NULL;
    char *retStr = new char[1024]; // WARNING!! may be segfault
    SgExpression *tmp = _exp;

    retStr[0] = '\0';
    int out = 0;
    int idx = 0;
    while (tmp && out != 2)
    {
        if (dimNum - idx == transformDims[0] || dimNum - idx == transformDims[1])
        {
            strcat(retStr, UnparseExpr(tmp->lhs()));
            strcat(retStr, "_");
            out++;
        }
        idx++;
        tmp = tmp->rhs();
    }

    for (unsigned i = 0; i < charEx.size(); ++i)
    {
        if (strcmp(charEx[i], retStr) == 0)
        {
            retVal = coefInAccess[i];
            break;
        }
    }

    if (retVal == NULL)
    {
        _charEx = new char[strlen(retStr) + 1];
        _charEx[0] = '\0';
        strcat(_charEx, retStr);
    }
    delete []retStr;
    return retVal;
}

void Array::addNewCoef(SgExpression *_exp, char *_charEx, SgSymbol* _symb)
{	
    SgExpression *tmp = _exp;
    
    int out = 0;
    int idx = 0;
    while (tmp && out != 2)
    {
        if (dimNum - idx == transformDims[0])
            firstEx.push_back(tmp->lhs());
        else if (dimNum - idx == transformDims[1])
            secondEx.push_back(tmp->lhs());
        idx++;
        tmp = tmp->rhs();
    }

    charEx.push_back(_charEx);
    coefInAccess.push_back(_symb);
}

void Array::generateAssigns(SgVarRefExp *offsetX, SgVarRefExp *offsetY, SgVarRefExp *Rx, SgVarRefExp *Ry, SgVarRefExp *slash)
{
    if (ifCalls.size() == 0 && elseCalls.size() == 0 && zeroSt.size() == 0)
    {
        for (unsigned i = 0; i < coefInAccess.size(); ++i)
        {
            zeroSt.push_back(AssignStatement(*new SgVarRefExp(coefInAccess[i]->copy()), *new SgValueExp(0)));
            SgFunctionCallExp *funcCallExpIf, *funcCallExpElse;

            funcCallExpIf = new SgFunctionCallExp(*(new SgSymbol(FUNCTION_NAME, funcDvmhConvXYname)));
            funcCallExpElse = new SgFunctionCallExp(*(new SgSymbol(FUNCTION_NAME, funcDvmhConvXYname)));

            funcCallExpIf->addArg(firstEx[i]->copy() - *offsetX);
            funcCallExpIf->addArg(secondEx[i]->copy() - *offsetY);
            funcCallExpIf->addArg(*Rx);
            funcCallExpIf->addArg(*Ry);
            funcCallExpIf->addArg(*slash);
            funcCallExpIf->addArg(*new SgVarRefExp(coefInAccess[i]->copy()));

            funcCallExpElse->addArg(secondEx[i]->copy() - *offsetX);
            funcCallExpElse->addArg(firstEx[i]->copy() - *offsetY);
            funcCallExpElse->addArg(*Rx);
            funcCallExpElse->addArg(*Ry);
            funcCallExpElse->addArg(*slash);
            funcCallExpElse->addArg(*new SgVarRefExp(coefInAccess[i]->copy()));

            ifCalls.push_back(funcCallExpIf);
            elseCalls.push_back(funcCallExpElse);
        }
    }
}

void Array::setDimNum(int _num)					 { dimNum = _num; }
int Array::getDimNum()							 { return dimNum; }
Loop* Array::getParentLoop()					 { return parentLoop; }
void Array::setParentLoop(Loop *_loop)			 { parentLoop = _loop; }
vector<int>* Array::getAcrDims()				 { return &acrossDims; }
vector<int>* Array::getAlignOnLoop()			 { return &alignOnLoop; }
void Array::addTfmDim(int _dim)					 { transformDims.push_back(_dim); }
vector<int>* Array::getTfmDims()				 { return &transformDims; }
void Array::addAccess(Access* _newAccess)		 { accesses.push_back(_newAccess); }
vector<Access*>* Array::getAccesses()			 { return &accesses; }
void Array::setArrayName(char* _name)			 { name = _name; }
char* Array::getArrayName()						 { return name; }
int Array::getAcrType()							 { return acrossType; }
void Array::setAcrType(int _type)				 { acrossType = _type; }
vector<SgFunctionCallExp*>* Array::getIfCals()   { return &ifCalls; }
vector<SgFunctionCallExp*>* Array::getElseCals() { return &elseCalls; }
vector<SgStatement*>* Array::getZeroSt()         { return &zeroSt; }
vector<SgSymbol* >* Array::getCoefInAccess()     { return  &coefInAccess; }
// ---------------------------------------------------------------------- // Loop

Loop::Loop(int _line)
{
    line = _line;
    acrossType = 0;
    loopDim = 0;
}

Loop::Loop(int _line, SgStatement *_body)
{
    line = _line;
    loopBody = _body;
    acrossType = 0;
    loopDim = 0;
}

Loop::Loop(int _acrType, int _line, SgStatement *_body)
{
    line = _line;
    loopBody = _body;
    acrossType = _acrType;
    loopDim = 0;
}

Loop::Loop(int _line, SgStatement *_body, bool withAnalyze)
{
    line = _line;
    loopBody = _body;
    acrossType = 0;
    loopDim = 0;

    if (withAnalyze)
        analyzeLoopBody();
}

void Loop::analyzeLoopBody()
{
    // create info of array
    SgStatement *stmt = loopBody;
    while (stmt)
    {
        if (stmt->variant() == ASSIGN_STAT)
        {
            SgExpression *exL = stmt->expr(0);
            SgExpression *exR = stmt->expr(1);

            if (exL)
                analyzeAssignOp(exL, 1);
            if (exR)
                analyzeAssignOp(exR, 0);
        }
        stmt = stmt->lexNext();
    }

    // create idxs info
    SgExpression *par_dir = dvm_parallel_dir->expr(2);
    while (par_dir)
    {
        symbols.push_back(par_dir->lhs()->symbol());
        par_dir = par_dir->rhs();
    }
    loopDim = symbols.size();

    // create private list
    SgExpression *tmp = dvm_parallel_dir->expr(1);
    while (tmp)
    {
        SgExpression *t = tmp->lhs();
        if (t->variant() == ACC_PRIVATE_OP)
        {
            t = t->lhs();
            while (t)
            {
                if (isSgArrayType(t->lhs()->symbol()->type()))
                    privateList.push_back(copyOfUnparse(t->lhs()->symbol()->identifier()));
                t = t->rhs();
            }
        }
        tmp = tmp->rhs();
    }

    // analyze acrossType and acrossDims in all arrays
    for (unsigned i = 0; i < arrays.size(); ++i)
    {
        if ( !isArrayInPrivate(arrays[i]->getArrayName()) )
        {
            arrays[i]->analyzeAcrDims();
            arrays[i]->analyzeAlignOnLoop();
        }
    }

    analyzeAcrossType();

    // analyze transformDims in all arrays
    if (acrossType > 1)
    {
        for (unsigned i = 0; i < arrays.size(); ++i)
        {
            if (!isArrayInPrivate(arrays[i]->getArrayName()))
                arrays[i]->analyzeTrDims();
        }
    }
}

void Loop::analyzeAssignOp(SgExpression *_exp, int oper)
{
    if (_exp->variant() != ARRAY_REF)
    {
        if (_exp->lhs())
            analyzeAssignOp(_exp->lhs(), oper);
        if (_exp->rhs())
            analyzeAssignOp(_exp->rhs(), oper);
    }
    else
    {
        SgSymbol *arrName = _exp->symbol();
        if (isSgArrayType(arrName->type())) // if array ref
        {
            int idx;
            Array *newArray = getArray(arrName->identifier(), &idx);
            if (newArray == NULL)
            {				
                Array *nArr = new Array(arrName->identifier(), this);
                Access *nAcc = new Access(_exp->lhs(), nArr);

                nArr->setDimNum(isSgArrayType(arrName->type())->dimension());
                nArr->addAccess(nAcc);
                addArray(nArr);

                if (oper == 1)
                    nAcc->incOperW();
                else if (oper == 0)
                    nAcc->incOperR();
            }
            else
            {
                char *strAcc = copyOfUnparse(_exp->lhs()->unparse());
                Access *tAcc = newArray->getAccess(strAcc);

                if (tAcc == NULL)
                {
                    tAcc = new Access(_exp->lhs(), newArray);
                    newArray->addAccess(tAcc);
                }

                if (oper == 1)
                    tAcc->incOperW();
                else if (oper == 0)
                    tAcc->incOperR();
            }
        }
    }
}

Array* Loop::getArray(char *name, int *_idx)
{
    int idx = -1;
    for (unsigned i = 0; i < arrays.size(); ++i)
    {
        if (strcmp(name, arrays[i]->getArrayName()) == 0)
        {
            idx = i;
            break;
        }
    }
    _idx[0] = idx;
    if (idx == -1)
        return NULL;
    else
        return arrays[idx];
}

Array* Loop::getArray(char *name)
{
    int idx = -1;
    for (unsigned i = 0; i < arrays.size(); ++i)
    {
        if (strcmp(name, arrays[i]->getArrayName()) == 0)
        {
            idx = i;
            break;
        }
    }
    
    if (idx == -1)
        return NULL;
    else
        return arrays[idx];
}

void Loop::analyzeAcrossType()
{
    for (int i = 0; i < loopDim; ++i)
        acrDims.push_back(-1);

    for (unsigned i = 0; i < arrays.size(); ++i)
    {
        std::vector<int>* tArrAcrDims = arrays[i]->getAcrDims();
        std::vector<int>* tArrAlign = arrays[i]->getAlignOnLoop();

        for (unsigned k = 0; k < tArrAlign->size(); ++k)
        {
            if ((*tArrAlign)[k] != -1)
                acrDims[(*tArrAlign)[k]] = MAX(acrDims[(*tArrAlign)[k]], (*tArrAcrDims)[(*tArrAlign)[k]]);
        }
    }
    
    acrossType = 0;
    for (int i = 0; i < loopDim; ++i)
    {
        if (acrDims[i] != -1)
            acrossType++;
    }

}

bool Loop::isArrayInPrivate(char *name)
{
    bool retVal = false;
    for (unsigned i = 0; i < privateList.size(); ++i)
    {
        if (strcmp(name, privateList[i]) == 0)
        {
            retVal = true;
            break;
        }
    }
    return retVal;
}

void Loop::addArray(Array *_array)    { arrays.push_back(_array); }
void Loop::setLine(int _line)         { line = _line;             }
int Loop::getLine()                   { return line;              }
void Loop::setAcrType(int _type)      { acrossType = _type;       }
int Loop::getAcrType()                { return acrossType;        }
vector<Array*>* Loop::getArrays()     { return &arrays;           }
vector<SgSymbol*>* Loop::getSymbols() { return &symbols;          }
int Loop::getLoopDim()                { return loopDim;           }
