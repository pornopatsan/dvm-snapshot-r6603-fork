#pragma once

#include <vector>
#include <string>
#include <map>
#include <set>
#include <algorithm>
#include <cassert>

#include "cdvmh_log.h"
#include "messages.h"

namespace cdvmh {

class ArrayRefDesc {
public:
    std::string name;
    std::pair<int, int> head;
    std::vector<std::pair<int, int> > indexes; // With brackets
public:
    void shift(int offs) {
        head.first += offs;
        head.second += offs;
        for (int j = 0; j < (int)indexes.size(); j++) {
            indexes[j].first += offs;
            indexes[j].second += offs;
        }
    }
};

class RangeDesc {
public:
    std::pair<int, int> beginValue, endValue;
    std::vector<RangeDesc *> children;
public:
    RangeDesc() {}
    RangeDesc(const RangeDesc &other) { *this = other; }
public:
    RangeDesc &operator=(const RangeDesc &other) {
        clear();
        beginValue = other.beginValue;
        endValue = other.endValue;
        for (int i = 0; i < (int)other.children.size(); i++)
            children.push_back(new RangeDesc(*other.children[i]));
        return *this;
    }
    void shift(int offs) {
        beginValue.first += offs;
        beginValue.second += offs;
        endValue.first += offs;
        endValue.second += offs;
        for (int i = 0; i < (int)children.size(); i++)
            children[i]->shift(offs);
    }
    bool collapse() {
        for (int i = 0; i < (int)children.size(); i++) {
            if (!children[i]->collapse()) {
                children.insert(children.end(), children[i]->children.begin(), children[i]->children.end());
                children[i]->children.clear();
                delete children[i];
                children[i] = children.back();
                children.pop_back();
                i--;
            }
        }
        return endValue.first >= 0;
    }
public:
    ~RangeDesc() { clear(); }
protected:
    void clear() {
        while (!children.empty()) {
            delete children.back();
            children.pop_back();
        }
    }
};

class MyExpr {
public:
    std::string strExpr;
    std::set<std::string> usedNames;
    std::set<std::string> topLevelNames;
    std::vector<ArrayRefDesc> arrayRefs;
    std::vector<RangeDesc *> ranges;
public:
    bool empty() const { return strExpr.empty(); }
public:
    MyExpr() {}
    MyExpr(const MyExpr &e) { *this = e; }
    explicit MyExpr(const MyExpr &begin, const MyExpr &end) {
        *this = begin;
        append(" : ");
        addMeta(end, strExpr.size());
        append(end.strExpr);
        RangeDesc *rootRange = new RangeDesc;
        rootRange->beginValue.first = 0;
        rootRange->beginValue.second = (int)begin.strExpr.size() - 1;
        rootRange->endValue.first = begin.strExpr.size() + 3;
        rootRange->endValue.second = (int)strExpr.size() - 1;
        rootRange->children = ranges;
        ranges.clear();
        ranges.push_back(rootRange);
    }
public:
    MyExpr &append(const std::string &s) { strExpr += s; return *this; }
    MyExpr &prepend(const std::string &s) {
        int offs = s.size();
        strExpr = s + strExpr;
        for (int i = 0; i < (int)arrayRefs.size(); i++)
            arrayRefs[i].shift(offs);
        for (int i = 0; i < (int)ranges.size(); i++)
            ranges[i]->shift(offs);
        return *this;
    }
    MyExpr &operator=(const MyExpr &e) {
        clear();
        strExpr = e.strExpr;
        usedNames = e.usedNames;
        topLevelNames = e.topLevelNames;
        arrayRefs = e.arrayRefs;
        for (int i = 0; i < (int)e.ranges.size(); i++)
            ranges.push_back(new RangeDesc(*e.ranges[i]));
        return *this;
    }
    bool operator==(const MyExpr &e) const { return strExpr == e.strExpr; }
public:
    ~MyExpr() { clear(); }
protected:
    void addMeta(const MyExpr &e, int offs) {
        for (int i = 0; i < (int)e.arrayRefs.size(); i++) {
            ArrayRefDesc refDesc = e.arrayRefs[i];
            refDesc.shift(offs);
            arrayRefs.push_back(refDesc);
        }
        for (int i = 0; i < (int)e.ranges.size(); i++) {
            RangeDesc *rd = new RangeDesc(*e.ranges[i]);
            rd->shift(offs);
            ranges.push_back(rd);
        }
        usedNames.insert(e.usedNames.begin(), e.usedNames.end());
        topLevelNames.insert(e.topLevelNames.begin(), e.topLevelNames.end());
    }
    void clear() {
        while (!ranges.empty()) {
            delete ranges.back();
            ranges.pop_back();
        }
    }
};

struct DerivedRHSExpr {
    MyExpr constExpr; // empty for replicated and linear search
    std::string dummyName; // empty for constant and replicated
    std::vector<std::string> addShadows; // shadow edge names to add for linear search
};

struct DerivedAxisRule {
    std::string templ; // name of array or template
    std::vector<DerivedRHSExpr> rhsExprs; // right-hand side - list of index expressions of the template
    std::vector<MyExpr> exprs; // left-hand side - list of index-range expressions for the target dimension
    std::set<std::string> externalNames; // list of variable names, which are used in the left-hand expressions
};

class DistribAxisRule {
public:
    enum DistrType {dtReplicated, dtBlock, dtWgtBlock, dtGenBlock, dtMultBlock, dtIndirect, dtDerived, dtInvalid};
public:
    DistribAxisRule(): distrType(dtInvalid) {}
public:
    bool isConstant() const;
public:
    DistrType distrType;
    std::pair<std::string, MyExpr> wgtBlockArray; // pair of name of array and its size
    std::string genBlockArray; // name of array
    MyExpr multBlockValue; // size expression
    std::string indirectArray; // name of array
    DerivedAxisRule derivedRule;
};

class DistribRule {
public:
    DistribRule(): rank(-1) {}
public:
    bool isConstant() const;
public:
    int rank;
    std::vector<DistribAxisRule> axes;
};

class AlignAxisRule {
public:
    AlignAxisRule(): axisNumber(-2) {}
public:
    MyExpr origExpr; // whole expression
    int axisNumber; // -1 means replicate. 0 means mapping to constant. 1-based
    MyExpr multiplier; // 0 means mapping to constant
    MyExpr summand;
};

class AlignRule {
public:
    AlignRule(): rank(-1), templ("unknown"), templRank(-1) {}
public:
    int rank;
    std::string templ;
    int templRank;
    std::map<std::string, int> nameToAxis;
    std::vector<AlignAxisRule> axisRules;
};

class SlicedArray {
public:
    SlicedArray(): name("unknown"), slicedFlag(-1) {}
public:
    std::string name;
    int slicedFlag;
    std::vector<std::pair<MyExpr, MyExpr> > bounds; // lower, high
};

class ClauseReduction {
public:
    ClauseReduction(): redType("unknown"), arrayName("unknown"), locName("unknown") {}
public:
    static std::string guessRedType(std::string tokStr);
public:
    std::string redType;
    std::string arrayName;
    std::string locName;
    MyExpr locSize;
public:
    bool isLoc() const { return redType == "rf_MINLOC" || redType == "rf_MAXLOC"; }
    bool hasOpenMP() const { return toOpenMP() != ""; }
    std::string toOpenMP() const;
    std::string toClause() const;
protected:
    static std::map<std::string, std::string> redTypes;
    static bool redTypesInitialized;
    static void initRedTypes();
};

struct AxisShadow {
    bool isIndirect;

    // For width-based
    MyExpr lower;
    MyExpr upper;

    // For indirect
    std::vector<std::string> names;
};

class ClauseShadowRenew {
public:
    ClauseShadowRenew(): arrayName("unknown"), rank(-1), isIndirect(false), cornerFlag(-1) {}
public:
    std::string arrayName;
    int rank;
    std::vector<AxisShadow> shadows;
    bool isIndirect;
    int cornerFlag;
};

class ClauseAcross {
public:
    ClauseAcross(): arrayName("unknown"), rank(-1) {}
public:
    int getDepCount() const;
public:
    std::string  arrayName;
    int rank;
    std::vector<std::pair<MyExpr, MyExpr> > widths; // flow, anti
};

class ClauseRemoteAccess {
public:
    ClauseRemoteAccess(): arrayName("unknown"), rank(-1), excluded(false) {}
public:
    bool matches(std::string seenExpr, int idx);
public:
    std::string arrayName;
    int rank;
    int nonConstRank;
    std::vector<int> axes; // non-const axis numbers. 1-based
    std::vector<AlignAxisRule> axisRules;
    bool excluded;
};

class DvmPragma {
public:
    enum Kind {
        pkTemplate, pkDistribArray, pkRedistribute, pkRealign, pkRegion, pkParallel,
        pkGetActual, pkSetActual, pkInherit, pkRemoteAccess, pkHostSection,
        pkInterval, pkEndInterval, pkExitInterval, pkInstantiations, pkShadowAdd,
        pkLocalize, pkUnlocalize, pkArrayCopy, pkNoKind, pkCheckpoint
    };
public:
    explicit DvmPragma(Kind aKind): fileName("unknown"), line(-1), srcFileName("unknown"), srcLine(-1), srcLineSpan(0), kind(aKind) {}
public:
    void copyCommonInfo(DvmPragma *other) {
        fileName = other->fileName;
        line = other->line;
        srcFileName = other->srcFileName;
        srcLine = other->srcLine;
        srcLineSpan = other->srcLineSpan;
    }
public:
    std::string fileName;
    int line;
    std::string srcFileName;
    int srcLine;
    int srcLineSpan;
    Kind kind;
};

class PragmaTemplate: public DvmPragma {
public:
    PragmaTemplate(): DvmPragma(pkTemplate), rank(-1), alignFlag(-1), dynamicFlag(-1) {}
public:
    int rank;
    std::vector<MyExpr> sizes;
    int alignFlag;
    int dynamicFlag;
    DistribRule distribRule;
    AlignRule alignRule;
};

class PragmaDistribArray: public DvmPragma {
public:
    PragmaDistribArray(): DvmPragma(pkDistribArray), rank(-1), alignFlag(-1), dynamicFlag(-1) {}
public:
    int rank;
    int alignFlag;
    int dynamicFlag;
    DistribRule distribRule;
    AlignRule alignRule;
    std::vector<std::pair<MyExpr, MyExpr> > shadows;
};

class PragmaRedistribute: public DvmPragma {
public:
    PragmaRedistribute(): DvmPragma(pkRedistribute), name("unknown"), rank(-1) {}
public:
    std::string name;
    int rank;
    DistribRule distribRule;
};

class PragmaRealign: public DvmPragma {
public:
    PragmaRealign(): DvmPragma(pkRealign), name("unknown"), rank(-1), newValueFlag(false) {}
public:
    std::string name;
    int rank;
    AlignRule alignRule;
    bool newValueFlag;
};

class PragmaRegion: public DvmPragma {
public:
    enum Intent {INTENT_IN = 1, INTENT_OUT = 2, INTENT_LOCAL = 4, INTENT_USE = 8};
    enum RegionFlags {REGION_ASYNC = 1, REGION_COMPARE_DEBUG = 2};
    enum DeviceTypes {DEVICE_TYPE_HOST = 1, DEVICE_TYPE_CUDA = 2};
public:
    PragmaRegion(): DvmPragma(pkRegion), flags(0), targets(0) {}
public:
    static std::string genStrTargets(int targets);
    static std::string genStrIntent(int intent);
    std::string getFlagsStr();
    std::string getTargetsStr() { return genStrTargets(targets); }
public:
    std::vector<std::pair<SlicedArray, int> > regVars;
    int flags;
    int targets;
};

class PragmaParallel: public DvmPragma {
public:
    PragmaParallel(): DvmPragma(pkParallel), rank(-1), mappedFlag(false) {}
public:
    int rank;
    bool mappedFlag;
    AlignRule mapRule;
    std::vector<ClauseReduction> reductions;
    MyExpr cudaBlock[3];
    std::vector<ClauseShadowRenew> shadowRenews;
    std::vector<std::string> privates;
    std::vector<ClauseAcross> acrosses;
    MyExpr stage;
    std::vector<ClauseRemoteAccess> rmas;
};

class PragmaGetSetActual: public DvmPragma {
public:
    explicit PragmaGetSetActual(Kind aKind): DvmPragma(aKind) { checkIntErrN(kind == pkGetActual || kind == pkSetActual, 921); }
public:
    std::vector<SlicedArray> vars;
};

class PragmaInherit: public DvmPragma {
public:
    PragmaInherit(): DvmPragma(pkInherit) {}
public:
    std::vector<std::string> names;
};

class PragmaRemoteAccess: public DvmPragma {
public:
    PragmaRemoteAccess(): DvmPragma(pkRemoteAccess) {}
public:
    std::vector<ClauseRemoteAccess> rmas;
};

class PragmaHostSection: public DvmPragma {
public:
    PragmaHostSection(): DvmPragma(pkHostSection) {}
};

class PragmaInterval: public DvmPragma {
public:
    PragmaInterval(): DvmPragma(pkInterval) {}
public:
    MyExpr userID;
};

class PragmaEndInterval: public DvmPragma {
public:
    PragmaEndInterval(): DvmPragma(pkEndInterval) {}
};

class PragmaExitInterval: public DvmPragma {
public:
     PragmaExitInterval(): DvmPragma(pkExitInterval) {}
public:
    std::vector<MyExpr> ids;
};

class PragmaInstantiations: public DvmPragma {
public:
    PragmaInstantiations(): DvmPragma(pkInstantiations) {}
public:
    std::set<std::map<std::string, std::string> > valueSets;
};

class PragmaShadowAdd: public DvmPragma {
public:
    PragmaShadowAdd(): DvmPragma(pkShadowAdd), rank(-1), ruleAxis(-1) {}
public:
    std::string targetName;
    int rank;
    int ruleAxis; // 1-based
    DerivedAxisRule rule;
    std::string shadowName;
    std::vector<std::string> includeList;
};

class PragmaLocalize: public DvmPragma {
public:
    PragmaLocalize(): DvmPragma(pkLocalize), targetRank(-1), targetAxis(-1) {}
public:
    std::string refName;
    std::string targetName;
    int targetRank;
    int targetAxis;
};

class PragmaUnlocalize: public DvmPragma {
public:
    PragmaUnlocalize(): DvmPragma(pkUnlocalize) {}
public:
    std::vector<std::string> nameList;
};

class PragmaArrayCopy: public DvmPragma {
public:
    PragmaArrayCopy(): DvmPragma(pkArrayCopy) {}
public:
    std::string srcName;
    std::string dstName;
};

class PragmaCheckpoint: public DvmPragma {
public:
    enum CPAction {cpDeclare, cpSave, cpLoad};
public:
    PragmaCheckpoint(CPAction aAction): DvmPragma(pkCheckpoint), action(aAction) {}
public:
    CPAction action;
    std::string cpName;
public:
    std::string getTypeStr() {
        switch (this->action) {
            case cpDeclare: return "create_or_bind";
            case cpSave: return "save";
            case cpLoad: return "load";
        }
    }
};

class PragmaCheckpointDeclare: public PragmaCheckpoint {
public:
    PragmaCheckpointDeclare(): PragmaCheckpoint(cpDeclare) {
        distribIndents.clear();
        scalarIndents.clear();
    }
public:
    std::string mode;
    std::string nFiles;
    std::vector<std::string> distribIndents;
    std::vector<std::string> scalarIndents;
public:
    static bool isTokenMode(std::string tokStr) {
        return (tokStr == "LOCAL") | (tokStr == "LOCAL_ASYNC") | (tokStr == "PARALLEL") | (tokStr == "PARALLEL_ASYNC");
    }
};

class PragmaCheckpointSave: public PragmaCheckpoint {
public:
    PragmaCheckpointSave(): PragmaCheckpoint(cpSave) {}
};

class PragmaCheckpointLoad: public PragmaCheckpoint {
public:
    PragmaCheckpointLoad(): PragmaCheckpoint(cpLoad) {}
};

}
