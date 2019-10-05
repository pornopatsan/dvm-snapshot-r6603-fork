#include "dvm.h"
#include "aks_structs.h"
#include "acc_data.h"

// extern block vars
extern SgStatement *loop_body, *dvm_parallel_dir, *first_do_par;

// extern block functions
extern void correctPrivateList(int);

// local block vars
static std::vector<SgStatement*> scalar_stmts;
static bool only_scalar;
static bool operation;

// local  functions
SgExpression *preCalculate(SgExpression*);
SgExpression *correctDvmDirPattern(SgExpression*, SgExpression*);

// for countInDims
static int leftBound;
static int rightBound;
static bool existLB;
static bool existRB;

//for analyzeVarRef
static std::vector<SgExpression*> lBound;
static std::vector<SgExpression*> rBound;
static std::vector<int> globalStep;
static std::vector<SgSymbol*> symbolsOfForNode;
static std::vector<int> actualDocycle;
static std::vector<int> loopMultCount;

static FILE *file;
static FILE *fileStmts;

static std::stack<SgStatement*> controlEndsOfIfStmt;
static std::stack<SgStatement*> controlEndsOfForStmt;

static unsigned generator = 0;
static bool unknownLoop = false;

//global variables
std::vector<SgSymbol*> loopVars;
ArrayIntents regionArrayInfo;
LoopInfo currentLoopInfo;

void printEXP(SgExpression *ex, int what, int lvl)
{
    if(what == 3)
        printf("ROOT var %d lvl %d\n", ex->variant(), lvl);
    else if(what == 2)
        printf("LHS var %d lvl %d\n", ex->variant(), lvl);
    else
        printf("RHS var %d lvl %d\n", ex->variant(),lvl);
    if(ex->lhs())
        printEXP(ex->lhs(), 2, lvl+1);
    if(ex->rhs())
        printEXP(ex->rhs(), 1, lvl+1);
}

void fprintEXP(SgExpression *ex, int what, int lvl)
{
    if(what == 3)
        fprintf(file, "ROOT var %d lvl %d\n", ex->variant(), lvl);
    else if(what == 2)
        fprintf(file, "LHS var %d lvl %d\n", ex->variant(), lvl);
    else
        fprintf(file, "RHS var %d lvl %d\n", ex->variant(),lvl);
    if(ex->lhs())
        fprintEXP(ex->lhs(), 2, lvl+1);
    if(ex->rhs())
        fprintEXP(ex->rhs(), 1, lvl+1);
}

void createDoAssigns(AnalyzeStat &currentStat, std::vector<SgSymbol*> &newSymbs, SgExpression *arrayRef, int dim, int dimNew, BestPattern &pattern, std::vector<SgStatement*> &writeStmts, std::vector<SgStatement*> &readStmts)
{	
    SgForStmt *forStmtR = NULL, *forStmtW = NULL;
    int leftBound;
    int rightBound;
    bool exL = false;
    bool exR = false;
    int wasFirst = 0;	

    if(dimNew >= 1)
    {
        SgArrayType *tpArrNew = new SgArrayType(*arrayRef->symbol()->type());
        for(size_t i = 0; i < pattern.what.size(); ++i)
        {
            if(pattern.what[i] < 0)
            {
                if(pattern.bounds[i].ifDdot)
                {
                    SgExprListExp *ex = new SgExprListExp(DDOT);
                    ex->setLhs(*new SgValueExp(pattern.bounds[i].L));
                    ex->setRhs(*new SgValueExp(pattern.bounds[i].R));
                    tpArrNew->addDimension(ex);
                }
                else
                    tpArrNew->addDimension(new SgValueExp(abs(pattern.bounds[i].R - pattern.bounds[i].L) + 1));
            }
        }

        SgExpression *subsc = arrayRef->lhs();
        SgSymbol *symbArray = new SgSymbol(VARIABLE_NAME, TestAndCorrectName(arrayRef->symbol()->identifier()));		

        symbArray->setType(tpArrNew);
                
        SgArrayRefExp *newArray = new SgArrayRefExp(*symbArray);
        SgArrayRefExp *oldArray = new SgArrayRefExp(*arrayRef->symbol());
        SgArrayRefExp *newArray1 = new SgArrayRefExp(*symbArray);
        SgArrayRefExp *oldArray1 = new SgArrayRefExp(*arrayRef->symbol());

        SgStatement *stmtW = new SgAssignStmt(*oldArray, *newArray);
        SgStatement *stmtR = new SgAssignStmt(*newArray1, *oldArray1);

        for(size_t i = 0; i < pattern.what.size(); ++i)
        {
            exL = exR = false;
            char *idx = new char[32];
            char *number = new char[32];
            idx[0] = number[0] = '\0';
            strcat(idx, arrayRef->symbol()->identifier());
            strcat(idx, "_");
            strcat(idx, "m");
            number[sprintf(number, "%u", (unsigned)i)] = 0;
            strcat(idx, number);

            if(pattern.what[i] < 0)
            {
                SgSymbol *doVarName = new SgSymbol(VARIABLE_NAME, TestAndCorrectName(idx));
                newSymbs.push_back(doVarName);

                leftBound = pattern.bounds[i].L;
                rightBound = pattern.bounds[i].R;
                exL = exR = true;
                
                if(leftBound > rightBound)
                {
                    int tmp = rightBound;
                    rightBound = leftBound;
                    leftBound = tmp;
                }
    
                if(exL && exR)
                {
                    if(wasFirst == 0)
                    {
                        forStmtR = new SgForStmt(doVarName, new SgValueExp(leftBound), new SgValueExp(rightBound), new SgValueExp(1), stmtR);
                        forStmtW = new SgForStmt(doVarName, new SgValueExp(leftBound), new SgValueExp(rightBound), new SgValueExp(1), stmtW);
                        wasFirst = 1;
                    }
                    else
                    {
                        forStmtR = new SgForStmt(doVarName, new SgValueExp(leftBound), new SgValueExp(rightBound), new SgValueExp(1), forStmtR);
                        forStmtW = new SgForStmt(doVarName, new SgValueExp(leftBound), new SgValueExp(rightBound), new SgValueExp(1), forStmtW);
                    }
                    if(pattern.bounds[i].additionalExpr)
                    {
                        SgExpression *ex = new SgExpression(SUBT_OP);
                        ex->setLhs(pattern.bounds[i].additionalExpr);
                        ex->setRhs(pattern.bounds[i].additionalExpr);
                        SgExpression *res = preCalculate(ex);
                        res = Calculate(res);

                        oldArray->addSubscript(subsc->lhs()->copy() + *new SgValueExp(res->valueInteger()) + *new SgVarRefExp(*doVarName));
                        oldArray1->addSubscript(subsc->lhs()->copy() + *new SgValueExp(res->valueInteger()) + *new SgVarRefExp(*doVarName));
                    }
                    else
                    {					
                        oldArray->addSubscript(*new SgVarRefExp(*doVarName));
                        oldArray1->addSubscript(*new SgVarRefExp(*doVarName));
                    }
                    newArray->addSubscript(*new SgVarRefExp(*doVarName));
                    newArray1->addSubscript(*new SgVarRefExp(*doVarName));
                }
            }
            else
            {
                oldArray->addSubscript(subsc->lhs()->copy());
                oldArray1->addSubscript(subsc->lhs()->copy());
            }
            subsc = subsc->rhs();			
        }

        readStmts.push_back(forStmtR);
        writeStmts.push_back(forStmtW);
        newSymbs.push_back(symbArray);
        currentStat.replaceSymbol = symbArray;
        currentStat.ifHasDim = 1;
    }
    else if(dimNew == 0)
    {
        SgArrayRefExp *oldArray = new SgArrayRefExp(*arrayRef->symbol());
        SgExpression *subsc = arrayRef->lhs();
        for(int i = 0; i < dim; ++i)
        {
            oldArray->addSubscript(subsc->lhs()->copy());
            subsc = subsc->rhs();
        }

        SgArrayRefExp *oldArray1 = new SgArrayRefExp(*arrayRef->symbol());
        subsc = arrayRef->lhs();
        for(int i = 0; i < dim; ++i)
        {
            oldArray1->addSubscript(subsc->lhs()->copy());
            subsc = subsc->rhs();
        }

        SgSymbol *scalar = new SgSymbol(VARIABLE_NAME, TestAndCorrectName(arrayRef->symbol()->identifier()));
        scalar->setType(arrayRef->symbol()->type()->baseType());

        SgStatement *stmtW = new SgAssignStmt(*oldArray, *new SgVarRefExp(scalar));
        SgStatement *stmtR = new SgAssignStmt(*new SgVarRefExp(scalar), *oldArray1);	

        readStmts.push_back(stmtR);
        writeStmts.push_back(stmtW);
        newSymbs.push_back(scalar);
        currentStat.replaceSymbol = scalar;
        currentStat.ifHasDim = 0;
    }
}

int findPattern(SgExpression *patt, AnalyzeStat &Stat)
{
    bool noEq = true;
    int num = -1;
    for(size_t i = 0; i < Stat.patterns.size(); ++i)
    {
        if(ExpCompare(patt, Stat.patterns[i].symbs) == 1)
        {
            noEq = false;
            num = i;
            break;
        }
    }
    return num;
}

void replaceInExpr(SgExpression *ex, SgExpression *by, int nested)
{
    if(ex)
    {
        bool L = false;
        bool R = false;
        if(ex->lhs())
        {
            if(ex->lhs()->variant() == VAR_REF)
            {
                if(ex->lhs()->symbol() == symbolsOfForNode[nested])
                    ex->setLhs(by);
            }
            L = true;
        }
        if(ex->rhs())
        {
            if(ex->rhs()->variant() == VAR_REF)
            {
                if(ex->rhs()->symbol() == symbolsOfForNode[nested])
                    ex->setRhs(by);
            }
            R = true;
        }
        if(L)
            replaceInExpr(ex->lhs(), by, nested);
        if(R)
            replaceInExpr(ex->rhs(), by, nested);		
    }
}

void _setsetPatternSymbs(int plus, bool &change, SgExpression *lBound, SgExpression *parent, int where_)
{
    if(lBound->variant() != INT_VAL)
    {
        if(lBound->lhs())
            _setsetPatternSymbs(plus, change, lBound->lhs(), lBound, 0);
        if(lBound->rhs())
            _setsetPatternSymbs(plus, change, lBound->rhs(), lBound, 1);
    }
    else
    {
        plus += lBound->valueInteger();
        if(where_ == 0)
            parent->setLhs(*new SgValueExp(plus));
        if(where_ == 1)
            parent->setRhs(*new SgValueExp(plus));
        if(where_ == -1)
            lBound = new SgValueExp(plus);
        change = true;
    }
}

void setPatternSymbs(SgExpression *patt, SgExpression *in, int plus, int nested)
{	
    SgExpression *returnEx = patt;
    SgExpression *localLB = new SgExpression(EXPR_LIST);
    localLB->setLhs(&lBound[nested]->copy());
    bool change = false;
    _setsetPatternSymbs(plus, change, localLB, localLB, -1);
    localLB = localLB->lhs();

    SgExpression *replace = Calculate(localLB);
    while(in)
    {
        SgExpression *newEx = new SgExpression(EXPR_LIST);
        newEx->setLhs(&in->lhs()->copy());		
        replaceInExpr(newEx, replace, nested);
        newEx = newEx->lhs();

        patt->setLhs(newEx);		
        in = in->rhs();
        if(in)
        {
            patt->setRhs(new SgExprListExp());
            patt = patt->rhs();
        }
    }
    patt = returnEx;
}

// заменить на многоуровневый поиск
SgExpression* findReplaceEx(SgSymbol *s)
{
    SgExpression *returnEx = NULL;
    if(scalar_stmts.size() != 0)
    {
        for(int i = scalar_stmts.size() - 1; i >= 0; i--)
        {
            if(scalar_stmts[i]->expr(0)->symbol() == s)
            {
                returnEx = scalar_stmts[i]->expr(1);
                break;
            }
        }
    }
    return returnEx;
}

void ifNeedReplace(SgExpression *s, SgExpression *parent, int where_)
{
    if(s->variant() == VAR_REF)
    {
        bool ifN = false;
        bool ifInAllSymb = false;
        for (size_t i = 0; i < symbolsOfForNode.size(); ++i)
        {
            if (symbolsOfForNode[i] == s->symbol())
            {
                ifInAllSymb = true;
                break;
            }
        }
        // if symbol isnt FOR symbol
        if(ifInAllSymb == false)
        {
            for(size_t i = 0; i < loopVars.size(); ++i)
            {
                if(loopVars[i] != s->symbol())
                {
                    ifN = true;
                    break;
                }
            }
            
            if(ifN) // replace
            {
                SgExpression *find = findReplaceEx(s->symbol());
                if(find)
                {
                    if(where_ == 0)
                        parent->setLhs(find);
                    else if(where_ == 1)
                        parent->setRhs(find);
                }
            }
        }
    }
    else
    {
        if(s->lhs())
            ifNeedReplace(s->lhs(), s, 0);
        if(s->rhs())
            ifNeedReplace(s->rhs(), s, 1);
    }
}

void correctIdxOfArraRef(SgExpression *ex)
{
    SgExpression *tmp = ex->lhs();
    while(tmp)
    {
        ifNeedReplace(tmp->lhs(), tmp, 0);
        tmp = tmp->rhs();
    }
}

void insertLoopVariatns(std::vector<AnalyzeStat> &allStat, int num, bool _new, SgSymbol *s, SgExpression *ex, int nested)
{
    if (actualDocycle[nested])
    {
        for (int i = 0; i < loopMultCount[nested]; ++i)
        {
            SgExpression *pattTmp = new SgExprListExp();
            setPatternSymbs(pattTmp, &ex->lhs()->copy(), globalStep[nested] * i, nested);
            if (nested == (int)actualDocycle.size() - 1)
            {
                if (_new)
                {
                    Pattern p;
                    p.count_read_op = 0;
                    p.count_write_op = 0;
                    if (operation == READ)
                        p.count_read_op = 1;
                    else
                        p.count_write_op = 1;
                    p.symbs = pattTmp;
                    allStat[num].patterns.push_back(p);
                }
                else
                {
                    int num_p = findPattern(pattTmp, allStat[num]);
                    if (num_p == -1)
                    {
                        Pattern p;
                        p.count_read_op = 0;
                        p.count_write_op = 0;
                        if (operation == READ)
                            p.count_read_op = 1;
                        else
                            p.count_write_op = 1;
                        p.symbs = pattTmp;
                        allStat[num].patterns.push_back(p);
                    }
                    else
                    {
                        if (operation == READ)
                            allStat[num].patterns[num_p].count_read_op++;
                        else
                            allStat[num].patterns[num_p].count_write_op++;
                    }
                }
            }
            else
                insertLoopVariatns(allStat, num, _new, s, ex, nested + 1);
        }
    }
    else if (nested != (int)actualDocycle.size() - 1)
        insertLoopVariatns(allStat, num, _new, s, ex, nested + 1);
}

void analyzeVarRef(std::set<SgSymbol*> &private_vars, std::vector<AnalyzeStat> &allStat, SgSymbol *s, SgExpression *ex)
{
    bool inPrivateList = private_vars.find(s) != private_vars.end();

    if(isSgArrayType(s->type()) && !inPrivateList) // if array ref
    {
        bool inList = false;
        int num = -1;
                
        correctIdxOfArraRef(ex);
        only_scalar = false;
        for(size_t i = 0; i < allStat.size(); ++i)
        {
            if(allStat[i].name_of_array == s)
            {
                inList = true;
                num = i;
                break;
            }
        }

        if(!inList)
        {			
            AnalyzeStat tmp;			
            tmp.name_of_array = s;
            tmp.ex_name_of_array = ex;			
            allStat.push_back(tmp);
            int newNum = allStat.size() - 1;

            // if stmt in loops
            if(symbolsOfForNode.size() != 0)
                insertLoopVariatns(allStat, newNum, true, s, ex, 0);
            else
            {
                Pattern p;
                p.count_read_op = 0;
                p.count_write_op = 0;
                if(operation == READ)
                    p.count_read_op = 1;
                else
                    p.count_write_op = 1;
                p.symbs = ex->lhs();
                allStat[newNum].patterns.push_back(p);
            }
            
        }
        else
        {
            // if stmt in loops
            if(symbolsOfForNode.size() != 0)
                insertLoopVariatns(allStat, num, false, s, ex, 0);
            else
            {
                int num_p = findPattern(ex->lhs(), allStat[num]);
                if(num_p == -1)
                {
                    Pattern p;				
                    p.count_read_op = 0;
                    p.count_write_op = 0;
                    if(operation == READ)
                        p.count_read_op = 1;
                    else
                        p.count_write_op = 1;
                    p.symbs = ex->lhs();
                    allStat[num].patterns.push_back(p);
                }
                else
                {
                    if(operation == READ)
                        allStat[num].patterns[num_p].count_read_op ++;
                    else
                        allStat[num].patterns[num_p].count_write_op ++;
                }
            }
        }
    }	
}

void analyzeRightAssing(std::set<SgSymbol*> &private_vars, std::vector<AnalyzeStat> &allStat, SgExpression *ex)
{
    //printf("var %d\n", ex->variant());
    if(ex->variant() != ARRAY_REF)
    {
        if(ex->lhs())
            analyzeRightAssing(private_vars, allStat, ex->lhs());
        if(ex->rhs())
            analyzeRightAssing(private_vars, allStat, ex->rhs());
    }
    else	
        analyzeVarRef(private_vars, allStat, ex->symbol(), ex);
}

void findBest(std::vector<AnalyzeStat> &allStat, std::vector<BestPattern> &best, SgExpression *dvm_dir_pattern)
{
    for(size_t i = 0; i < allStat.size(); ++i)
    {
        int count = 0;
        size_t first = allStat[i].patterns.size() + 1;
        SgExpression *ex = NULL;
        std::vector<int> flags;
        std::vector<SgExpression*> exps;
        std::vector<SgExpression*> dvm_dir;
        BestPattern tmp;

        tmp.count_of_pattern = 0;
        for(size_t it = 0; it < allStat[i].patterns.size(); ++it)
        {
            if(allStat[i].patterns[it].count_write_op != 0)
            {
                first = it;
                break;
            }
        }

        if(first > allStat[i].patterns.size())
        {
            ex = allStat[i].patterns[0].symbs;
            while(ex)
            {
                flags.push_back(false);
                ex = ex->rhs();
            }
        }
        else
        {
            SgExpression *t = correctDvmDirPattern(dvm_dir_pattern, allStat[i].patterns[first].symbs);
            ex = allStat[i].patterns[first].symbs;
            tmp.count_of_pattern += allStat[i].patterns[first].count_write_op;
            while(ex)
            {
                count++;
                exps.push_back(ex->lhs());
                flags.push_back(true);
                ex = ex->rhs();

                dvm_dir.push_back(t->lhs());
                t = t->rhs();
            }
            tmp.bounds = std::vector<Bound>(count);
            std::vector<SgExpression*> extraExprsInIdx = std::vector<SgExpression*>(count);
            std::vector<int> minVal = std::vector<int>(count);
            std::vector<int> maxVal = std::vector<int>(count);

            for(size_t k = first + 1; k < allStat[i].patterns.size(); ++k)
            {
                if(allStat[i].patterns[k].count_write_op != 0)
                {
                    tmp.count_of_pattern += allStat[i].patterns[k].count_write_op;
                    ex = allStat[i].patterns[k].symbs;			
                    for(int m = 0; m < count; ++m)
                    {
                        if(flags[m])
                        {
                            if(ExpCompare(ex->lhs(), exps[m]) != 1)
                            {
                                if(dvm_dir[m] != NULL)
                                {
                                    if(dvm_dir[m]->variant() != KEYWORD_VAL)
                                    {
                                        SgExprListExp *countEx = new SgExprListExp(SUBT_OP);
                                        countEx->setRhs(*exps[m]);
                                        countEx->setLhs(*ex->lhs());
                                        SgExpression *res = preCalculate(countEx);
                                
                                        res = Calculate(res);
                                        if(res->variant() != INT_VAL)
                                            flags[m] = false;
                                        else
                                        {
                                            int resval = res->valueInteger();
                                            if(extraExprsInIdx[m] == NULL)
                                            {
                                                extraExprsInIdx[m] = exps[m];
                                                minVal[m] =  maxVal[m] = 0;
                                            }
                                            if(resval < minVal[m])
                                                minVal[m] = resval;
                                            else if(resval > maxVal[m])
                                                maxVal[m] = resval;											
                                        }
                                    }
                                    else
                                    {
                                        flags[m] = false;
                                        extraExprsInIdx[m] = NULL;
                                    }
                                }
                                else
                                {
                                    flags[m] = false;
                                    extraExprsInIdx[m] = NULL;
                                }
                            }
                        }
                        ex = ex->rhs();
                    }
                }
            }

            for(int i = 0; i < count; ++i)
            {
                if(extraExprsInIdx[i] != NULL)
                {
                    Bound tmpB;
                    tmpB.additionalExpr = extraExprsInIdx[i];
                    tmpB.exL = true;
                    tmpB.exR = true;
                    tmpB.ifDdot = true;
                    tmpB.L = minVal[i];
                    tmpB.R = maxVal[i];
                    tmp.bounds[i] = tmpB;
                    flags[i] = false;
                }
            }
        }	
        tmp.what = flags;
        if(first < allStat[i].patterns.size())
            tmp.bestPatt = allStat[i].patterns[first].symbs;
        else
        {
            //printf(" NO FOUND!!! \n");
            tmp.bestPatt = NULL;
        }
        best.push_back(tmp);		
    }
}

void findSymbolInExpression(SgExpression *inFind, int &flag, std::vector<SgSymbol*> &symbsInDvmDir, int &numFind, SgSymbol *sFind)
{
    if(flag == 1)
    {
        SgExpression *left = inFind->lhs();
        SgExpression *right = inFind->rhs();
    
        if(inFind->variant() != VAR_REF)
        {
            if(left)
                findSymbolInExpression(left, flag, symbsInDvmDir, numFind, sFind);
            if(right)
                findSymbolInExpression(right, flag, symbsInDvmDir, numFind, sFind);
        }
        else
        {
            bool find = false;
            size_t i = 0;
            SgSymbol *s = inFind->symbol();
            for( ; i < symbsInDvmDir.size(); i++)
            {
                if(symbsInDvmDir[i] == s)
                {
                    find = true;
                    break;
                }
            }

            if(i < symbsInDvmDir.size())
            {
                if(numFind == -1)
                {
                    numFind = i;
                    sFind = inFind->symbol();
                }
                else if(numFind != (int)i)
                    flag = 0;
            }
        }
    }
}

SgExpression *correctDvmDirPattern(SgExpression *dvm_dir_pattern, SgExpression *firstPatt)
{
    SgExpression *tmp1 = dvm_dir_pattern;
    SgExpression *returnExp = dvm_dir_pattern;
    std::vector<SgSymbol*> symbsInDvmDir;
    int countDVM = 0;
    int count = 0;

    while(tmp1)
    {
        countDVM++;
        if(tmp1->lhs()->variant() == VAR_REF)
            symbsInDvmDir.push_back(tmp1->lhs()->symbol());
        tmp1 = tmp1->rhs();
    }
    tmp1 = firstPatt;
    while(tmp1)
    {
        count++;
        tmp1 = tmp1->rhs();
    }

    // if correction needed
    if(count != countDVM)
    {
        tmp1 = firstPatt;
        
        returnExp = new SgExprListExp();	
        SgExpression *t = returnExp;
        
        for(int i = 0; i < count; ++i)
        {
            int flag = 1;
            int numFind = -1;	
            SgSymbol *sFind = NULL;	
            
            findSymbolInExpression(tmp1->lhs(), flag, symbsInDvmDir, numFind, sFind);
            if(flag != 1)
            {
                returnExp = NULL;
                break;
            }
            else
            {

                SgExprListExp *newL = new SgExprListExp();
                if(numFind != -1)
                    t->setLhs(*new SgVarRefExp(symbsInDvmDir[numFind]));

                t->setRhs(newL);
                t = t->rhs();
            }
            tmp1 = tmp1->rhs();
        }
    }

    return returnExp;
}

void correctBestPattern(std::vector<AnalyzeStat> &allStat, std::vector<BestPattern> &best, SgExpression *dvm_dir_pattern)
{	
    for(size_t i = 0; i < allStat.size(); ++i)
    {
        SgExpression *t = dvm_dir_pattern;
        SgExpression *t1 = NULL;
        for(size_t p = 0; p < allStat[i].patterns.size(); ++p)
        {
            if(allStat[i].patterns[p].count_write_op != 0)
            {
                t1 = allStat[i].patterns[p].symbs;
                break;
            }
        }
        if(t1 != NULL)
        {
            t = correctDvmDirPattern(dvm_dir_pattern, t1);
            if(DVM_DEBUG_LVL > 1)
                if(t)
                    fprintf(file, " Found pattern is %s\n", copyOfUnparse(t->unparse()));

            if(t)
            {
                for(size_t k = 0; k < best[i].what.size(); ++k)
                {
                    if(best[i].what[k] != 0)
                    {
                        if(ExpCompare(t->lhs(), t1->lhs()) != 1)
                            best[i].what[k] = 0;
                    }
                    
                    t = t->rhs();
                    t1 = t1 ->rhs();
                }
            }
            else
            {
                for(size_t k = 0; k < best[i].what.size(); ++k)	
                    best[i].what[k] = 0;				
            }
        }
    }
}

int countSizeInDim(SgExpression *ex, bool &ifDdot)
{	
    int res = 0;
    existLB = existRB = false;
    SgExpression *result;
    if(ex->variant() == DDOT)
    {
        ifDdot = true;
        result = Calculate(ex->lhs());
        if(result->variant() == INT_VAL)
        {
            existLB = true;
            leftBound = result->valueInteger();
        }
        result = Calculate(ex->rhs());
        if(result->variant() == INT_VAL)
        {
            existRB = true;
            rightBound = result->valueInteger();
        }
        if(existLB && existRB)
            res = abs(leftBound - rightBound) + 1;
    }
    else 
    {
        result = Calculate(ex);
        existLB = true;
        leftBound = 1;
        if(result->variant() == INT_VAL)
        {
            existRB = true;
            rightBound = result->valueInteger();
        }
        if(existLB && existRB)
            res = abs(leftBound - rightBound) + 1;
    }
    return -1 * res;
}

bool compareWithPatten(SgExpression *inPatt, SgExpression *compared, std::vector<int> &flags)
{	
    bool retval = true;
    SgExpression *t1 = inPatt;
    SgExpression *t2 = compared;
    char **str = new char*[2];
    
    if(DVM_DEBUG_LVL > 1)
        fprintf(file, "%s  VS  %s  is ", copyOfUnparse(t1->unparse()), copyOfUnparse(t2->unparse()));

    for(size_t i = 0; i < flags.size(); ++i)
    {
        if(flags[i] == 1)
        {
            if(ExpCompare(t1->lhs(), t2->lhs()) != 1)
            {
                str[0] = copyOfUnparse(t1->lhs()->unparse());
                str[1] = copyOfUnparse(t2->lhs()->unparse());				
                retval = false;
                break;
            }
        }
        
        t1 = t1->rhs();
        t2 = t2->rhs();
    }
    if(DVM_DEBUG_LVL > 1)
    {
        fprintf(file, "retval = %d  flags: ", retval);		
        for(size_t i = 0; i < flags.size(); ++i)
            fprintf(file, "%d ", flags[i]);

        if(!retval)
            fprintf(file, "  %s VS %s ", str[0], str[1]);

        fprintf(file, "\n");
    }
    
    return retval;
}

void replaceInStmt(std::vector<AnalyzeStat> &allStat, std::vector<BestPattern> &best, SgExpression *expr, SgExpression *ex_parrent, SgStatement *ex_parrent_st, int RL)
{
    if(expr->variant() == ARRAY_REF)
    {
        size_t i = 0;
        SgSymbol *tmp = expr->symbol();
        for( ; i < allStat.size(); i++)
        {
            if(allStat[i].name_of_array == tmp)
                break;
        }
        if(i < allStat.size()) //if found
        {		
            if(best[i].count_of_pattern != 0)
            {
                if(compareWithPatten(best[i].bestPatt, expr->lhs(), best[i].what))
                {
                    SgArrayRefExp *newExp = NULL;					
                    if(allStat[i].ifHasDim)
                    {
                        newExp = new SgArrayRefExp(*allStat[i].replaceSymbol);
                        SgExpression *idxEx = expr->lhs();
                        for(size_t k = 0; k < best[i].what.size(); ++k)
                        {
                            if(best[i].what[k] != 1)
                            {
                                if(best[i].bounds[k].additionalExpr)							
                                    newExp->addSubscript(idxEx->lhs()->copy() - *best[i].bounds[k].additionalExpr);
                                else
                                    newExp->addSubscript(idxEx->lhs()->copy());
                            }
                            idxEx = idxEx->rhs();
                        }
                    }
                    if(ex_parrent)
                    {
                        if(RL == RIGHT)
                        {
                            if(newExp)
                                ex_parrent->setRhs(*newExp);
                            else
                                ex_parrent->setRhs(*new SgVarRefExp(*allStat[i].replaceSymbol));
                        }
                        else if(RL == LEFT)
                        {
                            if(newExp)
                                ex_parrent->setLhs(*newExp);
                            else
                                ex_parrent->setLhs(*new SgVarRefExp(*allStat[i].replaceSymbol));
                        }
                    }
                    else if(ex_parrent_st)
                    {
                        if(RL == RIGHT)
                        {
                            if(newExp)
                                ex_parrent_st->setExpression(1, *newExp);
                            else
                                ex_parrent_st->setExpression(1, *new SgVarRefExp(*allStat[i].replaceSymbol));
                        }
                        else if(RL == LEFT)
                        {
                            if(newExp)
                                ex_parrent_st->setExpression(0, *newExp);
                            else
                                ex_parrent_st->setExpression(0, *new SgVarRefExp(*allStat[i].replaceSymbol));
                        }
                    }
                }
            }
        }
    }
    else
    {
        if(expr->lhs())
            replaceInStmt(allStat, best, expr->lhs(), expr, NULL, LEFT);
        if(expr->rhs())
            replaceInStmt(allStat, best, expr->rhs(), expr, NULL, RIGHT);
    }
}

void generateOptimalExpressions(std::vector<AnalyzeStat> &allStat, std::vector<BestPattern> &best, std::vector<SgSymbol*> &newVars)
{
    std::vector<SgStatement*> writeStmts;
    std::vector<SgStatement*> readStmts;

    for(size_t i = 0; i < allStat.size(); ++i)
    {
        SgArrayType *type = isSgArrayType(allStat[i].name_of_array->type());
        if(type != NULL)
        {
            int dims = type->dimension();
            int sum = 1;
            bool ifSumChanged = false;
            //fprintf(file, "dims size ");
            for(int k = 0; k < dims; ++k)
            {
                if(!best[i].what[k] && best[i].count_of_pattern != 0)
                {
                    if(best[i].bounds[k].additionalExpr == NULL)
                    {						
                        SgExpression *ex = type->sizeInDim(k);
                        best[i].what[k] = countSizeInDim(ex, best[i].bounds[k].ifDdot);
                    
                        best[i].bounds[k].L = best[i].bounds[k].R = 0;
                        best[i].bounds[k].exL = existLB;
                        best[i].bounds[k].exR = existRB;					
                        if(existLB)
                            best[i].bounds[k].L = leftBound;
                        if(existRB)
                            best[i].bounds[k].R = rightBound;

                        sum *= (-1 * best[i].what[k]);
                    }
                    else
                    {
                        best[i].what[k] = -1 * (abs(best[i].bounds[k].L - best[i].bounds[k].R) + 1);
                        sum *= (-1 * best[i].what[k]);
                    }
                    ifSumChanged = true;
                }
                /*else
                {
                    Bound tmpB;
                    best[i].bounds.push_back(tmpB);
                }*/
                //fprintf(file, "%d ", best[i].what[k]);
            }
            //fprintf(file, "\n");
            if(!ifSumChanged) // scalar ?
                sum = 1;
            if(sum >= best[i].count_of_pattern)
            {
                if(DVM_DEBUG_LVL > 1)
                    fprintf(file, " [INFO] in array \" %s \" needed to read = %d, write operations = %d\n", allStat[i].name_of_array->identifier(), sum, best[i].count_of_pattern);

                for(int k = 0; k < dims; ++k)
                {
                    best[i].what[k] = 0;
                }
                best[i].count_of_pattern = 0;
            }
            else
            {
                if(DVM_DEBUG_LVL > 1)
                    fprintf(file, " [INFO] in array \" %s \" needed to read = %d, write operations = %d\n", allStat[i].name_of_array->identifier(), sum, best[i].count_of_pattern);
                sum = 0;
                for(int k = 0; k < dims; ++k)
                {
                    if(best[i].what[k] < 0)
                        sum ++;
                    if(best[i].what[k] == 0)
                    {
                        sum = -1;
                        break;
                    }
                }
                
                if(sum != -1)
                    createDoAssigns(allStat[i], newVars, allStat[i].ex_name_of_array, best[i].what.size(), sum, best[i], writeStmts, readStmts);
            }
        }
    }

    // insert and correct loop_body	
    SgStatement *tmp, *contrEnd = NULL;
    tmp = loop_body;
    if(readStmts.size() != 0)
        while(tmp)
        {
            if(tmp->variant() == ASSIGN_STAT)
            {
                if(DVM_DEBUG_LVL > 1)
                    fprintf(file, "COMPARE PATTERNS start:\n");

                replaceInStmt(allStat, best, tmp->expr(0), NULL, tmp, LEFT);
                replaceInStmt(allStat, best, tmp->expr(1), NULL, tmp, RIGHT);

                if(DVM_DEBUG_LVL > 1)
                    fprintf(file, "COMPARE PATTERNS stop:\n\n");			
            }
        
            tmp = tmp->lexNext();
        }
    
    for(size_t i = 0; i < readStmts.size(); ++i)
    {
        tmp = readStmts[i];
        tmp->lastNodeOfStmt()->setLexNext(*loop_body);
        loop_body = tmp;
    }

    tmp = loop_body;
    int count = 0;
    while(tmp)
    {
        tmp = tmp->lexNext();
        count++;
    }

    tmp = loop_body;
    for(int i = 0; i < count - 2; ++i)
    {
        tmp = tmp->lexNext();
    }
    if(tmp->lexNext()->variant() == CONTROL_END)
        contrEnd = tmp->lexNext();

    for(size_t i = 0; i < writeStmts.size(); ++i)
    {
        tmp->setLexNext(*writeStmts[i]);
        tmp = tmp->lexNext()->lastNodeOfStmt();
    }
    if(contrEnd)
        tmp->setLexNext(*contrEnd);

    // printf its
    if(DVM_DEBUG_LVL > 1)
    {
        if(readStmts.size() != 0)
            fprintf(file, "  Generated READ stms:\n");
        for(size_t i = 0; i < readStmts.size(); ++i)
            fprintf(file, "%s", readStmts[i]->unparse());
        if(writeStmts.size() != 0)
            fprintf(file, "  Generated WRITE stms:\n");
        for(size_t i = 0; i < writeStmts.size(); ++i)
            fprintf(file, "%s", writeStmts[i]->unparse());
    }
}

// sign = 0 - plus, sing = 1 - minus
void getInformation(std::vector<int> &signs, std::vector<SgSymbol *> &symbs, std::vector<int> &values, int sign, SgExpression *ex)
{
    if(ex->variant() == SUBT_OP)
    {
        getInformation(signs, symbs, values, 0, ex->lhs());
        getInformation(signs, symbs, values, 1, ex->rhs());
    }
    else if(ex->variant() == ADD_OP)
    {
        getInformation(signs, symbs, values, 0 + sign, ex->lhs());
        getInformation(signs, symbs, values, 0 + sign, ex->rhs());
    }
    else if(ex->variant() == VAR_REF)
    {
        symbs.push_back(ex->symbol());
        signs.push_back(sign);
    }
    else if(ex->variant() == INT_VAL)
    {
        if(sign == 1)
            values.push_back(-1 * ex->valueInteger());
        else
            values.push_back(ex->valueInteger());
    }
}

SgExpression *preCalculate(SgExpression *exprL) // доделать для всех остальных знаков
{
    std::vector<SgSymbol *> symbs;
    std::vector<int> values;
    std::vector<int> signs;
    int val = 0;
    bool ifALL = true;
    SgExpression *retval = exprL;

    getInformation(signs, symbs, values, 0, exprL);
    for(size_t i = 0; i < symbs.size(); ++i)
    {
        SgSymbol *s = symbs[i];
        for(size_t k = i + 1; k < symbs.size(); ++k)
        {
            if(s == symbs[k])
            {
                if(signs[i] * signs[k] == 0)
                {
                    symbs[i] = NULL;
                    symbs[k] = NULL;
                }
                break;
            }
        }
    }

    for(size_t i = 0; i < symbs.size(); ++i)
    {
        if(symbs[i])
        {
            ifALL = false;
            break;
        }
    }

    for(size_t i = 0; i < values.size(); ++i)
    {
        val += values[i];
    }

    if(ifALL)
    {
        retval = new SgValueExp(val);
    }
    return retval;
}

bool existEqOp(SgExpression *ex)
{
    bool retval = false;
    if(ex)
    {
        if(ex->variant() == EQ_OP)
            retval = true;
        else
        {
            if(ex->lhs())
                retval = retval || existEqOp(ex->lhs());
            if(ex->rhs() && !retval)
                retval = retval || existEqOp(ex->rhs());
        }
    }
    return retval;
}

// for <-gpuO1:lvl2>
void findGroups(std::vector<AnalyzeStat> &allStat, std::vector<ArrayGroup> &allArrayGroups)
{
    for (size_t i = 0; i < allStat.size(); ++i)
    {
        AnalyzeStat tmp = allStat[i];
        SgExpression *ex = tmp.patterns[0].symbs;
        int countOfVariants = 0;
        int position = 0;

        while (ex)
        {
            countOfVariants++;
            ex = ex->rhs();
        }

        std::vector<Group> allGroup;
        std::vector<PositionGroup> allPosGr;
        ArrayGroup newArrayGroup;

        newArrayGroup.arrayName = allStat[i].name_of_array;
        for (int k = 0; k < countOfVariants; ++k)
        {
            position = k;
            PositionGroup newGr;

            newGr.position = position;
            for (size_t gl = 0; gl < tmp.patterns.size(); ++gl)
            {
                ex = tmp.patterns[gl].symbs;
                std::vector<char *> charEx;
                SgExpression *exInPos = NULL;
                SgExprListExp *positions = new SgExprListExp();
                SgExpression *currentPos = positions;

                int num = 0;
                bool first = true;
                for (int m = 0; m < countOfVariants; ++m)
                {
                    if (m != k)
                    {
                        charEx.push_back(copyOfUnparse(ex->lhs()->unparse()));
                        num += strlen(charEx[charEx.size() - 1]);
                        if (first != true)
                        {							
                            currentPos->setRhs(new SgExprListExp());
                            currentPos = currentPos->rhs();							
                        }
                        else
                            first = false;

                        currentPos->setLhs(ex->lhs());
                        currentPos->setRhs(NULL);
                    }
                    else
                    {
                        exInPos = ex->lhs();
                        if (gl == 0)
                            newGr.idxInPos = ex->lhs();
                    }
                    ex = ex->rhs();
                }
                char *buf = new char[num + 16];
                buf[0] = '\0';
                strcat(buf, "(");
                for (size_t m = 0; m < charEx.size(); ++m)
                {
                    strcat(buf, charEx[m]);
                    if (m != charEx.size() - 1)
                        strcat(buf, ",");
                }
                strcat(buf, ")");

                bool exist = false;
                num = 0;
                for (size_t m = 0; m < newGr.allPosGr.size(); ++m)
                {
                    if (strcmp(newGr.allPosGr[m].strOfmain, buf) == 0)
                    {
                        num = m;
                        exist = true;
                        break;
                    }
                }

                if (exist)
                    newGr.allPosGr[num].inGroup.push_back(exInPos);
                else
                {
                    Group gr;
                    gr.inGroup.push_back(exInPos);
                    gr.strOfmain = buf;
                    gr.mainPattern = positions;
                    newGr.allPosGr.push_back(gr);
                }
            }
            allPosGr.push_back(newGr);
        }
        newArrayGroup.allGroups = allPosGr;
        allArrayGroups.push_back(newArrayGroup);
    }	
}

void createSwaps(newInfo &info)
{
    for (int i = 0; i < info.dimSize[0] - 1; ++i)
    {
        SgArrayRefExp *arrayEx = new SgArrayRefExp(*info.newArray);
        SgArrayRefExp *arrayEx1 = new SgArrayRefExp(*info.newArray);

        arrayEx->addSubscript(*new SgValueExp(i));
        arrayEx1->addSubscript(*new SgValueExp(i + 1));
        info.swapsDown.push_back(new SgAssignStmt(*arrayEx, *arrayEx1));
    }

    for (int i = 1; i < info.dimSize[0]; ++i)
    {
        SgArrayRefExp *arrayEx = new SgArrayRefExp(*info.newArray);
        SgArrayRefExp *arrayEx1 = new SgArrayRefExp(*info.newArray);

        arrayEx->addSubscript(*new SgValueExp(i - 1));
        arrayEx1->addSubscript(*new SgValueExp(i));
        info.swapsUp.push_back(new SgAssignStmt(*arrayEx1, *arrayEx));
    }
}

void createLoadsAndStores(Group &gr, newInfo &info, ArrayGroup &oldArray, int numGr, PositionGroup &posGr)
{
    SgExprListExp *ddot = new SgExprListExp(DDOT);
    SgArrayType *tpArrNew = new SgArrayType(*oldArray.arrayName->type());

    ddot->setLhs(*new SgValueExp(0));
    ddot->setRhs(*new SgValueExp(info.dimSize[0] - 1));
    
    tpArrNew->addDimension(ddot);
    info.newArray->setType(tpArrNew);

    for (int i = 0; i < info.dimSize[0]; ++i)
    {
        SgArrayRefExp *arrayEx = new SgArrayRefExp(*info.newArray);
        SgArrayRefExp *oldArrayEx = new SgArrayRefExp(*oldArray.arrayName);
        SgExpression *tmpEx = gr.mainPattern;
        int size = 0;
        
        while (tmpEx)
        {
            size++;
            tmpEx = tmpEx->rhs();
        }
        size++;

        tmpEx = gr.mainPattern;
        for (size_t k = 0; k < (size_t)size; ++k)
        {
            if ((int)k == numGr)
                oldArrayEx->addSubscript(*gr.inGroup[i]);
            else
            {
                oldArrayEx->addSubscript(*tmpEx->lhs());
                tmpEx = tmpEx->rhs();
            }
        }

        arrayEx->addSubscript(*new SgValueExp((int)i));
        // fill table
        posGr.tableReplace[copyOfUnparse(oldArrayEx->lhs()->unparse())] = arrayEx->copyPtr();

        if (i != info.dimSize[0] - 1)
            info.loadsBeforePlus.push_back(new SgAssignStmt(*arrayEx, *oldArrayEx));

        if (i != 0)
            info.loadsBeforeMinus.push_back(new SgAssignStmt(*arrayEx, *oldArrayEx));

        if (i == info.dimSize[0] - 1)
            info.loadsInForPlus.push_back(new SgAssignStmt(*arrayEx, *oldArrayEx));

        if (i == 0)
            info.loadsInForMinus.push_back(new SgAssignStmt(*arrayEx, *oldArrayEx));
        /*
        if (i == 0)
            info.stores.push_back(new SgAssignStmt(*oldArrayEx, *arrayEx));*/
    }
}

void sortInGroup(Group &gr)
{
    for (size_t i = 0; i < gr.sortLen.size() - 1; ++i)
    {
        for (size_t k = i; k < gr.sortLen.size() - 1; ++k)
        {
            if (gr.sortLen[k] > gr.sortLen[k + 1])
            {
                int tmp = gr.sortLen[k];
                SgExpression *tmpEx = gr.inGroup[k];

                gr.sortLen[k] = gr.sortLen[k + 1];
                gr.inGroup[k] = gr.inGroup[k + 1];
                gr.sortLen[k + 1] = tmp;
                gr.inGroup[k + 1] = tmpEx;
            }
        }
    }
}

SgExpression *substitutionStep(int stepSub, SgExpression *in, char *symb)
{
    SgExpression *ret = NULL;
    SgExpression *left = NULL, *right = NULL;
    if (in->variant() == VAR_REF)
    {
        if (strcmp(symb, in->symbol()->identifier()) == 0)
        {
            ret = new SgValueExp(stepSub);
        }
    }
    else
    {
        if (in->lhs())
            left = substitutionStep(stepSub, in->lhs(), symb);
        if (in->rhs())
            right = substitutionStep(stepSub, in->rhs(), symb);

        if (left != NULL && right != NULL)
        {
            ret = new SgExprListExp(in->variant());
            ret->setLhs(left);
            ret->setRhs(right);
        }
        else if (left != NULL)
        {
            ret = new SgExprListExp(in->variant());
            ret->setLhs(left);
        }
        else if (right != NULL)
        {
            ret = new SgExprListExp(in->variant());
            ret->setRhs(right);
        }
        else
        {
            ret = in;
        }
    }
    return ret;
}

SgExpression* replaceInExpr(SgExpression *current, SgExpression *parent, int nested, char *arrayS, PositionGroup &posGr)
{
    SgExpression *ret = NULL;
    if (current->variant() == ARRAY_REF)
    {
        if (strcmp(current->symbol()->identifier(), arrayS) == 0)
        {
            SgExpression *replace = NULL;
            char *need = copyOfUnparse(current->lhs()->unparse());

            replace = posGr.tableReplace[need];			
            if (replace != NULL)
            {
                SgSymbol *s = posGr.tableNewVars[replace->symbol()->identifier()];
                if (s == NULL)
                    posGr.tableNewVars[replace->symbol()->identifier()] = replace->symbol();

                if (nested == 0) // assign
                    ret = replace->copyPtr();
                else if (nested == -1) // left
                    parent->setLhs(replace);
                else if (nested == 1) // rights
                    parent->setRhs(replace);

                if (DVM_DEBUG_LVL > 1)
                {
                    char *old = NULL, *new_ = NULL;
                    old = copyOfUnparse(current->unparse());
                    new_ = copyOfUnparse(replace->unparse());
                    fprintf(file, "  %s -> %s\n", old, new_);
                }
            }
        }
    }
    else
    {
        if (current->lhs())
            replaceInExpr(current->lhs(), current, -1, arrayS, posGr);
        if (current->rhs())
            replaceInExpr(current->rhs(), current, 1, arrayS, posGr);
    }
    return ret;
}

void correctLoopBody(std::vector<ArrayGroup> &allArrayGroups)
{
    if (DVM_DEBUG_LVL > 1)
        fprintf(file, "********** [REPLACE INFO] *********\n");

    for (size_t i = 0; i < allArrayGroups.size(); ++i)
    {
        int bestPosition = -1;
        int bestSum = -1;
        // find best replace
        for (size_t k = 0; k < allArrayGroups[i].allGroups.size(); ++k)
        {
            int sum = 0;
            for (size_t m = 0; m < allArrayGroups[i].allGroups[k].allPosGr.size(); ++m)
            {
                if (allArrayGroups[i].allGroups[k].allPosGr[m].inGroup.size() > 1)
                    sum++;
            }
            if (sum >= bestSum && allArrayGroups[i].allGroups[k].position != 0)
            {
                bestSum = sum;
                bestPosition = allArrayGroups[i].allGroups[k].position;
            }
        }

        if (bestPosition != -1)
        {
            SgStatement *st = loop_body;			
            while (st)
            {
                if (st->variant() == ASSIGN_STAT)
                {
                    SgExpression *left, *right;
                    left = right = NULL;
                    left = replaceInExpr(st->expr(0), st->expr(0), 0, allArrayGroups[i].arrayName->identifier(), allArrayGroups[i].allGroups[bestPosition]);
                    right = replaceInExpr(st->expr(1), st->expr(1), 0, allArrayGroups[i].arrayName->identifier(), allArrayGroups[i].allGroups[bestPosition]);
                    if (left != NULL)
                        st->setExpression(0, *left);
                    if (right != NULL)
                        st->setExpression(1, *right);
                }
                st = st->lexNext();
            }
            
            for (std::map < std::string, SgSymbol*> ::iterator it = allArrayGroups[i].allGroups[bestPosition].tableNewVars.begin(); it != allArrayGroups[i].allGroups[bestPosition].tableNewVars.end(); it++)
            {				
                newVars.push_back(&*it->second);
            }			
        }
    }


    if (DVM_DEBUG_LVL > 1)
        fprintf(file, "********** [REPLACE INFO] *********\n");
}

void checkGroup(Group &gr, int stepCycle, SgSymbol *symb)
{
    int *old = new int[gr.sortLen.size()];
    for (size_t i = 0; i < gr.sortLen.size(); ++i)
        old[i] = gr.sortLen[i];

    for (size_t i = 0; i < gr.sortLen.size(); ++i)
    {
        for (size_t k = 0; k < gr.sortLen.size() - 1 - i; ++k)
        {
            if (old[k] > old[k + 1])
            {
                int tmp = old[k];
                old[k] = old[k + 1];
                old[k + 1] = tmp;
            }
        }
    }

    /*for (size_t i = 0; i < gr.sortLen.size(); ++i)
    {
        printf("%d ", old[i]);
    }
    printf("\n");*/

    size_t size_ = gr.sortLen.size();
    for (size_t i = 0; i < size_ - 1; ++i)
    {
        if (abs(old[i] - old[i + 1]) > abs(stepCycle))
        {
            int insertVal = old[i] + stepCycle;

            gr.sortLen.push_back(insertVal);
            if (insertVal == 0)
            {
                gr.len.push_back(0);
                gr.inGroup.push_back(new SgVarRefExp(*symb));
            }
            else
            {
                gr.len.push_back(abs(insertVal));
                SgExprListExp *add = NULL;
                if (insertVal < 0)
                {
                    add = new SgExprListExp(SUBT_OP);
                    add->setLhs(*new SgVarRefExp(*symb));
                    add->setRhs(*new SgValueExp(-insertVal));
                }
                else
                {
                    add = new SgExprListExp(ADD_OP);
                    add->setLhs(*new SgVarRefExp(*symb));
                    add->setRhs(*new SgValueExp(insertVal));
                }
                gr.inGroup.push_back(add);
            }
        }
    }
}

void correctGroups(std::vector<ArrayGroup> &allArrayGroups)
{
    for (size_t i = 0; i < allArrayGroups.size(); ++i)
    {
        for (size_t k = 0; k < allArrayGroups[i].allGroups.size(); ++k)
        {
            for (size_t m = 0; m < allArrayGroups[i].allGroups[k].allPosGr.size(); ++m)
            {				
                bool nextStep = false;
                if (strcmp(allArrayGroups[i].allGroups[k].allPosGr[m].strOfmain, "()") != 0 && allArrayGroups[i].allGroups[k].allPosGr[m].inGroup.size() > 1)
                {
                    nextStep = true;
                    allArrayGroups[i].allGroups[k].allPosGr[m].len.push_back(0);

                    for (size_t p = 1; p < allArrayGroups[i].allGroups[k].allPosGr[m].inGroup.size(); ++p)
                    {
                        SgExprListExp *expr = new SgExprListExp(SUBT_OP);
                        SgExpression *result;

                        expr->setLhs(allArrayGroups[i].allGroups[k].allPosGr[m].inGroup[p - 1]);
                        expr->setRhs(allArrayGroups[i].allGroups[k].allPosGr[m].inGroup[p]);
                        result = preCalculate(expr);
                        if (result->variant() == INT_VAL)
                            allArrayGroups[i].allGroups[k].allPosGr[m].len.push_back(abs(result->valueInteger()));
                        else
                        {
                            allArrayGroups[i].allGroups[k].allPosGr[m].len.clear();
                            nextStep = false;
                            break;
                        }
                    }

                    for (size_t p = 0; p < allArrayGroups[i].allGroups[k].allPosGr[m].inGroup.size() && nextStep; ++p)
                    {
                        SgExprListExp *expr = new SgExprListExp(SUBT_OP);
                        SgExpression *result;	

                        expr->setLhs(allArrayGroups[i].allGroups[k].allPosGr[m].inGroup[p]);
                        expr->setRhs(allArrayGroups[i].allGroups[k].idxInPos);
                        result = preCalculate(expr);
                        if (result->variant() == INT_VAL)
                            allArrayGroups[i].allGroups[k].allPosGr[m].sortLen.push_back(result->valueInteger());
                        else
                        {
                            allArrayGroups[i].allGroups[k].allPosGr[m].sortLen.clear();
                            nextStep = false;
                            break;
                        }
                    }

                    if (nextStep)
                    {
                        int stepCycle = 1; // временное значение, изменить на постоянное.
                        int size;
                        int shift = 0;						
                        char *symb = NULL;
                        bool allOk = true;

                        if (allArrayGroups[i].allGroups[k].idxInPos->symbol())
                            symb = allArrayGroups[i].allGroups[k].idxInPos->symbol()->identifier();
                        else
                            allOk = false;
                        if (allOk)
                        {
                            checkGroup(allArrayGroups[i].allGroups[k].allPosGr[m], stepCycle, allArrayGroups[i].allGroups[k].idxInPos->symbol());

                            size = allArrayGroups[i].allGroups[k].allPosGr[m].len.size();
                            SgExpression **template1 = new SgExpression*[size];
                            SgExpression **template2 = new SgExpression*[size];

                            // fill templates
                            for (int i1 = 0; i1 < size; ++i1)
                            {
                                template1[i1] = preCalculate(substitutionStep(0, allArrayGroups[i].allGroups[k].allPosGr[m].inGroup[i1], symb));
                                template2[i1] = preCalculate(substitutionStep(0 + stepCycle, allArrayGroups[i].allGroups[k].allPosGr[m].inGroup[i1], symb));
                            }

                            // find shift						
                            allOk = false;
                            for (int k1 = 1; k1 < size; ++k1)
                            {
                                shift = k1;
                                allOk = true;
                                for (int i = shift; i < size; ++i)
                                {
                                    SgExprListExp *compare = new SgExprListExp(SUBT_OP);
                                    SgExpression *zero = NULL;
                                    compare->setLhs(template1[i]);
                                    compare->setRhs(template2[i - shift]);
                                    zero = preCalculate(compare);
                                    if (zero->variant() == INT_VAL)
                                    {
                                        if (zero->valueInteger() != 0)
                                        {
                                            allOk = false;
                                            break;
                                        }
                                    }
                                    else
                                    {
                                        allOk = false;
                                        break;
                                    }
                                }
                                if (allOk)
                                    break;
                                else
                                    allOk = false;
                            }

                            // if found
                            if (allOk)
                            {
                                char buf[32];
                                char *newName = new char[strlen(allArrayGroups[i].arrayName->identifier()) + 32];

                                buf[0] = '\0';
                                sprintf(buf, "%d", generator);
                                generator++;
                                newName[0] = '\0';
                                strcat(newName, allArrayGroups[i].arrayName->identifier());
                                strcat(newName, "_");
                                strcat(newName, buf);
                                allArrayGroups[i].allGroups[k].allPosGr[m].replaceInfo.newArray = new SgSymbol(VARIABLE_NAME, newName);
                                allArrayGroups[i].allGroups[k].allPosGr[m].replaceInfo.dimSize.push_back(allArrayGroups[i].allGroups[k].allPosGr[m].inGroup.size());
                                sortInGroup(allArrayGroups[i].allGroups[k].allPosGr[m]);
                                // добавить определение размерности заменяемого выражения
                                createLoadsAndStores(allArrayGroups[i].allGroups[k].allPosGr[m], allArrayGroups[i].allGroups[k].allPosGr[m].replaceInfo, allArrayGroups[i], k, allArrayGroups[i].allGroups[k]);
                                createSwaps(allArrayGroups[i].allGroups[k].allPosGr[m].replaceInfo);
                            }

                            delete []template1;
                            delete []template2;
                        }
                    }
                }
            }
        }
    }
}

// main functions for <-gpuO1>. All above for this
AnalyzeReturnGpuO1 analyzeLoopBody(int type)
{
    SgStatement *loop_body_start = loop_body;
    SgStatement *analyze_stmt = loop_body_start;
    SgExpression *tmp = NULL;
    SgExpression *dvm_dir_pattern = NULL;
    std::set<SgSymbol*> private_vars;
    std::vector<AnalyzeStat> allStat;	
    std::vector<BestPattern> best_patterns;
    std::vector<ArrayGroup> allArrayGroup;
    bool ifBreak = false;
    std::set<int> otherVars;
        
    // изменить!!! убрать
    int lastDLVL = DVM_DEBUG_LVL;
    DVM_DEBUG_LVL = 2;

    loopVars.clear();
    scalar_stmts.clear();
    
    tmp = dvm_parallel_dir->expr(2);
    while(tmp)
    {
        loopVars.push_back(tmp->lhs()->symbol());
        tmp = tmp->rhs();
    }

    if(DVM_DEBUG_LVL > 1)
        if(file == NULL)
            file = fopen("log_optimization.txt", "w+");

    if(DVM_DEBUG_LVL > 1)
        if(fileStmts == NULL)
            fileStmts = fopen("log_stms.txt", "w+");

    dvm_dir_pattern = dvm_parallel_dir->expr(0)->lhs();	
    tmp = dvm_parallel_dir->expr(1);
    
    while(tmp)
    {
        SgExpression *t = tmp->lhs();		
        if(t->variant() == ACC_PRIVATE_OP)
        {
            t = t->lhs();
            while(t)
            {
                SgExpression *t1 = &t->lhs()->copy();				
                private_vars.insert(t1->symbol());
                //printf("symbol as private: %s\n",t1->symbol()->identifier());
                t = t->rhs();
            }
            break;
        }
        tmp = tmp->rhs();
    }
    
    // all stmts is not in internal loop 
    //loopMultCount = 1;

    if(DVM_DEBUG_LVL > 1)
        fprintf(file, "start analyze stmts in LOOP on line number %d\n", first_do_par->lineNumber());
    while(analyze_stmt)
    {		
        if(analyze_stmt->variant() == ASSIGN_STAT)
        {
            SgSymbol *s = analyze_stmt->expr(0)->symbol();
            SgExpression *ex = analyze_stmt->expr(0);

            only_scalar = true;
            operation = WRITE;
            analyzeVarRef(private_vars, allStat, s, ex);
            if(analyze_stmt->expr(1))
            {
                //printf("start\n");
                //analyze_stmt->expr(1)->unparsestdout();
                operation = READ;
                analyzeRightAssing(private_vars, allStat, analyze_stmt->expr(1));
                //printf("\nend\n\n");
            }
            if(only_scalar)
                scalar_stmts.push_back(analyze_stmt);
        }
        else if(analyze_stmt->variant() == FOR_NODE) // не предусмотрена вложенность внутренних циклов !!!
        {
            int step = 1;
            bool exStep = true;			
            SgExpression *ex = NULL;

            symbolsOfForNode.push_back(analyze_stmt->symbol());
            controlEndsOfForStmt.push(analyze_stmt->lastNodeOfStmt());

            if(analyze_stmt->expr(1))
            {
                ex = Calculate(analyze_stmt->expr(1));
                if(ex->variant() == INT_VAL)
                    step = ex->valueInteger();
                else
                    exStep = false;
                fprintf(file, "step is %s \n", copyOfUnparse(analyze_stmt->expr(1)->unparse()));
            }

            if(exStep)
            {				
                if(analyze_stmt->expr(0)->variant() == DDOT)
                {
                    SgExprListExp *exprL = new SgExprListExp(SUBT_OP);
                    
                    globalStep.push_back(step);
                    lBound.push_back(analyze_stmt->expr(0)->lhs());
                    rBound.push_back(analyze_stmt->expr(0)->rhs());
                    loopMultCount.push_back(-999);
                    exprL->setLhs(rBound[rBound.size() - 1]);
                    exprL->setRhs(lBound[lBound.size() - 1]);

                    ex = preCalculate(exprL);
                    ex = Calculate(ex);
                    if(ex->variant() == INT_VAL)
                    {						
                        loopMultCount[loopMultCount.size() - 1] = ((abs(ex->valueInteger()) + 1) / abs(step));
                        actualDocycle.push_back(1);
                        if(DVM_DEBUG_LVL > 1)
                            fprintf(file, " Change loopMultCount by number %d with symbol %s, calculation value = %d, [%s, %s]\n", loopMultCount[loopMultCount.size() - 1], symbolsOfForNode[symbolsOfForNode.size() - 1]->identifier(), ex->valueInteger(), copyOfUnparse(lBound[lBound.size() - 1]->unparse()), copyOfUnparse(rBound[rBound.size() - 1]->unparse()));
                    }
                    else
                    {
                        unknownLoop = true;
                        actualDocycle.push_back(1);
                        loopMultCount[loopMultCount.size() - 1] = 1;
                        fprintf(file, " **[ATTANTION]**: can't calculate expression << %s >> with variant %d\n", copyOfUnparse(ex->unparse()), analyze_stmt->expr(0)->variant());
                    }
                }
            }	
        }
        else if(analyze_stmt->variant() == CONTROL_END)
        {			
            if (controlEndsOfForStmt.size() != 0)
            {
                if (analyze_stmt == controlEndsOfForStmt.top())
                {
                    loopMultCount.pop_back();
                    symbolsOfForNode.pop_back();
                    lBound.pop_back();
                    rBound.pop_back();
                    actualDocycle.pop_back();
                    globalStep.pop_back();
                    controlEndsOfForStmt.pop();

                    if (DVM_DEBUG_LVL > 1)
                        fprintf(file, "   Return back value of loopMultCount\n");
                }
            }
            else if (controlEndsOfIfStmt.size() != 0)
            {
                if (analyze_stmt == controlEndsOfIfStmt.top())
                    controlEndsOfIfStmt.pop();
            }
            else
            {
                if (DVM_DEBUG_LVL > 1)
                    fprintf(file, " **[ATTENTION]**: unknown CONTROL_END!! It may be end of local \"loop_body\" \n");
            }
        }
        else if (analyze_stmt->variant() == IF_NODE || analyze_stmt->variant() == ELSEIF_NODE)// || analyze_stmt->variant() == LOGIF_NODE)
        {
            SgExpression *ex = analyze_stmt->expr(0);
            SgIfStmt *tmpIf = (SgIfStmt*)analyze_stmt;

            if (tmpIf->falseBody())
            {
                if (tmpIf->falseBody()->variant() != ELSEIF_NODE)
                    controlEndsOfIfStmt.push(analyze_stmt->lastNodeOfStmt());
            }
            else
                controlEndsOfIfStmt.push(analyze_stmt->lastNodeOfStmt());

            if(existEqOp(ex))
            {
                if (tmpIf->falseBody())
                {
                    if (tmpIf->falseBody()->variant() == ELSEIF_NODE)
                    {
                        analyze_stmt = tmpIf->falseBody();
                        continue;
                    }
                    else
                        analyze_stmt = tmpIf->falseBody();
                }
                else
                {
                    analyze_stmt = tmpIf->lastNodeOfStmt();
                    controlEndsOfIfStmt.pop();
                }
            }				
        }
        else
        {
            if(DVM_DEBUG_LVL > 1)
                otherVars.insert(analyze_stmt->variant());
        }
        if(DVM_DEBUG_LVL > 1)			
            fprintf(fileStmts, "%s \n", copyOfUnparse(analyze_stmt->unparse()));

        analyze_stmt = analyze_stmt->lexNext();
    }	
    
    if(DVM_DEBUG_LVL > 1)
    {
        for(std::set<int>::iterator t = otherVars.begin(); t != otherVars.end(); t++)
                fprintf(file, " [INFO] other variant is %d\n", *t);

        fprintf(file, "finish analyze stmts\n");
        fprintf(fileStmts, "//--------------------------------- end -------------------------------//\n\n");

        fflush(file);
        fflush(fileStmts);
    }

    if(!ifBreak)
    {
        // <-gpuO1 lvl1> BLOCK 
        findBest(allStat, best_patterns, dvm_dir_pattern);
        correctBestPattern(allStat, best_patterns, dvm_dir_pattern);
        generateOptimalExpressions(allStat, best_patterns, newVars);
        // end BLOCK

        // <-gpuO1 lvl2> BLOCK
        /*if (type == NON_ACROSS_TYPE && unknownLoop == false) 
        {
            findGroups(allStat, allArrayGroup);
            correctGroups(allArrayGroup);
            correctLoopBody(allArrayGroup);
        }*/
        // end BLOCK
        
        if(DVM_DEBUG_LVL > 1)
        {
            fprintf(file, "allStat size %u\n", (unsigned) allStat.size());
        
            for(size_t i = 0; i < allStat.size(); ++i)
            {
                fprintf(file, " name of array %s\n", allStat[i].name_of_array->identifier());
                fprintf(file, "  patterns size %u\n", (unsigned) allStat[i].patterns.size());
                for(size_t k = 0; k < allStat[i].patterns.size(); ++k)
                {	
                    if(allStat[i].patterns[k].count_write_op != 0)
                    {
                        fprintf(file, "    ex W = %d; ", allStat[i].patterns[k].count_write_op);
                        fprintf(file, "(%s)\n", copyOfUnparse(allStat[i].patterns[k].symbs->unparse()));
                    }
                }

                for(size_t k = 0; k < allStat[i].patterns.size(); ++k)
                {	
                    if(allStat[i].patterns[k].count_read_op != 0)
                    {
                        fprintf(file, "    ex R = %d; ", allStat[i].patterns[k].count_read_op);
                        fprintf(file, "(%s)\n", copyOfUnparse(allStat[i].patterns[k].symbs->unparse()));
                    }
                }

                if(best_patterns.size() != 0)
                {
                    fprintf(file, "  best pattern: ");
                    for(size_t k = 0; k < best_patterns[i].what.size(); ++k)
                        fprintf(file, "%d ", best_patterns[i].what[k]);

                    fprintf(file, " with count_of_pattern %d\n",  best_patterns[i].count_of_pattern);
                }
            }

            fprintf(file, "scalar_stmts size %u\n", (unsigned) scalar_stmts.size());
            for(size_t i = 0; i < scalar_stmts.size(); ++i)
            {
                fprintf(file, " stmt ");
                fprintf(file, "%s", copyOfUnparse(scalar_stmts[i]->unparse()));
            }
            fprintf(file, "finish analyze stmts\n");
            fprintf(file, "//--------------------------------- end -------------------------------//\n\n");
        }

        DVM_DEBUG_LVL = lastDLVL;
        if(newVars.size() != 0)
        {
            printf("  -------- Loop on line %d was optimized ---------- \n", first_do_par->lineNumber());
            correctPrivateList(ADD);
        }
    }

    AnalyzeReturnGpuO1 retStruct;
    retStruct.allArrayGroup = allArrayGroup;
    retStruct.allStat = allStat;
    retStruct.bestPatterns = best_patterns;

    return retStruct;
}

// optimization of one ACROSS, that is needed. BLOCK start

SgExpression* replaceInEx(std::vector<newInfo> &allNewInfo, std::vector<acrossInfo> &allInfo, SgExpression *ex, SgExpression *parent, int LR)
{
    SgExpression *ret = NULL;
    if (ex->variant() == ARRAY_REF)
    {
        char *name = ex->symbol()->identifier();
        for (size_t i = 0; i < allInfo.size(); ++i)
        {
            if (strcmp(name, allInfo[i].nameOfArray) == 0)
            {
                SgArrayRefExp *arrayEx = new SgArrayRefExp(*allNewInfo[i].newArray);
                SgExpression *list = ex->lhs();
                for (size_t k = 0; k < allInfo[i].dims.size(); ++k)
                {
                    if (allInfo[i].dims[k] != 1 && allInfo[i].acrossPos != (int)k)
                    {
                        arrayEx->addSubscript(*&list->lhs()->copy());
                    }
                    else if (allInfo[i].acrossPos == (int)k)
                    {
                        arrayEx->addSubscript(*&list->lhs()->copy() - *new SgVarRefExp(allInfo[i].symbs[k]));
                    }
                    list = list->rhs();
                }
                if (LR == 1)
                    parent->setLhs(arrayEx);
                else if (LR == 2)
                    parent->setRhs(arrayEx);
                else
                    ret = arrayEx;
                break;
            }
        }
    }
    else
    {
        if (ex->lhs())
            replaceInEx(allNewInfo, allInfo, ex->lhs(), ex, 1);
        if (ex->rhs())
            replaceInEx(allNewInfo, allInfo, ex->rhs(), ex, 2);
    }
    return ret;
}

void replace(std::vector<newInfo> &allNewInfo, std::vector<acrossInfo> &allInfo)
{
    SgStatement *body = loop_body;
    while (body)
    {
        if (body->variant() == ASSIGN_STAT)
        {
            SgExpression *left, *right;
            left = replaceInEx(allNewInfo, allInfo, body->expr(0), NULL, 3);
            right = replaceInEx(allNewInfo, allInfo, body->expr(1), NULL, 3);
            if (left != NULL && right != NULL)
            {
                body->setExpression(0, *left);
                body->setExpression(1, *right);
                
            }
            else if (left != NULL)
            {
                body->setExpression(0, *left);
            }
            else if (right != NULL)
            {
                body->setExpression(1, *right);
            }
        }
        body = body->lexNext();
    }
}

void createSwaps(newInfo &info, acrossInfo &oldInfo, int pos, std::vector<int> idxVal)
{
    if (info.dimSize.size() - 1 == (size_t)pos) // last and across
    {
        //down
        for (int i = oldInfo.widthL; i < oldInfo.widthR; ++i)
        {
            SgArrayRefExp *arrayEx = new SgArrayRefExp(*info.newArray);
            SgArrayRefExp *arrayExLast = new SgArrayRefExp(*info.newArray);
            
            for (size_t k = 0; k < idxVal.size(); ++k)
            {
                arrayEx->addSubscript(*new SgValueExp(idxVal[k]));
                arrayExLast->addSubscript(*new SgValueExp(idxVal[k]));
            }
            arrayEx->addSubscript(*new SgValueExp((int)i));
            arrayExLast->addSubscript(*new SgValueExp((int)(i + 1)));
            info.swapsDown.push_back(new SgAssignStmt(*arrayEx, *arrayExLast));
        }

        //up
        for (int i = oldInfo.widthR; i > oldInfo.widthL; i--)
        {
            SgArrayRefExp *arrayEx = new SgArrayRefExp(*info.newArray);
            SgArrayRefExp *arrayExLast = new SgArrayRefExp(*info.newArray);

            for (size_t k = 0; k < idxVal.size(); ++k)
            {
                arrayEx->addSubscript(*new SgValueExp(idxVal[k]));
                arrayExLast->addSubscript(*new SgValueExp(idxVal[k]));
            }
            arrayEx->addSubscript(*new SgValueExp((int)i));
            arrayExLast->addSubscript(*new SgValueExp((int)(i - 1)));
            info.swapsUp.push_back(new SgAssignStmt(*arrayEx, *arrayExLast));
        }
    }
    else
    {
        for (int i = 1; i <= info.dimSize[pos]; ++i)
        {
            std::vector<int> newIdx = idxVal;
            newIdx.push_back((int)i);
            createSwaps(info, oldInfo, pos + 1, newIdx);
        }
    }
}

void createLoadsAndStores(newInfo &info, acrossInfo &oldInfo, int pos, std::vector<int> idxVal)
{
    if (info.dimSize.size() - 1 == (size_t)pos) // last and across
    {		
        for (int i = oldInfo.widthL; i <= oldInfo.widthR; ++i)
        {
            SgArrayRefExp *arrayEx = new SgArrayRefExp(*info.newArray);
            SgArrayRefExp *oldArrayEx = new SgArrayRefExp(*oldInfo.symbol);
            int idxValp = 0;
            for (size_t k = 0; k < oldInfo.dims.size(); ++k)
            {
                if (oldInfo.dims[k] == 1)
                {
                    if ((int)k == oldInfo.acrossPos)
                        oldArrayEx->addSubscript(*new SgVarRefExp(oldInfo.symbs[k]) + *new SgValueExp((int)i));
                    else
                        oldArrayEx->addSubscript(*new SgVarRefExp(oldInfo.symbs[k]));
                }
                else
                {
                    oldArrayEx->addSubscript(*new SgValueExp(idxVal[idxValp]));
                    idxValp++;
                }
            }

            for (size_t k = 0; k < idxVal.size(); ++k)
            {
                arrayEx->addSubscript(*new SgValueExp(idxVal[k]));
            }
            arrayEx->addSubscript(*new SgValueExp((int)i));
            
            if (i == oldInfo.widthR)
            {
                info.loadsInForPlus.push_back(new SgAssignStmt(*arrayEx, *oldArrayEx));
                info.loadsBeforeMinus.push_back(new SgAssignStmt(*arrayEx, *oldArrayEx));
            }
            else if (i == oldInfo.widthL)
            {
                info.loadsBeforePlus.push_back(new SgAssignStmt(*arrayEx, *oldArrayEx));
                info.loadsInForMinus.push_back(new SgAssignStmt(*arrayEx, *oldArrayEx));					
            }
            else
            {
                info.loadsBeforePlus.push_back(new SgAssignStmt(*arrayEx, *oldArrayEx));
                info.loadsBeforeMinus.push_back(new SgAssignStmt(*arrayEx, *oldArrayEx));
            }
            if (i == 0)
                info.stores.push_back(new SgAssignStmt(*oldArrayEx, *arrayEx));
        }
    }
    else // non across
    {
        for (int i = 1; i <= info.dimSize[pos]; ++i)
        {			
            std::vector<int> newIdx = idxVal;
            newIdx.push_back((int)i);
            createLoadsAndStores(info, oldInfo, pos + 1, newIdx);
        }
    }
}

SgSymbol* searchOneIdx(SgExpression *ex)
{
    SgSymbol *ret = NULL;
    if (ex->variant() == VAR_REF)
    {
        for (size_t i = 0; i < loopVars.size(); ++i)
        {
            if (strcmp(loopVars[i]->identifier(), ex->symbol()->identifier()) == 0)
            {
                ret = loopVars[i];
                break;
            }
        }
    }
    else
    {
        if (ex->lhs() && ret == NULL)
        {
            ret = searchOneIdx(ex->lhs());
            if (ret == NULL && ex->rhs())
                ret = searchOneIdx(ex->rhs());
        }
    }
    return ret;
}

void searchIdxs(std::vector<acrossInfo> &allInfo, SgExpression *st)
{
    if (st->variant() == ARRAY_REF)
    {
        for (size_t i = 0; i < allInfo.size(); ++i)
        {
            if (strcmp(allInfo[i].nameOfArray, st->symbol()->identifier()) == 0)
            {
                int p = 0;
                SgExpression *list = st->lhs();
                while (list)
                {
                    if (allInfo[i].dims[p] == 0)
                    {
                        SgSymbol *stmp = searchOneIdx(list->lhs());
                        if (stmp != NULL)
                        {
                            allInfo[i].dims[p] = 1;
                            allInfo[i].symbs[p] = stmp;
                        }
                    }
                    list = list->rhs();
                    p++;
                }
                break;
            }
        }
    }
    else
    {
        if (st->lhs())
            searchIdxs(allInfo, st->lhs());
        if (st->rhs())
            searchIdxs(allInfo, st->rhs());
    }
}

void optimizeLoopBodyForOne(std::vector<newInfo> &allNewInfo)
{
    SgExpression *tmp = dvm_parallel_dir->expr(1);
    std::vector<acrossInfo> allInfo;
    bool nextStep;

    while (tmp)
    {
        SgExpression *t = tmp->lhs();
        if (t->variant() == ACROSS_OP)
        {
            std::vector<SgExpression *> toAnalyze;
            if (t->lhs()->variant() == EXPR_LIST)
                toAnalyze.push_back(t->lhs());
            else
            {
                if (t->lhs()->variant() == DDOT)
                    toAnalyze.push_back(t->lhs()->rhs());

                if (t->rhs())
                    if (t->rhs()->variant() == DDOT)
                        toAnalyze.push_back(t->rhs()->rhs());
            }

            for (int i = 0; i < toAnalyze.size(); ++i)
            {
                t = toAnalyze[i];
                while (t)
                {
                    acrossInfo tmpI;
                    tmpI.nameOfArray = t->lhs()->symbol()->identifier();
                    tmpI.symbol = t->lhs()->symbol();
                    tmpI.allDim = 0;
                    tmpI.widthL = 0;
                    tmpI.widthR = 0;
                    tmpI.acrossPos = 0;
                    tmpI.acrossNum = 0;
                    SgExpression *tt = t->lhs()->lhs();
                    int position = 0;
                    while (tt)
                    {
                        bool here = true;
                        if (tt->lhs()->lhs()->valueInteger() != 0)
                        {
                            tmpI.acrossPos = position;
                            tmpI.acrossNum++;
                            tmpI.widthL = (-1) * tt->lhs()->lhs()->valueInteger();
                            here = false;
                        }
                        if (tt->lhs()->rhs()->valueInteger() != 0)
                        {
                            tmpI.acrossPos = position;
                            if (here)
                                tmpI.acrossNum++;
                            tmpI.widthR = tt->lhs()->rhs()->valueInteger();
                        }
                        position++;
                        tt = tt->rhs();
                    }
                    for (int i = 0; i < position; ++i)
                    {
                        tmpI.dims.push_back(0);
                        tmpI.symbs.push_back(NULL);
                    }
                    allInfo.push_back(tmpI);

                    t = t->rhs();
                }
            }
            break;
        }
        tmp = tmp->rhs();
    }

    nextStep = true;
    for (size_t i = 0; i < allInfo.size(); ++i)
    {
        if (allInfo[i].acrossNum > 1)
        {
            nextStep = false;
            break;
        }
    }

    if (nextStep)
    {
        SgStatement *st = loop_body;
        loopVars.clear();

        tmp = dvm_parallel_dir->expr(2);
        while (tmp)
        {
            loopVars.push_back(tmp->lhs()->symbol());
            tmp = tmp->rhs();
        }

        while (st)
        {
            if (st->variant() == ASSIGN_STAT)
            {
                searchIdxs(allInfo, st->expr(0));
                searchIdxs(allInfo, st->expr(1));
            }
            st = st->lexNext();
        }

        for (size_t i = 0; i < allInfo.size(); ++i)
        {
            if (allInfo[i].symbs[allInfo[i].acrossPos] == NULL)
            {
                nextStep = false;
                break;
            }
        }

        if (nextStep)
        {
            for (size_t i = 0; i < allInfo.size(); ++i)
            {
                for (size_t k = 0; k < allInfo[i].dims.size(); ++k)
                {
                    if (allInfo[i].dims[k] == 0)
                    {
                        SgArrayType *tArr = isSgArrayType(allInfo[i].symbol->type());
                        if (tArr != NULL)
                        {
                            SgExpression *dimList =  tArr->getDimList();
                            if (dimList != NULL)
                            {
                                size_t p = 0;
                                while (dimList && p != k)
                                {
                                    p++;
                                    dimList = dimList->rhs();
                                }
                                // сделать для DDOT !!
                                int val = dimList->lhs()->valueInteger();
                                allInfo[i].dims[k] = val;
                            }
                        }
                    }
                }
            }

            for (size_t i = 0; i < allInfo.size(); ++i)
            {
                for (size_t k = 0; k < allInfo[i].dims.size(); ++k)
                {
                    if (allInfo[i].dims[k] == 0)
                    {
                        nextStep = false;
                        break;
                    }
                }
            }

            if (nextStep)
            {
                for (size_t i = 0; i < allInfo.size(); ++i)
                {
                    char *newName = new char[strlen(allInfo[i].nameOfArray) + 2];
                    newName[0] = '\0';
                    strcat(newName, allInfo[i].nameOfArray);
                    strcat(newName, "_");
                    newInfo tmpNewInfo;
                    tmpNewInfo.newArray = new SgSymbol(VARIABLE_NAME, TestAndCorrectName(newName));
                    SgArrayType *tpArrNew = new SgArrayType(*allInfo[i].symbol->type());
                    for (size_t k = 0; k < allInfo[i].dims.size(); ++k)
                    {
                        // доделать для DDOT
                        if (allInfo[i].dims[k] != 1)
                        {
                            tpArrNew->addDimension(new SgValueExp(allInfo[i].dims[k]));
                            tmpNewInfo.dimSize.push_back(allInfo[i].dims[k]);
                        }
                    }

                    SgExprListExp *ex = new SgExprListExp(DDOT);
                    ex->setLhs(*new SgValueExp(allInfo[i].widthL));
                    ex->setRhs(*new SgValueExp(allInfo[i].widthR));
                    tpArrNew->addDimension(ex);
                    tmpNewInfo.newArray->setType(tpArrNew);

                    tmpNewInfo.dimSize.push_back(abs(allInfo[i].widthR - allInfo[i].widthL) + 1);
                    allNewInfo.push_back(tmpNewInfo);
                }

                //create loads and stores
                // доделать для DDOT
                for (size_t i = 0; i < allNewInfo.size(); ++i)
                {
                    std::vector<int> tmp;
                    createLoadsAndStores(allNewInfo[i], allInfo[i], 0, tmp);
                    createSwaps(allNewInfo[i], allInfo[i], 0, tmp);
                }

                replace(allNewInfo, allInfo);
                for (size_t i = 0; i < allNewInfo.size(); ++i)
                    newVars.push_back(allNewInfo[i].newArray);
                if (newVars.size() != 0)
                {
                    correctPrivateList(ADD);
                    printf("  -------- Loop on line %d was optimized ---------- \n", first_do_par->lineNumber());
                }
                // TMP PRINT
                /*printf("plus before assigns\n");
                for (size_t i = 0; i < allNewInfo[0].loadsBeforePlus.size(); ++i)
                {
                    allNewInfo[0].loadsBeforePlus[i]->unparsestdout();
                }
                printf("minus before assigns\n");
                for (size_t i = 0; i < allNewInfo[0].loadsBeforeMinus.size(); ++i)
                {
                    allNewInfo[0].loadsBeforeMinus[i]->unparsestdout();
                }
                printf("plus in FOR assigns\n");
                for (size_t i = 0; i < allNewInfo[0].loadsInForPlus.size(); ++i)
                {
                    allNewInfo[0].loadsInForPlus[i]->unparsestdout();
                }
                printf("minus in FOR assigns\n");
                for (size_t i = 0; i < allNewInfo[0].loadsInForMinus.size(); ++i)
                {
                    allNewInfo[0].loadsInForMinus[i]->unparsestdout();
                }
                printf("stores assigns\n");
                for (size_t i = 0; i < allNewInfo[0].stores.size(); ++i)
                {
                    allNewInfo[0].stores[i]->unparsestdout();
                }
                printf("swaps Down assigns\n");
                for (size_t i = 0; i < allNewInfo[0].swapsDown.size(); ++i)
                {
                    allNewInfo[0].swapsDown[i]->unparsestdout();
                }
                printf("swaps Up assigns\n");
                for (size_t i = 0; i < allNewInfo[0].swapsUp.size(); ++i)
                {
                    allNewInfo[0].swapsUp[i]->unparsestdout();
                }*/
            }
        }
    }
}
// BLOCK end
