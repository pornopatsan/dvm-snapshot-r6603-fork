#include "file_ctx.h"

#include <fstream>

#include "messages.h"

namespace cdvmh {

// SourceFileContext

SourceFileContext::SourceFileContext(ProjectContext &aPrj, int idx): projectCtx(aPrj), file(projectCtx.getInputFile(idx)) {
    dvm0cBase = "__DVM0C";
    ompHandlerType = "__omp_h_type";
    dvm0cCount = 0;
    hasRegionsFlag = false;
    hasLoopsFlag = false;
    needsAllocatorFlag = false;
    isDebugPass = false;
    {
        tabbedIndent = false;
        std::ifstream f(file.fileName.c_str());
        std::string line;
        while (std::getline(f, line)) {
            if (!line.empty() && line[0] == '\t') {
                tabbedIndent = true;
                break;
            }
        }
    }
    ompHandlerTypeRequested = false;
    dvm0cRequested = false;
    textChanged = false;
    internalIncludes.insert("cdvmh_debug_helpers.h");
}

DvmPragma *SourceFileContext::getNextPragma(unsigned fid, int line, DvmPragma::Kind kind1, DvmPragma::Kind kind2, DvmPragma::Kind kind3, DvmPragma::Kind kind4)
{
    DvmPragma *res = 0;
    if (pragmasByLine.find(fid) != pragmasByLine.end()) {
        int prevPragmaLine = pragmasByLine[fid].second;
        std::map<int, DvmPragma *> &aMap = pragmasByLine[fid].first;
        std::map<int, DvmPragma *>::iterator it = aMap.upper_bound(prevPragmaLine);
        if (it != aMap.end()) {
            DvmPragma *cand = it->second;
            if (it->first < line) {
                if (kind1 == DvmPragma::pkNoKind) {
                    res = cand;
                } else if (cand->kind == kind1) {
                    res = cand;
                } else if (kind2 != DvmPragma::pkNoKind && cand->kind == kind2) {
                    res = cand;
                } else if (kind3 != DvmPragma::pkNoKind && cand->kind == kind3) {
                    res = cand;
                } else if (kind4 != DvmPragma::pkNoKind && cand->kind == kind4) {
                    res = cand;
                }
                if (res)
                    pragmasByLine[fid].second = it->first;
            }
        }
    }
    return res;
}

DvmPragma *SourceFileContext::getNextDebugPragma(unsigned fid, int line, DvmPragma::Kind kind) {
    DvmPragma *res = 0;
    if (pragmasForDebug.find(fid) != pragmasForDebug.end()) {
        std::map<int, DvmPragma *> &aMap = pragmasForDebug[fid].first;
        std::map<int, DvmPragma *>::iterator it = aMap.lower_bound(line);
        if (it != aMap.begin()) {
            --it;

            DvmPragma *cand = it->second;
            if (cand->kind == kind) {
                res = cand;
            }

            if (res != 0) {
                int pragmaLine = it->first;
                if (!pragmaIsOccupied(pragmaLine)) {
                    occupyPragma(pragmaLine);
                } else {
                    res = 0;
                }
            }
        }
    }

    return res;
}


void SourceFileContext::addPragma(unsigned fid, DvmPragma *curPragma) {
    if (isDebugPass) {
        if (curPragma->kind == DvmPragma::pkParallel || curPragma->kind == DvmPragma::pkDistribArray) {
            int line = curPragma->srcLine;
            std::map<int, DvmPragma*> &aMap = pragmasForDebug[fid].first;
            if (aMap.empty())
                pragmasForDebug[fid].second = 0;
            aMap[line] = curPragma;
        }
    } else {
        bool ignore = false;
        if (projectCtx.getOptions().seqOutput && curPragma->kind != DvmPragma::pkArrayCopy)
            ignore = true;
        if (projectCtx.getOptions().noH && (curPragma->kind == DvmPragma::pkGetActual || curPragma->kind == DvmPragma::pkHostSection ||
              curPragma->kind == DvmPragma::pkRegion || curPragma->kind == DvmPragma::pkSetActual))
            ignore = true;
        if (!ignore) {
            int line = curPragma->srcLine;
            std::map<int, DvmPragma *> &aMap = pragmasByLine[fid].first;
            if (aMap.empty())
                pragmasByLine[fid].second = 0;
            aMap[line] = curPragma;
        } else {
            delete curPragma;
        }
    }
}

void SourceFileContext::setGlobalNames() {
    dvm0cBase = getUniqueName("DVM0C", &seenGlobalNames, &seenMacroNames);
    if (isCompilable()) {
        ompHandlerType = getUniqueName("OMP_H_TYPE", &seenGlobalNames, &seenMacroNames);
        // XXX: Here still can be collision with contents of other files (since these functions are supposed to be external), but it is waaaay unlikely
        initGlobName = getUniqueName("initCdvmhGlobals_" + toCIdent(file.shortName, true) + "_" + toStr(rand()), &seenGlobalNames, &seenMacroNames);
        finishGlobName = getUniqueName("finishCdvmhGlobals_" + toCIdent(file.shortName, true) + "_" + toStr(rand()), &seenGlobalNames, &seenMacroNames);
    }
}

std::string SourceFileContext::genDvm0cText() const {
    std::string res;
    if (dvm0cRequested) {
        res += "#define " + dvm0cBase + "(n) ((DvmType)(n))\n";
        for (int i = 0; i < dvm0cCount; i++)
            res += "#define " + dvm0cBase + toStr(i) + " " + dvm0cBase + "(" + toStr(i) + ")\n";
    }
    return res;
}

std::string SourceFileContext::genOmpHandlerTypeText() const {
    std::string res;
    if (ompHandlerTypeRequested) {
        res += "#ifdef _OPENMP\n";
        res += genIndent(1, useTabs()) + "#define " + ompHandlerType + " (HANDLER_TYPE_MASTER | HANDLER_TYPE_PARALLEL)\n";
        res += "#else\n";
        res += genIndent(1, useTabs()) + "#define " + ompHandlerType + " 0\n";
        res += "#endif\n";
    }
    return res;
}

std::string SourceFileContext::genBlankHandlersText() const {
    std::string res;
    res += "static int DVMH_VARIABLE_ARRAY_SIZE = 0;\n";
    res += "\n";
    if (!blankHeading.empty()) {
        res += blankHeading;
        res += "\n";
    }
    for (int i = 0; i < (int)blankHandlers.size(); i++) {
        res += blankHandlers[i] + "\n";
    }
    return res;
}

std::string SourceFileContext::genHostHandlersText(bool withHeading) const {
    std::string res;
    if (usesOpenMP) {
        res += "#ifdef _OPENMP\n";
        res += "#include <omp.h>\n";
        res += "#endif\n";
        res += "\n";
    }
    if (withHeading && !hostHeading.empty()) {
        res += hostHeading;
        res += "\n";
    }
    for (int i = 0; i < (int)hostHandlers.size(); i++) {
        res += hostHandlers[i] + "\n";
    }
    if (withHeading && !hostTail.empty()) {
        res += hostTail;
        res += "\n";
    }
    return res;
}

std::string SourceFileContext::genCudaHandlersText() const {
    std::string res;
    if (!cudaHeading.empty()) {
        res += cudaHeading;
        res += "\n";
    }
    for (int i = 0; i < (int)cudaHandlers.size(); i++) {
        res += cudaHandlers[i].first + "\n";
    }
    if (!cudaTail.empty()) {
        res += cudaTail;
        res += "\n";
    }
    return res;
}

std::string SourceFileContext::genCudaInfoText() const {
    std::string res;
    for (int i = 0; i < (int)cudaHandlers.size(); i++) {
        res += cudaHandlers[i].second + "\n";
    }
    return res;
}

SourceFileContext::~SourceFileContext() {
    for (std::map<unsigned, std::pair<std::map<int, DvmPragma *>, int> >::iterator it = pragmasByLine.begin(); it != pragmasByLine.end(); it++)
        for (std::map<int, DvmPragma *>::iterator it2 = it->second.first.begin(); it2 != it->second.first.end(); it2++)
            delete it2->second;
    pragmasByLine.clear();

    if (projectCtx.getOptions().dvmDebugLvl > 0) {
        for (std::map<unsigned, std::pair<std::map<int, DvmPragma *>, int> >::iterator it = pragmasForDebug.begin(); it != pragmasForDebug.end(); it++)
            for (std::map<int, DvmPragma *>::iterator it2 = it->second.first.begin(); it2 != it->second.first.end(); it2++)
                delete it2->second;
    }
    pragmasForDebug.clear();
}

// VarState

void VarState::init(const std::string &varName, const std::string &typeName, const std::vector<MyExpr> &aSizes) {
    name = varName;
    baseTypeStr = typeName;
    isTemplate = false;
    isDvmArray = false;
    isRegular = true;
    hasDependentBaseType = false;
    sizes = aSizes;
    rank = sizes.size();
    headerArraySize = std::max(rank + 3, 64);
    isArray = rank > 0;
    isIncomplete = rank > 0 && sizes[0].empty();
    canBeRestrict = !isIncomplete;
    declPragma = 0;
    constSize.resize(rank);
    for (int i = 0; i < rank; i++) {
        if (i == 0 && isIncomplete)
            constSize[i] = true;
        else if (isNumber(sizes[i].strExpr))
            constSize[i] = true;
        else
            constSize[i] = false;
    }
}

void VarState::doDvmArray(PragmaDistribArray *curPragma) {
    isTemplate = false;
    isDvmArray = true;
    isRegular = false;
    declPragma = curPragma;
}

void VarState::doTemplate(PragmaTemplate *curPragma) {
    isTemplate = true;
    isDvmArray = false;
    isArray = false;
    isRegular = false;
    isIncomplete = false;
    if (curPragma) {
        rank = curPragma->rank;
        sizes = curPragma->sizes;
        constSize.clear();
        constSize.resize(rank, false);
    }
    declPragma = curPragma;
    headerArraySize = 1;
}

std::string VarState::genSizeExpr(int i) const {
    std::string sizeExpr;
    if (!isArray) {
        sizeExpr += "sizeof(" + name + ")";
    } else if (i == 0 && isIncomplete) {
        sizeExpr += "0";
    } else if (constSize[i]) {
        sizeExpr += sizes[i].strExpr;
    } else {
        sizeExpr += "sizeof(" + name;
        for (int j = 0; j < i; j++)
            sizeExpr += "[0]";
        sizeExpr += ") / sizeof(" + name;
        for (int j = 0; j < i + 1; j++)
            sizeExpr += "[0]";
        sizeExpr += ")";
    }
    return sizeExpr;
}

std::string VarState::genHeaderRef(SourceFileContext &fileCtx) const {
    std::string res;
    if (isDvmArray || isTemplate) {
        res += name;
    } else {
        res += std::string("dvmh_variable_gen_header_C((const void *)") + (isArray ? "" : "&") + name + ", " + toStr(rank) + ", ";
        if (isRedType(baseTypeStr))
            res += "-" + toRedType(baseTypeStr);
        else {
            res += "sizeof(" + name;
            for (int i = 0; i < rank; i++)
                res += "[0]";
            res += ")";
        }
        for (int i = 0; i < rank; i++)
            res += ", " + fileCtx.dvm0c(genSizeExpr(i));
        res += ")";
    }
    return res;
}

std::string VarState::genHeaderOrScalarRef(SourceFileContext &fileCtx, bool shortForm) const {
    std::string res;
    if (isDvmArray || isTemplate) {
        res += name;
    } else if (!isArray) {
        res += "(const void *)&" + name;
    } else {
        res += (shortForm ? "dvmh_variable_get_header_C((const void *)" + name + ")" : genHeaderRef(fileCtx));
    }
    return res;
}

std::string VarState::genDecl(const std::string &newName) const {
    std::string res;
    res += baseTypeStr + " ";
    if (isIncomplete)
        res += std::string(rank > 1 ? "(" : "") + "*" + newName + (rank > 1 ? ")" : "");
    else
        res += newName;
    for (int i = (isIncomplete ? 1 : 0); i < rank; i++) {
        checkUserErrN(constSize[i], __FILE__, __LINE__, 441); // Should be checked on the upper level as well
        res += "[" + sizes[i].strExpr + "]";
    }
    return res;
}

bool VarState::isConstSize() const {
    for (int i = 0; i < rank; i++)
        if (!constSize[i])
            return false;
    return true;
}

unsigned long long VarState::getTotalElemCount() const {
    assert(!isIncomplete && isConstSize());
    unsigned long long res = 1;
    for (int i = 0; i < rank; i++)
        res *= toNumber(sizes[i].strExpr);
    return res;
}

}
