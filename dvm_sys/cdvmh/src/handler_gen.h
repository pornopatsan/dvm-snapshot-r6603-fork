#pragma once

#include "aux_visitors.h"

namespace cdvmh {

struct MyVarState: public VarState {
    bool isWeirdRma;
    MyVarState(): isWeirdRma(false) {}
};

struct PragmaHandlerStub {
    int line;
    std::set<std::string> dvmArrays;
    std::set<std::string> regArrays;
    std::set<std::string> scalars;
    std::vector<LoopVarDesc> loopVars;
    std::vector<ClauseReduction> reductions;
    std::set<std::string> privates;
    std::set<std::string> weirdRmas;
    int minAcross;
    int maxAcross;
};

class BlankPragmaHandler: public PragmaHandler {
public:
    explicit BlankPragmaHandler(CompilerInstance &aComp): PragmaHandler("dvm"), comp(aComp) {}
public:
    void HandlePragma(Preprocessor &PP, PragmaIntroducerKind Introducer, Token &FirstToken);
public:
    PragmaHandlerStub *getPragmaAtLine(int line) {
        if (pragmas.find(line) != pragmas.end())
            return pragmas[line];
        else
            return 0;
    }
protected:
    CompilerInstance &comp;
    std::map<int, PragmaHandlerStub *> pragmas;
};

class Blank2HostVisitor: public RecursiveASTVisitor<Blank2HostVisitor> {
    typedef RecursiveASTVisitor<Blank2HostVisitor> base;
public:
    explicit Blank2HostVisitor(CompilerInstance &aComp, Rewriter &aRewr, BlankPragmaHandler *aPh, const std::map<std::string, HostReq> &aReqs, bool withHead):
            comp(aComp), rewr(aRewr), srcMgr(rewr.getSourceMgr()), ph(aPh), reqs(aReqs), withHeading(withHead) {
        firstHandler = true;
    }
public:
    bool VisitFunctionDecl(FunctionDecl *f);
    bool TraverseFunctionDecl(FunctionDecl *f);
protected:
    CompilerInstance &comp;
    Rewriter &rewr;
    SourceManager &srcMgr;

    BlankPragmaHandler *ph;
    const std::map<std::string, HostReq> &reqs;
    bool withHeading;

    bool firstHandler;
};

class Blank2HostConsumer: public ASTConsumer {
public:
    explicit Blank2HostConsumer(CompilerInstance &comp, Rewriter &rewr, BlankPragmaHandler *ph, const std::map<std::string, HostReq> &reqs, bool withHeading):
            rv(comp, rewr, ph, reqs, withHeading) {}
public:
    virtual void HandleTranslationUnit(ASTContext &Ctx) {
        rv.TraverseDecl(Ctx.getTranslationUnitDecl());
    }
protected:
    Blank2HostVisitor rv;
};

class BlankHandlerConverter {
public:
    explicit BlankHandlerConverter(const SourceFileContext &aFileCtx);
public:
    std::string genRmas(const std::string &src);
    std::string genHostHandlers(const std::string &src, bool withHeading = true);
    std::string genCudaHandlers(const std::string &src);
protected:
    std::string prepareFile(const std::string &src);
protected:
    const SourceFileContext &fileCtx;
    const ConverterOptions &opts;
};

}
