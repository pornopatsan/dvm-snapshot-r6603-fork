#pragma once


struct BasicBlock
{
    int index;
    SgStatement* head;
    SgStatement* tail;
    std::vector<int> in, out;// blocks this block takes control from / passes control to
    std::set<SgStatement*> INrd, OUTrd, INae, OUTae;// reaching definitions and extended available expressions
    std::map<SgExpression*, std::set<SgExpression*> > INss;// safe substitutes (expression propagation)
    BasicBlock() : index(-1), head(NULL), tail(NULL) {}
};

class Access;
class Array;
class Loop;

class Access
{
private:
    int blockIndex;
    SgExpression* expr;
    std::string str;
    Array* array;
    int* alignment;

public:
    int getBlockIndex() const { return blockIndex; }
    SgExpression* getSubscripts() { return expr; }
    int* getAlignment() const { return alignment; }

    Access(SgExpression* subscripts, std::string s, Array* array, int blockIndex) : expr(subscripts), str(s), array(array), blockIndex(blockIndex), alignment(NULL) {}

    void analyze();
    void getReferences(SgExpression* expr, std::set<SgExpression*>& references, std::map<SgExpression*, std::string>& unparsedRefs, std::map<std::string, SgExpression*>& refs) const;

    ~Access() { delete [] alignment; }
};

struct TfmInfo
{
    std::vector<int> transformDims;
    std::vector<std::pair<std::string, std::string> > exprs;
    std::vector<SgSymbol*> coefficients;
    std::vector<SgExpression*> first;
    std::vector<SgExpression*> second;
    std::vector<SgStatement*> zeroSt;
    std::map<SgStatement*, std::vector<SgFunctionCallExp*> > ifCalls;
    std::map<SgStatement*, std::vector<SgFunctionCallExp*> > elseCalls;
};

class Array
{
private:
    SgSymbol* symbol;
    int dimension;
    Loop* loop;
    std::map<std::string, Access*> accesses;
    int* alignment;
    std::vector<int> acrossDims;
    int acrossType;

    TfmInfo tfmInfo;

public:
    SgSymbol* getSymbol() const { return symbol; }
    Loop* getLoop() const { return loop; }
    int getDimension() const { return dimension; }
    int getAcrossType() const { return acrossType; }
    void setAcrossType(const int acrossType) { this->acrossType = acrossType; }
    std::vector<int>& getAcrossDims() { return acrossDims; }    
    int* getAlignment() const { return alignment; }
    std::map<std::string, Access*>& getAccesses() { return accesses; }
    TfmInfo& getTfmInfo() { return tfmInfo; }

    Array(SgSymbol* symbol, int dimension, Loop* loop) : symbol(symbol), dimension(dimension), loop(loop), alignment(NULL), acrossDims(dimension, -1), acrossType(0) {}

    void analyze();
    void analyzeTransformDimensions();
    SgSymbol* findAccess(SgExpression* subscripts, std::string& expr);
    void addCoefficient(SgExpression* subscripts, std::string& expr, SgSymbol* symbol);
    void generateAssigns(SgVarRefExp* offsetX, SgVarRefExp* offsetY, SgVarRefExp* Rx, SgVarRefExp* Ry, SgVarRefExp* slash);

    ~Array()
    {
        delete [] alignment;
        for (std::map<std::string, Access*>::iterator it = accesses.begin(); it != accesses.end(); ++it)
            delete it->second;
    }
};

class Loop
{
private:
    bool enable_opt;
    bool irregular_acc_opt;
    bool do_irreg_opt;
    std::vector<BasicBlock> blocks;
    std::map<SgStatement*, int> blockIn;

    enum { ENTRY, EXIT };

    SgStatement* loop_body;
    int dimension;
    std::map<SgSymbol*, Array*> arrays;
    int* acrossDims;
    int acrossType;
    std::vector<SgSymbol*> symbols;
    std::set<SgSymbol*> privateList;

    bool IsTargetable(SgStatement* stmt) const;
    void analyzeAcrossClause();
    void analyzeAcrossType();
    void analyzeAssignments(int blockIndex, SgStatement* stmt);
    void buildCFG();
    void setupSubstitutes();
    void analyzeAssignments(SgExpression* ex, const int blockIndex);
    void analyzeInderectAccess();

public:
    const std::vector<BasicBlock>& getBlocks() const { return blocks; }
    const std::map<SgStatement*, int>& getBlockIn() const { return blockIn; }
    const std::vector<SgSymbol*>& getSymbols() const { return symbols; }
    int getDimension() const { return dimension; }
    int getAcrossType() const { return acrossType; }
    std::map<SgSymbol*, Array*>& getArrays() { return arrays; }
    std::set<SgSymbol*>& getPrivateList() { return privateList; }

    Loop(SgStatement* loop_body, bool enable_opt, bool irreg_access = false);

    // only for RD/AE analyses, which can be later performed only on statements in loop_body
    // usage: Loop* loop = new Loop(loop_body_stmt); RDs = loop->RDsAt(stmtI); AEs = loop->AEsAt(stmtJ);
    // note: only one loop object can exist at a time (lhs, rhs, unparsedLhs, unparsedRhs are global for all methods for simplicity,
    // probably can be added to Loop class and transferred to Array/Access by pointer/reference, and this constraint will be solved)
    Loop(SgStatement* loop_body);
    // get defining statements which reach stmt
    std::set<SgStatement*> RDsAt(SgStatement* stmt) const;
    // get statements, rhss of which are available at stmt
    std::set<SgStatement*> AEsAt(SgStatement* stmt) const;

    void getRPN(SgExpression* expr, std::list<SgExpression*>& rpn) const;
    void unrollRPN(std::list<SgExpression*>& rpn, std::map<SgExpression*, int>& arity) const;
    void optimizeRPN(std::list<SgExpression*>& rpn, std::map<SgExpression*, int>& arity, bool unrolled) const;
    SgExpression* simplify(SgExpression* expr) const;
    void visualize(const char* scriptName) const;

    bool irregularAnalysisIsOn() const;
    ~Loop()
    {
        delete [] acrossDims;
        for (std::map<SgSymbol*, Array*>::iterator it = arrays.begin(); it != arrays.end(); ++it)
            delete it->second;
    }
};

