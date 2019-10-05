#pragma once

#include <clang/Frontend/CompilerInstance.h>
#include <clang/Lex/Pragma.h>
#include <clang/Lex/Preprocessor.h>
#include <clang/Rewrite/Core/Rewriter.h>

#include "pragmas.h"
#include "file_ctx.h"

using clang::PragmaHandler;
using clang::CompilerInstance;
using clang::Rewriter;
using clang::Preprocessor;
using clang::PragmaIntroducerKind;
using clang::Token;
using clang::FileID;

namespace cdvmh {

class DvmPragmaHandler: public PragmaHandler {
public:
    explicit DvmPragmaHandler(SourceFileContext &aFileCtx, CompilerInstance &aComp, Rewriter &aRewr): PragmaHandler("dvm"), fileCtx(aFileCtx),
            projectCtx(fileCtx.getProjectCtx()), comp(aComp), rewr(aRewr) {}
public:
    void HandlePragma(Preprocessor &PP, PragmaIntroducerKind Introducer, Token &FirstToken);
protected:
    DerivedAxisRule parseDerivedAxisRule(Preprocessor &PP, Token &Tok);
    DistribRule parseDistribRule(Preprocessor &PP, Token &Tok);
    AlignAxisRule parseAlignAxisRule(Preprocessor &PP, Token &Tok, std::map<std::string, int> nameToAxis, bool parLoopFlag);
    AlignRule parseAlignRule(Preprocessor &PP, Token &Tok, bool parLoopFlag = false);
    std::vector<SlicedArray> parseSubarrays(Preprocessor &PP, Token &Tok);
    MyExpr readExpr(Preprocessor &PP, Token &Tok, std::string stopTokens = "");
    std::pair<MyExpr, MyExpr> readRange(Preprocessor &PP, Token &Tok);
    ClauseRemoteAccess parseOneRma(Preprocessor &PP, Token &Tok, const std::map<std::string, int> &nameToAxis);
protected:
    SourceFileContext &fileCtx;
    ProjectContext &projectCtx;
    CompilerInstance &comp;
    Rewriter &rewr;
    DvmPragma *curPragma;
    FileID fileID;
};

}
