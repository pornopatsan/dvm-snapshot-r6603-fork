#include "handler_gen.h"

#include <cstdio>

#ifdef WIN32
#include <io.h>
#endif

#include "pass_ctx.h"
#include "messages.h"
#include "converter.h"

namespace cdvmh {

// BlankPragmaHandler

void BlankPragmaHandler::HandlePragma(Preprocessor &PP, PragmaIntroducer Introducer, Token &FirstToken) {
    SourceLocation loc = FirstToken.getLocation();
    loc = comp.getSourceManager().getFileLoc(loc);
    FileID fileID = comp.getSourceManager().getFileID(loc);
    int line = comp.getSourceManager().getLineNumber(fileID, comp.getSourceManager().getFileOffset(loc));
    Token &Tok = FirstToken;
    PP.LexNonComment(Tok);
    checkIntErrN(Tok.isAnyIdentifier(), 914);
    std::string tokStr = Tok.getIdentifierInfo()->getName().str();
    checkIntErrN(tokStr == "handler_stub", 914);
    PragmaHandlerStub *curPragma = new PragmaHandlerStub;
    curPragma->line = line;
    curPragma->minAcross = 0;
    curPragma->maxAcross = 0;
    PP.LexNonComment(Tok);
    while (Tok.isAnyIdentifier() || Tok.is(tok::kw_private)) {
        std::string clauseName = Tok.getIdentifierInfo()->getName().str();
        PP.LexNonComment(Tok);
        checkIntErrN(Tok.is(tok::l_paren), 914);
        PP.LexNonComment(Tok);
        if (clauseName == "dvm_array") {
            while (Tok.isAnyIdentifier()) {
                tokStr = Tok.getIdentifierInfo()->getName().str();
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
                tokStr = Tok.getIdentifierInfo()->getName().str();
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
                tokStr = Tok.getIdentifierInfo()->getName().str();
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
                tokStr = Tok.getIdentifierInfo()->getName().str();
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
                tokStr = Tok.getIdentifierInfo()->getName().str();
                red.redType = ClauseReduction::guessRedType(tokStr);
                checkIntErrN(!red.redType.empty(), 914);
                PP.LexNonComment(Tok);
                checkIntErrN(Tok.is(tok::l_paren), 914);
                PP.LexNonComment(Tok);
                checkIntErrN(Tok.isAnyIdentifier(), 914);
                red.arrayName = Tok.getIdentifierInfo()->getName().str();
                PP.LexNonComment(Tok);
                if (red.isLoc()) {
                    checkIntErrN(Tok.is(tok::comma), 914);
                    PP.LexNonComment(Tok);
                    checkIntErrN(Tok.isAnyIdentifier(), 914);
                    red.locName = Tok.getIdentifierInfo()->getName().str();
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
                tokStr = Tok.getIdentifierInfo()->getName().str();
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
                tokStr = Tok.getIdentifierInfo()->getName().str();
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
            while (Tok.isAnyIdentifier()) {
                ClauseBlankRma clause;
                tokStr = Tok.getIdentifierInfo()->getName().str();
                clause.origName = tokStr;
                PP.LexNonComment(Tok);
                while (!Tok.is(tok::l_paren)) {
                    PP.LexNonComment(Tok);
                }
                PP.LexNonComment(Tok);
                while (Tok.is(tok::l_square)) {
                    PP.LexNonComment(Tok);
                    if (Tok.is(tok::r_square)) {
                        clause.indexExprs.push_back("");
                    } else if (Tok.isAnyIdentifier()) {
                        tokStr = Tok.getIdentifierInfo()->getName().str();
                        clause.indexExprs.push_back(tokStr);
                        PP.LexNonComment(Tok);
                    } else {
                        // Zero constant
                        tokStr = PP.getSpelling(Tok);
                        checkIntErrN(tokStr == "0", 914);
                        clause.indexExprs.push_back(tokStr);
                        PP.LexNonComment(Tok);
                    }
                    checkIntErrN(Tok.is(tok::r_square), 914);
                    PP.LexNonComment(Tok);
                }
                checkIntErrN(Tok.is(tok::comma), 914);
                PP.LexNonComment(Tok);
                checkIntErrN(Tok.isAnyIdentifier(), 914);
                tokStr = Tok.getIdentifierInfo()->getName().str();
                checkIntErrN(tokStr == "appearances", 914);
                PP.LexNonComment(Tok);
                checkIntErrN(Tok.is(tok::l_paren), 914);
                PP.LexNonComment(Tok);
                while (!Tok.is(tok::r_paren)) {
                    tokStr = PP.getSpelling(Tok);
                    checkIntErrN(isNumber(tokStr), 914);
                    clause.appearances.push_back(toNumber(tokStr));
                    PP.LexNonComment(Tok);
                    checkIntErrN(Tok.is(tok::comma) || Tok.is(tok::r_paren), 914);
                    if (Tok.is(tok::comma))
                        PP.LexNonComment(Tok);
                }
                PP.LexNonComment(Tok);
                checkIntErrN(Tok.is(tok::r_paren), 914);
                curPragma->rmas.push_back(clause);
                PP.LexNonComment(Tok);
                checkIntErrN(Tok.is(tok::comma) || Tok.is(tok::r_paren), 914);
                if (Tok.is(tok::comma)) {
                    PP.LexNonComment(Tok);
                }
            }
            checkIntErrN(Tok.is(tok::r_paren), 914);
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

// BlankRemoteVisitor

bool BlankRemoteVisitor::VisitFunctionDecl(FunctionDecl *f) {
    FileID fileID = srcMgr.getFileID(f->getBeginLoc());
    int pragmaLine = srcMgr.getLineNumber(fileID, srcMgr.getFileOffset(f->getBeginLoc())) - 1;
    PragmaHandlerStub *curPragma = ph->getPragmaAtLine(pragmaLine);
    std::string funcName = f->getName().str();
    bool isHandler = curPragma != 0;
    if (!isHandler || curPragma->rmas.empty()) {
        return true;
    }
    this->curPragma = curPragma;
    parLoopBodyExprCounter = 0;
    inParLoopBody = false;
    parLoopBodyStmt = 0;
    CompoundStmt *body = cast<CompoundStmt>(f->getBody());
    for (Stmt **it = body->body_begin(); it != body->body_end(); it++) {
        CompoundStmt *candidate = llvm::dyn_cast<CompoundStmt>(*it);
        if (candidate) {
            parLoopBodyStmt = candidate;
            break;
        }
    }

    std::set<std::string> prohibitedNames;
    CollectNamesVisitor collectNamesVisitor(comp);
    collectNamesVisitor.TraverseStmt(body);
    prohibitedNames = collectNamesVisitor.getNames();
    for (std::set<std::string>::iterator it = curPragma->dvmArrays.begin(); it != curPragma->dvmArrays.end(); it++)
        prohibitedNames.insert(*it);
    for (std::set<std::string>::iterator it = curPragma->regArrays.begin(); it != curPragma->regArrays.end(); it++)
        prohibitedNames.insert(*it);
    for (std::set<std::string>::iterator it = curPragma->scalars.begin(); it != curPragma->scalars.end(); it++)
        prohibitedNames.insert(*it);

    std::string weirdRmasList;
    for (int i = 0; i < (int)curPragma->rmas.size(); i++) {
        std::string substName = getUniqueName(curPragma->rmas[i].origName + "_rma", &prohibitedNames, &seenMacroNames);
        curPragma->rmas[i].substName = substName;
        prohibitedNames.insert(substName);
        weirdRmasList = ", " + substName;
    }
    trimList(weirdRmasList);

    SourceLocation lineBegLoc = srcMgr.translateLineCol(fileID, pragmaLine, 1);
    const char *lineBeg = srcMgr.getBufferData(fileID).data() + srcMgr.getFileOffset(lineBegLoc);
    const char *lineEnd = strchr(lineBeg, '\n');
    SourceLocation lineEndLoc = lineBegLoc.getLocWithOffset(lineEnd - lineBeg);
    rewr.InsertTextAfter(lineEndLoc, ", weird_rma(" + weirdRmasList + ")");

    // TODO: Need to insert them to formal parameters as well :(
    int numParams = f->getNumParams();
    std::string rmaFormalParams;
    for (int i = 0; i < (int)curPragma->rmas.size(); i++) {
        ClauseBlankRma &clause = curPragma->rmas[i];
        int found = -1;
        for (int j = 0; j < numParams; j++) {
            const ParmVarDecl *pvd = f->getParamDecl(j);
            std::string paramName = pvd->getIdentifier()->getName().str();
            if (clause.origName == paramName) {
                found = j;
                break;
            }
        }
        checkIntErrN(found >= 0, 914);
        const ParmVarDecl *pvd = f->getParamDecl(found);
        VarState varState;
        fillVarState(pvd, false, comp, &varState);
        rmaFormalParams += ", " + varState.baseTypeStr + " " + clause.substName;
        for (int j = 0; j < (int)clause.indexExprs.size(); j++) {
            rmaFormalParams += "[DVMH_VARIABLE_ARRAY_SIZE]";
        }
    }
    const ParmVarDecl *pvd = f->getParamDecl(numParams - 1);
    SourceLocation endLoc = pvd->getEndLoc();
    endLoc = Lexer::getLocForEndOfToken(endLoc, 0, srcMgr, comp.getLangOpts());
    rewr.InsertTextAfter(endLoc, rmaFormalParams);

    return true;
}

bool BlankRemoteVisitor::TraverseStmt(Stmt *s) {
    if (s == parLoopBodyStmt) {
        inParLoopBody = true;
    }
    bool res = base::TraverseStmt(s);
    if (s == parLoopBodyStmt) {
        inParLoopBody = false;
    }
    return res;
}

bool BlankRemoteVisitor::VisitExpr(Expr *e) {
    if (inParLoopBody) {
        parLoopBodyExprCounter++;

        bool found = false;
        for (int i = 0; !found && i < (int)curPragma->rmas.size(); i++) {
            for (int j = 0; !found && j < (int)curPragma->rmas[i].appearances.size(); j++) {
                if (curPragma->rmas[i].appearances[j] == parLoopBodyExprCounter) {
                    found = true;
                    std::string subst;
                    subst += curPragma->rmas[i].substName;
                    for (int k = 0; k < (int)curPragma->rmas[i].indexExprs.size(); k++) {
                        subst += "[" + curPragma->rmas[i].indexExprs[k] + "]";
                    }
                    rewr.ReplaceText(e->getSourceRange(), subst);
                }
            }
        }
    }

    return true;
}

// Blank2HostVisitor

bool Blank2HostVisitor::VisitFunctionDecl(FunctionDecl *f) {
    FileID fileID = srcMgr.getFileID(f->getBeginLoc());
    int pragmaLine = srcMgr.getLineNumber(fileID, srcMgr.getFileOffset(f->getBeginLoc())) - 1;
    PragmaHandlerStub *curPragma = ph->getPragmaAtLine(pragmaLine);
    std::string funcName = f->getName().str();
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
        // call genHandler
    } else {
        if (isHandler) {
            // Cut it
            SourceRange sr(srcMgr.translateLineCol(fileID, pragmaLine, 1), f->getEndLoc());
            rewr.RemoveText(sr);
        }
    }
    return true;
}

bool Blank2HostVisitor::TraverseFunctionDecl(FunctionDecl *f) {
    bool res = base::TraverseFunctionDecl(f);
    return res;
}


// BlankHandlerConverter

BlankHandlerConverter::BlankHandlerConverter(const SourceFileContext &aFileCtx): fileCtx(aFileCtx), opts(fileCtx.getProjectCtx().getOptions()) {
    // Nothing to do here
}

std::string BlankHandlerConverter::genRmas(const std::string &src) {
    std::string res = src;
    std::string fn = prepareFile(src);
    {
        PassContext passCtx(fn);
        Rewriter *rewr = passCtx.getRewr();
        BlankPragmaHandler *pragmaHandler = new BlankPragmaHandler(*passCtx.getCompiler());
        passCtx.getPP()->AddPragmaHandler(pragmaHandler);
        BlankRemoteConsumer astConsumer(*passCtx.getCompiler(), *rewr, pragmaHandler, fileCtx.seenMacroNames);
        passCtx.parse(&astConsumer);
        const RewriteBuffer *rewriteBuf = rewr->getRewriteBufferFor(rewr->getSourceMgr().getMainFileID());
        if (rewriteBuf)
            res = std::string(rewriteBuf->begin(), rewriteBuf->end());
    }
    remove(fn.c_str());
    return res;
}

std::string BlankHandlerConverter::genHostHandlers(const std::string &src, bool withHeading) {
    std::string res = src;
    std::string fn = prepareFile(src);
    {
        PassContext passCtx(fn);
        Rewriter *rewr = passCtx.getRewr();
        BlankPragmaHandler *pragmaHandler = new BlankPragmaHandler(*passCtx.getCompiler());
        passCtx.getPP()->AddPragmaHandler(pragmaHandler);
        Blank2HostConsumer astConsumer(*passCtx.getCompiler(), *rewr, pragmaHandler, fileCtx.getHostReqMap(), withHeading);
        passCtx.parse(&astConsumer);
        const RewriteBuffer *rewriteBuf = rewr->getRewriteBufferFor(rewr->getSourceMgr().getMainFileID());
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
