#pragma once

#include <vector>
#include <string>
#include <map>
#include <algorithm>

#include "project_ctx.h"
#include "settings.h"
#include "pragmas.h"

namespace cdvmh {

struct HostReq {
    std::string handlerName;
    bool doOpenMP;
    void set(const std::string &name, bool omp) { handlerName = name; doOpenMP = omp; }
};

struct CudaReq {
    std::string handlerName;
};

class SourceFileContext {
public:
    std::set<std::string> seenGlobalNames;
    std::set<std::string> seenMacroNames;
public:
    bool isCompilable() const { return file.isCompilable; }
    int getLatestPragmaLine(unsigned fid) { return pragmasByLine[fid].second; }
    ProjectContext &getProjectCtx() const { return projectCtx; }
    const InputFile &getInputFile() const { return file; }
    void setHasRegions() { hasRegionsFlag = true; }
    void setHasLoops() { hasLoopsFlag = true; }
    void setNeedsAllocator() { needsAllocatorFlag = true; }
    const std::string &getOmpHandlerType() { ompHandlerTypeRequested = true; return ompHandlerType; }
    void setConvertedText(const std::string &convText, bool haveChanges) { convertedText = convText; textChanged = haveChanges; }
    bool isTextChanged() const { return textChanged || !mainTail.empty(); }
    std::string getConvertedText() const { return convertedText + "\n" + mainTail; }
    bool hasHostHandlers() const { return !hostHandlers.empty(); }
    bool hasCudaHandlers() const { return !cudaHandlers.empty() || !cudaTail.empty(); }
    const std::string getInitGlobName() const { return initGlobName; }
    const std::string getFinishGlobName() const { return finishGlobName; }
    const std::string getInitGlobText(bool includeOptional = true) const { return initGlobText + (includeOptional ? initGlobOptionalText : ""); }
    const std::string getFinishGlobText(bool includeOptional = true) const { return finishGlobText + (includeOptional ? finishGlobOptionalText : ""); }
    const std::string getHandlersForwardDecls() const { return handlersForwardDecls; }
    bool hasRegions() const { return hasRegionsFlag; }
    bool hasLoops() const { return hasLoopsFlag; }
    bool needsAllocator() const { return needsAllocatorFlag; }
    bool useTabs() const { return tabbedIndent; }
    const std::map<std::string, HostReq> &getHostReqMap() const { return hostRequests; }
    const std::map<std::string, CudaReq> &getCudaReqMap() const { return cudaRequests; }
    bool isExpansionForced(unsigned fid) const { return forceExpansion.find(fid) != forceExpansion.end(); }
    bool isUserInclude(unsigned fid) { return userIncludes.find(fid) != userIncludes.end(); }
    bool isInternalInclude(std::string what) { return internalIncludes.find(what) != internalIncludes.end(); }
public:
    explicit SourceFileContext(ProjectContext &aPrj, int idx);
public:
    std::string dvm0c(const std::string &strV) {
        dvm0cRequested = true;
        for (int i = 0; i < dvm0cMaxCount; i++)
            if (strV == toStr(i))
                return dvm0c(i);
        return dvm0cBase + "(" + strV + ")";
    }
    std::string dvm0c(int v) {
        dvm0cRequested = true;
        if (v >= 0 && v < dvm0cMaxCount) {
            if (v > dvm0cCount - 1)
                dvm0cCount = v + 1;
            return dvm0cBase + toStr(v);
        } else
            return dvm0cBase + "(" + toStr(v) + ")";
    }
    std::string dvm0cFunc() {
        dvm0cRequested = true;
        return dvm0cBase;
    }
    DvmPragma *getNextPragma(unsigned fid, int line, DvmPragma::Kind kind1 = DvmPragma::pkNoKind, DvmPragma::Kind kind2 = DvmPragma::pkNoKind,
            DvmPragma::Kind kind3 = DvmPragma::pkNoKind, DvmPragma::Kind kind4 = DvmPragma::pkNoKind);
    DvmPragma *getNextDebugPragma(unsigned fid, int line, DvmPragma::Kind kind = DvmPragma::pkNoKind);
    void addInitGlobText(const std::string &str, bool optional = false) {
        if (optional)
            initGlobOptionalText += str;
        else
            initGlobText += str;
    }
    void addFinishGlobText(const std::string &str, bool optional = false) {
        if (optional)
            finishGlobOptionalText = str + finishGlobOptionalText;
        else
            finishGlobText = str + finishGlobText;;
    }
    void addToForceExpansion(unsigned fid) {
        forceExpansion.insert(fid);
    }
    void addUserInclude(unsigned fid) {
        userIncludes.insert(fid);
    }
    void addHandlerForwardDecl(const std::string &handler) {
        handlersForwardDecls += handler;
    }
    void addToMainTail(const std::string &tail) {
        mainTail += tail;
    }
    void addBlankHandler(const std::string &handler) {
        blankHandlers.push_back(handler);
    }
    void addToHostHeading(const std::string &heading) {
        hostHeading += heading;
    }
    void addHostHandler(const std::string &handler, bool ompFlag) {
        hostHandlers.push_back(handler);
        usesOpenMP = usesOpenMP || ompFlag;
    }
    void addToHostTail(const std::string &tail) {
        hostTail += tail;
    }
    void addHostHandlerRequest(const std::string &blankName, const std::string &hostName, bool doingOpenMP) {
        hostRequests[blankName].set(hostName, doingOpenMP);
    }
    void addToCudaHeading(const std::string &heading) {
        cudaHeading += heading;
    }
    void addCudaHandler(const std::string &handler, const std::string &info) {
        cudaHandlers.push_back(std::make_pair(handler, info));
    }
    void addToCudaTail(const std::string &tail) {
        cudaTail += tail;
    }
    void addCudaHandlerRequest(const std::string &blankName, const std::string &cudaName) {
        cudaRequests[blankName].handlerName = cudaName;
    }
    void addPragma(unsigned fid, DvmPragma *curPragma);
    void setGlobalNames();
    std::string genDvm0cText() const;
    std::string genOmpHandlerTypeText() const;
    std::string genBlankHandlersText() const;
    std::string genHostHandlersText(bool withHeading = true) const;
    std::string genCudaHandlersText() const;
    std::string genCudaInfoText() const;
public:
    bool isDebugPass;
    std::map<int, int> loopNumbersByLine;
public:
    ~SourceFileContext();
protected:
    ProjectContext &projectCtx;
    const InputFile &file;

    std::map<unsigned, std::pair<std::map<int, DvmPragma *>, int> > pragmasByLine;
    std::map<unsigned, std::pair<std::map<int, DvmPragma *>, int> > pragmasForDebug;

    std::set<unsigned> forceExpansion;
    std::set<unsigned> userIncludes;
    std::set<std::string> internalIncludes;

    std::set<int> occupiedPragmas;
    void occupyPragma(int line) { occupiedPragmas.insert(line); }
    bool pragmaIsOccupied(int line) { return occupiedPragmas.find(line) != occupiedPragmas.end(); }

    int dvm0cCount;
    bool hasRegionsFlag;
    bool hasLoopsFlag;
    bool needsAllocatorFlag;
    bool tabbedIndent;
    bool ompHandlerTypeRequested;
    bool dvm0cRequested;
    bool usesOpenMP;
    std::string dvm0cBase;
    std::string ompHandlerType;
    std::string initGlobName; // name for special initing function
    std::string finishGlobName; // name for special finalization function
    std::string initGlobText; // body for special initing function (w/o outer braces)
    std::string finishGlobText; // body for special finalization function (w/o outer braces)
    std::string initGlobOptionalText; // optional statements for special initing function
    std::string finishGlobOptionalText; // optional statements for special finalization function
    // Output text and handlers
    std::string handlersForwardDecls;
    std::string convertedText; // converted text of the file (main part without handlers)
    std::string mainTail; // text to be inserted at the end of converted file
    bool textChanged; // flag of converted text to be not just the input file's text
    std::string blankHeading; // user-defined types go here
    std::vector<std::string> blankHandlers; // full texts of definitions of blank handlers
    std::string hostHeading; // user-defined types go here
    std::vector<std::string> hostHandlers; // full texts of definitions of host handlers
    std::string hostTail; // template instantiations go here
    std::map<std::string, HostReq> hostRequests;
    std::string cudaHeading; // user-defined types go here
    std::vector<std::pair<std::string, std::string> > cudaHandlers; // full texts of definitions of cuda handlers + 'info' texts
    std::string cudaTail; // template instantiations go here
    std::map<std::string, CudaReq> cudaRequests;
};

// Strange place for these classes

struct VarState {
    bool isTemplate;
    bool isDvmArray;
    bool isArray;
    bool isRegular;
    bool isIncomplete;
    bool hasDependentBaseType;
    bool canBeRestrict;
    int rank;
    int headerArraySize;
    std::string name;
    std::string baseTypeStr;
    std::vector<MyExpr> sizes;
    std::vector<bool> constSize;
    DvmPragma *declPragma;
    VarState() { init("unknown", "unknown", std::vector<MyExpr>()); }
    void init(const std::string &varName, const std::string &typeName, const std::vector<MyExpr> &aSizes);
    void doDvmArray(PragmaDistribArray *curPragma);
    void doTemplate(PragmaTemplate *curPragma);
    bool isTemplateLike() const { return isArray && isIncomplete && rank == 1 && baseTypeStr == "void"; }
    std::string genSizeExpr(int i) const;
    std::string genHeaderRef(SourceFileContext &fileCtx) const;
    std::string genHeaderOrScalarRef(SourceFileContext &fileCtx, bool shortForm = true) const;
    std::string genDecl(const std::string &newName) const;
    std::string genDecl() const { return genDecl(name); }
    bool isConstSize() const;
    unsigned long long getTotalElemCount() const;
};

struct LoopVarDesc {
    std::string name;
    std::string baseTypeStr;
    int stepSign; // +1 or -1
    std::string constStep; // empty if not const
};

}
