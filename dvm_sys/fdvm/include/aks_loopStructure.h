#pragma once
#include <vector>

class Access;
class Array;
class Loop;
class AllLoops;

class Access
{
public:
    Access(SgExpression *_exp, Array *_parent);
    ~Access() { }
    void setExp(char* _exp);
    void setExp(SgExpression *_exp);
    char* getExpChar();
    SgExpression* getExp();
    void incOperW();
    void incOperR();
    Array* getParentArray();
    void setParentArray(Array *_parent);
    std::vector<int>* getAlignOnLoop();

    void matchLoopIdxs(std::vector<SgSymbol*> &symbols);
private:
    char *expAcc;
    SgExpression *exp;
    int operation[2];
    Array *parentArray;
    std::vector<int> alignOnLoop;

    bool matchRecursion(SgExpression *_exp, SgSymbol *symb);
};

class Array
{
public:
    Array(int _dim, char *_name, Loop *_parent);
    Array(char *_name, Loop *_parent);
    ~Array() { }

    void setDimNum(int _num);
    int getDimNum();
    std::vector<int>* getAcrDims();
    std::vector<int>* getAlignOnLoop();
    void addTfmDim(int _dim);
    std::vector<int> *getTfmDims();
    void addAccess(Access* _newAccess);
    Access* getAccess(char* _expAcc);
    std::vector<Access*>* getAccesses();
    Loop* getParentLoop();
    void setParentLoop(Loop *_loop);
    void setArrayName(char* _name);
    char* getArrayName();
    int getAcrType();
    void setAcrType(int _type);
	std::vector<SgFunctionCallExp*>* getIfCals();
	std::vector<SgFunctionCallExp*>* getElseCals();
	std::vector<SgStatement*>* getZeroSt();
	std::vector<SgSymbol* >* getCoefInAccess();

    void analyzeAcrDims();
    void analyzeAlignOnLoop();
    void analyzeTrDims();
	 
	// diagTransform
	SgSymbol* findAccess(SgExpression *_exp, char *&_charEx);
	void addNewCoef(SgExpression *_exp, char *_charEx, SgSymbol *_symb);
	void generateAssigns(SgVarRefExp *offsetX, SgVarRefExp *offsetY, SgVarRefExp *Rx, SgVarRefExp *Ry, SgVarRefExp *slash);
private:
    int dimNum;
    int acrossType;
    std::vector<int> acrossDims;
    std::vector<int> alignOnLoop;
    std::vector<int> transformDims;
    std::vector<Access*> accesses;
    char *name;
    Loop *parentLoop;
	
	// diagTransform
	std::vector<SgSymbol* > coefInAccess;
	std::vector<SgExpression*> firstEx;
	std::vector<SgExpression*> secondEx;
	std::vector<char*> charEx;
	std::vector<SgStatement*> zeroSt;
	std::vector<SgFunctionCallExp*> ifCalls;
	std::vector<SgFunctionCallExp*> elseCalls;
};

class Loop
{
public:
	Loop(int _line, SgStatement *_body, bool withAnalyze);
    Loop(int _acrType, int _line, SgStatement *_body);
    Loop(int _line, SgStatement *_body);
    Loop(int _line);
    ~Loop() {};
    void setLine(int _line);
    int getLine();
    void setAcrType(int _type);
    int getAcrType();
    std::vector<Array*>* getArrays();
    void addArray(Array *_array);
    Array* getArray(char *name, int *_idx);
	Array* getArray(char *name);
    std::vector<SgSymbol*>* getSymbols();
	int getLoopDim();

    void analyzeLoopBody();
    void analyzeAcrossType();
    bool isArrayInPrivate(char *name);
    void unroll(int level); // TODO
private:
	int loopDim;
    std::vector<SgSymbol*> symbols;
    int line;
    int acrossType;
    std::vector<Array*> arrays;
    SgStatement* loopBody;
    std::vector<char*> privateList;
	std::vector<int> acrDims;	

    void analyzeAssignOp(SgExpression *_exp, int oper);	
};

// ---------------------------------------------------------------------- // AllLoops
class AllLoops
{
public:
    AllLoops() {}
    ~AllLoops() {}
    void addLoop(Loop *_loop)        { loops.push_back(_loop); }
    std::vector<Loop*>* getLoops()   { return &loops;          }
private:
    std::vector<Loop*> loops;
};
