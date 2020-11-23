
#include "dvm.h"
#include "acc_across_analyzer.h"

using namespace std;

// special storages to avoid recomputing
map<string, SgExpression*> lhs;
map<string, SgExpression*> rhs;
map<SgExpression*, string> unparsedLhs;
map<SgExpression*, string> unparsedRhs;

template<typename InIt1, typename InIt2, typename OutIt>
static inline OutIt difference(InIt1 first1, InIt1 last1, InIt2 first2, InIt2 last2, OutIt dest)
{
    for (; first1 != last1 && first2 != last2;)
    {
        if (*first1 < *first2)
        {
            *dest++ = *first1;
            ++first1;
        }
        else if (*first2 < *first1)
            ++first2;
        else
        {
            ++first1;
            ++first2;
        }
    }

    return copy(first1, last1, dest);
}

template<typename InIt1, typename InIt2, typename OutIt>
static inline OutIt intersection(InIt1 first1, InIt1 last1, InIt2 first2, InIt2 last2, OutIt dest)
{
    for (; first1 != last1 && first2 != last2;)
    {
        if (*first1 < *first2)
            ++first1;
        else if (*first2 < *first1)
            ++first2;
        else
        {
            *dest++ = *first1++;
            ++first2;
        }
    }
    return dest;
}

static int replace(SgExpression* expr, SgStatement* parent, SgExpression* patt, SgExpression* subst)
{
    if (ExpCompare(expr, patt) != 0)
    {
        *expr = subst->copy();
        if (ExpCompare(parent->expr(0), expr) != 0)
            parent->setExpression(0, *expr);
        else if (ExpCompare(parent->expr(1), expr) != 0)
            parent->setExpression(1, *expr);
        return 1;
    }
    int count = 0;
    vector<SgExpression*> subexprs;
    subexprs.push_back(NULL);
    subexprs.push_back(expr);
    int k = 1;
    vector<int> positions(2);
    for (vector<SgExpression*>::iterator p = subexprs.begin() + 1; p != subexprs.end(); ++k, p = subexprs.begin() + k)
    {
        if (ExpCompare(*p, patt) == 0)
        {
            SgExpression* lhs = (*p)->lhs();
            SgExpression* rhs = (*p)->rhs();
            if (lhs != NULL)
            {
                subexprs.push_back(lhs);
                positions.push_back(-k);
            }
            if (rhs != NULL)
            {
                subexprs.push_back(rhs);
                positions.push_back(k);
            }
        }
        else
        {
            if (positions[k] < 0)
                subexprs[-positions[k]]->setLhs(subst->copyPtr());
            else
                subexprs[positions[k]]->setRhs(subst->copyPtr());
            ++count;
        }
    }
    return count;
}

static int replaceInSubscripts(SgExpression* expr, SgStatement* parent, SgExpression* patt, SgExpression* subst)
{
    if (expr == NULL)
        return 0;
    int count = 0;
    vector<SgExpression*> subexprs;
    subexprs.push_back(expr);
    int k = 0;
    for (vector<SgExpression*>::iterator p = subexprs.begin(); p != subexprs.end(); ++k, p = subexprs.begin() + k)
    {
        if ((*p)->variant() == ARRAY_REF)
        {
            for (SgExpression* tmp = ((SgArrayRefExp*)* p)->subscripts(); tmp != NULL; tmp = tmp->rhs())
                count += replace(tmp->lhs(), parent, patt, subst);
        }
        else
        {
            SgExpression* lhs = (*p)->lhs();
            SgExpression* rhs = (*p)->rhs();
            if (lhs != NULL)
                subexprs.push_back(lhs);
            if (rhs != NULL)
                subexprs.push_back(rhs);
        }
    }
    return count;
}

#define add(a, b)      (a) + (b)
#define subtract(a, b) (a) - (b)
#define multiply(a, b) (a) * (b)
#define divide(a, b)   (a) / (b)

#define compute(lhs, rhs, parent, op, cast) \
switch (lhs->variant()) \
{ \
    case BOOL_VAL: \
        lhs = new SgValueExp(op(cast(((SgValueExp*)lhs)->boolValue() == true ? -1 : 0), rhs)); \
        break; \
    case INT_VAL: \
        lhs = new SgValueExp(op(cast((SgValueExp*)lhs)->intValue(), rhs)); \
        break; \
    case FLOAT_VAL: \
        lhs = new SgValueExp(op(cast strtod(((SgValueExp*)lhs)->floatValue(), NULL), rhs)); \
        break; \
    case DOUBLE_VAL: \
        lhs = new SgValueExp(op(cast strtod(((SgValueExp*)lhs)->doubleValue(), NULL), rhs)); \
        break; \
    default: \
        changed = false; \
        lhs = parent; \
        break; \
}

void Loop::getRPN(SgExpression* expr, list<SgExpression*>& rpn) const
{
    if (expr == NULL)
        return;
    stack<SgExpression*> stack;
    stack.push(expr);
    while (stack.empty() == false)
    {
        SgExpression* expr = stack.top();
        stack.pop();
        switch (expr->variant())
        {
            case ARRAY_REF:
            case FUNC_CALL:
                break;
            case SUBT_OP:
                *expr = *expr->lhs() + *new SgExpression(MINUS_OP, expr->rhs(), NULL, NULL);
                stack.push(expr->lhs());
                stack.push(expr->rhs());
                break;
            default:
                if (expr->lhs() != NULL)
                    stack.push(expr->lhs());
                if (expr->rhs() != NULL)
                    stack.push(expr->rhs());
                break;
        }
        rpn.push_front(expr);
    }
}

void Loop::unrollRPN(list<SgExpression*>& rpn, map<SgExpression*, int>& arity) const
{
    set<SgExpression*> visited;
    for (list<SgExpression*>::iterator it = rpn.begin(); it != rpn.end();)
    {
        if (visited.find(*it) == visited.end())
            visited.insert(*it);
        else
        {
            ++it;
            continue;
        }
        switch ((*it)->variant())
        {
            case ARRAY_REF:
            case FUNC_CALL:
                for (SgExpression* tmp = (*it)->lhs(); tmp != NULL; tmp = tmp->rhs())
                {
                    list<SgExpression*> subrpn;
                    getRPN(tmp->lhs(), subrpn);
                    optimizeRPN(subrpn, arity, false);
                    rpn.insert(it, subrpn.begin(), subrpn.end());
                }
                it = rpn.begin();
                break;
            default:
                ++it;
                break;
        }
    }
}

void Loop::optimizeRPN(list<SgExpression*>& rpn, map<SgExpression*, int>& arity, bool unrolled) const
{
    for (list<SgExpression*>::iterator it = rpn.begin(); it != rpn.end();)
    {
        if ((*it)->lhs() != NULL)
        {
            if ((*it)->rhs() != NULL)
            {
                int _arity = 2;
                int variant = (*it)->variant();
                switch (variant)
                {
                    case ADD_OP:
                    case MULT_OP:
                    {
                        if (arity.find(*it) != arity.end())
                        {
                            ++it;
                            break;
                        }
                        bool found = false;
                        list<SgExpression*>::iterator old = it, tmp = it;
                        for (++it; it != rpn.end(); ++it)
                        {
                            if ((*it)->variant() == variant && (((*it)->lhs() != NULL && (*it)->lhs()->variant() == variant) || ((*it)->rhs() != NULL && (*it)->rhs()->variant() == variant)))
                            {
                                rpn.erase(tmp);
                                tmp = it;
                                ++_arity;
                            }
                            else if ((*it)->lhs() != NULL || (unrolled && ((*it)->variant() == ARRAY_REF || (*it)->variant() == FUNC_CALL)))
                                break;
                            else if (found == false)
                            {
                                old = it;
                                found = true;
                            }
                        }

                        arity[*tmp] = _arity;
                        if (found == true)
                            it = ++old;
                        break;
                    }
                    default:
                        arity[*it] = _arity;
                        ++it;
                        break;
                }
            }
            else
            {
                if ((*it)->variant() == FUNC_CALL || (*it)->variant() == ARRAY_REF)
                    arity[*it] = ((SgExprListExp*)(*it)->lhs())->length();
                else
                    arity[*it] = 1;
                ++it;
            }
        }
        else
            ++it;
    }
}

SgExpression* Loop::simplify(SgExpression* expr) const
{
    if (enable_opt == false || expr == NULL)
        return expr;

    list<SgExpression*> rpn;
    map<SgExpression*, int> arity;

    getRPN(expr, rpn);
    optimizeRPN(rpn, arity, false);
    unrollRPN(rpn, arity);
    optimizeRPN(rpn, arity, true);

    bool changed = true;
    while (changed == true)
    {
        changed = false;
        stack<SgExpression*> stack;
        for (list<SgExpression*>::iterator it = rpn.begin(); it != rpn.end(); ++it)
        {
            if ((*it)->lhs() != NULL)
            {
                if ((*it)->rhs() != NULL)
                {
                    int _arity = arity[*it];
                    vector<SgExpression*> args(_arity);
                    for (int i = _arity - 1; i >= 0; --i)
                    {
                        args[i] = stack.top();
                        stack.pop();
                    }
                    SgExpression* result = NULL;
                    switch ((*it)->variant())
                    {
                        case ADD_OP:
                        {
                            result = new SgValueExp(0);
                            list<SgExpression*> _args;
                            for (int i = 0; i < _arity; ++i)
                            {
                                switch (args[i]->variant())
                                {
                                case BOOL_VAL:
                                    compute(result, ((SgValueExp*)args[i])->boolValue() == true ? -1 : 0, (*it), add, );
                                    break;
                                case INT_VAL:
                                    compute(result, ((SgValueExp*)args[i])->intValue(), (*it), add, );
                                    break;
                                case FLOAT_VAL:
                                    compute(result, (float)strtod(((SgValueExp*)args[i])->floatValue(), NULL), (*it), add, );
                                    break;
                                case DOUBLE_VAL:
                                    compute(result, strtod(((SgValueExp*)args[i])->doubleValue(), NULL), (*it), add, );
                                    break;
                                default:
                                    _args.push_back(args[i]);
                                    break;
                                }
                            }
                            for (list<SgExpression*>::iterator it1 = _args.begin(); it1 != _args.end();)
                            {
                                bool cond = (*it1)->variant() == MINUS_OP;
                                bool changed = false;
                                for (list<SgExpression*>::iterator it2 = it1; it2 != _args.end();)
                                {
                                    if (cond == true && ExpCompare((*it1)->lhs(), *it2) == 1 || cond == false && (*it2)->variant() == MINUS_OP && ExpCompare(*it1, (*it2)->lhs()) == 1)
                                    {
                                        it1 = _args.erase(it1);
                                        if (it1 == it2)
                                        {
                                            it2 = _args.erase(it2);
                                            it1 = it2;
                                        }
                                        else
                                            it2 = _args.erase(it2);
                                        changed = true;
                                    }
                                    else
                                        ++it2;
                                }
                                if (changed == false)
                                    ++it1;
                            }
                            if (_args.size() + 1 < args.size())
                                changed = true;
                            bool zero = false;
                            switch (result->variant())
                            {
                                case BOOL_VAL:
                                    zero = ((SgValueExp*)result)->boolValue() == false;
                                    break;
                                case INT_VAL:
                                    zero = ((SgValueExp*)result)->intValue() == 0;
                                    break;
                                case FLOAT_VAL:
                                    zero = (float)strtod(((SgValueExp*)result)->floatValue(), NULL) == 0.0f;
                                    break;
                                case DOUBLE_VAL:
                                    zero = strtod(((SgValueExp*)result)->doubleValue(), NULL) == 0.0;
                                    break;
                                default:
                                    break;
                            }
                            if (zero == true)
                            {
                                if (_args.size() != 0)
                                {
                                    result = *_args.begin();
                                    for (list<SgExpression*>::iterator it = ++_args.begin(); it != _args.end(); ++it)
                                        result = &(**it + *result);
                                }
                            }
                            else
                                for (list<SgExpression*>::iterator it = _args.begin(); it != _args.end(); ++it)
                                    result = &(**it + *result);
                            break;
                        }
                        case MULT_OP:
                        {
                            result = new SgValueExp(1);
                            list<SgExpression*> _args;
                            for (int i = 0; i < _arity; ++i)
                            {
                                switch (args[i]->variant())
                                {
                                case BOOL_VAL:
                                    compute(result, ((SgValueExp*)args[i])->boolValue() == true ? -1 : 0, (*it), multiply, );
                                    break;
                                case INT_VAL:
                                    compute(result, ((SgValueExp*)args[i])->intValue(), (*it), multiply, );
                                    break;
                                case FLOAT_VAL:
                                    compute(result, (float)strtod(((SgValueExp*)args[i])->floatValue(), NULL), (*it), multiply, );
                                    break;
                                case DOUBLE_VAL:
                                    compute(result, strtod(((SgValueExp*)args[i])->doubleValue(), NULL), (*it), multiply, );
                                    break;
                                default:
                                    _args.push_back(args[i]);
                                    break;
                                }
                            }

                            if (_args.size() + 1 < args.size())
                                changed = true;
                            bool one = false;
                            switch (result->variant())
                            {
                                case BOOL_VAL:
                                    one = ((SgValueExp*)result)->boolValue() == true;
                                    break;
                                case INT_VAL:
                                    one = ((SgValueExp*)result)->intValue() == 1;
                                    break;
                                case FLOAT_VAL:
                                    one = (float)strtod(((SgValueExp*)result)->floatValue(), NULL) == 1.0f;
                                    break;
                                case DOUBLE_VAL:
                                    one = strtod(((SgValueExp*)result)->doubleValue(), NULL) == 1.0;
                                    break;
                                default:
                                    break;
                            }

                            if (one == true)
                            {
                                if (_args.size() != 0)
                                {
                                    result = *_args.begin();
                                    for (list<SgExpression*>::iterator it = ++_args.begin(); it != _args.end(); ++it)
                                        result = &(**it * *result);
                                }
                            }
                            else
                            {
                                for (list<SgExpression*>::iterator it = _args.begin(); it != _args.end(); ++it)
                                    result = &(**it * *result);
                            }
                            break;
                        }
                        case DIV_OP:
                        {
                            SgExpression* lhs = args[0];
                            SgExpression* rhs = args[1];
                            changed = true;
                            if (ExpCompare(lhs, rhs) == 1)
                            {
                                result = new SgValueExp(1);
                                break;
                            }
                            else if (lhs->variant() == MINUS_OP && ExpCompare(lhs->lhs(), rhs) == 1 || rhs->variant() == MINUS_OP && ExpCompare(lhs, rhs->lhs()) == 1)
                            {
                                result = new SgValueExp(-1);
                                break;
                            }

                            result = new SgExpression(lhs->thellnd);
                            bool error = false;
                            switch (rhs->variant())
                            {
                                case BOOL_VAL:
                                {
                                    bool value = ((SgValueExp*)rhs)->boolValue();
                                    if (value == false)
                                    {
                                        error = true;
                                        break;
                                    }
                                    compute(result, value == true ? -1 : 0, (*it), divide,);
                                    break;
                                }
                                case INT_VAL:
                                {
                                    int value = ((SgValueExp*)rhs)->intValue();
                                    if (value == 0)
                                    {
                                        error = true;
                                        break;
                                    }
                                    compute(result, value, (*it), divide,);
                                    break;
                                }
                                case FLOAT_VAL:
                                {
                                    float value = (float)strtod(((SgValueExp*)rhs)->floatValue(), NULL);
                                    if (value == 0.0f)
                                    {
                                        error = true;
                                        break;
                                    }
                                    compute(result, value, (*it), divide,);
                                    break;
                                }
                                case DOUBLE_VAL:
                                {
                                    double value = strtod(((SgValueExp*)rhs)->doubleValue(), NULL);
                                    if (value == 0.0)
                                    {
                                        error = true;
                                        break;
                                    }
                                    compute(result, value, (*it), divide,);
                                    break;
                                }
                                default:
                                    changed = false;
                                    delete result;
                                    result = *it;
                                    break;
                            }
                            if (error == true)
                            {
                                changed = false;
                                delete result;
                                result = *it;
                            }
                            break;
                        }
                        case EXP_OP:
                        {
                            SgExpression* lhs = args[0];
                            SgExpression* rhs = args[1];
                            result = new SgExpression(lhs->thellnd);
                            changed = true;
                            switch (rhs->variant())
                            {
                                case BOOL_VAL:
                                    compute(result, (((SgValueExp*)rhs)->boolValue() == true ? -1 : 0), (*it), pow, (float));
                                    break;
                                case INT_VAL:
                                    compute(result, ((SgValueExp*)rhs)->intValue(), (*it), pow, (float));
                                    break;
                                case FLOAT_VAL:
                                    compute(result, strtod(((SgValueExp*)rhs)->floatValue(), NULL), (*it), pow,);
                                    break;
                                case DOUBLE_VAL:
                                    compute(result, strtod(((SgValueExp*)rhs)->doubleValue(), NULL), (*it), pow,);
                                    break;
                                default:
                                    changed = false;
                                    delete result;
                                    result = *it;
                                    break;
                            }
                            break;
                        }
                        default:
                            // unsupported node with two subtrees, let compiler deal with it
                            result = *it;
                            break;
                    }
                    stack.push(result);
                }
                else
                {
                    switch ((*it)->variant())
                    {
                        case FUNC_CALL:
                        {
                            vector<SgExpression*> args(arity[*it]);
                            for (int i = arity[*it] - 1; i >= 0; --i)
                            {
                                args[i] = stack.top();
                                stack.pop();
                            }
                            for (unsigned int i = 0; i < args.size(); ++i)
                                *((SgFunctionCallExp*)*it)->arg(i) = *args[i];

                            // probably can be evaluated
                            stack.push(*it);
                            break;
                        }
                        case ARRAY_REF:
                        {
                            vector<SgExpression*> subscripts(arity[*it]);
                            for (int i = arity[*it] - 1; i >= 0; --i)
                            {
                                subscripts[i] = stack.top();
                                stack.pop();
                            }
                            for (unsigned int i = 0; i < subscripts.size(); ++i)
                                *((SgArrayRefExp*)*it)->subscript(i) = *subscripts[i];

                            stack.push(*it);
                            break;
                        }
                        case MINUS_OP:
                        {
                            SgExpression* arg = stack.top();
                            SgExpression* result;
                            stack.pop();
                            changed = true;
                            switch (arg->variant())
                            {
                            case BOOL_VAL:
                                result = new SgValueExp(((SgValueExp*)arg)->boolValue() == true ? 1 : 0);
                                break;
                            case INT_VAL:
                                result = new SgValueExp(-((SgValueExp*)arg)->intValue());
                                break;
                            case FLOAT_VAL:
                                result = new SgValueExp(-(float)strtod(((SgValueExp*)arg)->floatValue(), NULL));
                                break;
                            case DOUBLE_VAL:
                                result = new SgValueExp(-strtod(((SgValueExp*)arg)->doubleValue(), NULL));
                                break;
                            case MINUS_OP:
                                result = arg->lhs();
                                break;
                            case UNARY_ADD_OP:
                                result = new SgExpression(MINUS_OP, new SgExpression(arg->lhs()->thellnd), NULL, NULL);
                            default:
                                changed = false;
                                result = *it;
                                break;
                            }
                            stack.push(result);
                            break;
                        }
                        case UNARY_ADD_OP:
                            break;
                        default:
                            // unsupported node with one subtree, let compiler deal with it
                            stack.push(*it);
                            break;
                    }
                }
            }
            else
                stack.push(*it);
        }

        if (changed == true)
        {
            rpn.clear();
            getRPN(stack.top(), rpn);
            arity.clear();
            optimizeRPN(rpn, arity, false);
            unrollRPN(rpn, arity);
            optimizeRPN(rpn, arity, true);
        }
        else
            *expr = *stack.top();
    }
    return expr;
}


void Access::getReferences(SgExpression* expr,
                           set<SgExpression*>& references,
                           map<SgExpression*, string>& unparsedRefs,
                           map<string, SgExpression*>& refs) const
{
    vector<SgExpression*> subexprs;
    subexprs.push_back(expr);
    int k = 0;
    for (vector<SgExpression*>::iterator p = subexprs.begin(); p != subexprs.end(); ++k, p = subexprs.begin() + k)
    {
        if ((*p)->variant() != VAR_REF && (*p)->variant() != ARRAY_REF)
        {
            SgExpression* lhs = (*p)->lhs();
            SgExpression* rhs = (*p)->rhs();
            if (lhs != NULL)
                subexprs.push_back(lhs);
            if (rhs != NULL)
                subexprs.push_back(rhs);
        }
        else
        {
            // array reference subscripts are not real dependencies on loop indices
            if ((*p)->variant() == ARRAY_REF)
                continue;
            string s((*p)->symbol()->identifier());
            refs[s] = *p;
            unparsedRefs[*p] = s;
        }
    }

    for (map<SgExpression*, string>::iterator it = unparsedRefs.begin(); it != unparsedRefs.end(); ++it)
        references.insert(refs[it->second]);
}

void Access::analyze()
{
    const Loop* loop = array->getLoop();
    const vector<BasicBlock>& blocks = loop->getBlocks();
    const map<SgStatement*, int>& blockIn = loop->getBlockIn();
    const vector<SgSymbol*>& symbols = loop->getSymbols();
    int dimension = array->getDimension();
    alignment = new int [dimension];

    for (int i = 0; i < dimension; ++i)
        alignment[i] = -1;

    int i = 0;
    for (SgExpression* expr = this->expr; expr != NULL; ++i, expr = expr->rhs())
    {
        map<SgExpression*, string> unparsedRefs;
        map<string, SgExpression*> refs;
        set<SgExpression*> references, result;
        getReferences(expr->lhs(), references, unparsedRefs, refs);
        result = references;
        map<SgExpression*, set<SgStatement*> > definitions;
        definitions[expr->lhs()] = blocks[blockIndex].INrd;
        bool changed = true;
        while (changed == true)
        {
            changed = false;
            set<SgExpression*> new_references;
            map<SgExpression*, set<SgStatement*> > new_definitions;
            for (set<SgExpression*>::iterator ref = references.begin(); ref != references.end(); ++ref)
            {
                bool found = false;
                for (size_t j = 0; j < symbols.size(); ++j)
                {
                    if (symbols[j] == (*ref)->symbol())
                    {
                        new_references.insert(*ref);
                        result.insert(*ref);
                        found = true;
                        break;
                    }
                }

                if (found == false)
                {
                    for (set<SgStatement*>::iterator def = definitions[*ref].begin(); def != definitions[*ref].end(); ++def)
                    {
                        if (unparsedLhs[(*def)->expr(0)] == unparsedRefs[*ref])
                        {
                            getReferences(rhs[unparsedRhs[(*def)->expr(1)]], new_references, unparsedRefs, refs);
                            for (set<SgExpression*>::iterator it = new_references.begin(); it != new_references.end(); ++it)
                                new_definitions[*it].insert(blocks[blockIn.at(*def)].INrd.begin(), blocks[blockIn.at(*def)].INrd.end());
                            found = true;
                        }
                    }

                    if (found == true)
                        result.erase(*ref);
                }
            }

            if (new_references != references)
            {
                references = new_references;
                definitions = new_definitions;
                changed = true;
            }
        }

        references.clear();
        for (set<SgExpression*>::iterator it = result.begin(); it != result.end(); ++it)
            references.insert(refs[unparsedRefs[*it]]);

        if (references.size() == 1)
        {
            for (size_t j = 0; j < symbols.size(); ++j)
            {
                if (symbols[j] == (*references.begin())->symbol())
                    alignment[i] = j;
            }
        }
        else if (references.size() > 1)
            alignment[i] = -2;
    }

    for (i = 0; i < symbols.size(); ++i)
    {
        int j;
        for (j = 0; j < dimension; ++j)
        {
            if (alignment[j] == i)
                break;
        }

        if (j == dimension)
            break;
    }

    if (i != symbols.size())
    {
        for (int i = 0; i < dimension; ++i)
        {
            if (alignment[i] == -2)
                err((string("array '") + array->getSymbol()->identifier() + "': dependence on multiple loop indices").c_str(), 421, first_do_par);
        }
    }
}

void Array::analyze()
{
    alignment = new int [dimension];
    for (int i = 0; i < dimension; ++i)
        alignment[i] = -1;
    if (accesses.size() == 0)
        return;
    for (map<string, Access*>::iterator it = accesses.begin(); it != accesses.end(); ++it)
        it->second->analyze();

    int* tmp = new int [dimension];
    int* prev = new int [dimension];
    for (int i = 0; i < dimension; ++i)
    {
        prev[i] = -2;
        tmp[i] = accesses.begin()->second->getAlignment()[i];
    }

    for (map<string, Access*>::iterator it1 = accesses.begin(); it1 != accesses.end(); ++it1)
    {
        const int* alignment = it1->second->getAlignment();
        for (int i = 0; i < dimension; ++i)
        {
            if (alignment[i] > tmp[i])
            {
                prev[i] = tmp[i];
                tmp[i] = alignment[i];
            }
        }
    }

    bool success = true;
    for (int i = 0; i < dimension; ++i)
    {
        if (prev[i] >= 0)
        {
            success = false;
            break;
        }
    }

    if (success == true)
    {
        for (int i = 0; i < dimension; ++i)
            alignment[i] = tmp[i];
    }
    else
        err((string("array '") + symbol->identifier() + "': accesses with different subscripts' dependencies were found").c_str(), 422, first_do_par);
}

void Array::analyzeTransformDimensions()
{
    int dimension = loop->getDimension();
    if (dimension <= 1 || loop->getAcrossType() <= 1)
        return;

    int symbols[] = { -1, -1 };
    if (dimension == loop->getAcrossType())
    {
        symbols[0] = dimension - 1;
        symbols[1] = dimension - 2;
    }
    else
    {
        for (size_t i = acrossDims.size() - 1, j = 0; i != 0 && j != 2; --i)
        {
            if (acrossDims[i] == 1)
                symbols[j++] = i;
        }
    }

    int indices[] = { -1, -1 };
    for (int i = 0; i < this->dimension; ++i)
    {
        if (symbols[0] == alignment[i])
            indices[0] = i;
        else if (symbols[1] == alignment[i])
            indices[1] = i;
    }

    if (indices[0] != -1 && indices[1] != -1)
    {
        indices[0] = this->dimension - indices[0];
        indices[1] = this->dimension - indices[1];
    }
    tfmInfo.transformDims.push_back(indices[0]);
    tfmInfo.transformDims.push_back(indices[1]);
}

SgSymbol* Array::findAccess(SgExpression* subscripts, string& expr)
{
    size_t i = 0;
    int j = 0;
    string id;
    for (SgExpression* tmp = subscripts; tmp != NULL && i < 2; ++j, tmp = tmp->rhs())
    {
        if (dimension - j == tfmInfo.transformDims[0] || dimension - j == tfmInfo.transformDims[1])
        {
            id.append(tmp->lhs()->unparse()).append("_");
            ++i;
        }
    }

    SgSymbol* result = NULL;
    for (i = 0; i < tfmInfo.exprs.size(); ++i)
    {
        if (tfmInfo.exprs[i].first == id)
        {
            result = tfmInfo.coefficients[i];
            break;
        }
    }

    if (result == NULL)
        expr = id;
    return result;
}

void Array::addCoefficient(SgExpression* subscripts, string& expr, SgSymbol* symbol)
{
    int i = 0;
    for (SgExpression* tmp = subscripts; tmp != NULL; ++i, tmp = tmp->rhs())
    {
        if (dimension - i == tfmInfo.transformDims[0])
            tfmInfo.first.push_back(tmp->lhs());
        else if (dimension - i == tfmInfo.transformDims[1])
            tfmInfo.second.push_back(tmp->lhs());
    }

    tfmInfo.exprs.push_back(pair<string, string>(expr, subscripts->unparse()));
    tfmInfo.coefficients.push_back(symbol);
}

void Loop::analyzeAcrossClause()
{
    for (SgExpression* expr = dvm_parallel_dir->expr(1); expr != NULL; expr = expr->rhs())
    {
        SgExpression* tmp = expr->lhs();
        if (tmp->variant() == ACROSS_OP)
        {
            vector<SgExpression*> toAnalyze;
            SgExpression* list = tmp->lhs();
            while (list)
            {
                if (list->lhs()->variant() == ARRAY_REF)
                    toAnalyze.push_back(list->lhs());
                else if (list->lhs()->variant() == ARRAY_OP)
                {
                    if (list->lhs()->lhs()->variant() == ARRAY_REF)
                        toAnalyze.push_back(list->lhs()->lhs());
                }
                list = list->rhs();
            }

            for (int k = 0; k < toAnalyze.size(); ++k)
            {
                tmp = toAnalyze[k];
                if (arrays.find(tmp->symbol()) == arrays.end())
                    warn((string("array '") + tmp->symbol()->identifier() + "': unused").c_str(), 900, first_do_par);
                else if (privateList.find(tmp->symbol()) != privateList.end())
                    err((string("array '") + tmp->symbol()->identifier() + "': incompatible qualifiers (ACROSS, PRIVATE)").c_str(), 423, first_do_par);
                else
                {
                    Array* array = arrays[tmp->symbol()];
                    SgExpression* dep = tmp->lhs();
                    int i = 0, raw, war, n = 0;
                    vector<int>& acrossDims = array->getAcrossDims();

                    while (dep != NULL)
                    {
                        raw = dep->lhs()->lhs()->valueInteger();
                        war = dep->lhs()->rhs()->valueInteger();
                        acrossDims[i] = (raw != 0 || war != 0) ? 1 : 0;
                        n += acrossDims[i];
                        i++;
                        dep = dep->rhs();
                    }

                    if (n != 0)
                        array->setAcrossType((1 << n) - 1);

                    for (int j = 0; j < abs(dimension - array->getDimension()); ++j)
                        acrossDims.push_back(-1);
                }
            }
        }
    }
}

void Loop::analyzeAcrossType()
{
    acrossDims = new int [dimension];
    for (int i = 0; i < dimension; ++i)
        acrossDims[i] = -1;

    for (map<SgSymbol*, Array*>::iterator it = arrays.begin(); it != arrays.end(); ++it)
    {
        const int* alignment = it->second->getAlignment();
        vector<int>& _acrossDims = it->second->getAcrossDims();
        if (alignment != NULL)
        {
            for (int i = 0; i < it->second->getDimension(); ++i)
            {
                if (alignment[i] != -1)
                    acrossDims[alignment[i]] = max(acrossDims[alignment[i]], _acrossDims[alignment[i]]);
            }
        }
    }

    for (int i = 0; i < dimension; ++i)
    {
        if (acrossDims[i] != -1)
            ++acrossType;
    }
}

void Array::generateAssigns(SgVarRefExp* offsetX, SgVarRefExp* offsetY, SgVarRefExp* Rx, SgVarRefExp* Ry, SgVarRefExp* slash)
{
    if (tfmInfo.ifCalls.size() == 0 && tfmInfo.elseCalls.size() == 0 && tfmInfo.zeroSt.size() == 0)
    {
        for (size_t i = 0; i < tfmInfo.coefficients.size(); ++i)
        {
            tfmInfo.zeroSt.push_back(AssignStatement(new SgVarRefExp(tfmInfo.coefficients[i]->copy()), new SgValueExp(0)));
            
            SgFunctionCallExp* funcCallExpIf = createNewFCall(funcDvmhConvXYname);
            SgFunctionCallExp* funcCallExpElse = createNewFCall(funcDvmhConvXYname);

            funcCallExpIf->addArg(*new SgCastExp(*offsetX->type(), tfmInfo.first[i]->copy()) - *offsetX);
            funcCallExpIf->addArg(*new SgCastExp(*offsetY->type(), tfmInfo.second[i]->copy()) - *offsetY);
            funcCallExpIf->addArg(*Rx);
            funcCallExpIf->addArg(*Ry);
            funcCallExpIf->addArg(*slash);
            funcCallExpIf->addArg(*new SgVarRefExp(tfmInfo.coefficients[i]->copy()));

            funcCallExpElse->addArg(*new SgCastExp(*offsetX->type(), tfmInfo.second[i]->copy()) - *offsetX);
            funcCallExpElse->addArg(*new SgCastExp(*offsetY->type(), tfmInfo.first[i]->copy()) - *offsetY);
            funcCallExpElse->addArg(*Rx);
            funcCallExpElse->addArg(*Ry);
            funcCallExpElse->addArg(*slash);
            funcCallExpElse->addArg(*new SgVarRefExp(tfmInfo.coefficients[i]->copy()));

            SgStatement* stmt = NULL;
            set<Access*> _accesses;
            for (map<string, Access*>::iterator it = accesses.begin(); it != accesses.end(); ++it)
            {
                bool found[2] = { false, false };
                string first(tfmInfo.first[i]->unparse());
                string second(tfmInfo.second[i]->unparse());
                for (SgExpression* tmp = it->second->getSubscripts(); tmp != NULL; tmp = tmp->rhs())
                {
                    string s(tmp->lhs()->unparse());
                    if (s == first)
                        found[0] = true;
                    else if (s == second)
                        found[1] = true;
                }
                if (found[0] == true && found[1] == true)
                    _accesses.insert(it->second);
            }

            map<Access*, set<int> > blockIndices;
            int minIndex = loop->getBlocks().size();
            for (set<Access*>::iterator it = _accesses.begin(); it != _accesses.end(); ++it)
            {
                set<SgSymbol*> symbols;
                int j = 0;
                for (SgExpression* tmp = (*it)->getSubscripts(); tmp != NULL; tmp = tmp->rhs())
                {
                    if (dimension - j != tfmInfo.transformDims[0] && dimension - j != tfmInfo.transformDims[1])
                        continue;
                    vector<SgExpression*> _subtrees;
                    _subtrees.push_back(tmp->lhs());
                    int k = 0;
                    for (vector<SgExpression*>::iterator p = _subtrees.begin(); p != _subtrees.end(); ++k, p = _subtrees.begin() + k)
                    {
                        if ((*p)->variant() == VAR_REF && (*p)->symbol() != NULL)
                            symbols.insert((*p)->symbol());
                        else
                        {
                            SgExpression* lhs = (*p)->lhs();
                            SgExpression* rhs = (*p)->rhs();
                            if (lhs != NULL)
                                _subtrees.push_back(lhs);
                            if (rhs != NULL)
                                _subtrees.push_back(rhs);
                        }
                    }
                }

                set<SgSymbol*> _symbols(loop->getSymbols().begin(), loop->getSymbols().end());
                set<SgSymbol*> diff;
                difference(symbols.begin(), symbols.end(), _symbols.begin(), _symbols.end(), inserter(diff, diff.end()));
                const vector<BasicBlock>& blocks = loop->getBlocks();

                if (diff.size() != 0)
                {
                    set<int> preds(blocks[(*it)->getBlockIndex()].in.begin(), blocks[(*it)->getBlockIndex()].in.end());
                    bool changed = true;
                    while (changed == true)
                    {
                        changed = false;
                        set<int> new_preds(preds);
                        for (set<int>::iterator pred = preds.begin(); pred != preds.end(); ++pred)
                            new_preds.insert(blocks[*pred].in.begin(), blocks[*pred].in.end());

                        if (preds != new_preds)
                        {
                            preds = new_preds;
                            changed = true;
                        }
                    }
                    blockIndices[*it].insert(preds.begin(), preds.end());
                }
                else
                    blockIndices[*it].insert(0);

                minIndex = min(minIndex, (*it)->getBlockIndex());
            }
            set<int> common_preds;
            for (set<Access*>::iterator it = _accesses.begin(); it != _accesses.end(); ++it)
                common_preds.insert(blockIndices[*it].begin(), blockIndices[*it].end());

            for (set<Access*>::iterator it = _accesses.begin(); it != _accesses.end(); ++it)
            {
                if (blockIndices[*it].size() == 1 && *blockIndices[*it].begin() == 0)
                    continue;
                else
                {
                    set<int> tmp;
                    intersection(common_preds.begin(), common_preds.end(), blockIndices[*it].begin(), blockIndices[*it].end(), inserter(tmp, tmp.end()));
                    common_preds = tmp;
                }
            }

            int max = 0;
            for (set<int>::iterator it = common_preds.begin(); it != common_preds.end(); ++it)
            {
                if (*it < minIndex)
                {
                    if (*it > max)
                        max = *it;
                }
            }

            stmt = loop->getBlocks()[max].head;
            tfmInfo.ifCalls[stmt].push_back(funcCallExpIf);
            tfmInfo.elseCalls[stmt].push_back(funcCallExpElse);
        }
    }
}

bool Loop::irregularAnalysisIsOn() const
{
    return do_irreg_opt;
}

static bool isOnlyParS(SgExpression* ex, SgSymbol* parS)
{
    bool ret = true;
    if (ex)
    {
        if (ex->variant() != VAR_REF || ex->variant() == CONST_REF)
            return false;
        if (ex->variant() == VAR_REF)
            if (ex->symbol()->identifier() != string(parS->identifier()))
                return false;

        bool left = isOnlyParS(ex->lhs(), parS);
        bool right = isOnlyParS(ex->rhs(), parS);
        ret = left && right;
    }
    return ret;
}

static void analyzeExpr(SgExpression* ex, SgSymbol* parS, int arrayLvl, bool& needOpt, bool& wasInderectAccess)
{
    if (ex)
    {
        if (ex->variant() == ARRAY_REF)
        {
            if (arrayLvl > 0)
                wasInderectAccess = true;
            arrayLvl++;
            if (isOnlyParS(ex->lhs(), parS) == false)
                needOpt = true;
        }

        analyzeExpr(ex->lhs(), parS, arrayLvl, needOpt, wasInderectAccess);
        analyzeExpr(ex->rhs(), parS, arrayLvl, needOpt, wasInderectAccess);
    }
}

void Loop::analyzeInderectAccess()
{
    if (symbols.size() != 1)
        return;

    SgStatement* stmt = loop_body;
    bool wasInderectAccess = false;
    bool needOpt = false;
    while (stmt)
    {
        for (int z = 0; z < 3; ++z)
            analyzeExpr(stmt->expr(z), symbols[0], 0, needOpt, wasInderectAccess);
        stmt = stmt->lexNext();
    }

    if (wasInderectAccess && needOpt)
        do_irreg_opt = true;
}

Loop::Loop(SgStatement* loop_body, bool enable_opt, bool irreg_access) : 
    irregular_acc_opt(irreg_access), enable_opt(enable_opt), loop_body(loop_body), 
    dimension(0), acrossType(0), acrossDims(NULL), do_irreg_opt(false)
{    
    lhs.clear();
    rhs.clear();
    unparsedLhs.clear();
    unparsedRhs.clear();

    buildCFG();
    setupSubstitutes();
    for (int i = 2; i < blocks.size(); ++i)
        if (blocks[i].head != NULL && (blocks[i].head->variant() == ASSIGN_STAT || blocks[i].head->variant() == PROC_STAT))
            analyzeAssignments(blocks[i].index, blocks[i].head);

    for (SgExpression* tmp = dvm_parallel_dir->expr(2); tmp != NULL; tmp = tmp->rhs())
    {
        symbols.push_back(tmp->lhs()->symbol());
        ++dimension;
    }

    for (SgExpression* tmp = dvm_parallel_dir->expr(1); tmp != NULL; tmp = tmp->rhs())
    {
        SgExpression* t = tmp->lhs();
        if (t->variant() == ACC_PRIVATE_OP)
        {
            for (t = t->lhs(); t != NULL; t = t->rhs())
            {
                if (isSgArrayType(t->lhs()->symbol()->type()) != NULL)
                    privateList.insert(t->lhs()->symbol());
            }
        }
    }

    SgSymbol* symbol = NULL;
    SgExpression* subscripts = NULL;

    if (dvm_parallel_dir->expr(0))
    {
        symbol = dvm_parallel_dir->expr(0)->symbol();
        subscripts = ((SgArrayRefExp*)dvm_parallel_dir->expr(0))->subscripts();
    }
    else // TIE
    {
        SgExpression* arc = findDirect(dvm_parallel_dir->expr(1), ACROSS_OP);
        SgExpression* tie = findDirect(dvm_parallel_dir->expr(1), ACC_TIE_OP);

        if (arc != NULL && tie == NULL)
        {
            err("internal error in across", 424, first_do_par);
            exit(-1);
        }
        else if (arc && tie)
        {
            map<string, SgExpression*> acrossArrays, tieArrays;
            SgExpression* ex = arc->lhs();
            while (ex)
            {
                acrossArrays[ex->lhs()->symbol()->identifier()] = ex->lhs();
                ex = ex->rhs();
            }
            ex = tie->lhs();
            while (ex)
            {
                tieArrays[ex->lhs()->symbol()->identifier()] = ex->lhs();
                ex = ex->rhs();
            }

            bool errM = false;
            for (map<string, SgExpression*>::iterator acrA = acrossArrays.begin(); acrA != acrossArrays.end(); acrA++)
            {
                if (tieArrays.find(acrA->first) == tieArrays.end())
                {
                    errM = true;
                    err((string("can not find array '") + acrA->first + "' in TIE clause").c_str(), 425, first_do_par);
                }
            }
            if (errM)
                exit(-1);

            //TODO: multiple arrays
            for (map<string, SgExpression*>::iterator acrA = acrossArrays.begin(); acrA != acrossArrays.end(); acrA++)
            {
                SgExpression* firstTie = tieArrays[acrA->first];
                symbol = firstTie->symbol();
                subscripts = ((SgArrayRefExp*)firstTie)->subscripts();
                break;
            }
        }
        else
        {
            if (irreg_access)
                analyzeInderectAccess();
            return;
        }
    }
    //TODO: tmp is undefined in this scope
    if (arrays.find(symbol) == arrays.end())
        warn((string("array '") + symbol->identifier() + "': unused").c_str(), 900, first_do_par);

    for (map<SgSymbol*, Array*>::iterator it1 = arrays.begin(); it1 != arrays.end(); ++it1)
    {
        if (privateList.find(it1->second->getSymbol()) == privateList.end())
            it1->second->analyze();
    }

    // ACROSS_ANALYZER
    if (WithAcrossClause() == 0)
    {
        if (irreg_access)
            analyzeInderectAccess();
        return;
    }

    analyzeAcrossClause();
    vector<int> acrossDims(symbols.size(), -1);
    if (arrays.find(symbol) != arrays.end())
        acrossDims = arrays[symbol]->getAcrossDims();

    size_t i;
    for (i = 0; i < symbols.size(); ++i)
    {
        if (acrossDims[i] != -1)
            break;
        if (i == symbols.size())
            err((string("array '") + symbol->identifier() + "': mapped on different template than corresponding parallel loop").c_str(), 424, first_do_par);
    }

    analyzeAcrossType();
    if (acrossType > 1)
    {
        for (map<SgSymbol*, Array*>::iterator it1 = arrays.begin(); it1 != arrays.end(); ++it1)
        {
            if (privateList.find(it1->second->getSymbol()) == privateList.end())
                it1->second->analyzeTransformDimensions();
        }
    }

#if 0
    printf("Loop indices(%d):", dimension);
    for (vector<SgSymbol*>::iterator it = symbols.begin(); it != symbols.end(); ++it)
        printf(" %s", (*it)->identifier());
    printf("\n");
    printf("Private arrays:");
    for (set<SgSymbol*>::iterator it = privateList.begin(); it != privateList.end(); ++it)
        printf(" \"%s\"", (*it)->identifier());
    printf("\n");
    for (map<SgSymbol*, Array*>::iterator it1 = arrays.begin(); it1 != arrays.end(); ++it1)
    {
        if (privateList.find(it1->first) == privateList.end())
        {
            printf("Array %s:", it1->second->getSymbol()->identifier());
            for (int i = 0; i < it1->second->getDimension(); ++i)
                printf(" %d", it1->second->getAlignment()[i]);
            printf("\n");
        }
        printf("  AcrossDims:");
        for (vector<int>::iterator it2 = it1->second->getAcrossDims().begin(); it2 != it1->second->getAcrossDims().end(); ++it2)
            printf(" %d", *it2);
        printf("\n");
        printf("  AcrossType: %d\n", it1->second->getAcrossType());
        if (privateList.find(it1->first) == privateList.end())
        {
            printf("  TransformDims:");
            for (vector<int>::iterator it2 = it1->second->getTfmInfo().transformDims.begin(); it2 != it1->second->getTfmInfo().transformDims.end(); ++it2)
                printf(" %d", *it2);
            printf("\n");
            for (map<string, Access*>::iterator it2 = it1->second->getAccesses().begin(); it2 != it1->second->getAccesses().end(); ++it2)
            {
                printf("    Access:");
                for (int i = 0; i < it1->second->getDimension(); ++i)
                    printf(" %d", it2->second->getAlignment()[i]);
                printf("\n");
            }
        }
    }
    printf("  LoopAcrossType: %d\n", acrossType);
    printf("  LoopAcrossDims:");
    for (int i = 0; i < dimension; ++i)
        printf(" %d", acrossDims[i]);
    printf("\n");
    char* scriptName = new char[64];
    sprintf(scriptName, "cfg.loop_%d.gv", first_do_par->lineNumber());
    visualize(scriptName);
    delete[]scriptName;
    printf("############################################################\n");
#endif
}

void Loop::analyzeAssignments(SgExpression* ex, const int blockIndex)
{
    if (ex->variant() != ARRAY_REF)
    {
        SgExpression* lhs = ex->lhs();
        SgExpression* rhs = ex->rhs();
        if (lhs)
            analyzeAssignments(lhs, blockIndex);
        if (rhs)
            analyzeAssignments(rhs, blockIndex);
    }
    else
    {
        SgSymbol* symbol = ex->symbol();
        if (isSgArrayType(symbol->type()) != NULL)
        {
            SgExpression* subscripts = ((SgArrayRefExp*)(ex))->subscripts();
            if (!subscripts)
                return;

            for (SgExpression* tmp = subscripts; tmp != NULL; tmp = tmp->rhs())
                tmp->setLhs(simplify(tmp->lhs()));

            string s(subscripts->unparse());
            if (arrays.find(symbol) == arrays.end())
            {
                Array* array = new Array(symbol, isSgArrayType(symbol->type())->dimension(), this);
                arrays[symbol] = array;
                array->getAccesses()[s] = new Access(subscripts, s, array, blockIndex);
            }
            else
            {
                Array* array = arrays[symbol];
                if (array->getAccesses().find(s) == array->getAccesses().end())
                    array->getAccesses()[s] = new Access(subscripts, s, array, blockIndex);
            }
        }
    }
}

void Loop::analyzeAssignments(int blockIndex, SgStatement* stmt)
{
    for (int i = 0; i < 3; ++i)
        if (stmt->expr(i))
            analyzeAssignments(stmt->expr(i), blockIndex);
}

inline bool Loop::IsTargetable(SgStatement* stmt) const
{
    return stmt != NULL
        && stmt->variant() != ELSEIF_NODE
        && stmt->variant() != CASE_NODE
        && stmt->variant() != DEFAULT_NODE
        && stmt->variant() != CONTROL_END;
}

void Loop::buildCFG()
{
    SgStatement* stmt = loop_body;
    map<SgStatement*, SgStatement*> controlFlow;
    map<int, vector<SgStatement*> > blockOut;

    map<int, set<string> > GENae, KILLae, INae, OUTae;
    map<string, vector<SgStatement*> > EXTRA;
    map<int, set<SgStatement*> > GENrd, KILLrd;
    map<int, map<string, SgStatement*> > blockAssignments;
    map<SgStatement*, string> assignments;
    set<string> allStmts;

    BasicBlock entry;
    entry.index = ENTRY;
    BasicBlock exit;
    exit.index = EXIT;
    blockOut[ENTRY].push_back(stmt);
    blockIn[NULL] = EXIT;
    blocks.push_back(entry);
    blocks.push_back(exit);
    int i = 2;

    while (stmt != NULL)
    {
        BasicBlock block;
        block.index = i;
        block.head = stmt;
        blockIn[stmt] = i;
        vector<SgStatement*>& out = blockOut[i];
        list<string> stmts;

        while (stmt != NULL)
        {
            bool tail = true;
            switch (stmt->variant())
            {
                case WHERE_NODE:
                    break;
                case WHERE_BLOCK_STMT:
                    break;
                case ELSEWH_NODE:
                    break;
                case SWITCH_NODE:
                {
                    SgSwitchStmt* _stmt = (SgSwitchStmt*)stmt;
                    controlFlow[_stmt] = IsTargetable(_stmt->lastNodeOfStmt()->lexNext())
                        && _stmt->lastNodeOfStmt()->lexNext()->controlParent() == _stmt->controlParent() ? _stmt->lastNodeOfStmt()->lexNext() : controlFlow[_stmt->controlParent()];

                    if (_stmt->caseOption(0) == NULL)
                    {
                        if (_stmt->defOption() == NULL)
                            out.push_back(controlFlow[_stmt]);
                        else
                            out.push_back(_stmt->defOption());
                    }
                    else
                        out.push_back(_stmt->caseOption(0));
                    break;
                }
                case CASE_NODE:
                {
                    SgSwitchStmt* switchStmt = ((SgSwitchStmt*)stmt->controlParent());
                    controlFlow[stmt] = controlFlow[switchStmt];
                    int i;
                    for (i = 0; i < switchStmt->numberOfCaseOptions() && stmt != switchStmt->caseOption(i); i++);

                    SgStatement* nextStmt = stmt->lexNext();
                    if (nextStmt->variant() != CASE_NODE && nextStmt->variant() != DEFAULT_NODE && nextStmt->variant() != CONTROL_END)
                        out.push_back(nextStmt);

                    if (i == switchStmt->numberOfCaseOptions() - 1)
                    {
                        if (switchStmt->defOption() != NULL)
                            out.push_back(switchStmt->defOption());
                        else
                            out.push_back(controlFlow[stmt]);
                    }
                    else
                        out.push_back(switchStmt->caseOption(i + 1));
                    break;
                }
                case DEFAULT_NODE:
                {
                    controlFlow[stmt] = controlFlow[stmt->controlParent()];
                    SgStatement* nextStmt = stmt->lexNext();

                    if (nextStmt->variant() != CASE_NODE && nextStmt->variant() != CONTROL_END)
                        out.push_back(nextStmt);
                    out.push_back(controlFlow[stmt]);
                    break;
                }
                case ARITHIF_NODE:
                    // something wrong with SgArithIfStmt::label(...) method, this seems ok
                    out.push_back(StmtWithLabel(((SgLabelRefExp*)LlndMapping(getPositionInExprList(BIF_LL2(stmt->thebif), 0)))->label()));
                    out.push_back(StmtWithLabel(((SgLabelRefExp*)LlndMapping(getPositionInExprList(BIF_LL2(stmt->thebif), 1)))->label()));
                    out.push_back(StmtWithLabel(((SgLabelRefExp*)LlndMapping(getPositionInExprList(BIF_LL2(stmt->thebif), 2)))->label()));
                    break;
                case IF_NODE:
                {
                    SgStatement* falseBody = ((SgIfStmt*)stmt)->falseBody();
                    SgStatement* _stmt = stmt;
                    while (falseBody != NULL && falseBody->variant() == ELSEIF_NODE)
                    {
                        _stmt = falseBody;
                        falseBody = ((SgIfStmt*)falseBody)->falseBody();
                    }

                    controlFlow[stmt] = IsTargetable(_stmt->lastNodeOfStmt()->lexNext())
                        && _stmt->lastNodeOfStmt()->lexNext()->controlParent() == stmt->controlParent() ? _stmt->lastNodeOfStmt()->lexNext() : controlFlow[stmt->controlParent()];

                    SgStatement* trueBody = ((SgIfStmt*)stmt)->trueBody();
                    falseBody = ((SgIfStmt*)stmt)->falseBody();
                    bool trueBodyCond = trueBody != NULL && trueBody->variant() != CONTROL_END;
                    bool falseBodyCond = falseBody != NULL && falseBody->variant() != CONTROL_END;

                    if (trueBodyCond == true)
                        out.push_back(trueBody);

                    if (falseBodyCond == true)
                        out.push_back(falseBody);

                    if (trueBodyCond == false || falseBodyCond == false)
                        out.push_back(controlFlow[stmt]);
                    break;
                }
                case ELSEIF_NODE:
                {
                    controlFlow[stmt] = controlFlow[stmt->controlParent()];
                    SgStatement* trueBody = ((SgIfStmt*)stmt)->trueBody();
                    SgStatement* falseBody = ((SgIfStmt*)stmt)->falseBody();
                    bool trueBodyCond = trueBody != NULL && trueBody->variant() != CONTROL_END;
                    bool falseBodyCond = falseBody != NULL && falseBody->variant() != CONTROL_END;
                    if (trueBodyCond == true)
                        out.push_back(trueBody);

                    if (falseBodyCond == true)
                        out.push_back(falseBody);

                    if (trueBodyCond == false || falseBodyCond == false)
                        out.push_back(controlFlow[stmt]);
                    break;
                }
                case LOGIF_NODE:
                    controlFlow[stmt] = IsTargetable(stmt->lastNodeOfStmt()->lexNext())
                        && stmt->lastNodeOfStmt()->lexNext()->controlParent() == stmt->controlParent() ? stmt->lastNodeOfStmt()->lexNext() : controlFlow[stmt->controlParent()];
                    out.push_back(((SgLogIfStmt*)stmt)->body());
                    out.push_back(controlFlow[stmt]);
                    break;
                case WHILE_NODE:
                {
                    SgWhileStmt* _stmt = (SgWhileStmt*)stmt;
                    controlFlow[stmt] = stmt;
                    out.push_back(_stmt->body());
                    SgStatement* st = _stmt->body();
                    while (st != NULL && st->controlParent() != stmt->controlParent())
                        st = st->lexNext();

                    SgStatement* nextStmt = IsTargetable(st)
                        && st->controlParent() == stmt->controlParent() ? st : controlFlow[stmt->controlParent()];

                    out.push_back(nextStmt);
                    break;
                }
                case COMGOTO_NODE:
                {
                    SgComputedGotoStmt* _stmt = (SgComputedGotoStmt*)stmt;
                    controlFlow[_stmt] = IsTargetable(_stmt->lastNodeOfStmt()->lexNext())
                        && _stmt->lastNodeOfStmt()->lexNext()->controlParent() == _stmt->controlParent() ? _stmt->lastNodeOfStmt()->lexNext() : controlFlow[_stmt->controlParent()];

                    SgExpression* labelList = _stmt->labelList();
                    for (int i = 0; i < _stmt->numberOfTargets(); i++, labelList = labelList->rhs())
                        out.push_back(StmtWithLabel(((SgLabelRefExp*)labelList->lhs())->label()));

                    out.push_back(controlFlow[_stmt]);
                    break;
                }
                case FOR_NODE:
                {
                    SgForStmt* _stmt = (SgForStmt*)stmt;
                    controlFlow[_stmt] = _stmt;
                    out.push_back(_stmt->body());
                    SgStatement* st = _stmt->body();
                    while (st != NULL && st->controlParent() != _stmt->controlParent())
                        st = st->lexNext();
                    SgStatement* nextStmt = IsTargetable(st)
                        && st->controlParent() == _stmt->controlParent() ? st : controlFlow[_stmt->controlParent()];
                    out.push_back(nextStmt);
                    if (_stmt->symbol() != NULL)
                    {
                        SgStatement* inc = new SgAssignStmt(*new SgVarRefExp(_stmt->symbol()), *new SgVarRefExp(_stmt->symbol()) + (_stmt->step() != NULL ? *new SgValueExp(_stmt->step()->valueInteger()) : *new SgValueExp(1)));
                        blockAssignments[i][inc->expr(0)->unparse()] = inc;
                        for (list<string>::iterator it = stmts.begin(); it != stmts.end();)
                        {
                            if (EXTRA[*it][0]->expr(1)->IsSymbolInExpression(*_stmt->symbol()) != NULL)
                                it = stmts.erase(it);
                            else
                                ++it;
                        }
                    }
                    break;
                }
                case GOTO_NODE:
                    out.push_back(StmtWithLabel(((SgGotoStmt*)stmt)->branchLabel()));
                    break;
                case EXIT_STMT:
                {
                    SgExitStmt* _stmt = (SgExitStmt*)stmt;
                    SgSymbol* constructName = _stmt->constructName();
                    SgStatement* parent = _stmt->controlParent();
                    if (constructName != NULL)
                        while (parent != NULL && ((parent->variant() != FOR_NODE && parent->variant() != WHILE_NODE) || strcmp(LlndMapping(BIF_LL3(parent->thebif))->unparse(), constructName->identifier()) != 0))
                            parent = parent->controlParent();
                    else
                        while (parent != NULL && parent->variant() != FOR_NODE && parent->variant() != WHILE_NODE)
                            parent = parent->controlParent();
                    if (parent != NULL)
                    {
                        SgStatement* st = ((SgForStmt*)parent)->body();
                        while (st != NULL && st->controlParent() != parent->controlParent())
                            st = st->lexNext();
                        out.push_back((IsTargetable(st) && st->controlParent() == parent->controlParent()) ? st : controlFlow[parent->controlParent()]);
                    }
                    else
                        out.push_back(NULL);//jump to parallel DOs
                    break;
                }
                case CYCLE_STMT:
                {
                    SgCycleStmt* _stmt = (SgCycleStmt*)stmt;
                    SgSymbol* constructName = _stmt->constructName();
                    SgStatement* parent = _stmt->controlParent();
                    if (constructName != NULL)
                        while (parent != NULL && ((parent->variant() != FOR_NODE && parent->variant() != WHILE_NODE) || strcmp(LlndMapping(BIF_LL3(parent->thebif))->unparse(), constructName->identifier()) != 0))
                            parent = parent->controlParent();
                    else
                        while (parent != NULL && parent->variant() != FOR_NODE && parent->variant() != WHILE_NODE)
                            parent = parent->controlParent();
                    out.push_back(parent);
                    break;
                }
                case ASSIGN_STAT:
                {
                    string s0(simplify(stmt->expr(0))->unparse());
                    string s1(simplify(stmt->expr(1))->unparse());
                    unparsedLhs[stmt->expr(0)] = s0;
                    unparsedRhs[stmt->expr(1)] = s1;
                    lhs[s0] = stmt->expr(0);
                    rhs[s1] = stmt->expr(1);
                    if (s0 != s1)
                    {
                        if (stmt->expr(0)->variant() == ARRAY_REF)
                        {
                            bool success = true;
                            for (SgExpression* tmp = ((SgArrayRefExp*)stmt->expr(0))->subscripts(); tmp != NULL; tmp = tmp->rhs())
                            {
                                if (tmp->lhs()->variant() != CONST_REF)
                                {
                                    success = false;
                                    break;
                                }
                            }

                            if (success == true)
                                blockAssignments[i][s0] = stmt;
                            else
                                blockAssignments[i][stmt->expr(0)->symbol()->identifier()] = stmt;
                        }
                        else
                            blockAssignments[i][s0] = stmt;

                        GENrd[i].insert(stmt);
                        assignments[stmt] = s1;
                        EXTRA[s1].push_back(stmt);
                        stmts.push_back(s1);
                        allStmts.insert(s1);

                        for (list<string>::iterator it = stmts.begin(); it != stmts.end();)
                        {
                            if (FindInExpr(stmt->expr(0), EXTRA[*it][0]->expr(1)) != 0)
                                it = stmts.erase(it);
                            else
                                ++it;
                        }
                    }
                }
                default:
                {
                    if (stmt->hasLabel() == false)
                        tail = false;
                    else
                    {
                        SgStatement* parent = stmt->controlParent();
                        while (parent != NULL && (parent->variant() == FOR_NODE || parent->variant() == WHILE_NODE))
                        {
                            if (BIF_LABEL_USE(parent->thebif) != NULL && LABEL_STMTNO(BIF_LABEL_USE(parent->thebif)) == LABEL_STMTNO(stmt->label()->thelabel))
                                out.push_back(parent);
                            parent = parent->controlParent();
                        }
                        if (out.size() != 0)
                            break;
                    }

                    SgStatement* _stmt = stmt->lexNext();
                    if (_stmt != NULL)
                    {
                        switch (_stmt->variant())
                        {
                        case FOR_NODE:
                        case WHILE_NODE:
                        case WHERE_NODE:
                        case WHERE_BLOCK_STMT:
                            tail = true;
                            out.push_back(_stmt);
                            break;
                        case ELSEIF_NODE:
                        case ELSEWH_NODE:
                        case CASE_NODE:
                        case DEFAULT_NODE:
                        case CONTROL_END:
                            tail = true;
                            out.push_back(controlFlow[_stmt->controlParent()]);
                            break;
                        case FORMAT_STAT:
                            tail = false;
                            break;
                        default:
                            if (_stmt->hasLabel() == false)
                            {
                                //tail = false;break;// builds CFG of Extended Basic Blocks
                                tail = true;
                                out.push_back(_stmt);
                                break;
                            }
                            else
                            {
                                SgStatement* parent = _stmt->controlParent();
                                while (parent != NULL && (parent->variant() == FOR_NODE || parent->variant() == WHILE_NODE))
                                {
                                    if (BIF_LABEL_USE(parent->thebif) != NULL && LABEL_STMTNO(BIF_LABEL_USE(parent->thebif)) == LABEL_STMTNO(_stmt->label()->thelabel))
                                    {
                                        tail = false;
                                        break;
                                    }
                                    parent = parent->controlParent();
                                }
                                //can't find way to get stmts referencing this label
                                //just start new block even if label is not referenced
                                tail = true;
                                out.push_back(_stmt);
                                break;
                            }
                            break;
                        }
                    }
                    else
                        out.push_back(NULL);
                    break;
                }
            }

            if (tail == true)
            {
                GENae[i].insert(stmts.begin(), stmts.end());
                block.tail = stmt;
                blocks.push_back(block);
            }

            stmt = stmt->lexNext();
            while (stmt != NULL && stmt->variant() == CONTROL_END)
                stmt = stmt->lexNext();

            if (tail == true)
                break;
        }
        i++;
    }

    for (map<int, vector<SgStatement*> >::iterator it1 = blockOut.begin(); it1 != blockOut.end(); ++it1)
    {
        for (vector<SgStatement*>::iterator it2 = it1->second.begin(); it2 != it1->second.end(); ++it2)
        {
            blocks[it1->first].out.push_back(blockIn[*it2]);
            blocks[blockIn[*it2]].in.push_back(it1->first);
        }
    }
    blockOut.clear();
    controlFlow.clear();

    for (vector<BasicBlock>::iterator block = blocks.begin() + 2; block != blocks.end(); ++block)
    {
        map<string, SgStatement*>* bAssignments = &blockAssignments[block->index];
        for (map<SgStatement*, string>::iterator it = assignments.begin(); it != assignments.end(); ++it)
        {
            SgStatement* stmt = NULL;
            SgExpression* lhs = it->first->expr(0);
            if (it->first->expr(0)->variant() == ARRAY_REF)
                stmt = bAssignments->find(lhs->symbol()->identifier()) != bAssignments->end() ?
                (*bAssignments)[lhs->symbol()->identifier()] : (*bAssignments)[unparsedLhs[lhs]];
            else
                stmt = (*bAssignments)[unparsedLhs[lhs]];

            if (stmt != NULL && stmt != it->first && blockIn[it->first] != block->index)
                KILLrd[block->index].insert(it->first);
        }

        for (SgStatement* stmt = block->head; stmt != block->tail->lexNext(); stmt = stmt->lexNext())
        {
            if (stmt == NULL)
                continue;
            if (stmt->variant() == ASSIGN_STAT || stmt->variant() == FOR_NODE)
            {
                SgExpression* expr = stmt->variant() == ASSIGN_STAT ? stmt->expr(0) : (*bAssignments)[stmt->symbol()->identifier()]->expr(0);
                for (map<SgStatement*, string>::iterator it = assignments.begin(); it != assignments.end(); ++it)
                    if (FindInExpr(expr, it->first->expr(1)) != 0)
                        KILLae[block->index].insert(it->second);
            }
        }
        block->OUTrd.swap(GENrd[block->index]);
        difference(allStmts.begin(), allStmts.end(), KILLae[block->index].begin(), KILLae[block->index].end(), inserter(OUTae[block->index], OUTae[block->index].end()));
    }
    allStmts.clear();
    assignments.clear();
    blockAssignments.clear();


    bool changed = true;
    while (changed == true)
    {
        changed = false;
        for (vector<BasicBlock>::iterator block = blocks.begin() + 2; block != blocks.end(); ++block)
        {
            for (vector<int>::iterator it = block->in.begin(); it != block->in.end(); ++it)
                block->INrd.insert(blocks[*it].OUTrd.begin(), blocks[*it].OUTrd.end());
            set<SgStatement*> newOUTrd(GENrd[block->index].begin(), GENrd[block->index].end());
            difference(block->INrd.begin(), block->INrd.end(), KILLrd[block->index].begin(), KILLrd[block->index].end(), inserter(newOUTrd, newOUTrd.end()));
            if (newOUTrd != block->OUTrd)
            {
                block->OUTrd.swap(newOUTrd);
                changed = true;
            }
        }
    }
    GENrd.clear();
    KILLrd.clear();

    changed = true;
    while (changed == true)
    {
        changed = false;
        for (vector<BasicBlock>::iterator block = blocks.begin() + 2; block != blocks.end(); ++block)
        {
            if (block->in.size() != 0)
            {
                INae[block->index] = set<string>(OUTae[block->in[0]].begin(), OUTae[block->in[0]].end());
                for (vector<int>::iterator it = block->in.begin() + 1; it != block->in.end(); ++it)
                {
                    set<string> tmp;
                    intersection(INae[block->index].begin(), INae[block->index].end(), OUTae[*it].begin(), OUTae[*it].end(), inserter(tmp, tmp.end()));
                    INae[block->index].swap(tmp);
                }
            }
            set<string> _union(GENae[block->index].begin(), GENae[block->index].end());
            _union.insert(INae[block->index].begin(), INae[block->index].end());
            set<string> newOUTae;
            difference(_union.begin(), _union.end(), KILLae[block->index].begin(), KILLae[block->index].end(), inserter(newOUTae, newOUTae.end()));
            if (newOUTae != OUTae[block->index])
            {
                OUTae[block->index].swap(newOUTae);
                changed = true;
            }
        }
    }
    GENae.clear();
    KILLae.clear();

    for (vector<BasicBlock>::iterator block = blocks.begin() + 2; block != blocks.end(); ++block)
    {
        for (set<string>::iterator it1 = INae[block->index].begin(); it1 != INae[block->index].end(); ++it1)
            block->INae.insert(EXTRA[*it1].begin(), EXTRA[*it1].end());

        for (set<string>::iterator it1 = OUTae[block->index].begin(); it1 != OUTae[block->index].end(); ++it1)
            block->OUTae.insert(EXTRA[*it1].begin(), EXTRA[*it1].end());
    }
}

Loop::Loop(SgStatement* stmt) : do_irreg_opt(false)
{    
    lhs.clear(); rhs.clear(); unparsedLhs.clear(); unparsedRhs.clear();
    buildCFG();
}

set<SgStatement*> Loop::RDsAt(SgStatement* stmt) const
{
    if (blockIn.find(stmt) == blockIn.end() || !(0 <= blockIn.at(stmt) && blockIn.at(stmt) < blocks.size()))
    {
        return set<SgStatement*>();
    }
    return blocks[blockIn.at(stmt)].INrd;
}

set<SgStatement*> Loop::AEsAt(SgStatement* stmt) const
{
    if (blockIn.find(stmt) == blockIn.end() || !(0 <= blockIn.at(stmt) && blockIn.at(stmt) < blocks.size()))
    {
        return set<SgStatement*>();
    }
    return blocks[blockIn.at(stmt)].INae;
}

void Loop::setupSubstitutes()
{
    for (vector<BasicBlock>::iterator block = blocks.begin() + 2; block != blocks.end(); ++block)
    {
        set<SgStatement*> ss;
        intersection(block->INrd.begin(), block->INrd.end(), block->INae.begin(), block->INae.end(), inserter(ss, ss.end()));
        block->OUTae.clear();
        block->OUTrd.clear();
        for (set<SgStatement*>::iterator it = ss.begin(); it != ss.end();)
        {
            if (FindInExpr((*it)->expr(0), (*it)->expr(1)) != 0)
                ss.erase(it++);
            else
                ++it;
        }
        map<SgExpression*, SgStatement*> parent;
        map<SgExpression*, set<SgExpression*> > INss;
        for (set<SgStatement*>::iterator it = ss.begin(); it != ss.end(); ++it)
        {
            SgExpression* expr0 = lhs[unparsedLhs[(*it)->expr(0)]];
            SgExpression* expr1 = rhs[unparsedRhs[(*it)->expr(1)]];
            INss[expr0].insert(expr1);
            parent[expr0] = *it;
            parent[expr1] = *it;
        }

        for (map<SgExpression*, set<SgExpression*> >::iterator it1 = INss.begin(); it1 != INss.end(); ++it1)
        {
            for (set<SgExpression*>::iterator it2 = it1->second.begin(); it2 != it1->second.end(); ++it2)
            {
                SgExpression* rhs = (*it2)->copyPtr();
                block->INss[it1->first].insert(rhs);
                parent[rhs] = parent[*it2];
            }
        }

        for (map<SgExpression*, set<SgExpression*> >::iterator it1 = block->INss.begin(); it1 != block->INss.end(); ++it1)
        {
            if (it1->second.size() != 1 || FindInExpr(it1->first, block->head->expr(1)) == 0)
                continue;
            bool changed = true;
            SgExpression* expr = *it1->second.begin();
            SgStatement* stmt = parent[it1->first];
            while (changed == true)
            {
                changed = false;
                for (map<SgExpression*, set<SgExpression*> >::iterator it3 = block->INss.begin(); it3 != block->INss.end(); ++it3)
                    if (it3->second.size() == 1 && it1->first != it3->first)
                        changed |= replace(expr, stmt, it3->first, *it3->second.begin()) != 0;
            }
        }
    }

    if (enable_opt == true)
    {
        for (vector<BasicBlock>::iterator block = blocks.begin() + 2; block != blocks.end(); ++block)
        {
            for (map<SgExpression*, set<SgExpression*> >::iterator it = block->INss.begin(); it != block->INss.end(); ++it)
            {
                if (it->second.size() == 1)
                {
                    if (block->head->variant() == ASSIGN_STAT)
                    {
                        replaceInSubscripts(block->head->expr(0), block->head, it->first, *it->second.begin());
                        replaceInSubscripts(block->head->expr(1), block->head, it->first, *it->second.begin());
                    }
                    else if (block->head->variant() == PROC_CALL)
                        replaceInSubscripts(block->head->expr(0), block->head, it->first, *it->second.begin());
                }
            }
        }
    }

    vector<bool> visited(blocks.size(), false);
    visited[ENTRY] = true;
    visited[EXIT] = true;
    visited[2] = true;
    vector<int> _blocks;
    map<int, int> dfn;
    dfn[ENTRY] = 0;
    dfn[EXIT] = 1;
    _blocks.push_back(2);
    int k = 0;
    int count = 2;

    for (vector<int>::iterator p = _blocks.begin(); p != _blocks.end(); ++k, p = _blocks.begin() + k)
    {
        int index = *p;
        visited[index] = true;
        for (vector<int>::iterator it = blocks[index].out.begin(); it != blocks[index].out.end(); ++it)
        {
            if (visited[*it] == false)
            {
                visited[*it] = true;
                _blocks.push_back(*it);
            }
        }
        dfn[index] = count;
        count++;
    }

    vector<BasicBlock> tmp(blocks.size());
    for (vector<BasicBlock>::iterator block = blocks.begin(); block != blocks.end(); ++block)
    {
        block->index = dfn[block->index];
        for (vector<int>::iterator it = block->out.begin(); it != block->out.end(); ++it)
            *it = dfn[*it];

        for (vector<int>::iterator it = block->in.begin(); it != block->in.end(); ++it)
            *it = dfn[*it];
        tmp[block->index] = *block;
    }
    blocks.swap(tmp);
}


// graphviz script, for debug
void Loop::visualize(const char* scriptName) const
{
    FILE* f = fopen(scriptName, "w");
    if (f == NULL)
    {
        printf("Failed to open file \"%s\"\n", scriptName);
        return;
    }
    fprintf(f, "digraph\n{\n0[label=\"{Entry|<out>}\",shape=record]\n1[label=\"{<in>Exit|}\",shape=record]\n");

    for (size_t i = 2; i < blocks.size(); ++i)
    {
        fprintf(f, "%d[label=\"{<in>B%d|<out>", blocks[i].index, blocks[i].index);
        for (SgStatement* stmt = blocks[i].head; stmt != NULL && stmt != blocks[i].tail->lexNext(); stmt = stmt->lexNext())
        {
            switch (stmt->variant())
            {
                case SWITCH_NODE:
                    if (stmt->label())
                        fprintf(f, "%ld  ", LABEL_STMTNO(stmt->label()->thelabel));
                    fprintf(f, "select case (%s)\\n", ((SgSwitchStmt*)stmt)->expr(0)->unparse());
                    break;
                case IF_NODE:
                    if (stmt->label())
                        fprintf(f, "%ld  ", LABEL_STMTNO(stmt->label()->thelabel));
                    fprintf(f, "if (%s) then\\n", ((SgIfStmt*)stmt)->conditional()->unparse());
                    break;
                case ELSEIF_NODE:
                    fprintf(f, "elseif (%s) then\\n", ((SgIfStmt*)stmt)->conditional()->unparse());
                    break;
                case LOGIF_NODE:
                    if (stmt->label())
                        fprintf(f, "%ld  ", LABEL_STMTNO(stmt->label()->thelabel));
                    fprintf(f, "if (%s)\\n", ((SgLogIfStmt*)stmt)->conditional()->unparse());
                    break;
                case WHILE_NODE:
                {
                    SgForStmt* _stmt = (SgForStmt*)stmt;
                    if (_stmt->hasLabel() == TRUE)
                        fprintf(f, "%ld  ", LABEL_STMTNO(_stmt->label()->thelabel));
                    if (LlndMapping(BIF_LL3(_stmt->thebif)) != NULL)
                        fprintf(f, "%s: ", LlndMapping(BIF_LL3(_stmt->thebif))->unparse());
                    fprintf(f, "do ");
                    if (BIF_LABEL_USE(_stmt->thebif) != NULL)
                        fprintf(f, "%ld ", LABEL_STMTNO(BIF_LABEL_USE(_stmt->thebif)));
                    fprintf(f, "while ");
                    if (((SgWhileStmt*)stmt)->conditional() != NULL)
                        fprintf(f, "(%s)\\n", ((SgWhileStmt*)stmt)->conditional()->unparse());
                    break;
                }
                case FOR_NODE:
                {
                    SgForStmt* _stmt = (SgForStmt*)stmt;
                    if (_stmt->hasLabel() == TRUE)
                        fprintf(f, "%ld ", LABEL_STMTNO(_stmt->label()->thelabel));
                    if (LlndMapping(BIF_LL3(_stmt->thebif)) != NULL)
                        fprintf(f, "%s: ", LlndMapping(BIF_LL3(_stmt->thebif))->unparse());
                    fprintf(f, "do ");
                    if (BIF_LABEL_USE(_stmt->thebif) != NULL)
                        fprintf(f, "%ld ", LABEL_STMTNO(BIF_LABEL_USE(_stmt->thebif)));
                    if (_stmt->doName().identifier() != NULL)
                        fprintf(f, "%s = ", _stmt->doName().identifier());
                    if (_stmt->start() != NULL)
                        fprintf(f, "%s, ", _stmt->start()->unparse());
                    if (_stmt->end() != NULL)
                        fprintf(f, "%s", _stmt->end()->unparse());
                    if (_stmt->step() != NULL)
                        fprintf(f, ", %s\\n", _stmt->step()->unparse());
                    break;
                }
                default:
                    fprintf(f, "%s\\n", stmt->unparse());
                    break;
            }
        }
        fprintf(f, "}\",shape=record]\n");
    }

    for (size_t i = 0; i < blocks.size(); ++i)
    {
        for (size_t j = 0; j < blocks[i].out.size(); ++j)
            fprintf(f, "%d:out->%d:in\n", blocks[i].index, blocks[i].out[j]);
    }
    fprintf(f, "}");
    fclose(f);
}


extern SgStatement* kernelScope;

SgExpression* analyzeArrayIndxs(SgSymbol* ar, SgExpression* subscripts)
{
    static int count = 0;
    SgSymbol* varName = NULL;
    if (subscripts == NULL || options.isOn(AUTO_TFM) == false || dontGenConvertXY || oneCase)
        return NULL;

    map<SgSymbol*, Array*>& arrays = currentLoop->getArrays();
    Array* array = NULL;
    
    string toFind = OriginalSymbol(ar)->identifier();
    for (map<SgSymbol*, Array*>::iterator it = arrays.begin(); it != arrays.end(); it++)
    {
        if (OriginalSymbol(it->first)->identifier() == toFind)
        {
            array = it->second;
            break;
        }
    }
        
    if (array != NULL)
    {
        string expr;
        SgSymbol* symbol = array->findAccess(subscripts, expr);
        if (symbol == NULL)
        {
            char* counter = new char[32];
            sprintf(counter, "%d", count);
            ++count;
            string name(ar->identifier() + string("_") + counter);
            delete[] counter;
            if (options.isOn(C_CUDA))
                varName = new SgSymbol(VARIABLE_NAME, TestAndCorrectName(name.c_str()), *C_DvmType(), *kernelScope);
            else
            {
                if (undefined_Tcuda)
                    varName = new SgSymbol(VARIABLE_NAME, TestAndCorrectName(name.c_str()), *new SgType(T_INT, new SgExpression(LEN_OP, new SgValueExp(8), NULL, NULL), SgTypeInt()), *kernelScope);
                else
                    varName = new SgSymbol(VARIABLE_NAME, TestAndCorrectName(name.c_str()), *SgTypeInt(), *kernelScope);
            }
            array->addCoefficient(subscripts, expr, varName);
        }
        else
            varName = symbol;
    }
    return new SgVarRefExp(varName);
}
