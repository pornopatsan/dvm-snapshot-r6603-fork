#pragma once
#define ACCAN_DEBUG 0

#define USE_INTRINSIC_DVM_LIST 1

#define PRIVATE_ANALYSIS_NOT_CONDUCTED 650
#define PRIVATE_ANALYSIS_ADD_VAR 651
#define PRIVATE_ANALYSIS_REMOVE_VAR 652
#define PRIVATE_ANALYSIS_NO_RECURSION_ANALYSIS 653

#ifdef __SPF
extern "C" void printLowLevelWarnings(const char *fileName, const int line, const wchar_t* messageR, const char* messageE, const int group);
extern "C" void printLowLevelNote(const char *fileName, const int line, const wchar_t *messageR, const char *messageE, const int group);

extern "C" void addToCollection(const int line, const char *file, void *pointer, int type);
extern "C" void removeFromCollection(void *pointer);
bool IsPureProcedureACC(SgSymbol* s);
#endif

struct AnalysedCallsList;

class ControlFlowItem
{
    int stmtNo;
    SgLabel* label;
    ControlFlowItem *jmp;
    bool for_jump_flag;
    SgLabel* label_jump;
    ControlFlowItem* prev;
    ControlFlowItem *next;
    bool leader;
    int bbno;
    bool is_parloop_start;
    SgExpression* private_list;
    bool is_parloop_end;
    SgStatement* prl_stmt;
    AnalysedCallsList* call;
    SgFunctionCallExp* func;
    SgExpression* pPl;
    bool fPl;
    bool drawn;
    AnalysedCallsList* thisproc;
    //int refs;
    SgStatement* originalStatement;
    ControlFlowItem* conditionFriend;
    union
    {
        SgStatement *stmt;
        SgExpression *expr;
    };
public:
    inline ControlFlowItem(AnalysedCallsList* proc) : 
        stmtNo(-1), label(NULL), jmp(NULL), label_jump(NULL), next(NULL), leader(false), bbno(0), 
        stmt(NULL), is_parloop_start(false), is_parloop_end(false), private_list(NULL), call(NULL), 
        func(NULL), thisproc(proc), originalStatement(NULL), for_jump_flag(false), drawn(false),
        prev(NULL), conditionFriend(NULL)
    {
#if __SPF
        addToCollection(__LINE__, __FILE__, this, 3);
#endif
    }

    inline ControlFlowItem(SgStatement *s, ControlFlowItem *n, AnalysedCallsList* proc, AnalysedCallsList* c = NULL) : 
        stmtNo(-1), label(s ? s->label() : NULL), jmp(NULL), label_jump(NULL), next(n), leader(false), 
        bbno(0), stmt(s), is_parloop_start(false), is_parloop_end(false), private_list(NULL), 
        call(c), func(NULL), thisproc(proc), originalStatement(NULL), for_jump_flag(false), drawn(false),
        prev(NULL), conditionFriend(NULL)
    {
        if (n) {
            n->prev = this;
        }
#if __SPF
        addToCollection(__LINE__, __FILE__, this, 3);
#endif
    }

    inline ControlFlowItem(SgExpression *e, ControlFlowItem *j, ControlFlowItem *n, SgLabel* l, AnalysedCallsList* proc, bool fjf = false, AnalysedCallsList* c = NULL) :
        stmtNo(-1), label(l), jmp(j), label_jump(NULL), next(n), leader(false), bbno(0), 
        expr(e), is_parloop_start(false), is_parloop_end(false), private_list(NULL), call(c), 
        func(NULL), thisproc(proc), originalStatement(NULL), for_jump_flag(fjf), drawn(false),
        prev(NULL), conditionFriend(NULL)
    {
        if (n) {
            n->prev = this;
        }
#if __SPF
        addToCollection(__LINE__, __FILE__, this, 3);
#endif
    }

    inline ControlFlowItem(SgExpression *e, SgLabel* j, ControlFlowItem* n, SgLabel* l, AnalysedCallsList* proc, AnalysedCallsList* c = NULL) : 
        stmtNo(-1), label(l), jmp(NULL), label_jump(j), next(n), leader(false), bbno(0), 
        expr(e), is_parloop_start(false), is_parloop_end(false), private_list(NULL), call(c), 
        func(NULL), thisproc(proc), originalStatement(NULL), for_jump_flag(false), drawn(false),
        prev(NULL), conditionFriend(NULL)
    {
        if (n) {
            n->prev = this;
        }
#if __SPF
        addToCollection(__LINE__, __FILE__, this, 3);
#endif
    }

    inline void setOriginalStatement(SgStatement* s)
    { originalStatement = s; }

    inline SgStatement* getOriginalStatement()
    { return originalStatement; }

    inline bool isEnumerated()
    { return stmtNo >= 0; }

    inline void setBBno(int bb)
    { bbno = bb; }

    inline int getBBno()
    { return bbno; }

    inline void AddNextItem(ControlFlowItem *n)
    { next = n; if (n) n->prev = this; }

    inline void MakeParloopStart()
    { is_parloop_start = true; }

    inline void MakeParloopEnd()
    { is_parloop_end = true; }

    inline bool IsParloopStart()
    { return is_parloop_start; }

    inline bool IsParloopEnd()
    { return is_parloop_end; }

    inline bool isUnconditionalJump()
    { return ((jmp != NULL || label_jump != NULL) && expr == NULL); }

    inline SgStatement* getStatement()
    {
        if (jmp == NULL)
            return stmt;
        else
            return NULL;
    }

    inline SgExpression* getExpression()
    {
        if (jmp != NULL)
            return expr;
        else
            return NULL;
    }

    inline ControlFlowItem* getJump()
    { return jmp; }

    inline ControlFlowItem* getNext()
    { return next; }

    inline void setLeader()
    { leader = true; }

    inline unsigned int getStmtNo()
    { return stmtNo; }

    inline void setStmtNo(unsigned int no)
    { stmtNo = no; }

    inline int isLeader()
    { return leader; }

    inline void setLabel(SgLabel* l)
    { label = l; }

    inline SgLabel* getLabel()
    { return label; }

    inline void setLabelJump(SgLabel* l)
    { label_jump = l; }

    inline SgLabel* getLabelJump()
    { return label_jump; }

    inline void initJump(ControlFlowItem* item)
    { jmp = item; }

    inline void setPrivateList(SgExpression* p, SgStatement* s, SgExpression* m, bool mf)
    { private_list = p; prl_stmt = s; pPl = m; fPl = mf; }

    inline SgExpression* getPrivateList()
    { return private_list; }

    inline SgStatement* getPrivateListStatement()
    { return prl_stmt; }

    inline SgExpression* getExpressionToModifyPrivateList(bool* rhs)
    { if (rhs) *rhs = fPl; return pPl; }

    inline AnalysedCallsList* getCall()
    { return call; }

    inline void setFunctionCall(SgFunctionCallExp* f)
    { func = f; }

    inline SgFunctionCallExp* getFunctionCall()
    { return func; }

    inline AnalysedCallsList* getProc()
    { return thisproc; }

    inline bool IsForJumpFlagSet()
    { return for_jump_flag; }

    inline bool IsEmptyCFI()
    { return getStatement() == NULL; }

    inline void SetIsDrawn()
    { drawn = true; }

    inline void ResetDrawnStatus()
    { drawn = false; }

    inline bool IsDrawn()
    { return drawn; }

    inline ControlFlowItem* GetPrev()
    { return prev; }

    inline void SetConditionFriend(ControlFlowItem* f)
    { conditionFriend = f; }

    inline ControlFlowItem* GetFriend()
    { return conditionFriend; }

    int GetLineNumber();

#if ACCAN_DEBUG
    void printDebugInfo();

#endif
    ~ControlFlowItem()
    {
#if __SPF
        removeFromCollection(this);
#endif
    }
};

class doLoopItem
{
    int label;
    SgSymbol* name;
    ControlFlowItem* iter;
    ControlFlowItem* emptyAfter;
    bool current;
    doLoopItem* next;
    int parallel_depth;
    SgExpression* prl;
    SgExpression* pPl;
    bool plf;
    SgStatement* prs;
public:
    inline doLoopItem(int l, SgSymbol* s, ControlFlowItem* i, ControlFlowItem* e) : 
        label(l), name(s), iter(i), emptyAfter(e), current(true), next(NULL), parallel_depth(-1)
    {
#if __SPF
        addToCollection(__LINE__, __FILE__, this, 4);
#endif
    }

    ~doLoopItem()
    {
#if __SPF
        removeFromCollection(this);
#endif
    }
    inline void HandleNewItem(doLoopItem* it) { setParallelDepth(it->parallel_depth, it->prl, it->prs, it->pPl, it->plf); }
    inline void setNext(doLoopItem* n) { next = n; }
    inline void setNewLabel(int l) { label = l; }
    inline ControlFlowItem* getSourceForCycle() { return iter; }
    inline ControlFlowItem* getSourceForExit() { return emptyAfter; }
    inline SgSymbol* getName() { return name; }
    inline doLoopItem* getNext() { return next; }
    inline int getLabel() { return label; }

    inline void setParallelDepth(int k, SgExpression* pl, SgStatement* ps, SgExpression* pPl, bool plf) 
    { parallel_depth = k; prl = pl; prs = ps; this->pPl = pPl; this->plf = plf; }

    inline SgStatement* GetParallelStatement() { return prs; }
    inline bool isLastParallel()
    {
        if (parallel_depth > 0)
            return --parallel_depth == 0;
        return 0;
    }
    inline SgExpression* getPrivateList() { return prl; }
    inline SgExpression* getExpressionToModifyPrivateList(bool* rhs)  { if (rhs) *rhs = plf; return pPl; }
};

class doLoops
{
    doLoopItem* first;
    doLoopItem* current;
    
    doLoopItem* findLoop(SgSymbol*);
public:
    inline doLoops() : first(NULL), current(NULL)
    {
#if __SPF
        addToCollection(__LINE__, __FILE__, this, 5);
#endif
    }
    
    inline ControlFlowItem* getSourceForCycle() { return current ? current->getSourceForCycle() : NULL; }
    inline ControlFlowItem* getSourceForCycle(SgSymbol* loop) { return loop ? findLoop(loop)->getSourceForCycle() : getSourceForCycle(); }
    inline ControlFlowItem* getSourceForExit() { return current ? current->getSourceForExit() : NULL; }
    inline ControlFlowItem* getSourceForExit(SgSymbol* loop) { return loop ? findLoop(loop)->getSourceForExit() : getSourceForExit(); }
    inline void setParallelDepth(int k, SgExpression* pl, SgStatement* ps, SgExpression* pPl, bool plf) { current->setParallelDepth(k, pl, ps, pPl, plf); }
    inline SgStatement* GetParallelStatement() { return current->GetParallelStatement(); }    
    inline bool isLastParallel() { return current && current->isLastParallel(); }
    inline SgExpression* getPrivateList() { return current->getPrivateList(); }
    inline SgExpression* getExpressionToModifyPrivateList(bool* rhs) { return current->getExpressionToModifyPrivateList(rhs); }
    void addLoop(int l, SgSymbol* s, ControlFlowItem* i, ControlFlowItem* e);

    ControlFlowItem* endLoop(ControlFlowItem* last); 
    ControlFlowItem* checkStatementForLoopEnding(int label, ControlFlowItem* item);    
    ~doLoops();
};

struct LabelCFI
{
    int l;
    ControlFlowItem* item;

    LabelCFI() : item(NULL)
    {
#if __SPF
        addToCollection(__LINE__, __FILE__, this, 6);
#endif
    }
    ~LabelCFI()
    {
#if __SPF
        removeFromCollection(this);
#endif
    }
};

struct CLAStatementItem
{
    SgStatement* stmt;
    CLAStatementItem* next;

    ~CLAStatementItem();
    CLAStatementItem* GetLast();

    CLAStatementItem() : stmt(NULL), next(NULL)
    {
#if __SPF
        addToCollection(__LINE__, __FILE__, this, 7);
#endif
    }
};

enum eVariableType
{
    VAR_REF_VAR_EXP,
    VAR_REF_RECORD_EXP,
    VAR_REF_ARRAY_EXP
};


#define PRIVATE_GET_LAST_ASSIGN 0

class CVarEntryInfo;

struct VarItem
{
    CVarEntryInfo* var;
    CVarEntryInfo* ov;
    int file_id;
    //CLAStatementItem* lastAssignments;
#if PRIVATE_GET_LAST_ASSIGN
    std::list<SgStatement*> lastAssignments;
#endif
    VarItem* next;

    VarItem() : var(NULL), ov(NULL), next(NULL)
    {
#if __SPF
        addToCollection(__LINE__, __FILE__, this, 8);
#endif
    }
    ~VarItem()
    {
#if __SPF
        removeFromCollection(this);
#endif
    }
};

class CArrayVarEntryInfo;

class VarSet
{
    VarItem* list;
public:
    inline VarSet() : list(NULL)
    {
#if __SPF
        addToCollection(__LINE__, __FILE__, this, 9);
#endif
    }

    void addToSet(CVarEntryInfo*, SgStatement*, CVarEntryInfo* ov = NULL);
    void PossiblyAffectArrayEntry(CArrayVarEntryInfo*);
    void intersect(VarSet*, bool, bool);
    void unite(VarSet*, bool);
    void minus(VarSet*, bool complete = false);
    void minusFinalize(VarSet*, bool complete = false);
    bool RecordBelong(CVarEntryInfo*);
    void LeaveOnlyRecords();
    void RemoveDoubtfulCommonVars(AnalysedCallsList*);
    VarItem* GetArrayRef(CArrayVarEntryInfo*);
    //VarItem* belongs(SgVarRefExp*, bool os = false);
    VarItem* belongs(const CVarEntryInfo*, bool os = false);
    VarItem* belongs(SgSymbol*, bool os = false);
    bool equal(VarSet*);
    int count();
    void print();
    void remove(const CVarEntryInfo*);
    VarItem* getFirst();
    ~VarSet();
    inline bool isEmpty()
    {
        return list == NULL;
    }
};

struct DoLoopDataItem
{
    int file_id;
    SgStatement* statement;
    SgExpression* l;
    SgExpression* r;
    SgExpression* st;
    SgSymbol* loop_var;
    DoLoopDataItem* next;

    DoLoopDataItem() : l(NULL), r(NULL), st(NULL), loop_var(NULL), next(NULL), statement(NULL), file_id(-1)
    {
#if __SPF
        addToCollection(__LINE__, __FILE__, this, 10);
#endif
    }
    ~DoLoopDataItem()
    {
#if __SPF
        removeFromCollection(this);
#endif
    }
};

class DoLoopDataList 
{
    DoLoopDataItem* list;

public:
    DoLoopDataList() : list(NULL) 
    {
#if __SPF
        addToCollection(__LINE__, __FILE__, this, 11);
#endif
    }
    void AddLoop(int file_id, SgStatement* st, SgExpression* l, SgExpression* r, SgExpression* step, SgSymbol* lv);
    DoLoopDataItem* FindLoop(SgStatement* st);
    ~DoLoopDataList();
};

struct ArraySubscriptData;

class CVarEntryInfo
{
    SgSymbol* symbol;
    int references;

public:
    CVarEntryInfo(SgSymbol* s) : symbol(s), references(1) 
    {
#if __SPF
        addToCollection(__LINE__, __FILE__, this, 12);
#endif
    }

    virtual ~CVarEntryInfo() 
    {
#if __SPF
        removeFromCollection(this);
#endif
    }
    virtual eVariableType GetVarType() const = 0;
    virtual CVarEntryInfo* Clone(SgSymbol*) const = 0;
    virtual CVarEntryInfo* Clone() const = 0;
    virtual CVarEntryInfo* GetLeftmostParent() = 0;
    virtual void RegisterUsage(VarSet* def, VarSet* use, SgStatement* st) = 0;
    virtual void RegisterDefinition(VarSet* def, VarSet* use, SgStatement* st) = 0;
    SgSymbol* GetSymbol() const { return symbol; }
    virtual bool operator==(const CVarEntryInfo& rhs) const = 0;
    void AddReference() { references++; }
    bool RemoveReference() { return --references == 0; }
    void SwitchSymbol(SgSymbol* s) { symbol = s; }
};

class CScalarVarEntryInfo: public CVarEntryInfo
{
public:
    CScalarVarEntryInfo(SgSymbol* s) : CVarEntryInfo(s) 
    {
#if __SPF
        addToCollection(__LINE__, __FILE__, this, 13);
#endif
    }
    ~CScalarVarEntryInfo() 
    {
#if __SPF
        removeFromCollection(this);
#endif
    }
    eVariableType GetVarType() const { return VAR_REF_VAR_EXP; }
    CVarEntryInfo* Clone(SgSymbol* s) const { return new CScalarVarEntryInfo(s); }
    CVarEntryInfo* Clone() const { return new CScalarVarEntryInfo(GetSymbol()); }
    bool operator==(const CVarEntryInfo& rhs) const { return rhs.GetVarType() == VAR_REF_VAR_EXP && rhs.GetSymbol() == GetSymbol(); }
    CVarEntryInfo* GetLeftmostParent() { return this; }
    void RegisterUsage(VarSet* def, VarSet* use, SgStatement* st)
    {
        if (def == NULL || !def->belongs(this))
            use->addToSet(this, st);
    }
    void RegisterDefinition(VarSet* def, VarSet* use, SgStatement* st) { def->addToSet(this, st); }
};

class CRecordVarEntryInfo : public CVarEntryInfo
{
    CVarEntryInfo* parent;
public:
    CRecordVarEntryInfo(SgSymbol* s, CVarEntryInfo* ptr) : CVarEntryInfo(s), parent(ptr) 
    {
#if __SPF
        addToCollection(__LINE__, __FILE__, this, 14);
#endif
    }

    ~CRecordVarEntryInfo()
    {
#if __SPF
        removeFromCollection(this);
        return;
#endif
        if (parent->RemoveReference())
            delete parent;
    }

    eVariableType GetVarType() const { return VAR_REF_RECORD_EXP; }
    CVarEntryInfo* Clone(SgSymbol* s) const { return new CRecordVarEntryInfo(s, parent->Clone()); }
    CVarEntryInfo* Clone() const { return new CRecordVarEntryInfo(GetSymbol(), parent->Clone()); }
    bool operator==(const CVarEntryInfo& rhs) const { return rhs.GetVarType() == VAR_REF_RECORD_EXP && rhs.GetSymbol() == GetSymbol() &&
        parent && static_cast<const CRecordVarEntryInfo&>(rhs).parent && *static_cast<const CRecordVarEntryInfo&>(rhs).parent == *parent; }
    CVarEntryInfo* GetLeftmostParent() { return parent->GetLeftmostParent(); }
    void RegisterUsage(VarSet* def, VarSet* use, SgStatement* st) 
    {
        if (def == NULL || !def->belongs(this))
            use->addToSet(this, st);
    }
    void RegisterDefinition(VarSet* def, VarSet* use, SgStatement* st) { def->addToSet(this, st); }
};

struct ArraySubscriptData
{
    bool defined;
    int bound_modifiers[2];
    int step;
    int coefs[2];
    DoLoopDataItem *loop;
    SgExpression *left_bound;
    SgExpression *right_bound;

    ArraySubscriptData() : loop(NULL), left_bound(NULL), right_bound(NULL)
    {
#if __SPF
        addToCollection(__LINE__, __FILE__, this, 15);
#endif
    }
    ~ArraySubscriptData()
    {
#if __SPF
        removeFromCollection(this);
#endif
    }
};

class CArrayVarEntryInfo : public CVarEntryInfo
{
    int subscripts;
    bool disabled;
    std::vector<ArraySubscriptData> data;
public:
    CArrayVarEntryInfo(SgSymbol* s, SgArrayRefExp* r);
    CArrayVarEntryInfo(SgSymbol* s, int sub, int ds, const std::vector<ArraySubscriptData> &d);
    ~CArrayVarEntryInfo() 
    {
#if __SPF
        removeFromCollection(this);
#endif
    } 

    CVarEntryInfo* Clone(SgSymbol* s) const { return new CArrayVarEntryInfo(s, subscripts, disabled, data); }
    CVarEntryInfo* Clone() const { return new CArrayVarEntryInfo(GetSymbol(), subscripts, disabled, data); }
    bool operator==(const CVarEntryInfo& rhs) const { return rhs.GetVarType() == VAR_REF_ARRAY_EXP && rhs.GetSymbol() == GetSymbol(); }
    friend CArrayVarEntryInfo* operator-(const CArrayVarEntryInfo&, const CArrayVarEntryInfo&);
    friend CArrayVarEntryInfo* operator+(const CArrayVarEntryInfo&, const CArrayVarEntryInfo&);
    CArrayVarEntryInfo& operator+=(const CArrayVarEntryInfo&);
    CArrayVarEntryInfo& operator-=(const CArrayVarEntryInfo&);
    CArrayVarEntryInfo& operator*=(const CArrayVarEntryInfo&);
    eVariableType GetVarType() const { return VAR_REF_ARRAY_EXP; }
    CVarEntryInfo* GetLeftmostParent() { return this; }
    void RegisterUsage(VarSet* def, VarSet* use, SgStatement* st);
    void RegisterDefinition(VarSet* def, VarSet* use, SgStatement* st);
    bool HasActiveElements() const;
    void MakeInactive();
    void ProcessChangesToUsedEntry(CArrayVarEntryInfo*);
};

class CBasicBlock;
class ControlFlowGraph;
struct AnalysedCallsList;
class PrivateDelayedItem;

struct BasicBlockItem
{
    CBasicBlock* block;
    bool for_jump_flag;
    bool cond_value;
    bool drawn;
    ControlFlowItem* jmp;
    BasicBlockItem* next;

    BasicBlockItem() : drawn(false)
    {
#if __SPF
        addToCollection(__LINE__, __FILE__, this, 17);
#endif
    }
    ~BasicBlockItem()
    {
#if __SPF
        removeFromCollection(this);
#endif
    }
};

struct CommonVarSet;
struct ActualDelayedData;

struct CallAnalysisLog
{
    AnalysedCallsList* el;
    int depth;
    CallAnalysisLog* prev;

    CallAnalysisLog() : el(NULL), prev(NULL)
    {
#if __SPF
        addToCollection(__LINE__, __FILE__, this, 18);
#endif
    }
    ~CallAnalysisLog()
    {
#if __SPF
        removeFromCollection(this);
#endif
    }
};

struct CExprList
{
    SgExpression* entry;
    CExprList* next;

    CExprList() : entry(NULL), next(NULL)
    {
#if __SPF
        addToCollection(__LINE__, __FILE__, this, 19);
#endif
    }
    ~CExprList()
    {
#if __SPF
        removeFromCollection(this);
#endif
    }
};

#ifdef __SPF
class SymbolKey 
{
private:
    SgSymbol *var;
    std::string varName;
    bool pointer;

public:
    SymbolKey(SgSymbol *v): var(v), varName(v->identifier()), pointer(false) 
    {
#if __SPF
        addToCollection(__LINE__, __FILE__, this, 20);
#endif
    }
    SymbolKey(SgSymbol *v, bool isPointer): var(v), varName(v->identifier()), pointer(isPointer) 
    {
#if __SPF
        addToCollection(__LINE__, __FILE__, this, 20);
#endif
    }
    ~SymbolKey()
    {
#if __SPF
        removeFromCollection(this);
#endif
    }
    inline const std::string& getVarName() const { return varName; }
    inline const SgSymbol* getSymbol() const { return var; }
    inline bool isPointer() const { return pointer; }
    inline bool operator<(const SymbolKey &rhs) const   { return varName < rhs.varName; }
    inline bool operator==(const SymbolKey &rhs) const  { return varName == rhs.varName; }
    inline bool operator==(SgSymbol *rhs) const         { return strcmp(varName.c_str(), rhs->identifier()) == 0; }
};

class ExpressionValue {
private:
    SgExpression *exp;
    std::string unparsed;
    SgStatement* from;
public:
    ExpressionValue(): exp(NULL), unparsed(""), from(NULL) {}
    //ExpressionValue(SgExpression *e): exp(e) { unparsed = (e != NULL ? e->unparse() : ""); }
    ExpressionValue(SgExpression *e, const std::string &unp) : exp(e), unparsed(unp), from(NULL) { }
    ExpressionValue(SgExpression *e, const std::string &unp, SgStatement* f) : exp(e), unparsed(unp), from(f) { }
    inline bool unparsedEquals(ExpressionValue &other) const {return unparsed == other.unparsed; }
    inline bool unparsedEquals(ExpressionValue *other) const {return unparsed == other->unparsed; }
    inline SgExpression* getExp() const { return exp; }
    inline const std::string& getUnparsed() const { return unparsed; }
    inline void setFrom(SgStatement* st) { from = st; }
    inline SgStatement* getFrom() const { return from; }
    inline bool operator<(const ExpressionValue &other) const   { return from == other.from ? unparsed < other.unparsed : from < other.from; }
    inline bool operator==(const ExpressionValue &other) const  { return from == other.from && unparsed == other.unparsed; }
    inline bool operator==(SgExpression* e) const               { return strcmp(unparsed.c_str(), e->unparse()) == 0; }
};
#endif

class CBasicBlock
{
    int num;
    ControlFlowGraph* parent;
    ControlFlowItem* start;
    AnalysedCallsList* proc;
    BasicBlockItem* prev;
    BasicBlockItem* succ;
    CBasicBlock* lexNext;
    CBasicBlock* lexPrev;
    VarSet* def;
    VarSet* use;
    VarSet* old_mrd_out;
    VarSet* old_mrd_in;
    VarSet* mrd_in;
    VarSet* mrd_out;
    VarSet* old_lv_out;
    VarSet* old_lv_in;
    VarSet* lv_in;
    VarSet* lv_out;
    CommonVarSet* common_def;
    CommonVarSet* common_use;
    bool undef;
    bool lv_undef;
    void setDefAndUse();
    char prev_status;
    bool temp;
    void addExprToUse(SgExpression* e, CArrayVarEntryInfo*, CExprList*);
    void AddOneExpressionToUse(SgExpression*, SgStatement*, CArrayVarEntryInfo*);
    void AddOneExpressionToDef(SgExpression*, SgStatement*, CArrayVarEntryInfo*);
    PrivateDelayedItem* privdata;
    CVarEntryInfo* findentity;
    std::string visname;
    std::string visunparse;
#ifdef __SPF
    bool varIsPointer(SgSymbol* symbol);
    void processAssignThroughPointer(SgSymbol *symbol, SgExpression *right, SgStatement *st);
    void processPointerAssignment(SgSymbol *symbol, SgExpression *right, SgStatement *st);
    void processReadStat(SgStatement* readSt);
    void addVarUnknownToGen(SymbolKey var, SgStatement *defSt);
    
    std::map <SymbolKey, std::set<SgExpression*>> gen_p;
    std::set <SymbolKey> kill_p;
    std::map <SymbolKey, std::set<ExpressionValue*>> in_defs_p;
    std::map <SymbolKey, std::set<ExpressionValue*>> out_defs_p;

    std::map <SymbolKey, ExpressionValue*> gen;
    std::set <SymbolKey> kill;
    std::map <SymbolKey, std::set<ExpressionValue*>> in_defs;
    std::map <SymbolKey, std::set<ExpressionValue*>> out_defs;

    std::set <ExpressionValue*> e_gen;
    std::set <ExpressionValue*> e_in;
    std::set <ExpressionValue*> e_out;
#endif

public:
    inline CBasicBlock(bool t, ControlFlowItem* st, int n, ControlFlowGraph* par, AnalysedCallsList* pr) : 
        temp(t), num(n), start(st), prev(NULL), lexNext(NULL), def(NULL), use(NULL), mrd_in(new VarSet()), mrd_out(new VarSet()), undef(true),
        lv_in(new VarSet()), lv_out(new VarSet()), lv_undef(false), succ(NULL), lexPrev(NULL), prev_status(-1), parent(par), common_def (NULL), 
        common_use(NULL), old_mrd_in(NULL), old_mrd_out(NULL), old_lv_in(NULL), old_lv_out(NULL), privdata(NULL), findentity(NULL), proc(pr)
    {
#if __SPF
        addToCollection(__LINE__, __FILE__, this, 21);
#endif
    }

    ~CBasicBlock();
    inline CommonVarSet* getCommonDef() { return common_def; }
    inline CommonVarSet* getCommonUse() { return common_use; }
    inline void setNext(CBasicBlock* next) { lexNext = next; }
    inline void setPrev(CBasicBlock* prev) { lexPrev = prev; }

    void addToPrev(CBasicBlock* pr, bool, bool, ControlFlowItem*);
    void addToSucc(CBasicBlock* su, bool, bool, ControlFlowItem*);
    VarSet* getDef();
    VarSet* getUse();
    VarSet* getMrdIn(bool);
    VarSet* getMrdOut(bool);
    VarSet* getLVIn();
    VarSet* getLVOut();
    bool stepMrdIn(bool);
    bool stepMrdOut(bool);
    bool stepLVIn();
    bool stepLVOut();

    ControlFlowItem* containsParloopStart();
    ControlFlowItem* containsParloopEnd();

    ControlFlowItem* getStart();
    ControlFlowItem* getEnd();

    inline CBasicBlock* getLexNext() { return lexNext; }
    inline CBasicBlock* getLexPrev() { return lexPrev; }
    inline BasicBlockItem* getPrev() { return prev; }
    inline BasicBlockItem* getSucc() { return succ; }
    inline VarSet* getLiveIn() { return lv_in; }
    inline int getNum() { return num; }
    inline void SetDelayedData(PrivateDelayedItem* p) { privdata = p; }
    inline PrivateDelayedItem* GetDelayedData() { return privdata; }

    void print();
    void markAsReached();
    bool hasPrev();

    void ProcessProcedureHeader(bool, SgProcHedrStmt*, void*, const char*);
    void ProcessIntrinsicProcedure(bool, int narg, void* f, const char*);
    void ProcessUserProcedure(bool isFun, void* call, AnalysedCallsList* c);
    void ProcessProcedureWithoutBody(bool, void*, bool);
    //SgExpression* GetProcedureArgument(bool, void*, int);
    //int GetNumberOfArguments(bool, void*);
    SgSymbol* GetProcedureName(bool, void*);
    void PrivateAnalysisForAllCalls();
    ActualDelayedData* GetDelayedDataForCall(CallAnalysisLog*);
    bool IsVarDefinedAfterThisBlock(CVarEntryInfo*, bool);
    bool ShouldThisBlockBeCheckedAgain(CVarEntryInfo* var) { return findentity && var && *var == *findentity; }

    std::string GetGraphVisDescription();
    std::string GetGraphVisData();
    bool IsEmptyBlock();
    std::string GetEdgesForBlock(std::string name, bool original, std::string);

    void addUniqObjects(std::set<ControlFlowItem*> &pointers) const
    {
        for (ControlFlowItem *it = start; it != NULL; it = it->getNext())
            pointers.insert(it);
    }

#ifdef __SPF
    AnalysedCallsList* getProc() { return proc; }
    void clearGenKill() { gen.clear(); kill.clear(); e_gen.clear(); }
    void clearGenKillPointers() { gen_p.clear(); kill_p.clear(); }
    void clearDefs() { in_defs.clear(); out_defs.clear(); e_in.clear(); e_out.clear(); }
    void clearDefsPointers() { in_defs_p.clear(); out_defs_p.clear(); }
    void addVarToGen(SymbolKey var, SgExpression* value, SgStatement *defSt);    
    void addVarToKill(const SymbolKey &key);
    void checkFuncAndProcCalls(ControlFlowItem* cfi);
    void adjustGenAndKill(ControlFlowItem* cfi);
    void adjustGenAndKillP(ControlFlowItem* cfi);
    std::set<SymbolKey>* getOutVars();
    void correctInDefsSimple();
    bool correctInDefsIterative();
    bool expressionIsAvailable(ExpressionValue* expValue);
    //const std::map<SymbolKey, std::set<SgExpression*>> getReachedDefinitions(SgStatement* stmt);
    const std::map<SymbolKey, std::set<ExpressionValue*>> getReachedDefinitionsExt(SgStatement* stmt);
    void initializeOut();
    void initializeEOut(std::set<ExpressionValue*>& allEDefs);
    bool updateEDefs();    

    inline std::map<SymbolKey, std::set<SgExpression*>>* getGenP() { return &gen_p; }
    inline std::set<SymbolKey>* getKillP() { return &kill_p; }
    inline void setInDefsP(std::map<SymbolKey, std::set<ExpressionValue*>>* inDefsP) { in_defs_p = *inDefsP; }
    inline std::map<SymbolKey, std::set<ExpressionValue*>>* getInDefsP() { return &in_defs_p; }
    inline std::map<SymbolKey, std::set<ExpressionValue*>>* getOutDefsP() { return &out_defs_p; }

    inline std::map<SymbolKey, ExpressionValue*>* getGen() { return &gen; }
    inline std::set<SymbolKey>* getKill() { return &kill; }
    inline void setInDefs(std::map<SymbolKey, std::set<ExpressionValue*>>* inDefs) { in_defs = *inDefs; }
    inline std::map<SymbolKey, std::set<ExpressionValue*>>* getInDefs() { return &in_defs; }
    inline std::map<SymbolKey, std::set<ExpressionValue*>>* getOutDefs() { return &out_defs; }

    inline std::set<ExpressionValue*>* getEGen() { return &e_gen; }
    inline std::set<ExpressionValue*>* getEIn() { return &e_in; }
    inline std::set<ExpressionValue*>* getEOut() { return &e_out; }

#endif
};

struct CommonVarInfo;
struct CommonVarSet
{
    CommonVarInfo* cvd;
    CommonVarSet* next;

    CommonVarSet(const CommonVarSet&);
    CommonVarSet() : cvd(NULL), next(NULL)
    {
#if __SPF
        addToCollection(__LINE__, __FILE__, this, 22);
#endif
    }
    ~CommonVarSet()
    {
#if __SPF
        removeFromCollection(this);
#endif
    }
};

struct ActualDelayedData;
class CommonData;

class CallData;

class ControlFlowGraph
{
    CBasicBlock* last;
    CBasicBlock* first;
    VarSet* def;
    VarSet* use;
    VarSet* pri;
    CommonVarSet* common_def;
    bool cdf;
    CommonVarSet* common_use;
    bool cuf;
    bool temp;
    bool main;
    int refs;
    bool hasBeenAnalyzed;
    void liveAnalysis();
#ifdef __SPF
    std::set<SymbolKey> pointers;
#endif
public:
    ControlFlowGraph(bool temp, bool main, ControlFlowItem* item, ControlFlowItem* end);
    ~ControlFlowGraph();
    VarSet* getPrivate();
    VarSet* getUse();
    VarSet* getDef();
    void privateAnalyzer();
    bool ProcessOneParallelLoop(ControlFlowItem* lstart, CBasicBlock* of, CBasicBlock*& p, bool);
    ActualDelayedData* ProcessDelayedPrivates(CommonData*, AnalysedCallsList*, CallAnalysisLog*, void*, bool, int);
    bool IsMain() { return main; } // change to refs
    void AddRef() { refs++; }
    bool RemoveRef() { return refs == 0; }
    ControlFlowItem* getCFI() { return first->getStart(); }
    CommonVarSet* getCommonDef() { return common_def; }
    CommonVarSet* getCommonUse() { return common_use; }
    inline CBasicBlock* getFirst() { return first; }
    inline CBasicBlock* getLast() { return last; }

    std::string GetVisualGraph(CallData*);
    void ResetDrawnStatusForAllItems();

    inline void addCFItoCollection(std::set<ControlFlowItem*> &collection) const
    {
        for (CBasicBlock *bb = first; bb != NULL; bb = bb->getLexNext())
            bb->addUniqObjects(collection);        
    }
#ifdef __SPF
    std::set<SymbolKey>* getPointers() { return &pointers; };
#endif
};

struct AnalysedCallsList 
{
    SgStatement* header;
    ControlFlowGraph* graph;
    bool isIntrinsic;
    bool isPure;
    bool isFunction;
    AnalysedCallsList* next;
    bool hasBeenAnalysed;
    bool isCurrent;
    int file_id;

    AnalysedCallsList(SgStatement* h, bool intr, bool pure, bool fun, const char* name, int fid) :
        header(h), isIntrinsic(intr), isPure(pure), isFunction(fun), hasBeenAnalysed(false), graph(NULL), funName(name), file_id(fid), next(NULL)
    {
#if __SPF
        addToCollection(__LINE__, __FILE__, this, 23);
#endif
    }
    ~AnalysedCallsList()
    {
#if __SPF
        removeFromCollection(this);
#endif
    }
    bool isArgIn(int num, CArrayVarEntryInfo**);
    bool isArgOut(int num, CArrayVarEntryInfo**);
    const char* funName;
    bool IsIntrinsic() { return isIntrinsic; }
};

class CommonData;
class CallData
{
    AnalysedCallsList* calls_list;
    bool recursion_flag;
public:
    AnalysedCallsList* getLinkToCall(SgExpression*, SgStatement*, CommonData*);
    AnalysedCallsList* AddHeader(SgStatement*, bool isFun, SgSymbol* name, int fid);
    void AssociateGraphWithHeader(SgStatement*, ControlFlowGraph*);
    AnalysedCallsList* IsHeaderInList(SgStatement*);
    AnalysedCallsList* GetDataForGraph(ControlFlowGraph*);
    CallData() 
    {
        recursion_flag = false; 
        calls_list = NULL; 
#if __SPF
        addToCollection(__LINE__, __FILE__, this, 24);
#endif
    }

    void printControlFlows();
    ~CallData();
};

struct CommonDataItem;
struct CommonVarInfo
{
    CVarEntryInfo* var;
    bool isPendingLastPrivate;
    bool isInUse;
    CommonDataItem* parent;
    CommonVarInfo* next;

    CommonVarInfo() : var(NULL), parent(NULL), next(NULL)
    {
#if __SPF
        addToCollection(__LINE__, __FILE__, this, 25);
#endif
    }
    ~CommonVarInfo()
    {
#if __SPF
        removeFromCollection(this);
#endif
    }
};

struct CommonDataItem
{
    SgStatement *cb;
    std::vector<SgExpression*> commonRefs;
    bool isUsable;
    bool onlyScalars;
    std::string name;
    AnalysedCallsList* proc;
    AnalysedCallsList* first;
    CommonVarInfo* info;
    CommonDataItem* next;

    CommonDataItem() : cb(NULL), proc(NULL), first(NULL), info(NULL), next(NULL)
    {
#if __SPF
        addToCollection(__LINE__, __FILE__, this, 26);
#endif
    }

    ~CommonDataItem()
    {
#if __SPF
        removeFromCollection(this);
#endif
    }
};

class CommonData
{
    CommonDataItem* list;
public:
    CommonDataItem* getList() { return list; }
    void RegisterCommonBlock(SgStatement*, AnalysedCallsList*);
    void MarkEndOfCommon(AnalysedCallsList*);
    void MarkAsUsed(VarSet*, AnalysedCallsList*);
    bool CanHaveNonScalarVars(CommonDataItem*);
    //void ProcessDelayedPrivates(PrivateDelayedItem*);
    CommonDataItem* IsThisCommonUsedInProcedure(CommonDataItem*, AnalysedCallsList*);
    CommonDataItem* GetItemForName(const std::string&, AnalysedCallsList*);
    CommonVarSet* GetCommonsForVarSet(VarSet*, AnalysedCallsList*);
    CommonDataItem* IsThisCommonVar(VarItem*, AnalysedCallsList*);
    CommonData() : list(NULL) 
    {
#if __SPF
        addToCollection(__LINE__, __FILE__, this, 27);
#endif
    }
    ~CommonData();
};

class PrivateDelayedItem
{
    VarSet* detected;
    VarSet* original;
    VarSet* lp;
    VarSet* delay;
    ControlFlowItem* lstart;
    ControlFlowGraph* graph;
    PrivateDelayedItem* next;
    int file_id;
public:
    PrivateDelayedItem(VarSet* d, VarSet* o, VarSet* p, ControlFlowItem* l, PrivateDelayedItem* n, ControlFlowGraph* g, VarSet* dl, int fd) : 
        detected(d), original(o), lp(p), lstart(l), next(n), graph(g), delay(dl), file_id(fd) 
    {
#if __SPF
        addToCollection(__LINE__, __FILE__, this, 28);
#endif
    }
    ~PrivateDelayedItem();
    void PrintWarnings();
    void MoveFromPrivateToLastPrivate(CVarEntryInfo*);
    VarSet* getDetected() { return detected; }
    VarSet* getDelayed() { return delay; }
};

#define MAX_ARGS_FOR_INTRINSIC 10
#define INTRINSIC_IN 1
#define INTRINSIC_OUT 2

struct IntrinsicParameterData
{
    int index;
    const char* name;
    unsigned char status;
};

struct IntrinsicSubroutineData
{
    const char* name;
    int args;
    IntrinsicParameterData parameters[MAX_ARGS_FOR_INTRINSIC];
};

struct ActualDelayedData
{
    PrivateDelayedItem* original;
    CommonVarSet* commons;
    VarSet* buse;
    ActualDelayedData* next;
    AnalysedCallsList* call;

    void MoveVarFromPrivateToLastPrivate(CVarEntryInfo*, CommonVarSet*, VarSet*);
    void RemoveVarFromCommonList(CommonVarSet*);

    ActualDelayedData() : original(NULL), commons(NULL), next(NULL), call(NULL)
    {
#if __SPF
        addToCollection(__LINE__, __FILE__, this, 29);
#endif
    }
    ~ActualDelayedData()
    {
#if __SPF
        removeFromCollection(this);
#endif
    }
};

ControlFlowGraph* GetControlFlowGraphWithCalls(bool, SgStatement*, CallData*, CommonData*);
void FillCFGSets(ControlFlowGraph*);
void SetUpVars(CommonData*, CallData*, AnalysedCallsList*, DoLoopDataList*);
AnalysedCallsList* GetCurrentProcedure();
int SwitchFile(int);

#if __SPF
ExpressionValue* allocateExpressionValue(SgExpression*, SgStatement*);
void deleteAllocatedExpressionValues(int file_id);
#endif
