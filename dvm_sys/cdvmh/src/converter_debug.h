#pragma once

#include <clang/Basic/Version.h>
#include <clang/AST/ASTConsumer.h>
#include <clang/AST/ASTContext.h>
#include <clang/AST/RecursiveASTVisitor.h>
#include <clang/AST/Stmt.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Lex/Preprocessor.h>
#include <clang/Rewrite/Core/Rewriter.h>

#include "pragmas.h"
#include "file_ctx.h"
#include "pass_ctx.h"
#include "converter.h"

#include <iostream>

using namespace clang;

namespace cdvmh {

class DebugASTVisitor : public RecursiveASTVisitor< DebugASTVisitor > {
    typedef RecursiveASTVisitor< DebugASTVisitor > base;

public:
    explicit DebugASTVisitor(SourceFileContext &aFileCtx, CompilerInstance &aComp, Rewriter &R);

public:
    bool VisitStmt(Stmt *s);
    bool VisitExpr(Expr *e);
    bool VisitVarDecl(VarDecl *vd);
    bool VisitFunctionDecl(FunctionDecl *f);
    bool TraverseStmt(Stmt *s);
    bool TraverseVarDecl(VarDecl *vd);
    bool TraverseFunctionDecl(FunctionDecl *f);

protected:
    bool HandleExpr(Expr *e);
    VarState fillVarState(VarDecl *vd); 
    SourceLocation escapeMacroBegin(SourceLocation loc);
    SourceLocation escapeMacroEnd(SourceLocation loc);
    SourceLocation getNormalEndLoc(SourceLocation end);
    SourceLocation getNormalStmtEndLoc(Stmt *s);
    SourceLocation getRealLoc(SourceLocation loc);
    bool isLoopVar(const std::string &name) const;

protected:
    bool forLoopIsParallel(ForStmt *curLoop);
    DvmPragma* forLoopParallelPragma(ForStmt *curLoop);

    VarDecl* getForLoopInitVar(ForStmt *curLoop);
    std::string getLoopLowerBound(ForStmt *curLoop);
    std::string getLoopUpperBound(ForStmt *curLoop);
    std::string getLoopStep(ForStmt *curLoop);
    std::string getLoopBounds(ForStmt *curLoop);
    std::string getFileName(const SourceLocation &loc);
    std::string getStmtIndent(Stmt *s);
    std::string genDvmReadVar(SourceLocation loc, const VarState &state);
    std::string genDvmWriteVar(SourceLocation loc, const VarState &state);
    std::string genDvmWriteVarInit(SourceLocation loc, const VarState &state);
    std::string genDvmReadVarArray(SourceLocation loc, const VarState &state, const std::string &refName);
    std::string genDvmWriteVarArray(SourceLocation loc, const VarState &state, const std::string &refName);
    unsigned getLineFromLoc(const SourceLocation &loc);
    void genSeqLoopCalls(ForStmt *curLoop, int curLoopNumber);
    void genSeqLoopIter(ForStmt *curLoop);
    void genParLoopCalls(ForStmt *curLoop, int curLoopNumber);
    void genParLoopIter(ForStmt *curLoop);

protected:
    std::string convertToString(Stmt *s) {
#if CLANG_VERSION_MAJOR < 4 && CLANG_VERSION_MINOR < 6
        return rewr.ConvertToString(s);
#else
    std::string SStr;
    llvm::raw_string_ostream S(SStr);
    s->printPretty(S, 0, PrintingPolicy(comp.getLangOpts()));
    return S.str();
#endif
    }

    std::string convertToString(Decl *d) {
    std::string str;
    llvm::raw_string_ostream s(str);
    d->print(s);
    std::string result = s.str();

    return result;
    }

protected:
    bool inFunction;
    bool inVarDecl;
    bool inLoopHeader;
    bool inParallelLoop;
    bool inArraySubscripts;
    int curLoopNumber;
    std::set<Expr*> varWritings;
    std::set<Expr*> currentSubscripts;
    std::set<VarDecl*> distribArrays;
    std::map<VarDecl*, VarState> varStates;
    std::vector<std::pair<Stmt*, std::string> > loopVarsStack;
    std::map<int, DvmPragma*> parallelPragmas;
    std::map<ForStmt*, int> loopNumbers;
    
protected:
    SourceFileContext &fileCtx;
    ProjectContext &projectCtx;
    const ConverterOptions &opts;
    CompilerInstance &comp;
    Rewriter &rewr;
    SourceManager &srcMgr;
    const LangOptions &langOpts;

};


class DebugConsumer: public ASTConsumer {
public:
    explicit DebugConsumer(SourceFileContext &aFileCtx, CompilerInstance &aComp, Rewriter &aRewr): fileCtx(aFileCtx), rv(aFileCtx, aComp, aRewr),
            rvNamesCollector(aComp) {}
    virtual bool HandleTopLevelDecl(DeclGroupRef D) {
        // TODO: Do something with global variables declaration
        /*for (DeclGroupRef::iterator it = D.begin(); it != D.end(); it++) {
            rv.addToDeclGroup(*D.begin(), *it);
        }*/
        return true;
    }
    virtual void HandleTranslationUnit(ASTContext &Ctx) {
        rvNamesCollector.TraverseDecl(Ctx.getTranslationUnitDecl());
        fileCtx.seenGlobalNames = rvNamesCollector.getNames();
        fileCtx.setGlobalNames();
        rv.TraverseDecl(Ctx.getTranslationUnitDecl());
    }
protected:
    SourceFileContext &fileCtx;
    DebugASTVisitor rv;
    CollectNamesVisitor rvNamesCollector;
};

}
