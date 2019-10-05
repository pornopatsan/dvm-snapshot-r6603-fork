#include "dvm.h"
#include "aks_structs.h"

#define DEBUG_LV1 true
#if 1
std::ostream &out = std::cout;
#else
std::ofstream out("_log_debug_info.txt");
#endif

extern SgStatement *dvm_parallel_dir;

SgExpression* findDirect(SgExpression *inExpr, int DIR)
{
    SgExpression *temp = NULL;
    if (inExpr)
    {
        if (inExpr->variant() == DIR)
        {
            return inExpr;
        }
        else
        {
            if (inExpr->lhs())				
                temp = findDirect(inExpr->lhs(), DIR);				

            if(temp == NULL && inExpr->rhs())				
                temp = findDirect(inExpr->rhs(), DIR);
        }
    }
    return temp;
}

SageArrayIdxs* GetIdxInParDir(SgExpression *on, SgExpression *across)
{
    SageArrayIdxs *ret = new SageArrayIdxs();
    SageArrayIdxs *act = ret;
    int allDim = 0;
    int dimInPar = 0;
    ret->next = NULL;
    ret->array_expr = NULL;
    ret->read_write = -1;
    ret->dim = 0;
    ret->symb = NULL;

    SgExpression *temp = on;
    while (temp)
    {
        dimInPar++;
        temp = temp->rhs();
    }
    SgSymbol **symbInPar = new SgSymbol*[dimInPar];
    temp = on;
    for (int i = 0; i < dimInPar; ++i)
    {
        symbInPar[i] = temp->lhs()->symbol();
        temp = temp->rhs();
    }

    std::vector<SgExpression*> toAnalyze;
    if (across->lhs()->variant() == EXPR_LIST)
        toAnalyze.push_back(across->lhs());
    else
    {
        if (across->lhs()->variant() == DDOT)
            toAnalyze.push_back(across->lhs()->rhs());
        if (across->rhs())
            if (across->rhs()->variant() == DDOT)
                toAnalyze.push_back(across->rhs()->rhs());
    }

    for (int i = 0; i < toAnalyze.size(); ++i)
    {
        across = toAnalyze[i];
        while (across)
        {
            SgExpression *t = across->lhs();
            int dim = 0;
            if (t->variant() == ARRAY_REF)
                t = t->lhs();
            else if (t->variant() == ARRAY_OP)
                t = t->lhs()->lhs();
            else
            {
                if (DEBUG_LV1)
                    out << "!!! unknown variant in ACROSS dir: " << t->variant() << std::endl;
            }
            SgExpression *tmp = t;
            while (tmp)
            {
                dim++;
                tmp = tmp->rhs();
            }
            act->next = new SageArrayIdxs();
            act = act->next;
            act->next = NULL;
            act->symb = new SageSymbols*[dim];
            act->dim = dim;
            for (int i = 0; i < dim; ++i)
            {
                act->symb[i] = new SageSymbols();
                act->symb[i]->across_left = t->lhs()->lhs()->valueInteger();
                act->symb[i]->across_right = t->lhs()->rhs()->valueInteger();
                if (act->symb[i]->across_left != 0 || act->symb[i]->across_right != 0)
                    act->symb[i]->symb = symbInPar[i];
                else if (i < dimInPar)
                    act->symb[i]->symb = symbInPar[i];
                else
                    act->symb[i]->symb = NULL;
                act->symb[i]->next = NULL;
                t = t->rhs();
            }

            allDim++;
            across = across->rhs();
        }
    }
    ret->dim = allDim;
    return ret;
}

SageAcrossInfo* GetLoopsWithParAndAcrDir()
{
    SageAcrossInfo *q = NULL;	
    SgStatement *temp = dvm_parallel_dir;

    if (temp->variant() == DVM_PARALLEL_ON_DIR)
    {
        SgExpression *t = findDirect(temp->expr(1), ACROSS_OP);
        if (t != NULL)
        {
            q = new SageAcrossInfo();
            q->idx = GetIdxInParDir(temp->expr(0)->lhs(), t);
            q->next = NULL;
        }
    }		
    return q;
}

SageSymbols *GetSymbInParalell(int *n, SgExpression *first)
{
    SageSymbols *retval;
    SageSymbols *p_t = new SageSymbols();
    retval = p_t;
    while(first)
    {
        SageSymbols *q = new SageSymbols();
        q->len = -1;
        q->next = NULL;
        q->symb = first->lhs()->symbol();
        p_t->next = q;
        p_t = q;
        n[0]++;
        first = first->rhs();
    }
    return retval->next;
}
