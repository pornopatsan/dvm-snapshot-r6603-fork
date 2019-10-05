#include "handler_gen.h"

#include <cstdio>

#ifdef WIN32
#include <io.h>
#endif

#include "pass_ctx.h"
#include "messages.h"

namespace cdvmh {

void BlankPragmaHandler::HandlePragma(Preprocessor &PP, PragmaIntroducerKind Introducer, Token &FirstToken) {
    SourceLocation loc = FirstToken.getLocation();
    loc = comp.getSourceManager().getFileLoc(loc);
    FileID fileID = comp.getSourceManager().getFileID(loc);
    int line = comp.getSourceManager().getLineNumber(fileID, comp.getSourceManager().getFileOffset(loc));
    Token &Tok = FirstToken;
    PP.LexNonComment(Tok);
    checkIntErrN(Tok.isAnyIdentifier(), 914);
    std::string tokStr = Tok.getIdentifierInfo()->getName();
    checkIntErrN(tokStr == "handler_stub", 914);
    PragmaHandlerStub *curPragma = new PragmaHandlerStub;
    curPragma->line = line;
    curPragma->minAcross = 0;
    curPragma->maxAcross = 0;
    PP.LexNonComment(Tok);
    while (Tok.isAnyIdentifier() || Tok.is(tok::kw_private)) {
        std::string clauseName = Tok.getIdentifierInfo()->getName();
        PP.LexNonComment(Tok);
        checkIntErrN(Tok.is(tok::l_paren), 914);
        PP.LexNonComment(Tok);
        if (clauseName == "dvm_array") {
            while (Tok.isAnyIdentifier()) {
                tokStr = Tok.getIdentifierInfo()->getName();
                curPragma->dvmArrays.insert(tokStr);
                PP.LexNonComment(Tok);
                checkIntErrN(Tok.is(tok::comma) || Tok.is(tok::r_paren), 914);
                if (Tok.is(tok::comma)) {
                    PP.LexNonComment(Tok);
                    checkIntErrN(Tok.isAnyIdentifier(), 914);
                }
            }
        } else if (clauseName == "regular_array") {
            while (Tok.isAnyIdentifier()) {
                tokStr = Tok.getIdentifierInfo()->getName();
                curPragma->regArrays.insert(tokStr);
                PP.LexNonComment(Tok);
                checkIntErrN(Tok.is(tok::comma) || Tok.is(tok::r_paren), 914);
                if (Tok.is(tok::comma)) {
                    PP.LexNonComment(Tok);
                    checkIntErrN(Tok.isAnyIdentifier(), 914);
                }
            }
        } else if (clauseName == "scalar") {
            while (Tok.isAnyIdentifier()) {
                tokStr = Tok.getIdentifierInfo()->getName();
                curPragma->scalars.insert(tokStr);
                PP.LexNonComment(Tok);
                checkIntErrN(Tok.is(tok::comma) || Tok.is(tok::r_paren), 914);
                if (Tok.is(tok::comma)) {
                    PP.LexNonComment(Tok);
                    checkIntErrN(Tok.isAnyIdentifier(), 914);
                }
            }
        } else if (clauseName == "loop_var") {
            while (Tok.isAnyIdentifier()) {
                LoopVarDesc loopVar;
                tokStr = Tok.getIdentifierInfo()->getName();
                loopVar.name = tokStr;
                PP.LexNonComment(Tok);
                checkIntErrN(Tok.is(tok::l_paren), 914);
                PP.LexNonComment(Tok);
                tokStr = PP.getSpelling(Tok);
                checkIntErrN(tokStr == "1" || tokStr == "-1", 914);
                loopVar.stepSign = atoi(tokStr.c_str());
                PP.LexNonComment(Tok);
                checkIntErrN(Tok.is(tok::comma), 914);
                PP.LexNonComment(Tok);
                if (Tok.isNot(tok::r_paren)) {
                    tokStr = PP.getSpelling(Tok);
                    checkIntErrN(isNumber(tokStr), 914);
                    loopVar.constStep = tokStr;
                    PP.LexNonComment(Tok);
                }
                checkIntErrN(Tok.is(tok::r_paren), 914);
                curPragma->loopVars.push_back(loopVar);
                PP.LexNonComment(Tok);
                checkIntErrN(Tok.is(tok::comma) || Tok.is(tok::r_paren), 914);
                if (Tok.is(tok::comma)) {
                    PP.LexNonComment(Tok);
                    checkIntErrN(Tok.isAnyIdentifier(), 914);
                }
            }
        } else if (clauseName == "reduction") {
            while (Tok.isAnyIdentifier()) {
                ClauseReduction red;
                tokStr = Tok.getIdentifierInfo()->getName();
                red.redType = ClauseReduction::guessRedType(tokStr);
                checkIntErrN(!red.redType.empty(), 914);
                PP.LexNonComment(Tok);
                checkIntErrN(Tok.is(tok::l_paren), 914);
                PP.LexNonComment(Tok);
                checkIntErrN(Tok.isAnyIdentifier(), 914);
                red.arrayName = Tok.getIdentifierInfo()->getName();
                PP.LexNonComment(Tok);
                if (red.isLoc()) {
                    checkIntErrN(Tok.is(tok::comma), 914);
                    PP.LexNonComment(Tok);
                    checkIntErrN(Tok.isAnyIdentifier(), 914);
                    red.locName = Tok.getIdentifierInfo()->getName();
                    PP.LexNonComment(Tok);
                    checkIntErrN(Tok.is(tok::comma), 914);
                    PP.LexNonComment(Tok);
                    tokStr = PP.getSpelling(Tok);
                    checkIntErrN(isNumber(tokStr), 914);
                    red.locSize.strExpr = tokStr;
                    PP.LexNonComment(Tok);
                }
                checkIntErrN(Tok.is(tok::r_paren), 914);
                curPragma->reductions.push_back(red);
                PP.LexNonComment(Tok);
                checkIntErrN(Tok.is(tok::comma) || Tok.is(tok::r_paren), 914);
                if (Tok.is(tok::comma)) {
                    PP.LexNonComment(Tok);
                    checkIntErrN(Tok.isAnyIdentifier(), 914);
                }
            }
        } else if (clauseName == "private") {
            while (Tok.isAnyIdentifier()) {
                tokStr = Tok.getIdentifierInfo()->getName();
                curPragma->privates.insert(tokStr);
                PP.LexNonComment(Tok);
                checkIntErrN(Tok.is(tok::comma) || Tok.is(tok::r_paren), 914);
                if (Tok.is(tok::comma)) {
                    PP.LexNonComment(Tok);
                    checkIntErrN(Tok.isAnyIdentifier(), 914);
                }
            }
        } else if (clauseName == "weird_rma") {
            while (Tok.isAnyIdentifier()) {
                tokStr = Tok.getIdentifierInfo()->getName();
                curPragma->weirdRmas.insert(tokStr);
                PP.LexNonComment(Tok);
                checkIntErrN(Tok.is(tok::comma) || Tok.is(tok::r_paren), 914);
                if (Tok.is(tok::comma)) {
                    PP.LexNonComment(Tok);
                    checkIntErrN(Tok.isAnyIdentifier(), 914);
                }
            }
        } else if (clauseName == "across") {
            tokStr = PP.getSpelling(Tok);
            checkIntErrN(isNumber(tokStr), 914);
            curPragma->minAcross = toNumber(tokStr);
            PP.LexNonComment(Tok);
            checkIntErrN(Tok.is(tok::comma), 914);
            PP.LexNonComment(Tok);
            tokStr = PP.getSpelling(Tok);
            checkIntErrN(isNumber(tokStr), 914);
            curPragma->maxAcross = toNumber(tokStr);
            PP.LexNonComment(Tok);
            checkIntErrN(Tok.is(tok::r_paren), 914);
        } else if (clauseName == "remote_access") {
            int depth = 1;
            while (Tok.isNot(tok::r_paren) || depth > 1) {
                if (Tok.is(tok::l_paren))
                    depth++;
                if (Tok.is(tok::r_paren))
                    depth--;
                PP.LexNonComment(Tok);
            }
        } else {
            checkIntErrN(false, 914);
        }
        checkIntErrN(Tok.is(tok::r_paren), 914);
        PP.LexNonComment(Tok);
        checkIntErrN(Tok.is(tok::comma) || Tok.is(tok::eod), 914);
        if (Tok.is(tok::comma))
            PP.LexNonComment(Tok);
    }
    checkIntErrN(Tok.is(tok::eod), 914);
    pragmas[line] = curPragma;
}

bool Blank2HostVisitor::VisitFunctionDecl(FunctionDecl *f) {
    FileID fileID = srcMgr.getFileID(f->getLocStart());
    int pragmaLine = srcMgr.getLineNumber(fileID, srcMgr.getFileOffset(f->getLocStart())) - 1;
    PragmaHandlerStub *curPragma = ph->getPragmaAtLine(pragmaLine);
    std::string funcName = f->getName();
    bool toConvert = reqs.find(funcName) != reqs.end();
    bool isHandler = curPragma != 0;
    if (!withHeading && firstHandler && isHandler) {
        firstHandler = false;
        if (pragmaLine > 2)
            rewr.RemoveText(SourceRange(srcMgr.translateLineCol(fileID, 1, 1), srcMgr.translateLineCol(fileID, pragmaLine, 1).getLocWithOffset(-1)));
    }
    if (toConvert) {
        checkIntErrN(isHandler, 915, funcName.c_str());
        // Remove pragma
        SourceLocation lineBegLoc = srcMgr.translateLineCol(fileID, pragmaLine, 1);
        const char *lineBeg = srcMgr.getBufferData(fileID).data() + srcMgr.getFileOffset(lineBegLoc);
        const char *lineEnd = strchr(lineBeg, '\n');
        rewr.RemoveText(lineBegLoc, lineEnd - lineBeg + 1);
    } else {
        if (isHandler) {
            // Cut it
            SourceRange sr(srcMgr.translateLineCol(fileID, pragmaLine, 1), f->getLocEnd());
            rewr.RemoveText(sr);
        }
    }
    return true;
}

bool Blank2HostVisitor::TraverseFunctionDecl(FunctionDecl *f) {
    bool res = base::TraverseFunctionDecl(f);
    return res;
}

BlankHandlerConverter::BlankHandlerConverter(const SourceFileContext &aFileCtx): fileCtx(aFileCtx), opts(fileCtx.getProjectCtx().getOptions()) {
    // Nothing to do here
}

std::string BlankHandlerConverter::genRmas(const std::string &src) {
    // TODO: implement
    std::string res = src;
    std::string fn = prepareFile(src);
#if 0
    {
        PassContext passCtx(fn);
        BlankPragmaHandler *pragmaHandler = new BlankPragmaHandler(*passCtx.getCompiler());
        passCtx.getPP()->AddPragmaHandler(pragmaHandler);
        Blank2RmaBlankConsumer astConsumer(*passCtx.getCompiler(), *passCtx.getRewr(), pragmaHandler);
        passCtx.parse(&astConsumer);
        const RewriteBuffer *rewriteBuf = passCtx.getRewr()->getRewriteBufferFor(passCtx.getRewr()->getSourceMgr().getMainFileID());
        if (rewriteBuf)
            res = std::string(rewriteBuf->begin(), rewriteBuf->end());
    }
#endif
    remove(fn.c_str());
    return res;
}

std::string BlankHandlerConverter::genHostHandlers(const std::string &src, bool withHeading) {
    std::string res = src;
    std::string fn = prepareFile(src);
    {
        PassContext passCtx(fn);
        BlankPragmaHandler *pragmaHandler = new BlankPragmaHandler(*passCtx.getCompiler());
        passCtx.getPP()->AddPragmaHandler(pragmaHandler);
        Blank2HostConsumer astConsumer(*passCtx.getCompiler(), *passCtx.getRewr(), pragmaHandler, fileCtx.getHostReqMap(), withHeading);
        passCtx.parse(&astConsumer);
        const RewriteBuffer *rewriteBuf = passCtx.getRewr()->getRewriteBufferFor(passCtx.getRewr()->getSourceMgr().getMainFileID());
        if (rewriteBuf)
            res = std::string(rewriteBuf->begin(), rewriteBuf->end());
    }
    remove(fn.c_str());
    return res;
}

std::string BlankHandlerConverter::genCudaHandlers(const std::string &src) {
    // TODO: implement
    std::string res;
    return res;
}

std::string BlankHandlerConverter::prepareFile(const std::string &src) {
    const char fnTempl[] = "CDVMH_XXXXXX";
    char *fn1 = myStrDup(fnTempl);
#ifndef WIN32
    char *tmpRes = mktemp(fn1);
#else
    char *tmpRes = _mktemp(fn1);
#endif
    checkIntErrN(tmpRes && tmpRes[0], 916);
    std::string fn(fn1);
    delete[] fn1;
    fn += (fileCtx.getInputFile().CPlusPlus ? ".cpp" : ".c");
    FILE *f = fopen(fn.c_str(), "wb");
    checkIntErrN(f, 916);
    fwrite(src.c_str(), src.size(), 1, f);
    fclose(f);
    return fn;
}

}
