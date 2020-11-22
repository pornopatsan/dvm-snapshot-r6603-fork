#include "converter_debug.h"

#include "messages.h"

namespace cdvmh {

DebugASTVisitor::DebugASTVisitor(SourceFileContext &aFileCtx, CompilerInstance &aComp, Rewriter &R): 
    fileCtx(aFileCtx),
    projectCtx(fileCtx.getProjectCtx()), 
    opts(projectCtx.getOptions()), 
    comp(aComp), 
    rewr(R), 
    srcMgr(rewr.getSourceMgr()), 
    langOpts(rewr.getLangOpts()) 
{
    inFunction = false;
    inVarDecl = false;
    inLoopHeader = false;
    inParallelLoop = false;
    inArraySubscripts = false;

    curLoopNumber = 0;
}


bool DebugASTVisitor::VisitExpr(Expr *e) {
    if (!inFunction || inLoopHeader) {
        return true;
    }
    return HandleExpr(e);
}

bool DebugASTVisitor::VisitStmt(Stmt *s) {
    SourceLocation fileLoc = srcMgr.getFileLoc(s->getBeginLoc());
    FileID fileID = srcMgr.getFileID(fileLoc);
    int line = srcMgr.getLineNumber(fileID, srcMgr.getFileOffset(fileLoc));
    std::pair<Stmt*, std::string> loopStackInfo;

    if (ForStmt *curFor = llvm::dyn_cast<ForStmt>(s)) {
        loopNumbers[curFor] = ++curLoopNumber;
        inLoopHeader = true;

        std::set<std::string> loopVarNames;
        VarDecl *vd = getForLoopInitVar(curFor);
        assert(vd); // is consequent of upper part
        bool uniqueFlag = loopVarNames.insert(vd->getName().str()).second;

        loopStackInfo = std::pair<Stmt*, std::string>(curFor->getBody(), vd->getName());
        if (uniqueFlag) {
            cdvmhLog(DEBUG, srcMgr.getFilename(fileLoc).str(), line, "Debug pass: push loop index to stack: %s", vd->getName().data());
            loopVarsStack.push_back(loopStackInfo);
        }

        if (DvmPragma *gotPragma = fileCtx.getNextDebugPragma(fileID.getHashValue(), line, DvmPragma::pkParallel)) {
            if (gotPragma->kind == DvmPragma::pkParallel) {
                cdvmhLog(DEBUG, srcMgr.getFilename(fileLoc).str(), line, "Debug pass: enter parallel loop");
                parallelPragmas.insert(std::make_pair(line, gotPragma));
                inParallelLoop = true;
            }
        }
    }

    if (!loopVarsStack.empty() && s == loopVarsStack.back().first) {
        inLoopHeader = false;
    }

    return true;
}


bool DebugASTVisitor::TraverseStmt(Stmt *s) {
    bool res = base::TraverseStmt(s);
    if (!s) return res;

    SourceLocation fileLoc = srcMgr.getFileLoc(s->getBeginLoc());
    FileID fileID = srcMgr.getFileID(fileLoc);
    int line = srcMgr.getLineNumber(fileID, srcMgr.getFileOffset(fileLoc));

    while (!loopVarsStack.empty() && s == loopVarsStack.back().first) {
        cdvmhLog(DEBUG, srcMgr.getFilename(fileLoc).str(), line, "Debug pass: pop loop index from stack: %s", loopVarsStack.back().second.c_str());
        loopVarsStack.pop_back();
    }

    if (ForStmt *curLoop = llvm::dyn_cast<ForStmt>(s)) {
        bool isCurrentLoopParallel = forLoopIsParallel(curLoop);
        if (isCurrentLoopParallel) {
            inParallelLoop = false;
            cdvmhLog(DEBUG, srcMgr.getFilename(fileLoc).str(), line, "Debug pass: leave parallel loop");

            if (opts.seqOutput) {
                // Generate parallel loop debug handlers only in sequential mode,
                // as they will be also generated inside main loop handler
                genParLoopIter(curLoop);
                genParLoopCalls(curLoop, loopNumbers[curLoop]);
            } else if (!inParallelLoop) {
                fileCtx.loopNumbersByLine[line] = loopNumbers[curLoop];
            }
        } else if (!inParallelLoop) {
            genSeqLoopIter(curLoop);
            genSeqLoopCalls(curLoop, loopNumbers[curLoop]);
        }
    }

    return true;
}

bool DebugASTVisitor::VisitFunctionDecl(FunctionDecl *f) {
    const FunctionDecl *Definition = 0;
    bool hasBody = f->hasBody(Definition);
    bool bodyIsHere = hasBody && Definition == f;

    if (bodyIsHere) {
        inFunction = true;
    }

    return true;
}


bool DebugASTVisitor::TraverseFunctionDecl(FunctionDecl *f) {
    bool res = base::TraverseFunctionDecl(f);
    const FunctionDecl *Definition = 0;
    bool hasBody = f->hasBody(Definition);
    bool bodyIsHere = hasBody && Definition == f;

    if (bodyIsHere) {
        inFunction = false;
    }

    return res;
}

bool DebugASTVisitor::VisitVarDecl(VarDecl *vd) {
    if (!inFunction || inLoopHeader || isa<ParmVarDecl>(vd)) {
        return true;
    }

    SourceLocation fileLoc = srcMgr.getFileLoc(vd->getSourceRange().getBegin());
    FileID fileID = srcMgr.getFileID(fileLoc);
    int line = getLineFromLoc(vd->getSourceRange().getBegin());

    bool isDistribArray = false;
    if (DvmPragma *gotPragma = fileCtx.getNextDebugPragma(fileID.getHashValue(), line, DvmPragma::pkDistribArray)) {
        if (gotPragma->kind == DvmPragma::pkDistribArray) {
            distribArrays.insert(vd);
            isDistribArray = true;
        }
    }

    if (!vd->hasInit()) {
        return true;
    }

    inVarDecl = true;

    if (opts.dvmDebugLvl & (isDistribArray ? dlWriteArrays : dlWriteVariables)) {
        Expr* rhs = vd->getInit();
        SourceRange range = vd->getInit()->getSourceRange();
        VarState state = fillVarState(vd);

        SourceLocation startLoc = range.getBegin();
        SourceLocation endLoc = getNormalEndLoc(range.getEnd());
        std::string startInsert = genDvmWriteVarInit(rhs->getExprLoc(), state);
        std::string endInsert = ")";

        rewr.InsertTextBefore(getRealLoc(startLoc), startInsert);
        rewr.InsertTextAfter(getRealLoc(endLoc), endInsert);
    }

    return true;
}

bool DebugASTVisitor::TraverseVarDecl(VarDecl *vd) {
    bool res = base::TraverseVarDecl(vd);

    inVarDecl = false;

    return res;
}

bool DebugASTVisitor::HandleExpr(Expr *e) {

    if (BinaryOperator *op = llvm::dyn_cast<BinaryOperator>(e)) {
        if (DeclRefExpr *declRef = llvm::dyn_cast<DeclRefExpr>(op->getLHS())) {

            if (VarDecl *vd = llvm::dyn_cast<VarDecl>(declRef->getDecl())) {
                if (!isa<ParmVarDecl>(vd) && !isLoopVar(vd->getNameAsString())) {
                    VarState varState = fillVarState(vd);
                    if (!varState.isArray)
                        varWritings.insert(declRef);

                    bool isTraceNeeded = opts.dvmDebugLvl & dlWriteVariables;
                    if (!varState.isArray && isTraceNeeded) {
                        SourceLocation startLoc = op->getBeginLoc();
                        SourceLocation endLoc = getNormalEndLoc(op->getEndLoc());
                        std::string startInsert = genDvmWriteVar(startLoc, varState);
                        std::string endInsert = ") /*" + convertToString(e) + "*/";

                        startLoc = rewr.getSourceMgr().getFileLoc(startLoc);
                        endLoc = rewr.getSourceMgr().getFileLoc(endLoc);

                        //if (!startLoc.isMacroID() && !endLoc.isMacroID()) {
                            rewr.InsertTextBefore(getRealLoc(startLoc), startInsert);
                            rewr.InsertTextAfter(getRealLoc(endLoc), endInsert);
                        //} 
                    }
                }
            }

        } else if (ArraySubscriptExpr *arrAcc = llvm::dyn_cast<ArraySubscriptExpr>(op->getLHS())) {

            Expr *curExp = arrAcc;
            while (isa<ArraySubscriptExpr>(curExp)) {
                curExp = cast<ArraySubscriptExpr>(curExp)->getBase();
                if (isa<ImplicitCastExpr>(curExp))
                    curExp = cast<ImplicitCastExpr>(curExp)->getSubExpr();
            }

            if (DeclRefExpr *baseArrRef = llvm::dyn_cast<DeclRefExpr>(curExp)) {
                if (VarDecl *vd = llvm::dyn_cast<VarDecl>(baseArrRef->getDecl())) {
                    if (!isa<ParmVarDecl>(vd)) {
                        VarState varState = fillVarState(vd);
                        if (varState.isArray)
                            varWritings.insert(arrAcc);

                        bool isDistribArray = distribArrays.find(vd) != distribArrays.end();
                        DebugLevel level = isDistribArray ? dlWriteArrays : dlWriteVariables;
                        bool isTraceNeeded = opts.dvmDebugLvl & level;

                        if (varState.isArray && isTraceNeeded) {
                            std::string startInsert = genDvmWriteVarArray(baseArrRef->getLocation(), varState, convertToString(arrAcc));
                            std::string endInsert = ")";
                            SourceLocation startLoc = op->getBeginLoc();
                            SourceLocation endLoc = getNormalEndLoc(op->getEndLoc());

                            if (!startLoc.isMacroID() && !endLoc.isMacroID()) {
                                rewr.InsertTextBefore(getRealLoc(startLoc), startInsert);
                                rewr.InsertTextAfter(getRealLoc(endLoc), endInsert);
                            }
                       }
                    }
                }
            }

        }
        //!!genDvmRegArr(op);
    }
    // end BinaryOperator handling
    else if (DeclRefExpr *declRef = llvm::dyn_cast<DeclRefExpr>(e)) {

        if (VarDecl *vd = llvm::dyn_cast<VarDecl>(declRef->getDecl())) {
            if (!isa<ParmVarDecl>(vd)) {
                VarState varState = fillVarState(vd);
                if (!varState.isArray && !isLoopVar(vd->getNameAsString())) {
                    bool isInVarWritings = varWritings.find(declRef) != varWritings.end();
                    bool isTraceNeeded = opts.dvmDebugLvl & dlReadVariables;

                    if (isTraceNeeded && !isInVarWritings && !inVarDecl) {
                        SourceRange range = declRef->getSourceRange();

                        SourceLocation startLoc = range.getBegin();
                        SourceLocation endLoc = getNormalEndLoc(range.getEnd());

                        std::string startInsert = genDvmReadVar(startLoc, varState);
                        std::string endInsert = ") /*" + convertToString(e) + "*/";

                        rewr.InsertTextBefore(startLoc, startInsert);
                        rewr.InsertTextAfter(endLoc, endInsert);
                    }
                }
            }
        }
    }
    // end DeclRefExpr handling
    else if (ArraySubscriptExpr *arrAcc = llvm::dyn_cast<ArraySubscriptExpr>(e)) {

        Expr *curExp = arrAcc;
        while (isa<ArraySubscriptExpr>(curExp)) {
            curExp = cast<ArraySubscriptExpr>(curExp)->getBase();
            if (isa<ImplicitCastExpr>(curExp))
                curExp = cast<ImplicitCastExpr>(curExp)->getSubExpr();
            if (isa<ArraySubscriptExpr>(curExp))
                currentSubscripts.insert(curExp);
        }

        if (DeclRefExpr *baseArrRef = llvm::dyn_cast<DeclRefExpr>(curExp)) {
            if (VarDecl *vd = llvm::dyn_cast<VarDecl>(baseArrRef->getDecl())) {
                if (!isa<ParmVarDecl>(vd)) {
                    VarState varState = fillVarState(vd);

                    bool isInVarWritings = varWritings.find(arrAcc) != varWritings.end();
                    bool inNestedSubscripts = currentSubscripts.find(arrAcc) != currentSubscripts.end();
                    bool isDistribArray = distribArrays.find(vd) != distribArrays.end();

                    if (inNestedSubscripts) {
                        currentSubscripts.erase(arrAcc);
                        return true;
                    }

                    DebugLevel level = isDistribArray ? dlReadArrays : dlReadVariables;
                    bool isTraceNeeded = opts.dvmDebugLvl & level;

                    if (varState.isArray && isTraceNeeded && !isInVarWritings && !inVarDecl) {
                        SourceRange range = arrAcc->getSourceRange();

                        SourceLocation startLoc = range.getBegin();
                        SourceLocation endLoc = getNormalEndLoc(range.getEnd());

                        std::string startInsert = genDvmReadVarArray(baseArrRef->getLocation(), varState, convertToString(arrAcc));
                        std::string endInsert = ")";

                        rewr.InsertTextBefore(startLoc, startInsert);
                        rewr.InsertTextAfter(endLoc, endInsert);
                    }
                }
            }
        }
    }

    return true;
}

VarState DebugASTVisitor::fillVarState(VarDecl *vd) {
    SourceLocation fileLoc = srcMgr.getFileLoc(vd->getLocation());
    std::string fileName = srcMgr.getFilename(fileLoc).str();
    FileID fileID = srcMgr.getFileID(fileLoc);
    int line = srcMgr.getLineNumber(fileID, srcMgr.getFileOffset(fileLoc));

    std::string varName = vd->getName().str();
    checkIntErrN(varStates.find(vd) == varStates.end(), 95, varName.c_str(), fileName.c_str(), line);
    bool hasRestrict = vd->getType().isRestrictQualified();
    const Type *baseType = vd->getType().getUnqualifiedType().getDesugaredType(comp.getASTContext()).split().Ty;
    std::vector<MyExpr> sizes;

    if (baseType->isPointerType() || isa<IncompleteArrayType>(baseType)) {
        sizes.push_back(MyExpr());
        if (baseType->isPointerType())
            baseType = baseType->getPointeeType().getUnqualifiedType().getDesugaredType(comp.getASTContext()).split().Ty;
        else
            baseType = cast<IncompleteArrayType>(baseType)->getArrayElementTypeNoTypeQual();
        cdvmhLog(DONT_LOG, fileName, line, "Outer pointer/incomplete array type found");
    }

    while (baseType->isArrayType()) {
        //checkUserErr(baseType->getAsArrayTypeUnsafe()->getSizeModifier() == ArrayType::Normal, fileName, line,
        //    "That kind of array size modifier is not supported for variable '%s'", varName.c_str());
        MyExpr nextSize;
        if (const ConstantArrayType *ca = llvm::dyn_cast<const ConstantArrayType>(baseType)) {
            nextSize.strExpr = toStr(ca->getSize().getZExtValue());
        } else if (const VariableArrayType *va = llvm::dyn_cast<const VariableArrayType>(baseType)) {
            Expr *e = va->getSizeExpr();
            // TODO: Fill somehow info on references in this expression
            nextSize.strExpr = convertToString(e);
        } else if (const DependentSizedArrayType *dsa = llvm::dyn_cast<const DependentSizedArrayType>(baseType)) {
            nextSize.strExpr = convertToString(dsa->getSizeExpr());
        } else {
            //checkUserErr(false, fileName, line,
            //    "That kind of array type is not supported for variable '%s'", varName.c_str());
            cdvmh_log(WARNING, 52, MSG(52), baseType->getTypeClassName());
            nextSize.strExpr = "0";
        }
        sizes.push_back(nextSize);
        baseType = baseType->getArrayElementTypeNoTypeQual();
    }

    std::string typeName = QualType(baseType, 0).getAsString();
    // XXX: dirty
    if (typeName == "_Bool" && fileCtx.getInputFile().CPlusPlus)
        typeName = "bool";

    VarState varState;
    varState.init(varName, typeName, sizes);
    if (strstr(baseType->getCanonicalTypeInternal().getAsString().c_str(), "type-parameter-"))
        varState.hasDependentBaseType = true;
    if (hasRestrict)
        varState.canBeRestrict = true;

    return varState;
}

SourceLocation DebugASTVisitor::escapeMacroBegin(SourceLocation loc) {
    bool ok = true;
    while (loc.isMacroID()) {
        ok = Lexer::isAtStartOfMacroExpansion(loc, srcMgr, langOpts, &loc);
        if (!ok) break;
    }
    if (!ok) {
        PresumedLoc ploc = srcMgr.getPresumedLoc(srcMgr.getFileLoc(loc));
        userErrN(ploc.getFilename(), ploc.getLine(), 53);
    }
    return loc;
}

SourceLocation DebugASTVisitor::escapeMacroEnd(SourceLocation loc) {
    bool ok = true;
    while (loc.isMacroID()) {
        ok = Lexer::isAtEndOfMacroExpansion(loc, srcMgr, langOpts, &loc);
        if (!ok) break;
    }
    if (!ok) {
        PresumedLoc ploc = srcMgr.getPresumedLoc(srcMgr.getFileLoc(loc));
        userErrN(ploc.getFilename(), ploc.getLine(), 53);
    }
    return loc;
}

SourceLocation DebugASTVisitor::getNormalEndLoc(SourceLocation loc) {
    SourceLocation endLoc = Lexer::getLocForEndOfToken(loc, 0, rewr.getSourceMgr(), rewr.getLangOpts());
    if (!endLoc.isValid()) {
        int offset = Lexer::MeasureTokenLength(loc, rewr.getSourceMgr(), rewr.getLangOpts());
        return loc.getLocWithOffset(offset);
    } else {
        return endLoc;
    }
}

SourceLocation DebugASTVisitor::getNormalStmtEndLoc(Stmt *s) {
    SourceLocation endLoc = getNormalEndLoc(s->getEndLoc());
    Token token;

    Lexer::getRawToken(endLoc.getLocWithOffset(0), token, srcMgr, rewr.getLangOpts());
    if (token.getKind() == tok::semi) {
        return endLoc.getLocWithOffset(1);
    }

    Lexer::getRawToken(endLoc.getLocWithOffset(-1), token, srcMgr, rewr.getLangOpts());
    if (token.getKind() != tok::r_brace) {
        endLoc = Lexer::findLocationAfterToken(endLoc, tok::semi, srcMgr, rewr.getLangOpts(), false);
    }
    return endLoc;
}

SourceLocation DebugASTVisitor::getRealLoc(SourceLocation loc) {
    return rewr.getSourceMgr().getFileLoc(loc);
}

bool DebugASTVisitor::isLoopVar(const std::string &name) const {
    for (int i = 0; i < (int)loopVarsStack.size(); ++i) {
        if (name == loopVarsStack[i].second) 
            return true;
    }
    return false;
}


std::string DebugASTVisitor::getFileName(const SourceLocation &loc) {
    PresumedLoc ploc = srcMgr.getPresumedLoc(srcMgr.getFileLoc(loc));
    return ploc.getFilename();
}

unsigned DebugASTVisitor::getLineFromLoc(const SourceLocation &loc) {
    PresumedLoc ploc = srcMgr.getPresumedLoc(srcMgr.getFileLoc(loc));
    return ploc.getLine();
}

std::string DebugASTVisitor::getStmtIndent(Stmt *s) {
    SourceLocation loc = s->getBeginLoc();
    SourceLocation fileLoc = srcMgr.getFileLoc(loc);
    FileID fileID = srcMgr.getFileID(fileLoc);
    unsigned line = getLineFromLoc(loc);
    const char *data = srcMgr.getCharacterData(srcMgr.translateLineCol(fileID, line, 1));
    std::string indent = extractIndent(data);

    return indent;
}

bool DebugASTVisitor::forLoopIsParallel(ForStmt *curLoop) {
    return forLoopParallelPragma(curLoop) != NULL;
}

DvmPragma* DebugASTVisitor::forLoopParallelPragma(ForStmt *curLoop) {
    int line = getLineFromLoc(curLoop->getBeginLoc());
    if (parallelPragmas.find(line) != parallelPragmas.end()) {
        return parallelPragmas.find(line)->second;
    } else {
        return NULL;
    }
}

VarDecl* DebugASTVisitor::getForLoopInitVar(ForStmt *curLoop) {
    VarDecl *vd = 0;

    Stmt *init = curLoop->getInit();
    if (BinaryOperator *binOp = llvm::dyn_cast<BinaryOperator>(init)) {
        if (DeclRefExpr *declRef = llvm::dyn_cast<DeclRefExpr>(binOp->getLHS())) {
            vd = llvm::dyn_cast<VarDecl>(declRef->getDecl());
        }
    } else if (DeclStmt *declStmt = llvm::dyn_cast<DeclStmt>(init)) {
        if (declStmt->isSingleDecl()) {
            vd = llvm::dyn_cast<VarDecl>(declStmt->getSingleDecl());
        }
    }

    return vd;
}

std::string DebugASTVisitor::getLoopLowerBound(ForStmt *curLoop) {
    std::string result = "";
    Stmt *init = curLoop->getInit();
    if (BinaryOperator *binOp = llvm::dyn_cast<BinaryOperator>(init)) {
        result = convertToString(binOp->getRHS());
    } else if (DeclStmt *declStmt = llvm::dyn_cast<DeclStmt>(init)) {
        if (declStmt->isSingleDecl()) {
            if (VarDecl *vd = llvm::dyn_cast<VarDecl>(declStmt->getSingleDecl())) {
                result = convertToString(vd->getInit());
            }
    }
    }
    return result;
}

std::string DebugASTVisitor::getLoopUpperBound(ForStmt *curLoop) {
    std::string result = "";
    Expr *cond = curLoop->getCond();
    if (BinaryOperator *binOp = llvm::dyn_cast<BinaryOperator>(cond)) {
        // BO_LT, BO_GT, BO_LE, BO_GE
    BinaryOperatorKind operation = binOp->getOpcode();
        std::string rhs = convertToString(binOp->getRHS());
    if (operation == BO_LE || operation == BO_GE) {
            result = rhs;
        } else if (operation == BO_LT) {
            result = rhs + " - 1";
    } else if (operation == BO_GT) {
            result = rhs + " + 1";
    }
    }
    return result;
}

std::string DebugASTVisitor::getLoopStep(ForStmt *curLoop) {
    std::string result = "";
    Expr *increment = curLoop->getInc();
    if (BinaryOperator *binOp = llvm::dyn_cast<BinaryOperator>(increment)) {
        // BO_AddAssign, BO_SubAssign
        BinaryOperatorKind operation = binOp->getOpcode();
        std::string rhs = convertToString(binOp->getRHS());
        if (operation == BO_AddAssign) {
            result = rhs;
        } else if (operation == BO_SubAssign) {
            result = "-(" + rhs + ")";
        }
    } else if (UnaryOperator *unOp = llvm::dyn_cast<UnaryOperator>(increment)) {
        // UO_PostInc, UO_PostDec, UO_PreInc, UO_PreDec
        UnaryOperatorKind operation = unOp->getOpcode();
        if (operation == UO_PostInc || operation == UO_PreInc) {
            result = "1";
        } else if (operation == UO_PostDec || operation == UO_PreDec) {
            result = "-1";
        }
    }

    return result;
}

std::string DebugASTVisitor::getLoopBounds(ForStmt *curLoop) {
    std::string lowerBoundList;
    std::string upperBoundList;
    std::string stepList;

    Stmt *curStmt = curLoop;
    int loopRank = 0;
    for (;;) {
        if (ForStmt *forStmt = llvm::dyn_cast<ForStmt>(curStmt)) {
            lowerBoundList += getLoopLowerBound(forStmt) + ", ";
            upperBoundList += getLoopUpperBound(forStmt) + ", ";
            stepList += getLoopStep(forStmt) + ", ";
            ++loopRank;
            curStmt = forStmt->getBody();
        } else if (CompoundStmt *compoundStmt = llvm::dyn_cast<CompoundStmt>(curStmt)) {
            curStmt = *compoundStmt->body_begin();
        } else {
            stepList = deleteTrailingComma(stepList);
            break;
        }
    }

    std::string loopBounds = toStr(loopRank) + ", " + lowerBoundList + upperBoundList + stepList;
    return loopBounds;
}

std::string DebugASTVisitor::genDvmReadVar(SourceLocation loc, const VarState &state) {
    std::string fileName = "\"" + getFileName(loc) + "\"";
    std::string line = toStr(getLineFromLoc(loc));
    std::string varType = genRtType(state.baseTypeStr);
    std::string base = "NULL";

    std::string toInsert = "DVMH_DBG_READ_VAR(" + fileName + ", " + line + ", " + varType + ", " + base + ", ";
    return toInsert;
}

std::string DebugASTVisitor::genDvmReadVarArray(SourceLocation loc, const VarState &state, const std::string &ref_name) {
    std::string fileName = "\"" + getFileName(loc) + "\"";
    std::string line = toStr(getLineFromLoc(loc));
    std::string varType = genRtType(state.baseTypeStr);
    std::string base = state.name;

    std::string toInsert = "DVMH_DBG_READ_VAR(" + fileName + ", " + line + ", " + varType + ", " + base + ", ";
    return toInsert;
}

std::string DebugASTVisitor::genDvmWriteVar(SourceLocation loc, const VarState &state) {
    std::string fileName = "\"" + getFileName(loc) + "\"";
    std::string line = toStr(getLineFromLoc(loc));
    std::string varName = state.name;
    std::string varType = genRtType(state.baseTypeStr);
    std::string base = "NULL";

    std::string toInsert = "DVMH_DBG_WRITE_VAR(" + fileName + ", " + line + ", " + varName + ", " + base + ", " + varType + ", ";
    return toInsert;
}

std::string DebugASTVisitor::genDvmWriteVarInit(SourceLocation loc, const VarState &state) {
    std::string fileName = "\"" + getFileName(loc) + "\"";
    std::string line = toStr( getLineFromLoc(loc) );
    std::string name = state.name;
    std::string varType = genRtType(state.baseTypeStr);
    std::string base = "NULL";

    std::string toInsert = "DVMH_DBG_INIT_VAR(" + fileName + ", " + line + ", " + name + ", " + base + ", " + varType + ", ";
    return toInsert;
}

std::string DebugASTVisitor::genDvmWriteVarArray(SourceLocation loc, const VarState &state, const std::string &refName) {
    std::string fileName = "\"" + getFileName(loc) + "\"";
    std::string line = toStr(getLineFromLoc(loc));
    std::string varName = refName;
    std::string varType = genRtType(state.baseTypeStr);
    std::string base = state.name;

    std::string toInsert = "DVMH_DBG_WRITE_VAR(" + fileName + ", " + line + ", " + varName + ", " + base + ", " + varType + ", ";
    return toInsert;
}
 
void DebugASTVisitor::genSeqLoopCalls(ForStmt *curLoop, int curLoopNumber) {
    SourceLocation startLoc = curLoop->getBeginLoc();
    SourceLocation endLoc = getNormalStmtEndLoc(curLoop);

    std::string file = "\"" + getFileName(curLoop->getBeginLoc()) + "\"";
    std::string line = toStr(getLineFromLoc(curLoop->getBeginLoc()));
    std::string loopNum = toStr(curLoopNumber);

    std::string startLoopCall = "DVMH_DBG_LOOP_SEQ_START(" + file + ", " + line + ", " + loopNum + ");";
    std::string endLoopCall = "DVMH_DBG_LOOP_END(" + file + ", " + line + ", " + loopNum + ");";
    std::string startInsert = "{ " + startLoopCall + " ";
    std::string endInsert = " " + endLoopCall + " }";

    rewr.InsertText(startLoc, startInsert, false, true);
    rewr.InsertText(endLoc, endInsert, true, false);
}

void DebugASTVisitor::genSeqLoopIter(ForStmt *curLoop) {
    VarDecl *vd = getForLoopInitVar(curLoop);
    if (vd != 0) {
        VarState state = fillVarState(vd);

        Stmt *loopBody = curLoop->getBody();
        SourceLocation startLocIter = loopBody->getBeginLoc();
        SourceLocation endLocIter = getNormalStmtEndLoc(loopBody);

        std::string fileName = "\"" + getFileName(curLoop->getBeginLoc()) + "\"";
        std::string line = toStr(getLineFromLoc(curLoop->getBeginLoc()));
        std::string rank = "1";
        std::string varType = "(DvmType)&" + state.name + ", " + genRtType(state.baseTypeStr);
        std::string iterCall = "DVMH_DBG_LOOP_ITER(" + fileName + ", " + line + ", (" + rank + ", " + varType + "));";
        std::string startInsertIter = "{ " + iterCall + " ";
        std::string endInsertIter = " }";

        rewr.InsertText(startLocIter, startInsertIter, false, true);
        rewr.InsertText(endLocIter, endInsertIter, true, false);
    }
}

void DebugASTVisitor::genParLoopCalls(ForStmt *curLoop, int curLoopNumber) {
    SourceLocation loopLoc = curLoop->getBeginLoc();
    int line = getLineFromLoc(loopLoc);

    // Get location before "parallel" pragma
    DvmPragma* pragma = parallelPragmas[line];
    SourceLocation startLoc = srcMgr.translateLineCol(srcMgr.getFileID(loopLoc), pragma->line, 1);
    SourceLocation endLoc = getNormalStmtEndLoc(curLoop);

    std::string file = "\"" + getFileName(loopLoc) + "\"";
    std::string lineStr = toStr(line);
    std::string loopBounds = getLoopBounds(curLoop);
    std::string loopNum = toStr(curLoopNumber);
    std::string indent = getStmtIndent(curLoop);

    std::string startLoopCall = "DVMH_DBG_LOOP_PAR_START(" + file + ", " + lineStr + ", (" + loopNum + ", " + loopBounds + "));";
    std::string endLoopCall = "DVMH_DBG_LOOP_END(" + file + ", " + lineStr + ", " + loopNum + ");";

    std::string startInsert = indent + "{ " + startLoopCall + "//"; // Comment "parallel" pragma
    std::string endInsert = " " + endLoopCall + " }";

    rewr.InsertText(startLoc, startInsert, false, false);
    rewr.InsertText(endLoc, endInsert, true, false);
}

void DebugASTVisitor::genParLoopIter(ForStmt *curLoop) {
    std::string varNames;
    std::string varTypes;
    int loopRank = 0;

    Stmt *curStmt = curLoop;
    for (;;) {
        if (ForStmt *forStmt = llvm::dyn_cast<ForStmt>(curStmt)) {
            VarDecl *vd = getForLoopInitVar(forStmt);
            if (vd != 0) {
                VarState state = fillVarState(vd);
                varNames += "(DvmType)&" + state.name + ", ";
                varTypes += genRtType(state.baseTypeStr) + ", ";
            }
            ++loopRank;
            curStmt = forStmt->getBody();
        } else if (CompoundStmt *compoundStmt = llvm::dyn_cast<CompoundStmt>(curStmt)) {
            Stmt* compoundChild = *compoundStmt->body_begin();
            if (!isa<ForStmt>(compoundChild)) {
                break;
            } else {
                curStmt = compoundChild;
            }
        } else {
            break;
        }
    }

    std::string varNameTypes = deleteTrailingComma(varNames + varTypes);

    Stmt *innerLoopBody = curStmt;
    SourceLocation startLoc = innerLoopBody->getBeginLoc();
    SourceLocation endLoc = getNormalStmtEndLoc(innerLoopBody);

    std::string file = "\"" + getFileName(curLoop->getBeginLoc()) + "\"";
    std::string line = toStr(getLineFromLoc(curLoop->getBeginLoc()));
    std::string rank = toStr(loopRank);

    std::string iterCall = "DVMH_DBG_LOOP_ITER(" + file + ", " + line + ", (" + rank + ", " + varNameTypes + "));";

    std::string startInsert = "{ " + iterCall + " ";
    std::string endInsert = " }";

    rewr.InsertText(startLoc, startInsert, false, true);
    rewr.InsertText(endLoc, endInsert, true, false);
}


}
