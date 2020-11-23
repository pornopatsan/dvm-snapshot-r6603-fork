// all unused code
#include "dvm.h"

/* FROM acc_index_analyzer (aks_structs.cpp) */
int dimentionOfArray(SgExpression *listIdxIn)
{
    int dim = 0;
    SgExpression *listIdx = listIdxIn;
    while (listIdx)
    {
        dim++;
        listIdx = listIdx->rhs();
    }
    return dim;
}

bool ifExist(std::vector<char*> &listL, char *str)
{
    bool retval = false;
    for (size_t i = 0; i < listL.size(); ++i)
    {
        if (strcmp(str, listL[i]) == 0)
        {
            retval = true;
            break;
        }
    }
    return retval;
}

int GetIdxPlaceInParDir(SageSymbols *inList, SgSymbol *id)
{
    int ret = -1;
    int count = 0;
    SageSymbols *tmp = inList;
    while (tmp)
    {
        if (strcmp(tmp->symb->identifier(), id->identifier()) == 0)
        {
            ret = count;
            break;
        }
        count++;
        tmp = tmp->next;
    }
    return ret;
}
/* END BLOCK */

/* FORM acc.app*/
template<int numFields> SgType *Type_N(SgType *type, char *name);
template<int numFields>
SgType *Type_N(SgType *type, char *name)
{
    SgSymbol *s_t = new SgSymbol(TYPE_NAME, name, *kernel_st);
    SgFieldSymb *sx, *sy, *sz, *sw, *s;

    if (numFields >= 1)
        s = sx = new SgFieldSymb("x", *type, *s_t);
    if (numFields >= 2)
    {
        s = sy = new SgFieldSymb("y", *type, *s_t);
        SYMB_NEXT_FIELD(sx->thesymb) = sy->thesymb;
    }
    if (numFields >= 3)
    {
        s = sz = new SgFieldSymb("z", *type, *s_t);
        SYMB_NEXT_FIELD(sy->thesymb) = sz->thesymb;
    }
    if (numFields >= 4)
    {
        s = sw = new SgFieldSymb("w", *type, *s_t);
        SYMB_NEXT_FIELD(sz->thesymb) = sw->thesymb;
    }
    SYMB_NEXT_FIELD(s->thesymb) = NULL;

    SgType *tstr = new SgType(T_STRUCT);
    TYPE_COLL_FIRST_FIELD(tstr->thetype) = sx->thesymb;
    s_t->setType(tstr);

    SgType *td = new SgType(T_DERIVED_TYPE);
    TYPE_SYMB_DERIVE(td->thetype) = s_t->thesymb;
    TYPE_SYMB(td->thetype) = s_t->thesymb;

    return(td);
}
/* END BLOCK */
