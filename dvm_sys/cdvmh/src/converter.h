#pragma once

#include <clang/Basic/Version.h>
#include <clang/AST/ParentMapContext.h>
#include <clang/AST/ASTTypeTraits.h>
#include <clang/AST/ASTConsumer.h>
#include <clang/AST/ASTContext.h>
#include <clang/AST/RecursiveASTVisitor.h>
#include <clang/AST/Stmt.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Lex/Preprocessor.h>
#include <clang/Rewrite/Core/Rewriter.h>

#include "pragmas.h"
#include "file_ctx.h"
#include "aux_visitors.h"
#include "converter_debug.h"

using namespace clang; // Sad, but there are too many used from this namespace

namespace cdvmh {

bool isGlobalC(const Decl *d);
std::string genRtType(std::string baseType);
std::string declToStr(Decl *d, bool addStatic, bool addExtern, bool truncateBody);
std::string convertToString(Stmt *s, CompilerInstance &comp);
VarState *fillVarState(const VarDecl *vd, bool CPlusPlus, CompilerInstance &comp, VarState *varState);

struct RmaSubstDesc {
    ClauseRemoteAccess clause;
    std::string nameSubst; // name of newly created REMOTE_ACCESS header
    std::vector<std::string> indexSubst;
    bool usedFlag;
};

struct RmaDesc {
    Stmt *stmt;
    DvmPragma *pragma;
    std::map<VarDecl *, std::vector<RmaSubstDesc> > substs;
};

struct ParallelRmaDesc {
    std::vector<VarDecl *> arrayDecls; // Order is the same as in PragmaParallel::rmas
};

class MyDeclContext {
public:
    MyDeclContext *getParent() const { return parent; }
public:
    explicit MyDeclContext(MyDeclContext *aParent = 0): parent(aParent) {}
public:
    VarDecl *lookupVar(const std::string &varName) const;
    bool add(VarDecl *vd);
protected:
    MyDeclContext *parent;
    std::map<std::string, VarDecl *> vars;
};

struct KernelDesc {
    std::string indexT;
    std::string kernelName;
    std::string regsVar;
    std::string rtIndexT;
    KernelDesc() {}
    explicit KernelDesc(std::string handlerName, std::string aIndexT): indexT(aIndexT) {
        kernelName = handlerName + "_kernel" + (indexT == "int" ? "_int" : (indexT == "long long" ? "_llong" : ""));
        regsVar = kernelName + "_regs";
        rtIndexT = (indexT == "int" ? "rt_INT" : (indexT == "long" ? "rt_LONG" : (indexT == "long long" ? "rt_LLONG" : "")));
        assert(!rtIndexT.empty());
    }
};

class ConverterASTVisitor: public RecursiveASTVisitor<ConverterASTVisitor> {
    typedef RecursiveASTVisitor<ConverterASTVisitor> base;
public:
    bool isDeclAllowed() { return !inRegion || inParLoopBody; }
    bool isDistrDeclAllowed() { return !inRegion && !inParLoop; }
public:
    explicit ConverterASTVisitor(SourceFileContext &aFileCtx, CompilerInstance &aComp, Rewriter &R);
public:
    void addToDeclGroup(Decl *head, Decl *what);
    void addSeenDecl(Decl *d);
    void afterTraversing();
public:
    bool VisitDecl(Decl *d);
    bool VisitVarDecl(VarDecl *vd);
    bool VisitDeclStmt(DeclStmt *ds);
    bool VisitFunctionDecl(FunctionDecl *f);
    bool TraverseFunctionDecl(FunctionDecl *f);
    bool TraverseFunctionProtoTypeLoc(FunctionProtoTypeLoc ft);
    bool TraverseFunctionTemplateDecl(FunctionTemplateDecl *f);
    bool VisitStmt(Stmt *s);
    bool TraverseStmt(Stmt *s);
    bool VisitCompoundStmt(CompoundStmt *s);
    bool TraverseCompoundStmt(CompoundStmt *s);
    bool VisitReturnStmt(ReturnStmt *s);
    bool VisitContinueStmt(ContinueStmt *s);
    bool VisitBreakStmt(BreakStmt *s);
    bool VisitExpr(Expr *e);
    bool VisitCallExpr(CallExpr *e);
    bool VisitDeclRefExpr(DeclRefExpr *e);
    bool VisitCXXNewExpr(CXXNewExpr *e);
    bool VisitCXXDeleteExpr(CXXDeleteExpr *e);
    bool VisitCXXRecordDecl(CXXRecordDecl *d);
public:
    ~ConverterASTVisitor() {
        checkIntErrN(declContexts.size() == 1, 918);
        delete declContexts.back();
        declContexts.pop_back();
    }
protected:
    VarState fillVarState(VarDecl *vd);
    void handleDeclGroup(Decl *head);
    std::string genDvmLine(std::string fn, int ln) {
        if (projectCtx.getOptions().dvmDebugLvl > 0) { ln -= 1; } // Subtract line with additional debug header
        return "dvmh_line_C(" + toStr(ln) + ", \"" + escapeStr(getBaseName(fn)) + "\");";
    }
    std::string genDvmLine(const DvmPragma *curPragma) { return genDvmLine(curPragma->fileName, curPragma->line); }
    std::string genDvmLine(const PresumedLoc &ploc) { return genDvmLine(ploc.getFilename(), ploc.getLine()); }
    std::string genDvmLine(const SourceLocation &loc) { return genDvmLine(srcMgr.getPresumedLoc(srcMgr.getFileLoc(loc))); }
    void genUnbinded(FileID fileID, int line);
    void genActuals(FileID fileID, int line);
    void genRedistributes(FileID fileID, int line);
    void genRealignes(FileID fileID, int line);
    void genIntervals(FileID fileID, int line);
    void genShadowAdds(FileID fileID, int line);
    void genLocalizes(FileID fileID, int line);
    void genUnlocalizes(FileID fileID, int line);
    void genArrayCopies(FileID fileID, int line);
    void genCheckpoints(FileID fileID, int line);
    VarDecl *seekVarDecl(std::string name, MyDeclContext *context = 0);
    void genDerivedFuncPair(const DerivedAxisRule &rule, std::string &countingFormalParamsFwd, std::string &countingFormalParams,
            std::string &countingFuncBody, std::string &fillingFormalParamsFwd, std::string &fillingFormalParams, std::string &fillingFuncBody,
            int &passParamsCount, std::string &passParams);
    std::string genDerivedAxisParams(DvmPragma *curPragma, const DerivedAxisRule &rule);
    std::string genDistribParams(DvmPragma *curPragma, DistribRule *rule);
    std::string genDistribCall(std::string varName, DvmPragma *curPragma, DistribRule *rule);
    std::string genAlignParams(DvmPragma *curPragma, std::string templ, int templRank, const std::vector<AlignAxisRule> &axisRules);
    std::string genAlignParams(DvmPragma *curPragma, AlignRule *rule);
    std::string genAlignCall(std::string varName, DvmPragma *curPragma, AlignRule *rule);

    void enterDeclContext(bool connected) {
        checkIntErrN(declContexts.size() >= 1, 919);
        MyDeclContext *parent = (connected ? declContexts.back() : 0);
        declContexts.push_back(new MyDeclContext(parent));
    }
    void leaveDeclContext() {
        checkIntErrN(declContexts.size() > 1, 920);
        delete declContexts.back();
        declContexts.pop_back();
    }
    void checkIntervalBalance(SourceLocation loc) {
        if (intervalStack.size() != 0){
            FileID fileID = srcMgr.getFileID(loc);
            int line = srcMgr.getLineNumber(fileID, srcMgr.getFileOffset(loc));
            checkUserErrN(intervalStack.back().second != declContexts.back(),srcMgr.getFilename(loc), line, 399);
        }
    }
    bool hasParallelLoopInRange(FileID fid, int startLine, int endLine) {
        std::map<std::pair<unsigned, int>, DvmPragma*>::iterator it;
        for (it = parallelLoops.begin(); it != parallelLoops.end(); it++)
            if (fid.getHashValue() == it->first.first)
                if (it->first.second <= endLine && it->first.second >= startLine)
                    return true;
        return false;
    }

    void addToDelete(std::string varName, std::string method = "dvmh_delete_object_") { toDelete.back().push_back(std::make_pair(method, varName)); }
    bool flushToDelete(std::string &toInsert, std::string indent, int uptoLevel = -1);
    void truncLevels(std::vector<int> &levels) {
        while (!levels.empty() && levels.back() >= (int)toDelete.size())
            levels.pop_back();
    }
    bool isFull(Stmt *s);
    template<typename T, typename NodeT>
    T *findUpwards(NodeT *s, int maxSteps = -1) {
        ast_type_traits::DynTypedNode dtN(ast_type_traits::DynTypedNode::create(*s));
        int step = 0;
        while (!comp.getASTContext().getParents(dtN).empty()) {
            dtN = *comp.getASTContext().getParents(dtN).begin();
            step++;
            if (maxSteps > 0 && step > maxSteps)
                break;
            if (dtN.get<T>())
                return const_cast<T *>(dtN.get<T>());
        }
        return 0;
    }
    std::string convertToString(Stmt *s) {
        return cdvmh::convertToString(s, comp);
    }
    bool isCudaFriendly(FunctionDecl *f);
    bool addFuncForCuda(FunctionDecl *f);
    SourceLocation escapeMacroBegin(SourceLocation loc);
    SourceLocation escapeMacroEnd(SourceLocation loc);
    SourceRange escapeMacro(SourceRange ran) { return SourceRange(escapeMacroBegin(ran.getBegin()), escapeMacroEnd(ran.getEnd())); }
    void checkNonDvmExpr(const MyExpr &expr, DvmPragma *curPragma);
    std::string declToStrForBlank(bool shallow, Decl *d);
    void genBlankHandler(const std::string &handlerName, const std::vector<VarDecl *> &outerParams, const std::vector<LoopVarDesc> &loopVars,
        const std::map<int, Decl *> localDecls, std::string &handlerText);
    void genHostHandler(std::string handlerName, const std::vector<VarDecl *> &outerParams, const std::vector<LoopVarDesc> &loopVars,
        std::string &handlerFormalParams, std::string &handlerBody, bool doingOpenMP);
    void genCudaKernel(const KernelDesc &kernelDesc, const std::vector<VarDecl *> &outerParams, const std::vector<LoopVarDesc> &loopVars,
        std::string handlerTemplateDecl, std::string &kernelText);
    void genCudaHandler(std::string handlerName, const std::vector<VarDecl *> &outerParams, const std::vector<LoopVarDesc> &loopVars,
        std::string handlerTemplateDecl, std::string handlerTemplateSpec, std::string &handlerFormalParams, std::string &handlerBody, std::string &kernelText,
        std::string &cudaInfoText);
    void genAcrossCudaCaseKernel(int dep_number, const KernelDesc &kernelDesc, const std::vector<VarDecl *> &outerParams, const std::vector<LoopVarDesc> &loopVars,
            std::string handlerTemplateDecl, std::string &kernelText);
    void genAcrossCudaCaseHandler(
        int dep_number, std::string baseHandlerName, std::string caseHandlerName, const std::vector<VarDecl *> &outerParams, const std::vector<LoopVarDesc> &loopVars,
        std::string handlerTemplateDecl, std::string handlerTemplateSpec, std::string &caseHandlerText , std::string &cudaInfoText );
    void genAcrossCudaHandler(std::string handlerName, const std::vector<VarDecl *> &outerParams, const std::vector<LoopVarDesc> &loopVars,
        std::string handlerTemplateDecl, std::string handlerTemplateSpec, std::string &handlerFormalParams, std::string &handlerBody,
        std::string &caseHandlers, std::string &cudaInfoText);
protected:
    SourceFileContext &fileCtx;
    ProjectContext &projectCtx;
    const ConverterOptions &opts;
    CompilerInstance &comp;
    Rewriter &rewr;
    SourceManager &srcMgr;
    const LangOptions &langOpts;
    std::string indentStep;
protected:
    std::map<Decl *, std::vector<Decl *> > declGroups;
    std::vector<MyDeclContext *> declContexts; // stack of MyDeclContexts
    bool preventExpandToDelete; // Do not create new element in the stack toDelete (because it is already created)
    std::vector<std::vector<std::pair<std::string, std::string> > > toDelete; // stack of vectors of pairs (method, variable)
    std::vector<int> funcLevels; // stack of indexes for toDelete
    std::vector<int> loopLevels; // stack of indexes for toDelete
    std::vector<int> switchLevels; // stack of indexes for toDelete
    std::map<Decl *, int> declOrder; // all decls in order of traverse

    std::map<VarDecl *, VarState> varStates;
    std::set<DeclRefExpr *> dontSubstitute;
    std::set<const FunctionDecl *> addedCudaFuncs;

    std::set<VarDecl *> curInherits;
    std::set<std::map<std::string, std::string> > curInstantiations;

    bool inRegion;
    PragmaRegion *curRegionPragma;
    Stmt *regionStmt;
    std::string regionInnerIndent;
    std::map<VarDecl *, int> needToRegister;
    int possibleTargets;
    std::string curRegion; // name of variable, which holds region handle
    std::string curLoop; // name of variable for parallel loops inside the region

    bool inParLoop;
    bool inParLoopBody;
    PragmaParallel *curParallelPragma;
    Stmt *parLoopStmt;
    Stmt *parLoopBodyStmt;
    std::set<VarDecl *> needsParams;
    std::set<VarDecl *> outerPrivates;
    std::set<VarDecl *> innerVars;
    std::set<VarDecl *> reductions;
    std::set<VarDecl *> varsToGetActual;
    std::vector<std::pair<int, int> > rmaAppearances;
    int parLoopBodyExprCounter;
    ParallelRmaDesc *parallelRmaDesc;
    std::set<const FunctionDecl *> addCudaFuncs; //TODO remove

    bool inHostSection;
    Stmt *hostSectionStmt;

    std::vector<std::pair<MyExpr, MyDeclContext *> > intervalStack; // stack for checking intervals' DeclContexts
    std::map<std::pair<unsigned, int>, DvmPragma*> parallelLoops; // parallel loops positions, for automatic intervals insertion

    std::vector<RmaDesc> rmaStack;

    std::set<Decl *> blankHandlerDeclsShallow;
    std::set<Decl *> blankHandlerDeclsDeep;

    friend class FuncAdder;
};

class ConverterConsumer: public ASTConsumer {
public:
    explicit ConverterConsumer(SourceFileContext &aFileCtx, CompilerInstance &aComp, Rewriter &aRewr): fileCtx(aFileCtx), rv(aFileCtx, aComp, aRewr),
            rvNamesCollector(aComp) {}
    virtual bool HandleTopLevelDecl(DeclGroupRef D) {
        for (DeclGroupRef::iterator it = D.begin(); it != D.end(); it++)
            rv.addToDeclGroup(*D.begin(), *it);
        return true;
    }
    virtual void HandleTranslationUnit(ASTContext &Ctx) {
        rvNamesCollector.TraverseDecl(Ctx.getTranslationUnitDecl());
        fileCtx.seenGlobalNames = rvNamesCollector.getNames();
        fileCtx.setGlobalNames();
        rv.TraverseDecl(Ctx.getTranslationUnitDecl());
        rv.afterTraversing();
    }
protected:
    SourceFileContext &fileCtx;

    ConverterASTVisitor rv;
    CollectNamesVisitor rvNamesCollector;
};

class IncludeRewriter: public PPCallbacks {
public:
    explicit IncludeRewriter(SourceFileContext &aFileCtx, Rewriter &aRewr): fileCtx(aFileCtx), projectCtx(fileCtx.getProjectCtx()), rewr(aRewr) {}
public:
#if CLANG_VERSION_MAJOR < 7
    virtual void InclusionDirective(SourceLocation HashLoc, const Token &IncludeTok, StringRef FileName, bool IsAngled, CharSourceRange FilenameRange,
            const FileEntry *File, StringRef SearchPath, StringRef RelativePath, const Module *Imported);
#else
    virtual void InclusionDirective(SourceLocation HashLoc, const Token &IncludeTok, StringRef FileName, bool IsAngled, CharSourceRange FilenameRange,
            const FileEntry *File, StringRef SearchPath, StringRef RelativePath, const Module *Imported, SrcMgr::CharacteristicKind FileType) override;
#endif
protected:
    SourceFileContext &fileCtx;
    ProjectContext &projectCtx;
    Rewriter &rewr;
};

class DeclUsageCollector: public RecursiveASTVisitor<DeclUsageCollector> {
    typedef RecursiveASTVisitor<DeclUsageCollector> base;
public:
    const std::set<Decl *> &getReferencedDeclsShallow() const { return referencedDeclsShallow; }
    const std::set<Decl *> &getReferencedDeclsDeep() const { return referencedDeclsDeep; }
public:
    DeclUsageCollector(std::set<Decl *> &shallow, std::set<Decl *> &deep, bool deepMode = false): referencedDeclsShallow(shallow), referencedDeclsDeep(deep), 
            deepMode(deepMode), becomeDeepInside(0) {}
public:
    bool VisitDeclRefExpr(DeclRefExpr *e);
    bool TraverseType(QualType t);
    bool VisitType(Type *t);
    bool VisitFunctionDecl(FunctionDecl *fd);
    bool TraverseStmt(Stmt *cs);
protected:
    void addAsReferenced(Decl *d, bool asDeep) {
        referencedDeclsDeep.insert(d);
        if (!asDeep)
            referencedDeclsShallow.insert(d);
    }
    void addType(const Type *t);
    void addDecl(Decl *d, bool forceDeep = false);
protected:
    std::set<Decl *> &referencedDeclsShallow;
    std::set<Decl *> &referencedDeclsDeep;
    bool deepMode;
    Stmt *becomeDeepInside;
};

}
