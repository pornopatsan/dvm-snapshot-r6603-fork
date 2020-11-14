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

static SgSymbol** fillDataOfArray(SgExpression* on, int& dimInPar)
{
    dimInPar = 0;
    SgExpression* temp = on;
    while (temp)
    {
        dimInPar++;
        temp = temp->rhs();
    }
    SgSymbol** symbInPar = new SgSymbol * [dimInPar];
    temp = on;
    for (int i = 0; i < dimInPar; ++i)
    {
        symbInPar[i] = temp->lhs()->symbol();
        temp = temp->rhs();
    }
    return symbInPar;
}

SageArrayIdxs* GetIdxInParDir(const std::map<std::string, SgExpression*>& on, SgExpression *across, bool tie = false)
{
    SageArrayIdxs *ret = new SageArrayIdxs();
    SageArrayIdxs *act = ret;
    int allDim = 0;
    int dimInPar = 0;
    SgSymbol** symbInPar = NULL;
    ret->next = NULL;
    ret->array_expr = NULL;
    ret->read_write = -1;
    ret->dim = 0;
    ret->symb = NULL;

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
            if (symbInPar == NULL)
            {
                if (on.size() == 0)
                {
                    fprintf(stderr, "internal error in across convertion for GPU\n");
                    exit(-1);
                }
                else if (on.size() == 1)
                    symbInPar = fillDataOfArray(on.begin()->second, dimInPar);                
            }

            SgExpression *t = across->lhs();
            int dim = 0;

            if (tie)
            {
                if (t->variant() == ARRAY_REF)
                {
                    if (on.find(t->symbol()->identifier()) == on.end())
                    {
                        fprintf(stderr, "internal error in across convertion for GPU\n");
                        exit(-1);
                    }
                    else
                        symbInPar = fillDataOfArray(on.find(t->symbol()->identifier())->second, dimInPar);
                }
                else if (t->variant() == ARRAY_OP)
                {
                    if (on.find(t->lhs()->symbol()->identifier()) == on.end())
                    {
                        fprintf(stderr, "internal error in across convertion for GPU\n");
                        exit(-1);
                    }
                    else
                        symbInPar = fillDataOfArray(on.find(t->lhs()->symbol()->identifier())->second, dimInPar);
                }
            }

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
        SgExpression *tie = findDirect(temp->expr(1), ACC_TIE_OP);
        
        std::map<std::string, SgExpression*> arrays;
        if (t != NULL)
        {
            q = new SageAcrossInfo();
            if (temp->expr(0) && temp->expr(0)->lhs())
            {
                arrays[temp->expr(0)->symbol()->identifier()] = temp->expr(0)->lhs();
                q->idx = GetIdxInParDir(arrays, t);
            }                
            else if (tie)
            {
                SgExpression* list = tie->lhs();
                while (list)
                {
                    arrays[list->lhs()->symbol()->identifier()] = list->lhs()->lhs();
                    list = list->rhs();
                }
                q->idx = GetIdxInParDir(arrays, t, true);
            }
            else
            {
                fprintf(stderr, "internal error in across convertion for GPU\n");
                exit(-1);
            }
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
