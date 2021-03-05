#include "converter.h"

#include <cstdio>

#include "aux_visitors.h"
#include "messages.h"

namespace cdvmh {

// ConverterASTVisitor

void ConverterASTVisitor::genActuals(FileID fileID, int line) {
    // Seek for GET_ACTUAL and ACTUAL pragmas
    while (DvmPragma *gotPragma = fileCtx.getNextPragma(fileID.getHashValue(), line, DvmPragma::pkGetActual, DvmPragma::pkSetActual)) {
        PragmaGetSetActual *curPragma = (PragmaGetSetActual *)gotPragma;
        //checkDirErrN(fileID == srcMgr.getMainFileID(), 451);
        checkDirErrN(fileCtx.isCompilable(), 452);
        checkDirErrN(!inParLoop, 453);
        SourceLocation loc(srcMgr.translateLineCol(fileID, curPragma->srcLine + curPragma->srcLineSpan, 1));
        std::string indent = extractIndent(srcMgr.getCharacterData(srcMgr.translateLineCol(fileID, curPragma->srcLine, 1)));
        std::string act;
        if (curPragma->kind == DvmPragma::pkGetActual)
            act = "get_actual";
        else
            act = "actual";
        std::string toInsert;
        if (!opts.lessDvmLines)
            toInsert += indent + genDvmLine(curPragma) + "\n";
        for (int i = 0; i < (int)curPragma->vars.size(); i++) {
            SlicedArray *arr = &curPragma->vars[i];
            VarDecl *vd = seekVarDecl(arr->name);
            checkDirErrN(vd, 301, arr->name.c_str());
            checkIntErrN(varStates.find(vd) != varStates.end(), 92, vd->getNameAsString().c_str());
            VarState *varState = &varStates[vd];
            bool isDvmArray = varState->isDvmArray;
            bool isArray = varState->isArray;
            if (!arr->slicedFlag) {
                if (!isDvmArray)
                    toInsert += indent + "dvmh_" + act + "_variable2_((" + (curPragma->kind == DvmPragma::pkGetActual ? "" : "const ") + "void *)" +
                            (isArray ? "" : "&") + arr->name + ");\n";
                else
                    toInsert += indent + "dvmh_" + act + "_array2_(" + arr->name + ");\n";
            } else {
                checkDirErrN(isArray, 351, arr->name.c_str());
                int rank = arr->bounds.size();
                checkDirErrN(rank == varState->rank, 352, arr->name.c_str());
                std::string paramsStr;
                if (!isDvmArray)
                    paramsStr += std::string("(") + (curPragma->kind == DvmPragma::pkGetActual ? "" : "const ") + "void *)";
                paramsStr += arr->name;
                paramsStr += ", " + toStr(arr->bounds.size());
                for (int j = 0; j < (int)arr->bounds.size(); j++) {
                    MyExpr lb = arr->bounds[j].first;
                    MyExpr rb = arr->bounds[j].second;
                    checkNonDvmExpr(lb, curPragma);
                    checkNonDvmExpr(rb, curPragma);
                    paramsStr += ", " + fileCtx.dvm0c(lb.empty() ? "UNDEF_BOUND" : lb.strExpr) + ", " + fileCtx.dvm0c(rb.empty() ? "UNDEF_BOUND" : rb.strExpr);
                }
                if (!isDvmArray)
                    toInsert += indent + "dvmh_" + act + "_subvariable_C(" + paramsStr + ");\n";
                else
                    toInsert += indent + "dvmh_" + act + "_subarray_C(" + paramsStr + ");\n";
            }
        }
        toInsert += "\n";
        rewr.InsertText(loc, toInsert, false, false);
    }
}

void ConverterASTVisitor::genRedistributes(FileID fileID, int line) {
    // Seek for REDISTRIBUTE pragmas
    while (DvmPragma *gotPragma = fileCtx.getNextPragma(fileID.getHashValue(), line, DvmPragma::pkRedistribute)) {
        PragmaRedistribute *curPragma = (PragmaRedistribute *)gotPragma;
        //checkDirErrN(fileID == srcMgr.getMainFileID(), 421);
        checkDirErrN(fileCtx.isCompilable(), 422);
        checkDirErrN(!inRegion && !inParLoop, 423);
        SourceLocation loc(srcMgr.translateLineCol(fileID, curPragma->srcLine + curPragma->srcLineSpan, 1));
        std::string indent = extractIndent(srcMgr.getCharacterData(srcMgr.translateLineCol(fileID, curPragma->srcLine, 1)));
        VarDecl *vd = seekVarDecl(curPragma->name);
        checkDirErrN(vd, 301, curPragma->name.c_str());
        checkIntErrN(varStates.find(vd) != varStates.end(), 92, vd->getNameAsString().c_str());
        VarState *varState = &varStates[vd];
        checkDirErrN(varState->isDvmArray || varState->isTemplate, 321);
        checkDirErrN(varState->rank == curPragma->rank, 304, varState->name.c_str());
        std::string toInsert;
        if (!opts.lessDvmLines)
            toInsert += indent + genDvmLine(curPragma) + "\n";
        toInsert += indent + "dvmh_redistribute_C(" + curPragma->name + ", " + genDistribParams(curPragma, &curPragma->distribRule) + ");\n";
        toInsert += "\n";
        rewr.InsertText(loc, toInsert, false, false);
    }
}

void ConverterASTVisitor::genRealignes(FileID fileID, int line) {
    // Seek for REALIGN pragmas
    while (DvmPragma *gotPragma = fileCtx.getNextPragma(fileID.getHashValue(), line, DvmPragma::pkRealign)) {
        PragmaRealign *curPragma = (PragmaRealign *)gotPragma;
        //checkDirErrN(fileID == srcMgr.getMainFileID(), 424);
        checkDirErrN(fileCtx.isCompilable(), 425);
        checkDirErrN(!inRegion && !inParLoop, 426);
        SourceLocation loc(srcMgr.translateLineCol(fileID, curPragma->srcLine + curPragma->srcLineSpan, 1));
        std::string indent = extractIndent(srcMgr.getCharacterData(srcMgr.translateLineCol(fileID, curPragma->srcLine, 1)));
        VarDecl *vd = seekVarDecl(curPragma->name);
        checkDirErrN(vd, 301, curPragma->name.c_str());
        checkIntErrN(varStates.find(vd) != varStates.end(), 92, vd->getNameAsString().c_str());
        VarState *varState = &varStates[vd];
        checkDirErrN(varState->isDvmArray, 322);
        checkDirErrN(varState->rank == curPragma->rank, 304, varState->name.c_str());
        std::string toInsert;
        if (!opts.lessDvmLines)
            toInsert += indent + genDvmLine(curPragma) + "\n";
        toInsert += indent + "dvmh_realign_C(" + curPragma->name + ", " + toStr(curPragma->newValueFlag ? 1 : 0) + ", " +
                genAlignParams(curPragma, &curPragma->alignRule) + ");\n";
        toInsert += "\n";
        rewr.InsertText(loc, toInsert, false, false);
    }
}

void ConverterASTVisitor::genIntervals(FileID fileID, int line) {
    // Seek for INTERVAL pragmas
    while (DvmPragma *curPragma = fileCtx.getNextPragma(fileID.getHashValue(), line, DvmPragma::pkInterval, DvmPragma::pkEndInterval, DvmPragma::pkExitInterval)) {
        SourceLocation loc(srcMgr.translateLineCol(fileID, curPragma->srcLine + curPragma->srcLineSpan, 1));
        std::string indent = extractIndent(srcMgr.getCharacterData(srcMgr.translateLineCol(fileID, curPragma->srcLine, 1)));
        std::string toInsert;

        if (!opts.lessDvmLines)
            toInsert += indent + genDvmLine(curPragma) + "\n";

        if (curPragma->kind == DvmPragma::pkInterval) {
            // Interval
            PragmaInterval *pragma = (PragmaInterval *)curPragma;
            cdvmhLog(DEBUG, pragma->fileName, pragma->line, "Entering interval");
            checkDirErrN(fileCtx.isCompilable(), 393);
            checkDirErrN(!inRegion && !inParLoop, 394);
            intervalStack.push_back(std::make_pair(pragma->userID, declContexts.back()));
            toInsert += indent + "dvmh_usr_interval_start_C(" + toStr(pragma->userID.strExpr) + ");\n";
        } else if (curPragma->kind == DvmPragma::pkEndInterval) {
            // EndInterval
            cdvmhLog(DEBUG, curPragma->fileName, curPragma->line, "Leaving interval");
            checkDirErrN(fileCtx.isCompilable(), 396);
            checkDirErrN(!inRegion && !inParLoop, 397);
            checkDirErrN(intervalStack.back().second == declContexts.back(), 398);
            intervalStack.pop_back();
            toInsert += indent + "dvmh_usr_interval_end_();\n";
        } else {
            // ExitInterval
            PragmaExitInterval *pragma = (PragmaExitInterval *)curPragma;
            cdvmhLog(DEBUG, pragma->fileName, pragma->line, "Exiting %lu intervals", pragma->ids.size());
            checkDirErrN(fileCtx.isCompilable(), 396);
            checkDirErrN(!inRegion && !inParLoop, 397);
            checkDirErrN(intervalStack.size() >= pragma->ids.size(), 3910);
            for (int i = 0; i < (int)pragma->ids.size(); i++) {
                MyExpr storedId = intervalStack[intervalStack.size() - i - 1].first;
                checkDirErrN(storedId == pragma->ids[i], 3911, pragma->ids[i].strExpr.c_str());
                toInsert += indent + "dvmh_usr_interval_end_();\n";
            }
        }

        toInsert += "\n";
        rewr.InsertText(loc, toInsert, false, false);
    }
}

void ConverterASTVisitor::genShadowAdds(FileID fileID, int line) {
    // Seek for SHADOW_ADD pragmas
    while (DvmPragma *gotPragma = fileCtx.getNextPragma(fileID.getHashValue(), line, DvmPragma::pkShadowAdd)) {
        PragmaShadowAdd *curPragma = (PragmaShadowAdd *)gotPragma;
        checkDirErrN(opts.enableIndirect, 323, "Adding indirect shadow");
        checkDirErr(!inRegion && !inParLoop, "shadow_add directive is not allowed in region or parallel loop");
        SourceLocation loc(srcMgr.translateLineCol(fileID, curPragma->srcLine + curPragma->srcLineSpan, 1));
        std::string indent = extractIndent(srcMgr.getCharacterData(srcMgr.translateLineCol(fileID, curPragma->srcLine, 1)));
        VarDecl *vd = seekVarDecl(curPragma->targetName);
        checkDirErrN(vd, 301, curPragma->targetName.c_str());
        checkIntErrN(varStates.find(vd) != varStates.end(), 92, vd->getNameAsString().c_str());
        VarState *varState = &varStates[vd];
        checkDirErr(varState->isDvmArray || varState->isTemplate, "Only DVM-array or template can be shadow_add'ed");
        checkDirErrN(varState->rank == curPragma->rank, 304, varState->name.c_str());
        std::string toInsert;
        if (!opts.lessDvmLines)
            toInsert += indent + genDvmLine(curPragma) + "\n";
        toInsert += indent + "dvmh_indirect_shadow_add_C(" + curPragma->targetName + ", " + toStr(curPragma->ruleAxis) + ", " +
                genDerivedAxisParams(curPragma, curPragma->rule) + ", " + "\"" + escapeStr(curPragma->shadowName) + "\", " +
                toStr(curPragma->includeList.size());
        for (int i = 0; i < (int)curPragma->includeList.size(); i++) {
            std::string name = curPragma->includeList[i];
            VarDecl *vd = seekVarDecl(name);
            checkDirErrN(vd, 301, name.c_str());
            checkIntErrN(varStates.find(vd) != varStates.end(), 92, vd->getNameAsString().c_str());
            VarState *varState = &varStates[vd];
            checkDirErr(varState->isDvmArray, "Only DVM-array can be shadow_include'ed");
            toInsert += ", " + name;
        }
        toInsert += ");\n";
        toInsert += "\n";
        rewr.InsertText(loc, toInsert, false, false);
    }
}

void ConverterASTVisitor::genLocalizes(FileID fileID, int line) {
    // Seek for LOCALIZE pragmas
    while (DvmPragma *gotPragma = fileCtx.getNextPragma(fileID.getHashValue(), line, DvmPragma::pkLocalize)) {
        PragmaLocalize *curPragma = (PragmaLocalize *)gotPragma;
        checkDirErr(!inRegion && !inParLoop, "localize directive is not allowed in region or parallel loop");
        SourceLocation loc(srcMgr.translateLineCol(fileID, curPragma->srcLine + curPragma->srcLineSpan, 1));
        std::string indent = extractIndent(srcMgr.getCharacterData(srcMgr.translateLineCol(fileID, curPragma->srcLine, 1)));
        VarDecl *vd = seekVarDecl(curPragma->refName);
        checkDirErrN(vd, 301, curPragma->refName.c_str());
        checkIntErrN(varStates.find(vd) != varStates.end(), 92, vd->getNameAsString().c_str());
        VarState *varState = &varStates[vd];
        checkDirErr(varState->isDvmArray, "Only DVM-array can be localized");
        vd = seekVarDecl(curPragma->targetName);
        checkDirErrN(vd, 301, curPragma->refName.c_str());
        checkIntErrN(varStates.find(vd) != varStates.end(), 92, vd->getNameAsString().c_str());
        varState = &varStates[vd];
        checkDirErr(varState->isDvmArray, "Only DVM-array can be a localization target");
        int targetAxis = -1;
        if (curPragma->targetRank == 0) {
            checkDirErrN(varState->rank == 1, 382);
            targetAxis = 1;
        } else {
            checkDirErrN(curPragma->targetRank == varState->rank, 304, curPragma->targetName.c_str());
            targetAxis = curPragma->targetAxis;
        }
        std::string toInsert;
        if (!opts.lessDvmLines)
            toInsert += indent + genDvmLine(curPragma) + "\n";
        toInsert += indent + "dvmh_indirect_localize_C(" + curPragma->refName + ", " + curPragma->targetName + ", " + toStr(targetAxis) + ");\n";
        toInsert += "\n";
        rewr.InsertText(loc, toInsert, false, false);
    }
}

void ConverterASTVisitor::genUnlocalizes(FileID fileID, int line) {
    // Seek for UNLOCALIZE pragmas
    while (DvmPragma *gotPragma = fileCtx.getNextPragma(fileID.getHashValue(), line, DvmPragma::pkUnlocalize)) {
        PragmaUnlocalize *curPragma = (PragmaUnlocalize *)gotPragma;
        checkDirErr(!inRegion && !inParLoop, "unlocalize directive is not allowed in region or parallel loop");
        SourceLocation loc(srcMgr.translateLineCol(fileID, curPragma->srcLine + curPragma->srcLineSpan, 1));
        std::string indent = extractIndent(srcMgr.getCharacterData(srcMgr.translateLineCol(fileID, curPragma->srcLine, 1)));
        std::string toInsert;
        if (!opts.lessDvmLines)
            toInsert += indent + genDvmLine(curPragma) + "\n";
        for (int i = 0; i < (int)curPragma->nameList.size(); i++) {
            std::string name = curPragma->nameList[i];
            VarDecl *vd = seekVarDecl(name);
            checkDirErrN(vd, 301, name.c_str());
            checkIntErrN(varStates.find(vd) != varStates.end(), 92, vd->getNameAsString().c_str());
            VarState *varState = &varStates[vd];
            checkDirErr(varState->isDvmArray, "Only DVM-array can be unlocalized");
            toInsert += indent + "dvmh_indirect_unlocalize_(" + name + ");\n";
        }
        toInsert += "\n";
        if (!curPragma->nameList.empty())
            rewr.InsertText(loc, toInsert, false, false);
    }
}

void ConverterASTVisitor::genArrayCopies(FileID fileID, int line) {
    // Seek for ARRAY_COPY pragmas
    while (DvmPragma *gotPragma = fileCtx.getNextPragma(fileID.getHashValue(), line, DvmPragma::pkArrayCopy)) {
        PragmaArrayCopy *curPragma = (PragmaArrayCopy *)gotPragma;
        checkDirErr(!inRegion && !inParLoop, "array_copy directive is not allowed in region or parallel loop");
        SourceLocation loc(srcMgr.translateLineCol(fileID, curPragma->srcLine + curPragma->srcLineSpan, 1));
        std::string indent = extractIndent(srcMgr.getCharacterData(srcMgr.translateLineCol(fileID, curPragma->srcLine, 1)));
        std::string toInsert;
        if (!opts.lessDvmLines)
            toInsert += indent + genDvmLine(curPragma) + "\n";
        VarDecl *vd = seekVarDecl(curPragma->srcName);
        checkDirErrN(vd, 301, curPragma->srcName.c_str());
        checkIntErrN(varStates.find(vd) != varStates.end(), 92, vd->getNameAsString().c_str());
        VarState *varState = &varStates[vd];
        checkDirErr(varState->isDvmArray, "Only DVM-arrays can be copied for now");
        vd = seekVarDecl(curPragma->dstName);
        checkDirErrN(vd, 301, curPragma->dstName.c_str());
        checkIntErrN(varStates.find(vd) != varStates.end(), 92, vd->getNameAsString().c_str());
        varState = &varStates[vd];
        checkDirErr(varState->isDvmArray, "Only DVM-arrays can be copied for now");
        toInsert += indent + "dvmh_array_copy_whole_(" + curPragma->srcName + ", " + curPragma->dstName + ");\n";
        toInsert += "\n";
        rewr.InsertText(loc, toInsert, false, false);
    }
}

void ConverterASTVisitor::genCheckpoints(FileID fileID, int line) {
    while (DvmPragma *gotPragma = fileCtx.getNextPragma(fileID.getHashValue(), line, DvmPragma::pkCheckpoint)) {
        PragmaCheckpoint *curPragma = (PragmaCheckpoint *)gotPragma;
        SourceLocation loc(srcMgr.translateLineCol(fileID, curPragma->srcLine + curPragma->srcLineSpan, 1));
        std::string indent = extractIndent(srcMgr.getCharacterData(srcMgr.translateLineCol(fileID, curPragma->srcLine, 1)));
        std::string toInsert;
        if (!opts.lessDvmLines)
            toInsert += indent + genDvmLine(curPragma) + "\n";

        auto pragmaType = curPragma->getTypeStr();
        if ((pragmaType == "save") || (pragmaType == "load")) {
            toInsert += indent + "dvmh_" + curPragma->getTypeStr() + "_control_point(\"" + curPragma->cpName + "\");\n";
            toInsert += "\n";
        } else {
            PragmaCheckpointDeclare *curPragmaDecl = (PragmaCheckpointDeclare *)curPragma;
            std::string namePrefix = "_" + curPragmaDecl->cpName;
            std::string dArraysName = namePrefix + "DistributedArrays";
            std::string scalarsName = namePrefix + "ScalarPointers";
            std::string sizesName = namePrefix + "ScalarSizes";
            size_t nDistributedVars = curPragmaDecl->distribIndents.size();
            size_t nScalarVars = curPragmaDecl->scalarIndents.size();

            // DvmType **dvmDesc = (DvmType **) malloc(1 * sizeof(DvmType *));
            toInsert += indent + "DvmType **" + dArraysName + " = (DvmType **) malloc(" + std::to_string(nDistributedVars) + "*sizeof(DvmType *));\n";
            for (int i = 0; i < nDistributedVars; ++i) {
                toInsert += indent + dArraysName + "[" + std::to_string(i) + "] = " +  curPragmaDecl->distribIndents[i] + ";\n";
            }

            // void **scalarPointers = (void **) malloc(2 * sizeof(void *));
            toInsert += indent + "void **" + scalarsName + " = (void **) malloc(" + std::to_string(nScalarVars) +  "*sizeof(void *));\n";
            // size_t *scalarsSizes = (size_t *) malloc(2 * sizeof(size_t *));
            toInsert += indent + "size_t *" + sizesName + " = (size_t *) malloc(" + std::to_string(nScalarVars) + "*sizeof(size_t *));\n";
            for (int i = 0; i < nScalarVars; ++i) {
                toInsert += indent + scalarsName + "[" + std::to_string(i) + "] = (void *) &" + curPragmaDecl->scalarIndents[i] +
                        "+" + std::to_string(curPragmaDecl->scalarOffsets[i]) + "; ";
                toInsert += sizesName + "[" + std::to_string(i) + "] = (size_t) sizeof(" + curPragmaDecl->scalarIndents[i] +
                        ")*" + std::to_string(curPragmaDecl->scalarSizes[i]) + ";\n";
            }

            toInsert += indent + "dvmh_" + curPragmaDecl->getTypeStr() + "_control_point(\"" + curPragma->cpName + "\", ";
            toInsert += std::to_string(curPragmaDecl->nFiles) + ", " + std::to_string(curPragmaDecl->mode2int()) + ", ";
            toInsert = toInsert + dArraysName + ", " + std::to_string(nDistributedVars) + ", ";
            toInsert = toInsert + scalarsName + ", " + sizesName + ", " + std::to_string(nScalarVars);
            toInsert += ");\n\n";
        }
        rewr.InsertText(loc, toInsert, false, false);
    }
}

void ConverterASTVisitor::genDerivedFuncPair(const DerivedAxisRule &rule, std::string &countingFormalParamsFwd, std::string &countingFormalParams,
        std::string &countingFuncBody, std::string &fillingFormalParamsFwd, std::string &fillingFormalParams, std::string &fillingFuncBody,
        int &passParamsCount, std::string &passParams)
{
    std::string indent = indentStep;
    std::string nam = rule.templ;
    VarDecl *vd = seekVarDecl(nam);
    assert(vd); // checked outside
    assert(varStates.find(vd) != varStates.end()); // checked outside
    VarState *varState = &varStates[vd];
    std::set<std::string> prohibitedNames = rule.externalNames;
    std::vector<std::string> freeVars;
    for (int j = 0; j < varState->rank; j++) {
        if (!rule.rhsExprs[j].dummyName.empty()) {
            // aligning (search through this dimension)
            freeVars.push_back(rule.rhsExprs[j].dummyName);
            prohibitedNames.insert(freeVars.back());
        }
    }
    std::string boundsLow = getUniqueName("boundsLow", &prohibitedNames, &fileCtx.seenMacroNames);
    std::string boundsHigh = getUniqueName("boundsHigh", &prohibitedNames, &fileCtx.seenMacroNames);
    std::string pElemCount = getUniqueName("pElemCount", &prohibitedNames, &fileCtx.seenMacroNames);
    std::string elemCount = getUniqueName("elemCount", &prohibitedNames, &fileCtx.seenMacroNames);
    std::string elemBuf = getUniqueName("elemBuf", &prohibitedNames, &fileCtx.seenMacroNames);
    std::string elemIndex = getUniqueName("elemIndex", &prohibitedNames, &fileCtx.seenMacroNames);
    std::string elemValue = getUniqueName("elemValue", &prohibitedNames, &fileCtx.seenMacroNames);
    std::map<std::string, std::string> dvmHeaders;
    std::map<std::string, std::string> scalarPtrs;
    dvmHeaders[varState->name] = getUniqueName(varState->name + "_hdr", &prohibitedNames, &fileCtx.seenMacroNames);
    passParamsCount = 1;
    passParams += ", " + varState->genHeaderOrScalarRef(fileCtx, false);
    for (std::set<std::string>::iterator it = rule.externalNames.begin(); it != rule.externalNames.end(); it++) {
        VarDecl *vd = seekVarDecl(*it);
        if (vd) {
            checkIntErrN(varStates.find(vd) != varStates.end(), 92, vd->getNameAsString().c_str());
            VarState *varState = &varStates[vd];
            if (varState->isArray)
                dvmHeaders[varState->name] = getUniqueName(varState->name + "_hdr", &prohibitedNames, &fileCtx.seenMacroNames);
            else
                scalarPtrs[varState->name] = getUniqueName(varState->name + "_ptr", &prohibitedNames, &fileCtx.seenMacroNames);
            passParamsCount++;
            passParams += ", " + varState->genHeaderOrScalarRef(fileCtx, false);
        }
    }
    countingFormalParamsFwd += ", DvmType *";
    countingFormalParams += ", DvmType *" + pElemCount;
    countingFuncBody += indent + "DvmType " + elemCount + " = *" + pElemCount + ";\n";
    fillingFormalParamsFwd += ", DvmType []";
    fillingFormalParams += ", DvmType " + elemBuf + "[]";
    for (int phase = 0; phase < 2; phase++) {
        std::string &formalParamsFwd = (phase == 0 ? countingFormalParamsFwd : fillingFormalParamsFwd);
        std::string &formalParams = (phase == 0 ? countingFormalParams : fillingFormalParams);
        std::string &funcBody = (phase == 0 ? countingFuncBody : fillingFuncBody);
        formalParamsFwd += ", DvmType [], DvmType []";
        formalParams += ", DvmType " + boundsLow + "[], DvmType " + boundsHigh + "[]";
        formalParamsFwd += ", DvmType []";
        formalParams += ", DvmType " + dvmHeaders[varState->name] + "[]";
        for (std::set<std::string>::iterator it = rule.externalNames.begin(); it != rule.externalNames.end(); it++) {
            VarDecl *vd = seekVarDecl(*it);
            if (vd) {
                checkIntErrN(varStates.find(vd) != varStates.end(), 92, vd->getNameAsString().c_str());
                VarState *varState = &varStates[vd];
                if (varState->isArray) {
                    int rank = varState->rank;
                    std::string refName = varState->name;
                    std::string hdrName = dvmHeaders[refName];
                    formalParamsFwd += ", DvmType []";
                    formalParams += ", DvmType " + hdrName + "[]";
                    funcBody += indent + varState->baseTypeStr + " (*" + (varState->canBeRestrict ? " DVMH_RESTRICT " : "") + refName + ")";
                    std::string castType = varState->baseTypeStr + " (*)";
                    for (int j = 2; j <= rank; j++) {
                        int hdrIdx = j - 1;
                        std::string curSize;
                        if (varState->isDvmArray || !varState->constSize[j - 1]) {
                            if (j < rank)
                                curSize = hdrName + "[" + toStr(hdrIdx) + "]/" + hdrName + "[" + toStr(hdrIdx + 1) + "]";
                            else
                                curSize = hdrName + "[" + toStr(hdrIdx) + "]";
                        } else {
                            curSize = varState->sizes[j - 1].strExpr;
                        }
                        funcBody += "[" + curSize + "]";
                        castType += "[" + curSize + "]";
                    }
                    funcBody += " = (" + castType + ")dvmh_get_natural_base_C(0, " + hdrName + ");\n";
                } else {
                    formalParamsFwd += ", " + varState->baseTypeStr + " *";
                    formalParams += ", " + varState->baseTypeStr + " *" + scalarPtrs[varState->name];
                    // All variables are read-only, so just copy the value
                    funcBody += indent + varState->baseTypeStr + " " + varState->name + " = *" + scalarPtrs[varState->name] + ";\n";
                }
            }
        }
        // Heading is done. Now we can iterate.
        bool usesValue = phase == 1;
        std::string assignStmt;
        if (phase == 0) {
            assignStmt = elemCount + "++;";
        } else {
            funcBody += indent + "DvmType " + elemIndex + " = 0;\n";
            assignStmt = elemBuf + "[" + elemIndex + "++] = " + elemValue + ";";
        }
        for (int i = 0; i < (int)freeVars.size(); i++) {
            std::string dummy = freeVars[i];
            std::string idxExpr = "[" + toStr(i) + "]";
            funcBody += indent + "for (DvmType " + dummy + " = " + boundsLow + idxExpr + "; " + dummy + " <= " + boundsHigh + idxExpr + "; " + dummy +
                    "++) {\n";
            indent += indentStep;
        }
        for (int expI = 0; expI < (int)rule.exprs.size(); expI++) {
            funcBody += indent + "{\n";
            indent += indentStep;
            const MyExpr &expr = rule.exprs[expI];
            // TODO: Walk through ranges
            checkIntErr(expr.ranges.empty(), "Ranges are not implemented yet");
            for (int i = 0; i < (int)freeVars.size(); i++) {
                if (expr.topLevelNames.find(freeVars[i]) != expr.topLevelNames.end())
                    cdvmh_log(INTERNAL, "Direct dummy variable referencing for derived distribution is not implemented yet");
            }
            if (usesValue)
                funcBody += indent + "DvmType " + elemValue + " = (" + expr.strExpr + ");\n";
            funcBody += indent + assignStmt + "\n";
            indent = subtractIndent(indent);
            funcBody += indent + "}\n";
        }
        for (int i = 0; i < (int)freeVars.size(); i++) {
            indent = subtractIndent(indent);
            funcBody += indent + "}\n";
        }
    }
    countingFuncBody += indent + "*" + pElemCount + " = " + elemCount + ";\n";

    trimList(countingFormalParamsFwd);
    trimList(countingFormalParams);
    trimList(fillingFormalParamsFwd);
    trimList(fillingFormalParams);
    trimList(passParams);
}

std::string ConverterASTVisitor::genDerivedAxisParams(DvmPragma *curPragma, const DerivedAxisRule &rule) {
    std::string res;
    std::string nam = rule.templ;
    VarDecl *vd = seekVarDecl(nam);
    checkDirErrN(vd, 301, nam.c_str());
    checkIntErrN(varStates.find(vd) != varStates.end(), 92, vd->getNameAsString().c_str());
    VarState *varState = &varStates[vd];
    checkDirErr(varState->isDvmArray || varState->isTemplate, "Template must be a DVM object");
    checkDirErr(varState->rank == (int)rule.rhsExprs.size(), "Dimension mismatch");
    res += "dvmh_derived_rhs_C(" + nam + ", " + toStr(varState->rank);
    for (int j = 0; j < varState->rank; j++) {
        if (rule.rhsExprs[j].constExpr.empty()) {
            if (rule.rhsExprs[j].dummyName.empty()) {
                // replication (do search if at least one is present)
                res += ", dvmh_derived_rhs_expr_ignore_()";
            } else {
                // aligning (search through this dimension)
                res += ", dvmh_derived_rhs_expr_scan_C(" + toStr(rule.rhsExprs[j].addShadows.size());
                for (int k = 0; k < (int)rule.rhsExprs[j].addShadows.size(); k++)
                    res += ", \"" + escapeStr(rule.rhsExprs[j].addShadows[k]) + "\"";
                res += ")";
            }
        } else {
            // constant (do search if specified index is present)
            res += ", dvmh_derived_rhs_expr_constant_C(" + rule.rhsExprs[j].constExpr.strExpr + ")";
        }
    }
    res += ")";
    std::string shortName = toCIdent(fileCtx.getInputFile().shortName, true);
    if (curPragma->srcFileName != fileCtx.getInputFile().fileName)
        shortName += "_" + toCIdent(getBaseName(curPragma->srcFileName));
    std::string countingFuncName = "indirect_counter_" + shortName + "_" + toStr(curPragma->line);
    std::string fillingFuncName = "indirect_filler_" + shortName + "_" + toStr(curPragma->line);
    countingFuncName = getUniqueName(countingFuncName, &fileCtx.seenGlobalNames, &fileCtx.seenMacroNames);
    fillingFuncName = getUniqueName(fillingFuncName, &fileCtx.seenGlobalNames, &fileCtx.seenMacroNames);
    fileCtx.seenGlobalNames.insert(countingFuncName);
    fileCtx.seenGlobalNames.insert(fillingFuncName);
    int passParamsCount = 0;
    std::string countingFormalParamsFwd, countingFormalParams, countingFuncBody, fillingFormalParamsFwd, fillingFormalParams, fillingFuncBody, passParams;
    genDerivedFuncPair(rule, countingFormalParamsFwd, countingFormalParams, countingFuncBody, fillingFormalParamsFwd, fillingFormalParams, fillingFuncBody,
        passParamsCount, passParams);
    res += ", dvmh_handler_func_C((DvmHandlerFunc)" + countingFuncName + ", " + toStr(passParamsCount);
    if (passParamsCount > 0)
        res += ", " + passParams;
    res += "), dvmh_handler_func_C((DvmHandlerFunc)" + fillingFuncName + ", " + toStr(passParamsCount);
    if (passParamsCount > 0)
        res += ", " + passParams;
    res += ")";
    fileCtx.addHandlerForwardDecl("void " + countingFuncName + "(" + countingFormalParamsFwd + ");\n");
    fileCtx.addHandlerForwardDecl("void " + fillingFuncName + "(" + fillingFormalParamsFwd + ");\n");
    fileCtx.addToMainTail("void " + countingFuncName + "(" + countingFormalParams + ") {\n" + countingFuncBody + "}\n\n");
    fileCtx.addToMainTail("void " + fillingFuncName + "(" + fillingFormalParams + ") {\n" + fillingFuncBody + "}\n\n");
    return res;
}

std::string ConverterASTVisitor::genDistribParams(DvmPragma *curPragma, DistribRule *rule) {
    std::string res;
    int rank = rule->rank;
    res += toStr(rank);
    int mpsAxis = 1;
    for (int i = 0; i < rank; i++) {
        res += ", ";
        DistribAxisRule axisRule = rule->axes[i];
        if (axisRule.distrType == DistribAxisRule::dtReplicated)
            res += "dvmh_distribution_replicated_()";
        else if (axisRule.distrType == DistribAxisRule::dtBlock)
            res += "dvmh_distribution_block_C(" + toStr(mpsAxis) + ")";
        else if (axisRule.distrType == DistribAxisRule::dtWgtBlock) {
            std::string nam = axisRule.wgtBlockArray.first;
            VarDecl *vd = seekVarDecl(nam);
            checkDirErrN(vd, 301, nam.c_str());
            checkIntErrN(varStates.find(vd) != varStates.end(), 92, vd->getNameAsString().c_str());
            VarState *varState = &varStates[vd];
            std::string wgtArrayType;
            if (varState->isDvmArray)
                wgtArrayType = "rt_UNKNOWN";
            else
                wgtArrayType = toRedType(varState->baseTypeStr);
            checkNonDvmExpr(axisRule.wgtBlockArray.second, curPragma);
            res += "dvmh_distribution_wgtblock_C(" + toStr(mpsAxis) + ", " + wgtArrayType + ", " + axisRule.wgtBlockArray.first + ", " + axisRule.wgtBlockArray.second.strExpr + ")";
        } else if (axisRule.distrType == DistribAxisRule::dtGenBlock) {
            std::string nam = axisRule.genBlockArray;
            VarDecl *vd = seekVarDecl(nam);
            checkDirErrN(vd, 301, nam.c_str());
            checkIntErrN(varStates.find(vd) != varStates.end(), 92, vd->getNameAsString().c_str());
            std::string gblArrayType = toRedType(varStates[vd].baseTypeStr);
            res += "dvmh_distribution_genblock_C(" + toStr(mpsAxis) + ", " + gblArrayType + ", " + axisRule.genBlockArray + ")";
        } else if (axisRule.distrType == DistribAxisRule::dtMultBlock) {
            checkNonDvmExpr(axisRule.multBlockValue, curPragma);
            res += "dvmh_distribution_multblock_C(" + toStr(mpsAxis) + ", " + axisRule.multBlockValue.strExpr + ")";
        } else if (axisRule.distrType == DistribAxisRule::dtIndirect) {
            checkDirErrN(opts.enableIndirect, 323, "Indirect distribution method");
            std::string nam = axisRule.indirectArray;
            VarDecl *vd = seekVarDecl(nam);
            checkDirErrN(vd, 301, nam.c_str());
            checkIntErrN(varStates.find(vd) != varStates.end(), 92, vd->getNameAsString().c_str());
            VarState *varState = &varStates[vd];
            std::string indirArrayType;
            if (varState->isDvmArray)
                indirArrayType = "rt_UNKNOWN";
            else
                indirArrayType = toRedType(varState->baseTypeStr);
            res += "dvmh_distribution_indirect_C(" + toStr(mpsAxis) + ", " + indirArrayType + ", " + axisRule.indirectArray + ")";
        } else if (axisRule.distrType == DistribAxisRule::dtDerived) {
            checkDirErrN(opts.enableIndirect, 323, "Derived distribution method");
            res += "dvmh_distribution_derived_C(" + toStr(mpsAxis) + ", " + genDerivedAxisParams(curPragma, axisRule.derivedRule) + ")";
        } else {
            assert(false);
        }
        if (axisRule.distrType != DistribAxisRule::dtReplicated) {
            mpsAxis++;
        }
    }
    return res;
}

std::string ConverterASTVisitor::genDistribCall(std::string varName, DvmPragma *curPragma, DistribRule *rule) {
    return "dvmh_distribute_C(" + varName + ", " + genDistribParams(curPragma, rule) + ");";
}

std::string ConverterASTVisitor::genAlignParams(DvmPragma *curPragma, std::string templ, int templRank, const std::vector<AlignAxisRule> &axisRules) {
    std::string res;
    res += templ;
    res += ", " + toStr(templRank);
    for (int i = 0; i < templRank; i++) {
        const AlignAxisRule *arule = &axisRules[i];
        res += ", dvmh_alignment_linear_C(" + toStr(arule->axisNumber);
        if (arule->axisNumber < 0) {
            res += ", 0, 0";
        } else if (arule->axisNumber == 0) {
            // To constant
            checkNonDvmExpr(arule->summand, curPragma);
            res += ", 0, " + arule->summand.strExpr;
        } else if (arule->axisNumber > 0) {
            // Linear rule
            checkNonDvmExpr(arule->multiplier, curPragma);
            checkNonDvmExpr(arule->summand, curPragma);
            res += ", " + arule->multiplier.strExpr + ", " + arule->summand.strExpr;
        } else {
            assert(false);
        }
        res += ")";
    }
    return res;
}

std::string ConverterASTVisitor::genAlignParams(DvmPragma *curPragma, AlignRule *rule) {
    return genAlignParams(curPragma, rule->templ, rule->templRank, rule->axisRules);
}

std::string ConverterASTVisitor::genAlignCall(std::string varName, DvmPragma *curPragma, AlignRule *rule) {
    return "dvmh_align_C(" + varName + ", " + genAlignParams(curPragma, rule) + ");";
}

void ConverterASTVisitor::handleDeclGroup(Decl *head) {
    SourceLocation fileLoc = srcMgr.getFileLoc(head->getEndLoc());
    std::string fileName = srcMgr.getFilename(fileLoc).str();
    FileID fileID = srcMgr.getFileID(fileLoc);
    int line = srcMgr.getLineNumber(fileID, srcMgr.getFileOffset(fileLoc));
    if (DvmPragma *curPragmaTmp = fileCtx.getNextPragma(fileID.getHashValue(), line)) {
        DvmPragma *curPragma = curPragmaTmp;
        checkDirErrN(curPragmaTmp->kind == DvmPragma::pkDistribArray || curPragmaTmp->kind == DvmPragma::pkTemplate, 303);
        bool globalFlag = false, hasExternal = false, hasStatic = false, isLocalStatic = false, firstDecl;
        std::string substText, curInitCode, curFinishCode, indent;
        firstDecl = true;
        const std::vector<Decl *> &vec = declGroups[head];
        cdvmh_log(TRACE, "Examining group of size %d with head %p", (int)vec.size(), (void *)head);
        for (int i = 0; i < (int)vec.size(); i++) {
            VarDecl *vd = llvm::dyn_cast<VarDecl>(vec[i]);
            checkDirErrN(vd, 411);
            checkDirErrN(!vd->hasInit(), 412);
            if (firstDecl) {
                globalFlag = isGlobalC(vd);
                hasExternal = vd->hasExternalStorage();
                hasStatic = vd->hasGlobalStorage() && (!globalFlag || !vd->hasExternalFormalLinkage());
                isLocalStatic = hasStatic && !globalFlag;
                checkDirErrN(hasExternal || fileCtx.isCompilable(), 413);
                substText += std::string(hasExternal ? "extern " : "") + (hasStatic ? "static " : "");
                if (globalFlag)
                    indent = indentStep;
                else
                    indent = extractIndent(srcMgr.getCharacterData(srcMgr.translateLineCol(fileID, line, 1)));
            }
            VarState varState = fillVarState(vd);
            std::string varName = varState.name;
            int rank = varState.rank;
            bool incompleteFlag = varState.isIncomplete;
            std::string typeName = varState.baseTypeStr;
            std::vector<MyExpr> sizes = varState.sizes;
            cdvmh_log(TRACE, "Handling %s", varName.c_str());
            if (curPragmaTmp->kind == DvmPragma::pkDistribArray) {
                PragmaDistribArray *curPragma = (PragmaDistribArray *)curPragmaTmp;
                checkDirErrN(isDistrDeclAllowed(), 414);
                cdvmhLog(TRACE, fileName, line, "Distributed array '%s'", varName.c_str());
                // array with or without DISTRIBUTE or ALIGN
                varState.doDvmArray(curPragma);
                // Must be an array
                checkDirErrN(rank > 0, 415);
                if (hasExternal)
                    checkDirErrN(curPragma->rank == -1, 304, varName.c_str());
                else
                    checkDirErrN(curPragma->rank == -1 || curPragma->rank == rank, 304, varName.c_str());
                MyExpr defaultShadowWidth;
                defaultShadowWidth.strExpr = opts.enableIndirect ? "0" : "1";
                while ((int)curPragma->shadows.size() < rank)
                    curPragma->shadows.push_back(std::make_pair(defaultShadowWidth, defaultShadowWidth));
                if (firstDecl)
                    substText += "DvmType ";
                else
                    substText += ", ";
                substText += varName + "[" + toStr(varState.headerArraySize) + "]";
                if (opts.extraComments)
                    substText += " /*array, rank=" + toStr(rank) + ", baseType=" + typeName + "*/";
                if (!hasExternal && isLocalStatic)
                    substText += " = {0}";
                if (!hasExternal) {
                    if (firstDecl && !opts.lessDvmLines)
                        curInitCode += indent + genDvmLine(curPragma) + "\n";
                    if (isLocalStatic) {
                        curInitCode += indent + "if (" + varName + "[0] == 0) {\n";
                        indent += indentStep;
                    }
                    if (!incompleteFlag) {
                        curInitCode += indent + "dvmh_array_create_C(" + varName + ", " + toStr(rank) + ", ";
                        if (isRedType(typeName))
                            curInitCode += "-" + toRedType(typeName);
                        else
                            curInitCode += "sizeof(" + typeName + ")";
                        for (int i = 0; i < rank; i++) {
                            checkNonDvmExpr(sizes[i], curPragma);
                            checkNonDvmExpr(curPragma->shadows[i].first, curPragma);
                            checkNonDvmExpr(curPragma->shadows[i].second, curPragma);
                            curInitCode += ", " + fileCtx.dvm0c(sizes[i].strExpr) + ", " + fileCtx.dvm0c(curPragma->shadows[i].first.strExpr) + ", " +
                                    fileCtx.dvm0c(curPragma->shadows[i].second.strExpr);
                        }
                        curInitCode += ");\n";
                        if (curPragma->alignFlag == 0)
                            curInitCode += indent + genDistribCall(varName, curPragma, &curPragma->distribRule) + "\n";
                        else if (curPragma->alignFlag == 1)
                            curInitCode += indent + genAlignCall(varName, curPragma, &curPragma->alignRule) + "\n";
                        // Else deferred distribution
                    } else {
                        checkDirErrN(curPragma->alignFlag == -1 || (curPragma->alignFlag == 0 && curPragma->distribRule.isConstant()), 311);
                        curInitCode += indent + "dvmh_array_declare_C(" + varName + ", " + toStr(rank) + ", ";
                        if (isRedType(typeName))
                            curInitCode += "-" + toRedType(typeName);
                        else
                            curInitCode += "sizeof(" + typeName + ")";
                        for (int i = 0; i < rank; i++) {
                            checkNonDvmExpr(sizes[i], curPragma);
                            checkNonDvmExpr(curPragma->shadows[i].first, curPragma);
                            checkNonDvmExpr(curPragma->shadows[i].second, curPragma);
                            curInitCode += ", " + (i > 0 ? fileCtx.dvm0c(sizes[i].strExpr) : fileCtx.dvm0c(0)) + ", " +
                                    fileCtx.dvm0c(curPragma->shadows[i].first.strExpr) + ", " + fileCtx.dvm0c(curPragma->shadows[i].second.strExpr);
                        }
                        curInitCode += ");\n";
                    }
                    if (isLocalStatic) {
                        indent = subtractIndent(indent);
                        curInitCode += indent + "}\n";
                    }
                    if (!globalFlag && !isLocalStatic) {
                        if (!incompleteFlag)
                            addToDelete(varName);
                        else
                            addToDelete(varName, "dvmh_forget_header_");
                    }
                    if (globalFlag) {
                        if (!incompleteFlag)
                            curFinishCode += indent + "dvmh_delete_object_(" + varName + ");\n";
                        else
                            curFinishCode += indent + "dvmh_forget_header_(" + varName + ");\n";
                    }
                    // TODO: Decide on finalization of static variables
                }
            } else if (curPragmaTmp->kind == DvmPragma::pkTemplate) {
                PragmaTemplate *curPragma = (PragmaTemplate *)curPragmaTmp;
                checkDirErrN(isDistrDeclAllowed(), 416);
                cdvmhLog(TRACE, fileName, line, "Template '%s'", varName.c_str());
                // TEMPLATE with or without DISTRIBUTE or ALIGN
                varState.doTemplate(curPragma);
                rank = curPragma->rank;
                sizes = curPragma->sizes;
                if (firstDecl)
                    substText += "DvmType ";
                else
                    substText += ", ";
                substText += varName + "[" + toStr(varState.headerArraySize) + "]";
                if (opts.extraComments)
                    substText += " /*template, rank=" + toStr(rank) + "*/";
                if (!hasExternal && isLocalStatic)
                    substText += " = {0}";
                if (!hasExternal) {
                    if (firstDecl && !opts.lessDvmLines)
                        curInitCode += indent + genDvmLine(curPragma) + "\n";
                    if (isLocalStatic) {
                        curInitCode += indent + "if (" + varName + "[0] == 0) {\n";
                        indent += indentStep;
                    }
                    curInitCode += indent + "dvmh_template_create_C(" + varName + ", " + toStr(rank);
                    for (int i = 0; i < rank; i++) {
                        checkNonDvmExpr(sizes[i], curPragma);
                        curInitCode += ", " + fileCtx.dvm0c(sizes[i].strExpr);
                    }
                    curInitCode += ");\n";
                    if (curPragma->alignFlag == 0)
                        curInitCode += indent + genDistribCall(varName, curPragma, &curPragma->distribRule) + "\n";
                    else if (curPragma->alignFlag == 1)
                        curInitCode += indent + genAlignCall(varName, curPragma, &curPragma->alignRule) + "\n";
                    // Else deferred distribution
                    if (isLocalStatic) {
                        indent = subtractIndent(indent);
                        curInitCode += indent + "}\n";
                    }
                    if (!globalFlag && !isLocalStatic)
                        addToDelete(varName);
                    if (globalFlag)
                        curFinishCode += indent + "dvmh_delete_object_(" + varName + ");\n";
                    // TODO: Decide on finalization of static variables
                }
            } else
                assert(false); // Unreachable
            varStates[vd] = varState;
            firstDecl = false;
        }
        if (!vec.empty()) {
            if (!curInitCode.empty()) {
                //checkDirErrN(srcMgr.isWrittenInMainFile(fileLoc), 417);
                if (globalFlag) {
                    assert(fileCtx.isCompilable()); // Is consequent of upper part
                    curInitCode += "\n";
                    fileCtx.addInitGlobText(curInitCode);
                    fileCtx.addFinishGlobText(curFinishCode);
                } else {
                    removeLastSemicolon(curInitCode);
                    substText += ";\n" + curInitCode;
                }
            }
            if (!projectCtx.hasInputFile(fileName))
                cdvmhLog(DEBUG, curPragmaTmp->fileName, curPragmaTmp->line, 418, MSG(418));
            rewr.ReplaceText(escapeMacro(SourceRange(head->getBeginLoc(), vec.back()->getEndLoc())), substText);
        }
    }
    declGroups.erase(head);
}

bool ConverterASTVisitor::VisitVarDecl(VarDecl *vd) {
    SourceLocation fileLoc = srcMgr.getFileLoc(vd->getLocation());
    PresumedLoc ploc = srcMgr.getPresumedLoc(fileLoc);
    handleDeclGroup(vd);
    VarState varState;
    if (varStates.find(vd) == varStates.end())
        varState = fillVarState(vd);
    else {
        varState = varStates[vd];
        cdvmh_log(TRACE, "Reusing varState for %s", varState.name.c_str());
    }
    bool globalFlag = isGlobalC(vd);
    bool hasExternal = vd->hasExternalStorage();
    bool hasStatic = vd->hasGlobalStorage() && (!globalFlag || !vd->hasExternalFormalLinkage());
    std::string varName = varState.name;
    if (projectCtx.hasInputFile(ploc.getFilename()))
        cdvmhLog(TRACE, ploc.getFilename(), ploc.getLine(), "VarDecl: varName=%s", varName.c_str());
    bool inUserFile = srcMgr.isInMainFile(fileLoc) || fileCtx.isUserInclude(srcMgr.getFileID(fileLoc).getHashValue());
    if (!opts.useDvmhStdio && inUserFile) {
        QualType t = vd->getType();
        while (t.getTypePtr()->isPointerType())
            t = t.getTypePtr()->getPointeeType().getUnqualifiedType();
        std::string strT = t.getAsString();
        if (!opts.seqOutput && projectCtx.hasDvmhReplacement(strT)) {
            std::string repl = projectCtx.getDvmhReplacement(strT);
            cdvmhLog(TRACE, ploc.getFilename(), ploc.getLine(), "Replaced %s to %s for %s", strT.c_str(), repl.c_str(), varName.c_str());
            SourceRange sr(escapeMacroBegin(vd->getTypeSpecStartLoc()), Lexer::getLocForEndOfToken(escapeMacroEnd(vd->getTypeSpecStartLoc()), 0, srcMgr,
                    langOpts));
            rewr.ReplaceText(sr, repl);
        }
    }

    if (isa<ParmVarDecl>(vd)) {
        if (curInherits.find(vd) != curInherits.end()) {
            if (varState.isTemplateLike())
                varState.doTemplate(0);
            else
                varState.doDvmArray(0);
            rewr.ReplaceText(escapeMacro(vd->getSourceRange()), "DvmType " + varName + "[" + toStr(varState.headerArraySize) + "]");
        }
    }

    if (varState.isRegular && (!varState.isArray || !varState.isIncomplete)) {
        if (!opts.seqOutput && fileCtx.isCompilable() && globalFlag && !hasExternal) {
            // XXX: Here external variables are not handled because linkage errors can be induced.
            std::string enterCode = indentStep + "dvmh_data_enter_C((const void *)" + (varState.isArray ? "" : "&") + varName + ", sizeof(" + varName + "));\n";
            std::string exitCode = indentStep + "dvmh_data_exit_C((const void *)" + (varState.isArray ? "" : "&") + varName + ", 0);\n";
            fileCtx.addInitGlobText(enterCode, true);
            fileCtx.addFinishGlobText(exitCode, true);
        }
        if (!opts.seqOutput && !globalFlag && hasStatic) {
            // TODO: Handle static local variables somehow: they should be dvmh_data_enter_'ed once in a lifetime
        }
    }
    if (inParLoopBody)
        innerVars.insert(vd);
    if (inRegion && inParLoop) {
        if ((possibleTargets & PragmaRegion::DEVICE_TYPE_CUDA) && !projectCtx.hasCudaReplacement(varState.baseTypeStr)) {
            cdvmhLog(WARNING, ploc.getFilename(), ploc.getLine(), 442, MSG(442));
            possibleTargets &= ~(PragmaRegion::DEVICE_TYPE_CUDA);
        }
    }
    varStates[vd] = varState;
    cdvmhLog(DONT_LOG, ploc.getFilename(), ploc.getLine(), "Adding variable '%s' declaration to MyDeclContext", varName.c_str());
    declContexts.back()->add(vd);
    return true;
}

bool ConverterASTVisitor::VisitFunctionDecl(FunctionDecl *f) {
    SourceLocation fileLoc = srcMgr.getFileLoc(f->getBeginLoc());
    std::string fileName = srcMgr.getFilename(fileLoc).str();
    FileID fileID = srcMgr.getFileID(fileLoc);
    int line = srcMgr.getLineNumber(fileID, srcMgr.getFileOffset(fileLoc));
    const FunctionDecl *Definition;
    bool hasBody = f->hasBody(Definition);
    bool bodyIsHere = hasBody && Definition == f;
    bool isMain = f->isMain();
    bool isEntry = (opts.dvmhLibraryEntry.empty() && isMain) || (!opts.dvmhLibraryEntry.empty() && f->getName() == opts.dvmhLibraryEntry);
    checkUserErrN(!inRegion, fileName, line, 431);
    if (srcMgr.isInMainFile(fileLoc) || projectCtx.hasInputFile(fileName)) {
        //checkUserErr(!bodyIsHere || fileCtx.isCompilable(), fileName, line, "Function definition in header file is not allowed");
        cdvmhLog(TRACE, fileName, line, "Function declaration '%s'", f->getDeclName().getAsString().c_str());
    }
    if (hasBody)
        funcLevels.push_back(toDelete.size());
    // Process function header
    std::set<std::string> inherits;
    if (DvmPragma *gotPragma = fileCtx.getNextPragma(fileID.getHashValue(), line)) {
        checkUserErrN(gotPragma->kind == DvmPragma::pkInherit, gotPragma->fileName, gotPragma->line, 303);
        PragmaInherit *curPragma = (PragmaInherit *)gotPragma;
        for (int i = 0; i < (int)curPragma->names.size(); i++)
            inherits.insert(curPragma->names[i]);
    }
    curInherits.clear();
    checkUserErrN(!isMain || inherits.empty(), fileName, line, 361);
    int nparams = f->getNumParams();
    // Check and fill curInherits
    for (int i = 0; i < nparams; i++) {
        std::string parmName = f->getParamDecl(i)->getName().str();
        if (inherits.find(parmName) != inherits.end()) {
            inherits.erase(parmName);
            curInherits.insert(f->getParamDecl(i));
        }
    }
    checkUserErrN(inherits.empty(), fileName, line, 362);
    if (bodyIsHere && isEntry) {
        std::string toInsert;
        std::string indent = indentStep;
        std::string argcName, argvName;
        if (isMain) {
            if (nparams >= 1) {
                checkUserErrN(f->getParamDecl(0)->getType().getUnqualifiedType().getDesugaredType(comp.getASTContext()).getAsString() == "int", fileName, line,
                        57);
                argcName = f->getParamDecl(0)->getName().str();
            }
            if (nparams >= 2) {
                const Type *t = f->getParamDecl(1)->getType().getTypePtr();
                checkUserErrN(t->isPointerType() || t->isArrayType(), fileName, line, 58);
                if (t->isPointerType())
                    t = t->getPointeeType().getUnqualifiedType().getTypePtr();
                else
                    t = t->getArrayElementTypeNoTypeQual();
                checkUserErrN(t->isPointerType(), fileName, line, 58);
                checkUserErrN(t->getPointeeType().getUnqualifiedType().getDesugaredType(comp.getASTContext()).getAsString() == "char", fileName, line, 58);
                argvName = f->getParamDecl(1)->getName().str();
            }
            if (nparams == 0) {
                argcName = "dvmh_argc";
                argvName = "dvmh_argv";
                rewr.ReplaceText(escapeMacro(SourceRange(f->getBeginLoc(), f->getBody()->getBeginLoc().getLocWithOffset(-1))),
                        "int main(int dvmh_argc, char **dvmh_argv) ");
            } else {
                checkUserErrN(nparams >= 2, fileName, line, 59);
            }
            checkUserErrN(!argcName.empty() && !argvName.empty(), fileName, line, 54);
        }

        toInsert += "\n";
        if (!opts.lessDvmLines)
            toInsert += indent + genDvmLine(fileLoc) + "\n";
        const char *initFlags[] = {"INITFLAG_FORTRAN", "INITFLAG_NOH", "INITFLAG_SEQUENTIAL", "INITFLAG_OPENMP", "INITFLAG_DEBUG"};
        int intInitFlags = 0 + (opts.noH ? 2 : 0) + (opts.seqOutput ? 4 : 0) + ((opts.dvmDebugLvl > 0) ? 16 : 0);
        std::string flagsString;
        if (opts.doOpenMP) {
            toInsert += "#ifdef _OPENMP\n";
            flagsString = flagsToStr(intInitFlags + 8, initFlags, 5);
            if (isMain)
                toInsert += indent + "dvmh_init_C(" + flagsString + ", &" + argcName + ", &" + argvName + ");\n";
            else
                toInsert += indent + "dvmh_init_lib_C(" + flagsString + ");\n";
            toInsert += "#else\n";
        }
        flagsString = flagsToStr(intInitFlags, initFlags, 5);
        if (isMain)
            toInsert += indent + "dvmh_init_C(" + flagsString + ", &" + argcName + ", &" + argvName + ");\n";
        else
            toInsert += indent + "dvmh_init_lib_C(" + flagsString + ");\n";
        if (opts.doOpenMP)
            toInsert += "#endif\n";
        //checkUserErrN(srcMgr.isWrittenInMainFile(fileLoc), fileName, line, 55);
        CompoundStmt *funcBody = llvm::dyn_cast<CompoundStmt>(f->getBody());
        checkIntErrN(funcBody != 0, 96, f->getDeclName().getAsString().c_str());
        SourceLocation loc = escapeMacroEnd(funcBody->getLBracLoc()).getLocWithOffset(1);
        rewr.InsertText(loc, toInsert, true, false);
    }
    return true;
}

bool ConverterASTVisitor::TraverseFunctionDecl(FunctionDecl *f) {
    bool res = base::TraverseFunctionDecl(f);
    SourceLocation fileLoc = srcMgr.getFileLoc(f->getLocation());
    std::string fileName = srcMgr.getFilename(fileLoc).str();
    FileID fileID = srcMgr.getFileID(fileLoc);
    int line = srcMgr.getLineNumber(fileID, srcMgr.getFileOffset(fileLoc));
    const FunctionDecl *Definition;
    bool hasBody = f->hasBody(Definition);
    bool bodyIsHere = hasBody && Definition == f;
    if (projectCtx.hasInputFile(fileName))
        cdvmhLog(TRACE, fileName, line, "Out of funcdecl %s\n", f->getDeclName().getAsString().c_str());
    if (bodyIsHere && f->isMain()) {
        SourceLocation loc = f->getBodyRBrace();
        std::string toInsert;
        toInsert += indentStep + "dvmh_exit_C(0);\n";
        rewr.InsertText(escapeMacroBegin(loc), toInsert, true, false);
    }
    curInherits.clear();
    return res;
}

bool ConverterASTVisitor::TraverseFunctionTemplateDecl(FunctionTemplateDecl *f) {
    SourceLocation fileLoc = srcMgr.getFileLoc(f->getLocation());
    std::string fileName = srcMgr.getFilename(fileLoc).str();
    FileID fileID = srcMgr.getFileID(fileLoc);
    int line = srcMgr.getLineNumber(fileID, srcMgr.getFileOffset(fileLoc));
    assert(curInstantiations.empty());
    while (DvmPragma *gotPragma = fileCtx.getNextPragma(fileID.getHashValue(), line, DvmPragma::pkInstantiations)) {
        PragmaInstantiations *curPragma = (PragmaInstantiations *)gotPragma;
        curInstantiations.insert(curPragma->valueSets.begin(), curPragma->valueSets.end());
    }
    bool res = base::TraverseFunctionTemplateDecl(f);
    curInstantiations.clear();
    return res;
}

bool ConverterASTVisitor::VisitStmt(Stmt *s) {
    SourceLocation fileLoc = srcMgr.getFileLoc(s->getBeginLoc());
    std::string fileName = srcMgr.getFilename(fileLoc).str();
    FileID fileID = srcMgr.getFileID(fileLoc);
    int line = srcMgr.getLineNumber(fileID, srcMgr.getFileOffset(fileLoc));
    genUnbinded(fileID, line);

    if (isa<SwitchStmt>(s))
        switchLevels.push_back(toDelete.size());
    if (isa<ForStmt>(s) || isa<WhileStmt>(s) || isa<DoStmt>(s))
        loopLevels.push_back(toDelete.size());
    if (isa<ForStmt>(s) || isa<WhileStmt>(s)) // They could be without compound statement as body
        toDelete.resize(toDelete.size() + 1);

    if (isa<IfStmt>(s) || isa<ForStmt>(s) || isa<WhileStmt>(s) || isa<CXXCatchStmt>(s))
        enterDeclContext(true);

    if (DvmPragma *gotPragma = fileCtx.getNextPragma(fileID.getHashValue(), line, DvmPragma::pkRegion, DvmPragma::pkParallel, DvmPragma::pkRemoteAccess,
            DvmPragma::pkHostSection)) {
        if (gotPragma->kind == DvmPragma::pkRegion) {
            PragmaRegion *curPragma = (PragmaRegion *)gotPragma;
            checkDirErrN(fileCtx.isCompilable(), 432);
            //checkDirErrN(srcMgr.isWrittenInMainFile(fileLoc), 433);
            checkDirErrN(rmaStack.empty(), 434);
            checkDirErrN(!inRegion, 435);
            checkDirErrN(!inParLoop, 436);
            checkDirErrN(isa<CompoundStmt>(s), 437);
            cdvmhLog(TRACE, curPragma->fileName, curPragma->line, "Entering region");
            fileCtx.setHasRegions();
            inRegion = true;
            regionStmt = s;
            curRegionPragma = curPragma;
            std::string indent = extractIndent(srcMgr.getCharacterData(srcMgr.translateLineCol(fileID, curPragma->srcLine, 1)));
            indent += indentStep;
            regionInnerIndent = indent;
            needToRegister.clear();
            possibleTargets = PragmaRegion::DEVICE_TYPE_HOST | (opts.noCuda ? 0 : PragmaRegion::DEVICE_TYPE_CUDA);
            if (curPragma->targets != 0)
                possibleTargets &= curPragma->targets | PragmaRegion::DEVICE_TYPE_HOST;
            assert(possibleTargets & PragmaRegion::DEVICE_TYPE_HOST); // is consequent of upper part
            if ((curPragma->targets & possibleTargets) != curPragma->targets) {
                curPragma->targets &= possibleTargets;
                if (!curPragma->targets)
                    curPragma->targets = PragmaRegion::DEVICE_TYPE_HOST;
                cdvmhLog(WARNING, curPragma->fileName, curPragma->line, 438, MSG(438), curPragma->getTargetsStr().c_str());
            }
            std::set<std::string> prohibitedNames;
            CollectNamesVisitor collectNamesVisitor(comp);
            collectNamesVisitor.TraverseStmt(s);
            prohibitedNames = collectNamesVisitor.getNames();
            curRegion = getUniqueName("cur_region", &prohibitedNames, &fileCtx.seenMacroNames);
            curLoop = getUniqueName("cur_loop", &prohibitedNames, &fileCtx.seenMacroNames);
        } else if (gotPragma->kind == DvmPragma::pkParallel) {
            PragmaParallel *curPragma = (PragmaParallel *)gotPragma;
            checkDirErrN(fileCtx.isCompilable(), 443);
            //checkDirErrN(srcMgr.isWrittenInMainFile(fileLoc), 444);
            checkDirErrN(rmaStack.empty(), 445);
            checkDirErrN(!inHostSection, 446);
            checkDirErrN(!inParLoop, 447);
            checkDirErrN(isa<ForStmt>(s), 448);

            cdvmhLog(TRACE, curPragma->fileName, curPragma->line, "Entering parallel loop");
            std::pair<unsigned, int> loopPosition = std::make_pair(fileID.getHashValue(), line);
            parallelLoops.insert(std::make_pair(loopPosition, gotPragma));

            if (!inRegion)
                possibleTargets = PragmaRegion::DEVICE_TYPE_HOST;
            fileCtx.setHasLoops();
            inParLoop = true;
            inParLoopBody = false;
            parLoopStmt = s;
            curParallelPragma = curPragma;
            needsParams.clear();
            outerPrivates.clear();
            std::set<std::string> loopVarNames;
            Stmt *curSt = s;
            for (int i = 0; i < curPragma->rank; i++) {
                if (i > 0 && isa<CompoundStmt>(curSt) && cast<CompoundStmt>(curSt)->size() == 1)
                    curSt = *cast<CompoundStmt>(curSt)->body_begin();
                checkDirErrN(isa<ForStmt>(curSt), 341, curPragma->rank);
                ForStmt *curFor = cast<ForStmt>(curSt);
                VarDecl *vd = 0;
                checkDirErrN(curFor->getInit() != 0, 449);
                if (isa<DeclStmt>(curFor->getInit())) {
                    cdvmhLog(TRACE, curPragma->fileName, curPragma->line, "For-loop with declaration");
                    DeclStmt *ds = cast<DeclStmt>(curFor->getInit());
                    assert(ds); // is consequent of upper part
                    checkDirErrN(ds->getDeclGroup().isSingleDecl(), 449);
                    vd = llvm::dyn_cast<VarDecl>(ds->getSingleDecl());
                    checkDirErrN(vd, 4410);
                    checkDirErrN(vd->getType().split().Ty->hasIntegerRepresentation(), 4410);
                } else {
                    BinaryOperator *initExpr = llvm::dyn_cast<BinaryOperator>(curFor->getInit());
                    checkDirErrN(initExpr && initExpr->isAssignmentOp(), 449);
                    checkDirErrN(initExpr->getLHS()->isLValue() && isa<DeclRefExpr>(initExpr->getLHS()), 4411);
                    vd = llvm::dyn_cast<VarDecl>(cast<DeclRefExpr>(initExpr->getLHS())->getDecl());
                    checkDirErrN(vd, 4411);
                    checkDirErrN(vd->getType().split().Ty->hasIntegerRepresentation(), 4411);
                }
                assert(vd); // is consequent of upper part
                outerPrivates.insert(vd);
                bool uniqueFlag = loopVarNames.insert(vd->getName().str()).second;
                checkDirErrN(uniqueFlag, 4412);
                if (curPragma->mapRule.isInitialized()) {
                    std::map<std::string, int>::iterator it = curPragma->mapRule.nameToAxis.find(vd->getName().str());
                    checkDirErrN(it != curPragma->mapRule.nameToAxis.end() && it->second == i + 1, 342);
                }
                curSt = curFor->getBody();
            }
            for (int i = 0; i < (int)curPragma->privates.size(); i++) {
                VarDecl *vd = seekVarDecl(curPragma->privates[i]);
                checkDirErrN(vd, 301, curPragma->privates[i].c_str());
                checkIntErrN(varStates.find(vd) != varStates.end(), 92, vd->getNameAsString().c_str());
                VarState *varState = &varStates[vd];
                checkDirErrN(varState->isConstSize(), 4413, varState->name.c_str());
                if (loopVarNames.find(varState->name) == loopVarNames.end())
                    outerPrivates.insert(vd);
                else
                    cdvmhLog(WARNING, curPragma->fileName, curPragma->line, 343, MSG(343), varState->name.c_str());
            }
            parLoopBodyStmt = curSt;
            innerVars.clear();
            reductions.clear();
            rmaAppearances.clear();
            parLoopBodyExprCounter = 0;
            delete parallelRmaDesc;
            parallelRmaDesc = new ParallelRmaDesc;
            for (int i = 0; i < (int)curPragma->reductions.size(); i++) {
                VarDecl *vd = seekVarDecl(curPragma->reductions[i].arrayName);
                checkDirErrN(vd, 301, curPragma->reductions[i].arrayName.c_str());
                checkIntErrN(varStates.find(vd) != varStates.end(), 92, vd->getNameAsString().c_str());
                VarState *varState = &varStates[vd];
                checkDirErrN(!varState->isIncomplete && varState->isConstSize(), 4414, varState->name.c_str());
                checkDirErrN(loopVarNames.find(varState->name) == loopVarNames.end(), 344, varState->name.c_str());
                reductions.insert(vd);
                if ((possibleTargets & PragmaRegion::DEVICE_TYPE_CUDA) && varState->isArray) {
                    cdvmhLog(WARNING, curPragma->fileName, curPragma->line, 4415, MSG(4415), varState->name.c_str());
                    possibleTargets &= ~(PragmaRegion::DEVICE_TYPE_CUDA);
                }
                if (curPragma->reductions[i].isLoc()) {
                    VarDecl *vd = seekVarDecl(curPragma->reductions[i].locName);
                    checkDirErrN(vd, 301, curPragma->reductions[i].locName.c_str());
                    checkIntErrN(varStates.find(vd) != varStates.end(), 92, vd->getNameAsString().c_str());
                    VarState *varState = &varStates[vd];
                    checkDirErrN(!varState->isIncomplete && varState->isConstSize(), 4414, varState->name.c_str());
                    checkDirErrN(loopVarNames.find(varState->name) == loopVarNames.end(), 344, varState->name.c_str());
                    if (curPragma->reductions[i].locSize.empty()) {
                        curPragma->reductions[i].locSize.strExpr = toStr(varState->getTotalElemCount());
                    }
                    checkDirErrN((unsigned long long)toNumber(curPragma->reductions[i].locSize.strExpr) <= varState->getTotalElemCount(),
                            345, varState->name.c_str());
                    reductions.insert(vd);
                    if ((possibleTargets & PragmaRegion::DEVICE_TYPE_CUDA) && varState->isArray) {
                        cdvmhLog(WARNING, curPragma->fileName, curPragma->line, 4415, MSG(4415), varState->name.c_str());
                        possibleTargets &= ~(PragmaRegion::DEVICE_TYPE_CUDA);
                    }
                }
            }
            varsToGetActual.clear();
            for (int i = 0; i < (int)curPragma->rmas.size(); i++) {
                std::string origName = curPragma->rmas[i].arrayName;
                int rank = curPragma->rmas[i].rank;
                VarDecl *vd = seekVarDecl(origName);
                checkDirErrN(vd, 301, origName.c_str());
                parallelRmaDesc->arrayDecls.push_back(vd);
                checkIntErrN(varStates.find(vd) != varStates.end(), 92, vd->getNameAsString().c_str());
                VarState *varState = &varStates[vd];
                checkDirErrN(rank == varState->rank, 304, origName.c_str());
                bool toTake;
                if (varState->isDvmArray) {
                    toTake = true;
                    for (int j = 0; j < (int)curPragma->rmas.size(); j++)
                        if (i != j && !curPragma->rmas[j].excluded) {
                            if (curPragma->rmas[j].arrayName == origName) {
                                bool matches = true;
                                checkDirErrN(curPragma->rmas[i].rank == curPragma->rmas[j].rank, 304, origName.c_str());
                                for (int k = 0; k < rank; k++)
                                    matches = matches && curPragma->rmas[j].matches(curPragma->rmas[i].axisRules[k].origExpr.strExpr, k);
                                if (matches) {
                                    toTake = false;
                                    break;
                                }
                            }
                        }
                } else
                    toTake = false;
                if (!toTake) {
                    curPragma->rmas[i].excluded = true;
                    cdvmhLog(WARNING, curPragma->fileName, curPragma->line, 371, MSG(371), origName.c_str());
                }
            }
        } else if (gotPragma->kind == DvmPragma::pkRemoteAccess) {
            PragmaRemoteAccess *curPragma = (PragmaRemoteAccess *)gotPragma;
            checkDirErrN(fileCtx.isCompilable(), 471);
            //checkDirErrN(srcMgr.isWrittenInMainFile(fileLoc), 472);
            // TODO: Allow REMOTE_ACCESS for sequential parts
            checkDirErrN(!inRegion || inHostSection, 473);
            checkDirErrN(!inParLoop, 474);
            checkDirErrN(isa<CompoundStmt>(s), 475);
            rmaStack.resize(rmaStack.size() + 1);
            rmaStack.back().stmt = s;
            rmaStack.back().pragma = curPragma;
            std::map<std::string, int> rmaCount;
            CollectNamesVisitor collectNamesVisitor(comp);
            collectNamesVisitor.TraverseStmt(s);
            std::set<std::string> prohibitedNames = collectNamesVisitor.getNames();
            std::string indent = extractIndent(srcMgr.getCharacterData(srcMgr.translateLineCol(fileID, curPragma->srcLine, 1)));
            indent += indentStep;
            toDelete.resize(toDelete.size() + 1);
            preventExpandToDelete = true;
            std::string toInsert;
            toInsert += "\n";
            if (!opts.lessDvmLines)
                toInsert += indent + genDvmLine(curPragma) + "\n";
            for (int i = 0; i < (int)curPragma->rmas.size(); i++) {
                std::string origName = curPragma->rmas[i].arrayName;
                int rank = curPragma->rmas[i].rank;
                VarDecl *vd = seekVarDecl(origName);
                checkDirErrN(vd, 301, origName.c_str());
                checkIntErrN(varStates.find(vd) != varStates.end(), 92, vd->getNameAsString().c_str());
                VarState *varState = &varStates[vd];
                checkDirErrN(rank == varState->rank, 304, origName.c_str());
                bool toTake;
                if (varState->isDvmArray) {
                    toTake = true;
                    for (int j = 0; j < (int)curPragma->rmas.size(); j++)
                        if (i != j && !curPragma->rmas[j].excluded) {
                            if (curPragma->rmas[j].arrayName == origName) {
                                bool matches = true;
                                checkDirErrN(curPragma->rmas[i].rank == curPragma->rmas[j].rank, 304, origName.c_str());
                                for (int k = 0; k < rank; k++)
                                    matches = matches && curPragma->rmas[j].matches(curPragma->rmas[i].axisRules[k].origExpr.strExpr, k);
                                if (matches) {
                                    toTake = false;
                                    break;
                                }
                            }
                        }
                } else {
                    toTake = false;
                }
                if (toTake) {
                    int curNumber = 1;
                    if (rmaCount.find(origName) != rmaCount.end())
                        curNumber = ++rmaCount[origName];
                    else
                        rmaCount[origName] = 1;
                    std::string rmaName = getUniqueName(origName + "_rma" + toStr(rmaStack.size()) + "_" + toStr(curNumber), &prohibitedNames,
                            &fileCtx.seenMacroNames);
                    VarDecl *vd = seekVarDecl(origName);
                    checkDirErrN(vd, 301, origName.c_str());
                    rmaStack.back().substs[vd].push_back(RmaSubstDesc());
                    RmaSubstDesc *desc = &rmaStack.back().substs[vd].back();
                    desc->clause = curPragma->rmas[i];
                    desc->usedFlag = false;
                    desc->nameSubst = rmaName;
                    // Since it is not in parallel loop, here can be either constant or replication. So we can simply use origExpr
                    for (int k = 0; k < rank; k++)
                        desc->indexSubst.push_back(curPragma->rmas[i].axisRules[k].origExpr.strExpr);
                    toInsert += indent + "DvmType " + rmaName + "[" + toStr(varState->headerArraySize) + "];\n";
                    toInsert += indent + "dvmh_remote_access_C(" + rmaName + ", " + genAlignParams(curPragma, origName, rank, curPragma->rmas[i].axisRules) +
                            ");\n";
                    addToDelete(rmaName);
                } else {
                    curPragma->rmas[i].excluded = true;
                    cdvmhLog(WARNING, curPragma->fileName, curPragma->line, 371, MSG(371), origName.c_str());
                }
            }
            SourceLocation loc = cast<CompoundStmt>(s)->getLBracLoc();
            rewr.InsertText(escapeMacroEnd(loc).getLocWithOffset(1), toInsert, true, false);
        } else if (gotPragma->kind == DvmPragma::pkHostSection) {
            PragmaHostSection *curPragma = (PragmaHostSection *)gotPragma;
            checkDirErrN(fileCtx.isCompilable(), 439);
            //checkDirErrN(srcMgr.isWrittenInMainFile(fileLoc), 4310);
            checkDirErrN(inRegion, 4311);
            checkDirErrN(!inParLoop, 4312);
            checkDirErrN(isa<CompoundStmt>(s), 4313);
            cdvmhLog(TRACE, curPragma->fileName, curPragma->line, "Entering host section");
            inHostSection = true;
            hostSectionStmt = s;
        }
    }
    if (inRegion && s != regionStmt && !inParLoop && !inHostSection) {
        // Sequential part
        checkUserErrN(!isa<DeclStmt>(s), fileName, line, 4314);
        cdvmhLog(TRACE, fileName, line, "Entering sequential part of region");
        fileCtx.setHasLoops();
        inParLoop = true;
        inParLoopBody = true;
        parLoopStmt = s;
        curParallelPragma = 0;
        needsParams.clear();
        outerPrivates.clear();
        parLoopBodyStmt = s;
        innerVars.clear();
        reductions.clear();
        varsToGetActual.clear();
        rmaAppearances.clear();
        parLoopBodyExprCounter = 0;
        delete parallelRmaDesc;
        parallelRmaDesc = 0;
    }
    if (s == parLoopBodyStmt)
        inParLoopBody = true;
    return true;
}

void ConverterASTVisitor::genBlankHandler(const std::string &handlerName, const std::vector<VarDecl *> &outerParams, const std::vector<LoopVarDesc> &loopVars,
        const std::map<int, Decl *> localDecls, std::string &handlerText) {
    std::string indent = indentStep;
    PragmaParallel *curPragma = curParallelPragma;
    bool isSequentialPart = curPragma == 0;
    bool isAcross = (isSequentialPart ? false : !curPragma->acrosses.empty());
    int loopRank = (isSequentialPart ? 0 : curPragma->rank);

    // Pre-handler pragma
    handlerText += "#pragma dvm handler_stub dvm_array(";
    std::string listTemp;
    for (int i = 0; i < (int)outerParams.size(); i++) {
        VarState *varState = &varStates[outerParams[i]];
        if (varState->isDvmArray)
            listTemp += ", " + varState->name;
    }
    trimList(listTemp);
    handlerText += listTemp + "), regular_array(";
    listTemp.clear();
    for (int i = 0; i < (int)outerParams.size(); i++) {
        VarState *varState = &varStates[outerParams[i]];
        if (varState->isArray && !varState->isDvmArray)
            listTemp += ", " + varState->name;
    }
    trimList(listTemp);
    handlerText += listTemp + "), scalar(";
    listTemp.clear();
    for (int i = 0; i < (int)outerParams.size(); i++) {
        VarState *varState = &varStates[outerParams[i]];
        if (!varState->isArray)
            listTemp += ", " + varState->name;
    }
    trimList(listTemp);
    handlerText += listTemp;
    if (!isSequentialPart) {
        handlerText += "), loop_var(";
        listTemp.clear();
        for (int i = 0; i < loopRank; i++)
            listTemp += ", " + loopVars[i].name + "(" + toStr(loopVars[i].stepSign) + ", " + loopVars[i].constStep + ")";
        trimList(listTemp);
        handlerText += listTemp + "), reduction(";
        listTemp.clear();
        for (int i = 0; i < (int)curPragma->reductions.size(); i++)
            listTemp += ", " + curPragma->reductions[i].toClause();
        trimList(listTemp);
        handlerText += listTemp + "), private(";
        listTemp.clear();
        for (int i = 0; i < (int)curPragma->privates.size(); i++)
            listTemp += ", " + curPragma->privates[i];
        trimList(listTemp);
        handlerText += listTemp;
        // TODO: Add RMA to sequential part as well
        handlerText += "), remote_access(";
        listTemp.clear();
        int rmaIndex = 0;
        for (int i = 0; i < (int)curPragma->rmas.size(); i++) {
            if (!curPragma->rmas[i].excluded) {
                rmaIndex++;
                listTemp += ", " + curPragma->rmas[i].arrayName;
                for (int j = 0; j < (int)curPragma->rmas[i].axisRules.size(); j++) {
                    AlignAxisRule *rule = &curPragma->rmas[i].axisRules[j];
                    listTemp += "[";
                    if (rule->axisNumber >= 0)
                        listTemp += rule->origExpr.strExpr;
                    listTemp += "]";
                }
                listTemp += "(";
                if (curPragma->rmas[i].nonConstRank > 0) {
                    for (int j = 0; j < (int)curPragma->rmas[i].axes.size(); j++) {
                        int arrayAxis = curPragma->rmas[i].axes[j];
                        AlignAxisRule *rule = &curPragma->rmas[i].axisRules[arrayAxis - 1];
                        listTemp += "[";
                        if (rule->axisNumber > 0)
                            listTemp += loopVars[rule->axisNumber - 1].name;
                        listTemp += "]";
                    }
                } else {
                    listTemp += "[0]";
                }
                listTemp += ", appearances(";
                std::string listTemp2;
                for (int j = 0; j < (int)rmaAppearances.size(); j++) {
                    if (rmaAppearances[j].first == rmaIndex)
                        listTemp2 += ", " + toStr(rmaAppearances[j].second);
                }
                trimList(listTemp2);
                listTemp += listTemp2 + "))";
            }
        }
        trimList(listTemp);
        handlerText += listTemp;
    }
    if (isAcross) {
        int minAcrossCount = curPragma->acrosses[0].getDepCount();
        int maxAcrossCount = 0;
        for (int i = 0; i < (int)curPragma->acrosses.size(); i++) {
            int curCount = curPragma->acrosses[i].getDepCount();
            minAcrossCount = std::max(minAcrossCount, curCount);
            maxAcrossCount += curCount;
        }
        minAcrossCount = std::min(minAcrossCount, loopRank);
        maxAcrossCount = std::min(maxAcrossCount, loopRank);
        handlerText += "), across(" + toStr(minAcrossCount) + ", " + toStr(maxAcrossCount);
    }
    handlerText += ")\n"; // end of pragma
    handlerText += "void " + handlerName + "(";
    listTemp.clear();
    for (int i = 0; i < (int)outerParams.size(); i++) {
        VarState *varState = &varStates[outerParams[i]];
        listTemp += ", " + varState->baseTypeStr;
        if (varState->isArray) {
            if (varState->isIncomplete) {
                if (varState->canBeRestrict) {
                    listTemp += " (* __restrict__ " + varState->name + ")";
                } else {
                    listTemp += " " + varState->name + "[]";
                }
            } else {
                listTemp += " " + varState->name;
            }
            for (int j = varState->isIncomplete; j < varState->rank; j++) {
                listTemp += "[" + (varState->constSize[j] ? varState->sizes[j].strExpr : "DVMH_VARIABLE_ARRAY_SIZE") + "]";
            }
        } else {
            listTemp += " " + varState->name;
        }
    }
    trimList(listTemp);
    handlerText += listTemp + ") {\n";
    for (std::map<int, Decl *>::const_iterator it = localDecls.begin(); it != localDecls.end(); it++) {
        Decl *d = it->second;
        handlerText += indent + declToStr(d, false, false, false) + ";\n";
    }
    for (std::set<VarDecl *>::iterator it = outerPrivates.begin(); it != outerPrivates.end(); it++) {
        VarState *varState = &varStates[*it];
        handlerText += indent + varState->genDecl() + ";\n";
    }
    for (std::set<VarDecl *>::iterator it = reductions.begin(); it != reductions.end(); it++) {
        VarState *varState = &varStates[*it];
        handlerText += indent + varState->genDecl() + ";\n";
    }
    handlerText += "\n";
    indent += genIndent(std::max(1, loopRank), fileCtx.useTabs());
    std::string loopBody = convertToString(parLoopBodyStmt);
    if (isa<CompoundStmt>(parLoopBodyStmt))
        indent = subtractIndent(indent);
    else
        handlerText += subtractIndent(indent) + "{\n";
    int lastPos = 0;
    while (loopBody.find('\n', lastPos) != std::string::npos) {
        int nlPos = loopBody.find('\n', lastPos);
        handlerText += indent + loopBody.substr(lastPos, nlPos - lastPos + 1);
        lastPos = nlPos + 1;
        if (lastPos >= (int)loopBody.size())
            break;
    }
    if (lastPos < (int)loopBody.size())
        handlerText += indent + loopBody.substr(lastPos) + "\n";
    if (!isFull(parLoopBodyStmt)) {
        handlerText[handlerText.size() - 1] = ';';
        handlerText += "\n";
    }
    if (!isa<CompoundStmt>(parLoopBodyStmt))
        handlerText += subtractIndent(indent) + "}\n";
    handlerText += "\n";
    handlerText += "}\n";
}

void ConverterASTVisitor::genHostHandler(std::string handlerName, const std::vector<VarDecl *> &outerParams, const std::vector<LoopVarDesc> &loopVars,
        std::string &handlerFormalParams, std::string &handlerBody, bool doingOpenMP) {
    std::string indent = indentStep;
    PragmaParallel *curPragma = curParallelPragma;
    bool isSequentialPart = curPragma == 0;
    int loopRank = (isSequentialPart ? 0 : curPragma->rank);
    bool isAcross = (isSequentialPart ? false : curPragma->acrosses.size() > 0);
    assert(!doingOpenMP || (loopRank >= 1 && (!isAcross || loopRank >= 2)));

    std::set<std::string> prohibitedNames;
    CollectNamesVisitor collectNamesVisitor(comp);
    collectNamesVisitor.TraverseStmt(parLoopBodyStmt);
    prohibitedNames = collectNamesVisitor.getNames();
    for (int i = 0; i < (int)outerParams.size(); i++)
        prohibitedNames.insert(outerParams[i]->getName().str());
    for (std::set<VarDecl *>::iterator it = outerPrivates.begin(); it != outerPrivates.end(); it++)
        prohibitedNames.insert((*it)->getName().str());
    for (std::set<VarDecl *>::iterator it = reductions.begin(); it != reductions.end(); it++)
        prohibitedNames.insert((*it)->getName().str());

    // XXX: collision detection with macro names can be avoided if host handlers will be placed in separate file
    std::string device_num = getUniqueName("device_num", &prohibitedNames, &fileCtx.seenMacroNames);
    std::string loop_ref = getUniqueName("loop_ref", &prohibitedNames, &fileCtx.seenMacroNames);
    std::string pLoopRef = getUniqueName("pLoopRef", &prohibitedNames, &fileCtx.seenMacroNames);
    std::string boundsLow = getUniqueName("boundsLow", &prohibitedNames, &fileCtx.seenMacroNames);
    std::string boundsHigh = getUniqueName("boundsHigh", &prohibitedNames, &fileCtx.seenMacroNames);
    std::string loopSteps = getUniqueName("loopSteps", &prohibitedNames, &fileCtx.seenMacroNames);
    std::string slotCount = getUniqueName("slotCount", &prohibitedNames, &fileCtx.seenMacroNames);
    std::string dependencyMask = getUniqueName("dependencyMask", &prohibitedNames, &fileCtx.seenMacroNames);
    std::string currentThread = getUniqueName("currentThread", &prohibitedNames, &fileCtx.seenMacroNames);
    std::string workingThreads = getUniqueName("workingThreads", &prohibitedNames, &fileCtx.seenMacroNames);
    std::string threadSync = getUniqueName("threadSync", &prohibitedNames, &fileCtx.seenMacroNames);
    std::map<std::string, std::string> dvmHeaders;
    std::map<std::string, std::string> scalarPtrs;
    for (int i = 0; i < (int)outerParams.size(); i++) {
        VarState *varState = &varStates[outerParams[i]];
        if (varState->isArray) {
            dvmHeaders[varState->name] = getUniqueName(varState->name + "_hdr", &prohibitedNames, &fileCtx.seenMacroNames);
        } else {
            scalarPtrs[varState->name] = getUniqueName(varState->name + "_ptr", &prohibitedNames, &fileCtx.seenMacroNames);
        }
    }

    handlerFormalParams += "DvmType *" + pLoopRef;
    handlerBody += indent + "/* Loop reference and device number */\n";
    handlerBody += indent + "DvmType " + loop_ref + " = *" + pLoopRef + ";\n";
    handlerBody += indent + "DvmType " + device_num + " = dvmh_loop_get_device_num_C(" + loop_ref + ");\n";
    handlerBody += indent + "/* Parameters */\n";
    for (int i = 0; i < (int)outerParams.size(); i++) {
        VarState *varState = &varStates[outerParams[i]];
        if (varState->isArray) {
            int rank = varState->rank;
            std::string refName = varState->name;
            std::string hdrName = dvmHeaders[refName];
            handlerFormalParams += ", DvmType " + hdrName + "[]";
            handlerBody += indent + varState->baseTypeStr + " (*" + (varState->canBeRestrict ? " DVMH_RESTRICT " : "") + refName + ")";
            std::string castType = varState->baseTypeStr + " (*)";
            for (int j = 2; j <= rank; j++) {
                int hdrIdx = j - 1;
                std::string curSize;
                if (varState->isDvmArray || !varState->constSize[j - 1]) {
                    if (j < rank)
                        curSize = hdrName + "[" + toStr(hdrIdx) + "]/" + hdrName + "[" + toStr(hdrIdx + 1) + "]";
                    else
                        curSize = hdrName + "[" + toStr(hdrIdx) + "]";
                } else {
                    curSize = varState->sizes[j - 1].strExpr;
                }
                handlerBody += "[" + curSize + "]";
                castType += "[" + curSize + "]";
            }
            handlerBody += " = (" + castType + ")dvmh_get_natural_base_C(" + device_num + ", " + hdrName + ");\n";
        } else {
            handlerFormalParams += ", " + varState->baseTypeStr + " *" + (varState->canBeRestrict ? " DVMH_RESTRICT " : "") + scalarPtrs[varState->name];
            handlerBody += indent + varState->baseTypeStr + " " +
                    (fileCtx.getInputFile().CPlusPlus ? "&" + std::string(varState->canBeRestrict ? " DVMH_RESTRICT_REF " : "") : "") +
                    varState->name + " = *" + scalarPtrs[varState->name] + ";\n";
        }
    }
    // Handler heading is done
    if (!isSequentialPart) {
        handlerBody += indent + "/* Supplementary variables for loop handling */\n";
        handlerBody += indent + "DvmType " + boundsLow + "[" + toStr(loopRank) + "], " + boundsHigh + "[" + toStr(loopRank) + "], " + loopSteps + "["
                + toStr(loopRank) + "];\n";
        if (doingOpenMP) {
            handlerBody += indent + "int " + slotCount + ";\n";
            if (isAcross)
                handlerBody += indent + "DvmType " + dependencyMask + ";\n";
        }
    }
    handlerBody += indent + "/* User variables - loop index variables and other private variables */\n";
    for (std::set<VarDecl *>::iterator it = outerPrivates.begin(); it != outerPrivates.end(); it++) {
        VarState *varState = &varStates[*it];
        handlerBody += indent + varState->genDecl() + ";\n";
    }
    for (std::set<VarDecl *>::iterator it = reductions.begin(); it != reductions.end(); it++) {
        VarState *varState = &varStates[*it];
        handlerBody += indent + varState->genDecl() + ";\n";
    }
    handlerBody += "\n";
    if (!isSequentialPart) {
        handlerBody += indent + "dvmh_loop_fill_bounds_C(" + loop_ref + ", " + boundsLow + ", " + boundsHigh + ", " + loopSteps + ");\n";
    }
    if (doingOpenMP) {
        handlerBody += indent + slotCount + " = dvmh_loop_get_slot_count_C(" + loop_ref + ");\n";
        if (isAcross) {
            handlerBody += indent + dependencyMask + " = dvmh_loop_get_dependency_mask_C(" + loop_ref + ");\n";
            handlerBody += "#ifdef _OPENMP\n";
            handlerBody += indent + "int " + threadSync + "[" + slotCount + "];\n";
            handlerBody += "#endif\n";
        }
    }
    std::set<int> ompReds;
    if (!isSequentialPart)
        for (int i = 0; i < (int)curPragma->reductions.size(); i++) {
            ClauseReduction *red = &curPragma->reductions[i];
            VarDecl *vd = seekVarDecl(red->arrayName);
            checkIntErrN(vd && reductions.find(vd) != reductions.end(), 97, red->arrayName.c_str(), curPragma->fileName.c_str(), curPragma->line);
            VarState *varState = &varStates[vd];
            if (red->isLoc()) {
                VarDecl *vd = seekVarDecl(red->locName);
                checkIntErrN(vd && reductions.find(vd) != reductions.end(), 97, red->locName.c_str(), curPragma->fileName.c_str(), curPragma->line);
                VarState *locVarState = &varStates[vd];
                if (!doingOpenMP || (opts.useOmpReduction && red->hasOpenMP() && !varState->isArray && !locVarState->isArray)) {
                    handlerBody += indent + "dvmh_loop_red_init_C(" + loop_ref + ", " + toStr(i + 1) + ", " + (varState->isArray ? "" : "&") + red->arrayName +
                            ", " + (locVarState->isArray ? "" : "&") + red->locName + ");\n";
                    if (opts.dvmDebugLvl & (dlReadVariables | dlWriteVariables)) {
                        handlerBody += indent + "dvmh_dbg_loop_handler_red_init_C(" + loop_ref + ", " + toStr(i + 1) + ", " +
                                       (varState->isArray ? "" : "&") + red->arrayName + ", \"" + red->arrayName + "\");\n";
                    }
                    if (doingOpenMP)
                        ompReds.insert(i);
                }
            } else {
                if (!doingOpenMP || (opts.useOmpReduction && red->hasOpenMP() && !varState->isArray)) {
                    handlerBody += indent + "dvmh_loop_red_init_C(" + loop_ref + ", " + toStr(i + 1) + ", " + (varState->isArray ? "" : "&") + red->arrayName +
                            ", 0);\n";
                    if (opts.dvmDebugLvl & (dlReadVariables | dlWriteVariables)) {
                        handlerBody += indent + "dvmh_dbg_loop_handler_red_init_C(" + loop_ref + ", " + toStr(i + 1) + ", " +
                                       (varState->isArray ? "" : "&") + red->arrayName + ", \"" + red->arrayName + "\");\n";
                    }
                    if (doingOpenMP)
                        ompReds.insert(i);
                }
            }
        }
    handlerBody += "\n";
    if (doingOpenMP) {
        handlerBody += "#ifdef _OPENMP\n";
        handlerBody += indent + "#pragma omp parallel num_threads(" + slotCount + ")";
        if (outerPrivates.size() > 0) {
            handlerBody += ", private(";
            std::string privateList;
            for (std::set<VarDecl *>::iterator it = outerPrivates.begin(); it != outerPrivates.end(); it++)
                privateList += ", " + (*it)->getName().str();
            trimList(privateList);
            handlerBody += privateList + ")";
        }
        for (int i = 0; i < (int)curPragma->reductions.size(); i++) {
            if (ompReds.find(i) != ompReds.end())
                handlerBody += ", reduction(" + curPragma->reductions[i].toOpenMP() + ":" + curPragma->reductions[i].arrayName + ")";
            else
                handlerBody += ", private(" + curPragma->reductions[i].arrayName +
                        (curPragma->reductions[i].isLoc() ? ", " + curPragma->reductions[i].locName : "") + ")";
        }
        handlerBody += "\n";
        handlerBody += "#endif\n";
        handlerBody += indent + "{\n";
        indent += indentStep;
        if (isAcross) {
            handlerBody += "#ifdef _OPENMP\n";
            handlerBody += indent + "int " + currentThread + " = 0, " + workingThreads + " = " + slotCount + ";\n";
            handlerBody += "#endif\n";
        }
        for (int i = 0; i < (int)curPragma->reductions.size(); i++)
            if (ompReds.find(i) == ompReds.end()) {
                ClauseReduction *red = &curPragma->reductions[i];
                VarDecl *vd = seekVarDecl(red->arrayName);
                VarState *varState = &varStates[vd];
                if (red->isLoc()) {
                    VarDecl *vd = seekVarDecl(red->locName);
                    VarState *locVarState = &varStates[vd];
                    handlerBody += indent + "dvmh_loop_red_init_C(" + loop_ref + ", " + toStr(i + 1) + ", " + (varState->isArray ? "" : "&") + red->arrayName +
                            ", " + (locVarState->isArray ? "" : "&") + red->locName + ");\n";
                } else
                    handlerBody += indent + "dvmh_loop_red_init_C(" + loop_ref + ", " + toStr(i + 1) + ", " + (varState->isArray ? "" : "&") + red->arrayName +
                            ", 0);\n";
            }
    }
    int loopVariantCount = (doingOpenMP && isAcross ? loopRank + 1 : 1);
    for (int parInd = 0; parInd < loopVariantCount; parInd++) {
        if (loopVariantCount > 1) {
            handlerBody += indent;
            if (parInd > 0)
                handlerBody += "} else ";
            if (parInd < loopRank)
                handlerBody += "if (((" + dependencyMask + " >> " + toStr(loopRank - parInd - 1) + ") & 1) == 0) ";
            handlerBody += "{\n";
            indent += indentStep;
        }
        std::string indentSave = indent;
        for (int i = 0; i < loopRank; i++) {
            if (doingOpenMP && parInd == i) {
                handlerBody += "#ifdef _OPENMP\n";
                handlerBody += indent + "#pragma omp for schedule(runtime), nowait\n";
                handlerBody += "#endif\n";
            }
            if (doingOpenMP && parInd == loopRank) {
                // TODO: implement. What?
                if (i == 0) {
                    handlerBody += "#ifdef _OPENMP\n";
                    std::string paralIters = "(" + boundsHigh + "[1] - " + boundsLow + "[1]) / " + loopSteps + "[1] + 1";
                    handlerBody += indent + "if (" + paralIters + " < " + workingThreads + ")\n";
                    handlerBody += indent + indentStep + workingThreads + " = " + paralIters + ";\n";
                    handlerBody += indent + currentThread + " = omp_get_thread_num();\n";
                    handlerBody += indent + threadSync + "[" + currentThread + "] = 0;\n";
                    handlerBody += indent + "#pragma omp barrier\n";
                    handlerBody += "#endif\n";
                } else if (i == 1) {
                    handlerBody += subtractIndent(indent) + "{\n";
                    handlerBody += "#ifdef _OPENMP\n";
                    handlerBody += indent + "if (" + currentThread + " > 0 && " + currentThread + " < " + workingThreads + ") {\n";
                    indent += indentStep;
                    handlerBody += indent + "do {\n";
                    handlerBody += indent + indentStep + "#pragma omp flush(" + threadSync + ")\n";
                    handlerBody += indent + "} while (!" + threadSync + "[" + currentThread + " - 1]);\n";
                    handlerBody += indent + threadSync + "[" + currentThread + " - 1] = 0;\n";
                    handlerBody += indent + "#pragma omp flush(" + threadSync + ")\n";
                    indent = subtractIndent(indent);
                    handlerBody += indent + "}\n";
                    handlerBody += indent + "#pragma omp for schedule(static), nowait\n";
                    handlerBody += "#endif\n";
                }
            }
            handlerBody += indent + "for (" + loopVars[i].name + " = " + boundsLow + "[" + toStr(i) + "]; " + loopVars[i].name + " " +
                    (loopVars[i].stepSign > 0 ? "<=" : ">=") + " " + boundsHigh + "[" + toStr(i) + "]; " + loopVars[i].name;
            if (loopVars[i].constStep == "1")
                handlerBody += "++";
            else if (loopVars[i].constStep == "-1")
                handlerBody += "--";
            else if (!loopVars[i].constStep.empty())
                handlerBody += " += (" + loopVars[i].constStep + ")";
            else
                handlerBody += " += " + loopSteps + "[" + toStr(i) + "]";
            handlerBody += ")\n";
            indent += indentStep;
        }
        if (!isSequentialPart && opts.dvmDebugLvl > 0) {
            handlerBody += indent + "{ dvmh_dbg_loop_iter_C(" + fileCtx.dvm0c(loopRank);
            for (int i = 0; i < loopRank; i++)
                handlerBody += ", (DvmType)(&" + loopVars[i].name + ")";
            for (int i = 0; i < loopRank; i++)
                handlerBody += ", " + genRtType(loopVars[i].baseTypeStr);
            handlerBody += ");\n";
        }
        if (isSequentialPart)
            indent += indentStep;
        std::string loopBody = convertToString(parLoopBodyStmt);
        if (isa<CompoundStmt>(parLoopBodyStmt))
            indent = subtractIndent(indent);
        else
            handlerBody += subtractIndent(indent) + "{\n";
        int lastPos = 0;
        while (loopBody.find('\n', lastPos) != std::string::npos) {
            int nlPos = loopBody.find('\n', lastPos);
            handlerBody += indent + loopBody.substr(lastPos, nlPos - lastPos + 1);
            lastPos = nlPos + 1;
            if (lastPos >= (int)loopBody.size())
                break;
        }
        if (lastPos < (int)loopBody.size())
            handlerBody += indent + loopBody.substr(lastPos) + "\n";
        if (!isFull(parLoopBodyStmt)) {
            handlerBody[handlerBody.size() - 1] = ';';
            handlerBody += "\n";
        }
        if (!isSequentialPart && opts.dvmDebugLvl > 0)
            handlerBody += indent + "}\n";
        if (!isa<CompoundStmt>(parLoopBodyStmt))
            handlerBody += subtractIndent(indent) + "}\n";
        indent = indentSave;
        if (doingOpenMP && parInd == loopRank) {
            indent += indentStep;
            handlerBody += "#ifdef _OPENMP\n";
            handlerBody += indent + "if (" + currentThread + " < " + workingThreads + " - 1) {\n";
            indent += indentStep;
            handlerBody += indent + "do {\n";
            handlerBody += indent + indentStep + "#pragma omp flush(" + threadSync + ")\n";
            handlerBody += indent + "} while (" + threadSync + "[" + currentThread + "]);\n";
            handlerBody += indent + threadSync + "[" + currentThread + "] = 1;\n";
            handlerBody += indent + "#pragma omp flush(" + threadSync + ")\n";
            indent = subtractIndent(indent);
            handlerBody += indent + "}\n";
            handlerBody += "#endif\n";
            indent = subtractIndent(indent);
            handlerBody += indent + "}\n";
        }
        if (loopVariantCount > 1)
            indent = subtractIndent(indent);
    }
    if (loopVariantCount > 1)
        handlerBody += indent + "}\n"; // across variants
    if (doingOpenMP) {
        for (int i = 0; i < (int)curPragma->reductions.size(); i++)
            if (ompReds.find(i) == ompReds.end()) {
                ClauseReduction *red = &curPragma->reductions[i];
                VarDecl *vd = seekVarDecl(red->arrayName);
                VarState *varState = &varStates[vd];
                if (red->isLoc()) {
                    VarDecl *vd = seekVarDecl(red->locName);
                    VarState *locVarState = &varStates[vd];
                    handlerBody += indent + "dvmh_loop_red_post_C(" + loop_ref + ", " + toStr(i + 1) + ", " + (varState->isArray ? "" : "&") + red->arrayName +
                            ", " + (locVarState->isArray ? "" : "&") + red->locName + ");\n";
                } else
                    handlerBody += indent + "dvmh_loop_red_post_C(" + loop_ref + ", " + toStr(i + 1) + ", " + (varState->isArray ? "" : "&") + red->arrayName +
                            ", 0);\n";
            }
        indent = subtractIndent(indent);
        handlerBody += indent + "}\n"; // omp parallel
    }
    if (!isSequentialPart) {
        if (curPragma->reductions.size() > 0)
            handlerBody += "\n";
        for (int i = 0; i < (int)curPragma->reductions.size(); i++) {
            ClauseReduction *red = &curPragma->reductions[i];
            VarDecl *vd = seekVarDecl(red->arrayName);
            assert(vd); // is consequent of upper part
            assert(reductions.find(vd) != reductions.end()); // is consequent of upper part
            VarState *varState = &varStates[vd];
            if (red->isLoc()) {
                VarDecl *vd = seekVarDecl(red->locName);
                assert(vd); // is consequent of upper part
                assert(reductions.find(vd) != reductions.end()); // is consequent of upper part
                VarState *locVarState = &varStates[vd];
                if (!doingOpenMP || ompReds.find(i) != ompReds.end())
                    handlerBody += indent + "dvmh_loop_red_post_C(" + loop_ref + ", " + toStr(i + 1) + ", " + (varState->isArray ? "" : "&") + red->arrayName +
                            ", " + (locVarState->isArray ? "" : "&") + red->locName + ");\n";
            } else {
                if (!doingOpenMP || ompReds.find(i) != ompReds.end())
                    handlerBody += indent + "dvmh_loop_red_post_C(" + loop_ref + ", " + toStr(i + 1) + ", " + (varState->isArray ? "" : "&") + red->arrayName +
                            ", 0);\n";
            }
        }
    }
    if (isSequentialPart && !fileCtx.getInputFile().CPlusPlus) {
        bool firstFlag = true;
        for (int i = 0; i < (int)outerParams.size(); i++) {
            VarState *varState = &varStates[outerParams[i]];
            if (!varState->isArray) {
                if (firstFlag)
                    handlerBody += "\n";
                firstFlag = false;
                handlerBody += indent + "*" + scalarPtrs[varState->name] + " = " + varState->name + ";\n";
            }
        }
    }
}

void ConverterASTVisitor::genCudaKernel(const KernelDesc &kernelDesc, const std::vector<VarDecl *> &outerParams, const std::vector<LoopVarDesc> &loopVars,
        std::string handlerTemplateDecl, std::string &kernelText)
{
    std::string kernelName = kernelDesc.kernelName;
    std::string indexT = kernelDesc.indexT;
    std::string indent = indentStep;
    PragmaParallel *curPragma = curParallelPragma;
    bool isSequentialPart = curPragma == 0;
    int loopRank = (isSequentialPart ? 0 : curPragma->rank);
    bool isAcross = (isSequentialPart ? false : curPragma->acrosses.size() > 0);
    bool isOneThread = isAcross || opts.oneThread;
    bool autoTfm = opts.autoTfm;
    bool prepareDiag = isAcross && autoTfm && (curPragma->acrosses.size() > 1 || curPragma->acrosses[0].getDepCount() > 1);
    // TODO: Handle ACROSS not only in oneThread mode
    assert(!isAcross || isOneThread); // is consequent of upper part

    std::set<std::string> prohibitedNames;
    CollectNamesVisitor collectNamesVisitor(comp);
    collectNamesVisitor.TraverseStmt(parLoopBodyStmt);
    prohibitedNames = collectNamesVisitor.getNames();
    for (int i = 0; i < (int)outerParams.size(); i++)
        prohibitedNames.insert(outerParams[i]->getName().str());
    for (std::set<VarDecl *>::iterator it = outerPrivates.begin(); it != outerPrivates.end(); it++)
        prohibitedNames.insert((*it)->getName().str());
    for (std::set<VarDecl *>::iterator it = reductions.begin(); it != reductions.end(); it++)
        prohibitedNames.insert((*it)->getName().str());

    std::string kernelFormalParams;
    std::string kernelBody;
    std::string boundsLow = getUniqueName("boundsLow", &prohibitedNames);
    std::string boundsHigh = getUniqueName("boundsHigh", &prohibitedNames);
    std::string loopSteps = getUniqueName("loopSteps", &prohibitedNames);
    std::string blocksS = getUniqueName("blocksS", &prohibitedNames);
    std::string restBlocks = getUniqueName("restBlocks", &prohibitedNames);
    std::string blockOffset = getUniqueName("blockOffset", &prohibitedNames);
    std::string curBlocks = getUniqueName("curBlocks", &prohibitedNames);
    std::map<std::string, std::string> scalarPtrs;
    std::map<std::string, std::vector<std::string> > dvmCoefs;
    std::map<std::string, std::map<std::string, std::string> > dvmDiagInfos;
    std::map<std::string, std::string> redGrid;
    for (int i = 0; i < (int)outerParams.size(); i++) {
        VarState *varState = &varStates[outerParams[i]];
        if (varState->isArray) {
            dvmCoefs[varState->name].clear();
            for (int j = 0; j < varState->headerArraySize; j++)
                dvmCoefs[varState->name].push_back(getUniqueName(varState->name + "_hdr" + toStr(j), &prohibitedNames));
            if (prepareDiag) {
                std::map<std::string, std::string> &m = dvmDiagInfos[varState->name];
                m.clear();
                m["tfmType"] = getUniqueName(varState->name + "_tfmType", &prohibitedNames);
                m["xAxis"] = getUniqueName(varState->name + "_xAxis", &prohibitedNames);
                m["yAxis"] = getUniqueName(varState->name + "_yAxis", &prohibitedNames);
                m["Rx"] = getUniqueName(varState->name + "_Rx", &prohibitedNames);
                m["Ry"] = getUniqueName(varState->name + "_Ry", &prohibitedNames);
                m["xOffset"] = getUniqueName(varState->name + "_xOffset", &prohibitedNames);
                m["yOffset"] = getUniqueName(varState->name + "_yOffset", &prohibitedNames);
            }
        } else {
            scalarPtrs[varState->name] = getUniqueName(varState->name + "_ptr", &prohibitedNames);
        }
    }
    for (std::set<VarDecl *>::iterator it = reductions.begin(); it != reductions.end(); it++) {
        VarState *varState = &varStates[*it];
        redGrid[varState->name] = getUniqueName(varState->name + "_grid", &prohibitedNames);
    }

    kernelBody += indent + "/* Parameters */\n";
    for (int i = 0; i < (int)outerParams.size(); i++) {
        VarState *varState = &varStates[outerParams[i]];
        int rank = varState->rank;
        std::string refName = varState->name;
        if (varState->isArray) {
            // XXX: Not so good solution, maybe
            std::string devBaseName = refName + "_base";

            if (rank > 1) {
                std::string elemT = varState->baseTypeStr; // TODO: Add 'const' where appropriate
                std::string ptrT = elemT + " *" + (varState->canBeRestrict ? " DVMH_RESTRICT" : "");
                kernelFormalParams += ", " + ptrT + " " + devBaseName;
                kernelBody += indent + (autoTfm ? (prepareDiag ? "DvmhDiagonalizedArrayHelper" : "DvmhPermutatedArrayHelper") : "DvmhArrayHelper") + "<" +
                        toStr(rank) + ", " + elemT + ", " + ptrT + ", " + indexT + "> " + refName + "(" + devBaseName;
                kernelBody += ", DvmhArrayCoefficients<" + toStr(autoTfm ? rank : rank - 1) + ", " + indexT +">(";
                std::string coefList;
                for (int j = 1; j <= rank; j++) {
                    int hdrIdx = j;
                    std::string coefName = dvmCoefs[refName][hdrIdx];
                    if (j < rank || autoTfm) {
                        kernelFormalParams += ", " + indexT + " " + coefName;
                        coefList += ", " + coefName;
                    }
                }
                trimList(coefList);
                kernelBody += coefList + ")";
                if (prepareDiag) {
                    std::map<std::string, std::string> &m = dvmDiagInfos[refName];
                    kernelBody += ", DvmhDiagInfo<" + indexT + ">(";
                    kernelFormalParams += ", int " + m["tfmType"];
                    kernelBody += m["tfmType"];
                    kernelFormalParams += ", int " + m["xAxis"];
                    kernelBody += ", " + m["xAxis"];
                    kernelFormalParams += ", int " + m["yAxis"];
                    kernelBody += ", " + m["yAxis"];
                    kernelFormalParams += ", " + indexT + " " + m["Rx"];
                    kernelBody += ", " + m["Rx"];
                    kernelFormalParams += ", " + indexT + " " + m["Ry"];
                    kernelBody += ", " + m["Ry"];
                    kernelFormalParams += ", " + indexT + " " + m["xOffset"];
                    kernelBody += ", " + m["xOffset"];
                    kernelFormalParams += ", " + indexT + " " + m["yOffset"];
                    kernelBody += ", " + m["yOffset"];
                    kernelBody += ")";
                }
                kernelBody += ");\n";
            } else {
                kernelFormalParams += ", " + varState->baseTypeStr + " *" + (varState->canBeRestrict ? " DVMH_RESTRICT " : "") + refName;
            }
        } else {
            // TODO: Add 'use' case for variables
            kernelFormalParams += ", " + varState->baseTypeStr + " *" + (varState->canBeRestrict ? " DVMH_RESTRICT " : "") + scalarPtrs[refName];
            kernelBody += indent + varState->baseTypeStr + " &" + (varState->canBeRestrict ? " DVMH_RESTRICT_REF " : "") + refName +
                    " = *" + scalarPtrs[refName] + ";\n";
        }
    }
    if (!isSequentialPart) {
        if (!isOneThread) {
            kernelBody += indent + "/* Supplementary variables for loop handling */\n";
            kernelBody += indent + indexT + " " + restBlocks + ", " + curBlocks + ";\n";
        }
    }
    kernelBody += indent + "/* User variables - loop index variables and other private variables */\n";
    for (std::set<VarDecl *>::iterator it = outerPrivates.begin(); it != outerPrivates.end(); it++) {
        VarState *varState = &varStates[*it];
        kernelBody += indent + varState->genDecl() + ";\n";
    }
    kernelBody += "\n";

    if (!isSequentialPart) {
        for (int i = 0; i < loopRank; i++) {
            kernelFormalParams += ", " + indexT + " " + boundsLow + "_" + toStr(i + 1) +
                    ", " + indexT + " " + boundsHigh + "_" + toStr(i + 1);
            if (loopVars[i].constStep.empty())
                kernelFormalParams += ", " + indexT + " " + loopSteps + "_" + toStr(i + 1);
            if (!isOneThread && i > 0)
                kernelFormalParams += ", " + indexT + " " + blocksS + "_" + toStr(i + 1);
        }
    }
    if (!isSequentialPart) {
        for (int i = 0; i < (int)curPragma->reductions.size(); i++) {
            // TODO: Add support for reduction arrays
            ClauseReduction *red = &curPragma->reductions[i];
            std::string epsGrid = redGrid[red->arrayName];
            VarDecl *vd = seekVarDecl(red->arrayName);
            checkIntErrN(vd && reductions.find(vd) != reductions.end(), 97, red->arrayName.c_str(), curPragma->fileName.c_str(), curPragma->line);
            VarState *varState = &varStates[vd];
            checkDirErrN(!varState->isArray, 4417, varState->name.c_str());

            kernelFormalParams += ", " + varState->baseTypeStr + " " + red->arrayName;
            kernelFormalParams += ", " + varState->baseTypeStr + " " + epsGrid + "[]";
            if (red->isLoc()) {
                std::string locGrid = redGrid[red->locName];
                VarDecl *lvd = seekVarDecl(red->locName);
                checkIntErrN(lvd && reductions.find(lvd) != reductions.end(), 97, red->locName.c_str(), curPragma->fileName.c_str(), curPragma->line);
                VarState *locVarState = &varStates[lvd];
                checkDirErrN(!locVarState->isArray, 4417, locVarState->name.c_str());

                kernelFormalParams += ", " + locVarState->baseTypeStr + " " + red->locName;
                kernelFormalParams += ", " + locVarState->baseTypeStr + " " + locGrid + "[]";

            }
        }
    }
    if (!isSequentialPart && !isOneThread)
        kernelFormalParams += ", " + indexT + " " + blockOffset;
    trimList(kernelFormalParams);
    // kernelFormalParams is done
    for (int i = 1; i <= loopRank; i++) {
        int idx = i - 1;
        if (!isOneThread) {
            if (i == 1)
                kernelBody += indent + restBlocks + " = blockIdx.x + " + blockOffset + ";\n";
            else
                kernelBody += indent + restBlocks + " = " + restBlocks + " - " + curBlocks + " * " + blocksS + "_" + toStr(i) + ";\n";
            if (i < loopRank)
                kernelBody += indent + curBlocks + " = " + restBlocks + " / " + blocksS + "_" + toStr(i + 1) + ";\n";
            else
                kernelBody += indent + curBlocks + " = " + restBlocks + ";\n";
            kernelBody += indent + loopVars[idx].name + " = " + boundsLow + "_" + toStr(i);
            if (loopVars[idx].constStep == "1")
                kernelBody += " + ";
            else if (loopVars[idx].constStep == "-1")
                kernelBody += " - ";
            else if (!loopVars[idx].constStep.empty())
                kernelBody += " + (" + loopVars[idx].constStep + ") * ";
            else
                kernelBody += " + " + loopSteps + "_" + toStr(i) + " * ";
            if (i == loopRank)
                kernelBody += "(" + curBlocks + " * blockDim.x + threadIdx.x);\n";
            else if (i == loopRank - 1)
                kernelBody += "(" + curBlocks + " * blockDim.y + threadIdx.y);\n";
            else if (i == loopRank - 2)
                kernelBody += "(" + curBlocks + " * blockDim.z + threadIdx.z);\n";
            else
                kernelBody += curBlocks + ";\n";
            kernelBody += indent + "if (" + loopVars[idx].name + (loopVars[idx].stepSign > 0 ? " <= " : " >= ") + boundsHigh + "_" + toStr(i) + ")" +
                    (i < loopRank ? " {\n" : "\n");
        } else {
            kernelBody += indent + "for (" + loopVars[idx].name + " = " + boundsLow + "_" + toStr(i) + "; " + loopVars[idx].name +
                    (loopVars[idx].stepSign > 0 ? " <= " : " >= ") + boundsHigh + "_" + toStr(i) + "; " + loopVars[idx].name;
            if (loopVars[idx].constStep == "1")
                kernelBody += "++";
            else if (loopVars[idx].constStep == "-1")
                kernelBody += "--";
            else if (!loopVars[idx].constStep.empty())
                kernelBody += " += (" + loopVars[idx].constStep + ")";
            else
                kernelBody += " += " + loopSteps + "_" + toStr(i);
            kernelBody += ")\n";
        }
        indent += indentStep;
    }
    if (isSequentialPart)
        indent += indentStep;
    std::string loopBody = convertToString(parLoopBodyStmt);
    if (isa<CompoundStmt>(parLoopBodyStmt))
        indent = subtractIndent(indent);
    else
        kernelBody += subtractIndent(indent) + "{\n";
    int lastPos = 0;
    while (loopBody.find('\n', lastPos) != std::string::npos) {
        int nlPos = loopBody.find('\n', lastPos);
        kernelBody += indent + loopBody.substr(lastPos, nlPos - lastPos + 1);
        lastPos = nlPos + 1;
        if (lastPos >= (int)loopBody.size())
            break;
    }
    if (lastPos < (int)loopBody.size())
        kernelBody += indent + loopBody.substr(lastPos) + "\n";
    if (!isFull(parLoopBodyStmt)) {
        kernelBody[kernelBody.size() - 1] = ';';
        kernelBody += "\n";
    }
    if (!isa<CompoundStmt>(parLoopBodyStmt))
        kernelBody += subtractIndent(indent) + "}\n";
    if (!isOneThread)
        for (int i = loopRank - 1; i >= 1; i--)
            kernelBody += genIndent(i, fileCtx.useTabs()) + "}\n";
    indent = indentStep;
    if (!isSequentialPart) {
        if (curPragma->reductions.size() > 0) {
            kernelBody += "\n";
            kernelBody += indent + "/* Write reduction values to global memory */\n";
        }
        for (int i = 0; i < (int)curPragma->reductions.size(); i++) {
            // TODO: Add support for reduction arrays
            ClauseReduction *red = &curPragma->reductions[i];
            std::string index;
            if (!isOneThread)
                index = "threadIdx.x + blockDim.x * (threadIdx.y + blockDim.y * (threadIdx.z + blockDim.z * (blockIdx.x + " + blockOffset +")))";
            else
                index = "0";
            kernelBody += indent + redGrid[red->arrayName] + "[" + index + "] = " + red->arrayName + ";\n";
            if (red->isLoc())
                kernelBody += indent + redGrid[red->locName] + "[" + index + "] = " + red->locName + ";\n";
        }
    }
    kernelText += handlerTemplateDecl;
    kernelText += "__global__ void " + kernelName + "(" + kernelFormalParams + ") {\n";
    kernelText += kernelBody;
    kernelText += "}\n";
}

void ConverterASTVisitor::genCudaHandler(std::string handlerName, const std::vector<VarDecl *> &outerParams, const std::vector<LoopVarDesc> &loopVars,
        std::string handlerTemplateDecl, std::string handlerTemplateSpec, std::string &handlerFormalParams, std::string &handlerBody, std::string &kernelText,
        std::string &cudaInfoText)
{
    std::string indent = indentStep;
    PragmaParallel *curPragma = curParallelPragma;
    bool isSequentialPart = curPragma == 0;
    int loopRank = (isSequentialPart ? 0 : curPragma->rank);
    bool isAcross = (isSequentialPart ? false : curPragma->acrosses.size() > 0);
    bool isOneThread = isAcross || opts.oneThread;
    bool autoTfm = opts.autoTfm;
    bool prepareDiag = isAcross && autoTfm && (curPragma->acrosses.size() > 1 || curPragma->acrosses[0].getDepCount() > 1);
    // TODO: Handle ACROSS not only in oneThread mode
    assert(!isAcross || isOneThread); // is consequent of upper part

    std::vector<KernelDesc> kernelsAvailable;
    kernelsAvailable.push_back(KernelDesc(handlerName, "int"));
    kernelsAvailable.push_back(KernelDesc(handlerName, "long long"));
    for (int i = 0; i < (int)kernelsAvailable.size(); i++)
        genCudaKernel(kernelsAvailable[i], outerParams, loopVars, handlerTemplateDecl, kernelText);

    std::set<std::string> prohibitedNames;
    CollectNamesVisitor collectNamesVisitor(comp);
    collectNamesVisitor.TraverseStmt(parLoopBodyStmt);
    prohibitedNames = collectNamesVisitor.getNames();
    for (int i = 0; i < (int)outerParams.size(); i++)
        prohibitedNames.insert(outerParams[i]->getName().str());
    for (std::set<VarDecl *>::iterator it = outerPrivates.begin(); it != outerPrivates.end(); it++)
        prohibitedNames.insert((*it)->getName().str());
    for (std::set<VarDecl *>::iterator it = reductions.begin(); it != reductions.end(); it++)
        prohibitedNames.insert((*it)->getName().str());

    std::string kernelFactParams;
    std::string tmpVar = getUniqueName("tmpVar", &prohibitedNames);
    std::string device_num = getUniqueName("device_num", &prohibitedNames);
    std::string loop_ref = getUniqueName("loop_ref", &prohibitedNames);
    std::string pLoopRef = getUniqueName("pLoopRef", &prohibitedNames);
    std::string boundsLow = getUniqueName("boundsLow", &prohibitedNames);
    std::string boundsHigh = getUniqueName("boundsHigh", &prohibitedNames);
    std::string loopSteps = getUniqueName("loopSteps", &prohibitedNames);
    std::string blocksS = getUniqueName("blocksS", &prohibitedNames);
    std::string blocks = getUniqueName("blocks", &prohibitedNames);
    std::string threads = getUniqueName("threads", &prohibitedNames);
    std::string stream = getUniqueName("stream", &prohibitedNames);
    std::string restBlocks = getUniqueName("restBlocks", &prohibitedNames);
    std::string kernelIndexT = getUniqueName("kernelIndexT", &prohibitedNames);
    std::map<std::string, std::string> dvmHeaders;
    std::map<std::string, std::string> dvmDevHeaders;
    std::map<std::string, std::string> scalarPtrs;
    std::map<std::string, std::vector<std::string> > dvmCoefs;
    std::map<std::string, std::string> redGrid;
    for (int i = 0; i < (int)outerParams.size(); i++) {
        VarState *varState = &varStates[outerParams[i]];
        if (varState->isArray) {
            dvmHeaders[varState->name] = getUniqueName(varState->name + "_hdr", &prohibitedNames);
            dvmDevHeaders[varState->name] = getUniqueName(varState->name + "_devHdr", &prohibitedNames);
            dvmCoefs[varState->name].clear();
            for (int j = 0; j < varState->headerArraySize; j++)
                dvmCoefs[varState->name].push_back(getUniqueName(varState->name + "_hdr" + toStr(j), &prohibitedNames));
        } else {
            scalarPtrs[varState->name] = getUniqueName(varState->name + "_ptr", &prohibitedNames);
        }
    }
    for (std::set<VarDecl *>::iterator it = reductions.begin(); it != reductions.end(); it++) {
        VarState *varState = &varStates[*it];
        redGrid[varState->name] = getUniqueName(varState->name + "_grid", &prohibitedNames);
    }

    handlerBody += indent + "DvmType " + tmpVar + ";\n";

    handlerFormalParams += "DvmType *" + pLoopRef;
    handlerBody += indent + "/* Loop reference and device number */\n";
    handlerBody += indent + "DvmType " + loop_ref + " = *" + pLoopRef + ";\n";
    handlerBody += indent + "DvmType " + device_num + " = dvmh_loop_get_device_num_C(" + loop_ref + ");\n";
    handlerBody += indent + "/* Parameters */\n";

    for (int i = 0; i < (int)outerParams.size(); i++) {
        VarState *varState = &varStates[outerParams[i]];
        int rank = varState->rank;
        std::string refName = varState->name;
        if (varState->isArray) {
            std::string hdrName = dvmHeaders[refName];
            std::string devHdrName = dvmDevHeaders[refName];

            handlerFormalParams += ", DvmType " + hdrName + "[]";
            if (autoTfm)
                handlerBody += indent + "dvmh_loop_autotransform_C(" + loop_ref + ", " + hdrName + ");\n";
            handlerBody += indent + varState->baseTypeStr + " *" + refName;
            handlerBody += " = (" + varState->baseTypeStr + " *)dvmh_get_natural_base_C(" + device_num + ", " + hdrName + ");\n";
            handlerBody += indent + "DvmType " + devHdrName + "[" + toStr(varState->headerArraySize + (prepareDiag ? 7 : 0)) + "];\n";
            handlerBody += indent + tmpVar + " = dvmh_fill_header_C(" + device_num + ", " + refName + ", " + hdrName + ", " + devHdrName + ", " +
                    (prepareDiag ? devHdrName + " + " + toStr(varState->headerArraySize) : "0") + ");\n";
            if (!autoTfm)
                handlerBody += indent + "assert(" + tmpVar + " == 0);\n";
            else if (!prepareDiag)
                handlerBody += indent + "assert(" + tmpVar + " == 0 || " + tmpVar + " == 1);\n";
            if (prepareDiag) {
                handlerBody += indent + "if (" + tmpVar + " == 2) " + tmpVar + " += 4 * " + devHdrName + "[" + toStr(varState->headerArraySize + 6) + "];\n";
                handlerBody += indent + devHdrName + "[" + toStr(varState->headerArraySize + 6) + "] = " + tmpVar + ";\n";
            }

            kernelFactParams += ", " + refName;
            if (rank > 1) {
                for (int j = 1; j <= rank; j++) {
                    int hdrIdx = j;
                    if (j < rank || autoTfm)
                        kernelFactParams += ", " + devHdrName + "[" + toStr(hdrIdx) + "]";
                }
                if (prepareDiag) {
                    int diagIdxs[] = {6, 0, 3, 2, 5, 1, 4};
                    for (int j = 0; j < 7; j++)
                        kernelFactParams += ", " + devHdrName + "[" + toStr(varState->headerArraySize + diagIdxs[j]) + "]";
                }
            }
        } else {
            // TODO: Add 'use' case for variables
            handlerFormalParams += ", " + varState->baseTypeStr + " *" + scalarPtrs[refName];
            handlerBody += indent + varState->baseTypeStr + " *" + refName;
            handlerBody += " = (" + varState->baseTypeStr + " *)dvmh_get_device_addr_C(" + device_num + ", " + scalarPtrs[refName] + ");\n";

            kernelFactParams += ", " + refName;
        }
    }
    // Handler heading is done
    if (!isSequentialPart) {
        handlerBody += indent + "/* Supplementary variables for loop handling */\n";
        handlerBody += indent + "DvmType " + boundsLow + "[" + toStr(loopRank) + "], " + boundsHigh + "[" + toStr(loopRank) + "], " + loopSteps + "["
                + toStr(loopRank) + "];\n";
        if (!isOneThread)
            handlerBody += indent + "DvmType " + blocksS + "[" + toStr(loopRank) + "];\n";
    }

    handlerBody += indent + "DvmType " + restBlocks + ";\n";
    handlerBody += indent + "dim3 " + blocks + "(1, 1, 1), " + threads + (isOneThread ? "(1, 1, 1)" : "(0, 0, 0)") + ";\n";
    handlerBody += indent + "cudaStream_t " + stream + ";\n";
    handlerBody += "\n";
    handlerBody += indent + "/* Choose index type for CUDA kernel */\n";
    handlerBody += indent + "int " + kernelIndexT + " = dvmh_loop_guess_index_type_C(" + loop_ref + ");\n";
    handlerBody += indent + "if (" + kernelIndexT + " == rt_LONG) " + kernelIndexT + " = (sizeof(long) <= sizeof(int) ? rt_INT : rt_LLONG);\n";
    handlerBody += indent + "assert(" + kernelIndexT + " == rt_INT || " + kernelIndexT + " == rt_LLONG);\n";
    handlerBody += "\n";
    handlerBody += indent + "/* Get CUDA configuration parameters */\n";
    if (!isSequentialPart) {
        for (int i = 0; i < (int)kernelsAvailable.size(); i++) {
            std::string regsVar = kernelsAvailable[i].regsVar;
            std::string rtIndexT = kernelsAvailable[i].rtIndexT;
            cudaInfoText += "#ifdef " + toUpper(regsVar) + "\n";
            cudaInfoText += indent + "DvmType " + regsVar + " = " + toUpper(regsVar) + ";\n";
            cudaInfoText += "#else\n";
            cudaInfoText += indent + "DvmType " + regsVar + " = 0;\n";
            cudaInfoText += "#endif\n";
            handlerBody += indent + "extern DvmType " + regsVar + ";\n";
            handlerBody += indent + "if (" + kernelIndexT + " == " + rtIndexT + ") dvmh_loop_cuda_get_config_C(" + loop_ref + ", 0, " + regsVar + ", &" +
                    threads + ", &" + stream + ", 0);\n";
        }
    } else {
        handlerBody += indent + "dvmh_loop_cuda_get_config_C(" + loop_ref + ", 0, 0, &" + threads + ", &" + stream + ", 0);\n";
    }
    if (isOneThread)
        handlerBody += indent + threads + " = dim3(1, 1, 1);\n";
    handlerBody += "\n";
    if (!isSequentialPart) {
        handlerBody += indent + "/* Calculate computation distribution parameters */\n";
        handlerBody += indent + "dvmh_loop_fill_bounds_C(" + loop_ref + ", " + boundsLow + ", " + boundsHigh + ", " + loopSteps + ");\n";
        if (!isOneThread) {
            for (int i = loopRank; i >= 1; i--) {
                int idx = i - 1;
                std::string threadsCoord;
                if (i == loopRank)
                    threadsCoord = threads + ".x";
                else if (i == loopRank - 1)
                    threadsCoord = threads + ".y";
                else if (i == loopRank - 2)
                    threadsCoord = threads + ".z";
                else
                    threadsCoord = "1";
                handlerBody += indent + blocksS + "[" + toStr(idx) + "] = ";
                if (i < loopRank)
                    handlerBody += blocksS + "[" + toStr(idx + 1) + "] * (";
                handlerBody += "((" + boundsHigh + "[" + toStr(idx) + "] - " + boundsLow + "[" + toStr(idx) + "]" + ") / " + loopSteps + "[" + toStr(idx) +
                        "] + 1 + (" + threadsCoord + " - 1)) / " + threadsCoord;
                if (i < loopRank)
                    handlerBody += ")";
                handlerBody += ";\n";
            }
        }
        for (int i = 0; i < loopRank; i++) {
            kernelFactParams += ", " + boundsLow + "[" + toStr(i) + "], " + boundsHigh + "[" + toStr(i) + "]";
            if (loopVars[i].constStep.empty())
                kernelFactParams += ", " + loopSteps + "[" + toStr(i) + "]";
            if (!isOneThread && i > 0)
                kernelFactParams += ", " + blocksS + "[" + toStr(i) + "]";
        }
        handlerBody += "\n";
    }
    if (!isSequentialPart) {
        if (curPragma->reductions.size() > 0)
            handlerBody += indent + "/* Reductions-related stuff */\n";
        for (int i = 0; i < (int)curPragma->reductions.size(); i++) {
            // TODO: Add support for reduction arrays
            ClauseReduction *red = &curPragma->reductions[i];
            std::string epsGrid = redGrid[red->arrayName];
            VarDecl *vd = seekVarDecl(red->arrayName);
            checkIntErrN(vd && reductions.find(vd) != reductions.end(), 97, red->arrayName.c_str(), curPragma->fileName.c_str(), curPragma->line);
            VarState *varState = &varStates[vd];
            checkDirErrN(!varState->isArray, 4417, varState->name.c_str());
            handlerBody += indent + varState->baseTypeStr + " " + red->arrayName + ";\n";
            handlerBody += indent + varState->baseTypeStr + " *" + epsGrid + ";\n";

            kernelFactParams += ", " + red->arrayName;
            kernelFactParams += ", " + epsGrid;
            if (red->isLoc()) {
                std::string locGrid = redGrid[red->locName];
                VarDecl *lvd = seekVarDecl(red->locName);
                checkIntErrN(lvd && reductions.find(lvd) != reductions.end(), 97, red->locName.c_str(), curPragma->fileName.c_str(), curPragma->line);
                VarState *locVarState = &varStates[lvd];
                checkDirErrN(!locVarState->isArray, 4417, locVarState->name.c_str());
                handlerBody += indent + locVarState->baseTypeStr + " " + red->locName + ";\n";
                handlerBody += indent + locVarState->baseTypeStr + " *" + locGrid + ";\n";

                kernelFactParams += ", " + red->locName;
                kernelFactParams += ", " + locGrid;

                handlerBody += indent + "dvmh_loop_cuda_register_red_C(" + loop_ref + ", " + toStr(i + 1) + ", (void **)&" + epsGrid + ", (void **)&" +
                        locGrid + ");\n";
                handlerBody += indent + "dvmh_loop_red_init_C(" + loop_ref + ", " + toStr(i + 1) + ", " + (varState->isArray ? "" : "&") + red->arrayName +
                        ", " + (locVarState->isArray ? "" : "&") + red->locName + ");\n";
            } else {
                handlerBody += indent + "dvmh_loop_cuda_register_red_C(" + loop_ref + ", " + toStr(i + 1) + ", (void **)&" + epsGrid + ", 0);\n";
                handlerBody += indent + "dvmh_loop_red_init_C(" + loop_ref + ", " + toStr(i + 1) + ", " + (varState->isArray ? "" : "&") + red->arrayName +
                        ", 0);\n";
            }
            if (!isOneThread)
                handlerBody += indent + "dvmh_loop_cuda_red_prepare_C(" + loop_ref + ", " + toStr(i + 1) + ", " + (blocksS + "[0] * " + threads + ".x * "
                        + threads + ".y * " + threads + ".z") + ", 0);\n";
            else
                handlerBody += indent + "dvmh_loop_cuda_red_prepare_C(" + loop_ref + ", " + toStr(i + 1) + ", 1, 0);\n";
        }
        if (curPragma->reductions.size() > 0)
            handlerBody += "\n";
    }
    if (!isSequentialPart && !isOneThread)
        kernelFactParams += ", " + blocksS + "[0] - " + restBlocks;
    trimList(kernelFactParams);
    // kernelFactParams is done
    handlerBody += indent + "/* GPU execution */\n";
    if (!isSequentialPart && !isOneThread)
        handlerBody += indent + restBlocks + " = " + blocksS + "[0];\n";
    else
        handlerBody += indent + restBlocks + " = 1;\n";
    handlerBody += indent + "while (" + restBlocks + " > 0) {\n";
    indent += indentStep;
    handlerBody += indent + blocks + ".x = (" + restBlocks + " <= 65535 ? " + restBlocks + " : (" + restBlocks + " <= 65535 * 2 ? " + restBlocks +
            " / 2 : 65535));\n";
    for (int i = 0; i < (int)kernelsAvailable.size(); i++) {
        std::string kernelName = kernelsAvailable[i].kernelName;
        std::string rtIndexT = kernelsAvailable[i].rtIndexT;
        handlerBody += indent + "if (" + kernelIndexT + " == " + rtIndexT + ") " + kernelName + handlerTemplateSpec + "<<<" + blocks + ", " + threads + ", 0, "
                + stream + ">>>(" + kernelFactParams + ");\n";
    }
    handlerBody += indent + restBlocks + " -= " + blocks + ".x;\n";
    indent = subtractIndent(indent);
    handlerBody += indent + "}\n";
    if (!isSequentialPart) {
        if (curPragma->reductions.size() > 0)
            handlerBody += "\n";
        for (int i = 0; i < (int)curPragma->reductions.size(); i++)
            handlerBody += indent + "dvmh_loop_cuda_red_finish_C(" + loop_ref + ", " + toStr(i + 1) + ");\n";
    }
}

bool ConverterASTVisitor::TraverseStmt(Stmt *s) {
    bool res = base::TraverseStmt(s);
    if (!s)
        return res; // For some reason there could be NULL

    if (isa<IfStmt>(s) || isa<ForStmt>(s) || isa<WhileStmt>(s) || isa<CXXCatchStmt>(s))
    {
        checkIntervalBalance(srcMgr.getFileLoc(s->getEndLoc()));
        leaveDeclContext();
    }

    if (!inParLoop && (opts.perfDbgLvl == 4 || opts.perfDbgLvl == 1 || opts.perfDbgLvl == 3))
        if (isa<ForStmt>(s) || isa<WhileStmt>(s) || isa<DoStmt>(s)) {
            SourceLocation fileLoc = srcMgr.getFileLoc(s->getBeginLoc());
            std::string fileName = srcMgr.getFilename(fileLoc).str();
            FileID fileID = srcMgr.getFileID(fileLoc);
            int line = srcMgr.getLineNumber(fileID, srcMgr.getFileOffset(fileLoc));

            SourceLocation lastLoc = escapeMacroEnd(s->getSourceRange().getEnd());
            SourceLocation endLoc;

            Token tok;
            Lexer::getRawToken(lastLoc, tok, rewr.getSourceMgr(), rewr.getLangOpts());
            if (tok.is(tok::r_brace) || tok.is(tok::semi))
                endLoc = Lexer::getLocForEndOfToken(lastLoc, 0, rewr.getSourceMgr(), rewr.getLangOpts());
            else
                endLoc = Lexer::findLocationAfterToken(lastLoc, tok::semi, rewr.getSourceMgr(), rewr.getLangOpts(), false);

            int endLine = srcMgr.getPresumedLineNumber(endLoc, 0);
            int isSeqLoopEnclosingParLoop = hasParallelLoopInRange(fileID, line, endLine);

            if (isSeqLoopEnclosingParLoop || opts.perfDbgLvl == 4) {
                std::string startInsert, endInsert;
                std::string indent = extractIndent(srcMgr.getCharacterData(srcMgr.translateLineCol(fileID, line, 1)));

                bool needBoundaries = !(findUpwards<CompoundStmt>(s, 1));
                if (needBoundaries)
                    startInsert += "do {\n";

                startInsert += indent + genDvmLine(s->getBeginLoc()) + '\n';
                startInsert += indent + "dvmh_seq_interval_start_C();\n";
                startInsert += indent;
                rewr.InsertTextBefore(s->getBeginLoc(), startInsert);

                endInsert += "\n" + indent + genDvmLine(endLoc) + '\n';
                endInsert += indent + "dvmh_sp_interval_end_();\n";

                if (needBoundaries)
                    endInsert += indent + "} while (0);\n";

                rewr.InsertTextAfter(endLoc, endInsert);
            }
        }
    if (inParLoop && parLoopStmt == s) {
        // Finish parallel loop
        if (curParallelPragma)
            checkIntErrN(isa<ForStmt>(s), 93);
        SourceLocation fileLoc = srcMgr.getFileLoc(s->getBeginLoc());
        std::string fileName = srcMgr.getFilename(fileLoc).str();
        FileID fileID = srcMgr.getFileID(fileLoc);
        int line = srcMgr.getLineNumber(fileID, srcMgr.getFileOffset(fileLoc));
        PragmaParallel *curPragma = curParallelPragma;
        int loopRank = (curPragma ? curPragma->rank : 0);
        bool isSequentialPart = loopRank == 0;
        checkIntErrN((!curPragma && loopRank == 0) || (curPragma && loopRank > 0), 98);
        if (!isSequentialPart)
            cdvmhLog(TRACE, curPragma->fileName, curPragma->line, "Leaving parallel loop");
        else
            cdvmhLog(TRACE, fileName, line, "Leaving sequential part of region");
        std::string toInsert;
        std::string indent;
        checkIntErrN(inRegion || curPragma, 93);
        bool needBoundaries = !inRegion;
        if (!inRegion) {
            std::set<std::string> prohibitedNames;
            CollectNamesVisitor collectNamesVisitor(comp);
            collectNamesVisitor.TraverseStmt(s);
            prohibitedNames = collectNamesVisitor.getNames();
            curLoop = getUniqueName("cur_loop", &prohibitedNames, &fileCtx.seenMacroNames);
        }
        if (inRegion)
            indent = regionInnerIndent;
        else
            indent = extractIndent(srcMgr.getCharacterData(srcMgr.translateLineCol(fileID, curPragma->srcLine, 1)));
        if (needBoundaries) {
            toInsert += indent + "do {\n";
            indent += indentStep;
        }
        if (!isSequentialPart)
            toInsert += indent + genDvmLine(curPragma) + "\n";
        else
            toInsert += genDvmLine(s->getBeginLoc()) + "\n";

        if (!inRegion && opts.perfDbgLvl != 0 && opts.perfDbgLvl != 2) {
            toInsert += indent + "dvmh_par_interval_start_C();\n";
        }

        if (!isSequentialPart) {
            for (int i = 0; i < (int)curPragma->shadowRenews.size(); i++) {
                ClauseShadowRenew *shad = &curPragma->shadowRenews[i];
                VarDecl *vd = seekVarDecl(shad->arrayName);
                checkDirErrN(vd, 301, shad->arrayName.c_str());
                checkIntErrN(varStates.find(vd) != varStates.end(), 92, vd->getNameAsString().c_str());
                VarState *varState = &varStates[vd];
                checkDirErrN(varState->isDvmArray, 346, shad->arrayName.c_str());
                // First, the common block part (or unspecified, which could also be indirect)
                toInsert += indent + "dvmh_shadow_renew_C(" + shad->arrayName + ", " + (shad->cornerFlag ? "1" : "0");
                if (shad->rank > 0) {
                    checkIntErrN((int)shad->shadows.size() == shad->rank, 99);
                    checkDirErrN(shad->rank == varState->rank, 304, shad->arrayName.c_str());
                    toInsert += ", " + toStr(shad->rank);
                    for (int j = 0; j < shad->rank; j++) {
                        if (!shad->shadows[j].isIndirect) {
                            checkNonDvmExpr(shad->shadows[j].lower, curPragma);
                            checkNonDvmExpr(shad->shadows[j].upper, curPragma);
                            toInsert += ", " + fileCtx.dvm0c(shad->shadows[j].lower.strExpr) + ", " + fileCtx.dvm0c(shad->shadows[j].upper.strExpr);
                        } else {
                            toInsert += ", " + fileCtx.dvm0c(0) + ", " + fileCtx.dvm0c(0);
                        }
                    }
                } else {
                    toInsert += ", 0";
                }
                toInsert += ");\n";
                // Second, the indirect part (if present)
                if (shad->isIndirect) {
                    assert(!shad->cornerFlag);
                    assert(shad->rank > 0);
                    for (int j = 0; j < shad->rank; j++) {
                        const AxisShadow &shadow = shad->shadows[j];
                        if (shadow.isIndirect) {
                            for (int k = 0; k < (int)shadow.names.size(); k++) {
                                toInsert += indent + "dvmh_indirect_shadow_renew_C(" + shad->arrayName + ", " + toStr(j + 1) + ", \"" +
                                        escapeStr(shadow.names[k]) + "\");\n";
                            }
                        }
                    }
                }
            }
        }
        for (std::set<VarDecl *>::iterator it = varsToGetActual.begin(); it != varsToGetActual.end(); it++) {
            VarDecl *vd = *it;
            checkIntErrN(varStates.find(vd) != varStates.end(), 92, vd->getNameAsString().c_str());
            VarState *varState = &varStates[vd];
            if (!varState->isDvmArray)
                toInsert += indent + "dvmh_get_actual_variable2_((void *)" + (varState->isArray ? "" : "&") + varState->name + ");\n";
            else
                toInsert += indent + "dvmh_get_actual_array2_(" + varState->name + ");\n";
        }
        if (!isSequentialPart && curPragma->mappedFlag) {
            VarDecl *vd = seekVarDecl(curPragma->mapRule.templ);
            checkDirErrN(vd, 301, curPragma->mapRule.templ.c_str());
            checkIntErrN(varStates.find(vd) != varStates.end(), 92, vd->getNameAsString().c_str());
            VarState *varState = &varStates[vd];
            checkDirErrN(varState->isDvmArray || varState->isTemplate, 347);
            needToRegister[vd] |= 0;
        }
        std::vector<VarDecl *> outerParams;
        {
            std::map<std::string, VarDecl *> tm;
            for (std::set<VarDecl *>::iterator it = needsParams.begin(); it != needsParams.end(); it++)
                tm[varStates[*it].name] = *it;
            for (std::map<std::string, VarDecl *>::iterator it = tm.begin(); it != tm.end(); it++)
                outerParams.push_back(it->second);
        }
        checkIntErrN(outerParams.size() == needsParams.size(), 910);
        if (!inRegion) {
            // Enter implicit data region
            for (int i = 0; i < (int)outerParams.size(); i++) {
                VarState *varState = &varStates[outerParams[i]];
                if (varState->isRegular && varState->isArray) {
                    toInsert += indent + "dvmh_data_enter_C((const void *)" + varState->name + ", ";
                    if (varState->isIncomplete)
                        toInsert += "0";
                    else
                        toInsert += "sizeof(" + varState->name + ")";
                    toInsert += ");\n";
                }
            }
        }
        if (!srcMgr.isWrittenInMainFile(fileLoc))
            fileCtx.addToForceExpansion(fileID.getHashValue());
        if (opts.extraComments)
            toInsert += indent + "/* Loop's mandatory characteristics */\n";
        toInsert += indent + (inRegion ? "" : "DvmType ") + curLoop + " = ";
        toInsert += (opts.dvmDebugLvl > 0) ? "dvmh_dbg_loop_create_C(" : "dvmh_loop_create_C(";
        toInsert += curRegion + ", " + toStr(loopRank);
        std::vector<LoopVarDesc> loopVars(loopRank);
        Stmt *curSt = s;
        std::string lowBoundsParameters;
        std::string highBoundsParameters;
        std::string loopStepsParameters;

        for (int i = 0; i < loopRank; i++) {
            if (i > 0 && isa<CompoundStmt>(curSt) && cast<CompoundStmt>(curSt)->size() == 1)
                curSt = *cast<CompoundStmt>(curSt)->body_begin();
            assert(isa<ForStmt>(curSt)); // duplicate
            ForStmt *curFor = cast<ForStmt>(curSt);
            std::string loopVar;
            MyExpr initVal;
            VarDecl *vd;
            if (isa<DeclStmt>(curFor->getInit())) {
                DeclStmt *ds = cast<DeclStmt>(curFor->getInit());
                assert(ds); // duplicate
                assert(ds->getDeclGroup().isSingleDecl()); // duplicate
                vd = cast<VarDecl>(ds->getSingleDecl());
                assert(vd); // duplicate
                checkDirErrN(vd->hasInit(), 4416);
                // TODO: Fill somehow info on references in this expression
                initVal.strExpr = convertToString(vd->getInit());
            } else {
                BinaryOperator *initExpr = cast<BinaryOperator>(curFor->getInit());
                assert(initExpr); // duplicate
                assert(initExpr->isAssignmentOp()); // duplicate
                assert(initExpr->getLHS()->isLValue()); // duplicate
                assert(isa<DeclRefExpr>(initExpr->getLHS())); // duplicate
                vd = cast<VarDecl>(cast<DeclRefExpr>(initExpr->getLHS())->getDecl());
                assert(vd); // duplicate
                // TODO: Fill somehow info on references in this expression
                initVal.strExpr = convertToString(initExpr->getRHS());
            }
            loopVar = vd->getName().str();
            loopVars[i].name = loopVar;
            checkIntErrN(varStates.find(vd) != varStates.end(), 92, vd->getNameAsString().c_str());
            VarState *varState = &varStates[vd];
            checkDirErrN(!varState->isArray, 348, varState->name.c_str());
            loopVars[i].baseTypeStr = varState->baseTypeStr;
            BinaryOperator *condExpr = cast<BinaryOperator>(curFor->getCond());
            checkDirErrN(condExpr->isRelationalOp(), 4418);
            checkDirErrN(loopVar == convertToString(condExpr->getLHS()), 4419);
            int rightBndAdd = 0;
            if (condExpr->getOpcode() == BO_LT)
                rightBndAdd = -1;
            else if (condExpr->getOpcode() == BO_GT)
                rightBndAdd = 1;
            else
                checkDirErrN(condExpr->getOpcode() == BO_LE || condExpr->getOpcode() == BO_GE, 4420);
            loopVars[i].stepSign = ((condExpr->getOpcode() == BO_LT || condExpr->getOpcode() == BO_LE) ? 1 : -1);
            MyExpr endVal;
            // TODO: Fill somehow info on references in this expression
            endVal.strExpr = convertToString(condExpr->getRHS());
            if (rightBndAdd < 0) {
                endVal.prepend("(");
                endVal.append(") - 1");
            } else if (rightBndAdd > 0) {
                endVal.prepend("(");
                endVal.append(") + 1");
            }
            MyExpr stepVal;
            bool constStep = false;
            if (isa<UnaryOperator>(curFor->getInc())) {
                UnaryOperator *incrExpr = cast<UnaryOperator>(curFor->getInc());
                checkDirErrN(loopVar == convertToString(incrExpr->getSubExpr()), 4422);
                if (incrExpr->isIncrementOp())
                    stepVal.strExpr = "1";
                else if (incrExpr->isDecrementOp())
                    stepVal.strExpr = "-1";
                else
                    checkDirErrN(false, 4421);
                constStep = true;
            } else {
                BinaryOperator *incrExpr = llvm::dyn_cast<BinaryOperator>(curFor->getInc());
                checkDirErrN(incrExpr, 4421);
                checkDirErrN(loopVar == convertToString(incrExpr->getLHS()), 4422);
                llvm::APSInt stepConstVal;
                clang::Expr::EvalResult stepConstValResult;
                constStep = incrExpr->getRHS()->EvaluateAsInt(stepConstValResult, comp.getASTContext());
                stepConstVal = stepConstValResult.Val.getInt();
                bool negate = incrExpr->getOpcode() == BO_SubAssign;
                checkDirErrN(incrExpr->getOpcode() == BO_AddAssign || incrExpr->getOpcode() == BO_SubAssign, 4421);
                if (constStep) {
                    stepVal.strExpr = toStr((negate ? -1 : 1) * stepConstVal.getSExtValue());
                } else {
                    // TODO: Fill somehow info on references in this expression
                    stepVal.strExpr = convertToString(incrExpr->getRHS());
                    if (negate) {
                        stepVal.prepend("-(");
                        stepVal.append(")");
                    }
                }
            }
            if (constStep)
                loopVars[i].constStep = stepVal.strExpr;
            else
                loopVars[i].constStep = "";
            checkNonDvmExpr(initVal, curPragma);
            checkNonDvmExpr(endVal, curPragma);
            checkNonDvmExpr(stepVal, curPragma);
            toInsert += ", " + fileCtx.dvm0c(initVal.strExpr) + ", " + fileCtx.dvm0c(endVal.strExpr) + ", " + fileCtx.dvm0c(stepVal.strExpr);

            lowBoundsParameters += ", " + fileCtx.dvm0c(initVal.strExpr);
            highBoundsParameters += ", " + fileCtx.dvm0c(endVal.strExpr);
            loopStepsParameters += ", " + fileCtx.dvm0c(stepVal.strExpr);

            curSt = curFor->getBody();
        }
        toInsert += ");\n";
        if (!isSequentialPart && curPragma->mappedFlag)
            toInsert += indent + "dvmh_loop_map_C(" + curLoop + ", " + genAlignParams(curPragma, &curPragma->mapRule) + ");\n";
        if (!isSequentialPart) {
            bool hasOptionalClauses = (curPragma->cudaBlock[0].strExpr != "0" && curPragma->cudaBlock[1].strExpr != "0" &&
                    curPragma->cudaBlock[2].strExpr != "0") || curPragma->reductions.size() > 0 || curPragma->acrosses.size() > 0 ||
                    curPragma->rmas.size() > 0 || curPragma->ties.size() > 0;
            if (hasOptionalClauses && opts.extraComments)
                toInsert += indent + "/* Optional clauses */\n";
            for (int i = 0; i < (int)curPragma->acrosses.size(); i++) {
                ClauseAcross *acr = &curPragma->acrosses[i];
                VarDecl *vd = seekVarDecl(acr->arrayName);
                checkDirErrN(vd, 301, acr->arrayName.c_str());
                checkIntErrN(varStates.find(vd) != varStates.end(), 92, vd->getNameAsString().c_str());
                VarState *varState = &varStates[vd];
                checkDirErrN(varState->isDvmArray || varState->isRegular, 349, varState->name.c_str());
                checkDirErrN(varState->rank == acr->rank, 304, acr->arrayName.c_str());
                toInsert += indent + "dvmh_loop_across_C(" + curLoop + ", " + (acr->isOut ? "1" : "0") + ", " + varState->genHeaderRef(fileCtx);
                toInsert += ", " + toStr(varState->rank);
                for (int j = 0; j < varState->rank; j++) {
                    checkNonDvmExpr(acr->widths[j].first, curPragma);
                    checkNonDvmExpr(acr->widths[j].second, curPragma);
                    toInsert += ", " + fileCtx.dvm0c(acr->widths[j].first.strExpr) + ", " + fileCtx.dvm0c(acr->widths[j].second.strExpr);
                }
                toInsert += ");\n";
            }
            if (curPragma->acrosses.size() > 0 && curPragma->stage.strExpr != "0") {
                checkNonDvmExpr(curPragma->stage, curPragma);
                toInsert += indent + "dvmh_loop_set_stage_C(" + curLoop + ", " + curPragma->stage.strExpr + ");\n";
            }
            if (opts.dvmDebugLvl > 0 && !curPragma->reductions.empty()) {
                toInsert += indent + "dvmh_dbg_loop_red_group_create_C(" + curLoop + ");\n";
            }
            for (int i = 0; i < (int)curPragma->reductions.size(); i++) {
                ClauseReduction *red = &curPragma->reductions[i];
                VarDecl *vd = seekVarDecl(red->arrayName);
                checkDirErrN(vd, 301, red->arrayName.c_str());
                checkIntErrN(varStates.find(vd) != varStates.end(), 92, vd->getNameAsString().c_str());
                VarState *varState = &varStates[vd];
                toInsert += indent + "dvmh_loop_reduction_C(" + curLoop + ", " + red->redType + ", " + (varState->isArray ? "" : "&") + red->arrayName
                        + ", " + toRedType(varState->baseTypeStr) + ", " + toStr(varState->getTotalElemCount());
                if (red->isLoc()) {
                    VarDecl *vd = seekVarDecl(red->locName);
                    checkDirErrN(vd, 301, red->locName.c_str());
                    checkIntErrN(varStates.find(vd) != varStates.end(), 92, vd->getNameAsString().c_str());
                    VarState *varState = &varStates[vd];
                    checkNonDvmExpr(red->locSize, curPragma);
                    toInsert += std::string(", ") + (varState->isArray ? "" : "&") + red->locName +
                            ", " + "sizeof(" + varState->baseTypeStr + ") * (" + red->locSize.strExpr + ")";
                } else
                    toInsert += ", 0, 0";
                toInsert += ");\n";

                if (opts.dvmDebugLvl & (dlWriteVariables | dlReadVariables)) {
                     toInsert += indent + "dvmh_dbg_loop_global_red_init_C(" + curLoop + ", " + toStr(i + 1) + ", " +
                                    (varState->isArray ? "" : "&") + red->arrayName + ", \"" + red->arrayName + "\");\n";

                }
            }
            if (curPragma->cudaBlock[0].strExpr != "0" && curPragma->cudaBlock[1].strExpr != "0" && curPragma->cudaBlock[2].strExpr != "0") {
                checkNonDvmExpr(curPragma->cudaBlock[0], curPragma);
                checkNonDvmExpr(curPragma->cudaBlock[1], curPragma);
                checkNonDvmExpr(curPragma->cudaBlock[2], curPragma);
                toInsert += indent + "dvmh_loop_set_cuda_block_C(" + curLoop + ", " + curPragma->cudaBlock[0].strExpr + ", " + curPragma->cudaBlock[1].strExpr
                        + ", " + curPragma->cudaBlock[2].strExpr + ");\n";
            }
            for (int i = 0; i < (int)curPragma->rmas.size(); i++) {
                std::string origName = curPragma->rmas[i].arrayName;
                int rank = curPragma->rmas[i].rank;
                if (!curPragma->rmas[i].excluded) {
                    toInsert += indent + "dvmh_loop_remote_access_C(" + curLoop + ", " + genAlignParams(curPragma, origName, rank, curPragma->rmas[i].axisRules)
                            + ");\n";
                }
            }
            for (int i = 0; i < (int)curPragma->ties.size(); i++) {
                ClauseTie *tie = &curPragma->ties[i];
                VarDecl *vd = seekVarDecl(tie->arrayName);
                checkDirErrN(vd, 301, tie->arrayName.c_str());
                checkIntErrN(varStates.find(vd) != varStates.end(), 92, vd->getNameAsString().c_str());
                VarState *varState = &varStates[vd];
                checkDirErrN(varState->isDvmArray || varState->isRegular, 3443, varState->name.c_str());
                checkDirErrN(varState->rank == (int)tie->loopAxes.size(), 304, tie->arrayName.c_str());
                toInsert += indent + "dvmh_loop_array_correspondence_C(" + curLoop + ", " + varState->genHeaderRef(fileCtx);
                toInsert += ", " + toStr(varState->rank);
                for (int j = 0; j < varState->rank; j++) {
                    toInsert += ", " + fileCtx.dvm0c(tie->loopAxes[j]);
                }
                toInsert += ");\n";
            }
        }
        std::string handlerTemplateDecl;
        std::string handlerTemplateCall;
        std::set<std::string> handlerTemplateInstantiations;
        std::string handlerFactParams;
        std::set<std::string> templatedTypes;
        std::string handlerFormalParamsFwd = "DvmType *";
        for (int i = 0; i < (int)outerParams.size(); i++) {
            VarState *varState = &varStates[outerParams[i]];
            handlerFactParams += ", " + varState->genHeaderOrScalarRef(fileCtx, inRegion);
            if (varState->isArray)
                handlerFormalParamsFwd += ", DvmType []";
            else
                handlerFormalParamsFwd += ", " + varState->baseTypeStr + " *" + (varState->canBeRestrict ? " DVMH_RESTRICT" : "");
            if (varState->hasDependentBaseType)
                templatedTypes.insert(varState->baseTypeStr);
        }
        trimList(handlerFactParams);
        if (!templatedTypes.empty()) {
            handlerTemplateDecl = "template <";
            handlerTemplateCall = "<";
            std::string declList, callList;
            for (std::set<std::string>::iterator it = templatedTypes.begin(); it != templatedTypes.end(); it++) {
                declList += ", typename " + *it;
                callList += ", " + *it;
            }
            trimList(declList);
            trimList(callList);
            handlerTemplateDecl += declList + ">\n";
            handlerTemplateCall += callList + ">";
            for (std::set<std::map<std::string, std::string> >::iterator setIt = curInstantiations.begin(); setIt != curInstantiations.end(); setIt++) {
                std::string curInst;
                bool ok = true;
                for (std::set<std::string>::iterator it = templatedTypes.begin(); it != templatedTypes.end(); it++) {
                    std::string paramName = *it;
                    if (setIt->find(paramName) == setIt->end()) {
                        if (possibleTargets & PragmaRegion::DEVICE_TYPE_CUDA) {
                            cdvmhLog(WARNING, curPragma->fileName, curPragma->line, 461, MSG(461), paramName.c_str());
                            possibleTargets &= ~(PragmaRegion::DEVICE_TYPE_CUDA);
                        }
                        checkDirErrN(!opts.useBlank, 462, paramName.c_str());
                        ok = false;
                    } else {
                        curInst += ", " + setIt->at(paramName);
                    }
                }
                trimList(curInst);
                if (ok)
                    handlerTemplateInstantiations.insert("<" + curInst + ">");
            }
        }
        if (opts.extraComments)
            toInsert += indent + "/* Register handlers */\n";
        std::string shortName = toCIdent(fileCtx.getInputFile().shortName, true);
        if (!srcMgr.isWrittenInMainFile(fileLoc))
            shortName += "_" + toCIdent(getBaseName(srcMgr.getFileEntryForID(fileID)->getName().str()));
        std::string blankHandlerName = (isSequentialPart ? "sequence_" : "loop_") + shortName + "_" + toStr(line);
        blankHandlerName = getUniqueName(blankHandlerName, &fileCtx.seenGlobalNames, &fileCtx.seenMacroNames);
        fileCtx.seenGlobalNames.insert(blankHandlerName);
        if (true) {
            std::set<Decl *> shallow, deep;
            DeclUsageCollector usageCollector(shallow, deep);
            usageCollector.TraverseStmt(parLoopStmt);
            std::map<int, Decl *> localDecls;
            for (std::set<Decl *>::const_iterator it = usageCollector.getReferencedDeclsShallow().begin(); it != usageCollector.getReferencedDeclsShallow().end(); it++) {
                Decl *d = *it;
                bool isSystem = srcMgr.getFileCharacteristic(d->getLocation()) == SrcMgr::C_System || srcMgr.getFileCharacteristic(d->getLocation()) == SrcMgr::C_ExternCSystem;
                bool isGlobal = d->getDeclContext()->isFileContext();
                if (!isGlobal) {
                    int order = declOrder[d];
                    localDecls[order] = d;
                } else if (!isSystem) {
                    blankHandlerDeclsShallow.insert(d);
                }
            }
            for (std::set<Decl *>::const_iterator it = usageCollector.getReferencedDeclsDeep().begin(); it != usageCollector.getReferencedDeclsDeep().end(); it++) {
                Decl *d = *it;
                bool isSystem = srcMgr.getFileCharacteristic(d->getLocation()) == SrcMgr::C_System || srcMgr.getFileCharacteristic(d->getLocation()) == SrcMgr::C_ExternCSystem;
                bool isGlobal = d->getDeclContext()->isFileContext();
                if (isGlobal && !isSystem) {
                    blankHandlerDeclsDeep.insert(d);
                }
            }
            std::string handlerText;
            genBlankHandler(blankHandlerName, outerParams, loopVars, localDecls, handlerText);
            fileCtx.addBlankHandler(handlerText);

            for (std::set<Decl *>::const_iterator it = usageCollector.getReferencedDeclsDeep().begin(); it != usageCollector.getReferencedDeclsDeep().end(); it++) {
                Decl *d = *it;
                bool isSystem = srcMgr.getFileCharacteristic(d->getLocation()) == SrcMgr::C_System || srcMgr.getFileCharacteristic(d->getLocation()) == SrcMgr::C_ExternCSystem;
                bool isGlobal = d->getDeclContext()->isFileContext();
                bool isShallow = usageCollector.getReferencedDeclsShallow().find(d) != usageCollector.getReferencedDeclsShallow().end();
                cdvmh_log(DEBUG, "Found referenced from parloop on %s:%d decl (%s) (%s) (%s): %s", curPragma->fileName.c_str(), curPragma->line,
                        (isSystem ? "system" : "user"), (isGlobal ? "global" : "local"), (isShallow ? "shallow" : "deep"), declToStr(*it, false, false, false).c_str());
            }
        }
        if (possibleTargets & PragmaRegion::DEVICE_TYPE_HOST) {
            std::string handlerName = (isSequentialPart ? "sequence_" : "loop_") + shortName + "_" + toStr(line) + "_host";
            handlerName = getUniqueName(handlerName, &fileCtx.seenGlobalNames, &fileCtx.seenMacroNames);
            fileCtx.seenGlobalNames.insert(handlerName);
            fileCtx.addHandlerForwardDecl(handlerTemplateDecl + "void " + handlerName + "(" + handlerFormalParamsFwd + ");\n");
            bool doingOpenMP = opts.doOpenMP && !isSequentialPart && (inRegion || opts.paralOutside);
            if (curPragma && (curPragma->rank == 0 || (curPragma->rank == 1 && !curPragma->acrosses.empty())))
                doingOpenMP = false;
            if (!opts.useBlank) {
                std::string handlerFormalParams;
                std::string handlerBody;
                genHostHandler(handlerName, outerParams, loopVars, handlerFormalParams, handlerBody, doingOpenMP);
                std::string handlerText = handlerTemplateDecl + "void " + handlerName + "(" + handlerFormalParams + ") {\n" + handlerBody + "}\n";
                fileCtx.addHostHandler(handlerText, doingOpenMP);
            } else {
                fileCtx.addHostHandlerRequest(blankHandlerName, handlerName, doingOpenMP);
            }
            for (std::set<std::string>::iterator it = handlerTemplateInstantiations.begin(); it != handlerTemplateInstantiations.end(); it++)
                fileCtx.addToHostTail("template void " + handlerName + *it + "(" + handlerFormalParamsFwd + ");\n");
            std::string handlerType = (doingOpenMP ? fileCtx.getOmpHandlerType() : (!inRegion && !opts.paralOutside ? "HANDLER_TYPE_MASTER" : "0"));
            toInsert += indent + "dvmh_loop_register_handler_C(" + curLoop + ", DEVICE_TYPE_HOST, " + handlerType + ", dvmh_handler_func_C((DvmHandlerFunc)" +
                    handlerName + handlerTemplateCall + ", " + toStr(outerParams.size());
            if (outerParams.size() > 0)
                toInsert += ", " + handlerFactParams;
            toInsert += "));\n";
        }
        if (possibleTargets & PragmaRegion::DEVICE_TYPE_CUDA) {
            std::string handlerName = (isSequentialPart ? "sequence_" : "loop_") + shortName + "_" + toStr(line) + "_cuda";
            handlerName = getUniqueName(handlerName, &fileCtx.seenGlobalNames, &fileCtx.seenMacroNames);
            fileCtx.seenGlobalNames.insert(handlerName);
            fileCtx.addHandlerForwardDecl(handlerTemplateDecl + "void " + handlerName + "(" + handlerFormalParamsFwd + ");\n");
            if (!opts.useBlank) {
                std::string handlerFormalParams;
                std::string handlerBody;
                std::string kernelText;
                std::string cudaInfoText;
                bool isAcross = (isSequentialPart ? false : curParallelPragma->acrosses.size() > 0);
                if ( !isAcross ) {
                genCudaHandler(handlerName, outerParams, loopVars, handlerTemplateDecl, handlerTemplateCall, handlerFormalParams, handlerBody, kernelText,
                        cudaInfoText);
                std::string handlerText = kernelText + "\n" + (fileCtx.getInputFile().CPlusPlus ? handlerTemplateDecl : "extern \"C\" ") + "void " +
                        handlerName + "(" + handlerFormalParams + ") {\n" + handlerBody + "}\n";
                fileCtx.addCudaHandler(handlerText, cudaInfoText);
                } else {
                    //across implementations for GPU
                    std::string caseHandlers;
                    genAcrossCudaHandler(handlerName, outerParams, loopVars, handlerTemplateDecl, handlerTemplateCall, handlerFormalParams, handlerBody,
                        caseHandlers, cudaInfoText);
                    std::string handlerText = caseHandlers + (fileCtx.getInputFile().CPlusPlus ? handlerTemplateDecl : "extern \"C\" ") + "void " +
                        handlerName + "(" + handlerFormalParams + ") {\n" + handlerBody + "}\n";
                    fileCtx.addCudaHandler(handlerText, cudaInfoText);
                }
            } else {
                fileCtx.addCudaHandlerRequest(blankHandlerName, handlerName);
            }
            for (std::set<std::string>::iterator it = handlerTemplateInstantiations.begin(); it != handlerTemplateInstantiations.end(); it++)
                fileCtx.addToCudaTail("template void " + handlerName + *it + "(" + handlerFormalParamsFwd + ");\n");
            toInsert += indent + "dvmh_loop_register_handler_C(" + curLoop + ", DEVICE_TYPE_CUDA, 0, dvmh_handler_func_C((DvmHandlerFunc)" + handlerName +
                    handlerTemplateCall + ", " + toStr(outerParams.size());
            if (outerParams.size() > 0)
                toInsert += ", " + handlerFactParams;
            toInsert += "));\n";
            addedCudaFuncs.insert(addCudaFuncs.begin(), addCudaFuncs.end());
        }
        addCudaFuncs.clear();
        toInsert += "\n";

        if (!opts.lessDvmLines)
            toInsert += indent + genDvmLine(s->getBeginLoc()) + "\n";
        if (opts.dvmDebugLvl > 0) {
            std::string loopNumber = toStr(fileCtx.loopNumbersByLine[curPragma->line]);
            toInsert += indent + "dvmh_dbg_loop_par_start_C(" + loopNumber + ", " + toStr(loopRank);
            toInsert += lowBoundsParameters + highBoundsParameters + loopStepsParameters + ");\n";
        }
        toInsert += indent + "dvmh_loop_perform_C(" + curLoop + ");\n";
        toInsert += indent + curLoop + " = 0;\n";
        if (!inRegion) {
            // Exit implicit data region
            for (int i = 0; i < (int)outerParams.size(); i++) {
                VarState *varState = &varStates[outerParams[i]];
                if (varState->isRegular && varState->isArray)
                    toInsert += indent + "dvmh_data_exit_C((const void *)" + varState->name + ", 1);\n";
            }
            // Exit automatic interval
            if (opts.perfDbgLvl != 0 && opts.perfDbgLvl != 2) {
                toInsert += indent + genDvmLine(s->getEndLoc()) + '\n';
                toInsert += indent + "dvmh_sp_interval_end_();\n";
            }
        }
        if (needBoundaries) {
            indent = subtractIndent(indent);
            toInsert += indent + "} while (0);\n";
        }

        if (!isFull(parLoopBodyStmt))
            removeLastSemicolon(toInsert);
        removeFirstIndent(toInsert);
        rewr.ReplaceText(escapeMacro(s->getSourceRange()), toInsert);

        inParLoop = false;
        inParLoopBody = false;
    }
    if (inRegion && regionStmt == s) {
        // Finish region
        checkIntErrN(isa<CompoundStmt>(s), 911);
        SourceLocation fileLoc = srcMgr.getFileLoc(s->getBeginLoc());
        std::string fileName = srcMgr.getFilename(fileLoc).str();
        FileID fileID = srcMgr.getFileID(fileLoc);
        int line = srcMgr.getLineNumber(fileID, srcMgr.getFileOffset(fileLoc));
        cdvmhLog(TRACE, fileName, line, "Leaving region");
        PragmaRegion *curPragma = curRegionPragma;
        std::string toInsert;
        std::string indent = regionInnerIndent;
        toInsert += "\n";
        toInsert += indent + genDvmLine(curPragma) + "\n";
        // Enter implicit data region
        for (std::map<VarDecl *, int>::iterator it = needToRegister.begin(); it != needToRegister.end(); it++) {
            VarDecl *vd = it->first;
            checkIntErrN(varStates.find(vd) != varStates.end(), 92, vd->getNameAsString().c_str());
            VarState *varState = &varStates[vd];
            if (varState->isRegular) {
                toInsert += indent + "dvmh_data_enter_C((const void *)" + (varState->isArray ? "" : "&") + varState->name + ", ";
                if (varState->isIncomplete)
                    toInsert += "0";
                else
                    toInsert += "sizeof(" + varState->name + ")";
                toInsert += ");\n";
            }
        }
        toInsert += indent + "DvmType " + curRegion + " = dvmh_region_create_C(" + curPragma->getFlagsStr() + ");\n";
        toInsert += indent + "DvmType " + curLoop + " = 0;\n";
        std::set<VarDecl *> registered;
        for (std::map<VarDecl *, int>::iterator it = needToRegister.begin(); it != needToRegister.end(); it++) {
            VarDecl *vd = it->first;
            assert(vd); // is consequent of upper part
            VarState *varState = &varStates[vd];
            if (!varState->isArray && it->second == PragmaRegion::INTENT_IN) {
                // TODO: Uncomment following when other parts will be ready
                //it->second = PragmaRegion::INTENT_USE;
            }
        }
        for (int i = 0; i < (int)curPragma->regVars.size(); i++) {
            SlicedArray *arr = &curPragma->regVars[i].first;
            std::string intent = curPragma->genStrIntent(curPragma->regVars[i].second);
            VarDecl *vd = seekVarDecl(arr->name);
            checkDirErrN(vd, 301, arr->name.c_str());
            if (needToRegister.find(vd) != needToRegister.end()) {
                // XXX: It is helpful by means of excluding unnecessary scalar registration (that are only private and reduction) but now it prevents some hacks with registering unused variables. For example, to force comparing debug pass.
                assert(vd); // is consequent of upper part
                VarState *varState = &varStates[vd];
                if (arr->slicedFlag)
                    checkDirErrN((int)arr->bounds.size() == varState->rank, 304, arr->name.c_str());
                if (varState->isArray && varState->isRegular && varState->isIncomplete)
                    cdvmhLog(DEBUG, curPragma->fileName, curPragma->line, 4315, MSG(4315), varState->name.c_str());
                std::string arrHeaderRef = varState->genHeaderRef(fileCtx);
                if (!arr->slicedFlag) {
                    toInsert += indent + "dvmh_region_register_array_C(" + curRegion + ", " + intent + ", " + arrHeaderRef
                            + ", \"" + arr->name + "\");\n";
                } else {
                    checkDirErrN(varState->isArray, 331, arr->name.c_str());
                    std::string paramsStr = curRegion + ", " + intent + ", " + arrHeaderRef + ", \"" + arr->name + "\"";
                    int rank = arr->bounds.size();
                    checkDirErrN(rank == varState->rank, 332, arr->name.c_str());
                    paramsStr += ", " + toStr(rank);
                    for (int j = 0; j < (int)arr->bounds.size(); j++) {
                        MyExpr lb = arr->bounds[j].first;
                        MyExpr rb = arr->bounds[j].second;
                        checkNonDvmExpr(lb, curPragma);
                        checkNonDvmExpr(rb, curPragma);
                        paramsStr += ", " + fileCtx.dvm0c(lb.empty() ? "UNDEF_BOUND" : lb.strExpr) + ", " +
                                fileCtx.dvm0c(rb.empty() ? "UNDEF_BOUND" : rb.strExpr);
                    }
                    toInsert += indent + "dvmh_region_register_subarray_C(" + paramsStr + ");\n";
                }
                registered.insert(vd);
            }
        }
        for (std::map<VarDecl *, int>::iterator it = needToRegister.begin(); it != needToRegister.end(); it++)
            if (registered.find(it->first) == registered.end()) {
                VarDecl *vd = it->first;
                int intIntent = it->second;
                std::string intent = curPragma->genStrIntent(intIntent);
                assert(vd); // is consequent of upper part
                VarState *varState = &varStates[vd];
                if (varState->isArray && varState->isRegular && varState->isIncomplete)
                    cdvmhLog(DEBUG, curPragma->fileName, curPragma->line, 4315, MSG(4315), varState->name.c_str());
                std::string arrHeaderRef = varState->genHeaderRef(fileCtx);
                toInsert += indent + "dvmh_region_register_array_C(" + curRegion + ", " + intent + ", " + arrHeaderRef
                        + ", \"" + varState->name + "\");\n";
                registered.insert(vd);
            }
        if (curPragma->targets != 0)
            checkDirErrN((curPragma->targets | PragmaRegion::DEVICE_TYPE_HOST) == possibleTargets, 4316);
        toInsert += regionInnerIndent + "dvmh_region_execute_on_targets_C(" + curRegion + ", " +
                PragmaRegion::genStrTargets(curPragma->targets ? curPragma->targets : possibleTargets) + ");\n";
        SourceLocation loc;
        loc = escapeMacroEnd(s->getSourceRange().getBegin()).getLocWithOffset(1);
        rewr.InsertText(loc, toInsert, true, false);
        // Write tail
        toInsert = "";
        indent = regionInnerIndent;
        loc = escapeMacroBegin(cast<CompoundStmt>(s)->getRBracLoc());
        toInsert += "\n";
        if (!opts.lessDvmLines)
            toInsert += indent + genDvmLine(loc) + "\n";
        toInsert += indent + "dvmh_region_end_C(" + curRegion + ");\n";
        // Exit implicit data region
        for (std::set<VarDecl *>::iterator it = registered.begin(); it != registered.end(); it++) {
            VarState *varState = &varStates[*it];
            if (varState->isRegular)
                toInsert += indent + "dvmh_data_exit_C((const void *)" + (varState->isArray ? "" : "&") + varState->name + ", 1);\n";
        }

        toInsert += subtractIndent(indent);
        rewr.InsertText(loc, toInsert, true, false);
        inRegion = false;
        curRegion = "0";
    }
    if (inHostSection && hostSectionStmt == s) {
        inHostSection = false;
    }
    if (!rmaStack.empty() && rmaStack.back().stmt == s) {
        rmaStack.pop_back();
    }
    if (isa<ForStmt>(s) || isa<WhileStmt>(s)) // They could be without compound statement as body
        toDelete.pop_back();
    return res;
}

bool ConverterASTVisitor::TraverseCompoundStmt(CompoundStmt *s) {
    bool res = base::TraverseCompoundStmt(s);
    SourceLocation loc = s->getRBracLoc();
    SourceLocation fileLoc = srcMgr.getFileLoc(loc);
    std::string fileName = srcMgr.getFilename(fileLoc).str();
    FileID fileID = srcMgr.getFileID(fileLoc);
    int line = srcMgr.getLineNumber(fileID, srcMgr.getFileOffset(fileLoc));
    genUnbinded(fileID, line);
    if (toDelete.back().size() > 0) {
        std::string indent = extractIndent(srcMgr.getCharacterData(srcMgr.translateLineCol(fileID, line, 1)));
        indent += indentStep;
        std::string toInsert;
        toInsert += "\n";
        if (!opts.lessDvmLines)
            toInsert += indent + genDvmLine(fileLoc) + "\n";
        flushToDelete(toInsert, indent);
        toInsert += subtractIndent(indent);
        rewr.InsertText(escapeMacroBegin(loc), toInsert, true, false);
    }
    toDelete.pop_back();
    truncLevels(funcLevels);
    truncLevels(loopLevels);
    truncLevels(switchLevels);
    checkIntervalBalance(srcMgr.getFileLoc(s->getEndLoc()));
    leaveDeclContext();
    return res;
}

bool ConverterASTVisitor::VisitReturnStmt(ReturnStmt *s) {
    SourceLocation loc = s->getReturnLoc();
    SourceLocation fileLoc = srcMgr.getFileLoc(loc);
    std::string fileName = srcMgr.getFilename(fileLoc).str();
    FileID fileID = srcMgr.getFileID(fileLoc);
    int line = srcMgr.getLineNumber(fileID, srcMgr.getFileOffset(fileLoc));
    checkUserErrN(!inRegion && !inParLoop, fileName, line, 4423);
    std::string toInsert1;
    std::string indent = extractIndent(srcMgr.getCharacterData(srcMgr.translateLineCol(fileID, line, 1)));
    bool needBoundaries = true;
    if (findUpwards<CompoundStmt>(s, 1))
        needBoundaries = false;
    if (needBoundaries) {
        toInsert1 += "do {\n";
        indent += indentStep;
    }
    if (!opts.lessDvmLines)
        toInsert1 += indent + genDvmLine(fileLoc) + "\n";
    checkIntErrN(funcLevels.size() > 0 && toDelete.size() > 0, 912, fileName.c_str(), line);
    bool haveSomething = flushToDelete(toInsert1, indent, funcLevels.back());
    FunctionDecl *f = findUpwards<FunctionDecl>(s);
    checkIntErrN(f, 913, fileName.c_str(), line);
    bool fromMain = f->isMain();
    if (fromMain)
        haveSomething = true;
    toInsert1 += indent;
    if (fromMain)
        toInsert1 += "dvmh_exit_C(";
    std::string toInsert2;
    if (fromMain)
        toInsert2 += ")";
    if (needBoundaries) {
        toInsert2 += ";\n";
        indent = subtractIndent(indent);
        toInsert2 += indent + "} while (0)";
    }
    if (haveSomething) {
        //checkUserErrN(srcMgr.isWrittenInMainFile(fileLoc), fileName, line, 51);
        removeFirstIndent(toInsert1);
        rewr.InsertText(escapeMacroBegin(loc), toInsert1, true, false);
        if (fromMain) {
            Rewriter::RewriteOptions opts;
            opts.IncludeInsertsAtBeginOfRange = false;
            opts.IncludeInsertsAtEndOfRange = false;
            SourceLocation nonMacroLoc = escapeMacroBegin(loc);
            rewr.RemoveText(nonMacroLoc, Lexer::MeasureTokenLength(nonMacroLoc, srcMgr, langOpts), opts);
        }
        if (!toInsert2.empty()) {
            SourceLocation lastLoc = s->getSourceRange().getEnd();
            rewr.InsertText(Lexer::getLocForEndOfToken(escapeMacroEnd(lastLoc), 0, srcMgr, langOpts), toInsert2, true, false);
        }
    }
    return true;
}

bool ConverterASTVisitor::VisitContinueStmt(ContinueStmt *s) {
    // TODO: Figure out correctness of continue statement
    if (!inParLoop) {
        SourceLocation loc = s->getContinueLoc();
        SourceLocation fileLoc = srcMgr.getFileLoc(loc);
        std::string fileName = srcMgr.getFilename(fileLoc).str();
        FileID fileID = srcMgr.getFileID(fileLoc);
        int line = srcMgr.getLineNumber(fileID, srcMgr.getFileOffset(fileLoc));
        std::string toInsert1;
        std::string indent = extractIndent(srcMgr.getCharacterData(srcMgr.translateLineCol(fileID, line, 1)));
        bool needBoundaries = true;
        if (findUpwards<CompoundStmt>(s, 1))
            needBoundaries = false;
        if (needBoundaries) {
            toInsert1 += "if (1) {\n";
            indent += indentStep;
        }
        if (!opts.lessDvmLines)
            toInsert1 += indent + genDvmLine(fileLoc) + "\n";
        checkIntErrN(loopLevels.size() > 0 && toDelete.size() > 0, 912, fileName.c_str(), line);
        bool haveSomething = flushToDelete(toInsert1, indent, loopLevels.back());
        toInsert1 += indent;
        std::string toInsert2;
        if (needBoundaries) {
            toInsert2 += ";\n";
            indent = subtractIndent(indent);
            toInsert2 += indent + "} else ((void)0)";
        }
        if (haveSomething) {
            //checkUserErrN(srcMgr.isWrittenInMainFile(fileLoc), fileName, line, 51);
            removeFirstIndent(toInsert1);
            rewr.InsertText(escapeMacroBegin(loc), toInsert1, true, false);
            if (!toInsert2.empty()) {
                SourceLocation lastLoc = s->getSourceRange().getEnd();
                rewr.InsertText(Lexer::getLocForEndOfToken(escapeMacroEnd(lastLoc), 0, srcMgr, langOpts), toInsert2, true, false);
            }
        }
    }
    return true;
}

bool ConverterASTVisitor::VisitBreakStmt(BreakStmt *s) {
    // TODO: Figure out correctness of break statement
    if (!inParLoop) {
        SourceLocation loc = s->getBreakLoc();
        SourceLocation fileLoc = srcMgr.getFileLoc(loc);
        std::string fileName = srcMgr.getFilename(fileLoc).str();
        FileID fileID = srcMgr.getFileID(fileLoc);
        int line = srcMgr.getLineNumber(fileID, srcMgr.getFileOffset(fileLoc));
        std::string toInsert1;
        std::string indent = extractIndent(srcMgr.getCharacterData(srcMgr.translateLineCol(fileID, line, 1)));
        bool needBoundaries = true;
        if (findUpwards<CompoundStmt>(s, 1))
            needBoundaries = false;
        if (needBoundaries) {
            toInsert1 += "if (1) {\n";
            indent += indentStep;
        }
        if (!opts.lessDvmLines)
            toInsert1 += indent + genDvmLine(fileLoc) + "\n";
        checkIntErrN((loopLevels.size() > 0 || switchLevels.size() > 0) && toDelete.size() > 0, 912, fileName.c_str(), line);
        int stopLevel = 0;
        if (loopLevels.size() > 0)
            stopLevel = std::max(stopLevel, loopLevels.back());
        if (switchLevels.size() > 0)
            stopLevel = std::max(stopLevel, switchLevels.back());
        bool haveSomething = flushToDelete(toInsert1, indent, stopLevel);
        toInsert1 += indent;
        std::string toInsert2;
        if (needBoundaries) {
            toInsert2 += ";\n";
            indent = subtractIndent(indent);
            toInsert2 += indent + "} else ((void)0)";
        }
        if (haveSomething) {
            //checkUserErrN(srcMgr.isWrittenInMainFile(fileLoc), fileName, line, 51);
            removeFirstIndent(toInsert1);
            rewr.InsertText(escapeMacroBegin(loc), toInsert1, true, false);
            if (!toInsert2.empty()) {
                SourceLocation lastLoc = s->getSourceRange().getEnd();
                rewr.InsertText(Lexer::getLocForEndOfToken(escapeMacroEnd(lastLoc), 0, srcMgr, langOpts), toInsert2, true, false);
            }
        }
    }
    return true;
}

bool ConverterASTVisitor::VisitExpr(Expr *e) {
    if (!inParLoop) {
        // Self-owned calculations rule, dynamic distributed array allocation.
        bool operMatches = false;
        Expr *lhs = 0;
        Expr *rhs = 0;
        bool isAssign = false;
        if (isa<CompoundAssignOperator>(e)) {
            operMatches = true;
            lhs = cast<CompoundAssignOperator>(e)->getLHS();
        } else if (isa<BinaryOperator>(e)) {
            BinaryOperator *s = cast<BinaryOperator>(e);
            if (s->getOpcode() == BO_Assign) {
                operMatches = true;
                isAssign = true;
                lhs = s->getLHS();
                rhs = s->getRHS();
            }
        } else if (isa<UnaryOperator>(e)) {
            UnaryOperator *s = cast<UnaryOperator>(e);
            if (s->isIncrementOp() || s->isDecrementOp()) {
                operMatches = true;
                lhs = s->getSubExpr();
            }
        }
        if (operMatches) {
            // TODO: Add possibility to check self-owned calculations rule recursively
            assert(lhs && "NULL subexpression");
            int rank = 0;
            Expr *curExp = lhs;
            std::vector<ArraySubscriptExpr *> subscripts;
            while (isa<ArraySubscriptExpr>(curExp)) {
                rank++;
                subscripts.push_back(cast<ArraySubscriptExpr>(curExp));
                curExp = cast<ArraySubscriptExpr>(curExp)->getBase();
                if (isa<ImplicitCastExpr>(curExp))
                    curExp = cast<ImplicitCastExpr>(curExp)->getSubExpr();
            }
            for (int i = 0; i < rank / 2; i++)
                std::swap(subscripts[i], subscripts[rank - 1 - i]);
            if (isa<DeclRefExpr>(curExp)) {
                DeclRefExpr *dre = cast<DeclRefExpr>(curExp);
                VarDecl *vd = llvm::dyn_cast<VarDecl>(dre->getDecl());
                if (vd) {
                    checkIntErrN(varStates.find(vd) != varStates.end(), 92, vd->getNameAsString().c_str());
                    VarState *varState = &varStates[vd];
                    if (varState->isDvmArray) {
                        if (rank == varState->rank) {
                            std::string toInsert1;
                            toInsert1 += "(dvmh_get_own_element_addr_C(" + varState->name;
                            toInsert1 += ", " + toStr(rank);
                            for (int i = 0; i < rank; i++)
                                toInsert1 += ", " + fileCtx.dvm0c(convertToString(subscripts[i]->getIdx()));
                            toInsert1 += ") ? (";
                            // TODO: Remove duplicate, use one-time variables. Use the same technique as in right-hand referencing (multiple insertions between index expressions).
                            std::string subst;
                            subst += "(*(" + varState->baseTypeStr + " *)dvmh_get_own_element_addr_C(" + varState->name;
                            subst += ", " + toStr(rank);
                            for (int i = 0; i < rank; i++)
                                subst += ", " + fileCtx.dvm0c(convertToString(subscripts[i]->getIdx()));
                            subst += "))";
                            std::string toInsert2;
                            toInsert2 += ") : ((void)0))";
                            rewr.ReplaceText(escapeMacro(lhs->getSourceRange()), subst);
                            dontSubstitute.insert(dre);
                            rewr.InsertText(escapeMacroBegin(e->getBeginLoc()), toInsert1, true, false);
                            SourceLocation lastLoc = e->getSourceRange().getEnd();
                            rewr.InsertText(Lexer::getLocForEndOfToken(escapeMacroEnd(lastLoc), 0, srcMgr, langOpts), toInsert2, true, false);
                        } else {
                            SourceLocation fileLoc = srcMgr.getFileLoc(e->getBeginLoc());
                            std::string fileName = srcMgr.getFilename(fileLoc).str();
                            FileID fileID = srcMgr.getFileID(fileLoc);
                            int line = srcMgr.getLineNumber(fileID, srcMgr.getFileOffset(fileLoc));
                            checkUserErrN(rank == 0 && isAssign, fileName, line, 419, varState->name.c_str());
                            curExp = rhs;
                            while (isa<CastExpr>(curExp))
                                curExp = cast<CastExpr>(curExp)->getSubExpr();
                            checkUserErrN(isa<CallExpr>(curExp), fileName, line, 419, varState->name.c_str());
                            CallExpr *ce = cast<CallExpr>(curExp);
                            FunctionDecl *f = ce->getDirectCallee();
                            std::string funcName = f ? f->getDeclName().getAsString() : std::string();
                            if (funcName == "calloc" || funcName == "realloc")
                                checkUserErrN(false, fileName, line, 4110);
                            checkUserErrN(funcName == "malloc", fileName, line, 419, varState->name.c_str());
                            {
                                Expr *funcRef = ce->getCallee();
                                while (isa<CastExpr>(funcRef))
                                    funcRef = cast<CastExpr>(funcRef)->getSubExpr();
                                if (isa<DeclRefExpr>(funcRef))
                                    dontSubstitute.insert(cast<DeclRefExpr>(funcRef));
                            }
                            std::string indent = extractIndent(srcMgr.getCharacterData(srcMgr.translateLineCol(fileID, line, 1)));
                            std::string toInsert1, toInsert2;
                            bool needBoundaries = true;
                            if (findUpwards<CompoundStmt>(e, 1))
                                needBoundaries = false;
                            if (needBoundaries) {
                                toInsert1 += "do {\n";
                                indent += indentStep;
                            }
                            if (!opts.lessDvmLines)
                                toInsert1 += indent + genDvmLine(fileLoc) + "\n";
                            toInsert1 += indent + "dvmh_array_alloc_C(" + varState->name + ", ";
                            toInsert2 += ");\n";
                            PragmaDistribArray *curPragma = (PragmaDistribArray *)varState->declPragma;
                            if (curPragma->alignFlag == 0)
                                toInsert2 += indent + genDistribCall(varState->name, curPragma, &curPragma->distribRule) + "\n";
                            else if (curPragma->alignFlag == 1)
                                toInsert2 += indent + genAlignCall(varState->name, curPragma, &curPragma->alignRule) + "\n";
                            if (needBoundaries) {
                                indent = subtractIndent(indent);
                                toInsert2 += indent + "} while (0)";
                            } else
                                removeLastSemicolon(toInsert2);
                            removeFirstIndent(toInsert1);
                            rewr.ReplaceText(SourceRange(escapeMacroBegin(e->getBeginLoc()),
                                    escapeMacroEnd(ce->getCallee()->getEndLoc())), toInsert1);
                            rewr.InsertTextAfterToken(e->getEndLoc(), toInsert2);
                        }
                    }
                }
            }
        }
    }
    if (!inParLoop) {
        // Change references to distributed arrays
        Expr *curExp = e;
        int rank = 0;
        std::vector<ArraySubscriptExpr *> subscripts;
        while (isa<ArraySubscriptExpr>(curExp)) {
            rank++;
            subscripts.push_back(cast<ArraySubscriptExpr>(curExp));
            curExp = cast<ArraySubscriptExpr>(curExp)->getBase();
            if (isa<ImplicitCastExpr>(curExp))
                curExp = cast<ImplicitCastExpr>(curExp)->getSubExpr();
        }
        for (int i = 0; i < rank / 2; i++)
            std::swap(subscripts[i], subscripts[rank - 1 - i]);
        if (rank > 0 && isa<DeclRefExpr>(curExp)) {
            DeclRefExpr *dre = cast<DeclRefExpr>(curExp);
            VarDecl *vd = llvm::dyn_cast<VarDecl>(dre->getDecl());
            if (vd && dontSubstitute.find(dre) == dontSubstitute.end()) {
                checkIntErrN(varStates.find(vd) != varStates.end(), 92, vd->getNameAsString().c_str());
                VarState *varState = &varStates[vd];
                if (rank == varState->rank && varState->isDvmArray) {
                    // First - REMOTE_ACCESS
                    // XXX: Recursive substitution for REMOTE_ACCESS references is not allowed
                    bool done = false;
                    if (!rmaStack.empty()) {
                        std::pair<int, int> foundSubst(-1, -1);
                        for (int i = (int)rmaStack.size() - 1; foundSubst == std::make_pair(-1, -1) && i >= 0; i--) {
                            if (rmaStack[i].substs.find(vd) != rmaStack[i].substs.end()) {
                                for (int j = 0; foundSubst == std::make_pair(-1, -1) && j < (int)rmaStack[i].substs[vd].size(); j++) {
                                    DvmPragma *curPragma = rmaStack[i].pragma;
                                    checkDirErrN(varState->rank == rmaStack[i].substs[vd][j].clause.rank, 304, varState->name.c_str());
                                    bool matches = true;
                                    for (int k = 0; k < rank; k++)
                                        matches = matches && rmaStack[i].substs[vd][j].clause.matches(convertToString(subscripts[k]->getIdx()), k);
                                    if (matches)
                                        foundSubst = std::make_pair(i, j);
                                }
                            }
                        }
                        if (foundSubst != std::make_pair(-1, -1)) {
                            int rmaLevel = foundSubst.first;
                            int substIdx = foundSubst.second;
                            RmaSubstDesc &sdesc = rmaStack[rmaLevel].substs[vd][substIdx];
                            sdesc.usedFlag = true;
                            int nonConstRank = sdesc.clause.nonConstRank;
                            int bufRank = (nonConstRank > 0 ? nonConstRank : 1);
                            std::string hdrName = sdesc.nameSubst;
                            std::string subst;
                            subst += "(*(" + varState->baseTypeStr + " *)dvmh_get_own_element_addr_C(" + hdrName + ", " + toStr(bufRank);
                            for (int i = 1; i <= nonConstRank; i++) {
                                subst += ", ";
                                // Since we are in stand-alone remote_access directive, no index substitution needed
                                subst += fileCtx.dvm0cFunc() + "(" + convertToString(subscripts[sdesc.clause.axes[i - 1] - 1]->getIdx()) + ")";
                            }
                            for (int i = 0; i < bufRank - nonConstRank; i++)
                                subst += ", 0";
                            subst += "))";
                            rewr.ReplaceText(escapeMacro(e->getSourceRange()), subst);
                            done = true;
                        }
                    }
                    if (!done) {
                        // Recursive substitution-aware case
                        std::string hdrName = varState->name;
                        for (int i = rank; i >= 0; i--) {
                            std::string toInsert1, toInsert2;
                            SourceLocation firstLoc;
                            SourceLocation lastLoc;
                            bool replaceFlag = i > 0;
                            if (i == 0) {
                                firstLoc = dre->getBeginLoc();
                                lastLoc = dre->getEndLoc();
                                toInsert1 = "(*(" + varState->baseTypeStr + " *)dvmh_get_element_addr_C(";
                                toInsert2 += ", " + toStr(rank);
                            } else {
                                firstLoc = escapeMacroEnd(subscripts[i - 1]->getLHS()->getEndLoc());
                                firstLoc = Lexer::findLocationAfterToken(firstLoc, tok::l_square, srcMgr, langOpts, false);
                                firstLoc = firstLoc.getLocWithOffset(-1);
                                lastLoc = subscripts[i - 1]->getRBracketLoc();
                                toInsert1 += ", " + fileCtx.dvm0cFunc() + "(";
                                toInsert2 += ")";
                                if (i == rank)
                                    toInsert2 += "))";
                            }
                            if (replaceFlag) {
                                rewr.RemoveText(escapeMacro(SourceRange(firstLoc, firstLoc)));
                                rewr.RemoveText(escapeMacro(SourceRange(lastLoc, lastLoc)));
                            }
                            rewr.InsertText(escapeMacroBegin(firstLoc), toInsert1);
                            rewr.InsertText(Lexer::getLocForEndOfToken(escapeMacroEnd(lastLoc), 0, srcMgr, langOpts), toInsert2, false);
                        }
                        done = true;
                    }
                }
            }
        }
    }
    if (inParLoopBody) {
        parLoopBodyExprCounter++;

        // Detect writes to variables
        // TODO: Passing arrays as parameters is also a potentially writing operation
        bool operMatches = false;
        bool strictWrite = true;
        Expr *lhs = 0;
        if (isa<CompoundAssignOperator>(e)) {
            operMatches = true;
            lhs = cast<CompoundAssignOperator>(e)->getLHS();
        } else if (isa<BinaryOperator>(e)) {
            BinaryOperator *s = cast<BinaryOperator>(e);
            if (s->getOpcode() == BO_Assign) {
                operMatches = true;
                lhs = s->getLHS();
            }
        } else if (isa<UnaryOperator>(e)) {
            UnaryOperator *s = cast<UnaryOperator>(e);
            if (s->isIncrementOp() || s->isDecrementOp() || s->getOpcode() == UO_AddrOf) {
                operMatches = true;
                lhs = s->getSubExpr();
                if (s->getOpcode() == UO_AddrOf)
                    strictWrite = false;
            }
        }
        if (operMatches) {
            assert(lhs && "NULL subexpression");
            int rank = 0;
            Expr *curExp = lhs;
            while (isa<ArraySubscriptExpr>(curExp)) {
                rank++;
                curExp = cast<ArraySubscriptExpr>(curExp)->getBase();
                if (isa<ImplicitCastExpr>(curExp))
                    curExp = cast<ImplicitCastExpr>(curExp)->getSubExpr();
            }
            if (isa<DeclRefExpr>(curExp)) {
                DeclRefExpr *dre = cast<DeclRefExpr>(curExp);
                VarDecl *vd = llvm::dyn_cast<VarDecl>(dre->getDecl());
                if (vd) {
                    SourceLocation fileLoc = srcMgr.getFileLoc(dre->getLocation());
                    std::string fileName = srcMgr.getFilename(fileLoc).str();
                    int line = srcMgr.getLineNumber(srcMgr.getFileID(fileLoc), srcMgr.getFileOffset(fileLoc));
                    std::string varName = vd->getName().str();
                    cdvmhLog(TRACE, fileName, line, "Detected write to variable '%s'", varName.c_str());
                    if (innerVars.find(vd) == innerVars.end() && outerPrivates.find(vd) == outerPrivates.end() &&
                            reductions.find(vd) == reductions.end()) {
                        if (curParallelPragma && rank == 0 && (inRegion || opts.paralOutside)) {
                            // Write to non-private non-reduction scalar in parallel loop is prohibited
                            if (strictWrite)
                                userErrN(fileName, line, 4424, varName.c_str());
                            else
                                cdvmhLog(WARNING, fileName, line, 4425, MSG(4425), varName.c_str());
                        }
                        if (inRegion && (!curParallelPragma || rank > 0)) {
                            needToRegister[vd] |= PragmaRegion::INTENT_IN | PragmaRegion::INTENT_OUT;
                        }
                    }
                }
            }
        }

        // Collect Remote access appearances to convert them later
        Expr *curExp = e;
        int rank = 0;
        std::vector<ArraySubscriptExpr *> subscripts;
        while (isa<ArraySubscriptExpr>(curExp)) {
            rank++;
            subscripts.push_back(cast<ArraySubscriptExpr>(curExp));
            curExp = cast<ArraySubscriptExpr>(curExp)->getBase();
            if (isa<ImplicitCastExpr>(curExp))
                curExp = cast<ImplicitCastExpr>(curExp)->getSubExpr();
        }
        for (int i = 0; i < rank / 2; i++)
            std::swap(subscripts[i], subscripts[rank - 1 - i]);
        if (rank > 0 && isa<DeclRefExpr>(curExp) && curParallelPragma && !curParallelPragma->rmas.empty()) {
            DeclRefExpr *dre = cast<DeclRefExpr>(curExp);
            VarDecl *vd = llvm::dyn_cast<VarDecl>(dre->getDecl());
            if (vd && dontSubstitute.find(dre) == dontSubstitute.end()) {
                checkIntErrN(varStates.find(vd) != varStates.end(), 92, vd->getNameAsString().c_str());
                VarState *varState = &varStates[vd];
                if (rank == varState->rank && varState->isDvmArray) {
                    int foundIndex = -1;
                    for (int i = 0; i < (int)curParallelPragma->rmas.size(); i++) {
                        const ClauseRemoteAccess &clause = curParallelPragma->rmas[i];
                        if (!clause.excluded && vd == parallelRmaDesc->arrayDecls[i]) {
                            DvmPragma *curPragma = curParallelPragma;
                            checkDirErrN(varState->rank == clause.rank, 304, varState->name.c_str());
                            bool matches = true;
                            for (int k = 0; k < rank; k++)
                                matches = matches && clause.matches(convertToString(subscripts[k]->getIdx()), k);
                            if (matches) {
                                foundIndex = i;
                                break;
                            }
                        }
                    }
                    if (foundIndex >= 0) {
                        rmaAppearances.push_back(std::make_pair(foundIndex + 1, parLoopBodyExprCounter));
                    }
                }
            }
        }
    }
    return true;
}

bool ConverterASTVisitor::VisitCallExpr(CallExpr *e) {
    if (e->getDirectCallee()) {
        FunctionDecl *f = e->getDirectCallee();
        if (f && f->getDeclName().getAsString() == "free") {
            Expr *curExp = e->getArg(0);
            while (isa<CastExpr>(curExp))
                curExp = cast<CastExpr>(curExp)->getSubExpr();
            DeclRefExpr *re = llvm::dyn_cast<DeclRefExpr>(curExp);
            if (re) {
                VarDecl *vd = llvm::dyn_cast<VarDecl>(re->getDecl());
                if (vd) {
                    checkIntErrN(varStates.find(vd) != varStates.end(), 92, vd->getNameAsString().c_str());
                    VarState *varState = &varStates[vd];
                    if (varState->isDvmArray) {
                        SourceLocation fileLoc = srcMgr.getFileLoc(e->getBeginLoc());
                        std::string fileName = srcMgr.getFilename(fileLoc).str();
                        FileID fileID = srcMgr.getFileID(fileLoc);
                        int line = srcMgr.getLineNumber(fileID, srcMgr.getFileOffset(fileLoc));
                        std::string indent = extractIndent(srcMgr.getCharacterData(srcMgr.translateLineCol(fileID, line, 1)));
                        std::string toInsert;
                        bool needBoundaries = true;
                        if (findUpwards<CompoundStmt>(e, 1))
                            needBoundaries = false;
                        if (needBoundaries) {
                            toInsert += "do {\n";
                            indent += indentStep;
                        }
                        if (!opts.lessDvmLines)
                            toInsert += indent + genDvmLine(fileLoc) + "\n";
                        toInsert += indent + "dvmh_array_free_C(" + varState->name + ");\n";
                        if (needBoundaries) {
                            indent = subtractIndent(indent);
                            toInsert += indent + "} while (0)";
                        } else
                            removeLastSemicolon(toInsert);
                        removeFirstIndent(toInsert);
                        rewr.ReplaceText(escapeMacro(e->getSourceRange()), toInsert);
                        {
                            Expr *funcRef = e->getCallee();
                            while (isa<CastExpr>(funcRef))
                                funcRef = cast<CastExpr>(funcRef)->getSubExpr();
                            if (isa<DeclRefExpr>(funcRef))
                                dontSubstitute.insert(cast<DeclRefExpr>(funcRef));
                        }
                    }
                }
            }
        }
    }
    if (!opts.useDvmhStdio) {
        // Convert references to distributed arrays in calls to DVM headers (since all such functions are fwrite and fread, which have no implementation in LibDVMH)
        bool convertToDvmHeader = false;
        if (e->getDirectCallee()) {
            std::string name = e->getDirectCallee()->getDeclName().getAsString();
            convertToDvmHeader = name == "fread" || name == "fwrite";
        }
        if (convertToDvmHeader) {
            for (unsigned i = 0; i < e->getNumArgs(); i++) {
                Expr *curExp = e->getArg(i);
                while (isa<CastExpr>(curExp))
                    curExp = cast<CastExpr>(curExp)->getSubExpr();
                DeclRefExpr *re = llvm::dyn_cast<DeclRefExpr>(curExp);
                if (re) {
                    VarDecl *vd = llvm::dyn_cast<VarDecl>(re->getDecl());
                    if (vd) {
                        checkIntErrN(varStates.find(vd) != varStates.end(), 92, vd->getNameAsString().c_str());
                        VarState *varState = &varStates[vd];
                        if (varState->isDvmArray) {
                            cdvmh_log(TRACE, "Converting parameter: %s", varState->name.c_str());
                            std::string toInsert;
                            toInsert += "dvmh_get_dvm_header_C(" + varState->name + ")";
                            rewr.ReplaceText(escapeMacro(re->getSourceRange()), toInsert);
                        }
                    }
                }
            }
        }
    }
    return true;
}

bool ConverterASTVisitor::VisitDeclRefExpr(DeclRefExpr *e) {
    std::string name = e->getNameInfo().getAsString();
    SourceLocation declFileLoc = srcMgr.getFileLoc(e->getDecl()->getLocation());
    std::string declFileName = srcMgr.getFilename(declFileLoc).str();
    if (declFileName.rfind(PATH_SEP) != std::string::npos)
        declFileName = declFileName.substr(declFileName.rfind(PATH_SEP) + 1);
    bool globalDecl = isGlobalC(e->getDecl());
    SourceLocation exprFileLoc = srcMgr.getFileLoc(e->getLocation());
    std::string exprFileName = srcMgr.getFilename(exprFileLoc).str();
    FileID fileID = srcMgr.getFileID(exprFileLoc);
    int exprLine = srcMgr.getLineNumber(fileID, srcMgr.getFileOffset(exprFileLoc));
    if (projectCtx.hasInputFile(exprFileName))
        cdvmhLog(TRACE, exprFileName, exprLine, "DeclRefExpr is seen for %s declared in %s. DeclContext kind = %s, global flag = %d", name.c_str(),
                declFileName.c_str(), e->getDecl()->getDeclContext()->getDeclKindName(), (int)globalDecl);
    if (inRegion && inParLoopBody && !isa<VarDecl>(e->getDecl())) {
        if ((possibleTargets & PragmaRegion::DEVICE_TYPE_CUDA) && (!globalDecl || !projectCtx.hasCudaReplacement(name))) {
            if (globalDecl && isCudaFriendly(llvm::dyn_cast<FunctionDecl>(e->getDecl()))) {
                addFuncForCuda(cast<FunctionDecl>(e->getDecl()));
            } else {
                cdvmhLog(WARNING, exprFileName, exprLine, 4426, MSG(4426), e->getDecl()->getNameAsString().c_str());
                possibleTargets &= ~(PragmaRegion::DEVICE_TYPE_CUDA);
            }
        }
    }
    if (inParLoop && isa<VarDecl>(e->getDecl())) {
        VarDecl *vd = cast<VarDecl>(e->getDecl());
        if (inParLoopBody && innerVars.find(vd) == innerVars.end() && outerPrivates.find(vd) == outerPrivates.end() && reductions.find(vd) == reductions.end())
        {
            needsParams.insert(vd);
            if (inRegion) {
                needToRegister[vd] |= PragmaRegion::INTENT_IN;
            }
        } else if (!inParLoopBody) {
            // Usage in heading of parallel loop
            checkIntErrN(varStates.find(vd) != varStates.end(), 92, vd->getNameAsString().c_str());
            VarState *varState = &varStates[vd];
            checkUserErrN(varState->isRegular, curParallelPragma->fileName, curParallelPragma->line, 4427, varState->name.c_str());
            if (inRegion && outerPrivates.find(vd) == outerPrivates.end())
                varsToGetActual.insert(vd);
        }
        if (inRegion) {
            checkIntErrN(varStates.find(vd) != varStates.end(), 92, vd->getNameAsString().c_str());
            VarState *varState = &varStates[vd];
            if ((possibleTargets & PragmaRegion::DEVICE_TYPE_CUDA) && !projectCtx.hasCudaReplacement(varState->baseTypeStr) && !varState->hasDependentBaseType)
            {
                cdvmhLog(WARNING, exprFileName, exprLine, 4428, MSG(4428), varState->baseTypeStr.c_str());
                possibleTargets &= ~(PragmaRegion::DEVICE_TYPE_CUDA);
            }
        }
    }
    bool inUserFile = srcMgr.isInMainFile(exprFileLoc) || fileCtx.isUserInclude(fileID.getHashValue());
    if (inUserFile && globalDecl) {
        if (!opts.seqOutput && projectCtx.hasDvmhReplacement(name) && dontSubstitute.find(e) == dontSubstitute.end()) {
            checkUserErrN(!inParLoopBody, exprFileName, exprLine, 4429, name.c_str());
            bool canBeVoid = false;
            bool distribArrayIO = false;
            ast_type_traits::DynTypedNode dtN(ast_type_traits::DynTypedNode::create(*e));
            dtN = *comp.getASTContext().getParents(dtN).begin();
            while (dtN.get<CastExpr>())
                dtN = *comp.getASTContext().getParents(dtN).begin();
            if (const CallExpr *ce = dtN.get<CallExpr>()) {
                dtN = *comp.getASTContext().getParents(dtN).begin();
                if (dtN.get<CompoundStmt>()) {
                    canBeVoid = true;
                } else if (const IfStmt *ifStmt = dtN.get<IfStmt>()) {
                    if (ifStmt->getThen() == ce || ifStmt->getElse() == ce)
                        canBeVoid = true;
                } else if (const ForStmt *forStmt = dtN.get<ForStmt>()) {
                    if (forStmt->getBody() == ce)
                        canBeVoid = true;
                } else if (const WhileStmt *whileStmt = dtN.get<WhileStmt>()) {
                    if (whileStmt->getBody() == ce)
                        canBeVoid = true;
                } else if (const DoStmt *doStmt = dtN.get<DoStmt>()) {
                    if (doStmt->getBody() == ce)
                        canBeVoid = true;
                } else if (const BinaryOperator *binOp = dtN.get<BinaryOperator>()) {
                    if (binOp->getOpcode() == BO_Comma && binOp->getLHS() == ce)
                        canBeVoid = true;
                } else {
                    //dtN.dump(llvm::errs(), comp.getSourceManager());
                }
                if (opts.useDvmhStdio && (name == "fread" || name == "fwrite")) {
                    if (ce->getNumArgs() >= 1) {
                        const Expr *arg0 = ce->getArg(0);
                        while (isa<CastExpr>(arg0))
                            arg0 = cast<CastExpr>(arg0)->getSubExpr();
                        if (isa<DeclRefExpr>(arg0)) {
                            std::string ptrVarName = cast<DeclRefExpr>(arg0)->getNameInfo().getAsString();
                            VarDecl *vd = seekVarDecl(ptrVarName);
                            if (vd) {
                                checkIntErrN(varStates.find(vd) != varStates.end(), 92, vd->getNameAsString().c_str());
                                VarState *varState = &varStates[vd];
                                distribArrayIO = varState->isDvmArray;
                            }
                        }
                    }
                }
            }
            rewr.ReplaceText(escapeMacro(e->getSourceRange()), projectCtx.getDvmhReplacement(name, canBeVoid, distribArrayIO));
        }
    }
    return true;
}

bool ConverterASTVisitor::VisitCXXNewExpr(CXXNewExpr *e) {
    if (e->getNumPlacementArgs() == 0) {
        const Type *t = e->getType().getTypePtr();
        bool isPOD = false;
        if (t && t->isPointerType() && t->getPointeeType().isCXX11PODType(comp.getASTContext()))
            isPOD = true;
        if (t && !t->hasUnnamedOrLocalType() && isPOD) {
            SourceLocation exprFileLoc = srcMgr.getFileLoc(e->getExprLoc());
            FileID fileID = srcMgr.getFileID(exprFileLoc);
            bool inUserFile = srcMgr.isInMainFile(exprFileLoc) || fileCtx.isUserInclude(fileID.getHashValue());
            bool newInUserFile = false;
            if (e->getOperatorNew()) {
                SourceLocation newFileLoc = srcMgr.getFileLoc(e->getOperatorNew()->getLocation());
                FileID newFileID = srcMgr.getFileID(newFileLoc);
                newInUserFile = srcMgr.isInMainFile(newFileLoc) || fileCtx.isUserInclude(newFileID.getHashValue());
            }
            if (inUserFile && !newInUserFile && !opts.seqOutput) {
                Token Tok;
                if (!Lexer::getRawToken(e->getBeginLoc(), Tok, srcMgr, langOpts, true)) {
                    std::string s = comp.getPreprocessor().getSpelling(Tok);
                    if (s == "::") {
                        Lexer::getRawToken(Tok.getLocation().getLocWithOffset(Tok.getLength()), Tok, srcMgr, langOpts, true);
                        s = comp.getPreprocessor().getSpelling(Tok);
                    }
                    std::string fileName = srcMgr.getFilename(exprFileLoc).str();
                    int line = srcMgr.getLineNumber(fileID, srcMgr.getFileOffset(exprFileLoc));
                    std::string fileLine = "\"" + escapeStr(getBaseName(fileName)) + "\", " + toStr(line);
                    if (s == "new") {
                        rewr.InsertText(Tok.getLocation().getLocWithOffset(Tok.getLength()), "(dvmhDummyAllocator, " + fileLine + ")");
                        fileCtx.setNeedsAllocator();
                    } else {
                        cdvmhLog(WARNING, srcMgr.getFilename(exprFileLoc).str(), srcMgr.getLineNumber(fileID, srcMgr.getFileOffset(exprFileLoc)),
                                "Can not convert the new statement");
                    }
                }
            }
        }
    }
    return true;
}

bool ConverterASTVisitor::VisitCXXDeleteExpr(CXXDeleteExpr *e) {
    const Type *t = e->getArgument()->getType().getNonReferenceType().getTypePtr();
    bool isPOD = false;
    if (t && t->isPointerType() && t->getPointeeType().isCXX11PODType(comp.getASTContext()))
        isPOD = true;
    else if (t && t->isArrayType() && t->getAsArrayTypeUnsafe()->getElementType().isCXX11PODType(comp.getASTContext()))
        isPOD = true;
    if (t && !t->hasUnnamedOrLocalType() && isPOD) {
        SourceLocation exprFileLoc = srcMgr.getFileLoc(e->getExprLoc());
        FileID fileID = srcMgr.getFileID(exprFileLoc);
        bool inUserFile = srcMgr.isInMainFile(exprFileLoc) || fileCtx.isUserInclude(fileID.getHashValue());
        bool deleteInUserFile = false;
        if (e->getOperatorDelete()) {
            SourceLocation deleteFileLoc = srcMgr.getFileLoc(e->getOperatorDelete()->getLocation());
            FileID deleteFileID = srcMgr.getFileID(deleteFileLoc);
            deleteInUserFile = srcMgr.isInMainFile(deleteFileLoc) || fileCtx.isUserInclude(deleteFileID.getHashValue());
        }
        if (inUserFile && !deleteInUserFile && !opts.seqOutput) {
            SourceRange r1(e->getBeginLoc(), e->getArgument()->getBeginLoc().getLocWithOffset(-1));
            Token Tok;
            Lexer::getRawToken(e->getArgument()->getEndLoc(), Tok, srcMgr, langOpts, true);
            SourceLocation loc2(Tok.getLocation().getLocWithOffset(Tok.getLength()));
            std::string fileName = srcMgr.getFilename(exprFileLoc).str();
            int line = srcMgr.getLineNumber(fileID, srcMgr.getFileOffset(exprFileLoc));
            std::string fileLine = "\"" + escapeStr(getBaseName(fileName)) + "\", " + toStr(line);
            rewr.ReplaceText(escapeMacro(r1), (e->isArrayForm() ? "dvmh_delete_array(" : "dvmh_delete_one("));
            rewr.InsertText(loc2, ", " + fileLine + ")");
        }
    }
    return true;
}

bool ConverterASTVisitor::VisitCXXRecordDecl(CXXRecordDecl *d) {
    // XXX: Prepared piece of code to insert friends. For now, not needed yet.
    //const Type *t = d->getTypeForDecl();
    if (0 && d->getDefinition() == d) {
        SourceLocation fileLoc = srcMgr.getFileLoc(d->getLocation());
        FileID fileID = srcMgr.getFileID(fileLoc);
        bool inUserFile = srcMgr.isInMainFile(fileLoc) || fileCtx.isUserInclude(fileID.getHashValue());
        if (inUserFile && !opts.seqOutput) {
            bool insertFriends = true;
            if (insertFriends) {
#if CLANG_VERSION_MAJOR < 4 && CLANG_VERSION_MINOR <= 8
                rewr.InsertText(d->getRBraceLoc(), "friend class ::DvmhFriends;");
#else
                rewr.InsertText(d->getBraceRange().getEnd(), "friend class ::DvmhFriends;");
#endif
            }
        }
    }
    return true;
}

// IncludeRewriter

#if CLANG_VERSION_MAJOR < 7
void IncludeRewriter::InclusionDirective(SourceLocation HashLoc, const Token &IncludeTok, StringRef FileName, bool IsAngled, CharSourceRange FilenameRange,
        const FileEntry *File, StringRef SearchPath, StringRef RelativePath, const Module *Imported) {
#else
void IncludeRewriter::InclusionDirective(SourceLocation HashLoc, const Token &IncludeTok, StringRef FileName, bool IsAngled, CharSourceRange FilenameRange,
        const FileEntry *File, StringRef SearchPath, StringRef RelativePath, const Module *Imported, SrcMgr::CharacteristicKind FileType) {
#endif
    if (projectCtx.hasInputFile(File->getName().str())) {
        std::string convName = projectCtx.getInputFile(File->getName().str()).canonicalConverted;
        rewr.ReplaceText(FilenameRange.getAsRange(),  "\"" + convName + "\"");
    }
}

}
