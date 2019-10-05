#include "leak_detector.h"

#include "dvm.h"
#include "acc_analyzer.h"
#include "calls.h"
#include <fstream>
#include <stdint.h>

using std::string;
using std::vector;
using std::map;
using std::list;
using std::make_pair;
using std::set;
using std::pair;

#if __SPF && _WIN32
using std::wstring;
#include "../_src/Utils/AstWrapper.h"
#include "../_src/Utils/utils.h"

static pair<wstring, string> getText(const char *s, const wchar_t *s1, const char *t, int num, SgStatement *stmt, int &line)
{
    pair<wstring, string> ret;

    wchar_t bufW[1024];
    swprintf(bufW, s1, to_wstring(t).c_str());
    ret.first = bufW;

    char buf[1024];
    sprintf(buf, s, t);
    ret.second = buf;

    line = stmt->lineNumber();
    if (line == 0)
    {
        line = 1;
        if (stmt->variant() == DVM_PARALLEL_ON_DIR)
        {
            line = stmt->lexNext()->lineNumber();
            ret.first += L" дл€ этого цикла";
            ret.second += " for this loop";
        }
    }

    if (stmt->variant() == SPF_ANALYSIS_DIR)
    {
        ret.first += L" дл€ этого цикла";
        ret.second += " for this loop";
    }

    return ret;
}

static inline bool ifVarIsLoopSymb(SgStatement *stmt, const string symb)
{    
    bool ret = false;
    if (stmt == NULL)
        return ret;

    int var = stmt->variant();    
    if (var == SPF_ANALYSIS_DIR || var == SPF_PARALLEL_DIR || var == SPF_TRANSFORM_DIR || var == SPF_PARALLEL_REG_DIR || var == SPF_END_PARALLEL_REG_DIR)
        stmt = stmt->lexNext();
        
    SgForStmt *forS = isSgForStmt(stmt);
    if (forS)
    {
        SgStatement *end = forS->lastNodeOfStmt();
        for (; stmt != end && !ret; stmt = stmt->lexNext())
            if (stmt->variant() == FOR_NODE)
                if (isSgForStmt(stmt)->symbol()->identifier() == symb)
                    ret = true;
    }

    return ret;
}


template<typename fillType> void fillPrivatesFromComment(Statement *st, std::set<fillType> &privates);

inline void Warning(const char *s, const wchar_t *s1, const char *t, int num, SgStatement *stmt)
{
    //TODO: is it correct?
    if (stmt == NULL)
        return;

    if (num == PRIVATE_ANALYSIS_REMOVE_VAR)
    {
        SgStatement *found = SgStatement::getStatementByFileAndLine(string(stmt->fileName()), stmt->lineNumber());
        if (found != NULL)
        {
            if (ifVarIsLoopSymb(found, t))
                return;
        }

        set<string> privates;
        fillPrivatesFromComment(new Statement(stmt), privates);
        if (privates.find(t) != privates.end())
            return;
    }

    
    int line;
    auto retVal = getText(s, s1, t, num, stmt, line);
    printLowLevelWarnings(stmt->fileName(), line, retVal.first.c_str(), retVal.second.c_str(), 1029);
}

inline void Note(const char *s, const wchar_t *s1, const char *t, int num, SgStatement *stmt)
{    
    int line;
    auto retVal = getText(s, s1, t, num, stmt, line);
    printLowLevelNote(stmt->fileName(), line, retVal.first.c_str(), retVal.second.c_str(), 1030);
}
#elif __SPF
inline void Warning(const char *s, const char *t, int num, SgStatement *stmt) { }
#endif

// local functions
static ControlFlowItem* getControlFlowList(SgStatement*, SgStatement*, ControlFlowItem**, SgStatement**, doLoops*, CallData*, CommonData*);
static ControlFlowItem* processOneStatement(SgStatement** stmt, ControlFlowItem** pred, ControlFlowItem **list, ControlFlowItem* oldcur, doLoops*, CallData*, CommonData*);
static ControlFlowItem* switchItem(SgStatement* stmt, ControlFlowItem* empty, SgStatement** lastAnStmt, doLoops* loops, CallData* calls, CommonData*);
static ControlFlowItem* ifItem(SgStatement*, ControlFlowItem*, SgStatement** lastAnStmt, doLoops* loops, bool ins, CallData*, CommonData*);
static void setLeaders(ControlFlowItem*);
static void clearList(ControlFlowItem*);
static void fillLabelJumps(ControlFlowItem*);
static SgExpression* GetProcedureArgument(bool isF, void* f, int i);
static int GetNumberOfArguments(bool isF, void* f);
#if ACCAN_DEBUG
static void printControlFlowList(ControlFlowItem*, ControlFlowItem* last = NULL);
#endif

//static ControlFlowGraph* GetControlFlowGraphWithCalls(bool, SgStatement*, CallData*, CommonData*);
//static void FillCFGSets(ControlFlowGraph*);
static void FillPrivates(ControlFlowGraph*);
static ControlFlowItem* AddFunctionCalls(SgStatement*, CallData*, ControlFlowItem**, CommonData*);

const char* is_correct = NULL;
const char* failed_proc_name = NULL;
static PrivateDelayedItem* privateDelayedList = NULL;
static AnalysedCallsList* currentProcedure = NULL;
static AnalysedCallsList* mainProcedure = NULL;
static DoLoopDataList* doLoopList = NULL;
static CommonData* pCommons;
static CallData* pCalls;

int total_privates = 0;
int total_pl = 0;

static const IntrinsicSubroutineData intrinsicData[] = {
    {"date_and_time", 4, { {-1, "date", INTRINSIC_OUT}, {-1, "time", INTRINSIC_OUT }, {-1, "zone", INTRINSIC_OUT }, {-1, "values", INTRINSIC_OUT } } },
    {"mod", 2, { {1, NULL, INTRINSIC_IN}, {2, NULL, INTRINSIC_IN} } },
    {"dvtime", 0, {}},
    {"abs", 1, { {1, NULL, INTRINSIC_IN} } },
    {"max", 2, { {1, NULL, INTRINSIC_IN}, {2, NULL, INTRINSIC_IN} } },
    {"min", 2, { {1, NULL, INTRINSIC_IN}, {2, NULL, INTRINSIC_IN} } },
    {"wtime", 1, { {1, NULL, INTRINSIC_IN} } },
    {"dble", 1, { {1, NULL, INTRINSIC_IN } } },
    {"dabs", 1, { {1, NULL, INTRINSIC_IN } } },
    {"dmax1", 2, { {1, NULL, INTRINSIC_IN}, {2, NULL, INTRINSIC_IN } } },
    {"dmin1", 2, { {1, NULL, INTRINSIC_IN}, {2, NULL, INTRINSIC_IN } } },
    {"dsqrt", 1, { {1, NULL, INTRINSIC_IN} } },
    {"dcos", 1, { {1, NULL, INTRINSIC_IN} } },
    {"datan2", 2, { {1, NULL, INTRINSIC_IN}, {2, NULL, INTRINSIC_IN} } },
    {"dsign", 2, { {1, NULL, INTRINSIC_IN}, {2, NULL, INTRINSIC_IN} } },
    {"dlog", 1, { {1, NULL, INTRINSIC_IN} } },
    {"dexp", 1, { {1, NULL, INTRINSIC_IN} } },
    {"omp_get_wtime", 0, {}},
    {"sqrt", 1, { {1, NULL, INTRINSIC_IN} } },
    {"int", 1, { {1, NULL, INTRINSIC_IN} } },
    {"iabs", 1, { {1, NULL, INTRINSIC_IN} } },
    {"fnpr", 4, { {1, NULL, INTRINSIC_IN},{ 2, NULL, INTRINSIC_IN },{ 3, NULL, INTRINSIC_IN },{ 4, NULL, INTRINSIC_IN } } },
    {"isnan", 1, { {1, NULL, INTRINSIC_IN } } }
};

//TODO: it does not work
//static map<SgStatement*, tuple<ControlFlowGraph*, CallData*, CommonData*>> CFG_cache;


static bool isIntrinsicFunctionNameACC(char* name)
{
#if USE_INTRINSIC_DVM_LIST
    return isIntrinsicFunctionName(name);
#else
    return false;
#endif
}

int SwitchFile(int file_id)
{
    if (file_id == current_file_id)
        return file_id;
    int stored_file_id = current_file_id;
    current_file_id = file_id;
    current_file = &(CurrentProject->file(current_file_id));
    return stored_file_id;
}

SgStatement * lastStmtOfDoACC(SgStatement *stdo)
{
    // is a copied function
    SgStatement *st;
    // second version  (change 04.03.08) 
    st = stdo;
RE: st = st->lastNodeOfStmt();
    if ((st->variant() == FOR_NODE) || (st->variant() == WHILE_NODE))
        goto RE;

    else if (st->variant() == LOGIF_NODE)
        return(st->lexNext());

    else
        return(st);

}

#ifdef __SPF
bool IsPureProcedureACC(SgSymbol* s)
#else
static bool IsPureProcedureACC(SgSymbol* s)
#endif
{
    // is a copied function
    SgSymbol *shedr = NULL;

#ifndef __SPF 
    shedr = GetProcedureHeaderSymbol(s);
#else
    if (ATTR_NODE(s))
        shedr = (GRAPHNODE(s)->symb);
#endif
    if (shedr)
        return(shedr->attributes() & PURE_BIT);
    else
        return 0;
}

static bool IsUserFunctionACC(SgSymbol* s)
{
    // is a copied function
    return(s->attributes() & USER_PROCEDURE_BIT);
}

static const IntrinsicSubroutineData* IsAnIntrinsicSubroutine(const char* name)
{
    for (int i = 0; i < sizeof(intrinsicData)/ sizeof(intrinsicData[0]); i++)
        if (strcmp(name, intrinsicData[i].name) == 0)
            return &(intrinsicData[i]);
    return NULL;
}

static SgExpression* CheckIntrinsicParameterFlag(const char* name, int arg, SgExpression* p, unsigned char flag)
{
    const IntrinsicSubroutineData* info = IsAnIntrinsicSubroutine(name);
    if (!info)
        return NULL; //better avoid this
    for (int i = 0; i < info->args; i++) {
        const IntrinsicParameterData* pd = &(info->parameters[i]);
        if (pd->index == arg + 1)
            return (pd->status & flag) != 0 ? p : NULL;
        SgKeywordArgExp* kw = isSgKeywordArgExp(p);
        if (kw) {
            SgExpression* a = kw->arg();
            SgExpression* val = kw->value();
            if (strcmp(a->unparse(), pd->name) == 0) {
                return (pd->status & flag) != 0 ? val : NULL;
            }
        }
    }
    return NULL;
}
/*
//For parameters replacements in expressions
//#ifdef __SPF

VarsKeeper varsKeeper;

SgExpression* GetValueOfVar(SgExpression* var)
{
    return varsKeeper.GetValueOfVar(var);
}

void VarsKeeper::GatherVars(SgStatement* start)
{
    pCommons = &(data->commons);
    pCalls = &(data->calls);
    currentProcedure = data->calls.AddHeader(start, false, start->symbol());
    mainProcedure = currentProcedure;
    //stage 1: preparing graph data
    data->graph = GetControlFlowGraphWithCalls(true, start, &(data->calls), &(data->commons));
    data->calls.AssociateGraphWithHeader(start, data->graph);
    data->commons.MarkEndOfCommon(currentProcedure);
    //calls.printControlFlows();
    //stage 2: data flow analysis
    FillCFGSets(data->graph);
    //stage 3: fulfilling loop data
    FillPrivates(data->graph);

    if (privateDelayedList)
            delete privateDelayedList;
        privateDelayedList = NULL;
}

SgExpression* VarsKeeper::GetValueOfVar(SgExpression* var)
{
    FuncData* curData = data;
}

//#endif
*/



void SetUpVars(CommonData* commons, CallData* calls, AnalysedCallsList* m, DoLoopDataList* list)
{
    pCommons = commons;
    pCalls = calls;
    currentProcedure = m;
    mainProcedure = currentProcedure;
    doLoopList = list;
}

AnalysedCallsList* GetCurrentProcedure()
{
    return currentProcedure;
}
//interprocedural analysis, called for main procedure
void Private_Vars_Analyzer(SgStatement* start)
{
#ifndef __SPF 
    if (!options.isOn(PRIVATE_ANALYSIS)) {
        return;
    }
#endif
    CallData calls;
    CommonData commons;
    DoLoopDataList doloopList;
    SetUpVars(&commons, &calls, calls.AddHeader(start, false, start->symbol(), current_file_id), &doloopList);

    //stage 1: preparing graph data
    ControlFlowGraph* CGraph = GetControlFlowGraphWithCalls(true, start, &calls, &commons);
    calls.AssociateGraphWithHeader(start, CGraph);
    commons.MarkEndOfCommon(currentProcedure);
        
    currentProcedure->graph->getPrivate();
#if ACCAN_DEBUG
    calls.printControlFlows();
#endif
    //stage 2: data flow analysis
    FillCFGSets(CGraph);
    //stage 3: fulfilling loop data
    FillPrivates(CGraph);

    //test: graphvis
    /*std::fstream fs;
    fs.open("graph_old.txt", std::fstream::out);
    fs << CGraph->GetVisualGraph(&calls);
    fs.close();*/

#if !__SPF
    delete CGraph;
#endif

    if (privateDelayedList)
        delete privateDelayedList;
    privateDelayedList = NULL;
}

CallData::~CallData()
{
#if __SPF
    removeFromCollection(this);
    return;
#endif
    /*
    for (AnalysedCallsList* l = calls_list; l != NULL;) 
    {
        if (!l->isIntrinsic && l->graph)
        {
            if (l->graph->RemoveRef() && !l->graph->IsMain())
            {
                delete l->graph;
                l->graph = NULL;
            }
        }
        AnalysedCallsList *temp = l;
        l = l->next;
        delete temp;
        temp = NULL;
    }*/
}

CommonData::~CommonData()
{
#if __SPF
    removeFromCollection(this);
    return;
#endif
    for (CommonDataItem* i = list; i != NULL;) {
        for (CommonVarInfo* info = i->info; info != NULL;) {
            CommonVarInfo* t = info;
            info = info->next;
            delete t;
        }
        CommonDataItem* tp = i;
        i = i->next;
        delete tp;
    }
}

ControlFlowGraph::~ControlFlowGraph()
{
#if __SPF
    removeFromCollection(this);
    return;
#endif
    while (common_def != NULL) 
    {
        CommonVarSet* t = common_def;
        common_def = common_def->next;
        delete t;
    }
    while (common_use != NULL) 
    {
        CommonVarSet* t = common_use;
        common_use = common_use->next;
        delete t;
    }

    if (def)
        delete def;    

    if (use)
        delete use;   

    if (!temp && pri)
        delete pri;    

    for (CBasicBlock *bb = first; bb != NULL;) 
    {
        CBasicBlock *tmp = bb;
        bb = bb->getLexNext();
            
        delete tmp;
        tmp = NULL;
    }
}

CBasicBlock::~CBasicBlock()
{
#if __SPF
    removeFromCollection(this);
    return;
#endif    

    CommonVarSet* d = getCommonDef();
    while (d != NULL) 
    {
        CommonVarSet* t = d;
        d = d->next;
        delete t;
    }

    d = getCommonUse();
    while (d != NULL) 
    {
        CommonVarSet* t = d;
        d = d->next;
        delete t;
    }

    for (BasicBlockItem* bbi = prev; bbi != NULL;)
    {
        BasicBlockItem *tmp = bbi;
        bbi = bbi->next;
        delete tmp;
        tmp = NULL;
    }

    for (BasicBlockItem *bbi = succ; bbi != NULL;)
    {
        BasicBlockItem *tmp = bbi;
        bbi = bbi->next;
        delete tmp;
        tmp = NULL;
    }

    if (def)
        delete def;    

    if (use)
        delete use;    

    if (old_mrd_out)
        delete old_mrd_out;
    
    if (old_mrd_in)
        delete old_mrd_in;
    
    if (mrd_in)
        delete mrd_in;
    
    if (mrd_out)
        delete mrd_out;
    
    if (old_lv_out)
        delete old_lv_out;
    
    if (old_lv_in)
        delete old_lv_in;
    
    if (lv_in)
        delete lv_in;
    
    if (lv_out)
        delete lv_out;
}

doLoops::~doLoops()
{
#if __SPF
    removeFromCollection(this);
    return;
#endif
    for (doLoopItem *it = first; it != NULL; ) 
    {
        doLoopItem *tmp = it;
        it = it->getNext();
        delete tmp;
    }
}

PrivateDelayedItem::~PrivateDelayedItem()
{
#if __SPF
    removeFromCollection(this);
    return;
#endif
    if (delay)
        delete delay;
    if (next)
        delete next;    
}

VarSet::~VarSet()
{
#if __SPF
    removeFromCollection(this);
    return;
#endif
    for (VarItem* it = list; it != NULL;) 
    {
        VarItem* tmp = it;
        it = it->next;
        if (tmp->var)
            if (tmp->var->RemoveReference())
                delete tmp->var;
        delete tmp;
    }
}

CommonVarSet::CommonVarSet(const CommonVarSet& c)
{    
    cvd = c.cvd;
    if (c.next)
        next = new CommonVarSet(*c.next);
    else
        next = NULL;

#if __SPF
    addToCollection(__LINE__, __FILE__, this, 22);
#endif
}

std::string ControlFlowGraph::GetVisualGraph(CallData* calls)
{
    std::string result;
    result += "digraph ";
    char tmp[512];
    AnalysedCallsList* cd = calls->GetDataForGraph(this);
    //if (cd == NULL || cd->header == NULL)
        sprintf(tmp, "g_%llx", (uintptr_t)this);
    //else
    //    sprintf(tmp, "g_%500s", cd->header->symbol());
    result += tmp;
    result += "{ \n";
    for (CBasicBlock* b = this->first; b != NULL; b = b->getLexNext()) {
        if (!b->IsEmptyBlock()) {
            result += '\t' + b->GetGraphVisDescription() + "[shape=box,label=\"";
            result += b->GetGraphVisData() + "\"];\n";
        }
    }
    for (CBasicBlock* b = first; b != NULL; b = b->getLexNext()) {
        if (!b->IsEmptyBlock())
            result += b->GetEdgesForBlock(b->GetGraphVisDescription(), true, "");
    }
    result += '}';
    ResetDrawnStatusForAllItems();
    return result;
}

void ControlFlowGraph::ResetDrawnStatusForAllItems() {
    for (CBasicBlock* b = first; b != NULL; b = b->getLexNext()) {
        for (ControlFlowItem* it = b->getStart(); it != NULL && (it->isLeader() == false || it == b->getStart()); it = it->getNext()) {
            it->ResetDrawnStatus();
        }
    }
}

std::string GetConditionWithLineNumber(ControlFlowItem* eit)
{
    std::string res;
    if (eit->getOriginalStatement()) {
        char tmp[16];
        sprintf(tmp, "%d: ", eit->getOriginalStatement()->lineNumber());
        res = tmp;
    }
    return res + eit->getExpression()->unparse();
}

std::string GetActualCondition(ControlFlowItem** pItem) {
    std::string res = "";
    ControlFlowItem* eit = *pItem;
    while (true) 
    {
        if (eit == NULL || eit->getJump() != NULL || eit->getStatement() != NULL) 
        {
            if (eit && eit->getJump() != NULL) 
            {
                if (eit->getExpression() != NULL) 
                {
                    *pItem = eit;
                    return GetConditionWithLineNumber(eit);
                }
                else 
                {
                    *pItem = NULL;
                    return res;
                }
                break;
            }
            *pItem = NULL;
            return res;
        }
        eit = eit->GetPrev();
    }
    return res;
}

std::string CBasicBlock::GetEdgesForBlock(std::string name, bool original, std::string modifier)
{
    std::string result;
    for (BasicBlockItem* it = getSucc(); it != NULL; it = it->next) {
        if (it->drawn)
            continue;
        it->drawn = true;
        char lo = original;
        std::string cond;
        ControlFlowItem* eit = NULL;
        bool pf = false;
        if (it->jmp != NULL) {
            if (it->jmp->getExpression() != NULL) {
                eit = it->jmp;
                cond = GetConditionWithLineNumber(eit);
            }
            else {
                pf = true;
                eit = it->jmp->GetPrev();
                cond = GetActualCondition(&eit);
            }
        }
        if (eit && eit->GetFriend()) {
            lo = false;
            eit = eit->GetFriend();
        }
        if (!it->block->IsEmptyBlock() || cond.length() != 0) {
            if (cond.length() != 0 && eit && !pf){
                char tmp[32];
                sprintf(tmp, "c_%llx", (uintptr_t)eit);
                if (!eit->IsDrawn()) {
                    result += '\t';
                    result += tmp;
                    result += "[shape=diamond,label=\"";
                    result += cond;
                    result += "\"];\n";
                }
                if (it->cond_value && !pf) {
                    result += '\t' + name + "->";
                    result += tmp;
                    result += modifier;
                    result += '\n';
                }
                eit->SetIsDrawn();
            }
            if (cond.length() != 0) {
                if (lo) {
                    char tmp[32];
                    sprintf(tmp, "c_%llx", (uintptr_t)eit);
                    if (!it->block->IsEmptyBlock()) {
                        result += '\t';
                        result += tmp;
                        result += "->" + it->block->GetGraphVisDescription();
                        result += "[label=";
                        result += (!pf && it->cond_value) ? "T]" : "F]";
                        result += ";\n";
                    }
                    else {
                        std::string n = tmp;
                        std::string label;
                        label += "[label=";
                        label += (!pf && it->cond_value) ? "T]" : "F]";
                        result += it->block->GetEdgesForBlock(n, original, label);
                    }
                }
            }
            else {
                result += '\t' + name + " -> " + it->block->GetGraphVisDescription();
                result += modifier;
                result += ";\n";
            }
            
        }
        else {
            result += it->block->GetEdgesForBlock(name, original, "");
        }
    }
    return result;
}

std::string CBasicBlock::GetGraphVisDescription()
{
    if (visname.length() != 0)
        return visname;
    char tmp[16];
    sprintf(tmp, "%d", num);
    visname = tmp;
    return visname;
}

std::string CBasicBlock::GetGraphVisData()
{
    if (visunparse.length() != 0)
        return visunparse;
    std::string result;
    for (ControlFlowItem* it = start; it != NULL && (it->isLeader() == false || it == start); it = it->getNext()) {
        if (it->getStatement() != NULL) {
            int ln = it->GetLineNumber();
            char tmp[16];
            sprintf(tmp, "%d: ", ln);
            result += tmp;
            result += it->getStatement()->unparse();
        }
    }
    visunparse = result;
    return result;
}

int ControlFlowItem::GetLineNumber()
{
    if (getStatement() == NULL)
        return 0;
    if (getStatement()->lineNumber() == 0){
        if (getOriginalStatement() == NULL)
            return 0;
        return getOriginalStatement()->lineNumber();
    }
    return getStatement()->lineNumber();
}

bool CBasicBlock::IsEmptyBlock()
{
    for (ControlFlowItem* it = start; it != NULL && (it->isLeader() == false || it == start); it = it->getNext()) {        
        if (!it->IsEmptyCFI())
            return false;
    }
    return true;
}

AnalysedCallsList* CallData::GetDataForGraph(ControlFlowGraph* s)
{
    for (AnalysedCallsList* it = calls_list; it != NULL; it = it->next) {
        if (it->graph == s)
            return it;
    }
    return NULL;
}

ControlFlowGraph* GetControlFlowGraphWithCalls(bool main, SgStatement* start, CallData* calls, CommonData* commons)
{
    if (start == NULL) 
    {
        //is_correct = "no body for call found";
        return NULL;
    }

    ControlFlowGraph *cfgRet = NULL;
    /*
#if __SPF
    auto itF = CFG_cache.find(start);
    if (itF != CFG_cache.end())
    {
        calls = std::get<1>(itF->second);
        commons = std::get<2>(itF->second);
        return std::get<0>(itF->second);
    }
#endif*/
    doLoops l;
    ControlFlowItem *funcGraph = getControlFlowList(start, start->lastNodeOfStmt(), NULL, NULL, &l, calls, commons);
    fillLabelJumps(funcGraph);
    setLeaders(funcGraph);

    
    cfgRet = new ControlFlowGraph(false, main, funcGraph, NULL);
    //CFG_cache[start] = std::make_tuple(cfgRet, calls, commons);
    return cfgRet;
}

void FillCFGSets(ControlFlowGraph* graph)
{
    graph->privateAnalyzer();
}

static void ClearMemoryAfterDelay(ActualDelayedData* d)
{
    while (d != NULL) {
        CommonVarSet* cd = d->commons;
        while (cd != NULL) {
            CommonVarSet* t = cd;
            cd = cd->next;
            delete t;
        }
        delete d->buse;
        ActualDelayedData* tmp = d;
        d = d->next;
        delete tmp;
    }
}

static void FillPrivates(ControlFlowGraph* graph)
{
    ActualDelayedData* d = graph->ProcessDelayedPrivates(pCommons, mainProcedure, NULL, NULL, false, -1);
    ClearMemoryAfterDelay(d);
    if (privateDelayedList)
        privateDelayedList->PrintWarnings();
}

ActualDelayedData* CBasicBlock::GetDelayedDataForCall(CallAnalysisLog* log)
{
    for (ControlFlowItem* it = start; it != NULL && (!it->isLeader() || it == start); it = it->getNext())
    {
        AnalysedCallsList* c = it->getCall();
        void* cf = it->getFunctionCall();
        bool isFun = true;
        if (!cf) {
            cf = it->getStatement();
            isFun = false;
        }
        if (c != NULL && c != (AnalysedCallsList*)(-1) && c != (AnalysedCallsList*)(-2) && c->graph != NULL)
            return c->graph->ProcessDelayedPrivates(pCommons, c, log, cf, isFun, it->getProc()->file_id);
    }
    return NULL;
}

void PrivateDelayedItem::MoveFromPrivateToLastPrivate(CVarEntryInfo* var)
{
    VarItem* el = detected->belongs(var);
    if (el) {
        eVariableType storedType = el->var->GetVarType();
        detected->remove(el->var);
        lp->addToSet(var, NULL);
    }
}

void ActualDelayedData::RemoveVarFromCommonList(CommonVarSet* c)
{
    if (commons == NULL || c == NULL)
        return;
    if (c == commons) 
    {
        commons = commons->next;
        delete c;
        return;
    }
    CommonVarSet* prev = c;
    for (CommonVarSet* cur = c->next; cur != NULL; cur = cur->next) 
    {
        if (cur == c) 
        {
            prev->next = c->next;
            delete c;
            return;
        }
        else 
            prev = cur;        
    }
}

void ActualDelayedData::MoveVarFromPrivateToLastPrivate(CVarEntryInfo* var, CommonVarSet* c, VarSet* vs)
{
    original->MoveFromPrivateToLastPrivate(var);
    RemoveVarFromCommonList(c);
    if (vs) 
    {
        if (vs->belongs(var))
            vs->remove(var);
    }
}

int IsThisVariableAParameterOfSubroutine(AnalysedCallsList* lst, SgSymbol* s)
{
    if (!lst->header)
        return -1;
    int stored = SwitchFile(lst->file_id);
    SgProcHedrStmt* h = isSgProcHedrStmt(lst->header);
    if (!h)
        return -1;
    for (int i = 0; i < h->numberOfParameters(); i++) {
        SgSymbol* par = h->parameter(i);
        if (par == s) {
            SwitchFile(stored);
            return i;
        }
    }
    SwitchFile(stored);
    return -1;
}

ActualDelayedData* ControlFlowGraph::ProcessDelayedPrivates(CommonData* commons, AnalysedCallsList* call, CallAnalysisLog* log, void* c, bool isFun, int file_id)
{
    for (CallAnalysisLog* i = log; i != NULL; i = i->prev) {
        if (i->el == call) 
        {
            //TODO: add name of common
#if __SPF && _WIN32
            Warning("Recursion is not analyzed for privates in common blocks '%s'", L"–екурси€ не анализируетс€ дл€ приватных переменных в коммон блоке '%s'",
                    "TODO!", PRIVATE_ANALYSIS_NO_RECURSION_ANALYSIS, call->header);
#else
            Warning("Recursion is not analyzed for privates in common blocks '%s'", "TODO!", PRIVATE_ANALYSIS_NO_RECURSION_ANALYSIS, call->header);
#endif
            return NULL;
        }
    }
    CallAnalysisLog* nl = new CallAnalysisLog();
    nl->el = call;
    nl->prev = log;
    if (log == NULL)
        nl->depth = 0;
    else
        nl->depth = log->depth + 1;
    log = nl;
    ActualDelayedData* my = NULL;
    for (CBasicBlock* bb = first; bb != NULL; bb = bb->getLexNext()) {
        if (bb->containsParloopStart()) {
            if (bb->GetDelayedData()) {
                ActualDelayedData* data = new ActualDelayedData();
                data->original = bb->GetDelayedData();
                data->commons = commons->GetCommonsForVarSet(data->original->getDetected(), call);
                VarSet* bu = new VarSet();
                bu->unite(data->original->getDelayed(), false);
                VarSet* tbu = new VarSet();
                while (!bu->isEmpty()) {
                    if (IS_BY_USE(bu->getFirst()->var->GetSymbol()))
                        tbu->addToSet(bu->getFirst()->var, NULL);
                    else {
                        CVarEntryInfo* old = bu->getFirst()->var;
                        int arg_id = IsThisVariableAParameterOfSubroutine(call, bu->getFirst()->var->GetSymbol());
                        if (arg_id != -1 && c != NULL) {
                            int stored = SwitchFile(file_id);
                            SgExpression* exp = GetProcedureArgument(isFun, c, arg_id);
                            if (isSgVarRefExp(exp) || isSgArrayRefExp(exp)) {
                                SgSymbol* sym = exp->symbol();
                                CVarEntryInfo* v;
                                if (isSgVarRefExp(exp)) {
                                    v = new CScalarVarEntryInfo(sym);
                                }
                                else {
                                    v = old->Clone(sym);
                                }
                                tbu->addToSet(v, NULL, old);
                            }
                            SwitchFile(stored);
                            
                        }
                    }
                    bu->remove(bu->getFirst()->var);
                }
                data->buse = tbu;
                delete bu;
                data->next = my;
                data->call = call;
                my = data;
            }
        }
        ActualDelayedData* calldata = bb->GetDelayedDataForCall(log);
        while (calldata != NULL) {
            CommonVarSet* nxt = NULL;
            for (CommonVarSet* t = calldata->commons; t != NULL; t = nxt) {
                nxt = t->next;
                CommonVarInfo* cvd = t->cvd;
                CommonDataItem* d = commons->IsThisCommonUsedInProcedure(cvd->parent, call);
                if (!d || commons->CanHaveNonScalarVars(d))
                    continue;
                CommonVarInfo* j = cvd->parent->info;
                CommonVarInfo* i = d->info;
                while (j != cvd) {
                    j = j->next;
                    if (i)
                        i = i->next;
                }
                if (!i)
                    continue;
                CVarEntryInfo* var = i->var;
                if (bb->getLexNext()->getLiveIn()->belongs(var->GetSymbol()) && calldata->original->getDelayed()->belongs(cvd->var)) {
                    calldata->MoveVarFromPrivateToLastPrivate(cvd->var, t, NULL);
                }
                if (bb->IsVarDefinedAfterThisBlock(var, false)) {
                    calldata->RemoveVarFromCommonList(t);
                }
                
            }
            if (log->el->header == calldata->call->header) {
                VarSet* pr = new VarSet();
                pr->unite(calldata->original->getDelayed(), false);
                pr->intersect(bb->getLexNext()->getLiveIn(), false, true);
                for (VarItem* exp = pr->getFirst(); exp != NULL; pr->getFirst()) {
                    calldata->MoveVarFromPrivateToLastPrivate(exp->var, NULL, NULL);
                    pr->remove(exp->var);
                }
                delete pr;
            }
            VarSet* tmp_use = new VarSet();
            tmp_use->unite(calldata->buse, false);
            while (!tmp_use->isEmpty()) {
                VarItem* v = tmp_use->getFirst();
                CVarEntryInfo* tmp = v->var->Clone(OriginalSymbol(v->var->GetSymbol()));
                if (bb->getLexNext()->getLiveIn()->belongs(tmp->GetSymbol(), true)) {
                    calldata->MoveVarFromPrivateToLastPrivate(v->ov ? v->ov : v->var, NULL, calldata->buse);
                }
                if (bb->IsVarDefinedAfterThisBlock(v->var, true)) {
                    calldata->buse->remove(v->ov ? v->ov : v->var);
                }
                delete tmp;
                tmp_use->remove(v->var);
            }
            delete tmp_use;
            ActualDelayedData* tmp = calldata->next;
            calldata->next = my;
            my = calldata;
            calldata = tmp;
        }
    }
    nl = log;
    log = log->prev;

    delete nl;
    return my;
}

extern graph_node* node_list;
void Private_Vars_Function_Analyzer(SgStatement* start);

void Private_Vars_Project_Analyzer()
{
    graph_node* node = node_list;
    while (node) {
        if (node->st_header) {
            int stored_file_id = SwitchFile(node->file_id);
            Private_Vars_Function_Analyzer(node->st_header);
            SwitchFile(stored_file_id);
        }
        node = node->next;
    }
}

// CALL function for PRIVATE analyzing
void Private_Vars_Function_Analyzer(SgStatement* start)
{
    //temporary state
#ifndef __SPF 
    if (!options.isOn(PRIVATE_ANALYSIS)){
        return;
    }
#endif

    if (start->variant() == PROG_HEDR) {
        Private_Vars_Analyzer(start);
    }
    /*
    ControlFlowItem* funcGraph = getControlFlowList(start, start->lastNodeOfStmt(), NULL, NULL, new doLoops());
    fillLabelJumps(funcGraph);
    setLeaders(funcGraph);
#if ACCAN_DEBUG
    printControlFlowList(funcGraph);
#endif
    ControlFlowItem* p = funcGraph;
    ControlFlowItem* pl_start = NULL;
    ControlFlowItem* pl_end = NULL;
    ControlFlowGraph* graph = new ControlFlowGraph(funcGraph, NULL);
    graph->privateAnalyzer();
    */
}
/*
// CALL function for PRIVATE analyzing
void Private_Vars_Analyzer(SgStatement *firstSt, SgStatement *lastSt)
{
    // temporary state
    //return;
    SgExpression* par_des = firstSt->expr(2);
    SgSymbol* l;
    SgForStmt* chk;
    int correct = 1;
    firstSt = firstSt->lexNext();
    while (correct && (par_des != NULL) && (par_des->lhs() != NULL) && ((l = par_des->lhs()->symbol()) != NULL)){
        if (firstSt->variant() == FOR_NODE){
            chk = isSgForStmt(firstSt);
            if (chk->symbol() != l)
                correct = 0;
            firstSt = firstSt->lexNext();
            par_des = par_des->rhs();
        }
        else{
            correct = 0;
        }
    }
    if (correct){
        doLoops* loops = new doLoops();
        ControlFlowItem* cfList = getControlFlowList(firstSt, lastSt, NULL, NULL, loops);
        fillLabelJumps(cfList);
        setLeaders(cfList);
#if ACCAN_DEBUG
        printControlFlowList(cfList);
#endif
        VarSet* priv = ControlFlowGraph(cfList, NULL).getPrivate();
#if ACCAN_DEBUG
        priv->print();
#endif
        clearList(cfList);
    }
}
*/

static void fillLabelJumps(ControlFlowItem* cfList)
{
    if (cfList != NULL){
        ControlFlowItem* temp = cfList;
        ControlFlowItem* temp2;
        unsigned int label_no = 0;
        while (temp != NULL){
            if (temp->getLabel() != NULL)
                label_no++;
            temp = temp->getNext();
        }
        LabelCFI* table = new LabelCFI[label_no + 1];
        unsigned int li = 0;
        for (temp = cfList; temp != NULL; temp = temp->getNext()){
            SgLabel* label;
            if ((label = temp->getLabel()) != NULL){
                table[li].item = temp;
                table[li++].l = label->id();
            }
            temp2 = temp;
        }
        temp = new ControlFlowItem(currentProcedure);
        temp2->AddNextItem(temp);
        table[label_no].item = temp2;
        table[label_no].l = -1;
        for (temp = cfList; temp != NULL; temp = temp->getNext()){
            SgLabel* jump = temp->getLabelJump();
            int l;
            if (jump != NULL){
                l = jump->id();
                for (unsigned int i = 0; i < label_no + 1; i++){
                    if (table[i].l == l || i == label_no){
                        temp->initJump(table[i].item);
                        break;
                    }
                }
            }
        }
        delete[] table;
    }
}

static void setLeaders(ControlFlowItem* cfList)
{
    if (cfList != NULL)
        cfList->setLeader();
    while (cfList != NULL)
    {
        if (cfList->getJump() != NULL)
        {
            cfList->getJump()->setLeader();
            if (cfList->getNext() != NULL)
                cfList->getNext()->setLeader();
        }
        if (cfList->getCall() != NULL)
        {
            if (cfList->getNext() != NULL)
                cfList->getNext()->setLeader();
        }
        cfList = cfList->getNext();
    }
}

static void clearList(ControlFlowItem *list)
{
    if (list != NULL)
    {
        if (list->getNext() != NULL)
            clearList(list->getNext());

        delete list;
    }
}

static ControlFlowItem* ifItem(SgStatement* stmt, ControlFlowItem* empty, SgStatement** lastAnStmt, doLoops* loops, bool ins, CallData* calls, CommonData* commons)
{
    if (stmt == NULL)
        return empty;
    SgIfStmt* cond;
    if (stmt->variant() == ELSEIF_NODE)
        cond = (SgIfStmt*)stmt;
    if (stmt->variant() == ELSEIF_NODE || (!ins && (cond = isSgIfStmt(stmt)) != NULL))
    {
        SgExpression* c = &(SgNotOp((cond->conditional()->copy())));
        ControlFlowItem *n, *j;
        ControlFlowItem* last;
        if ((n = getControlFlowList(cond->trueBody(), NULL, &last, lastAnStmt, loops, calls, commons)) == NULL)
            return NULL;
        j = ifItem(cond->falseBody(), empty, lastAnStmt, loops, cond->falseBody() != NULL ? cond->falseBody()->variant() == IF_NODE : false, calls, commons);
        ControlFlowItem* gotoEmpty = new ControlFlowItem(NULL, empty, j, NULL, currentProcedure);
        if (last != NULL)
            last->AddNextItem(gotoEmpty);
        else
            n = gotoEmpty;
        ControlFlowItem* tn = new ControlFlowItem(c, j, n, stmt->label(), currentProcedure);
        tn->setOriginalStatement(stmt);
        return tn;
    }
    else
    {
        ControlFlowItem* last;
        ControlFlowItem* ret;
        if ((ret = getControlFlowList(stmt, NULL, &last, lastAnStmt, loops, calls, commons)) == NULL)
            return NULL;
        last->AddNextItem(empty);
        return ret;
    }
}

static ControlFlowItem* switchItem(SgStatement* stmt, ControlFlowItem* empty, SgStatement** lastAnStmt, doLoops* loops, CallData* calls, CommonData* commons)
{
    SgSwitchStmt* sw = isSgSwitchStmt(stmt);
    SgExpression* sw_cond = (sw->selector());
    stmt = stmt->lexNext();
    *lastAnStmt = stmt;
    ControlFlowItem* last_sw = NULL;
    ControlFlowItem* first = NULL;
    bool is_def_last = false;
    SgStatement* not_def_last;
    while (stmt->variant() == CASE_NODE || stmt->variant() == DEFAULT_NODE)
    {
        if (stmt->variant() == DEFAULT_NODE){
            while (stmt->variant() != CONTROL_END && stmt->variant() != CASE_NODE)
                stmt = stmt->lexNext();
            if (stmt->variant() == CONTROL_END)
                stmt = stmt->lexNext();
            is_def_last = true;
            continue;
        }
        SgExpression* c = ((SgCaseOptionStmt*)stmt)->caseRange(0);
        SgExpression *lhs = NULL;
        SgExpression *rhs = NULL;
        if (c->variant() == DDOT){
            lhs = c->lhs();
            rhs = c->rhs();
            if (rhs == NULL)
                c = &(*lhs <= *sw_cond);
            else if (lhs == NULL)
                c = &(*sw_cond <= *rhs);
            else
                c = &(*lhs <= *sw_cond && *sw_cond <= *rhs);
        }
        else
            c = &SgNeqOp(*sw_cond, *c);
        ControlFlowItem *n, *j;
        ControlFlowItem* last;
        if ((n = getControlFlowList(stmt->lexNext(), NULL, &last, lastAnStmt, loops, calls, commons)) == NULL)
            return NULL;
        j = new ControlFlowItem(currentProcedure);
        ControlFlowItem* gotoEmpty = new ControlFlowItem(NULL, empty, j, NULL, currentProcedure);
        if (last != NULL)
            last->AddNextItem(gotoEmpty);
        else
            n = gotoEmpty;
        ControlFlowItem* cond = new ControlFlowItem(c, j, n, stmt->label(), currentProcedure);
        cond->setOriginalStatement(stmt);
        if (last_sw == NULL)
            first = cond;
        else
            last_sw->AddNextItem(cond);
        last_sw = j;
        is_def_last = false;
        not_def_last = *lastAnStmt;
        stmt = *lastAnStmt;
    }
    SgStatement* def = sw->defOption();
    if (def != NULL){
        ControlFlowItem* last;
        ControlFlowItem* n;
        if ((n = getControlFlowList(def->lexNext(), NULL, &last, lastAnStmt, loops, calls, commons)) == NULL)
            return NULL;
        if (last != NULL)
            last->AddNextItem(empty);
        if (last_sw == NULL)
            first = n;
        else
            last_sw->AddNextItem(n);
        last_sw = last;
    }
    last_sw->AddNextItem(empty);
    if (!is_def_last)
        *lastAnStmt = not_def_last;
    return first;
}

static ControlFlowItem* getControlFlowList(SgStatement *firstSt, SgStatement *lastSt, ControlFlowItem **last, SgStatement **lastAnStmt, doLoops* loops, CallData* calls, CommonData* commons)
{
    ControlFlowItem *list = new ControlFlowItem(currentProcedure);
    ControlFlowItem *cur = list;
    ControlFlowItem *pred = list;
    SgStatement *stmt;
    for (stmt = firstSt; (
        stmt != lastSt
        && stmt->variant() != CONTAINS_STMT
        && (lastSt != NULL || stmt->variant() != ELSEIF_NODE)
        && (lastSt != NULL || stmt->variant() != CASE_NODE)
        && (lastSt != NULL || stmt->variant() != DEFAULT_NODE));
        stmt = stmt->lexNext())
    {
        if (stmt->variant() == CONTROL_END)
        {
            if (isSgExecutableStatement(stmt))
                break;
        }

        cur = processOneStatement(&stmt, &pred, &list, cur, loops, calls, commons);
        if (cur == NULL)
        {
            clearList(list);
            return NULL;
        }
    }
    if (cur == NULL){
        cur = list = new ControlFlowItem(currentProcedure);
    }
    if (last != NULL)
        *last = cur;
    if (lastAnStmt != NULL)
        *lastAnStmt = stmt;
    return list;
}

AnalysedCallsList* CallData::IsHeaderInList(SgStatement* header)
{
    if (header == NULL)
        return NULL;
    AnalysedCallsList* p = calls_list;
    while (p != NULL) {
        if (p->header == header)
            return p;
        p = p->next;
    }
    return NULL;
}

void CallData::AssociateGraphWithHeader(SgStatement* st, ControlFlowGraph* gr)
{
    AnalysedCallsList* l = calls_list;
    while (l != NULL) {
        if (l->header == st) {
            if (gr == l->graph && gr != NULL)
                gr->AddRef();
            l->graph = gr;
            return;
        }
        l = l->next;
    }
    delete gr;
}

AnalysedCallsList* CallData::AddHeader(SgStatement* st, bool isFun, SgSymbol* name, int fid)
{
    //test
    bool add_intr = IsAnIntrinsicSubroutine(name->identifier()) != NULL;
    AnalysedCallsList* l = new AnalysedCallsList(st, (isIntrinsicFunctionNameACC(name->identifier()) || add_intr) && !IsUserFunctionACC(name), IsPureProcedureACC(name), isFun, name->identifier(), fid);
    l->next = calls_list;
    calls_list = l;
    return l;
}

extern int isStatementFunction(SgSymbol *s);

AnalysedCallsList* CallData::getLinkToCall(SgExpression* e, SgStatement* s, CommonData* commons)
{
    SgStatement* header = NULL;
    SgSymbol* name;
    bool isFun;
    graph_node* g = NULL;
    if (e == NULL) {
        //s - procedure call
        SgCallStmt* f = isSgCallStmt(s);
        SgSymbol* fdaf = f->name();
        if (ATTR_NODE(f->name()) != NULL)
            g = GRAPHNODE(f->name());
        if (g == NULL) {
            
            is_correct = "no header for procedure";
            failed_proc_name = f->name()->identifier();
            return (AnalysedCallsList*)(-1);
            
        }
        if (g)
            header = isSgProcHedrStmt(g->st_header);
        name = f->name();
        isFun = false;
        //intr = isIntrinsicFunctionNameACC(f->name()->identifier()) && !IsUserFunctionACC(f->name());
        //IsPureProcedureACC(f->name());
    }
    else {
        //e - function call
        SgFunctionCallExp* f = isSgFunctionCallExp(e);
        if (isStatementFunction(f->funName()))
            return (AnalysedCallsList*)(-2);
        if (ATTR_NODE(f->funName()) != NULL)
            g = GRAPHNODE(f->funName());
        if (g == NULL) {
            is_correct = "no header for function";
            failed_proc_name = f->funName()->identifier();
            return (AnalysedCallsList*)(-1);
        }
        header = isSgFuncHedrStmt(g->st_header);
        name = f->funName();
        isFun = true;
    }
    AnalysedCallsList* p;
    if ((p = IsHeaderInList(header))) {
        recursion_flag = recursion_flag || p->graph != NULL;
        return p;
    }
    AnalysedCallsList* prev = currentProcedure;
    currentProcedure = p = AddHeader(header, isFun, name, g->file_id);
    if (!p->isIntrinsic) {
        int stored = SwitchFile(g->file_id);
        
        ControlFlowGraph* graph = GetControlFlowGraphWithCalls(false, header, this, commons);
        //if (graph == NULL)
            //failed_proc_name = name->identifier();
        
        SwitchFile(stored);
        
        AssociateGraphWithHeader(header, graph);
        commons->MarkEndOfCommon(p);
    }
    currentProcedure = prev;
    return p;
}

static ControlFlowItem* GetFuncCallsForExpr(SgExpression* e, CallData* calls, ControlFlowItem** last, CommonData* commons, SgStatement* os)
{
    if (e == NULL) {
        *last = NULL;
        return NULL;
    }
    SgFunctionCallExp* f = isSgFunctionCallExp(e);
    if (f) {
        ControlFlowItem* head = new ControlFlowItem(NULL, NULL, currentProcedure, calls->getLinkToCall(e, NULL, commons));
        head->setOriginalStatement(os);
        ControlFlowItem* curl = head;
        head->setFunctionCall(f);
        ControlFlowItem* l1, *l2;
        ControlFlowItem* tail1 = GetFuncCallsForExpr(e->lhs(), calls, &l1, commons, os);
        ControlFlowItem* tail2 = GetFuncCallsForExpr(e->rhs(), calls, &l2, commons, os);
        *last = head;
        if (tail2 != NULL) {
            l2->AddNextItem(head);
            head = tail2;
        }
        if (tail1 != NULL) {
            l1->AddNextItem(head);
            head = tail1;
        }

        return head;
    }
    f = isSgFunctionCallExp(e->lhs());
    if (f) {
        ControlFlowItem* head = new ControlFlowItem(NULL, NULL, currentProcedure, calls->getLinkToCall(e->lhs(), NULL, commons));
        head->setOriginalStatement(os);
        head->setFunctionCall(f);
        ControlFlowItem* l1, *l2, *l3;
        ControlFlowItem* tail1 = GetFuncCallsForExpr(e->lhs()->lhs(), calls, &l1, commons, os);
        ControlFlowItem* tail2 = GetFuncCallsForExpr(e->lhs()->rhs(), calls, &l2, commons, os);
        ControlFlowItem* tail3 = GetFuncCallsForExpr(e->rhs(), calls, &l3, commons, os);
        *last = head;
        if (tail2 != NULL) {
            l2->AddNextItem(head);
            head = tail2;
        }
        if (tail1 != NULL) {
            l1->AddNextItem(head);
            head = tail1;
        }
        if (tail3 != NULL) {
            (*last)->AddNextItem(tail3);
            *last = l3;
        }
        return head;
    }
    return GetFuncCallsForExpr(e->rhs(), calls, last, commons, os);
}

static ControlFlowItem* AddFunctionCalls(SgStatement* st, CallData* calls, ControlFlowItem** last, CommonData* commons)
{
    ControlFlowItem* retv = GetFuncCallsForExpr(st->expr(0), calls, last, commons, st);
    ControlFlowItem* l2 = NULL;
    ControlFlowItem* second = GetFuncCallsForExpr(st->expr(1), calls, &l2, commons, st);
    if (retv == NULL) {
        retv = second;
        *last = l2;
    }
    else if (second != NULL) {
        (*last)->AddNextItem(second);
        *last = l2;
    }
    ControlFlowItem* l3 = NULL;
    ControlFlowItem* third = GetFuncCallsForExpr(st->expr(2), calls, &l3, commons, st);
    if (retv == NULL) {
        retv = third;
        *last = l3;
    }
    else if (third != NULL) {
        (*last)->AddNextItem(third);
        *last = l3;
    }
    return retv;
}

void DoLoopDataList::AddLoop(int file_id, SgStatement* st, SgExpression* l, SgExpression* r, SgExpression* step, SgSymbol* lv)
{
    DoLoopDataItem* nt = new DoLoopDataItem();
    nt->file_id = file_id;
    nt->statement = st;
    nt->l = l;
    nt->r = r;
    nt->st = step;
    nt->loop_var = lv;
    nt->next = list;
    list = nt;
}

DoLoopDataList::~DoLoopDataList()
{
#if __SPF
    removeFromCollection(this);
    return;
#endif
    while (list != NULL) {
        DoLoopDataItem* t = list->next;
        delete list;
        list = t;
    }
}

static ControlFlowItem* processOneStatement(SgStatement** stmt, ControlFlowItem** pred, ControlFlowItem **list, ControlFlowItem* oldcur, doLoops* loops, CallData* calls, CommonData* commons)
{
    ControlFlowItem* lastf;
    ControlFlowItem* funcs = AddFunctionCalls(*stmt, calls, &lastf, commons);
    if (funcs != NULL) {
        if (*pred != NULL)
            (*pred)->AddNextItem(funcs);
        else
            *list = funcs;
        *pred = lastf;
    }

    switch ((*stmt)->variant())
    {
        case IF_NODE:
        {
            ControlFlowItem* emptyAfterIf = new ControlFlowItem(currentProcedure); //empty item to avoid second pass
            /*
            if ((*stmt)->hasLabel()){
                ControlFlowItem* emptyBeforeIf = new ControlFlowItem();
                emptyBeforeIf->setLabel((*stmt)->label());
                if (*pred != NULL)
                    (*pred)->AddNextItem(emptyBeforeIf);
                else
                    *list = emptyBeforeIf;
                *pred = emptyBeforeIf;
            }
            */
            ControlFlowItem* cur = ifItem(*stmt, emptyAfterIf, stmt, loops, false, calls, commons);
            emptyAfterIf->setLabel((*stmt)->label());
            if (*pred != NULL)
                (*pred)->AddNextItem(cur);
            else
                *list = cur;
            return (*pred = emptyAfterIf);
        }
        case ASSIGN_STAT:
        case POINTER_ASSIGN_STAT:
        case PROC_STAT:
        case PRINT_STAT:
        case READ_STAT:
        case WRITE_STAT:
        case ALLOCATE_STMT:
        case DEALLOCATE_STMT:
        {
            ControlFlowItem* cur = new ControlFlowItem(*stmt, NULL, currentProcedure, (*stmt)->variant() == PROC_STAT ? calls->getLinkToCall(NULL, *stmt, commons) : NULL);
            if (*pred != NULL)
                (*pred)->AddNextItem(cur);
            else
                *list = cur;
            return (*pred = loops->checkStatementForLoopEnding(cur->getLabel() ? cur->getLabel()->id() : -1, cur));
        }
        case LOGIF_NODE:
        {
            ControlFlowItem* emptyAfterIf = new ControlFlowItem(currentProcedure); //empty item to avoid second pass
            SgLogIfStmt* cond = isSgLogIfStmt(*stmt);
            SgLabel* lbl = (*stmt)->label();
            SgExpression* c = &(SgNotOp((cond->conditional()->copy())));
            ControlFlowItem* cur = new ControlFlowItem(c, emptyAfterIf, NULL, (*stmt)->label(), currentProcedure);
            cur->setOriginalStatement(*stmt);
            if (*pred != NULL)
                (*pred)->AddNextItem(cur);
            else
                *list = cur;
            *stmt = (*stmt)->lexNext();
            ControlFlowItem* body;
            if ((body = processOneStatement(stmt, &cur, list, cur, loops, calls, commons)) == NULL){
                return NULL;
            }
            body->AddNextItem(emptyAfterIf);
            return (*pred = loops->checkStatementForLoopEnding(lbl ? lbl->id() : -1, emptyAfterIf));
        }
        case WHILE_NODE:
        {
            SgWhileStmt* cond = isSgWhileStmt(*stmt);
            bool isEndDo = (*stmt)->lastNodeOfStmt()->variant() == CONTROL_END;
            SgExpression* c;
            if (cond->conditional())
                c = &(SgNotOp((cond->conditional()->copy())));
            else
                c = new SgValueExp(1);
            ControlFlowItem* emptyAfterWhile = new ControlFlowItem(currentProcedure);
            ControlFlowItem* emptyBeforeBody = new ControlFlowItem(currentProcedure);
            ControlFlowItem* cur = new ControlFlowItem(c, emptyAfterWhile, emptyBeforeBody, (*stmt)->label(), currentProcedure);
            cur->setOriginalStatement(cond);
            ControlFlowItem* gotoStart = new ControlFlowItem(NULL, cur, emptyAfterWhile, NULL, currentProcedure);
            ControlFlowItem* emptyBefore = new ControlFlowItem(NULL, (ControlFlowItem*)NULL, cur, cond->label(), currentProcedure);
            SgVarRefExp* doName = (isSgVarRefExp((*stmt)->expr(2)));
            int lbl = -1;
            if (!isEndDo){
                SgStatement* end = lastStmtOfDoACC(cond);
                if (end->controlParent() && end->controlParent()->variant() == LOGIF_NODE)
                    lbl = end->controlParent()->label()->id();
                else
                    lbl = end->label()->id();
            }
            loops->addLoop(lbl, doName ? doName->symbol() : NULL, gotoStart, emptyAfterWhile);
            ControlFlowItem* n, *last;
            if (isEndDo){
                if ((n = getControlFlowList((*stmt)->lexNext(), NULL, &last, stmt, loops, calls, commons)) == NULL)
                    return NULL;
                emptyBeforeBody->AddNextItem(n);
                loops->endLoop(last);
            }
            if (*pred != NULL)
                (*pred)->AddNextItem(emptyBefore);
            else
                *list = emptyBefore;
            if (isEndDo)
                return (*pred = emptyAfterWhile);
            return (*pred = emptyBeforeBody);
        }
        case FOR_NODE:
        {
            SgForStmt* fst = isSgForStmt(*stmt);
#if __SPF
            SgStatement *p = NULL;
            for (int i = 0; i < fst->numberOfAttributes(); ++i)
            {
                if (fst->attributeType(i) == SPF_ANALYSIS_DIR)
                {
                    p = (SgStatement *)(fst->getAttribute(i)->getAttributeData());
                    break;
                }
            }
            bool isParLoop = (p && p->variant() == SPF_ANALYSIS_DIR);
#else
            SgStatement* p = (*stmt)->lexPrev();
            bool isParLoop = (p && p->variant() == DVM_PARALLEL_ON_DIR);
#endif
            SgExpression* pl = NULL;
            SgExpression* pPl = NULL;
            bool pl_flag = true;
            if (isParLoop){
#if __SPF
                SgExpression* el = p->expr(0);
#else
                SgExpression* el = p->expr(1);
#endif
                pPl = el;
                while (el != NULL) {
                    SgExpression* e = el->lhs();
                    if (e->variant() == ACC_PRIVATE_OP) {
                        pl = e;
                        break;
                    }
                    pPl = el;
                    pl_flag = false;
                    el = el->rhs();
                }
                //pl->unparsestdout();
            }
            bool isEndDo = fst->isEnddoLoop();
            SgExpression* lh = new SgVarRefExp(fst->symbol());
            SgStatement* fa = new SgAssignStmt(*lh, *fst->start());
            bool needs_goto = true;
#if !__SPF
            // create goto edge if can not calculate count of loop's iterations
            if (fst->start()->variant() == INT_VAL && fst->end()->variant() == INT_VAL && fst->start()->valueInteger() < fst->end()->valueInteger())
                needs_goto = false;
#endif
            //fa->setLabel(*(*stmt)->label());
            ControlFlowItem* last;
            ControlFlowItem* emptyAfterDo = new ControlFlowItem(currentProcedure);
            ControlFlowItem* emptyBeforeDo = new ControlFlowItem(currentProcedure);
            ControlFlowItem* gotoEndInitial = NULL;
            if (needs_goto) {
                SgExpression* sendc = new SgExpression(GT_OP, new SgVarRefExp(fst->symbol()), fst->end(), NULL);
                gotoEndInitial = new ControlFlowItem(sendc, emptyAfterDo, emptyBeforeDo, NULL, currentProcedure, true);
                gotoEndInitial->setOriginalStatement(fst);
            }
            ControlFlowItem* stcf = new ControlFlowItem(fa, needs_goto ? gotoEndInitial : emptyBeforeDo, currentProcedure);
            stcf->setOriginalStatement(fst);
            stcf->setLabel((*stmt)->label());
            SgExpression* rh = new SgExpression(ADD_OP, new SgVarRefExp(fst->symbol()), new SgValueExp(1), NULL);
            SgStatement* add = new SgAssignStmt(*lh, *rh);
            SgExpression* endc = new SgExpression(GT_OP, new SgVarRefExp(fst->symbol()), fst->end(), NULL);
            ControlFlowItem* gotoStart = new ControlFlowItem(NULL, emptyBeforeDo, emptyAfterDo, NULL, currentProcedure);
            ControlFlowItem* gotoEnd = new ControlFlowItem(endc, emptyAfterDo, gotoStart, NULL, currentProcedure);
            gotoEnd->setOriginalStatement(fst);
            if (needs_goto) {
                gotoEnd->SetConditionFriend(gotoEndInitial);
            }
            ControlFlowItem* loop_d = new ControlFlowItem(add, gotoEnd, currentProcedure);
            loop_d->setOriginalStatement(fst);
            ControlFlowItem* loop_emp = new ControlFlowItem(NULL, loop_d, currentProcedure);
            SgVarRefExp* doName = (isSgVarRefExp((*stmt)->expr(2)));
            int lbl = -1;
            if (!isEndDo){
                SgStatement* end = lastStmtOfDoACC(fst);
                if (end->variant() == LOGIF_NODE)
                    lbl = end->controlParent()->label()->id();
                else
                    lbl = end->label()->id();
            }
            loops->addLoop(lbl, doName ? doName->symbol() : NULL, loop_emp, emptyAfterDo);
            doLoopList->AddLoop(current_file_id, *stmt, fst->start(), fst->end(), fst->step(), fst->symbol());
            if (isParLoop) {
#if __SPF
                // all loop has depth == 1 ? is it correct?
                int k = 1;
#else
                SgExpression* par_des = p->expr(2);
                int k = 0;
                while (par_des != NULL && par_des->lhs() != NULL) {
                    k++;
                    par_des = par_des->rhs();
                }
#endif
                loops->setParallelDepth(k, pl, p, pPl, pl_flag);
            }

            if (loops->isLastParallel()) {
                SgExpression* ex = loops->getPrivateList();
                emptyBeforeDo->MakeParloopStart();
                bool f;
                SgExpression* e = loops->getExpressionToModifyPrivateList(&f);
                emptyBeforeDo->setPrivateList(ex, loops->GetParallelStatement(), e, f);
                loop_d->MakeParloopEnd();
            }
            if (isEndDo){
                ControlFlowItem* body;
                if ((body = getControlFlowList(fst->body(), NULL, &last, stmt, loops, calls, commons)) == NULL)
                    return NULL;
                emptyBeforeDo->AddNextItem(body);
                loops->endLoop(last);
            }
            if (*pred != NULL)
                (*pred)->AddNextItem(stcf);
            else
                *list = stcf;
            if (isEndDo)
                return (*pred = emptyAfterDo);
            return (*pred = emptyBeforeDo);
        }
        case GOTO_NODE:
        {
            SgGotoStmt* gst = isSgGotoStmt(*stmt);
            ControlFlowItem* gt = new ControlFlowItem(NULL, gst->branchLabel(), NULL, gst->label(), currentProcedure);
            if (*pred != NULL)
                (*pred)->AddNextItem(gt);
            else
                *list = gt;
            return (*pred = gt);
        }
        case ARITHIF_NODE:
        {
            SgArithIfStmt* arif = (SgArithIfStmt*)(*stmt);
            ControlFlowItem* gt3 = new ControlFlowItem(NULL, ((SgLabelRefExp*)(*stmt)->expr(1)->rhs()->rhs()->lhs())->label(), NULL, NULL, currentProcedure);
            ControlFlowItem* gt2 = new ControlFlowItem(&SgEqOp(*(arif->conditional()), *new SgValueExp(0)), ((SgLabelRefExp*)(*stmt)->expr(1)->rhs()->lhs())->label(), gt3, NULL, currentProcedure);
            gt2->setOriginalStatement(arif);
            ControlFlowItem* gt1 = new ControlFlowItem(&(*arif->conditional() < *new SgValueExp(0)), ((SgLabelRefExp*)(*stmt)->expr(1)->lhs())->label(), gt2, (*stmt)->label(), currentProcedure);
            gt1->setOriginalStatement(arif);
            if (*pred != NULL)
                (*pred)->AddNextItem(gt1);
            else
                *list = gt1;
            return (*pred = gt3);
        }
        case COMGOTO_NODE:
        {
            SgComputedGotoStmt* cgt = (SgComputedGotoStmt*)(*stmt);
            SgExpression* label = cgt->labelList();
            int i = 0;
            SgLabel* lbl = ((SgLabelRefExp *)(label->lhs()))->label();
            ControlFlowItem* gt = new ControlFlowItem(&SgEqOp(*(cgt->exp()), *new SgValueExp(++i)), lbl, NULL, cgt->label(), currentProcedure);
            gt->setOriginalStatement(cgt);
            if (*pred != NULL)
                (*pred)->AddNextItem(gt);
            else
                *list = gt;
            ControlFlowItem* old = gt;
            while ((label = label->rhs()))
            {
                lbl = ((SgLabelRefExp *)(label->lhs()))->label();
                gt = new ControlFlowItem(&SgEqOp(*(cgt->exp()), *new SgValueExp(++i)), lbl, NULL, NULL, currentProcedure);
                gt->setOriginalStatement(cgt);
                old->AddNextItem(gt);
                old = gt;
            }
            return (*pred = gt);
        }
        case SWITCH_NODE:
        {
             ControlFlowItem* emptyAfterSwitch = new ControlFlowItem(currentProcedure);
             ControlFlowItem* cur = switchItem(*stmt, emptyAfterSwitch, stmt, loops, calls, commons);
             emptyAfterSwitch->setLabel((*stmt)->label());
             if (*pred != NULL)
                 (*pred)->AddNextItem(cur);
             else
                 *list = cur;
             return (*pred = emptyAfterSwitch);
        }
        case CONT_STAT:
        {
             ControlFlowItem* cur = new ControlFlowItem(NULL, (ControlFlowItem*)NULL, NULL, (*stmt)->label(), currentProcedure);
             if (*pred != NULL)
                 (*pred)->AddNextItem(cur);
             else
                 *list = cur;
             return (*pred = loops->checkStatementForLoopEnding(cur->getLabel() ? cur->getLabel()->id() : -1, cur));
        }
        case CYCLE_STMT:
        {
             SgSymbol* ref = (*stmt)->symbol();
             ControlFlowItem* cur = new ControlFlowItem(NULL, loops->getSourceForCycle(ref), NULL, (*stmt)->label(), currentProcedure);
             if (*pred != NULL)
                 (*pred)->AddNextItem(cur);
             else
                 *list = cur;
             return (*pred = cur);
        }
        case EXIT_STMT:
        {
             SgSymbol* ref = (*stmt)->symbol();
             ControlFlowItem* cur = new ControlFlowItem(NULL, loops->getSourceForExit(ref), NULL, (*stmt)->label(), currentProcedure);
             if (*pred != NULL)
                 (*pred)->AddNextItem(cur);
             else
                 *list = cur;
             return (*pred = cur);
        }
        case COMMENT_STAT:
            return *pred;
        case COMM_STAT:
        {
            commons->RegisterCommonBlock(*stmt, currentProcedure);
            return *pred;
        }
        default:
            return *pred;
            //return NULL;
    }
}

ControlFlowGraph::ControlFlowGraph(bool t, bool m, ControlFlowItem* list, ControlFlowItem* end) : temp(t), main(m), refs(1), def(NULL), use(NULL), pri(NULL), common_def(NULL), common_use(NULL), hasBeenAnalyzed(false)
#ifdef __SPF
, pointers(set<SymbolKey>())
#endif
{
#if __SPF
    addToCollection(__LINE__, __FILE__, this, 30);
#endif
    int n = 0;
    ControlFlowItem* orig = list;
    CBasicBlock* prev = NULL;
    CBasicBlock* start = NULL;
    int stmtNo = 0;
    bool ns = list->isEnumerated();
    if (list != NULL && !ns){
        while (list != NULL && list != end)
        {
            list->setStmtNo(++stmtNo);
            list = list->getNext();
        }
    }
    ControlFlowItem* last_prev = NULL;
    list = orig;
    while (list != NULL && list != end)
    {
        CBasicBlock* bb = new CBasicBlock(t, list, ++n, this, list->getProc());
        last = bb;
        bb->setPrev(prev);
        if (prev != NULL){
            prev->setNext(bb);
            if (!last_prev->isUnconditionalJump()){
                bb->addToPrev(prev, last_prev->IsForJumpFlagSet(), false, last_prev);
                prev->addToSucc(bb, last_prev->IsForJumpFlagSet(), false, last_prev);
            }
        }
        if (start == NULL)
            start = bb;
        prev = bb;
        while (list->getNext() != NULL && list->getNext() != end && !list->getNext()->isLeader()){
            list->setBBno(n);
            list = list->getNext();
        }
        list->setBBno(n);
        last_prev = list;
        list = list->getNext();
    }
    list = orig;
    while (list != NULL && list != end)
    {
        ControlFlowItem* target;
        if ((target = list->getJump()) != NULL)
        {
//            //no back edges
//            if (target->getBBno() > list->getBBno())
//            { 
                CBasicBlock* tmp1 = start;
                CBasicBlock* tmp2 = start;
                for (int i = 1; i < target->getBBno() || i < list->getBBno(); i++)
                {
                    if (i < list->getBBno()) {
                        tmp2 = tmp2->getLexNext();
                        if (!tmp2)
                            break;
                    }
                    if (i < target->getBBno()) {
                        tmp1 = tmp1->getLexNext();
                        if (!tmp1)
                            break;
                    }
                }
                if (tmp1 && tmp2) {
                    tmp1->addToPrev(tmp2, list->IsForJumpFlagSet(), true, list);
                    tmp2->addToSucc(tmp1, list->IsForJumpFlagSet(), true, list);
                }
//            }
        }
        list = list->getNext();
    }
    start->markAsReached();
    first = start;
    common_use = NULL;
    cuf = false;
    common_def = NULL;
    cdf = false;
}

CommonDataItem* CommonData::IsThisCommonVar(VarItem* item, AnalysedCallsList* call)
{
    for (CommonDataItem* it = list; it != NULL; it = it->next) {
        if (it->proc == call) {
            for (CommonVarInfo* inf = it->info; inf != NULL; inf = inf->next) {
                if (inf->var && item->var && *inf->var == *item->var)
                    return it;
            }
        }
    }
    return NULL;
}

CommonDataItem* CommonData::GetItemForName(const string &name, AnalysedCallsList *call)
{
    for (CommonDataItem* it = list; it != NULL; it = it->next) {
        if (it->name == name && it->proc == call)
            return it;
    }
    return NULL;
}

void CommonData::RegisterCommonBlock(SgStatement *st, AnalysedCallsList *cur)
{
    //TODO: multiple common blocks in one procedure with same name
    for (SgExpression *common = st->expr(0); common; common = common->rhs())
    {
        bool newBlock = false;
        SgExprListExp* vars = (SgExprListExp*)common->lhs();
        if (vars == NULL)
            continue;

        const string currCommonName = (common->symbol()) ? common->symbol()->identifier() : "spf_unnamed";

        CommonDataItem* it = GetItemForName(currCommonName, cur);
        if (!it) {
            it = new CommonDataItem();
            it->cb = st;
            it->name = currCommonName;
            it->isUsable = true;
            it->proc = cur;
            it->first = cur;
            it->onlyScalars = true;
            newBlock = true;

            for (CommonDataItem *i = list; i != NULL; i = i->next)
                if (i->name == currCommonName && i->isUsable)
                    it->first = i->first;
        }
        it->commonRefs.push_back(common);

        for (int i = 0; i < vars->length(); ++i)
        {
            SgVarRefExp *e = isSgVarRefExp(vars->elem(i));
            if (e && !IS_ARRAY(e->symbol()))
            {
                CommonVarInfo* c = new CommonVarInfo();
                c->var = new CScalarVarEntryInfo(e->symbol());
                c->isPendingLastPrivate = false;
                c->isInUse = false;
                c->parent = it;
                c->next = it->info;
                it->info = c;
            }
            else if (isSgArrayRefExp(vars->elem(i))) {
                it->onlyScalars = false;
            }
            else {
                CommonVarInfo* c = new CommonVarInfo();
                c->var = new CArrayVarEntryInfo(vars->elem(i)->symbol(), isSgArrayRefExp(vars->elem(i)));
                c->isPendingLastPrivate = false;
                c->isInUse = false;
                c->parent = it;
                c->next = it->info;
                it->info = c;
                it->onlyScalars = false;
            }
        }

        if (newBlock)
        {
            it->next = list;
            list = it;
        }
    }
}

void CommonData::MarkEndOfCommon(AnalysedCallsList* cur)
{
    for (CommonDataItem* i = list; i != NULL; i = i->next)
    {
        if (i->first == cur)
            i->isUsable = false;
    }
}

void CBasicBlock::markAsReached()
{
    prev_status = 1;
    BasicBlockItem* s = succ;
    while (s != NULL){
        CBasicBlock* b = s->block;
        if (b->prev_status == -1)
            b->markAsReached();
        s = s->next;
    }
}

bool ControlFlowGraph::ProcessOneParallelLoop(ControlFlowItem* lstart, CBasicBlock* of, CBasicBlock*& p, bool first)
{
    int stored_fid = SwitchFile(lstart->getProc()->file_id);
    ControlFlowItem* lend;
    if (is_correct != NULL)
    {
        const char* expanded_log;
        char* tmp = NULL;
        if (failed_proc_name)
        {
            tmp = new char[strlen(is_correct) + 2 + strlen(failed_proc_name) + 1];
            strcpy(tmp, is_correct);
            strcat(tmp, ": ");
            strcat(tmp, failed_proc_name);
            expanded_log = tmp;
        }
        else
            expanded_log = is_correct;
#if __SPF && _WIN32
        Warning("Private analysis is not conducted for loop: '%s'", L"јнализ приватных переменных невозможен дл€ данного цикла из-за: '%s'", 
                 expanded_log ? expanded_log : "", PRIVATE_ANALYSIS_NOT_CONDUCTED, lstart->getPrivateListStatement());
#else
        Warning("Private analysis is not conducted for loop: '%s'", expanded_log ? expanded_log : "", PRIVATE_ANALYSIS_NOT_CONDUCTED, lstart->getPrivateListStatement());
#endif
        if (tmp)
            delete[] tmp;

    }
    else
    {
        while ((lend = p->containsParloopEnd()) == NULL)
        {
            p->PrivateAnalysisForAllCalls();
            p = p->getLexNext();
            ControlFlowItem* mstart;
            if ((mstart = p->containsParloopStart()) != NULL)
            {
                CBasicBlock* mp = p;
                if (first) {
                    if (!ProcessOneParallelLoop(mstart, of, mp, false)) {
                        SwitchFile(stored_fid);
                        return false;
                    }
                }
            }
        }
        CBasicBlock* afterParLoop = p->getLexNext()->getLexNext();
        VarSet* l_pri = ControlFlowGraph(true, false, lstart, lend).getPrivate();
        if (is_correct != NULL)
        {
            const char* expanded_log;
            char* tmp = NULL;
            if (failed_proc_name)
            {
                tmp = new char[strlen(is_correct) + 2 + strlen(failed_proc_name) + 1];
                strcpy(tmp, is_correct);
                strcat(tmp, ": ");
                strcat(tmp, failed_proc_name);
                expanded_log = tmp;
            }
            else
                expanded_log = is_correct;

#if __SPF && _WIN32
            Warning("Private analysis is not conducted for loop: '%s'", L"јнализ приватных переменных невозможен дл€ данного цикла из-за: '%s'", 
                     expanded_log ? expanded_log : "", PRIVATE_ANALYSIS_NOT_CONDUCTED, lstart->getPrivateListStatement());
#else
            Warning("Private analysis is not conducted for loop: '%s'", expanded_log ? expanded_log : "", PRIVATE_ANALYSIS_NOT_CONDUCTED, lstart->getPrivateListStatement());
#endif
            if (tmp)
                delete[] tmp;
            SwitchFile(stored_fid);
            return false;
        }
        VarSet* p_pri = new VarSet();
        SgExpression* ex_p = lstart->getPrivateList();
        if (ex_p != NULL)
            ex_p = ex_p->lhs();
        for (; ex_p != NULL; ex_p = ex_p->rhs())
        {
            SgVarRefExp* pr;
            if (pr = isSgVarRefExp(ex_p->lhs()))
            {
                CScalarVarEntryInfo* tmp = new CScalarVarEntryInfo(pr->symbol());
                p_pri->addToSet(tmp, NULL);
                delete tmp;
            }
            SgArrayRefExp* ar;
            if (ar = isSgArrayRefExp(ex_p->lhs()))
            {
                CArrayVarEntryInfo* tmp = new CArrayVarEntryInfo(ar->symbol(), ar);
                p_pri->addToSet(tmp, NULL);
                delete tmp;
            }
        }

        VarSet* live = afterParLoop->getLiveIn();
        VarSet* adef = afterParLoop->getDef();
        VarSet* pri = new VarSet();
        VarSet* tmp = new VarSet();
        VarSet* delay = new VarSet();
        tmp->unite(l_pri, false);

        for (VarItem* exp = tmp->getFirst(); exp != NULL; exp = tmp->getFirst())
        {
            if (!afterParLoop->IsVarDefinedAfterThisBlock(exp->var, false))
                delay->addToSet(exp->var, NULL);
            tmp->remove(exp->var);
        }
        delete tmp;
        pri->unite(l_pri, false);
        pri->minus(live, true);
        privateDelayedList = new PrivateDelayedItem(pri, p_pri, l_pri, lstart, privateDelayedList, this, delay, current_file_id);
        of->SetDelayedData(privateDelayedList);
    }
    SwitchFile(stored_fid);
    return true;
}

void ControlFlowGraph::privateAnalyzer()
{
    if (hasBeenAnalyzed)
        return;
    CBasicBlock* p = first;
    /*
    printf("GRAPH:\n");
    while (p != NULL){
        printf("block %d: ", p->getNum());
        if (p->containsParloopStart())
            printf("start");
        if (p->containsParloopEnd())
            printf("end");
        p->print();
        p = p->getLexNext();
    }
    */
    p = first;
    liveAnalysis();
    while (1) 
    {
        ControlFlowItem* lstart;
        CBasicBlock* of = p;
        p->PrivateAnalysisForAllCalls();
        if ((lstart = p->containsParloopStart()) != NULL) 
        {
            if (!ProcessOneParallelLoop(lstart, of, p, true))
                break;
        }
        if (p == last)
            break;
        p = p->getLexNext();
    }
    hasBeenAnalyzed = true;
}

/*#ifdef __SPF
void PrivateDelayedItem::PrintWarnings()
{
    if (next)
        next->PrintWarnings();
    lp->minus(detected);
    while (!detected->isEmpty()) {
        SgVarRefExp* var = detected->getFirst();
        detected->remove(var);
        Warning("Variable '%s' detected as private", var->unparse(), PRIVATE_ANALYSIS_ADD_VAR, lstart->getPrivateListStatement());
    }
    while (!lp->isEmpty()) {
        SgVarRefExp* var = lp->getFirst();
        lp->remove(var);
        Warning("Variable '%s' detected as last private", var->unparse(), PRIVATE_ANALYSIS_ADD_VAR, lstart->getPrivateListStatement());
    }
    if (detected)
        delete detected;
    if (original)
        delete original;
    if (lp)
        delete lp;
}
#else*/

bool CArrayVarEntryInfo::HasActiveElements() const
{
    bool result = false;
    if (disabled)
        return false;
    if (subscripts == 0)
        return true;
    for (int i = 0; i < subscripts; i++) 
    {
        if (!data[i].defined)
            return false;
        if (data[i].left_bound != data[i].right_bound)
            result = true;
        if (data[i].left_bound == data[i].right_bound && data[i].bound_modifiers[0] <= data[i].bound_modifiers[1])
            result = true;
    }
    return result;
}

void CArrayVarEntryInfo::MakeInactive()
{
    disabled = true;
    for (int i = 0; i < subscripts; i++) 
    {
        data[i].left_bound = data[i].right_bound = NULL;
        data[i].bound_modifiers[0] = data[i].bound_modifiers[1] = 0;
    }
}

void PrivateDelayedItem::PrintWarnings()
{
    if (next)
        next->PrintWarnings();
    int stored_fid = SwitchFile(file_id);
    total_privates += detected->count();
    total_pl++;
    lp->minus(detected);
    detected->LeaveOnlyRecords();
    detected->RemoveDoubtfulCommonVars(lstart->getProc());
    VarSet* test1 = new VarSet();
    test1->unite(detected, false);
    VarSet* test2 = new VarSet();
    test2->unite(original, false);
    test2->minus(detected);
    test1->minus(original);
    int extra = 0, missing = 0;
    SgExpression* prl = lstart->getPrivateList();
    SgStatement* prs = lstart->getPrivateListStatement();
    if (prl == NULL && !test1->isEmpty()) 
    {
        SgExpression* lst = new SgExprListExp();
        prl = new SgExpression(ACC_PRIVATE_OP);
        lst->setLhs(prl);
        lst->setRhs(NULL);
#if __SPF
        SgExpression* clauses = prs->expr(0);
#else
        SgExpression* clauses = prs->expr(1);
#endif
        if (clauses) {
            while (clauses->rhs() != NULL)
                clauses = clauses->rhs();
            clauses->setRhs(lst);
        }
        else {
#if __SPF
            prs->setExpression(0, *lst);
#else
            prs->setExpression(1, *lst);
#endif
        }
    }
    SgExpression* op = prl;

    while (!test2->isEmpty()) {
        //printf("EXTRA IN PRIVATE LIST: ");
        //test2->print();
        extra = 1;
        VarItem* var = test2->getFirst();
        CVarEntryInfo* syb = var->var->Clone();
        int change_fid = var->file_id;
        test2->remove(var->var);
        int stored_fid = SwitchFile(change_fid);
        if (syb->GetVarType() != VAR_REF_ARRAY_EXP)
        {
#if __SPF && _WIN32
            Warning("var '%s' from private list wasn't classified as private", L"ѕеременна€ '%s' из списка приватных переменных не может быть классифицирована как приватна€",
                     syb->GetSymbol()->identifier(), PRIVATE_ANALYSIS_REMOVE_VAR, lstart->getPrivateListStatement());
#else
            Warning("var '%s' from private list wasn't classified as private", syb->GetSymbol()->identifier(), PRIVATE_ANALYSIS_REMOVE_VAR, lstart->getPrivateListStatement());
#endif
        }
        else 
        {
            CArrayVarEntryInfo* tt = (CArrayVarEntryInfo*)syb;
            if (tt->HasActiveElements())
            {
#if __SPF && _WIN32
                Warning("array '%s' from private list wasn't classified as private", L"ћассив '%s' из списка приватных переменных не может быть классифицирована как приватный",
                         syb->GetSymbol()->identifier(), PRIVATE_ANALYSIS_REMOVE_VAR, lstart->getPrivateListStatement());
#else
                Warning("array '%s' from private list wasn't classified as private", syb->GetSymbol()->identifier(), PRIVATE_ANALYSIS_REMOVE_VAR, lstart->getPrivateListStatement());
#endif
            }
        }
        delete(syb);
        SwitchFile(stored_fid);
    }
    while (!test1->isEmpty()) {
        //printf("MISSING IN PRIVATE LIST: ");
        //test1->print();
        missing = 1;
        VarItem* var = test1->getFirst();
        CVarEntryInfo* syb = var->var->Clone();
        int change_fid = var->file_id;
        test1->remove(var->var);
        int stored_fid = SwitchFile(change_fid);
        if (syb->GetVarType() != VAR_REF_ARRAY_EXP) {
#if __SPF && _WIN32
            Note("add private scalar '%s'", L"ƒобавлена приватна€ переменна€ '%s'", syb->GetSymbol()->identifier(), PRIVATE_ANALYSIS_ADD_VAR, lstart->getPrivateListStatement());
#else
            Warning("var '%s' was added to private list", syb->GetSymbol()->identifier(), PRIVATE_ANALYSIS_ADD_VAR, lstart->getPrivateListStatement());
#endif
            SgExprListExp* nls = new SgExprListExp();
            SgVarRefExp* nvr = new SgVarRefExp(syb->GetSymbol());
            nls->setLhs(nvr);
            nls->setRhs(prl->lhs());
            prl->setLhs(nls);
        }
        else 
        {
            CArrayVarEntryInfo* tt = (CArrayVarEntryInfo*)syb;
            if (tt->HasActiveElements()) 
            {
#if __SPF && _WIN32
                Note("add private array '%s'", L"ƒобавлена приватна€ переменна€ по массиву '%s'", syb->GetSymbol()->identifier(), PRIVATE_ANALYSIS_ADD_VAR, lstart->getPrivateListStatement());
#else
                Warning("var '%s' was added to private list", syb->GetSymbol()->identifier(), PRIVATE_ANALYSIS_ADD_VAR, lstart->getPrivateListStatement());
#endif

// TODO: need to check all situation before commit it to release 
#if !__SPF
                SgExprListExp *nls = new SgExprListExp();
                SgArrayRefExp *nvr = new SgArrayRefExp(*syb->GetSymbol());
                nls->setLhs(nvr);
                nls->setRhs(prl->lhs());
                prl->setLhs(nls);
#endif
            }
        }
        delete(syb);
        SwitchFile(stored_fid);
        
        /*printf("modified parallel stmt:\n");
        prs->unparsestdout();
        printf("\n");*/
    }
    if (extra == 0 && missing == 0) {
#if ACCAN_DEBUG
        Warning("Correct", "", 0, lstart->getPrivateListStatement());
#endif
    }
    //printf("PRIVATE VARS: ");
    //detected->print();
    //printf("DECLARATION: ");
    //p_pri->print();
    //printf("LAST PRIVATE VARS: ");
    //lp->print();
    if (test1)
        delete test1;
    

    if (test2)
        delete test2;
    
    if (detected)
        delete detected;
    
    if (original)
        delete original;
    
    if (lp)
        delete lp;
    
    SwitchFile(stored_fid);
}
//#endif

ControlFlowItem* doLoops::checkStatementForLoopEnding(int label, ControlFlowItem* last)
{

    if (current == NULL || label == -1 || label != current->getLabel())
        return last;
    return checkStatementForLoopEnding(label, endLoop(last));
}

doLoopItem* doLoops::findLoop(SgSymbol* s)
{
    doLoopItem* l = first;
    while (l != NULL){
        if (l->getName() == s)
            return l;
        l = l->getNext();
    }
    return NULL;
}

void doLoops::addLoop(int l, SgSymbol* s, ControlFlowItem* i, ControlFlowItem* e)
{
    doLoopItem* nl = new doLoopItem(l, s, i, e);
    if (first == NULL)
        first = current = nl;
    else{
        current->setNext(nl);
        nl->HandleNewItem(current);
        current = nl;
    }
}

ControlFlowItem* doLoops::endLoop(ControlFlowItem* last)
{
    doLoopItem* removed = current;
    if (first == current)
        first = current = NULL;
    else{
        doLoopItem* prev = first;
        while (prev->getNext() != current)
            prev = prev->getNext();
        prev->setNext(NULL);
        current = prev;
    }
    last->AddNextItem(removed->getSourceForCycle());
    ControlFlowItem* empty = removed->getSourceForExit();
    delete removed;
    return empty;
}

VarSet* ControlFlowGraph::getPrivate()
{
    //printControlFlowList(first->getStart(), last->getStart());
    if (pri == NULL)
    {
        bool same = false;
        int it = 0;
        CBasicBlock* p = first;
        /*
        printf("GRAPH:\n");
        while (p != NULL){
            printf("block %d: ", p->getNum());
            p->print();
            p = p->getLexNext();
        }
        */
        p = first;
        while (!same){
            p = first;
            same = true;
            while (p != NULL){
                same = p->stepMrdIn(false) && same;
                same = p->stepMrdOut(false) && same;
                p = p->getLexNext();
            }
            it++;
            //printf("iters: %d\n", it);
        }
        p = first;
        while (p != NULL) {
            p->stepMrdIn(true);
            p->stepMrdOut(true);
            //p->getMrdIn(false)->print();
            p = p->getLexNext();
        }
        
        p = first;
        VarSet* res = new VarSet();
        VarSet* loc = new VarSet();
        bool il = false;
        while (p != NULL)
        {
            res->unite(p->getUse(), false);
            loc->unite(p->getDef(), false);
            p = p->getLexNext();
        }
        //printf("USE: ");
        //res->print();
        //printf("LOC: ");
        //loc->print();
        res->unite(loc, false);
        //printf("GETUSE: ");
        //getUse()->print();

        //res->minus(getUse()); //test!
        res->minusFinalize(getUse(), true);
        pri = res;
    }
    return pri;
}

void ControlFlowGraph::liveAnalysis()
{
    bool same = false;
    int it = 0;
    CBasicBlock* p = first;
    p = first;
    while (!same){
        p = last;
        same = true;
        while (p != NULL){
            same = p->stepLVOut() && same;
            same = p->stepLVIn() && same;
            p = p->getLexPrev();
        }
        it++;
        //printf("iters: %d\n", it);
    }
}

VarSet* ControlFlowGraph::getUse()
{
    if (use == NULL)
    {
        CBasicBlock* p = first;
        VarSet* res = new VarSet();
        while (p != NULL)
        {
            VarSet* tmp = new VarSet();
            tmp->unite(p->getUse(), false);
            tmp->minus(p->getMrdIn(false));
            //printf("BLOCK %d INSTR %d USE: ", p->getNum(), p->getStart()->getStmtNo());
            //tmp->print();
            res->unite(tmp, false);
            delete tmp;
            p = p->getLexNext();
        }
        use = res;

    }
    if (!cuf)
    {
        AnalysedCallsList* call = first->getStart()->getProc();
        cuf = true;
        if (call) {
            CommonVarSet* s = pCommons->GetCommonsForVarSet(use, call);
            common_use = s;
            for (CBasicBlock* i = first; i != NULL; i = i->getLexNext()){
                for (CommonVarSet* c = i->getCommonUse(); c != NULL; c = c->next) {
                    /*
                    CommonVarSet* n = new CommonVarSet();
                    n->cvd = c->cvd;
                    n->cvd->refs++;
                    */
                    CommonVarSet* n = new CommonVarSet(*c);
                    CommonVarSet* t;
                    for (t = n; t->next != NULL; t = t->next);
                    t->next = common_use;
                    common_use = n;
                }
            }
        }
    }
    return use;
}

VarSet* ControlFlowGraph::getDef()
{
    if (def == NULL) {
        def = new VarSet();
        def->unite(last->getMrdOut(false), true);
    }
    if (!cdf)
    {
        AnalysedCallsList* call = first->getStart()->getProc();
        if (call) {
            cdf = true;
            CommonVarSet* s = pCommons->GetCommonsForVarSet(def, call);
            common_def = s;
            for (CBasicBlock* i = first; i != NULL; i = i->getLexNext()) {
                for (CommonVarSet* c = i->getCommonDef(); c != NULL; c = c->next) {
                    /*
                    CommonVarSet* n = new CommonVarSet();
                    n->cvd = c->cvd;
                    n->cvd->refs++;
                    */
                    CommonVarSet *n = new CommonVarSet(*c);
                    CommonVarSet* t;
                    for (t = n; t->next != NULL; t = t->next);
                    t->next = common_def;
                    common_def = n;
                }
            }
        }
    }
    return def;
}

CommonVarSet* CommonData::GetCommonsForVarSet(VarSet* set, AnalysedCallsList* call)
{
    CommonVarSet* res = NULL;
    for (CommonDataItem* i = list; i != NULL; i = i->next) {
        if (i->proc == call) {
            for (CommonVarInfo* v = i->info; v != NULL; v = v->next) {
                if (set->belongs(v->var)) {
                    CommonVarSet* n = new CommonVarSet();
                    n->cvd = v;
                    n->next = res;
                    res = n;
                }
            }
        }
    }
    return res;
}

void CBasicBlock::PrivateAnalysisForAllCalls()
{
    ControlFlowItem* p = start;
    while (p != NULL && (p == start || !p->isLeader())) {
        AnalysedCallsList* c = p->getCall();
        const char* oic = is_correct;
        const char* fpn = failed_proc_name;
        is_correct = NULL;
        failed_proc_name = NULL;
        if (c != NULL && c != (AnalysedCallsList*)(-1) && c != (AnalysedCallsList*)(-2) && c->header != NULL && !c->hasBeenAnalysed) {
            c->hasBeenAnalysed = true;
            
            int stored_fid = SwitchFile(c->file_id);
            
            c->graph->privateAnalyzer();
            
            SwitchFile(stored_fid);
            
        }
        is_correct = oic;
        failed_proc_name = fpn;
        p = p->getNext();
    }
    return;
}

ControlFlowItem* CBasicBlock::containsParloopEnd()
{
    ControlFlowItem* p = start;
    while (p != NULL && (p == start || !p->isLeader())){
        if (p->IsParloopEnd())
            return p;
        p = p->getNext();
    }
    return NULL;
}

ControlFlowItem* CBasicBlock::containsParloopStart()
{
    ControlFlowItem* p = start;
    while (p != NULL && (p == start || !p->isLeader())){
        if (p->IsParloopStart())
            return p;
        p = p->getNext();
    }
    return NULL;
}

void CBasicBlock::print()
{
    printf("block %d: prev: ", num);
    BasicBlockItem* p = prev;
    while (p != NULL){
        printf("%d ", p->block->num);
        p = p->next;
    }
    printf("\n");
}

ControlFlowItem* CBasicBlock::getStart()
{
    return start;
}

ControlFlowItem* CBasicBlock::getEnd()
{
    ControlFlowItem* p = start;
    ControlFlowItem* end = p;
    while (p != NULL && (p == start || !p->isLeader())){
        end = p;
        p = p->getNext();
    }
    return end;
}

VarSet* CBasicBlock::getLVOut()
{
    if (lv_out == NULL)
    {
        VarSet* res = new VarSet();
        BasicBlockItem* p = succ;
        bool first = true;
        while (p != NULL)
        {
            CBasicBlock* b = p->block;
            if (b != NULL && !b->lv_undef)
            {
                res->unite(b->getLVIn(), false);
            }
            p = p->next;
        }
        lv_out = res;
    }
    return lv_out;
}

VarSet* CBasicBlock::getLVIn()
{
    if (lv_in == NULL)
    {
        VarSet* res = new VarSet();
        res->unite(getLVOut(), false);
        res->minus(getDef());
        res->unite(getUse(), false);
        lv_in = res;
    }
    return lv_in;
}

bool CBasicBlock::IsVarDefinedAfterThisBlock(CVarEntryInfo* var, bool os)
{
    findentity = var;
    if (def->belongs(var, os)) {
        findentity = NULL;
        return true;
    }
    BasicBlockItem* p = succ;
    while (p != NULL)
    {
        CBasicBlock* b = p->block;
        if (b->ShouldThisBlockBeCheckedAgain(var) && b->IsVarDefinedAfterThisBlock(var, os)) {
            findentity = NULL;
            return true;
        }
        p = p->next;
    }
    findentity = NULL;
    return false;
}

bool CBasicBlock::stepLVOut()
{
    if (old_lv_out)
        delete old_lv_out;
    
    old_lv_out = lv_out;
    lv_out = NULL;
    getLVOut();
    lv_undef = false;
    //printf("block %d\n", num);
    //old_mrd_out->print();
    //mrd_out->print();
    return (lv_out->equal(old_lv_out));
    //return true;
}

bool CBasicBlock::stepLVIn()
{
    if (old_lv_in)
        delete old_lv_in;

    old_lv_in = lv_in;
    lv_in = NULL;
    getLVIn();
    return (lv_in->equal(old_lv_in));
    //return true;
}

VarSet* CBasicBlock::getMrdIn(bool la)
{
    if (mrd_in == NULL)
    {
        CBasicBlock* delayed = NULL;
        VarSet* res = new VarSet();
        BasicBlockItem* p = prev;
        bool first = true;
        while (p != NULL)
        {
            CBasicBlock* b = p->block;
            if (b != NULL && !b->undef && b->hasPrev())
            {
                if (!p->for_jump_flag && first) {
                    if (first) {
                        res->unite(b->getMrdOut(la), la);
                        if (delayed) {
                            res->intersect(delayed->getMrdOut(la), la, true);
                            delayed = NULL;
                        }
                        first = false;
                    }
                    else
                        res->intersect(b->getMrdOut(la), la, p->for_jump_flag);
                }
                else {
                    delayed = b;
                }
            }
            p = p->next;
        }
        if (delayed) {
            if (first)
                res->unite(delayed->getMrdOut(la), la);
            else
                res->intersect(delayed->getMrdOut(la), la, true);
        }
        mrd_in = res;
    }
    return mrd_in;
}

bool CBasicBlock::hasPrev()
{
    return prev_status == 1;
}

VarSet* CBasicBlock::getMrdOut(bool la)
{
    if (mrd_out == NULL)
    {
        VarSet* res = new VarSet();
        res->unite(getMrdIn(la), la);
        res->unite(getDef(), la);
        mrd_out = res;
        //printf("BLOCK %d INSTR %d MRDOUT: ", num, start->getStmtNo());
        //mrd_out->print();
        //print();
    }
    return mrd_out;
}

bool CBasicBlock::stepMrdOut(bool la)
{
    if (old_mrd_out)
        delete old_mrd_out;
    
    old_mrd_out = mrd_out;
    mrd_out = NULL;
    getMrdOut(la);
    undef = false;
    //printf("block %d\n", num);
    //old_mrd_out->print();
    //mrd_out->print();
    return (mrd_out->equal(old_mrd_out));
    //return true;
}

bool CBasicBlock::stepMrdIn(bool la)
{
    if (old_mrd_in)
        delete old_mrd_in;
    
    old_mrd_in = mrd_in;
    mrd_in = NULL;
    getMrdIn(la);
    return (mrd_in->equal(old_mrd_in));
    //return true;
}

bool IsPresentInExprList(SgExpression* ex, CExprList* lst)
{
    while (lst != NULL) {
        if (lst->entry == ex)
            return true;
        lst = lst->next;
    }
    return false;
}

CRecordVarEntryInfo* AddRecordVarRef(SgRecordRefExp* ref)
{
    if (isSgRecordRefExp(ref->lhs())) {
        CVarEntryInfo* parent = AddRecordVarRef(isSgRecordRefExp(ref->lhs()));
        if (parent)
            return new CRecordVarEntryInfo(ref->rhs()->symbol(), parent);
        return NULL;
    }
    if (isSgVarRefExp(ref->lhs())) {
        CVarEntryInfo* parent = new CScalarVarEntryInfo(isSgVarRefExp(ref->lhs())->symbol());
        return new CRecordVarEntryInfo(ref->rhs()->symbol(), parent);
    }
    if (isSgArrayRefExp(ref->lhs())) {
        CVarEntryInfo* parent = new CArrayVarEntryInfo(isSgArrayRefExp(ref->lhs())->symbol(), isSgArrayRefExp(ref->lhs()));
        return new CRecordVarEntryInfo(ref->rhs()->symbol(), parent);
    }
    return NULL;
}

void CBasicBlock::AddOneExpressionToUse(SgExpression* ex, SgStatement* st, CArrayVarEntryInfo* v)
{
    CVarEntryInfo* var = NULL;
    SgVarRefExp* r;
    if ((r = isSgVarRefExp(ex)))
        var = new CScalarVarEntryInfo(r->symbol());
    SgArrayRefExp* ar;
    if ((ar = isSgArrayRefExp(ex))) {
        if (!v)
            var = new CArrayVarEntryInfo(ar->symbol(), ar);
        else {
            var = v->Clone();
            var->SwitchSymbol(ar->symbol());
        }
    }
    SgRecordRefExp* rr;
    if ((rr = isSgRecordRefExp(ex)))
        var = AddRecordVarRef(rr);
    if (var) {
        var->RegisterUsage(def, use, st);
        delete var;
    }
}

void CBasicBlock::AddOneExpressionToDef(SgExpression* ex, SgStatement* st, CArrayVarEntryInfo* v)
{
    CVarEntryInfo* var = NULL;
    SgVarRefExp* r;
    if ((r = isSgVarRefExp(ex)))
        var = new CScalarVarEntryInfo(r->symbol());
    SgRecordRefExp* rr;
    if ((rr = isSgRecordRefExp(ex)))
        var = AddRecordVarRef(rr);
    SgArrayRefExp* ar;
    if ((ar = isSgArrayRefExp(ex))) {
        if (!v)
            var = new CArrayVarEntryInfo(ar->symbol(), ar);
        else {
            var = v->Clone();
            var->SwitchSymbol(ar->symbol());
        }
    }
    if (var) {
        var->RegisterDefinition(def, use, st);
        delete var;
    }
}

void CBasicBlock::addExprToUse(SgExpression* ex, CArrayVarEntryInfo* v = NULL, CExprList* lst = NULL)
{
    if (ex != NULL)
    {
        CExprList* cur = new CExprList();
        cur->entry = ex;
        cur->next = lst;
        SgFunctionCallExp* f = isSgFunctionCallExp(ex);
        if (!f) {
            if (!IsPresentInExprList(ex->lhs(), cur))
                addExprToUse(ex->lhs(), v, cur);
            if (!isSgUnaryExp(ex))
                if (!IsPresentInExprList(ex->rhs(), cur))
                    addExprToUse(ex->rhs(), v, cur);
            AddOneExpressionToUse(ex, NULL, v);
        }
        delete cur;
        /*
        SgVarRefExp* r;
        //printf(" %s\n", f->funName()->identifier());
        bool intr = isIntrinsicFunctionNameACC(f->funName()->identifier()) && !IsUserFunctionACC(f->funName());
        bool pure = IsPureProcedureACC(f->funName());
        if (!intr && !pure){
            printf("function not intristic or pure: %s\n", f->funName()->identifier());
            is_correct = false;
            return;
        }
        if (intr) {
            ProcessIntristicProcedure(true, f->numberOfArgs(), f);
            return;
        }
        ProcessProcedureHeader(true, isSgProcHedrStmt(GRAPHNODE(f->funName())->st_header), f);
        */
    }
}

void CBasicBlock::ProcessIntrinsicProcedure(bool isF, int narg, void* f, const char* name)
{
    for (int i = 0; i < narg; i++) {
        SgExpression* ar = GetProcedureArgument(isF, f, i);
        if (IsAnIntrinsicSubroutine(name)) {
            SgExpression* v = CheckIntrinsicParameterFlag(name, i, ar, INTRINSIC_IN);
            if (v) {
                addExprToUse(v);
            }
        }else{
            addExprToUse(ar);
        }
        AddOneExpressionToDef(CheckIntrinsicParameterFlag(name, i, ar, INTRINSIC_OUT), NULL, NULL);
    }
}

void CBasicBlock::ProcessProcedureWithoutBody(bool isF, void* f, bool out)
{
    for (int i = 0; i < GetNumberOfArguments(isF, f); i++){
        addExprToUse(GetProcedureArgument(isF, f, i));
        if (out)
            AddOneExpressionToDef(GetProcedureArgument(isF, f, i), NULL, NULL);
    }
}

SgSymbol* CBasicBlock::GetProcedureName(bool isFunc, void* f)
{
    if (isFunc) {
        SgFunctionCallExp* fc = (SgFunctionCallExp*)f;
        return fc->funName();
    }
    SgCallStmt* pc = (SgCallStmt*)f;
    return pc->name();
}

int GetNumberOfArguments(bool isF, void* f)
{
    if (isF) {
        SgFunctionCallExp* fc = (SgFunctionCallExp*)f;
        return fc->numberOfArgs();
    }
    SgCallStmt* pc = (SgCallStmt*)f;
    return pc->numberOfArgs();
}

SgExpression* GetProcedureArgument(bool isF, void *f, const int i)
{
    SgExpression *arg = NULL;
    if (isF)
    {
        SgFunctionCallExp* fc = (SgFunctionCallExp*)f;
        arg = fc->arg(i);
    }
    else
    {
        SgCallStmt *pc = (SgCallStmt*)f;
        arg = pc->arg(i);
    }
    return arg;
}

void CBasicBlock::ProcessProcedureHeader(bool isF, SgProcHedrStmt *header, void *f, const char* name)
{   
    if (!header) 
    {
        is_correct = "no header found";
        failed_proc_name = name;
        return;
    }
    
    for (int i = 0; i < header->numberOfParameters(); ++i) 
    {
        int stored = SwitchFile(header->getFileId());
        SgSymbol *arg = header->parameter(i);
        SwitchFile(stored);

        if (arg->attributes() & (IN_BIT)) 
        {
            SgExpression *ar = GetProcedureArgument(isF, f, i);
            addExprToUse(ar);
        }        
        else if (arg->attributes() & (INOUT_BIT)) 
        {
            addExprToUse(GetProcedureArgument(isF, f, i));
            AddOneExpressionToDef(GetProcedureArgument(isF, f, i), NULL, NULL);
        }
        else if (arg->attributes() & (OUT_BIT))
            AddOneExpressionToDef(GetProcedureArgument(isF, f, i), NULL, NULL);
        else 
        {
            is_correct = "no bitflag set for pure procedure";
            break;
        }
    }    
}

bool AnalysedCallsList::isArgIn(int i, CArrayVarEntryInfo** p)
{
    int stored = SwitchFile(this->file_id);
    SgProcHedrStmt* h = isSgProcHedrStmt(header);
    VarSet* use = graph->getUse();
    SgSymbol* par = h->parameter(i);
    /*
    CScalarVarEntryInfo* var = new CScalarVarEntryInfo(par);
    bool result = false;
    if (use->belongs(var))
        result = true;
    delete var;
    */
    VarItem* result = use->belongs(par);
    if (result && result->var->GetVarType() == VAR_REF_ARRAY_EXP && p) 
        *p = (CArrayVarEntryInfo*)result->var;
    SwitchFile(stored);

    return result;
}

bool AnalysedCallsList::isArgOut(int i, CArrayVarEntryInfo** p)
{
    int stored = SwitchFile(this->file_id);
    SgProcHedrStmt* h = isSgProcHedrStmt(header);
    graph->privateAnalyzer();
    VarSet* def = graph->getDef();
    SgSymbol* par = h->parameter(i);
    /*
    CScalarVarEntryInfo* var = new CScalarVarEntryInfo(par);
    bool result = false;
    if (def->belongs(var))
        result = true;
    delete var;
    */
    VarItem* result = def->belongs(par);
    if (result && result->var->GetVarType() == VAR_REF_ARRAY_EXP && p) 
        *p = (CArrayVarEntryInfo*)result->var;
    SwitchFile(stored);

    return result;
}

void CommonData::MarkAsUsed(VarSet* use, AnalysedCallsList* lst)
{
    for (CommonDataItem* it = list; it != NULL; it = it->next) {
        if (it->proc == lst) {
            for (CommonVarInfo* v = it->info; v != NULL; v = v->next) {
                CVarEntryInfo* r = v->var;
                if (use->belongs(r))
                    v->isInUse = true;
            }
        }
    }
}

void CBasicBlock::ProcessUserProcedure(bool isFun, void* call, AnalysedCallsList* c)
{
    /*
    if (c == NULL || c->graph == NULL) {
        is_correct = "no body found for procedure";
        if (c != NULL)
            failed_proc_name = c->funName;
        else
            failed_proc_name = NULL;
        return;
    }
    */
    if (c != (AnalysedCallsList*)(-1) && c != (AnalysedCallsList*)(-2) && c != NULL && c->graph != NULL) 
    {
        int stored_file_id = SwitchFile(c->file_id);
        c->graph->getPrivate(); //all sets actually
        SgStatement *cp = c->header->controlParent();
        SwitchFile(stored_file_id);

        if (proc && proc->header->variant() == PROC_HEDR && cp == proc->header) {
            VarSet* use_c = new VarSet();
            use_c->unite(c->graph->getUse(), false);
            for (VarItem* exp = use_c->getFirst(); exp != NULL; exp = use_c->getFirst()) {
                if (exp->var->GetSymbol()->scope() == proc->header) {
                    addExprToUse(new SgVarRefExp(exp->var->GetSymbol())); // TESTING
                }
                use_c->remove(exp->var);
            }
            delete use_c;
            VarSet* def_c = new VarSet();
            def_c->unite(c->graph->getDef(), true);
            for (VarItem* exp = def_c->getFirst(); exp != NULL; exp = def_c->getFirst()) {
                if (exp->var->GetSymbol()->scope() == proc->header) {
                    def->addToSet(exp->var, NULL);
                }
                def_c->remove(exp->var);
            }
            delete def_c;
        }

        pCommons->MarkAsUsed(c->graph->getUse(), c);
        SgProcHedrStmt* header = isSgProcHedrStmt(c->header);
        if (!header) {
            is_correct = "no header for procedure";
            failed_proc_name = c->funName;
            return;
        }
    }

    for (int i = 0; i < GetNumberOfArguments(isFun, call); i++) 
    {
        SgExpression* ar = GetProcedureArgument(isFun, call, i);
        CArrayVarEntryInfo* tp = NULL;
        if (c == (AnalysedCallsList*)(-1) || c == (AnalysedCallsList*)(-2) || c == NULL || c->graph == NULL || c->isArgIn(i, &tp))
            addExprToUse(ar, tp);
        tp = NULL;
        if (c == (AnalysedCallsList*)(-1) || c == NULL || c->graph == NULL || c->isArgOut(i, &tp))
            AddOneExpressionToDef(GetProcedureArgument(isFun, call, i), NULL, tp);
    }

    if (c != (AnalysedCallsList*)(-1) && c != (AnalysedCallsList*)(-2) && c != NULL && c->graph != NULL) {
        for (CommonVarSet* cu = c->graph->getCommonUse(); cu != NULL; cu = cu->next) {
            CommonVarInfo* v = cu->cvd;
            AnalysedCallsList* tp = start->getProc();
            CommonDataItem* p = v->parent;
            if (CommonDataItem* it = pCommons->IsThisCommonUsedInProcedure(p, tp)) {
                if (pCommons->CanHaveNonScalarVars(it))
                    continue;
                CommonVarInfo* i = it->info;
                CommonVarInfo* j = p->info;
                while (j != v) {
                    j = j->next;
                    if (i)
                        i = i->next;
                    else
                        continue;
                }
                if (!i)
                    continue;
                SgVarRefExp* var = new SgVarRefExp(i->var->GetSymbol());
                addExprToUse(var);
            }
            else {
                common_use = new CommonVarSet(*cu);
            }
        }
        for (CommonVarSet* cd = c->graph->getCommonDef(); cd != NULL; cd = cd->next) {
            CommonVarInfo* v = cd->cvd;
            AnalysedCallsList* tp = start->getProc();
            CommonDataItem* p = v->parent;
            if (CommonDataItem* it = pCommons->IsThisCommonUsedInProcedure(p, tp)) {
                if (pCommons->CanHaveNonScalarVars(it))
                    continue;
                CommonVarInfo* i = it->info;
                CommonVarInfo* j = p->info;
                while (j != v) {
                    j = j->next;
                    if (i)
                        i = i->next;
                }
                if (!i)
                    continue;
                def->addToSet(i->var, NULL);
            }
            else {
                common_def = new CommonVarSet(*cd);
            }
        }
    }

}

bool CommonData::CanHaveNonScalarVars(CommonDataItem* item)
{
    for (CommonDataItem* it = list; it != NULL; it = it->next) {
        if (it->name == item->name && it->first == item->first && !it->onlyScalars)
            return true;
    }
    bool res = !item->onlyScalars;
    //printf("CommonData::CanHaveNonScalarVars: %d\n", res);
    return res;
}

CommonDataItem* CommonData::IsThisCommonUsedInProcedure(CommonDataItem* item, AnalysedCallsList* p)
{
    for (CommonDataItem* it = list; it != NULL; it = it->next) {
        if (it->proc == p) {
            if (it->name == item->name)
                return it;
        }
    }
    return NULL;
}

void CBasicBlock::setDefAndUse()
{
    ControlFlowItem* p = start;
    while (p != NULL && (p == start || !p->isLeader()))
    {
        if (p->getJump() == NULL)
        {
            SgStatement* st = p->getStatement();
            SgFunctionCallExp* f = p->getFunctionCall();

            if (f != NULL)
            {
                bool add_intr = IsAnIntrinsicSubroutine(f->funName()->identifier()) != NULL; // strcmp(f->funName()->identifier(), "date_and_time") == 0;
                bool intr = (isIntrinsicFunctionNameACC(f->funName()->identifier()) || add_intr) && !IsUserFunctionACC(f->funName());
                bool pure = IsPureProcedureACC(f->funName());
                AnalysedCallsList* c = p->getCall();
                if (!intr && !pure && c && c != (AnalysedCallsList*)(-1) && c != (AnalysedCallsList*)(-2) && !(c->IsIntrinsic())) {

                    if (c->header == NULL) {
                        is_correct = "no header for procedure";
                        failed_proc_name = c->funName;
                    }
                    else {
                        //graph_node* oldgn = currentGraphNode;
                        //graph_node* newgn = GRAPHNODE(f->funName())->file_id;
                        //currentGraphNode = newgn;
                        ProcessUserProcedure(true, f, c);
                        //currentGraphNode = oldgn;

                    }
                }
                else if (c == (AnalysedCallsList*)(-1) || c == (AnalysedCallsList*)(-2))
                    ProcessProcedureWithoutBody(true, f, c == (AnalysedCallsList*)(-1));
                else if (intr || (c && c->IsIntrinsic())) {
                    ProcessIntrinsicProcedure(true, f->numberOfArgs(), f, f->funName()->identifier());
                }else
                    ProcessProcedureHeader(true, isSgProcHedrStmt(GRAPHNODE(f->funName())->st_header), f, f->funName()->identifier());
            }


            if (st != NULL)
            {
                switch (st->variant())
                {
                case ASSIGN_STAT:
                    {
                        SgAssignStmt* s = isSgAssignStmt(st);
                        SgExpression* l = s->lhs();
                        SgExpression* r = s->rhs();
                        addExprToUse(r);
                        AddOneExpressionToDef(l, st, NULL);
                        break;
                    }
                case PRINT_STAT:
                case WRITE_STAT:
                case READ_STAT:
                    {
                        SgInputOutputStmt* s = isSgInputOutputStmt(st);
                        if (s) {
                            SgExpression* ex = s->itemList();
                            while (ex && ex->lhs()) {
                                if (st->variant() == READ_STAT) {
                                    AddOneExpressionToDef(ex->lhs(), st, NULL);
                                }
                                else {
                                    addExprToUse(ex->lhs());
                                }
                                ex = ex->rhs();
                            }
                        }
                        break;
                    }
                case PROC_STAT:
                {
                    SgCallStmt* f = isSgCallStmt(st);
                    bool add_intr = IsAnIntrinsicSubroutine(f->name()->identifier()) != NULL;
                    bool intr = (isIntrinsicFunctionNameACC(f->name()->identifier()) || add_intr) && !IsUserFunctionACC(f->name());
                    bool pure = IsPureProcedureACC(f->name());
                    if (!intr && !pure) {
                        AnalysedCallsList* c = p->getCall();
                        //graph_node* oldgn = currentGraphNode;
                        //graph_node* newgn = GRAPHNODE(f->name());
                        //currentGraphNode = newgn;
                        ProcessUserProcedure(false, f, c);
                        //currentGraphNode = oldgn;
                        break;
                    }
                    if (intr) {
                        ProcessIntrinsicProcedure(false, f->numberOfArgs(), f, f->name()->identifier());
                        break;
                    }
                    ProcessProcedureHeader(false, isSgProcHedrStmt(GRAPHNODE(f->name())->st_header), f, f->name()->identifier());
                }
                default:
                    break;
                }
            }
        }
        else
            addExprToUse(p->getExpression());
            p = p->getNext();
    }
}

VarSet* CBasicBlock::getDef()
{
    if (def == NULL)
    {
        def = new VarSet();
        use = new VarSet();
        setDefAndUse();
    }
    return def;
}

VarSet* CBasicBlock::getUse()
{
    if (use == NULL)
    {
        use = new VarSet();
        def = new VarSet();
        setDefAndUse();
    }
    return use;
}

#ifdef __SPF
template<typename IN_TYPE, typename OUT_TYPE>
const vector<OUT_TYPE> getAttributes(IN_TYPE st, const set<int> dataType);
#endif

DoLoopDataItem* DoLoopDataList::FindLoop(SgStatement* st)
{
    DoLoopDataItem* it = list;
    while (it != NULL) {
        if (it->statement == st)
            return it;
        it = it->next;
    }
    return NULL;
}

bool GetExpressionAndCoefficientOfBound(SgExpression* exp, SgExpression** end, int* coef)
{
    if (exp->variant() == SUBT_OP) {
        if (exp->rhs() && exp->rhs()->variant() == INT_VAL) {
            *end = exp->lhs();
            *coef = -exp->rhs()->valueInteger();
            return true;
        }
    }
    if (exp->variant() == ADD_OP) {
        if (exp->lhs() && exp->lhs()->variant() == INT_VAL) {
            *end = exp->rhs();
            *coef = exp->lhs()->valueInteger();
            return true;
        }
        if (exp->rhs() && exp->rhs()->variant() == INT_VAL) {
            *end = exp->lhs();
            *coef = exp->lhs()->valueInteger();
            return true;
        }
    }
    return false;
}

CArrayVarEntryInfo::CArrayVarEntryInfo(SgSymbol* s, SgArrayRefExp* r) : CVarEntryInfo(s)
{
#if __SPF
    addToCollection(__LINE__, __FILE__, this, 16);
#endif
    // TODO: need to check all alhorithm!!
    disabled = true;

    if (!r)
        subscripts = 0;
    else
        subscripts = r->numberOfSubscripts();
    if (subscripts)
        data.resize(subscripts);    

    for (int i = 0; i < subscripts; i++) 
    {
        data[i].defined = false;
        data[i].bound_modifiers[0] = data[i].bound_modifiers[1] = 0;
        data[i].step = 1;
        data[i].left_bound = data[i].right_bound = NULL;
        data[i].coefs[0] = data[i].coefs[1] = 0;
        data[i].loop = NULL;
#if __SPF
        const vector<int*> coefs = getAttributes<SgExpression*, int*>(r->subscript(i), set<int>{ INT_VAL });
        const vector<SgStatement*> fs = getAttributes<SgExpression*, SgStatement*>(r->subscript(i), set<int>{ FOR_NODE });
        if (fs.size() == 1) 
        {
            if (data[i].loop != NULL) 
            {
                if (coefs.size() == 1) 
                {
                    data[i].defined = true;
                    data[i].bound_modifiers[0] = data[i].bound_modifiers[1] = coefs[0][1];
                    data[i].coefs[0] = coefs[0][0];
                    data[i].coefs[1] = coefs[0][1];
                    data[i].step = coefs[0][0];
                    int tmp;

                    SgExpression *et;
                    if (GetExpressionAndCoefficientOfBound(data[i].loop->l, &et, &tmp)) 
                    {
                        data[i].left_bound = et;
                        data[i].bound_modifiers[0] += tmp;
                    }
                    else 
                        data[i].left_bound = data[i].loop->l;
                    
                    if (GetExpressionAndCoefficientOfBound(data[i].loop->r, &et, &tmp)) 
                    {
                        data[i].right_bound = et;
                        data[i].bound_modifiers[1] += tmp;
                    }
                    else 
                        data[i].right_bound = data[i].loop->r;                    
                }
            }
        }
#endif
        if (!data[i].defined) 
        {
            SgExpression* ex = r->subscript(i);
            if (ex->variant() == INT_VAL) 
            {
                data[i].bound_modifiers[0] = ex->valueInteger();
                data[i].bound_modifiers[1] = ex->valueInteger();
                data[i].defined = true;
            }
            else 
            {
                data[i].bound_modifiers[0] = 0;
                data[i].bound_modifiers[1] = 0;
                data[i].left_bound = data[i].right_bound = ex;
                data[i].defined = true;
            }
        }
    }
}

CArrayVarEntryInfo::CArrayVarEntryInfo(SgSymbol *s, int sub, int ds, const vector<ArraySubscriptData> &d) 
                                       : CVarEntryInfo(s), subscripts(sub), disabled(ds)
{ 
#if __SPF
    addToCollection(__LINE__, __FILE__, this, 16);
#endif
    if (sub > 0) 
        data = d;
}

VarItem* VarSet::GetArrayRef(CArrayVarEntryInfo* info)
{
    VarItem* it = list;
    while (it != NULL) {
        CVarEntryInfo* v = it->var;
        if (v->GetVarType() == VAR_REF_ARRAY_EXP) {
            if (OriginalSymbol(info->GetSymbol()) == OriginalSymbol(v->GetSymbol()))
                return it;
        }
        it = it->next;
    }
    return NULL;
}

void CArrayVarEntryInfo::RegisterUsage(VarSet *def, VarSet *use, SgStatement *st)
{
    VarItem *it = def->GetArrayRef(this);
    CArrayVarEntryInfo *add = this;
    if (it != NULL) 
        add = *this - *(CArrayVarEntryInfo*)(it->var);
    
    if (use != NULL && add != NULL && add->HasActiveElements()) 
        use->addToSet(add, st);
    
    if (add != this)
        delete add;
}

CArrayVarEntryInfo& CArrayVarEntryInfo::operator-=(const CArrayVarEntryInfo& b)
{
    if (subscripts == 0)
    {
        if (b.HasActiveElements()) 
            disabled = true;        
        return *this;
    }

    if (b.subscripts == 0) 
    {
        if (HasActiveElements()) 
            MakeInactive();        
        return *this;
    }

    if (subscripts != b.subscripts || !data.size() || !b.data.size() || !(data[0].defined) || !(b.data[0].defined)) 
        return *this;
    
    for (int i = 0; i < subscripts; i++) 
    {
        if (b.data[i].left_bound == NULL) 
        {
            if (data[i].left_bound && data[i].left_bound->variant() == INT_VAL) 
            {
                if (data[i].left_bound->valueInteger() + data[i].bound_modifiers[0] == b.data[i].bound_modifiers[0]) 
                {
                    data[i].bound_modifiers[0]++;
                    continue;
                }
            }
        }

        if (data[i].left_bound == NULL && b.data[i].left_bound == NULL &&
            data[i].right_bound == NULL && b.data[i].right_bound == NULL) 
        {
            if (data[i].bound_modifiers[0] < b.data[i].bound_modifiers[0]) 
            {
                data[i].bound_modifiers[1] = b.data[i].bound_modifiers[0] - 1;
                continue;
            }

            if (data[i].bound_modifiers[1] > b.data[i].bound_modifiers[1]) 
            {
                data[i].bound_modifiers[0] = b.data[i].bound_modifiers[1] + 1;
                continue;
            }
            data[i].defined = false;
        }

        if (data[i].left_bound == b.data[i].left_bound && data[i].bound_modifiers[0] < b.data[i].bound_modifiers[0]) 
        {
            data[i].bound_modifiers[0] = data[i].bound_modifiers[0];
            data[i].bound_modifiers[1] = b.data[i].bound_modifiers[0] - 1;
            data[i].right_bound = data[i].left_bound;
        }

        if (data[i].right_bound == b.data[i].right_bound && data[i].bound_modifiers[1] > b.data[i].bound_modifiers[1]) 
        {
            data[i].bound_modifiers[0] = b.data[i].bound_modifiers[1] + 1;
            data[i].bound_modifiers[1] = data[i].bound_modifiers[1];
            data[i].left_bound = data[i].right_bound;
        }

        if (b.data[i].left_bound == NULL && b.data[i].right_bound == NULL && 
            (data[i].left_bound != NULL || data[i].right_bound != NULL))
            continue;        
        else 
        {
            data[i].bound_modifiers[0] = data[i].bound_modifiers[1] = 0;
            data[i].left_bound = NULL;
            data[i].right_bound = NULL;
            data[i].defined = false;
            //empty set
        }
    }
    return *this;
}

CArrayVarEntryInfo* operator-(const CArrayVarEntryInfo& a, const CArrayVarEntryInfo& b)
{
    //return NULL;
    CArrayVarEntryInfo* nv = (CArrayVarEntryInfo*)a.Clone();
    *nv -= b;
    return nv;
}

CArrayVarEntryInfo* operator+(const CArrayVarEntryInfo& a, const CArrayVarEntryInfo& b)
{
    CArrayVarEntryInfo* nv = (CArrayVarEntryInfo*)a.Clone();
    *nv += b;
    return nv;
}

void CArrayVarEntryInfo::RegisterDefinition(VarSet* def, VarSet* use, SgStatement* st)
{
    def->addToSet(this, st);
    use->PossiblyAffectArrayEntry(this);
}

void VarSet::PossiblyAffectArrayEntry(CArrayVarEntryInfo* var)
{
    VarItem* it = GetArrayRef(var);
    if (!it)
        return;
    ((CArrayVarEntryInfo*)(it->var))->ProcessChangesToUsedEntry(var);
}

void CArrayVarEntryInfo::ProcessChangesToUsedEntry(CArrayVarEntryInfo* var)
{
    if (disabled || var->disabled || subscripts != var->subscripts)
        return;
    for (int i = 0; i < subscripts; i++) 
    {
        if (!data[i].defined)
            continue;

        if (data[i].loop == var->data[i].loop && data[i].loop != NULL) 
        {
            if (data[i].coefs[0] == var->data[i].coefs[0])
            {
                if (data[i].coefs[1] < var->data[i].coefs[1]) 
                {
                    if (data[i].left_bound && data[i].left_bound->variant() == INT_VAL) 
                    {
                        data[i].bound_modifiers[0] = data[i].left_bound->valueInteger() + data[i].bound_modifiers[0];
                        data[i].bound_modifiers[1] = data[i].left_bound->valueInteger() + var->data[i].coefs[1] - 1;
                        data[i].left_bound = data[i].right_bound = NULL;
                    }
                    else 
                    {
                        //maybe add something, not sure
                    }
                }
            }
        }
    }
}

CArrayVarEntryInfo& CArrayVarEntryInfo::operator*=(const CArrayVarEntryInfo& b)
{
    if (subscripts == 0)
    {
        if (b.HasActiveElements())
            disabled = true;
        return *this;
    }

    if (b.subscripts == 0)
    {
        if (HasActiveElements())
            MakeInactive();
        return *this;
    }

    //return *this;
    if (subscripts != b.subscripts || subscripts == 0 || b.subscripts == 0 || !data.size() || !b.data.size() || !(data[0].defined) || !(b.data[0].defined)) 
        return *this;
    
    for (int i = 0; i < subscripts; i++) 
    {
        if (b.disabled) 
            data[i].left_bound = data[i].right_bound = NULL;
        
        if (data[i].left_bound == b.data[i].left_bound) 
            data[i].bound_modifiers[0] = std::max(data[i].bound_modifiers[0], b.data[i].bound_modifiers[0]);
        
        if (data[i].right_bound == b.data[i].right_bound) 
            data[i].bound_modifiers[1] = std::min(data[i].bound_modifiers[1], b.data[i].bound_modifiers[1]);        
    }
    return *this;
}

CArrayVarEntryInfo& CArrayVarEntryInfo::operator+=(const CArrayVarEntryInfo& b)
{
    if (subscripts == 0)
    {
        if (b.HasActiveElements())
            disabled = true;
        return *this;
    }

    if (b.subscripts == 0)
    {
        if (HasActiveElements())
            MakeInactive();
        return *this;
    }

    //return *this;
    if (disabled && !b.disabled && b.data.size()) 
    {
        for (int i = 0; i < subscripts; i++)
            data[i] = b.data[i];
        disabled = false;
        return *this;
    }

    if (subscripts != b.subscripts || subscripts == 0 || b.subscripts == 0 || !data.size() || !b.data.size() || disabled || b.disabled) 
        return *this;
    
    for (int i = 0; i < subscripts; i++) 
    {

        if (data[i].left_bound == b.data[i].left_bound) 
            data[i].bound_modifiers[0] = std::min(data[i].bound_modifiers[0], b.data[i].bound_modifiers[0]);
        
        if (data[i].right_bound == b.data[i].right_bound) 
            data[i].bound_modifiers[1] = std::max(data[i].bound_modifiers[1], b.data[i].bound_modifiers[1]);
        
        if (data[i].left_bound == NULL && data[i].right_bound == NULL && (b.data[i].left_bound != NULL || b.data[i].right_bound != NULL)) 
        {
            const ArraySubscriptData &tmp = data[i];
            data[i] = b.data[i];
            if (data[i].left_bound && data[i].left_bound->variant() == INT_VAL) 
            {
                if (tmp.bound_modifiers[1] == data[i].left_bound->valueInteger() + data[i].bound_modifiers[0] - 1) 
                    data[i].bound_modifiers[0] -= (1 + tmp.bound_modifiers[1] - tmp.bound_modifiers[0]);
                
            }

            if (data[i].right_bound && data[i].right_bound->variant() == INT_VAL) 
            {
                if (tmp.bound_modifiers[0] == data[i].left_bound->valueInteger() + data[i].bound_modifiers[1] + 1) 
                    data[i].bound_modifiers[1] += (1 + tmp.bound_modifiers[1] - tmp.bound_modifiers[0]);                
            }
        }
    }
    return *this;
}

void VarSet::RemoveDoubtfulCommonVars(AnalysedCallsList* call)
{
    VarItem* it = list;
    VarItem* prev = NULL;
    while (it != NULL) {
        CommonDataItem* d = pCommons->IsThisCommonVar(it, call);
        if (d && pCommons->CanHaveNonScalarVars(d)) {
            if (prev == NULL) {
                it = it->next;
                delete list;
                list = it;
            }
            else {
                prev->next = it->next;
                delete it;
                it = prev->next;
            }
            continue;
        }
        prev = it;
        it = it->next;
    }
}

int VarSet::count()
{
    VarItem* it = list;
    int t = 0;
    while (it != NULL) {
        it = it->next;
        t++;
    }
    return t;
}

void VarSet::LeaveOnlyRecords()
{
    VarItem* p = list;
    VarItem* prev = NULL;
    while (p != NULL) {
        if (p->var->GetVarType() == VAR_REF_RECORD_EXP) {
            CVarEntryInfo* rrec = p->var->GetLeftmostParent();
            CVarEntryInfo* old = p->var;
            if (old->RemoveReference())
                delete old;
            if (!belongs(rrec)) {
                p->var = rrec;
                prev = p;
            }
            else {
                if (prev == NULL)
                    list = list->next;
                else
                {
                    prev->next = p->next;
                    delete(p);
                    p = prev;
                }
            }
        }
        else {
            prev = p;
        }
        p = p->next;
    }
}

VarItem* VarSet::belongs(const CVarEntryInfo* var, bool os)
{
    VarItem* l = list;
    while (l != NULL)
    {
        if ((*l->var == *var))
            return l;
        if (os && OriginalSymbol(l->var->GetSymbol()) == OriginalSymbol(var->GetSymbol()))
            return l;
        l = l->next;
    }
    return NULL;
}

VarItem* VarSet::belongs(SgSymbol* s, bool os)
{
    VarItem* l = list;
    while (l != NULL)
    {
        if ((l->var->GetSymbol() == s))
            if (l->var->GetVarType() == VAR_REF_ARRAY_EXP)
                return ((CArrayVarEntryInfo*)(l->var))->HasActiveElements() ? l : NULL;
            return l;
        if (os && OriginalSymbol(l->var->GetSymbol()) == OriginalSymbol(s))
            return l;
        l = l->next;
    }
    return NULL;
}

/*
VarItem* VarSet::belongs(SgVarRefExp* var, bool os)
{
    return belongs(var->symbol(), os);
}
*/

bool VarSet::equal(VarSet* p2)
{
    if (p2 == NULL)
        return false;
    VarItem* p = list;
    VarItem* prev = NULL;
    while (p != NULL)
    {
        if (!p2->belongs(p->var) && (p->var->GetVarType() != VAR_REF_ARRAY_EXP || ((CArrayVarEntryInfo*)(p->var))->HasActiveElements()))
            return false;
        p = p->next;
    }
    p = p2->list;
    while (p != NULL) {
        if (!belongs(p->var) && (p->var->GetVarType() != VAR_REF_ARRAY_EXP || ((CArrayVarEntryInfo*)(p->var))->HasActiveElements()))
            return false;
        p = p->next;
    }
    return true;
}

void VarSet::print()
{
    VarItem* l = list;
    while (l != NULL)
    {
        if (l->var->GetVarType() != VAR_REF_ARRAY_EXP || ((CArrayVarEntryInfo*)(l->var))->HasActiveElements())
            printf("%s ", l->var->GetSymbol()->identifier());
#if PRIVATE_GET_LAST_ASSIGN
        printf("last assignments: %d\n", l->lastAssignments.size());
        for (list<SgStatement*>::iterator it = l->lastAssignments.begin(); it != l->lastAssignments.end(); it++){
            if (*it)
                printf("%s", (*it)->unparse());
        }
#endif
        l = l->next;
    }
    putchar('\n');
}

void VarSet::addToSet(CVarEntryInfo* var, SgStatement* source, CVarEntryInfo* ov)
{
    bool add = false;
    if (var->GetVarType() != VAR_REF_ARRAY_EXP) {
        VarItem* p = belongs(var, false);
        add = p == NULL;
#if PRIVATE_GET_LAST_ASSIGN
        p->lastAssignments.clear();
        p->lastAssignments.push_back(source);
#endif
        //delete p->lastAssignments;
        //p->lastAssignments = new CLAStatementItem();
        //p->lastAssignments->stmt = source;
        //p->lastAssignments->next = NULL;
    }
    else {
        CArrayVarEntryInfo* av = (CArrayVarEntryInfo*)var;
        VarItem* p = GetArrayRef(av);
        if (p == NULL)
            add = true;
        else {
            CArrayVarEntryInfo* fv = (CArrayVarEntryInfo*)p->var;
            *fv += *av;
        }
    }
    if (add) {
        VarItem* p = new VarItem();
        p->var = var->Clone();
        p->ov = ov;
        p->next = list;
        p->file_id = current_file_id;
        list = p;
    }
}

void VarSet::intersect(VarSet* set, bool la, bool array_mode = false)
{
    VarItem* p = list;
    VarItem* prev = NULL;
    while (p != NULL)
    {        
        VarItem* n = set->belongs(p->var);
        if (!n)
        {
            if (!array_mode) {
                if (prev == NULL)
                    list = list->next;
                else
                {
                    prev->next = p->next;
                    delete(p);
                    p = prev;
                }
            }
        }
        else {
#if PRIVATE_GET_LAST_ASSIGN
            if (la)
                p->lastAssignments.insert(p->lastAssignments.end(), n->lastAssignments.begin(), n->lastAssignments.end());
#endif
            if (p->var->GetVarType() == VAR_REF_ARRAY_EXP) {
                if (!array_mode)
                    *(CArrayVarEntryInfo*)(p->var) *= *(CArrayVarEntryInfo*)(n->var);
                else
                    *(CArrayVarEntryInfo*)(p->var) += *(CArrayVarEntryInfo*)(n->var);
            }
            prev = p;
        }
        p = p->next;
    }
    
}

VarItem* VarSet::getFirst()
{
    return list;
}

void VarSet::remove(const CVarEntryInfo* var)
{
    VarItem* p = list;
    VarItem* prev = NULL;
    while (p != NULL)
    {
        if (var == (p->var))
        {
            if (prev == NULL) {
                VarItem* t = list;
                list = list->next;
                delete(t);
                p = list;

            }
            else
            {
                prev->next = p->next;
                delete(p);
                p = prev->next;
            }
        }
        else {
            prev = p;
            p = p->next;
        }
    }
}

void VarSet::minus(VarSet* set, bool complete)
{
    VarItem* p = list;
    VarItem* prev = NULL;
    while (p != NULL)
    {
        VarItem* d = set->belongs(p->var);
        if (d && (p->var->GetVarType() != VAR_REF_ARRAY_EXP || ((CArrayVarEntryInfo*)(d->var))->HasActiveElements()))
        {
            if (p->var->GetVarType() == VAR_REF_ARRAY_EXP && !complete) {
                *(CArrayVarEntryInfo*)(p->var) -= *(CArrayVarEntryInfo*)(d->var);
                prev = p;
            }
            else if (prev == NULL)
                list = list->next;
            else
            {
                prev->next = p->next;
                delete(p);
                p = prev;
            }
        }
        else
            prev = p;

        p = p->next;
    }
}

bool VarSet::RecordBelong(CVarEntryInfo* rec) 
{
    if (rec->GetVarType() != VAR_REF_RECORD_EXP)
        return false;
    CRecordVarEntryInfo* rrec = static_cast<CRecordVarEntryInfo*>(rec);
    CVarEntryInfo* lm = rrec->GetLeftmostParent();
    VarItem* p = list;
    while (p != NULL) {
        if (*lm == *(p->var->GetLeftmostParent()))
            return true;
        p = p->next;
    }
    return false;
}

void VarSet::minusFinalize(VarSet* set, bool complete)
{
    minus(set, complete);
    VarItem* p = list;
    VarItem* prev = NULL;
    while (p != NULL)
    {
        if (set->RecordBelong(p->var)) {
            {
                if (prev == NULL)
                    list = list->next;
                else
                {
                    prev->next = p->next;
                    delete(p);
                    p = prev;
                }
            }
        }
        else
            prev = p;

        p = p->next;
    }
}

unsigned int counter = 0;

CLAStatementItem::~CLAStatementItem()
{
#if __SPF
    removeFromCollection(this);
#endif
    if (next)
        delete next;
}

CLAStatementItem* CLAStatementItem::GetLast()
{
    if (next == NULL)
        return this;
    return next->GetLast();
}

void VarSet::unite(VarSet* set, bool la)
{
    VarItem* arg2 = set->list;
    while (arg2 != NULL)
    {
        VarItem* n = belongs(arg2->var);
        if (!n)
        {
            n = new VarItem();
            if (arg2->var->GetVarType() == VAR_REF_ARRAY_EXP)
                n->var = arg2->var->Clone();
            else {
                n->var = arg2->var;
                n->var->AddReference();
            }
            n->ov = arg2->ov;
            n->next = list;
            n->file_id = arg2->file_id;
#if PRIVATE_GET_LAST_ASSIGN
            if (la)
                n->lastAssignments = arg2->lastAssignments;
#endif
            list = n;
        }
        else {
#if PRIVATE_GET_LAST_ASSIGN
            if (la) {
                //n->lastAssignments.insert(n->lastAssignments.end(), arg2->lastAssignments.begin(), arg2->lastAssignments.end());
                //n->lastAssignments.splice(n->lastAssignments.end(), arg2->lastAssignments);
                //n->lastAssignments->GetLast()->next = arg2->lastAssignments;
                n->lastAssignments = arg2->lastAssignments;
            }
#endif
            //counter++;
            //if (counter % 100 == 0)
                //printf("%d!\n", counter);
            if (n->var->GetVarType() == VAR_REF_ARRAY_EXP) {
                *(CArrayVarEntryInfo*)(n->var) += *(CArrayVarEntryInfo*)(arg2->var);
            }
        }
        arg2 = arg2->next;
    }
}



void CBasicBlock::addToPrev(CBasicBlock* bb, bool for_jump_flag, bool c, ControlFlowItem* check)
{
    BasicBlockItem* n = new BasicBlockItem();
    n->block = bb;
    n->next = prev;
    n->for_jump_flag = for_jump_flag;
    n->cond_value = c;
    n->jmp = check;
    prev = n;
}

void CBasicBlock::addToSucc(CBasicBlock* bb, bool for_jump_flag, bool c, ControlFlowItem* check)
{
    BasicBlockItem* n = new BasicBlockItem();
    n->block = bb;
    n->for_jump_flag = for_jump_flag;
    n->next = succ;
    n->cond_value = c;
    n->jmp = check;
    succ = n;
}

#if ACCAN_DEBUG

void ControlFlowItem::printDebugInfo()
{
    if (jmp == NULL && stmt == NULL && func != NULL)
        printf("FUNCTION CALL: %s\n", func->unparse());
    if (jmp == NULL)
        if (stmt != NULL)
            if (label != NULL)
                printf("%d: %s %s %s lab %4d %s", stmtNo, this->isLeader() ? "L" : " ", this->IsParloopStart() ? "S" : " ", this->IsParloopEnd() ? "E" : " ", label->id(), stmt->unparse());
            else
                printf("%d: %s %s %s          %s", stmtNo, this->isLeader() ? "L" : " ", this->IsParloopStart() ? "S" : " ", this->IsParloopEnd() ? "E" : " ", stmt->unparse());
        else
            if (label != NULL)
                printf("%d: %s %s %s lab %4d \n", stmtNo, this->isLeader() ? "L" : " ", this->IsParloopStart() ? "S" : " ", this->IsParloopEnd() ? "E" : " ", label->id());
            else
                printf("%d: %s %s %s         \n", stmtNo, this->isLeader() ? "L" : " ", this->IsParloopStart() ? "S" : " ", this->IsParloopEnd() ? "E" : " ");
    else
        if (expr == NULL)
            if (label != NULL)
                printf("%d: %s %s %s lab %4d       goto %d\n", stmtNo, this->isLeader() ? "L" : " ", this->IsParloopStart() ? "S" : " ", this->IsParloopEnd() ? "E" : " ", label->id(), jmp->getStmtNo());
            else
                printf("%d: %s %s %s                goto %d\n", stmtNo, this->isLeader() ? "L" : " ", this->IsParloopStart() ? "S" : " ", this->IsParloopEnd() ? "E" : " ", jmp->getStmtNo());
        else
            if (label != NULL)
                printf("%d: %s %s %s lab %4d       if %s goto %d\n", stmtNo, this->isLeader() ? "L" : " ", this->IsParloopStart() ? "S" : " ", this->IsParloopEnd() ? "E" : " ", label->id(), expr->unparse(), jmp->getStmtNo());
            else
                printf("%d: %s %s %s                if %s goto %d\n", stmtNo, this->isLeader() ? "L" : " ", this->IsParloopStart() ? "S" : " ", this->IsParloopEnd() ? "E" : " ", expr->unparse(), jmp->getStmtNo());
}

static void printControlFlowList(ControlFlowItem* list, ControlFlowItem* last)
{

    printf("DEBUG PRINT START\n");
    unsigned int stmtNo = 0;
    ControlFlowItem* list_copy = list;
    while (list != NULL )
    {
        list->setStmtNo(++stmtNo);
        if (list == last)
            break;
        list = list->getNext();
    }

    list = list_copy;
    while (list != NULL)
    {
        list->printDebugInfo();
        if (list == last)
            break;
        list = list->getNext();
    }
    printf("DEBUG PRINT END\n\n");
}
#endif

void CallData::printControlFlows()
{
#if ACCAN_DEBUG
    AnalysedCallsList* l = calls_list;
    while (l != NULL) {
        if (!l->isIntrinsic && l->graph != NULL && l->header != NULL) {
            ControlFlowGraph* g = l->graph;
            SgStatement* h = l->header;
            printf("CFI for %s\n\n" ,h->symbol()->identifier());
            if (g != NULL) {
                printControlFlowList(g->getCFI());
            }
            else
                printf("ERROR: DOES NOT HAVE CFI\n");
        }
        l = l->next;
    }
#endif
}
