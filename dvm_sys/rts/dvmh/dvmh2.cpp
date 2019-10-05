#include "dvmh2.h"

#include <cassert>
#include <cstdarg>
#include <stdint.h>
#include <map>

#include "include/dvmhlib_const.h"

#include "distrib.h"
#include "dvmh_buffer.h"
#include "dvmh_data.h"
#include "dvmh_debug.h"
#include "dvmh_log.h"
#include "dvmh_pieces.h"
#include "dvmh_rts.h"
#include "dvmh_stat.h"
#include "dvmh_types.h"
#ifndef NO_DVM
#include "dvmlib_adapter.h"
#endif
#include "loop.h"
#include "mps.h"

#if defined(__APPLE__)
#include <crt_externs.h>
static char **environ = *_NSGetEnviron();
#elif !defined(WIN32)
extern char **environ;
#else
#ifndef environ
extern char **_environ;
static char **&environ = _environ;
#endif
#endif

using namespace libdvmh;

namespace libdvmh {

enum ValuePassingType {vptValue = 0, vptPointer = 1};

static DvmType extractValue(va_list &ap, ValuePassingType vpt) {
    if (vpt == vptPointer) {
        const DvmType *pV = va_arg(ap, DvmType *);
        checkInternal(pV);
        return *pV;
    } else {
        return va_arg(ap, DvmType);
    }
}

template <typename T>
static void extractArray(va_list &ap, int len, T res[], ValuePassingType vpt) {
    for (int i = 0; i < len; i++)
        extractObject(ap, res[i], vpt);
}

template <typename T>
static void extractArray(va_list &ap, std::vector<T> &res, ValuePassingType vpt) {
    extractArray(ap, res.size(), &res[0], vpt);
}

static void extractObject(va_list &ap, DvmType &res, ValuePassingType vpt) {
    res = extractValue(ap, vpt);
}

static void extractObject(va_list &ap, Interval &res, ValuePassingType vpt) {
    res[0] = extractValue(ap, vpt);
    res[1] = extractValue(ap, vpt);
}

static void extractObject(va_list &ap, LoopBounds &loopBounds, ValuePassingType vpt) {
    loopBounds[0] = extractValue(ap, vpt);
    loopBounds[1] = extractValue(ap, vpt);
    loopBounds[2] = extractValue(ap, vpt);
}

static void extractObject(va_list &ap, ShdWidth &w, ValuePassingType vpt) {
    w[0] = extractValue(ap, vpt);
    w[1] = extractValue(ap, vpt);
}

template <typename T>
static void extractObject(va_list &ap, T &res, ValuePassingType vpt) {
    loadObject(extractValue(ap, vpt), res);
}

union SavedParameter {
    DvmType intValue;
    void *addr;
};

static std::vector<SavedParameter> savedParameters;

static int saveParameter(const void *addr) {
    SavedParameter val;
    val.addr = (void *)addr;
    savedParameters.push_back(val);
    return savedParameters.size();
}

static int saveParameter(DvmType intValue) {
    SavedParameter val;
    val.intValue = intValue;
    savedParameters.push_back(val);
    return savedParameters.size();
}

class InterfaceFunctionGuard {
public:
    ~InterfaceFunctionGuard() {
        savedParameters.clear();
        stringBuffer.clear();
        stringVariables.clear();
    }
};

struct WrappedDvmHandler {
    DvmHandlerFunc func;
    std::vector<void *> params;
};

static void loadObject(int idx, WrappedDvmHandler &handler) {
    idx--;
    handler.func = (DvmHandlerFunc)savedParameters[idx++].addr;
    int paramCount = savedParameters[idx++].intValue;
    for (int i = 0; i < paramCount; i++)
        handler.params.push_back(savedParameters[idx++].addr);
}

struct DerivedRHSExpr {
    DvmType axisIndex;
    DvmType constValue;
    std::vector<std::string> addShadows;
};

static void loadObject(int idx, DerivedRHSExpr &expr) {
    idx--;
    expr.axisIndex = savedParameters[idx++].intValue;
    if (expr.axisIndex == 0) {
        expr.constValue = savedParameters[idx++].intValue;
    } else if (expr.axisIndex > 0) {
        DvmType count = savedParameters[idx++].intValue;
        checkInternal(count >= 0);
        for (int i = 0; i < count; i++)
            expr.addShadows.push_back(getStr(savedParameters[idx++].intValue));
    }
}

struct DerivedRHS {
    DvmType *templ;
    std::vector<DerivedRHSExpr> rhsExprs;
};

static void loadObject(int idx, DerivedRHS &rhs) {
    idx--;
    rhs.templ = (DvmType *)savedParameters[idx++].addr;
    DvmType templRank = savedParameters[idx++].intValue;
    checkInternal3(templRank >= 1, "Incorrect derived template rank (" DTFMT ")", templRank);
    rhs.rhsExprs.resize(templRank);
    for (int i = 0; i < templRank; i++)
        loadObject(savedParameters[idx++].intValue, rhs.rhsExprs[i]);
}

struct DerivedAxisInfo {
    DerivedRHS rhs;
    WrappedDvmHandler countingHandler;
    WrappedDvmHandler fillingHandler;
};

struct DistribAxisInfo {
    int distType;

    int mpsAxis;

    void *wgtArray;
    DvmhData::DataType wgtArrayType;
    DvmType wgtArrayLen;

    void *genBlkArray;
    DvmhData::DataType genBlkArrayType;

    DvmType multQuant;

    void *indirectArray;
    DvmhData::DataType indirectArrayType;

    DerivedAxisInfo derivedInfo;
};

void loadObject(int idx, DistribAxisInfo &axisInfo) {
    idx--;
    DvmType distType = savedParameters[idx++].intValue;
    axisInfo.distType = distType;
    checkInternal2(distType >= 0 && distType <= 6, "Unknown distribution type");
    if (distType >= 1) {
        axisInfo.mpsAxis = savedParameters[idx++].intValue;
    }
    if (distType == 2) {
        axisInfo.wgtArrayType = (DvmhData::DataType)savedParameters[idx++].intValue;
        axisInfo.wgtArray = savedParameters[idx++].addr;
        axisInfo.wgtArrayLen = savedParameters[idx++].intValue;
    } else if (distType == 3) {
        axisInfo.genBlkArrayType = (DvmhData::DataType)savedParameters[idx++].intValue;
        axisInfo.genBlkArray = savedParameters[idx++].addr;
    } else if (distType == 4) {
        axisInfo.multQuant = savedParameters[idx++].intValue;
    } else if (distType == 5) {
        axisInfo.indirectArrayType = (DvmhData::DataType)savedParameters[idx++].intValue;
        axisInfo.indirectArray = savedParameters[idx++].addr;
    } else if (distType == 6) {
        loadObject(savedParameters[idx++].intValue, axisInfo.derivedInfo.rhs);
        loadObject(savedParameters[idx++].intValue, axisInfo.derivedInfo.countingHandler);
        loadObject(savedParameters[idx++].intValue, axisInfo.derivedInfo.fillingHandler);
    }
}

static void loadObject(int idx, DvmhAxisAlignRule &axisRule) {
    idx--;
    axisRule.axisNumber = savedParameters[idx++].intValue;
    checkInternal3(axisRule.axisNumber >= -1, "Corrupted align rule: axisNumber = %d", axisRule.axisNumber);
    axisRule.multiplier = savedParameters[idx++].intValue;
    axisRule.summand = savedParameters[idx++].intValue;
    axisRule.summandLocal = axisRule.summand;
}

class DynDeclDesc: public DvmhObject, private Uncopyable {
public:
    int rank;
    DvmType typeSize;
    Interval *space;
    ShdWidth *shdWidths;
public:
    explicit DynDeclDesc(int rank): rank(rank) {
        typeSize = 0;
        space = new Interval[rank];
        shdWidths = new ShdWidth[rank];
    }
public:
    ~DynDeclDesc() {
        delete[] space;
        delete[] shdWidths;
    }
};

static DvmhObject *passOrGetDvmh(DvmType handle) {
    DvmhObject *obj = (DvmhObject *)handle;
#ifndef NO_DVM
    DvmhObject *obj2 = getDvmh(handle);
    if (obj2)
        obj = obj2;
#endif
    return obj;
}

}

extern "C" void dvmh_line_C(DvmType lineNumber, const char fileName[]) {
    InterfaceFunctionGuard guard;
    checkInternal2(fileName, "NULL pointer is passed to dvmh_line");
    checkInternal2(lineNumber >= 0, "Illegal line number is passed to dvmh_line");
    int fileNameLength = strlen(fileName);
    checkInternal2(fileNameLength > 0, "Illegal file name length is passed to dvmh_line");
    currentLine = lineNumber;
    typedMemcpy(currentFile, fileName, fileNameLength);
    currentFile[fileNameLength] = 0;
#ifndef NO_DVM
    dvmlf_(&lineNumber, currentFile, fileNameLength);
#endif
}

extern "C" void dvmh_line_(const DvmType *pLineNumber, const DvmType *pFileNameStr) {
    InterfaceFunctionGuard guard;
    checkInternal(pLineNumber && pFileNameStr);
    dvmh_line_C(*pLineNumber, getStr(pFileNameStr));
}

static void broadcastSettings(MPI_Comm mpiComm, int root = 0) {
    int count = 0;
    DvmhCommunicator comm(mpiComm);
    int myRank = comm.getCommRank();
    if (myRank == root) {
        for (int i = 0; environ[i]; i++) {
            if (!strncmp("DVMH_", environ[i], 5))
                count++;
        }
    }
    comm.bcast(root, count);
    if (myRank == root) {
        for (int i = 0; environ[i]; i++) {
          if (!strncmp("DVMH_", environ[i], 5)) {
             int len = strlen(environ[i]);
             comm.bcast(root, len);
             comm.bcast(root, environ[i], len + 1);
          }
        }
    } else {
       for (int i = 0; i < count; i++) {
          int len;
          comm.bcast(root, len);
          char *myEnv = new char[len + 1];
          comm.bcast(root, myEnv, len + 1);
          char *eqSign = strchr(myEnv, '=');
          if (eqSign)
              *eqSign = 0;
          if (!getenv(myEnv)) {
              if (eqSign)
                  *eqSign = '=';
              putenv(myEnv);
          }
       }
    }
}

static void dvmhInitInternal(DvmType flags, int *pArgc, char ***pArgv, bool acceptAlreadyInitialized, MPI_Comm comm) {
    checkInternal(pArgc && pArgv);
    dvmhSettings.setDefault();
    dvmhSettings.setFromInitFlags(flags);
    if (!dvmhSettings.idleRun) {
        int threadingState = MPI_THREAD_SINGLE;
        int isInited = 0;
        checkInternalMPI(MPI_Initialized(&isInited));
        if (acceptAlreadyInitialized && isInited) {
            checkInternalMPI(MPI_Query_thread(&threadingState));
        } else {
            checkInternal(comm == MPI_COMM_WORLD);
            checkInternalMPI(MPI_Init_thread(pArgc, pArgv, MPI_THREAD_MULTIPLE, &threadingState));
        }
        if (threadingState < MPI_THREAD_FUNNELED) {
            // XXX: Adjusted to funneled because in reality it will still work
            threadingState = MPI_THREAD_FUNNELED;
        }
        checkInternal3(threadingState >= MPI_THREAD_FUNNELED, "MPI does not provide a sufficient threading support. Required MPI_THREAD_FUNNELED. Provided %d.",
                threadingState);
        dvmhSettings.parallelMPI = threadingState == MPI_THREAD_MULTIPLE;
        broadcastSettings(comm);
    }
    int &userArgc = *pArgc;
    char **&userArgv = *pArgv;
    checkError2(userArgc >= 0, "Command-line arguments are corrupted");
    for (int i = 0; i < userArgc; i++)
        checkError2(userArgv[i], "Command-line arguments are corrupted");
    checkError2(userArgv[userArgc] == 0, "Command-line arguments are corrupted");
#ifndef NO_DVM
    char *args = 0;
    const char *envV = getenv("DVMH_ARGS");
    if (envV && *envV) {
        args = new char[strlen(envV) + 1];
        strcpy(args, envV);
    } else {
        args = new char[4];
        strcpy(args, "-cp");
        args[0] = Minus;
    }
    assert(args);
    std::string argsSave = args;
    int dvmArgc = 2;
    char *s = strtok(args, " ");
    while (s) {
        if (strlen(s) > 0)
            dvmArgc++;
        s = strtok(0, " ");
    }
    char **dvmArgv = new char *[dvmArgc + 1];
    int i = 0;
    if (userArgc > 0) {
        dvmArgv[i] = new char[strlen(userArgv[0]) + 1];
        strcpy(dvmArgv[i], userArgv[0]);
    } else {
        std::string execFN = getExecutingFileName();
        if (execFN.empty())
            execFN = "program";
        dvmArgv[i] = new char[execFN.size() + 1];
        strcpy(dvmArgv[i], execFN.c_str());
    }
    i++;
    dvmArgv[i] = new char[4];
    strcpy(dvmArgv[i], "dvm");
    i++;
    strcpy(args, argsSave.c_str());
    s = strtok(args, " ");
    while (s) {
        if (strlen(s) > 0) {
            dvmArgv[i] = new char[strlen(s) + 1];
            strcpy(dvmArgv[i], s);
            i++;
        }
        s = strtok(0, " ");
    }
    assert(i == dvmArgc);
    delete[] args;
    dvmArgv[dvmArgc] = 0;
    DvmType dvmFlags = 0;
    if (dvmhSettings.fortranProgram)
        FortranFlag = 1;
    rtl_init(dvmFlags, dvmArgc, dvmArgv);
    for (int i = 0; i < dvmArgc; i++)
        delete[] dvmArgv[i];
    delete[] dvmArgv;
#endif
    dvmhInitialize();
    for (int i = 0; i < userArgc; i++)
        dvmh_log(DEBUG, "Command-line argument #%d - '%s'", i, userArgv[i]);

    // Create first interval
    DvmType nRootInterId = -1;
    bsloop_(&nRootInterId);
}

extern "C" void dvmh_init_C(DvmType flags, int *pArgc, char ***pArgv) {
    InterfaceFunctionGuard guard;
    dvmhInitInternal(flags, pArgc, pArgv, false, MPI_COMM_WORLD);
}

extern "C" void dvmh_init2_(const DvmType *pFlags) {
    InterfaceFunctionGuard guard;
    checkInternal(pFlags);
    int argc = 0;
    char *realArgv[1] = {0};
    char **argvPtr = realArgv;
    dvmh_init_C(*pFlags | ifFortran, &argc, &argvPtr);
}

extern "C" void dvmh_init_lib_C(DvmType flags) {
    InterfaceFunctionGuard guard;
    MPI_Comm comm = MPI_COMM_WORLD;
    if (inited) {
        checkError2(rootMPS->getComm() == comm, "Initialization must be completed by the same multiprocessor system");
        PersistentSettings sett;
        sett.setFromInitFlags(flags);
        dvmhLogger.startMasterRegion();
        if (dvmhSettings.noH && !sett.noH)
            dvmh_log(WARNING, "LibDVMH was previously initialized with noH flag, all regions will be executed on HOST only");
        if (dvmhSettings.idleRun && !sett.idleRun)
            dvmh_log(WARNING, "LibDVMH was previously initialized with SERIAL flag, all regions will be executed sequentially on HOST only");
        if (!dvmhSettings.useOpenMP && sett.useOpenMP)
            dvmh_log(WARNING, "LibDVMH was previously initialized without OPENMP flag, parallel execution on CPU may be ineffective");
        dvmh_log(DEBUG, "Ignored library initialization call");
        dvmhLogger.endMasterRegion();
    } else {
        int argc = 0;
        char *realArgv[1] = {0};
        char **argvPtr = realArgv;
        dvmhInitInternal(flags, &argc, &argvPtr, true, comm);
    }
}

extern "C" void dvmh_init_lib_(const DvmType *pFlags) {
    InterfaceFunctionGuard guard;
    checkInternal(pFlags);
    dvmh_init_lib_C(*pFlags | ifFortran);
}

extern "C" void dvmh_exit_C(DvmType exitCode) {
    InterfaceFunctionGuard guard;
    checkInternal2(inited, "LibDVMH is not initialized");
    bool cleanup = true;
    dvmhFinalize(cleanup);
#ifndef NO_DVM
    lexit_(&exitCode);
#endif
    if (noLibdvm) {
        if (!dvmhSettings.idleRun) {
            int fin_flag;
            MPI_Finalized(&fin_flag);
            checkInternal2(!fin_flag, "Trying to invoke MPI_Finalize() twice");
            checkInternalMPI(MPI_Finalize());
        }
        exit(exitCode);
    }
    abort(); // Should be unreachable
}

extern "C" void dvmh_exit_(const DvmType *pExitCode) {
    InterfaceFunctionGuard guard;
    checkInternal(pExitCode);
    dvmh_exit_C(*pExitCode);
}

static void decomposeTypeSize(DvmType specifiedTypeSize, DvmhData::DataType *pDataType, DvmhData::TypeType *pTypeType, UDvmType *pTypeSize) {
    DvmhData::DataType dataType = DvmhData::dtUnknown;
    DvmhData::TypeType typeType = DvmhData::ttUnknown;
    UDvmType typeSize = 0;
    if (specifiedTypeSize <= 0) {
        checkInternal2(-specifiedTypeSize >= 0 && -specifiedTypeSize < DvmhData::DATA_TYPES, "Unknown data type");
        dataType = (DvmhData::DataType)-specifiedTypeSize;
        typeType = DvmhData::getTypeType((DvmhData::DataType)-specifiedTypeSize);
        typeSize = DvmhData::getTypeSize((DvmhData::DataType)-specifiedTypeSize);
    } else {
        typeSize = specifiedTypeSize;
    }
    if (pDataType)
        *pDataType = dataType;
    if (pTypeType)
        *pTypeType = typeType;
    if (pTypeSize)
        *pTypeSize = typeSize;
}

extern "C" void dvmh_array_declare_C(DvmType dvmDesc[], DvmType rank, DvmType typeSize, /* DvmType axisSize, DvmType shadowLow, DvmType shadowHigh */...) {
    InterfaceFunctionGuard guard;
    checkInternal2(inited, "LibDVMH is not initialized");
    checkError2(!currentRegion && !currentLoop, "Call to dvmh_array_declare is not allowed in region or parallel loop");
    checkInternal2(rank > 0, "Array rank must be positive");
    decomposeTypeSize(typeSize, 0, 0, 0);
    DynDeclDesc *desc = new DynDeclDesc(rank);
    desc->typeSize = typeSize;
    va_list ap;
    va_start(ap, typeSize);
    for (int i = 0; i < rank; i++) {
        desc->space[i][0] = 0;
        desc->space[i][1] = extractValue(ap, vptValue) - 1;
        desc->shdWidths[i][0] = extractValue(ap, vptValue);
        desc->shdWidths[i][1] = extractValue(ap, vptValue);
        checkError3(i == 0 || desc->space[i].size() > 0, "Negative size of array on axis %d: " DTFMT, i + 1, desc->space[i][1] + 1);
        checkError3(desc->shdWidths[i][0] >= 0, "Negative lower shadow width on axis %d: " DTFMT, i + 1, desc->shdWidths[i][0]);
        checkError3(desc->shdWidths[i][1] >= 0, "Negative upper shadow width on axis %d: " DTFMT, i + 1, desc->shdWidths[i][1]);
    }
    va_end(ap);
    allObjects.insert(desc);
    dvmDesc[0] = (DvmType)desc;
}

static DvmhData *dvmhArrayCreate(int rank, DvmType typeSize, const Interval indexes[], const ShdWidth shadows[]) {
    assert(rank > 0);
    for (int i = 0; i < rank; i++) {
        checkError3(indexes[i][0] <= indexes[i][1], "Upper index of array (" DTFMT ") is less than lower (" DTFMT ") on axis %d", indexes[i][1], indexes[i][0],
                i + 1);
        checkError3(shadows[i][0] >= 0, "Negative lower shadow width on axis %d: " DTFMT, i + 1, shadows[i][0]);
        checkError3(shadows[i][1] >= 0, "Negative upper shadow width on axis %d: " DTFMT, i + 1, shadows[i][1]);
    }
    DvmhData::DataType dataType;
    DvmhData::TypeType typeType;
    UDvmType realTypeSize;
    decomposeTypeSize(typeSize, &dataType, &typeType, &realTypeSize);
    DvmhData *data = 0;
    if (dataType != DvmhData::dtUnknown)
        data = new DvmhData(dataType, rank, indexes, shadows);
    else
        data = new DvmhData(realTypeSize, typeType, rank, indexes, shadows);
    allObjects.insert(data);
    return data;
}

extern "C" void dvmh_array_create_C(DvmType dvmDesc[], DvmType rank, DvmType typeSize, /* DvmType axisSize, DvmType shadowLow, DvmType shadowHigh */...) {
    InterfaceFunctionGuard guard;
    checkInternal2(inited, "LibDVMH is not initialized");
    checkError2(!currentRegion && !currentLoop, "Call to dvmh_array_create is not allowed in region or parallel loop");
    checkInternal(dvmDesc);
    checkInternal2(rank > 0, "Array rank must be positive");
#ifdef NON_CONST_AUTOS
    Interval indexes[rank];
    ShdWidth shadows[rank];
#else
    Interval indexes[MAX_ARRAY_RANK];
    ShdWidth shadows[MAX_ARRAY_RANK];
#endif
    va_list ap;
    va_start(ap, typeSize);
    for (int i = 0; i < rank; i++) {
        indexes[i][0] = 0;
        indexes[i][1] = extractValue(ap, vptValue) - 1;
        shadows[i][0] = extractValue(ap, vptValue);
        shadows[i][1] = extractValue(ap, vptValue);
    }
    va_end(ap);
    DvmhData *data = dvmhArrayCreate(rank, typeSize, indexes, shadows);
    data->addHeader(dvmDesc, 0);
}

extern "C" void dvmh_array_create_(DvmType dvmDesc[], const void *baseAddr, const DvmType *pRank, const DvmType *pTypeSize,
        /* const DvmType *pSpaceLow, const DvmType *pSpaceHigh, const DvmType *pShadowLow, const DvmType *pShadowHigh */...) {
    InterfaceFunctionGuard guard;
    checkInternal2(inited, "LibDVMH is not initialized");
    checkError2(!currentRegion && !currentLoop, "Call to dvmh_array_create is not allowed in region or parallel loop");
    checkInternal(dvmDesc && pRank && pTypeSize);
    int rank = *pRank;
    checkInternal2(rank > 0, "Array rank must be positive");
#ifdef NON_CONST_AUTOS
    Interval indexes[rank];
    ShdWidth shadows[rank];
#else
    Interval indexes[MAX_ARRAY_RANK];
    ShdWidth shadows[MAX_ARRAY_RANK];
#endif
    va_list ap;
    va_start(ap, pTypeSize);
    for (int i = 0; i < rank; i++) {
        indexes[i][0] = extractValue(ap, vptPointer);
        indexes[i][1] = extractValue(ap, vptPointer);
        shadows[i][0] = extractValue(ap, vptPointer);
        shadows[i][1] = extractValue(ap, vptPointer);
    }
    va_end(ap);
    DvmhData *data = dvmhArrayCreate(rank, *pTypeSize, indexes, shadows);
    data->addHeader(dvmDesc, baseAddr);
}

static DvmhDistribSpace *dvmhTemplateCreate(int rank, const Interval indexes[], bool standAlone = true) {
    assert(rank > 0);
    for (int i = 0; i < rank; i++) {
        checkError3(indexes[i][0] <= indexes[i][1], "Upper index of template (" DTFMT ") is less than lower (" DTFMT ") on axis %d", indexes[i][1],
                indexes[i][0], i + 1);
    }
    DvmhDistribSpace *dspace = new DvmhDistribSpace(rank, indexes);
    if (standAlone)
        dspace->addRef();
    if (standAlone)
        allObjects.insert(dspace);
    return dspace;
}

extern "C" void dvmh_template_create_C(DvmType dvmDesc[], DvmType rank, /* DvmType axisSize */...) {
    InterfaceFunctionGuard guard;
    checkInternal2(inited, "LibDVMH is not initialized");
    checkError2(!currentRegion && !currentLoop, "Call to dvmh_template_create is not allowed in region or parallel loop");
    checkInternal(dvmDesc);
    checkInternal2(rank > 0, "Template rank must be positive");
#ifdef NON_CONST_AUTOS
    Interval indexes[rank];
#else
    Interval indexes[MAX_ARRAY_RANK];
#endif
    va_list ap;
    va_start(ap, rank);
    for (int i = 0; i < rank; i++) {
        indexes[i][0] = 0;
        indexes[i][1] = extractValue(ap, vptValue) - 1;
    }
    va_end(ap);
    dvmDesc[0] = (DvmType)dvmhTemplateCreate(rank, indexes);
}

extern "C" void dvmh_template_create_(DvmType dvmDesc[], const DvmType *pRank, /* const DvmType *pSpaceLow, const DvmType *pSpaceHigh */...) {
    InterfaceFunctionGuard guard;
    checkInternal2(inited, "LibDVMH is not initialized");
    checkError2(!currentRegion && !currentLoop, "Call to dvmh_template_create is not allowed in region or parallel loop");
    checkInternal(dvmDesc && pRank);
    int rank = *pRank;
    checkInternal2(rank > 0, "Template rank must be positive");
#ifdef NON_CONST_AUTOS
    Interval indexes[rank];
#else
    Interval indexes[MAX_ARRAY_RANK];
#endif
    va_list ap;
    va_start(ap, pRank);
    extractArray(ap, rank, indexes, vptPointer);
    va_end(ap);
    dvmDesc[0] = (DvmType)dvmhTemplateCreate(rank, indexes);
}

extern "C" void dvmh_array_alloc_C(DvmType dvmDesc[], DvmType byteCount) {
    InterfaceFunctionGuard guard;
    checkInternal2(inited, "LibDVMH is not initialized");
    checkError2(!currentRegion && !currentLoop, "Call to dvmh_array_alloc is not allowed in region or parallel loop");
    checkInternal(dvmDesc);
    DvmhObject *obj = (DvmhObject *)dvmDesc[0];
    checkError2(obj, "NULL object is passed to dvmh_array_alloc");
    checkError2(allObjects.find(obj) != allObjects.end(), "Unknown object is passed to dvmh_array_alloc");
    if (obj->is<DvmhData>()) {
        checkError2(false, "Either DVM-array usage violation or memory leak detected. Consider using another DVM-array variable or freeing dynamically allocated memory beforehand.");
        // TODO: Maybe we will devise some good scheme of memory leakage support and passing-by-value for dynamic arrays (as in C with pointers)
        DvmhData *data = obj->as<DvmhData>();
        data->removeHeader(dvmDesc);
        int rank = data->getRank();
        DynDeclDesc *desc = new DynDeclDesc(rank);
        if (data->getDataType() != DvmhData::dtUnknown)
            desc->typeSize = -data->getDataType();
        else
            desc->typeSize = data->getTypeSize();
        desc->space->blockAssign(rank, data->getSpace());
        for (int i = 0; i < rank; i++)
            desc->shdWidths[i] = data->getShdWidth(i + 1);
        allObjects.insert(desc);
        obj = desc;
    }
    checkInternal2(obj->isExactly<DynDeclDesc>(), "Only declared array pointer can be passed to dvmh_array_alloc");
    DynDeclDesc *desc = obj->as<DynDeclDesc>();
    int rank = desc->rank;
    checkInternal2(rank > 0, "Array rank must be positive");
    DvmType typeSize = desc->typeSize;
    UDvmType realTypeSize;
    decomposeTypeSize(typeSize, 0, 0, &realTypeSize);
    UDvmType restSizes = (desc->space + 1)->blockSize(rank - 1);
    checkInternal2(restSizes > 0, "Size must be positve");
    checkError3(byteCount > 0 && byteCount % (realTypeSize * restSizes) == 0,
            "Allocation amount (" DTFMT ") must be exact, i.e. a product of all the array's dimension sizes", byteCount);
    desc->space[0][1] = desc->space[0][0] + byteCount / (realTypeSize * restSizes) - 1;
    DvmhData *data = dvmhArrayCreate(rank, typeSize, desc->space, desc->shdWidths);
    allObjects.erase(desc);
    delete desc;
    data->addHeader(dvmDesc, 0);
}

extern "C" DvmType dvmh_handler_func_C(DvmHandlerFunc handlerFunc, DvmType customParamCount, /* void *param */...) {
    DvmType res = saveParameter((const void *)handlerFunc);
    saveParameter(customParamCount);
    va_list ap;
    va_start(ap, customParamCount);
    for (int i = 0; i < customParamCount; i++)
        saveParameter(va_arg(ap, void *));
    va_end(ap);
    return res;
}

extern "C" DvmType dvmh_handler_func_(DvmHandlerFunc handlerFunc, const DvmType *pCustomParamCount, /* void *param */...) {
    checkInternal(pCustomParamCount);
    DvmType res = saveParameter((const void *)handlerFunc);
    saveParameter(*pCustomParamCount);
    va_list ap;
    va_start(ap, pCustomParamCount);
    for (int i = 0; i < *pCustomParamCount; i++)
        saveParameter(va_arg(ap, void *));
    va_end(ap);
    return res;
}

extern "C" DvmType dvmh_derived_rhs_expr_ignore_() {
    return saveParameter(-1);
}

extern "C" DvmType dvmh_derived_rhs_expr_constant_C(DvmType indexValue) {
    DvmType res = saveParameter((DvmType)0);
    saveParameter(indexValue);
    return res;
}

extern "C" DvmType dvmh_derived_rhs_expr_constant_(const DvmType *pIndexValue) {
    checkInternal(pIndexValue);
    return dvmh_derived_rhs_expr_constant_C(*pIndexValue);
}

extern "C" DvmType dvmh_derived_rhs_expr_scan_C(DvmType shadowCount, /* const char shadowName[] */...) {
    DvmType res = saveParameter(1);
    saveParameter(shadowCount);
    va_list ap;
    va_start(ap, shadowCount);
    for (int i = 0; i < shadowCount; i++) {
        char *str = va_arg(ap, char *);
        int len = strlen(str);
        saveParameter(dvmh_string_(str, len));
    }
    va_end(ap);
    return res;
}

extern "C" DvmType dvmh_derived_rhs_expr_scan_(const DvmType *pShadowCount, /* const DvmType *pShadowNameStr */...) {
    checkInternal(pShadowCount);
    DvmType res = saveParameter(1);
    saveParameter(*pShadowCount);
    va_list ap;
    va_start(ap, pShadowCount);
    for (int i = 0; i < *pShadowCount; i++)
        saveParameter(extractValue(ap, vptPointer));
    va_end(ap);
    return res;
}

extern "C" DvmType dvmh_derived_rhs_C(const DvmType templDesc[], DvmType templRank, /* DvmType derivedRhsExprHelper */...) {
    DvmType res = saveParameter(templDesc);
    saveParameter(templRank);
    va_list ap;
    va_start(ap, templRank);
    for (int i = 0; i < templRank; i++)
        saveParameter(extractValue(ap, vptValue));
    va_end(ap);
    return res;
}

extern "C" DvmType dvmh_derived_rhs_(const DvmType templDesc[], const DvmType *pTemplRank, /* const DvmType *pDerivedRhsExprHelper */...) {
    checkInternal(pTemplRank);
    DvmType res = saveParameter(templDesc);
    saveParameter(*pTemplRank);
    va_list ap;
    va_start(ap, pTemplRank);
    for (int i = 0; i < *pTemplRank; i++)
        saveParameter(extractValue(ap, vptPointer));
    va_end(ap);
    return res;
}

extern "C" DvmType dvmh_distribution_replicated_() {
    return saveParameter((DvmType)0);
}

extern "C" DvmType dvmh_distribution_block_C(DvmType mpsAxis) {
    DvmType res = saveParameter(1);
    saveParameter(mpsAxis);
    return res;
}

extern "C" DvmType dvmh_distribution_block_(const DvmType *pMpsAxis) {
    checkInternal(pMpsAxis);
    return dvmh_distribution_block_C(*pMpsAxis);
}

extern "C" DvmType dvmh_distribution_wgtblock_C(DvmType mpsAxis, DvmType elemType, const void *arrayAddr, DvmType elemCount) {
    DvmType res = saveParameter(2);
    saveParameter(mpsAxis);
    saveParameter(elemType);
    saveParameter(arrayAddr);
    saveParameter(elemCount);
    return res;
}

extern "C" DvmType dvmh_distribution_wgtblock_(const DvmType *pMpsAxis, const DvmType *pElemType, const void *arrayAddr, const DvmType *pElemCount) {
    checkInternal(pMpsAxis && pElemType && pElemCount);
    return dvmh_distribution_wgtblock_C(*pMpsAxis, *pElemType, arrayAddr, *pElemCount);
}

extern "C" DvmType dvmh_distribution_genblock_C(DvmType mpsAxis, DvmType elemType, const void *arrayAddr) {
    DvmType res = saveParameter(3);
    saveParameter(mpsAxis);
    saveParameter(elemType);
    saveParameter(arrayAddr);
    return res;
}

extern "C" DvmType dvmh_distribution_genblock_(const DvmType *pMpsAxis, const DvmType *pElemType, const void *arrayAddr) {
    checkInternal(pMpsAxis && pElemType);
    return dvmh_distribution_genblock_C(*pMpsAxis, *pElemType, arrayAddr);
}

extern "C" DvmType dvmh_distribution_multblock_C(DvmType mpsAxis, DvmType multBlock) {
    DvmType res = saveParameter(4);
    saveParameter(mpsAxis);
    saveParameter(multBlock);
    return res;
}

extern "C" DvmType dvmh_distribution_multblock_(const DvmType *pMpsAxis, const DvmType *pMultBlock) {
    checkInternal(pMpsAxis && pMultBlock);
    return dvmh_distribution_multblock_C(*pMpsAxis, *pMultBlock);
}

extern "C" DvmType dvmh_distribution_indirect_C(DvmType mpsAxis, DvmType elemType, const void *arrayAddr) {
    DvmType res = saveParameter(5);
    saveParameter(mpsAxis);
    saveParameter(elemType);
    saveParameter(arrayAddr);
    return res;
}

extern "C" DvmType dvmh_distribution_indirect_(const DvmType *pMpsAxis, const DvmType *pElemType, const void *arrayAddr) {
    checkInternal(pMpsAxis && pElemType);
    return dvmh_distribution_indirect_C(*pMpsAxis, *pElemType, arrayAddr);
}

extern "C" DvmType dvmh_distribution_derived_C(DvmType mpsAxis, DvmType derivedRhsHelper, DvmType countingHandlerHelper, DvmType fillingHandlerHelper) {
    DvmType res = saveParameter(6);
    saveParameter(mpsAxis);
    saveParameter(derivedRhsHelper);
    saveParameter(countingHandlerHelper);
    saveParameter(fillingHandlerHelper);
    return res;
}

extern "C" DvmType dvmh_distribution_derived_(const DvmType *pMpsAxis, const DvmType *pDerivedRhsHelper, const DvmType *pCountingHandlerHelper, const DvmType *pFillingHandlerHelper) {
    checkInternal(pMpsAxis && pDerivedRhsHelper && pCountingHandlerHelper && pFillingHandlerHelper);
    return dvmh_distribution_derived_C(*pMpsAxis, *pDerivedRhsHelper, *pCountingHandlerHelper, *pFillingHandlerHelper);
}

static void dvmhAlignRealign(bool realignFlag, DvmhData *data, DvmhObject *templ, DvmhAxisAlignRule rules[], bool newValueFlag, bool ownDistribSpace);

#ifndef NO_DVM
static void dvmCreateTiedAMV(DvmhDistribSpace *dspace) {
    int rank = dspace->getRank();
#ifdef NON_CONST_AUTOS
    DvmType sizes[rank];
#else
    DvmType sizes[MAX_DISTRIB_SPACE_RANK];
#endif
    for (int i = 0; i < rank; i++)
        sizes[i] = dspace->getSpace()[i].size();
    DvmType staticSign = 1;
    DvmType dvmRank = rank;
    UDvmType amvRef = crtamv_(0, &dvmRank, sizes, &staticSign);
    tieDvm(dspace, amvRef);
}
#endif

static void checkDerivedAndFillBuffer(const DerivedAxisInfo &derived, UDvmType *pDerivedCount, DvmType **pDerivedBuf) {
    DvmhObject *templ = passOrGetOrCreateDvmh(derived.rhs.templ[0], true);
    checkError2(templ, "NULL pointer is passed as a template in DERIVED distribution");
    checkError2(allObjects.find(templ) != allObjects.end(), "Unknown object is passed as a template in DERIVED distribution");
    checkInternal2(templ->isExactly<DvmhDistribSpace>() || templ->isExactly<DvmhData>(),
            "Only array or template can be passed as a template in DERIVED distribution");
    bool isArray = !templ->isExactly<DvmhDistribSpace>();
    int templRank = !isArray ? templ->as<DvmhDistribSpace>()->getRank() : templ->as<DvmhData>()->getRank();
    checkInternal2(templRank > 0, "Rank must be positive");
    checkError3(templRank == (int)derived.rhs.rhsExprs.size(), "Rank in DERIVED rule must be the same as in declaration of the %s",
            (isArray ? "array" : "template"));
    DvmhDistribSpace *dspace = 0;
    const DvmhAlignRule *underRule = 0;
    DvmhData *templData = 0;
    if (isArray) {
        templData = templ->as<DvmhData>();
        checkError2(templData->isDistributed(), "Array on which alignment is requested is not aligned nor distributed itself");
        underRule = templData->getAlignRule();
        dspace = templData->getAlignRule()->getDspace();
    } else {
        dspace = templ->as<DvmhDistribSpace>();
    }
    assert(dspace);
    int dspaceRank = dspace->getRank();
#ifdef NON_CONST_AUTOS
    DvmhAxisAlignRule resRules[dspaceRank], rules[templRank];
    Interval searchSpace[dspaceRank], searchBlock[dspaceRank], dspacePart[dspaceRank];
    DvmType blockLow[dspaceRank], blockHigh[dspaceRank];
    void *counterParams[derived.countingHandler.params.size() + 3];
    void *fillerParams[derived.fillingHandler.params.size() + 3];
#else
    DvmhAxisAlignRule resRules[MAX_DISTRIB_SPACE_RANK], rules[MAX_DISTRIB_SPACE_RANK];
    Interval searchSpace[MAX_DISTRIB_SPACE_RANK], searchBlock[MAX_DISTRIB_SPACE_RANK], dspacePart[MAX_DISTRIB_SPACE_RANK];
    DvmType blockLow[MAX_DISTRIB_SPACE_RANK], blockHigh[MAX_DISTRIB_SPACE_RANK];
    void *counterParams[MAX_PARAM_COUNT];
    void *fillerParams[MAX_PARAM_COUNT];
#endif
    int nextFreeAxis = 1;
    for (int j = 0; j < templRank; j++) {
        const DerivedRHSExpr &expr = derived.rhs.rhsExprs[j];
        Interval templInt = (isArray ? templData->getAxisSpace(j + 1) : dspace->getAxisSpace(j + 1));
        if (expr.axisIndex == -1) {
            rules[j].setReplicated(1, templInt);
        } else if (expr.axisIndex == 0) {
            checkError2(templInt.contains(expr.constValue), "Index out of bounds for DERIVED template");
            rules[j].setConstant(expr.constValue);
        } else {
            checkInternal2(expr.axisIndex == 1, "Corrupted DERIVED rule");
            rules[j].setLinear(nextFreeAxis, 1, 0);
            searchSpace[nextFreeAxis - 1] = templInt;
            nextFreeAxis++;
            checkInternal2(expr.addShadows.empty(), "Adding shadow edges are not implemented yet");
        }
    }
    int searchSpaceRank = nextFreeAxis - 1;
    for (int j = 0; j < dspaceRank; j++) {
        if (!underRule) {
            resRules[j] = rules[j];
        } else {
            resRules[j] = *underRule->getAxisRule(j + 1);
            resRules[j].composite(rules);
        }
    }
    counterParams[0] = 0; // To be filled later
    counterParams[1] = blockLow;
    counterParams[2] = blockHigh;
    for (int i = 0; i < (int)derived.countingHandler.params.size(); i++)
        counterParams[3 + i] = derived.countingHandler.params[i];
    fillerParams[0] = 0; // To be filled later
    fillerParams[1] = blockLow;
    fillerParams[2] = blockHigh;
    for (int i = 0; i < (int)derived.fillingHandler.params.size(); i++)
        fillerParams[3 + i] = derived.fillingHandler.params[i];

    UDvmType derivedCount = 0;
    std::vector<DvmType> elemsPerBlock;
    DvmType *derivedBuf = 0;
    DvmType *currentDerivedBuf = 0;
    DvmhTimer tm(true);
    for (int phase = 0; phase < 2; phase++) {
        if (phase == 1) {
            derivedBuf = new DvmType[derivedCount + 1];
            std::fill(derivedBuf, derivedBuf + derivedCount + 1, (DvmType)UNDEF_BOUND);
            currentDerivedBuf = derivedBuf;
            dvmh_log(DEBUG, "Time to allocate and prefill derived buffer: %g", tm.lap());
        }
        int blockCounter = 0;

        // TODO: Add shadows, loop through blocks
        bool notEmpty = false;
        {
            dspacePart->blockAssign(dspaceRank, dspace->getLocalPart());
            DvmhAlignRule *alignRule = new DvmhAlignRule(searchSpaceRank, dspace, resRules);
            searchBlock->blockAssign(searchSpaceRank, searchSpace);
            notEmpty = alignRule->mapOnPart(dspace->getLocalPart(), searchBlock, false);
            delete alignRule;
        }
        if (notEmpty) {
            for (int i = 0; i < searchSpaceRank; i++) {
                blockLow[i] = searchBlock[i][0];
                blockHigh[i] = searchBlock[i][1];
            }
            if (phase == 0) {
                DvmType currentElemCount = 0;
                counterParams[0] = &currentElemCount;
                executeFunction(derived.countingHandler.func, counterParams, derived.countingHandler.params.size() + 3);
                checkInternal(currentElemCount >= 0);
                elemsPerBlock.push_back(currentElemCount);
                derivedCount += currentElemCount;
            } else {
                fillerParams[0] = currentDerivedBuf;
                executeFunction(derived.fillingHandler.func, fillerParams, derived.fillingHandler.params.size() + 3);
                assert(blockCounter < (int)elemsPerBlock.size());
                currentDerivedBuf += elemsPerBlock[blockCounter];
            }
            blockCounter++;
        }

        assert(blockCounter == (int)elemsPerBlock.size());
        if (phase == 0)
            dvmh_log(DEBUG, "Time to count indexes: %g", tm.lap());
        else
            dvmh_log(DEBUG, "Time to fill indexes: %g", tm.lap());
    }
    assert(currentDerivedBuf == derivedBuf + derivedCount);
    checkInternal(derivedBuf[derivedCount] == UNDEF_BOUND);
    UDvmType lookLikeUnfilledCount = 0;
    for (UDvmType i = 0; i < derivedCount; i++)
        lookLikeUnfilledCount += derivedBuf[i] == UNDEF_BOUND;
    dvmh_log(DEBUG, "Time to make check for unfilledness: %g", tm.lap());
    if (lookLikeUnfilledCount > 0) {
        dvmh_log(WARNING, "Looks like some parts (" UDTFMT " out of " UDTFMT ") of the indirect buffer are not filled", lookLikeUnfilledCount, derivedCount);
    }
    dvmh_log(DEBUG, "Total time for indirect buffer creation: %g", tm.total());
    *pDerivedBuf = derivedBuf;
    *pDerivedCount = derivedCount;
}

static void dvmhDistributeRedistribute(bool redistrFlag, DvmhObject *obj, const std::vector<DistribAxisInfo> &axisInfos) {
    DvmhDistribSpace *dspace = 0;
    bool isArray = obj->isExactly<DvmhData>();
    if (obj->isExactly<DvmhDistribSpace>()) {
        dspace = obj->as<DvmhDistribSpace>();
    } else if (obj->isExactly<DvmhData>()) {
        DvmhData *data = obj->as<DvmhData>();
        if (redistrFlag && !data->isAligned())
            redistrFlag = false;
        if (redistrFlag) {
            checkInternal2(data->isDistributed(), "Can not redistribute non-distributed (regular) array");
            dspace = data->getAlignRule()->getDspace();
            assert(dspace);
        } else {
            checkInternal2(!data->isAligned(), "Array is already aligned or distributed");
            dspace = dvmhTemplateCreate(data->getRank(), data->getSpace(), false);
        }
    }
    int rank = dspace->getRank();
    if (redistrFlag && !dspace->isDistributed())
        redistrFlag = false;
    if (!redistrFlag)
        checkInternal2(dspace->isDistributed() == false, "Template is already distributed");
    checkInternal2(currentMPS, "Current multiprocessor system must be present");
    bool hadIndirect = redistrFlag && dspace->getDistribRule()->hasIndirect();
    bool hasIndirect = false;
    DvmhDistribRule *rule = new DvmhDistribRule(rank, currentMPS);
    std::set<int> mpsAxesSeen;
    for (int i = 1; i <= rank; i++) {
        Interval spaceDim = dspace->getAxisSpace(i);
        UDvmType dimSize = spaceDim.size();
        DvmhAxisDistribRule *axisRule = 0;
        if (axisInfos[i - 1].distType == 0)
            axisRule = DvmhAxisDistribRule::createReplicated(currentMPS, spaceDim);
        else {
            int mpsAxis = axisInfos[i - 1].mpsAxis;
            checkInternal3(mpsAxis >= 1, "Invalid multiprocessor system axis %d is supplied for the template dimension %d. Must be a positive integer number.", mpsAxis, i);
            checkInternal3(mpsAxesSeen.find(mpsAxis) == mpsAxesSeen.end(), "Multiprocessor system axes cannot be used more than once. Encountered usage of %d for template dimension %d.", mpsAxis, i);
            mpsAxesSeen.insert(mpsAxis);
            if (axisInfos[i - 1].distType == 1) {
                // BLOCK
                axisRule = DvmhAxisDistribRule::createBlock(currentMPS, mpsAxis, spaceDim);
            } else if (axisInfos[i - 1].distType == 2) {
                // WGT_BLOCK
                double wgtSum = 0;
                checkInternal2(axisInfos[i - 1].wgtArrayType == DvmhData::dtFloat || axisInfos[i - 1].wgtArrayType == DvmhData::dtDouble,
                        "WGT_BLOCK parameter must be regular array of 'float' or 'double' type");
                checkError2(axisInfos[i - 1].wgtArrayLen >= 1, "WGT_BLOCK parameter must contain at least one number");
                DvmType wgtArrayLen = axisInfos[i - 1].wgtArrayLen;
                bool floatFlag = axisInfos[i - 1].wgtArrayType == DvmhData::dtFloat;
                void *ptr = axisInfos[i - 1].wgtArray;
                checkError2(ptr, "NULL pointer is passed as WGT_BLOCK parameter");
                for (DvmType j = 0; j < wgtArrayLen; j++) {
                    double curWgt = (floatFlag ? (double)((float *)ptr)[j] : ((double *)ptr)[j]);
                    checkError2(curWgt >= 0, "WGT_BLOCK parameter must contain only non-negative numbers");
                    wgtSum += curWgt;
                }
                if (wgtSum <= 0) {
                    dvmhLogger.startMasterRegion();
                    dvmh_log(WARNING, "WGT_BLOCK parameter contains only zeroes. Treating it as BLOCK");
                    dvmhLogger.endMasterRegion();
                    axisRule = DvmhAxisDistribRule::createBlock(currentMPS, mpsAxis, spaceDim);
                } else {
                    DvmhData *wgtData = DvmhData::fromRegularArray(axisInfos[i - 1].wgtArrayType, ptr, wgtArrayLen);
                    axisRule = DvmhAxisDistribRule::createWeightBlock(currentMPS, mpsAxis, spaceDim, wgtData);
                    delete wgtData;
                }
            } else if (axisInfos[i - 1].distType == 3) {
                // GEN_BLOCK
                checkInternal2(axisInfos[i - 1].genBlkArrayType == DvmhData::dtInt || axisInfos[i - 1].genBlkArrayType == DvmhData::dtLong,
                        "GEN_BLOCK parameter must be regular array of 'int' or 'long' type");
                void *ptr = axisInfos[i - 1].genBlkArray;
                checkError2(ptr, "NULL pointer is passed as GEN_BLOCK parameter");
                UDvmType resSize = 0;
                int procCount = currentMPS->getAxis(mpsAxis).procCount;
                for (int p = 0; p < procCount; p++) {
                    DvmType curSize = (axisInfos[i - 1].genBlkArrayType == DvmhData::dtInt ? (long)((int *)ptr)[p] : ((long *)ptr)[p]);
                    checkError3(curSize >= 0, "GEN_BLOCK parameter must contain only non-negative numbers: value " DTFMT " encountered at index %d",
                            curSize, p);
                    resSize += curSize;
                }
                checkError3(resSize == dimSize, "Sum of GEN_BLOCK elements (" UDTFMT ") must be equal to template's dimension size (" UDTFMT ")",
                        resSize, dimSize);
                DvmhData *gblData = DvmhData::fromRegularArray(axisInfos[i - 1].genBlkArrayType, ptr, procCount);
                axisRule = DvmhAxisDistribRule::createGenBlock(currentMPS, mpsAxis, spaceDim, gblData);
                delete gblData;
            } else if (axisInfos[i - 1].distType == 4) {
                // MULT_BLOCK
                DvmType multQuant = axisInfos[i - 1].multQuant;
                checkError3(multQuant > 0, "MULT_BLOCK parameter (" DTFMT ") must be positive number", multQuant);
                checkError3(dimSize % multQuant == 0, "Template dimension size (" UDTFMT ") must be multiple of MULT_BLOCK parameter (" DTFMT ")",
                        dimSize, multQuant);
                axisRule = DvmhAxisDistribRule::createMultBlock(currentMPS, mpsAxis, spaceDim, multQuant);
            } else if (axisInfos[i - 1].distType == 5) {
                // INDIRECT
                hasIndirect = true;
                bool tempData = false;
                DvmhData *indirectData = 0;
                checkError2(axisInfos[i - 1].indirectArray, "NULL pointer is passed as INDIRECT parameter");
                if (axisInfos[i - 1].indirectArrayType == DvmhData::dtUnknown) {
                    // It is a distributed array
                    DvmhObject *obj = passOrGetOrCreateDvmh(((DvmType *)axisInfos[i - 1].indirectArray)[0], true);
                    checkError2(obj && allObjects.find(obj) != allObjects.end(), "Unknown object is passed as INDIRECT parameter");
                    checkInternal2(obj->is<DvmhData>(), "Only arrays can be passed as INDIRECT parameter");
                    indirectData = obj->as<DvmhData>();
                    checkInternal2(indirectData->getDataType() == DvmhData::dtInt || indirectData->getDataType() == DvmhData::dtLong,
                            "INDIRECT parameter must be array of 'int' or 'long' type");
                } else {
                    // It is a regular array
                    checkInternal2(axisInfos[i - 1].indirectArrayType == DvmhData::dtInt || axisInfos[i - 1].indirectArrayType == DvmhData::dtLong,
                            "INDIRECT parameter must be array of 'int' or 'long' type");
                    indirectData = DvmhData::fromRegularArray(axisInfos[i - 1].indirectArrayType, axisInfos[i - 1].indirectArray, dimSize, spaceDim[0]);
                    tempData = true;
                }
                assert(indirectData);
                axisRule = DvmhAxisDistribRule::createIndirect(currentMPS, mpsAxis, indirectData);
                if (tempData)
                    delete indirectData;
            } else if (axisInfos[i - 1].distType == 6) {
                // DERIVED
                hasIndirect = true;
                DvmType *derivedBuf = 0;
                UDvmType derivedCount = 0;
                checkDerivedAndFillBuffer(axisInfos[i - 1].derivedInfo, &derivedCount, &derivedBuf);
                axisRule = DvmhAxisDistribRule::createDerived(currentMPS, mpsAxis, spaceDim, derivedBuf, derivedCount);
                delete[] derivedBuf;
            } else {
                checkInternal2(0, "Internal inconsistency");
            }
        }
        rule->setAxisRule(i, axisRule);
    }
    dspace->redistribute(rule);
    dvmh_log(TRACE, "New dspace local part:");
    custom_log(TRACE, blockOut, dspace->getRank(), dspace->getLocalPart());
    for (std::set<DvmhData *>::const_iterator it = dspace->getAlignedDatas().begin(); it != dspace->getAlignedDatas().end(); it++) {
        DvmhData *data = *it;
        dvmh_log(TRACE, "New local part:");
        custom_log(TRACE, blockOut, data->getRank(), data->getLocalPart());
    }
#ifndef NO_DVM
    if (redistrFlag)
        checkInternal2(hadIndirect == hasIndirect, "Transition between block-distribution and indirect-distribution is not implemented yet");
    if (!hasIndirect) {
        if (!redistrFlag)
            dvmCreateTiedAMV(dspace);
        bool wasDvmMain = false;
        AMViewRef amvHandle = (AMViewRef)getDvm(dspace, &wasDvmMain);
        bool hasWgtBlock = false;
        bool hasGenBlock = false;
        bool hasMultBlock = false;
        bool hasProcWeight = false;
        for (int i = 0; i < rank; i++) {
            if (axisInfos[i].distType == 2)
                hasWgtBlock = true;
            if (axisInfos[i].distType == 3)
                hasGenBlock = true;
            if (axisInfos[i].distType == 4)
                hasMultBlock = true;
            if (axisInfos[i].distType == 1 || axisInfos[i].distType == 2 || axisInfos[i].distType == 4)
                hasProcWeight = hasProcWeight || currentMPS->getAxis(rule->getAxisRule(i + 1)->getMPSAxis()).isWeighted();
        }
        bool useGenBlock = dvmhSettings.useGenblock || hasWgtBlock || hasGenBlock || hasMultBlock || hasProcWeight;
        int distrAxesCount = 0;
        for (int i = 1; i <= rank; i++) {
            const DvmhAxisDistribRule *axisRule = dspace->getAxisDistribRule(i);
            if (!axisRule->isReplicated()) {
                distrAxesCount++;
            }
        }
#ifdef NON_CONST_AUTOS
        DvmType multQuants[rank], distrAxes[distrAxesCount], nullArray[rank];
        int *genBlocks[distrAxesCount];
        AddrType gblAddresses[distrAxesCount];
#else
        DvmType multQuants[MAX_DISTRIB_SPACE_RANK], distrAxes[MAX_MPS_RANK], nullArray[MAX_DISTRIB_SPACE_RANK];
        int *genBlocks[MAX_MPS_RANK];
        AddrType gblAddresses[MAX_MPS_RANK];
#endif
        for (int i = 1; i <= rank; i++) {
            const DvmhAxisDistribRule *axisRule = dspace->getAxisDistribRule(i);
            assert(axisRule->isReplicated() || axisRule->isBlockDistributed());
            multQuants[i - 1] = 1;
            if (!axisRule->isReplicated()) {
                int mpsAxis = axisRule->getMPSAxis();
                distrAxes[mpsAxis - 1] = i;
                if (!useGenBlock) {
                    if (axisInfos[i - 1].distType == 4)
                        multQuants[i - 1] = axisInfos[i - 1].multQuant;
                    genBlocks[mpsAxis - 1] = 0;
                } else {
                    int procCount = axisRule->getMPS()->getAxis(axisRule->getMPSAxis()).procCount;
                    genBlocks[mpsAxis - 1] = new int[procCount];
                    for (int p = 0; p < procCount; p++)
                        genBlocks[mpsAxis - 1][p] = axisRule->asBlockDistributed()->getLocalElems(p).size();
                    gblAddresses[mpsAxis - 1] = (AddrType)genBlocks[mpsAxis - 1];
                }
            }
            nullArray[i - 1] = 0;
        }
        PSRef curPS = 0;
        if (!useGenBlock) {
            if (hasMultBlock) {
                DvmType tmp = rank;
                blkdiv_(&amvHandle, multQuants, &tmp);
            }
        } else {
            if (distrAxesCount > 0) {
                DvmType tmp = distrAxesCount;
                genbli_(&curPS, &amvHandle, gblAddresses, &tmp);
            }
        }
        if (redistrFlag) {
            untieDvm(dspace);
            std::map<DvmhData *, std::pair<DvmType *, bool> > dataToDvmHeader;
            for (std::set<DvmhData *>::const_iterator it = dspace->getAlignedDatas().begin(); it != dspace->getAlignedDatas().end(); it++) {
                DvmhData *data = *it;
                bool isDvmMain = false;
                SysHandle *hndl = getDvm(data, &isDvmMain);
                dataToDvmHeader[data] = std::make_pair((DvmType *)hndl->HeaderPtr, isDvmMain);
                untieDvm(data);
            }
            DvmType tmp[2] = {distrAxesCount, 0};
            redis_(&amvHandle, &curPS, &tmp[0], distrAxes, nullArray, &tmp[1]);
            SysHandle *newAMV = (SysHandle *)amvHandle;
            tieDvm(dspace, newAMV, wasDvmMain);
            for (std::set<DvmhData *>::const_iterator it = dspace->getAlignedDatas().begin(); it != dspace->getAlignedDatas().end(); it++) {
                DvmhData *data = *it;
                SysHandle *hndl = (SysHandle *)dataToDvmHeader[data].first[0];
                bool isDvmMain = dataToDvmHeader[data].second;
                s_DISARRAY *dvmHead = (s_DISARRAY *)hndl->pP;
                if (data->hasLocal()) {
#ifdef NON_CONST_AUTOS
                    Interval hostPortion[data->getRank()];
#else
                    Interval hostPortion[MAX_ARRAY_RANK];
#endif
                    for (int i = 0; i < data->getRank(); i++) {
                        DvmType starti = data->getAxisSpace(i + 1)[0];
                        hostPortion[i][0] = dvmHead->ArrBlock.Block.Set[i].Lower + starti;
                        hostPortion[i][1] = dvmHead->ArrBlock.Block.Set[i].Upper + starti;
                        dvmh_log(DEBUG, "LibDVM's local part is [" DTFMT ".." DTFMT "]", dvmHead->Block.Set[i].Lower + starti,
                                dvmHead->Block.Set[i].Upper + starti);
                        assert(dvmHead->Block.Set[i].Lower + starti == data->getLocalPart()[i][0]);
                        assert(dvmHead->Block.Set[i].Upper + starti == data->getLocalPart()[i][1]);
                    }
                    bool hadHostRepr = data->getRepr(0) != 0;
                    data->setHostPortion(getAddrFromDvm(dvmHead), hostPortion);
                    if (!hadHostRepr) {
                        data->getRepr(0)->getActualState()->uniteOne(data->getLocalPart());
                        data->initActualShadow();
                    }
                }
                tieDvm(data, hndl, isDvmMain);
            }
        } else {
            DvmType tmp = distrAxesCount;
            distr_(&amvHandle, &curPS, &tmp, distrAxes, nullArray);
        }
        if (!useGenBlock) {
            if (hasMultBlock)
                blkdiv_(&amvHandle, multQuants, nullArray);
        } else {
            for (int i = 0; i < distrAxesCount; i++)
                delete[] genBlocks[i];
        }
    }
#endif
    if (!redistrFlag && isArray) {
#ifdef NON_CONST_AUTOS
        DvmhAxisAlignRule rules[rank];
#else
        DvmhAxisAlignRule rules[MAX_DISTRIB_SPACE_RANK];
#endif
        for (int i = 0; i < rank; i++)
            rules[i].setLinear(i + 1, 1, 0);
        dvmhAlignRealign(false, obj->as<DvmhData>(), dspace, rules, true, true);
    }
    for (std::set<DvmhData *>::const_iterator it = dspace->getAlignedDatas().begin(); it != dspace->getAlignedDatas().end(); it++) {
        DvmhData *data = *it;
        if (data->hasLocal()) {
            if (!data->getRepr(0))
                data->createNewRepr(0, data->getLocalPlusShadow());
            if (!redistrFlag)
                data->initActual(0);
        }
    }
}

static void dvmhDistributeRedistributeCommon(bool redistrFlag, DvmType dvmDesc[], DvmType givenRank, va_list &ap, bool fortranFlag) {
    const char *call = (redistrFlag ? "dvmh_redistribute" : "dvmh_distribute");
    checkInternal2(inited, "LibDVMH is not initialized");
    checkError3(!currentRegion && !currentLoop, "Call to %s is not allowed in region or parallel loop", call);
    checkInternal(dvmDesc);
    DvmhObject *obj = passOrGetOrCreateDvmh(dvmDesc[0], true);
    checkError3(obj, "NULL pointer is passed to %s", call);
    checkError3(allObjects.find(obj) != allObjects.end(), "Unknown object is passed to %s", call);
    checkInternal3(obj->isExactly<DvmhDistribSpace>() || obj->isExactly<DvmhData>(), "Unknown type of object is passed to %s", call);
    bool isArray = !obj->isExactly<DvmhDistribSpace>();
    int rank = !isArray ? obj->as<DvmhDistribSpace>()->getRank() : obj->as<DvmhData>()->getRank();
    checkInternal2(rank > 0, "Rank must be positive");
    checkError3(rank == givenRank, "Rank in %s directive must be the same as in declaration of the %s", (redistrFlag ? "redistribute" : "distribute"),
            (isArray ? "array" : "template"));
    std::vector<DistribAxisInfo> axisInfos(rank);
    extractArray(ap, axisInfos, (fortranFlag ? vptPointer : vptValue));
    dvmhDistributeRedistribute(redistrFlag, obj, axisInfos);
}

extern "C" void dvmh_distribute_C(DvmType dvmDesc[], DvmType rank, /* DvmType distributionHelper */...) {
    InterfaceFunctionGuard guard;
    va_list ap;
    va_start(ap, rank);
    dvmhDistributeRedistributeCommon(false, dvmDesc, rank, ap, false);
    va_end(ap);
}

extern "C" void dvmh_distribute_(DvmType dvmDesc[], const DvmType *pRank, /* const DvmType *pDistributionHelper */...) {
    InterfaceFunctionGuard guard;
    checkInternal(pRank);
    va_list ap;
    va_start(ap, pRank);
    dvmhDistributeRedistributeCommon(false, dvmDesc, *pRank, ap, true);
    va_end(ap);
}

extern "C" void dvmh_redistribute_C(DvmType dvmDesc[], DvmType rank, /* DvmType distributionHelper */...) {
    InterfaceFunctionGuard guard;
    va_list ap;
    va_start(ap, rank);
    dvmhDistributeRedistributeCommon(true, dvmDesc, rank, ap, false);
    va_end(ap);
}

extern "C" void dvmh_redistribute2_(DvmType dvmDesc[], const DvmType *pRank, /* const DvmType *pDistributionHelper */...) {
    InterfaceFunctionGuard guard;
    checkInternal(pRank);
    va_list ap;
    va_start(ap, pRank);
    dvmhDistributeRedistributeCommon(true, dvmDesc, *pRank, ap, true);
    va_end(ap);
}

extern "C" DvmType dvmh_alignment_linear_C(DvmType axis, DvmType multiplier, DvmType summand) {
    DvmType res = saveParameter(axis);
    saveParameter(multiplier);
    saveParameter(summand);
    return res;
}

extern "C" DvmType dvmh_alignment_linear_(const DvmType *pAxis, const DvmType *pMultiplier, const DvmType *pSummand) {
    checkInternal(pAxis && pMultiplier && pSummand);
    return dvmh_alignment_linear_C(*pAxis, *pMultiplier, *pSummand);
}

#ifndef NO_DVM
static void dvmCreateTiedDA(DvmhData *data) {
    int rank = data->getRank();
#ifdef NON_CONST_AUTOS
    DvmType sizes[rank], shadowsLow[rank], shadowsHigh[rank];
#else
    DvmType sizes[MAX_ARRAY_RANK], shadowsLow[MAX_ARRAY_RANK], shadowsHigh[MAX_ARRAY_RANK];
#endif
    DvmType *dvmHeader = data->getAnyHeader();
    void *base = (void *)dvmHeader[rank + 2];
    data->removeHeader(dvmHeader);
    for (int i = 0; i < rank; i++) {
        dvmHeader[rank + 2 + i] = data->getAxisSpace(i + 1)[0];
        sizes[i] = data->getAxisSpace(i + 1).size();
        shadowsLow[i] = data->getShdWidth(i + 1)[0];
        shadowsHigh[i] = data->getShdWidth(i + 1)[1];
    }
    DvmType tmp[5];
    tmp[0] = 1;
    tmp[1] = 1;
    tmp[2] = 3;
    tmp[3] = rank;
    tmp[4] = data->getTypeSize();
    crtda_(dvmHeader, &tmp[0], base, &tmp[3], &tmp[4], sizes, &tmp[1], &tmp[2], shadowsLow, shadowsHigh);
    tieDvm(data, dvmHeader[0]);
}
#endif

void dvmhAlignRealign(bool realignFlag, DvmhData *data, DvmhObject *templ, DvmhAxisAlignRule rules[], bool newValueFlag, bool ownDistribSpace) {
    bool isArray = templ->isExactly<DvmhData>();
    DvmhDistribSpace *dspace = 0;
    const DvmhAlignRule *underRule = 0;
    DvmhData *templData = 0;
    int templRank = 0;
    if (isArray) {
        templData = templ->as<DvmhData>();
        templRank = templData->getRank();
        checkError2(templData->isDistributed(), "Array on which alignment is requested is not aligned nor distributed itself");
        underRule = templData->getAlignRule();
        dspace = templData->getAlignRule()->getDspace();
    } else {
        dspace = templ->as<DvmhDistribSpace>();
        templRank = dspace->getRank();
    }
    assert(dspace);
    int dataRank = data->getRank();
    int dspaceRank = dspace->getRank();
#ifdef NON_CONST_AUTOS
    DvmhAxisAlignRule resRules[dspaceRank];
#else
    DvmhAxisAlignRule resRules[MAX_DISTRIB_SPACE_RANK];
#endif
    if (realignFlag && !data->isAligned())
        realignFlag = false;
    if (!realignFlag)
        checkInternal2(!data->isAligned(), "Array is already aligned or distributed");
    bool hadIndirect = realignFlag && data->isDistributed() && data->getAlignRule()->getDistribRule()->hasIndirect();
    bool hasIndirect = dspace->getDistribRule()->hasIndirect();
    for (int i = 0; i < templRank; i++) {
        checkInternal2(rules[i].axisNumber >= -1 && rules[i].axisNumber <= dataRank, "Corrupted align rule");
        Interval templInt = (isArray ? templData->getAxisSpace(i + 1) : dspace->getAxisSpace(i + 1));
        if (rules[i].axisNumber == -1)
            rules[i].setReplicated(1, templInt);
        if (rules[i].axisNumber == 0) {
            checkError2(templInt.contains(rules[i].summand), "Alignment on constant is out of bounds");
            rules[i].setConstant(rules[i].summand);
        }
        if (rules[i].axisNumber > 0) {
            checkError2(rules[i].multiplier != 0, "Linear alignment must have non-zero multiplier");
            Interval dataInt = data->getAxisSpace(rules[i].axisNumber);
            checkError2(templInt.contains(dataInt[0] * rules[i].multiplier + rules[i].summand) &&
                    templInt.contains(dataInt[1] * rules[i].multiplier + rules[i].summand), "Linear alignment is out of bounds");
            rules[i].setLinear(rules[i].axisNumber, rules[i].multiplier, rules[i].summand);
        }
    }
    for (int i = 0; i < dspaceRank; i++) {
        if (!underRule)
            resRules[i] = rules[i];
        else {
            resRules[i] = *underRule->getAxisRule(i + 1);
            resRules[i].composite(rules);
        }
    }
    if (realignFlag && !newValueFlag)
        checkInternal2(!hadIndirect && !hasIndirect, "Realign for indirectly-distributed arrays is not implemented yet");
    DvmhAlignRule *alignRule = new DvmhAlignRule(dataRank, dspace, resRules);
    data->realign(alignRule, ownDistribSpace, newValueFlag);
    dvmh_log(TRACE, "New local part:");
    custom_log(TRACE, blockOut, dataRank, data->getLocalPart());
#ifndef NO_DVM
    if (realignFlag)
        checkInternal2(hadIndirect == hasIndirect, "Transition between block-distribution and indirect-distribution is not implemented yet");
    if (!hasIndirect) {
        if (!realignFlag)
            dvmCreateTiedDA(data);
        bool wasDvmMain = false;
        SysHandle *hndl1 = getDvm(data, &wasDvmMain);
        DvmType *dvmHeader1 = (DvmType *)hndl1->HeaderPtr;
        SysHandle *hndl2 = getDvm(dspace);
        DvmType *dvmHeader2 = (isArray ? (DvmType *)getDvm(templData)->HeaderPtr : (DvmType *)&hndl2);
#ifdef NON_CONST_AUTOS
        DvmType dvmAxes[templRank], dvmCoeffs[templRank], dvmSummands[templRank];
#else
        DvmType dvmAxes[MAX_DISTRIB_SPACE_RANK], dvmCoeffs[MAX_DISTRIB_SPACE_RANK], dvmSummands[MAX_DISTRIB_SPACE_RANK];
#endif
        for (int i = 0; i < templRank; i++) {
            DvmType templStarti = (isArray ? templData->getAxisSpace(i + 1)[0] : dspace->getAxisSpace(i + 1)[0]);
            if (rules[i].axisNumber == -1) {
                dvmAxes[i] = -1;
                dvmCoeffs[i] = 1;
                dvmSummands[i] = 0;
            } else if (rules[i].axisNumber == 0) {
                dvmAxes[i] = 0;
                dvmCoeffs[i] = 0;
                dvmSummands[i] = rules[i].summand - templStarti;
            } else {
                DvmType arrStarti = data->getAxisSpace(rules[i].axisNumber)[0];
                dvmAxes[i] = rules[i].axisNumber;
                dvmCoeffs[i] = rules[i].multiplier;
                dvmSummands[i] = rules[i].summand + rules[i].multiplier * arrStarti - templStarti;
            }
        }
        if (realignFlag) {
            untieDvm(data);
            DvmType tmp = (newValueFlag ? 1 : 0);
            realn_(dvmHeader1, (PatternRef *)dvmHeader2, dvmAxes, dvmCoeffs, dvmSummands, &tmp);
            hndl1 = (SysHandle *)dvmHeader1[0];
            tieDvm(data, hndl1, wasDvmMain);
        } else
            align_(dvmHeader1, (PatternRef *)dvmHeader2, dvmAxes, dvmCoeffs, dvmSummands);
        if (data->hasLocal()) {
#ifdef NON_CONST_AUTOS
            Interval hostPortion[dataRank];
#else
            Interval hostPortion[MAX_ARRAY_RANK];
#endif
            s_DISARRAY *dvmHead = (s_DISARRAY *)hndl1->pP;
            for (int i = 0; i < dataRank; i++) {
                DvmType starti = data->getAxisSpace(i + 1)[0];
                hostPortion[i][0] = dvmHead->ArrBlock.Block.Set[i].Lower + starti;
                hostPortion[i][1] = dvmHead->ArrBlock.Block.Set[i].Upper + starti;
                checkInternal(dvmHead->Block.Set[i].Lower + starti == data->getLocalPart()[i][0]);
                checkInternal(dvmHead->Block.Set[i].Upper + starti == data->getLocalPart()[i][1]);
                checkInternal(hostPortion[i].contains(data->getLocalPlusShadow()[i]));
            }
            bool hadHostRepr = data->getRepr(0) != 0;
            data->setHostPortion(getAddrFromDvm(dvmHead), hostPortion);
            if (!hadHostRepr) {
                data->getRepr(0)->getActualState()->uniteOne(data->getLocalPart());
                data->initActualShadow();
            }
        }
    }
#endif
    if (data->hasLocal()) {
        if (!data->getRepr(0))
            data->createNewRepr(0, data->getLocalPlusShadow());
        if (!realignFlag)
            data->initActual(0);
    }
}

static void dvmhAlignRealignCommon(bool realignFlag, bool newValueFlag, DvmType dvmDesc[], const DvmType templDesc[], DvmType givenTemplRank, va_list &ap,
        bool fortranFlag) {
    const char *call = (realignFlag ? "dvmh_realign" : "dvmh_align");
    checkInternal2(inited, "LibDVMH is not initialized");
    checkError3(!currentRegion && !currentLoop, "Call to %s is not allowed in region or parallel loop", call);
    checkInternal(dvmDesc && templDesc);
    DvmhObject *obj1 = passOrGetOrCreateDvmh(dvmDesc[0], true);
    DvmhObject *obj2 = passOrGetOrCreateDvmh(templDesc[0], true);
    checkError3(obj1 && obj2, "NULL pointer is passed to %s", call);
    checkError3(allObjects.find(obj1) != allObjects.end() && allObjects.find(obj2) != allObjects.end(), "Unknown object is passed to %s", call);
    checkInternal2(obj1->isExactly<DvmhData>(), "Only array can be aligned");
    checkInternal2(obj2->isExactly<DvmhDistribSpace>() || obj2->isExactly<DvmhData>(), "Array can be aligned only on array or template");
    int templRank = obj2->isExactly<DvmhDistribSpace>() ? obj2->as<DvmhDistribSpace>()->getRank() : obj2->as<DvmhData>()->getRank();
    checkInternal2(templRank > 0, "Rank must be positive");
    checkError3(templRank == givenTemplRank, "Rank in %s directive must be the same as in declaration of the %s", (realignFlag ? "realign" : "align"),
            (obj2->isExactly<DvmhDistribSpace>() ? "template" : "array"));
    DvmhData *data = obj1->as<DvmhData>();
#ifdef NON_CONST_AUTOS
    DvmhAxisAlignRule axisRules[templRank];
#else
    DvmhAxisAlignRule axisRules[MAX_DISTRIB_SPACE_RANK];
#endif
    extractArray(ap, templRank, axisRules, (fortranFlag ? vptPointer : vptValue));
    dvmhAlignRealign(realignFlag, data, obj2, axisRules, newValueFlag, false);
}

extern "C" void dvmh_align_C(DvmType dvmDesc[], const DvmType templDesc[], DvmType templRank, /* DvmType alignmentHelper */...) {
    InterfaceFunctionGuard guard;
    va_list ap;
    va_start(ap, templRank);
    dvmhAlignRealignCommon(false, true, dvmDesc, templDesc, templRank, ap, false);
    va_end(ap);
}

extern "C" void dvmh_align_(DvmType dvmDesc[], const DvmType templDesc[], const DvmType *pTemplRank, /* const DvmType *pAlignmentHelper */...) {
    InterfaceFunctionGuard guard;
    checkInternal(pTemplRank);
    va_list ap;
    va_start(ap, pTemplRank);
    dvmhAlignRealignCommon(false, true, dvmDesc, templDesc, *pTemplRank, ap, true);
    va_end(ap);
}

extern "C" void dvmh_realign_C(DvmType dvmDesc[], DvmType newValueFlag, const DvmType templDesc[], DvmType templRank, /* DvmType alignmentHelper */...) {
    InterfaceFunctionGuard guard;
    va_list ap;
    va_start(ap, templRank);
    dvmhAlignRealignCommon(true, newValueFlag != 0, dvmDesc, templDesc, templRank, ap, false);
    va_end(ap);
}

extern "C" void dvmh_realign2_(DvmType dvmDesc[], const DvmType *pNewValueFlag, const DvmType templDesc[], const DvmType *pTemplRank,
        /* const DvmType *pAlignmentHelper */...) {
    InterfaceFunctionGuard guard;
    checkInternal(pNewValueFlag && pTemplRank);
    va_list ap;
    va_start(ap, pTemplRank);
    dvmhAlignRealignCommon(true, *pNewValueFlag != 0, dvmDesc, templDesc, *pTemplRank, ap, true);
    va_end(ap);
}

static void dvmhIndirectShadowAddCommon(DvmType dvmDesc[], DvmType axis, DvmType derivedRhsHelper, DvmType countingHandlerHelper, DvmType fillingHandlerHelper,
        const char shadowName[], DvmType includeCount, va_list &ap) {
    checkInternal2(inited, "LibDVMH is not initialized");
    checkError2(!currentRegion && !currentLoop, "Call to dvmh_indirect_shadow_add is not allowed in region or parallel loop");
    checkInternal(dvmDesc);
    DvmhObject *obj = (DvmhObject *)dvmDesc[0];
    checkError2(obj, "NULL pointer is passed to dvmh_indirect_shadow_add");
    checkError2(allObjects.find(obj) != allObjects.end(), "Unknown object is passed to dvmh_indirect_shadow_add");
    checkInternal2(obj->isExactly<DvmhDistribSpace>(), "Indirect shadow edge can be added only to template");
    DvmhDistribSpace *dspace = obj->as<DvmhDistribSpace>();
    checkInternal2(axis > 0, "Axis must be positive");
    int targetRank = dspace->getRank();
    checkError2(targetRank >= axis, "Rank mismatch");
    checkError2(dspace->getAxisDistribRule(axis)->isIndirect(), "Only indirectly-distributed template axis can be subject to indirect shadow edge addition");
    DerivedAxisInfo derived;
    loadObject(derivedRhsHelper, derived.rhs);
    loadObject(countingHandlerHelper, derived.countingHandler);
    loadObject(fillingHandlerHelper, derived.fillingHandler);
    std::vector<DvmType *> includeList(includeCount);
    for (int i = 0; i < includeCount; i++)
        includeList[i] = va_arg(ap, DvmType *);

    DvmType *derivedBuf = 0;
    UDvmType derivedCount = 0;
    checkDerivedAndFillBuffer(derived, &derivedCount, &derivedBuf);
    dspace->getAxisDistribRule(axis)->asIndirect()->addShadow(derivedBuf, derivedCount, shadowName);
    delete[] derivedBuf;
    for (int i = 0; i < includeCount; i++) {
        checkInternal(includeList[i]);
        DvmhObject *obj = (DvmhObject *)includeList[i][0];
        checkError2(obj, "NULL pointer is passed to dvmh_indirect_shadow_add");
        checkError2(allObjects.find(obj) != allObjects.end(), "Unknown object is passed to dvmh_indirect_shadow_add");
        checkInternal2(obj->isExactly<DvmhData>(), "Indirect shadow edge can be included only to array");
        DvmhData *data = obj->as<DvmhData>();
        checkError2(data->isDistributed(), "Array must be aligned or distributed");
        checkError2(data->getAlignRule()->getDspace() == dspace, "Array must be aligned with the template for which shadow edge is being specified");
        int dataAxis = data->getAlignRule()->getAxisRule(axis)->axisNumber;
        checkError2(dataAxis > 0, "Array alignment must be linear on the axis");
        data->includeIndirectShadow(dataAxis, shadowName);
    }
}

extern "C" void dvmh_indirect_shadow_add_C(DvmType dvmDesc[], DvmType axis, DvmType derivedRhsHelper, DvmType countingHandlerHelper,
        DvmType fillingHandlerHelper, const char shadowName[], DvmType includeCount, /* DvmType dvmDesc[] */...) {
    InterfaceFunctionGuard guard;
    va_list ap;
    va_start(ap, includeCount);
    dvmhIndirectShadowAddCommon(dvmDesc, axis, derivedRhsHelper, countingHandlerHelper, fillingHandlerHelper, shadowName, includeCount, ap);
    va_end(ap);
}

extern "C" void dvmh_indirect_shadow_add_(DvmType dvmDesc[], const DvmType *pAxis, const DvmType *pDerivedRhsHelper, const DvmType *pCountingHandlerHelper,
        const DvmType *pFillingHandlerHelper, const DvmType *pShadowNameStr, const DvmType *pIncludeCount, /* DvmType dvmDesc[] */...) {
    InterfaceFunctionGuard guard;
    checkInternal(pAxis && pDerivedRhsHelper && pCountingHandlerHelper && pFillingHandlerHelper && pShadowNameStr && pIncludeCount);
    va_list ap;
    va_start(ap, pIncludeCount);
    dvmhIndirectShadowAddCommon(dvmDesc, *pAxis, *pDerivedRhsHelper, *pCountingHandlerHelper, *pFillingHandlerHelper, getStr(*pShadowNameStr), *pIncludeCount,
            ap);
    va_end(ap);
}

static void dvmhDeleteObject(DvmhObject *obj, bool keepAutomaticDvm = false) {
    assert(obj);
    DvmhData *data = obj->as_s<DvmhData>();
    DvmhDistribSpace *dspace = obj->as_s<DvmhDistribSpace>();
#ifndef NO_DVM
    bool dvmIsMain = false;
    SysHandle *hndl = getDvm(obj, &dvmIsMain);
    if (hndl) {
        if (!keepAutomaticDvm || isDvmObjectStatic(hndl, true)) {
            ObjectRef tmp = (ObjectRef)hndl;
            delobj_(&tmp);
        }
        if (data) {
            if (data->isDistributed() && data->hasOwnDistribSpace()) {
                // For implicitly created templates delete its LibDVM mirror too
                DvmhDistribSpace *dspace = data->getAlignRule()->getDspace();
                SysHandle *hndl2 = getDvm(dspace);
                if (hndl2 && (!keepAutomaticDvm || isDvmObjectStatic(hndl2, true))) {
                    ObjectRef tmp = (ObjectRef)hndl2;
                    delobj_(&tmp);
                }
                untieDvm(dspace);
            }
        }
    }
    untieDvm(obj);
#endif
    if (dspace && allObjects.find(dspace) != allObjects.end()) {
        dspace->removeRef();
    }
    if (data && !data->isDistributed() && data->getRepr(0)) {
        RegularVar *regVar = 0;
        {
            SpinLockGuard guard(regularVarsLock);
            regVar = dictFind2(regularVars, data->getBuffer(0)->getDeviceAddr());
        }
        if (regVar)
            regVar->setData(0);
    }
    allObjects.erase(obj);
    delete obj;
}

extern "C" void dvmh_array_free_C(DvmType dvmDesc[]) {
    InterfaceFunctionGuard guard;
    checkInternal2(inited, "LibDVMH is not initialized");
    checkError2(!currentRegion && !currentLoop, "Call to dvmh_array_free is not allowed in region or parallel loop");
    checkInternal(dvmDesc);
    DvmhObject *obj = passOrGetOrCreateDvmh(dvmDesc[0], true);
    checkError2(obj, "NULL object is passed to dvmh_array_free");
    checkInternal2(obj->isExactly<DvmhData>(), "Only array can be passed to dvmh_array_free");
    DvmhData *data = obj->as<DvmhData>();
    int rank = data->getRank();
    DynDeclDesc *desc = new DynDeclDesc(rank);
    if (data->getDataType() != DvmhData::dtUnknown)
        desc->typeSize = -data->getDataType();
    else
        desc->typeSize = data->getTypeSize();
    desc->space->blockAssign(rank, data->getSpace());
    for (int i = 0; i < rank; i++)
        desc->shdWidths[i] = data->getShdWidth(i + 1);
    dvmhDeleteObject(data);
    allObjects.insert(desc);
    dvmDesc[0] = (DvmType)desc;
}

extern "C" void dvmh_delete_object_(DvmType dvmDesc[]) {
    InterfaceFunctionGuard guard;
    checkInternal2(inited, "LibDVMH is not initialized");
    checkInternal(dvmDesc);
    DvmhObject *obj = (DvmhObject *)dvmDesc[0];
    checkError2(obj, "NULL object is passed to dvmh_delete_object");
    if (allObjects.find(obj) == allObjects.end()) {
        checkError2(!noLibdvm, "Unknown object is passed to dvmh_delete_object");
        DvmhObject *obj2 = getDvmh(dvmDesc[0]);
        if (obj2)
            obj = obj2;
    }
    if (allObjects.find(obj) != allObjects.end()) {
        dvmhDeleteObject(obj);
        dvmDesc[0] = 0;
    } else {
#ifndef NO_DVM
        ObjectRef tmp = (ObjectRef)dvmDesc[0];
        delobj_(&tmp);
#endif
    }
}

extern "C" void dvmh_forget_header_(DvmType dvmDesc[]) {
    InterfaceFunctionGuard guard;
    checkInternal2(inited, "LibDVMH is not initialized");
    checkInternal(dvmDesc);
    DvmhObject *obj = passOrGetOrCreateDvmh(dvmDesc[0], true);
    if (obj) {
        if (obj->is<DvmhData>()) {
            if (obj->as<DvmhData>()->hasHeader(dvmDesc))
                obj->as<DvmhData>()->removeHeader(dvmDesc);
            else
                checkError2(false, "Memory leak detected. Consider freeing dynamically allocated memory beforehand.");
        } else if (obj->is<DynDeclDesc>()) {
            allObjects.erase(obj);
            delete obj;
        } else {
            checkInternal2(false, "Only array can be passed to dvmh_forget_header");
        }
    }
    dvmDesc[0] = 0;
}

static std::set<const void *> autoDataExit;

static DvmhData *getOrCreateData(const void *addr, int rank, DvmType typeSize, Interval indexes[]) {
    RegularVar *regVar = 0;
    {
        SpinLockGuard guard(regularVarsLock);
        regVar = dictFind2(regularVars, addr);
    }
    if (!regVar && ((currentRegion && currentRegion->getPhase() == DvmhRegion::rpRegistrations) ||
            (!currentRegion && currentLoop && currentLoop->getPhase() == DvmhLoop::lpRegistrations))) {
        // Automatic data region enter for such variables
        dvmh_data_enter_C(addr, 0);
        autoDataExit.insert(addr);
        {
            SpinLockGuard guard(regularVarsLock);
            regVar = dictFind2(regularVars, addr);
        }
    }
    checkInternal2(regVar, "Unknown regular variable");
    DvmhData::DataType dataType;
    DvmhData::TypeType typeType;
    UDvmType realTypeSize;
    decomposeTypeSize(typeSize, &dataType, &typeType, &realTypeSize);
    DvmhData *data = regVar->getData();
    regVar->expandSize(realTypeSize * indexes->blockSize(rank));
    if (rank > 0)
        indexes[0][1] = indexes[0][0] + (regVar->getSize() / (realTypeSize * (indexes + 1)->blockSize(rank - 1))) - 1;
    if (data) {
        static const char msg[] = "Regular variable configuration has been changed";
        checkError2(realTypeSize == data->getTypeSize(), msg);
        checkError2(dataType == DvmhData::dtUnknown || data->getDataType() == DvmhData::dtUnknown || dataType == data->getDataType(), msg);
        checkError2(rank == data->getRank(), msg);
        if (rank > 0) {
            checkError2(indexes[0][0] == data->getAxisSpace(1)[0] && (indexes[0].empty() || data->isIncomplete()
                    || indexes[0][1] == data->getAxisSpace(1)[1]), msg);
            if (data->isIncomplete() && !indexes[0].empty())
                data->expandIncomplete(indexes[0][1]);
        }
        for (int i = 1; i < rank; i++)
            checkError2(indexes[i] == data->getAxisSpace(i + 1), msg);
    } else {
        if (dataType != DvmhData::dtUnknown)
            data = new DvmhData(dataType, rank, indexes);
        else
            data = new DvmhData(realTypeSize, typeType, rank, indexes);
        data->realign(0);
        data->setHostPortion((void *)addr, indexes);
        data->initActual(0);
        regVar->setData(data);
        {
            SpinLockGuard guard(regularVarsLock);
            std::map<const void *, RegularVar *>::iterator it1, it = regularVars.find(addr);
            if (it != regularVars.begin()) {
                it1 = it;
                it1--;
                if (it1 != regularVars.end() && it1->second->getData())
                    checkError2(!it1->second->getData()->getBuffer(0)->overlaps(data->getBuffer(0)),
                            "Overlapping of regular variables detected. Maybe part of array is used as stand-alone array.");
            }
            it1 = it;
            it1++;
            if (it1 != regularVars.end() && it1->second->getData())
                checkError2(!it1->second->getData()->getBuffer(0)->overlaps(data->getBuffer(0)),
                        "Overlapping of regular variables detected. Maybe part of array is used as stand-alone array.");
        }
        allObjects.insert(data);
        data->createHeader();
    }
    assert(data);
    return data;
}

extern "C" DvmType *dvmh_variable_gen_header_C(const void *addr, DvmType rank, DvmType typeSize, /* DvmType axisSize */...) {
    checkInternal2(inited, "LibDVMH is not initialized");
    checkError2(addr, "NULL pointer is passed to dvmh_variable_gen_header");
    checkInternal2(rank >= 0, "Invalid rank is passed to dvmh_variable_gen_header");
#ifdef NON_CONST_AUTOS
    Interval indexes[rank];
#else
    Interval indexes[MAX_ARRAY_RANK];
#endif
    if (rank > 0) {
        va_list ap;
        va_start(ap, typeSize);
        for (int i = 0; i < rank; i++) {
            indexes[i][0] = 0;
            indexes[i][1] = extractValue(ap, vptValue) - 1;
        }
        va_end(ap);
    }
    return getOrCreateData(addr, rank, typeSize, indexes)->getAnyHeader();
}

extern "C" void dvmh_variable_fill_header_(DvmType dvmDesc[], const void *baseAddr, const void *addr, const DvmType *pRank, const DvmType *pTypeSize,
        /* const DvmType *pSpaceLow, const DvmType *pSpaceHigh */...) {
    InterfaceFunctionGuard guard;
    checkInternal2(inited, "LibDVMH is not initialized");
    checkInternal(dvmDesc && pRank && pTypeSize);
    checkError2(addr, "NULL pointer is passed to dvmh_variable_fill_header");
    int rank = *pRank;
    checkInternal2(rank >= 0, "Invalid rank is passed to dvmh_variable_fill_header");
#ifdef NON_CONST_AUTOS
    Interval indexes[rank];
#else
    Interval indexes[MAX_ARRAY_RANK];
#endif
    if (rank > 0) {
        va_list ap;
        va_start(ap, pTypeSize);
        extractArray(ap, rank, indexes, vptPointer);
        va_end(ap);
    }
    DvmhData *data = getOrCreateData(addr, rank, *pTypeSize, indexes);
    data->addHeader(dvmDesc, baseAddr);
}

extern "C" DvmType dvmh_variable_gen_header_(const void *addr, const DvmType *pRank, const DvmType *pTypeSize,
        /* const DvmType *pSpaceLow, const DvmType *pSpaceHigh */...) {
    checkInternal2(inited, "LibDVMH is not initialized");
    checkInternal(pRank && pTypeSize);
    checkError2(addr, "NULL pointer is passed to dvmh_variable_gen_header");
    int rank = *pRank;
    checkInternal2(rank >= 0, "Invalid rank is passed to dvmh_variable_gen_header");
#ifdef NON_CONST_AUTOS
    Interval indexes[rank];
#else
    Interval indexes[MAX_ARRAY_RANK];
#endif
    if (rank > 0) {
        va_list ap;
        va_start(ap, pTypeSize);
        extractArray(ap, rank, indexes, vptPointer);
        va_end(ap);
    }
    return getOrCreateData(addr, rank, *pTypeSize, indexes)->getAnyHeader()[0];
}

extern "C" DvmType *dvmh_variable_get_header_C(const void *addr) {
    checkInternal2(inited, "LibDVMH is not initialized");
    checkError2(addr, "NULL pointer is passed to dvmh_variable_get_header");
    RegularVar *regVar = 0;
    {
        SpinLockGuard guard(regularVarsLock);
        regVar = dictFind2(regularVars, addr);
    }
    DvmhData *data = (regVar ? regVar->getData() : 0);
    checkInternal2(data, "Unknown variable is passed to dvmh_variable_get_header");
    return data->getAnyHeader();
}

extern "C" void dvmh_data_enter_C(const void *addr, DvmType size) {
    checkInternal2(inited, "LibDVMH is not initialized");
    checkInternal2(size >= 0, "Negative size is passed to dvmh_data_enter");
    checkError2(addr, "NULL pointer is passed to dvmh_data_enter");
    RegularVar *regVar = 0;
    {
        SpinLockGuard guard(regularVarsLock);
        regVar = dictFind2(regularVars, addr);
        if (!regVar) {
            regVar = new RegularVar(false, size);
            regularVars.insert(std::make_pair(addr, regVar));
        } else {
            regVar->expandSize(size);
        }
    }
    assert(regVar);
    regVar->dataRegionEnter();
    dvmh_log(TRACE, "Variable at %p with size " DTFMT " entered data region. Depth is " UDTFMT ".", addr, size, regVar->getDataRegionDepth());
}

extern "C" void dvmh_data_enter_(const void *addr, const DvmType *pSize) {
    checkInternal(pSize);
    dvmh_data_enter_C(addr, *pSize);
}

extern "C" void dvmh_data_exit_C(const void *addr, DvmType saveFlag) {
    checkInternal2(inited, "LibDVMH is not initialized");
    checkError2(addr, "NULL pointer is passed to dvmh_data_exit");
    RegularVar *regVar = 0;
    {
        SpinLockGuard guard(regularVarsLock);
        regVar = dictFind2(regularVars, addr);
    }
    checkError2(regVar, "Unknown variable is passed to dvmh_data_exit");
    checkError2(regVar->getDataRegionDepth() > 0, "Data region enter/exit imbalance detected");
    regVar->dataRegionExit();
    dvmh_log(TRACE, "Variable at %p with size " UDTFMT " exited data region with save flag %d. Depth is " UDTFMT ".", addr, regVar->getSize(), (int)saveFlag,
            regVar->getDataRegionDepth());
    if (regVar->getDataRegionDepth() == 0) {
        DvmhData *data = regVar->getData();
        if (data) {
            // TODO: Check somehow that we are not in parallel loop now
            data->syncAllAccesses();
            if (saveFlag)
                data->getActual(data->getLocalPlusShadow(), true);
            dvmhDeleteObject(data);
        }
        delete regVar;
        {
            SpinLockGuard guard(regularVarsLock);
            regularVars.erase(addr);
        }
    }
}

extern "C" void dvmh_data_exit_(const void *addr, const DvmType *pSaveFlag) {
    checkInternal(pSaveFlag);
    dvmh_data_exit_C(addr, *pSaveFlag);
}

extern "C" void *dvmh_malloc_C(size_t size) {
    void *res = malloc(size);
    dvmh_log(TRACE, "Malloc'd " UDTFMT " bytes at %p", (UDvmType)size, res);
    if (res) {
        {
            SpinLockGuard guard(regularVarsLock);
            checkInternal2(regularVars.find(res) == regularVars.end(), "Variable tracking error");
        }
        dvmh_data_enter_C(res, size);
    }
    return res;
}

extern "C" void *dvmh_calloc_C(size_t nmemb, size_t size) {
    void *res = calloc(nmemb, size);
    if (res) {
        {
            SpinLockGuard guard(regularVarsLock);
            checkInternal2(regularVars.find(res) == regularVars.end(), "Variable tracking error");
        }
        dvmh_data_enter_C(res, nmemb * size);
    }
    return res;
}

extern "C" void *dvmh_realloc_C(void *ptr, size_t size) {
    bool found = false;
    {
        SpinLockGuard guard(regularVarsLock);
        found = regularVars.find(ptr) != regularVars.end();
    }
    if (found)
        dvmh_data_exit_C(ptr, 1);
    else
        dvmh_log(DEBUG, "Unknown pointer passed to dvmh_realloc");
    {
        SpinLockGuard guard(regularVarsLock);
        checkInternal2(regularVars.find(ptr) == regularVars.end(), "Dynamically allocated variable enter/exit imbalance");
    }
    void *res = realloc(ptr, size);
    if (res) {
        {
            SpinLockGuard guard(regularVarsLock);
            checkInternal2(regularVars.find(res) == regularVars.end(), "Variable tracking error");
        }
        dvmh_data_enter_C(res, size);
    }
    return res;
}

extern "C" char *dvmh_strdup_C(const char *s) {
    char *res = strdup(s);
    if (res) {
        {
            SpinLockGuard guard(regularVarsLock);
            checkInternal2(regularVars.find(res) == regularVars.end(), "Variable tracking error");
        }
        dvmh_data_enter_C(res, strlen(res) + 1);
    }
    return res;
}

extern "C" void dvmh_free_C(void *ptr) {
    bool found = false;
    {
        SpinLockGuard guard(regularVarsLock);
        found = regularVars.find(ptr) != regularVars.end();
    }
    if (found)
        dvmh_data_exit_C(ptr, 0);
    else
        dvmh_log(DEBUG, "Unknown pointer %p passed to dvmh_free", ptr);
    {
        SpinLockGuard guard(regularVarsLock);
        checkInternal2(regularVars.find(ptr) == regularVars.end(), "Dynamically allocated variable enter/exit imbalance");
    }
    free(ptr);
    dvmh_log(TRACE, "Free'd at %p", ptr);
}


struct ArraySection {
    std::vector<Interval> bounds;
    std::vector<DvmType> steps;
};

static void loadObject(int idx, ArraySection &section) {
    idx--;
    int rank = savedParameters[idx++].intValue;
    section.bounds.resize(rank);
    section.steps.resize(rank);
    for (int i = 0; i < rank; i++) {
        section.bounds[i][0] = savedParameters[idx++].intValue;
        section.bounds[i][1] = savedParameters[idx++].intValue;
        section.steps[i] = savedParameters[idx++].intValue;
    }
}

static DvmType dvmhArraySliceCommon(int rank, va_list &ap, bool fortranFlag) {
    checkInternal(rank >= 0);
    int res = saveParameter(rank);
    for (int i = 0; i < rank; i++) {
        DvmType start, end, step;
        start = extractValue(ap, fortranFlag ? vptPointer : vptValue);
        end = extractValue(ap, fortranFlag ? vptPointer : vptValue);
        step = extractValue(ap, fortranFlag ? vptPointer : vptValue);
        checkError2(start <= end, "Starting index must be not less than ending");
        checkError2(step >= 1, "Step must be positive");
        checkError2((end + 1 - start) % step == 0, "Step must be a divisor of (end + 1 - start)");
        saveParameter(start);
        saveParameter(end);
        saveParameter(step);
    }
    return res;
}

extern "C" DvmType dvmh_array_slice_C(DvmType rank, /* DvmType start, DvmType end, DvmType step */...) {
    va_list ap;
    va_start(ap, rank);
    DvmType res = dvmhArraySliceCommon(rank, ap, false);
    va_end(ap);
    return res;
}

extern "C" DvmType dvmh_array_slice_(const DvmType *pRank, /* const DvmType *pStart, const DvmType *pEnd, const DvmType *pStep */...) {
    checkInternal(pRank);
    va_list ap;
    va_start(ap, pRank);
    DvmType res = dvmhArraySliceCommon(*pRank, ap, true);
    va_end(ap);
    return res;
}

extern "C" void dvmh_array_copy_whole_(const DvmType srcDvmDesc[], DvmType dstDvmDesc[]) {
    checkInternal(srcDvmDesc && dstDvmDesc);
    DvmhObject *obj = passOrGetOrCreateDvmh(srcDvmDesc[0], true);
    checkError2(obj, "NULL object is passed to dvmh_array_copy_whole");
    checkInternal2(obj->isExactly<DvmhData>(), "Only array can be passed to dvmh_array_copy_whole");
    DvmhData *srcData = obj->as<DvmhData>();
    obj = passOrGetOrCreateDvmh(dstDvmDesc[0], true);
    checkError2(obj, "NULL object is passed to dvmh_array_copy_whole");
    checkInternal2(obj->isExactly<DvmhData>(), "Only array can be passed to dvmh_array_copy_whole");
    DvmhData *dstData = obj->as<DvmhData>();
    dvmhCopyArrayWhole(srcData, dstData);
}

extern "C" void dvmh_array_copy_C(const DvmType srcDvmDesc[], DvmType srcSliceHelper, DvmType dstDvmDesc[], DvmType dstSliceHelper) {
    InterfaceFunctionGuard guard;
    checkInternal(srcDvmDesc && dstDvmDesc);
    DvmhObject *obj = passOrGetDvmh(srcDvmDesc[0]);
    checkError2(obj, "NULL object is passed to dvmh_array_copy");
    checkInternal2(obj->isExactly<DvmhData>(), "Only array can be passed to dvmh_array_copy");
    DvmhData *srcData = obj->as<DvmhData>();
    obj = passOrGetDvmh(dstDvmDesc[0]);
    checkError2(obj, "NULL object is passed to dvmh_array_copy");
    checkInternal2(obj->isExactly<DvmhData>(), "Only array can be passed to dvmh_array_copy");
    DvmhData *dstData = obj->as<DvmhData>();
    ArraySection srcSection, dstSection;
    loadObject(srcSliceHelper, srcSection);
    loadObject(dstSliceHelper, dstSection);
    int srcRank = srcData->getRank();
    int dstRank = dstData->getRank();
    checkError2((int)srcSection.bounds.size() == srcRank, "Dimension mismatch for source array");
    checkError2((int)dstSection.bounds.size() == dstRank, "Dimension mismatch for destination array");
    std::vector<DvmType> sizes;
    std::vector<int> srcAxes;
#ifdef NON_CONST_AUTOS
    Interval srcBlock[srcRank], dstBlock[dstRank];
    DvmType srcSteps[srcRank], dstSteps[dstRank];
    int dstAxisToSrc[dstRank];
#else
    Interval srcBlock[MAX_ARRAY_RANK], dstBlock[MAX_ARRAY_RANK];
    DvmType srcSteps[MAX_ARRAY_RANK], dstSteps[MAX_ARRAY_RANK];
    int dstAxisToSrc[MAX_ARRAY_RANK];
#endif
    for (int i = 0; i < srcRank; i++) {
        srcBlock[i] = srcSection.bounds[i];
        srcSteps[i] = srcSection.steps[i];
        checkError2(srcData->getAxisSpace(i + 1).contains(srcBlock[i]), "Slice specification is out of bounds");
        if (srcBlock[i].size() > 1) {
            sizes.push_back((srcBlock[i].size() - 1) / srcSteps[i] + 1);
            srcAxes.push_back(i + 1);
        }
    }
    int sizesIndex = 0;
    for (int i = 0; i < dstRank; i++) {
        dstBlock[i] = dstSection.bounds[i];
        dstSteps[i] = dstSection.steps[i];
        checkError2(dstData->getAxisSpace(i + 1).contains(dstBlock[i]), "Slice specification is out of bounds");
        if (dstBlock[i].size() > 1) {
            checkError2(sizesIndex < (int)sizes.size(), "Configurations mismatch");
            checkError2(sizes[sizesIndex] == ((DvmType)dstBlock[i].size() - 1) / dstSteps[i] + 1, "Configurations mismatch");
            dstAxisToSrc[i] = srcAxes[sizesIndex];
            sizesIndex++;
        } else {
            dstAxisToSrc[i] = -1;
        }
    }
    checkError2(sizesIndex == (int)sizes.size(), "Configurations mismatch");
    dvmhCopyArrayArray(srcData, srcBlock, srcSteps, dstData, dstBlock, dstSteps, dstAxisToSrc);
}

extern "C" void dvmh_array_copy_(const DvmType srcDvmDesc[], DvmType *pSrcSliceHelper, DvmType dstDvmDesc[], DvmType *pDstSliceHelper) {
    InterfaceFunctionGuard guard;
    checkInternal(pSrcSliceHelper && pDstSliceHelper);
    dvmh_array_copy_C(srcDvmDesc, *pSrcSliceHelper, dstDvmDesc, *pDstSliceHelper);
}


extern "C" void *dvmh_get_natural_base_C(DvmType deviceNum, const DvmType dvmDesc[]) {
    checkInternal2(inited, "LibDVMH is not initialized");
    checkInternal3(deviceNum >= 0 && deviceNum < devicesCount, "Illegal device number is passed to dvmh_get_natural_base (" DTFMT ")", deviceNum);
    checkInternal(dvmDesc);
    DvmhObject *obj = passOrGetDvmh(dvmDesc[0]);
    checkError2(obj, "NULL object is passed to dvmh_get_natural_base");
    checkInternal2(obj->isExactly<DvmhData>(), "Only array can be passed to dvmh_get_natural_base");
    DvmhData *data = obj->as<DvmhData>();
    DvmhBuffer *buf = data->getBuffer(deviceNum);
    if (buf)
        return buf->getNaturalBase(true);
    else
        return 0;
}

extern "C" void *dvmh_get_device_addr_C(DvmType deviceNum, const void *addr) {
    checkInternal2(inited, "LibDVMH is not initialized");
    checkInternal3(deviceNum >= 0 && deviceNum < devicesCount, "Illegal device number is passed to dvmh_get_natural_base (" DTFMT ")", deviceNum);
    if (deviceNum == 0) {
        return (void *)addr;
    } else {
        RegularVar *regVar = 0;
        {
            SpinLockGuard guard(regularVarsLock);
            regVar = dictFind2(regularVars, addr);
        }
        DvmhData *data = (regVar ? regVar->getData() : 0);
        checkInternal2(data, "Can not get device address of unknown variable");
        DvmhBuffer *buf = data->getBuffer(deviceNum);
        if (buf)
            return buf->getDeviceAddr();
        else
            return 0;
    }
}

extern "C" DvmType dvmh_fill_header_C(DvmType deviceNum, const void *baseAddr, const DvmType dvmDesc[], DvmType devHeader[], DvmType extendedParams[]) {
    InterfaceFunctionGuard guard;
    checkInternal2(inited, "LibDVMH is not initialized");
    checkInternal3(deviceNum >= 0 && deviceNum < devicesCount, "Illegal device number is passed to dvmh_fill_header (" DTFMT ")", deviceNum);
    checkInternal(dvmDesc && devHeader);
    DvmhObject *obj = passOrGetDvmh(dvmDesc[0]);
    checkError2(obj, "NULL object is passed to dvmh_fill_header");
    checkInternal2(obj->isExactly<DvmhData>(), "Only array can be passed to dvmh_fill_header");
    DvmhData *data = obj->as<DvmhData>();
    DvmhBuffer *buf = data->getBuffer(deviceNum);
    int res = 0;
    if (buf) {
        DvmhDiagonalInfo diagInfo;
        buf->fillHeader(baseAddr, devHeader, true, &diagInfo);
        int transType = buf->isTransformed() + buf->isDiagonalized();
        res = transType;
        if (extendedParams) {
            if (transType == 2) {
                extendedParams[0] = diagInfo.x_axis;
                extendedParams[1] = diagInfo.x_first;
                extendedParams[2] = diagInfo.x_length;
                extendedParams[3] = diagInfo.y_axis;
                extendedParams[4] = diagInfo.y_first;
                extendedParams[5] = diagInfo.y_length;
                extendedParams[6] = diagInfo.slashFlag;
            }
        }
    } else {
        fillHeader(data->getRank(), data->getTypeSize(), baseAddr, baseAddr, 0, 0, devHeader);
        devHeader[0] = (DvmType)data;
    }
    return res;
}

extern "C" DvmType dvmh_fill_header2_(const DvmType *pDeviceNum, const void *baseAddr, const DvmType dvmDesc[], DvmType devHeader[]) {
    InterfaceFunctionGuard guard;
    checkInternal(pDeviceNum);
    return dvmh_fill_header_C(*pDeviceNum, baseAddr, dvmDesc, devHeader, 0);
}

extern "C" DvmType dvmh_fill_header_ex2_(const DvmType *pDeviceNum, const void *baseAddr, const DvmType dvmDesc[], DvmType devHeader[],
        DvmType extendedParams[]) {
    InterfaceFunctionGuard guard;
    checkInternal(pDeviceNum);
    return dvmh_fill_header_C(*pDeviceNum, baseAddr, dvmDesc, devHeader, extendedParams);
}

static void dvmhGetActualReal(DvmhData *data, const Interval indexes[]) {
    data->syncWriteAccesses();
    PushCurrentPurpose purpose(DvmhCopyingPurpose::dcpGetActual);
    data->getActual(indexes, (currentRegion ? currentRegion->canAddToActual(data, indexes) : true));
}

static void dvmhGetActualInternal(DvmhData *data, DvmType givenRank, va_list &ap, bool fortranFlag) {
    int rank = data->getRank();
    checkError2(rank == givenRank, "Rank in get_actual directive must be the same as in declaration of the variable");
#ifdef NON_CONST_AUTOS
    Interval indexes[rank];
#else
    Interval indexes[MAX_ARRAY_RANK];
#endif
    extractArray(ap, rank, indexes, (fortranFlag ? vptPointer : vptValue));
    if (makeBlockReal(rank, data->getSpace(), indexes)) {
        checkError2(data->getSpace()->blockContains(rank, indexes), "Index out of bounds");
        indexes->blockIntersectInplace(rank, data->getLocalPlusShadow());
        dvmhGetActualReal(data, indexes);
    }
}

static void dvmhGetActualSubvariableCommon(void *addr, DvmType rank, va_list &ap, bool fortranFlag) {
    checkInternal2(inited, "LibDVMH is not initialized");
    checkError2(addr, "NULL pointer is passed to dvmh_get_actual_subvariable");
    RegularVar *regVar = 0;
    {
        SpinLockGuard guard(regularVarsLock);
        regVar = dictFind2(regularVars, addr);
    }
    DvmhData *data = (regVar ? regVar->getData() : 0);
    if (data)
        dvmhGetActualInternal(data, rank, ap, fortranFlag);
}

extern "C" void dvmh_get_actual_subvariable_C(void *addr, DvmType rank, /* DvmType indexLow, DvmType indexHigh */...) {
    InterfaceFunctionGuard guard;
    va_list ap;
    va_start(ap, rank);
    dvmhGetActualSubvariableCommon(addr, rank, ap, false);
    va_end(ap);
}

extern "C" void dvmh_get_actual_subvariable2_(void *addr, const DvmType *pRank, /* const DvmType *pIndexLow, const DvmType *pIndexHigh */...) {
    InterfaceFunctionGuard guard;
    checkInternal(pRank);
    va_list ap;
    va_start(ap, pRank);
    dvmhGetActualSubvariableCommon(addr, *pRank, ap, true);
    va_end(ap);
}

extern "C" void dvmh_get_actual_variable2_(void *addr) {
    InterfaceFunctionGuard guard;
    checkInternal2(inited, "LibDVMH is not initialized");
    checkError2(addr, "NULL pointer is passed to dvmh_get_actual_variable");
    RegularVar *regVar = 0;
    {
        SpinLockGuard guard(regularVarsLock);
        regVar = dictFind2(regularVars, addr);
    }
    DvmhData *data = (regVar ? regVar->getData() : 0);
    if (data)
        dvmhGetActualReal(data, data->getLocalPlusShadow());
}

static void dvmhGetActualSubarrayCommon(const DvmType dvmDesc[], DvmType rank, va_list &ap, bool fortranFlag) {
    checkInternal2(inited, "LibDVMH is not initialized");
    checkInternal(dvmDesc);
    DvmhObject *obj = passOrGetOrCreateDvmh(dvmDesc[0], true);
    checkError2(obj, "NULL object is passed to dvmh_get_actual_subarray");
    checkInternal2(obj->isExactly<DvmhData>(), "Only array can be passed to dvmh_get_actual_subarray");
    DvmhData *data = obj->as<DvmhData>();
    dvmhGetActualInternal(data, rank, ap, fortranFlag);
}

extern "C" void dvmh_get_actual_subarray_C(const DvmType dvmDesc[], DvmType rank, /* DvmType indexLow, DvmType indexHigh */...) {
    InterfaceFunctionGuard guard;
    va_list ap;
    va_start(ap, rank);
    dvmhGetActualSubarrayCommon(dvmDesc, rank, ap, false);
    va_end(ap);
}

extern "C" void dvmh_get_actual_subarray2_(const DvmType dvmDesc[], const DvmType *pRank, /* const DvmType *pIndexLow, const DvmType *pIndexHigh */...) {
    InterfaceFunctionGuard guard;
    checkInternal(pRank);
    va_list ap;
    va_start(ap, pRank);
    dvmhGetActualSubarrayCommon(dvmDesc, *pRank, ap, true);
    va_end(ap);
}

extern "C" void dvmh_get_actual_array2_(const DvmType dvmDesc[]) {
    InterfaceFunctionGuard guard;
    checkInternal2(inited, "LibDVMH is not initialized");
    checkInternal(dvmDesc);
    DvmhObject *obj = passOrGetOrCreateDvmh(dvmDesc[0], true);
    checkError2(obj, "NULL object passed to dvmh_get_actual_array");
    checkInternal2(obj->isExactly<DvmhData>(), "Only array can be passed to dvmh_get_actual_array");
    DvmhData *data = obj->as<DvmhData>();
    dvmhGetActualReal(data, data->getLocalPlusShadow());
}

extern "C" void dvmh_get_actual_all2_() {
    InterfaceFunctionGuard guard;
    checkInternal2(inited, "LibDVMH is not initialized");
    for (std::set<DvmhObject *>::iterator it = allObjects.begin(); it != allObjects.end(); it++) {
        DvmhData *data = (*it)->as_s<DvmhData>();
        if (data)
            dvmhGetActualReal(data, data->getLocalPlusShadow());
    }
}

static void dvmhActualReal(DvmhData *data, const Interval indexes[]) {
    data->syncAllAccesses();
    data->setActual(indexes);
    if (currentRegion)
        currentRegion->markToRenew(data);
}

static void dvmhActualInternal(DvmhData *data, DvmType givenRank, va_list &ap, bool fortranFlag) {
    int rank = data->getRank();
    checkError2(rank == givenRank, "Rank in actual directive must be the same as in declaration of the variable");
#ifdef NON_CONST_AUTOS
    Interval indexes[rank];
#else
    Interval indexes[MAX_ARRAY_RANK];
#endif
    extractArray(ap, rank, indexes, (fortranFlag ? vptPointer : vptValue));
    if (makeBlockReal(rank, data->getSpace(), indexes)) {
        checkError2(data->getSpace()->blockContains(rank, indexes), "Index out of bounds");
        indexes->blockIntersectInplace(rank, data->getLocalPart());
        dvmhActualReal(data, indexes);
    }
}

static void dvmhActualSubvariableCommon(const void *addr, DvmType rank, va_list &ap, bool fortranFlag) {
    checkInternal2(inited, "LibDVMH is not initialized");
    checkError2(addr, "NULL pointer is passed to dvmh_actual_subvariable");
    RegularVar *regVar = 0;
    {
        SpinLockGuard guard(regularVarsLock);
        regVar = dictFind2(regularVars, addr);
    }
    DvmhData *data = (regVar ? regVar->getData() : 0);
    if (data)
        dvmhActualInternal(data, rank, ap, fortranFlag);
}

extern "C" void dvmh_actual_subvariable_C(const void *addr, DvmType rank, /* DvmType indexLow, DvmType indexHigh */...) {
    InterfaceFunctionGuard guard;
    va_list ap;
    va_start(ap, rank);
    dvmhActualSubvariableCommon(addr, rank, ap, false);
    va_end(ap);
}

extern "C" void dvmh_actual_subvariable2_(const void *addr, const DvmType *pRank, /* const DvmType *pIndexLow, const DvmType *pIndexHigh */...) {
    InterfaceFunctionGuard guard;
    checkInternal(pRank);
    va_list ap;
    va_start(ap, pRank);
    dvmhActualSubvariableCommon(addr, *pRank, ap, true);
    va_end(ap);
}

extern "C" void dvmh_actual_variable2_(const void *addr) {
    InterfaceFunctionGuard guard;
    checkInternal2(inited, "LibDVMH is not initialized");
    checkError2(addr, "NULL pointer is passed to dvmh_actual_variable");
    RegularVar *regVar = 0;
    {
        SpinLockGuard guard(regularVarsLock);
        regVar = dictFind2(regularVars, addr);
    }
    DvmhData *data = (regVar ? regVar->getData() : 0);
    if (data)
        dvmhActualReal(data, data->getLocalPart());
}

static void dvmhActualSubarrayCommon(const DvmType dvmDesc[], DvmType rank, va_list &ap, bool fortranFlag) {
    checkInternal2(inited, "LibDVMH is not initialized");
    checkInternal(dvmDesc);
    DvmhObject *obj = passOrGetOrCreateDvmh(dvmDesc[0], true);
    checkError2(obj, "NULL object is passed to dvmh_actual_subarray");
    checkInternal2(obj->isExactly<DvmhData>(), "Only array can be passed to dvmh_actual_subarray");
    DvmhData *data = obj->as<DvmhData>();
    dvmhActualInternal(data, rank, ap, fortranFlag);
}

extern "C" void dvmh_actual_subarray_C(const DvmType dvmDesc[], DvmType rank, /* DvmType indexLow, DvmType indexHigh */...) {
    InterfaceFunctionGuard guard;
    va_list ap;
    va_start(ap, rank);
    dvmhActualSubarrayCommon(dvmDesc, rank, ap, false);
    va_end(ap);
}

extern "C" void dvmh_actual_subarray2_(const DvmType dvmDesc[], const DvmType *pRank, /* const DvmType *pIndexLow, const DvmType *pIndexHigh */...) {
    InterfaceFunctionGuard guard;
    checkInternal(pRank);
    va_list ap;
    va_start(ap, pRank);
    dvmhActualSubarrayCommon(dvmDesc, *pRank, ap, false);
    va_end(ap);
}

extern "C" void dvmh_actual_array2_(const DvmType dvmDesc[]) {
    InterfaceFunctionGuard guard;
    checkInternal2(inited, "LibDVMH is not initialized");
    checkInternal(dvmDesc);
    DvmhObject *obj = passOrGetOrCreateDvmh(dvmDesc[0], true);
    checkError2(obj, "NULL object is passed to dvmh_actual_array");
    checkInternal2(obj->isExactly<DvmhData>(), "Only array can be passed to dvmh_actual_array");
    DvmhData *data = obj->as<DvmhData>();
    dvmhActualReal(data, data->getLocalPart());
}

extern "C" void dvmh_actual_all2_() {
    InterfaceFunctionGuard guard;
    checkInternal2(inited, "LibDVMH is not initialized");
    for (std::set<DvmhObject *>::iterator it = allObjects.begin(); it != allObjects.end(); it++) {
        DvmhData *data = (*it)->as_s<DvmhData>();
        if (data)
            dvmhActualReal(data, data->getLocalPart());
    }
}

#ifndef NO_DVM
static ShadowGroupRef dvmCreateShG(const DvmhShadow &shadow, bool createStatic = true) {
    DvmType tmpVar = (createStatic ? 1 : 0);
    ShadowGroupRef bg = crtshg_(&tmpVar);
    for (int i = 0; i < shadow.dataCount(); i++) {
        const DvmhShadowData &sdata = shadow.getData(i);
        DvmhData *data = sdata.data;
        int rank = data->getRank();
        SysHandle *dvmHandle = getDvm(data);
        if (dvmHandle) {
            DvmType *dvmHeader = (DvmType *)dvmHandle->HeaderPtr;
#ifdef NON_CONST_AUTOS
            DvmType lowerBounds[rank], upperBounds[rank];
#else
            DvmType lowerBounds[MAX_ARRAY_RANK], upperBounds[MAX_ARRAY_RANK];
#endif
            for (int i = 0; i < rank; i++) {
                lowerBounds[i] = sdata.shdWidths[i][0];
                upperBounds[i] = sdata.shdWidths[i][1];
            }
            tmpVar = (sdata.cornerFlag ? 1 : 0);
            inssh_(&bg, dvmHeader, lowerBounds, upperBounds, &tmpVar);
        }
    }
    return bg;
}

static void dvmShadowRenew(const DvmhShadow &shadow) {
    if (!shadow.empty()) {
        ShadowGroupRef bg = dvmCreateShG(shadow);
        strtsh_(&bg);
        waitsh_(&bg);
        delshg_(&bg);
    }
}
#endif

static void dvmhShadowRenew(DvmhData *data, bool cornerFlag, ShdWidth widths[] = 0) {
    DvmhShadowData sdata;
    sdata.data = data;
    sdata.cornerFlag = cornerFlag;
    sdata.shdWidths = new ShdWidth[data->getRank()];
    if (widths) {
        for (int i = 0; i < data->getRank(); i++) {
            checkError2(widths[i][0] >= 0 && widths[i][1] >= 0 && widths[i][0] <= data->getShdWidth(i + 1)[0] && widths[i][1] <= data->getShdWidth(i + 1)[1],
                    "Shadow widths must be non-negative and must not exceed array's initial shadow widths");
            if (!widths[i].empty() && data->isDistributed())
                checkError2(!data->getAlignRule()->isIndirect(i + 1),
                        "Array axes with indirect distribution can have only indirect shadow edges and renewed appropriately.");
            sdata.shdWidths[i] = widths[i];
        }
    } else {
        for (int i = 0; i < data->getRank(); i++) {
            if (data->isDistributed() && data->getAlignRule()->isIndirect(i + 1))
                sdata.shdWidths[i] = ShdWidth::createEmpty();
            else
                sdata.shdWidths[i] = data->getShdWidth(i + 1);
        }
    }
    DvmhShadow shadow;
    shadow.add(sdata);
    shadow.renew(currentRegion, noLibdvm);
#ifndef NO_DVM
    dvmShadowRenew(shadow);
#endif
    // Renew all indirect shadow edges if any
    if (!widths && data->getIndirectShadows()) {
        const std::set<std::string> *shadows = data->getIndirectShadows();
        DvmhShadow shadow;
        for (int i = 0; i < data->getRank(); i++) {
            for (std::set<std::string>::const_iterator it = shadows[i].begin(); it != shadows[i].end(); it++) {
                DvmhShadowData sdata;
                sdata.data = data;
                sdata.isIndirect = true;
                sdata.indirectAxis = i + 1;
                sdata.indirectName = *it;
                shadow.add(sdata);
            }
        }
        checkError2(!cornerFlag || shadow.empty(), "Corner specification can not be used to renew indirect shadow edges");
        shadow.renew(currentRegion, true);
    }
}

static void dvmhShadowRenewCommon(const DvmType dvmDesc[], bool cornerFlag, DvmType specifiedRank, va_list &ap, bool fortranFlag) {
    checkInternal2(inited, "LibDVMH is not initialized");
    checkInternal(dvmDesc);
    DvmhObject *obj = passOrGetOrCreateDvmh(dvmDesc[0], true);
    checkError2(obj, "NULL object is passed to dvmh_shadow_renew");
    checkInternal2(obj->isExactly<DvmhData>(), "Only array can be passed to dvmh_shadow_renew");
    DvmhData *data = obj->as<DvmhData>();
    if (specifiedRank) {
        checkError2(data->getRank() == specifiedRank, "Rank in shadow_renew directive must be the same as in declaration of the variable");
#ifdef NON_CONST_AUTOS
        ShdWidth widths[data->getRank()];
#else
        ShdWidth widths[MAX_ARRAY_RANK];
#endif
        extractArray(ap, data->getRank(), widths, (fortranFlag ? vptPointer : vptValue));
        dvmhShadowRenew(data, cornerFlag, widths);
    } else {
        dvmhShadowRenew(data, cornerFlag);
    }
}

extern "C" void dvmh_shadow_renew_C(const DvmType dvmDesc[], DvmType cornerFlag, DvmType specifiedRank, /* DvmType shadowLow, DvmType shadowHigh */...) {
    InterfaceFunctionGuard guard;
    va_list ap;
    va_start(ap, specifiedRank);
    dvmhShadowRenewCommon(dvmDesc, cornerFlag != 0, specifiedRank, ap, false);
    va_end(ap);
}

extern "C" void dvmh_shadow_renew2_(const DvmType dvmDesc[], const DvmType *pCornerFlag, const DvmType *pSpecifiedRank,
        /* const DvmType *pShadowLow, const DvmType *pShadowHigh */...) {
    InterfaceFunctionGuard guard;
    checkInternal(pCornerFlag && pSpecifiedRank);
    va_list ap;
    va_start(ap, pSpecifiedRank);
    dvmhShadowRenewCommon(dvmDesc, *pCornerFlag != 0, *pSpecifiedRank, ap, true);
    va_end(ap);
}

extern "C" void dvmh_indirect_shadow_renew_C(const DvmType dvmDesc[], DvmType axis, const char shadowName[]) {
    InterfaceFunctionGuard guard;
    checkInternal2(inited, "LibDVMH is not initialized");
    checkInternal(dvmDesc);
    DvmhObject *obj = (DvmhObject *)dvmDesc[0];
    checkError2(obj, "NULL object is passed to dvmh_indirect_shadow_renew");
    checkInternal2(obj->isExactly<DvmhData>(), "Only array can be passed to dvmh_indirect_shadow_renew");
    DvmhData *data = obj->as<DvmhData>();
    checkInternal2(axis >= 1 && axis <= data->getRank(), "Invalid axis passed to dvmh_indirect_shadow_renew");
    checkError3(data->hasIndirectShadow(axis, shadowName), "Shadow edge '%s' needs to be included to the distributed array before usage", shadowName);
    DvmhShadowData sdata;
    sdata.data = data;
    sdata.isIndirect = true;
    sdata.indirectAxis = axis;
    sdata.indirectName = shadowName;
    DvmhShadow shadow;
    shadow.add(sdata);
    shadow.renew(currentRegion, true);
}

extern "C" void dvmh_indirect_shadow_renew_(const DvmType dvmDesc[], const DvmType *pAxis, const DvmType *pShadowNameStr) {
    checkInternal(pAxis && pShadowNameStr);
    dvmh_indirect_shadow_renew_C(dvmDesc, *pAxis, getStr(*pShadowNameStr));
}

extern "C" void dvmh_indirect_localize_C(const DvmType refDvmDesc[], const DvmType targetDvmDesc[], DvmType targetAxis) {
    InterfaceFunctionGuard guard;
    checkInternal2(inited, "LibDVMH is not initialized");
    checkInternal(refDvmDesc && targetDvmDesc);
    DvmhObject *obj = passOrGetOrCreateDvmh(refDvmDesc[0], true);
    checkError2(obj, "NULL object is passed to dvmh_indirect_localize");
    checkInternal2(obj->isExactly<DvmhData>(), "Only array can be passed to dvmh_indirect_localize");
    DvmhData *refData = obj->as<DvmhData>();
    obj = passOrGetOrCreateDvmh(targetDvmDesc[0], true);
    checkError2(obj, "NULL object is passed to dvmh_indirect_localize");
    checkInternal2(obj->isExactly<DvmhData>(), "Only array can be passed to dvmh_indirect_localize");
    DvmhData *targetData = obj->as<DvmhData>();
    checkInternal2(targetAxis >= 1 && targetAxis <= targetData->getRank(), "Invalid axis passed to dvmh_indirect_localize");
    checkError2(refData->getDataType() == DvmhData::dtInt || refData->getDataType() == DvmhData::dtLong || refData->getDataType() == DvmhData::dtLongLong,
            "Only arrays of types int, long, or long long can be localized.");
    refData->localizeAsReferenceFor(targetData, targetAxis);
}

extern "C" void dvmh_indirect_localize_(const DvmType refDvmDesc[], const DvmType targetDvmDesc[], const DvmType *pTargetAxis) {
    checkInternal(pTargetAxis);
    dvmh_indirect_localize_C(refDvmDesc, targetDvmDesc, *pTargetAxis);
}

extern "C" void dvmh_indirect_unlocalize_(const DvmType dvmDesc[]) {
    InterfaceFunctionGuard guard;
    checkInternal2(inited, "LibDVMH is not initialized");
    checkInternal(dvmDesc);
    DvmhObject *obj = passOrGetOrCreateDvmh(dvmDesc[0], true);
    checkError2(obj, "NULL object is passed to dvmh_indirect_unlocalize");
    checkInternal2(obj->isExactly<DvmhData>(), "Only array can be passed to dvmh_indirect_unlocalize");
    DvmhData *data = obj->as<DvmhData>();
    data->unlocalize();
}

// TODO: Maybe use different type of object for remote access buffers
static DvmhData *dvmhRemoteAccess(DvmhData *data, DvmhAxisAlignRule axisRules[], DvmhLoop *loop, DvmType rmaHdr[], const void *baseAddr) {
    DvmhData *rmaData = 0;
    int dataRank = data->getRank();
    DvmhRegion *region = loop ? loop->region : 0;
#ifndef NO_DVM
    DvmType *dvmHdr = (DvmType *)getDvm(data)->HeaderPtr;
    DvmType *dvmRmaHdr = rmaHdr;
    if (!loop || !loop->alignRule) {
#ifdef NON_CONST_AUTOS
        DvmType coordArray[dataRank];
#else
        DvmType coordArray[MAX_ARRAY_RANK];
#endif
        for (int i = 0; i < dataRank; i++) {
            if (axisRules[i].axisNumber == -1)
                coordArray[i] = -1;
            else if (axisRules[i].axisNumber == 0)
                coordArray[i] = axisRules[i].summand - data->getAxisSpace(i + 1).begin();
            else
                coordArray[i] = -1;
        }
        DvmType tmp[2] = {1, 0};
        crtrbp_(dvmHdr, dvmRmaHdr, (void *)baseAddr, &tmp[0], (PSRef *)&tmp[1], coordArray);
    } else {
        LoopRef dvmLoop = (LoopRef)getDvm(loop);
        checkInternal(dvmLoop);
#ifdef NON_CONST_AUTOS
        DvmType axisArray[dataRank], coefArray[dataRank], constArray[dataRank];
#else
        DvmType axisArray[MAX_ARRAY_RANK], coefArray[MAX_ARRAY_RANK], constArray[MAX_ARRAY_RANK];
#endif
        for (int i = 0; i < dataRank; i++) {
            if (axisRules[i].axisNumber == -1) {
                axisArray[i] = -1;
                coefArray[i] = 1;
                constArray[i] = 0;
            } else if (axisRules[i].axisNumber == 0) {
                axisArray[i] = 0;
                coefArray[i] = 0;
                constArray[i] = axisRules[i].summand - data->getAxisSpace(i + 1).begin();
            } else {
                axisArray[i] = axisRules[i].axisNumber;
                coefArray[i] = axisRules[i].multiplier;
                constArray[i] = axisRules[i].summand - data->getAxisSpace(i + 1).begin();
            }
        }
        DvmType tmp = 1;
        crtrbl_(dvmHdr, dvmRmaHdr, (void *)baseAddr, &tmp, &dvmLoop, axisArray, coefArray, constArray);
    }
    SysHandle *dvmRmaHandle = (SysHandle *)dvmRmaHdr[0];
    s_DISARRAY *dvmRmaArr = (s_DISARRAY *)dvmRmaHandle->pP;
    s_REGBUF *RegBuf = dvmRmaArr->RegBuf;
#ifdef NON_CONST_AUTOS
    Interval rmaIndexes[dataRank];
#else
    Interval rmaIndexes[MAX_ARRAY_RANK];
#endif
    for (int i = 0; i < dataRank; i++) {
        rmaIndexes[i] = Interval::create(RegBuf->InitIndex[i], RegBuf->LastIndex[i]) + data->getAxisSpace(i + 1).begin();
        rmaIndexes[i].intersectInplace(data->getLocalPlusShadow()[i]);
        dvmh_log(TRACE, "RegularBufferIndexes[%d] = [" DTFMT ".." DTFMT "]", i, rmaIndexes[i][0], rmaIndexes[i][1]);
    }
    data->syncWriteAccesses();
    {
        PushCurrentPurpose purpose(DvmhCopyingPurpose::dcpRemote);
        data->getActual(rmaIndexes, (region ? region->canAddToActual(data, rmaIndexes) : true));
    }
    DvmType tmp = 0;
    loadrb_(dvmRmaHdr, &tmp);
    waitrb_(dvmRmaHdr);
    rmaData = createDataFromDvmArray(dvmRmaArr, 0, data->getTypeType());
    tieDvm(rmaData, dvmRmaHandle);
#endif
    if (noLibdvm) {
        // TODO: Better to use different approach similar to the realign's one: do not copy data if possible
        int bufRank = 0;
        for (int i = 0; i < dataRank; i++) {
            if (axisRules[i].axisNumber != 0)
                bufRank++;
        }
        if (bufRank == 0)
            bufRank = 1;
        DvmhDistribSpace *dspace = 0;
        if (loop && loop->alignRule) {
            dspace = loop->alignRule->getDspace();
        }
        int dspaceRank = dspace ? dspace->getRank() : 0;
#ifdef NON_CONST_AUTOS
        Interval bufSpace[bufRank], srcBlock[dataRank];
        int dstAxisToSrc[dataRank];
        DvmhAxisAlignRule bufAxisRules[dspaceRank];
        DvmType dstSteps[bufRank], srcSteps[dataRank];
#else
        Interval bufSpace[MAX_ARRAY_RANK], srcBlock[MAX_ARRAY_RANK];
        int dstAxisToSrc[MAX_ARRAY_RANK];
        DvmhAxisAlignRule bufAxisRules[MAX_DISTRIB_SPACE_RANK];
        DvmType dstSteps[MAX_ARRAY_RANK], srcSteps[MAX_ARRAY_RANK];
#endif
        for (int i = 0; i < dspaceRank; i++)
            bufAxisRules[i].setReplicated(1, dspace->getAxisSpace(i + 1));
        int bufIdx = 0;
        for (int i = 0; i < dataRank; i++) {
            if (axisRules[i].axisNumber == -1) {
                bufSpace[bufIdx] = data->getAxisSpace(i + 1);
                srcBlock[i] = data->getAxisSpace(i + 1);
                dstAxisToSrc[bufIdx] = i + 1;
                srcSteps[i] = 1;
                dstSteps[bufIdx] = 1;
                bufIdx++;
            } else if (axisRules[i].axisNumber > 0) {
                int ax = axisRules[i].axisNumber;
                assert(loop);
                DvmType a = loop->loopBounds[ax - 1][0] * axisRules[i].multiplier + axisRules[i].summand;
                DvmType b = loop->loopBounds[ax - 1][1] * axisRules[i].multiplier + axisRules[i].summand;
                Interval dataInterval = Interval::create(std::min(a, b), std::max(a, b));
                checkError2(dataInterval[0] >= data->getAxisSpace(i + 1)[0] && dataInterval[1] <= data->getAxisSpace(i + 1)[1],
                        "Remote access out of array bounds");
                bufSpace[bufIdx] = loop->loopBounds[ax - 1].toInterval();
                srcBlock[i] = dataInterval;
                dstAxisToSrc[bufIdx] = i + 1;
                if (loop->alignRule) {
                    assert(loop->alignRule->getDspace() == dspace);
                    int dspaceAxis = loop->alignRule->getDspaceAxis(ax);
                    bufAxisRules[dspaceAxis - 1] = *loop->alignRule->getAxisRule(dspaceAxis);
                }
                srcSteps[i] = std::abs(axisRules[i].multiplier);
                dstSteps[bufIdx] = 1;
                bufIdx++;
            } else {
                srcSteps[i] = 1;
                srcBlock[i] = Interval::create(axisRules[i].summand, axisRules[i].summand);
            }
        }
        while (bufIdx < bufRank) {
            bufSpace[bufIdx] = Interval::create(0, 0);
            dstAxisToSrc[bufIdx] = -1;
            dstSteps[bufIdx] = 1;
            bufIdx++;
        }
        if (data->getDataType() != DvmhData::dtUnknown)
            rmaData = new DvmhData(data->getDataType(), bufRank, bufSpace, 0);
        else
            rmaData = new DvmhData(data->getTypeSize(), data->getTypeType(), bufRank, bufSpace, 0);
        if (dspace) {
            DvmhAlignRule *alRule = new DvmhAlignRule(dspaceRank, dspace, bufAxisRules);
            rmaData->realign(alRule);
        } else {
            rmaData->realign(0);
        }
        if (rmaData->hasLocal()) {
            rmaData->createNewRepr(0, rmaData->getLocalPlusShadow());
            rmaData->initActual(0);
        }
        dvmhCopyArrayArray(data, srcBlock, srcSteps, rmaData, bufSpace, dstSteps, dstAxisToSrc);
        rmaData->addHeader(rmaHdr, baseAddr);
    }
    allObjects.insert(rmaData);
    if (region)
        region->addRemoteGroup(rmaData);
    return rmaData;
}

static void dvmhRemoteAccessCommon(DvmType rmaDesc[], const void *baseAddr, const DvmType dvmDesc[], DvmType givenRank, va_list &ap, bool fortranFlag) {
    checkInternal2(inited, "LibDVMH is not initialized");
    checkInternal(rmaDesc && dvmDesc);
    DvmhObject *obj = passOrGetOrCreateDvmh(dvmDesc[0], true);
    checkError2(obj, "NULL object is passed to dvmh_remote_access");
    checkInternal2(obj->isExactly<DvmhData>(), "Only array can be passed to dvmh_remote_access");
    DvmhData *data = obj->as<DvmhData>();
    checkError2(data->getRank() == givenRank, "Rank in remote_access directive must be the same as in declaration of the variable");
#ifdef NON_CONST_AUTOS
    DvmhAxisAlignRule axisRules[data->getRank()];
#else
    DvmhAxisAlignRule axisRules[MAX_ARRAY_RANK];
#endif
    extractArray(ap, data->getRank(), axisRules, (fortranFlag ? vptPointer : vptValue));
    DvmhData *rmaData = dvmhRemoteAccess(data, axisRules, 0, rmaDesc, baseAddr);
    assert(rmaData);
}

extern "C" void dvmh_remote_access_C(DvmType rmaDesc[], const DvmType dvmDesc[], DvmType rank, /* DvmType alignmentHelper */...) {
    InterfaceFunctionGuard guard;
    va_list ap;
    va_start(ap, rank);
    dvmhRemoteAccessCommon(rmaDesc, 0, dvmDesc, rank, ap, false);
    va_end(ap);
}

extern "C" void dvmh_remote_access2_(DvmType rmaDesc[], const void *baseAddr, const DvmType dvmDesc[], const DvmType *pRank,
        /* const DvmType *pAlignmentHelper */...) {
    InterfaceFunctionGuard guard;
    checkInternal(pRank);
    va_list ap;
    va_start(ap, pRank);
    dvmhRemoteAccessCommon(rmaDesc, baseAddr, dvmDesc, *pRank, ap, true);
    va_end(ap);
}

static bool dvmhHasElementCommon(const DvmType dvmDesc[], DvmType givenRank, va_list &ap, bool fortranFlag) {
    checkInternal2(inited, "LibDVMH is not initialized");
    checkInternal(dvmDesc);
    DvmhObject *obj = passOrGetOrCreateDvmh(dvmDesc[0], true);
    checkError2(obj, "NULL object is passed to dvmh_has_element");
    checkInternal2(obj->isExactly<DvmhData>(), "Only array can be passed to dvmh_has_element");
    DvmhData *data = obj->as<DvmhData>();
    checkError2(data->getRank() == givenRank, "Rank in every indexing expression must be the same as in declaration of the variable");
#ifdef NON_CONST_AUTOS
    DvmType indexes[data->getRank()];
#else
    DvmType indexes[MAX_ARRAY_RANK];
#endif
    extractArray(ap, data->getRank(), indexes, (fortranFlag ? vptPointer : vptValue));
    bool res = data->convertGlobalToLocal2(indexes);
    res = res && data->hasElement(indexes);
    return res;
}

extern "C" DvmType dvmh_has_element_C(const DvmType dvmDesc[], DvmType rank, /* DvmType index */...) {
    va_list ap;
    va_start(ap, rank);
    bool res = dvmhHasElementCommon(dvmDesc, rank, ap, false);
    va_end(ap);
    return (res ? 1 : 0);
}

extern "C" DvmType dvmh_has_element_(const DvmType dvmDesc[], const DvmType *pRank, /* const DvmType *pIndex */...) {
    checkInternal(pRank);
    va_list ap;
    va_start(ap, pRank);
    bool res = dvmhHasElementCommon(dvmDesc, *pRank, ap, true);
    va_end(ap);
    return (res ? 1 : 0);
}

static DvmType dvmhCalcLinearCommon(const DvmType dvmDesc[], DvmType givenRank, va_list &ap, bool fortranFlag) {
    // This function must be as fast as possible
    DvmhData *data = passOrGetOrCreateDvmh(dvmDesc[0], true)->as<DvmhData>();
    int rank = data->getRank();
    assert(rank == givenRank);
#ifdef NON_CONST_AUTOS
    DvmType indexes[rank];
#else
    DvmType indexes[MAX_ARRAY_RANK];
#endif
    extractArray(ap, rank, indexes, (fortranFlag ? vptPointer : vptValue));
    bool ok = data->convertGlobalToLocal2(indexes);
    assert(ok);
    DvmType res = dvmDesc[rank + 1];
    for (int i = 1; i < rank; i++)
        res += dvmDesc[i] * indexes[i - 1];
    if (rank >= 1)
        res += indexes[rank - 1];
    return res;
}

extern "C" DvmType dvmh_calc_linear_C(const DvmType dvmDesc[], DvmType rank, /* DvmType globalIndex */...) {
    va_list ap;
    va_start(ap, rank);
    DvmType res = dvmhCalcLinearCommon(dvmDesc, rank, ap, false);
    va_end(ap);
    return res;
}

extern "C" DvmType dvmh_calc_linear_(const DvmType dvmDesc[], const DvmType *pRank, /* const DvmType *pGlobalIndex */...) {
    checkInternal(pRank);
    va_list ap;
    va_start(ap, pRank);
    DvmType res = dvmhCalcLinearCommon(dvmDesc, *pRank, ap, true);
    va_end(ap);
    return res;
}

extern "C" void *dvmh_get_own_element_addr_C(const DvmType dvmDesc[], DvmType rank, /* DvmType globalIndex */...) {
    // This function must be as fast as possible
    checkInternal2(inited, "LibDVMH is not initialized");
    checkInternal(dvmDesc);
    DvmhObject *obj = passOrGetOrCreateDvmh(dvmDesc[0], true);
    checkError2(obj, "NULL object is passed to dvmh_get_own_element_addr");
    checkInternal2(obj->isExactly<DvmhData>(), "Only array can be passed to dvmh_get_own_element_addr");
    DvmhData *data = obj->as<DvmhData>();
    checkError2(data->getRank() == rank, "Rank in every indexing expression must be the same as in declaration of the variable");
#ifdef NON_CONST_AUTOS
    DvmType indexes[rank];
#else
    DvmType indexes[MAX_ARRAY_RANK];
#endif
    va_list ap;
    va_start(ap, rank);
    extractArray(ap, rank, indexes, vptValue);
    bool isLocal = data->convertGlobalToLocal2(indexes);
    isLocal = isLocal && data->hasElement(indexes);
    return isLocal ? data->getBuffer(0)->getElemAddr(indexes) : 0;
}

extern "C" void *dvmh_get_element_addr_C(const DvmType dvmDesc[], DvmType rank, /* DvmType globalIndex */...) {
    // This function must be as fast as possible
    checkInternal2(inited, "LibDVMH is not initialized");
    checkInternal(dvmDesc);
    DvmhObject *obj = passOrGetOrCreateDvmh(dvmDesc[0], true);
    checkError2(obj, "NULL object is passed to dvmh_get_element_addr");
    checkInternal2(obj->isExactly<DvmhData>(), "Only array can be passed to dvmh_get_element_addr");
    DvmhData *data = obj->as<DvmhData>();
    checkError2(data->getRank() == rank, "Rank in every indexing expression must be the same as in declaration of the variable");
#ifdef NON_CONST_AUTOS
    DvmType indexes[rank];
#else
    DvmType indexes[MAX_ARRAY_RANK];
#endif
    va_list ap;
    va_start(ap, rank);
    extractArray(ap, rank, indexes, vptValue);
    bool hasElement = data->convertGlobalToLocal2(indexes);
    hasElement = hasElement && data->getLocalPlusShadow()->blockContains(rank, indexes);
    return hasElement ? data->getBuffer(0)->getElemAddr(indexes) : 0;
}


extern "C" DvmType dvmh_region_create_C(DvmType regionFlags) {
    InterfaceFunctionGuard guard;
    checkInternal2(inited, "LibDVMH is not initialized");
    checkError2(currentRegion == 0, "Nested regions are not allowed");
    SourcePosition sp(currentFile, currentLine);
    DvmhRegionPersistentInfo *persInfo = dictFind2(regionDict, sp);
    if (!persInfo) {
        int appearanceNumber = regionDict.size() + 1;
        persInfo = new DvmhRegionPersistentInfo(sp, appearanceNumber);
        regionDict[sp] = persInfo;
    }
    assert(persInfo);
    DvmhRegion *region = new DvmhRegion(regionFlags, persInfo);
    dvmh_log(TRACE, "region_create ok");
    currentRegion = region;
    return (DvmType)region;
}

extern "C" DvmType dvmh_region_create_(const DvmType *pRegionFlags) {
    InterfaceFunctionGuard guard;
    checkInternal(pRegionFlags);
    return dvmh_region_create_C(*pRegionFlags);
}

static void dvmhRegionRegisterSubarrayCommon(DvmType curRegion, DvmType intent, const DvmType dvmDesc[], const char varName[], DvmType givenRank, va_list &ap,
        bool fortranFlag) {
    checkInternal2(inited, "LibDVMH is not initialized");
    checkInternal(dvmDesc);
    DvmhRegion *region = (DvmhRegion *)curRegion;
    checkInternal2(region && region == currentRegion, "Incorrect region reference is passed to dvmh_region_register_subarray");
    DvmhObject *obj = passOrGetOrCreateDvmh(dvmDesc[0], true);
    checkError2(obj, "NULL object is passed to dvmh_region_register_subarray");
    checkInternal2(obj->isExactly<DvmhData>(), "Only array can be passed to dvmh_region_register_subarray");
    DvmhData *data = obj->as<DvmhData>();
    int rank = data->getRank();
    checkError2(rank == givenRank, "Rank in region directive must be the same as in declaration of the variable");
#ifdef NON_CONST_AUTOS
    Interval indexes[rank];
#else
    Interval indexes[MAX_ARRAY_RANK];
#endif
    extractArray(ap, rank, indexes, (fortranFlag ? vptPointer : vptValue));
    makeBlockReal(rank, data->getSpace(), indexes);
    checkError2(data->getSpace()->blockContains(rank, indexes), "Index out of bounds");
    indexes->blockIntersectInplace(rank, data->getLocalPlusShadow());
    region->registerData(data, intent, indexes);
    region->setDataName(data, varName);
}

extern "C" void dvmh_region_register_subarray_C(DvmType curRegion, DvmType intent, const DvmType dvmDesc[], const char varName[], DvmType rank,
        /* DvmType indexLow, DvmType indexHigh */...) {
    InterfaceFunctionGuard guard;
    va_list ap;
    va_start(ap, rank);
    dvmhRegionRegisterSubarrayCommon(curRegion, intent, dvmDesc, varName, rank, ap, false);
    va_end(ap);
}

extern "C" void dvmh_region_register_subarray_(const DvmType *pCurRegion, const DvmType *pIntent, const DvmType dvmDesc[], const DvmType *pVarNameStr,
        const DvmType *pRank, /* const DvmType *pIndexLow, const DvmType *pIndexHigh */...) {
    InterfaceFunctionGuard guard;
    checkInternal(pCurRegion && pIntent && pVarNameStr && pRank);
    va_list ap;
    va_start(ap, pRank);
    dvmhRegionRegisterSubarrayCommon(*pCurRegion, *pIntent, dvmDesc, getStr(pVarNameStr), *pRank, ap, true);
    va_end(ap);
}

extern "C" void dvmh_region_register_array_C(DvmType curRegion, DvmType intent, const DvmType dvmDesc[], const char varName[]) {
    InterfaceFunctionGuard guard;
    checkInternal2(inited, "LibDVMH is not initialized");
    DvmhRegion *region = (DvmhRegion *)curRegion;
    checkInternal2(region && region == currentRegion, "Incorrect region reference is passed to dvmh_region_register_array");
    DvmhObject *obj = passOrGetOrCreateDvmh(dvmDesc[0], true);
    checkError2(obj, "NULL object is passed to dvmh_region_register_array");
    checkInternal2(obj->isExactly<DvmhData>() || obj->isExactly<DvmhDistribSpace>(), "Only array or template can be passed to dvmh_region_register_array");
    if (obj->is<DvmhData>()) {
        DvmhData *data = obj->as<DvmhData>();
        region->registerData(data, intent, data->getLocalPlusShadow());
        if (varName)
            region->setDataName(data, varName);
    } else {
        assert(obj->is<DvmhDistribSpace>());
        DvmhDistribSpace *dspace = obj->as<DvmhDistribSpace>();
        region->registerDspace(dspace);
    }
}

extern "C" void dvmh_region_register_array_(const DvmType *pCurRegion, const DvmType *pIntent, const DvmType dvmDesc[], const DvmType *pVarNameStr) {
    InterfaceFunctionGuard guard;
    checkInternal(pCurRegion && pIntent && dvmDesc && pVarNameStr);
    dvmh_region_register_array_C(*pCurRegion, *pIntent, dvmDesc, getStr(pVarNameStr));
}

extern "C" void dvmh_region_register_scalar_C(DvmType curRegion, DvmType intent, const void *addr, DvmType typeSize, const char varName[]) {
    InterfaceFunctionGuard guard;
    dvmh_region_register_array_C(curRegion, intent, dvmh_variable_gen_header_C(addr, 0, typeSize), varName);
}

extern "C" void dvmh_region_register_scalar_(const DvmType *pCurRegion, const DvmType *pIntent, const void *addr, const DvmType *pTypeSize,
        const DvmType *pVarNameStr) {
    InterfaceFunctionGuard guard;
    checkInternal(pCurRegion && pIntent && pTypeSize && pVarNameStr);
    dvmh_region_register_scalar_C(*pCurRegion, *pIntent, addr, *pTypeSize, getStr(pVarNameStr));
}

extern "C" void dvmh_region_execute_on_targets_C(DvmType curRegion, DvmType deviceTypes) {
    InterfaceFunctionGuard guard;
    checkInternal2(inited, "LibDVMH is not initialized");
    DvmhRegion *region = (DvmhRegion *)curRegion;
    checkInternal2(region && region == currentRegion, "Incorrect region reference is passed to dvmh_region_execute_on_targets");
    region->executeOnTargets(deviceTypes);
}

extern "C" void dvmh_region_execute_on_targets_(const DvmType *pCurRegion, const DvmType *pDeviceTypes) {
    InterfaceFunctionGuard guard;
    checkInternal(pCurRegion && pDeviceTypes);
    dvmh_region_execute_on_targets_C(*pCurRegion, *pDeviceTypes);
}

extern "C" void dvmh_region_end_C(DvmType curRegion) {
    InterfaceFunctionGuard guard;
    checkInternal2(inited, "LibDVMH is not initialized");
    DvmhRegion *region = (DvmhRegion *)curRegion;
    checkInternal2(region && region == currentRegion, "Incorrect region reference is passed to dvmh_region_end");
    region->finish();
    delete region;
    currentRegion = 0;
    for (std::set<const void *>::iterator it = autoDataExit.begin(); it != autoDataExit.end(); it++) {
        dvmh_data_exit_C(*it, 1);
    }
    autoDataExit.clear();
}

extern "C" void dvmh_region_end_(const DvmType *pCurRegion) {
    InterfaceFunctionGuard guard;
    checkInternal(pCurRegion);
    dvmh_region_end_C(*pCurRegion);
}


static DvmhLoop *dvmhLoopCreate(DvmhRegion *region, int rank, LoopBounds loopBounds[]) {
    SourcePosition sp(currentFile, currentLine);
    DvmhLoopPersistentInfo *persInfo = dictFind2(loopDict, sp);
    if (!persInfo) {
        persInfo = new DvmhLoopPersistentInfo(sp, (region ? region->getPersistentInfo() : 0));
        loopDict[sp] = persInfo;
    }
    assert(persInfo);
    assert(rank >= 0);

    DvmhLoop *loop = new DvmhLoop(rank, loopBounds);
    loop->setRegion(region);
    loop->setPersistentInfo(persInfo);
    return loop;
}

static DvmType dvmhLoopCreateCommon(DvmType curRegion, DvmType rank, va_list &ap, bool fortranFlag, bool debugFlag) {
    checkInternal2(inited, "LibDVMH is not initialized");
    DvmhRegion *region = (DvmhRegion *)curRegion;
    checkInternal2(region == currentRegion, "Incorrect region reference is passed to dvmh_loop_create");
    checkError2(!currentLoop, "Cannot create loop inside another loop");
    checkInternal3(rank >= 0, "Incorrect rank passed to dvmh_loop_create (%d < 0)", (int)rank);
    DvmhLoop *loop;
    if (rank > 0) {
#ifdef NON_CONST_AUTOS
        LoopBounds loopBounds[rank];
#else
        LoopBounds loopBounds[MAX_LOOP_RANK];
#endif
        extractArray(ap, rank, loopBounds, (fortranFlag ? vptPointer : vptValue));
        loop = dvmhLoopCreate(region, rank, loopBounds);
    } else
        loop = dvmhLoopCreate(region, rank, 0);
    loop->debugFlag = debugFlag;
    currentLoop = loop;
    return (DvmType)loop;
}

extern "C" DvmType dvmh_loop_create_C(DvmType curRegion, DvmType rank, /* DvmType start, DvmType end, DvmType step */...) {
    InterfaceFunctionGuard guard;
    va_list ap;
    va_start(ap, rank);
    DvmType res = dvmhLoopCreateCommon(curRegion, rank, ap, false, false);
    va_end(ap);
    return res;
}

extern "C" DvmType dvmh_loop_create_(const DvmType *pCurRegion, const DvmType *pRank, /* const DvmType *pStart, const DvmType *pEnd, const DvmType *pStep */...)
{
    InterfaceFunctionGuard guard;
    checkInternal(pCurRegion && pRank);
    va_list ap;
    va_start(ap, pRank);
    DvmType res = dvmhLoopCreateCommon(*pCurRegion, *pRank, ap, true, false);
    va_end(ap);
    return res;
}

extern "C" DvmType dvmh_dbg_loop_create_C(DvmType curRegion, DvmType rank, /* DvmType start, DvmType end, DvmType step */...) {
    InterfaceFunctionGuard guard;
    va_list ap;
    va_start(ap, rank);
    DvmType res = dvmhLoopCreateCommon(curRegion, rank, ap, false, true);
    va_end(ap);
    return res;
}

static void dvmhLoopMap(DvmhLoop *loop, DvmhObject *templ, DvmhAxisAlignRule rules[]) {
    checkInternal2(loop->rank > 0, "Sequential part can not be mapped");
    assert(rules);
    DvmhDistribSpace *dspace;
    DvmhData *mappingData = 0;
    const DvmhAlignRule *underRule = 0;
    if (templ->isExactly<DvmhData>()) {
        mappingData = templ->as<DvmhData>();
        underRule = templ->as<DvmhData>()->getAlignRule();
        dspace = underRule->getDspace();
        for (int i = 0; i < mappingData->getRank(); i++)
            if (rules[i].axisNumber == -1)
                rules[i].setReplicated(1, mappingData->getAxisSpace(i + 1));
    } else {
        dspace = templ->as<DvmhDistribSpace>();
        for (int i = 0; i < dspace->getRank(); i++)
            if (rules[i].axisNumber == -1)
                rules[i].setReplicated(1, dspace->getAxisSpace(i + 1));
    }
    int dspaceRank = dspace->getRank();
#ifdef NON_CONST_AUTOS
    DvmhAxisAlignRule resRules[dspaceRank];
#else
    DvmhAxisAlignRule resRules[MAX_DISTRIB_SPACE_RANK];
#endif
    for (int i = 0; i < dspaceRank; i++) {
        if (!underRule)
            resRules[i] = rules[i];
        else {
            resRules[i] = *underRule->getAxisRule(i + 1);
            resRules[i].composite(rules);
        }
    }
    loop->setAlignRule(new DvmhAlignRule(loop->rank, dspace, resRules));
    if (mappingData) {
        loop->mappingData = mappingData;
        loop->mappingDataRules = new DvmhAxisAlignRule[mappingData->getRank()];
        typedMemcpy(loop->mappingDataRules, rules, mappingData->getRank());
        if (loop->region) {
            DvmhRegionData *rdata = dictFind2(*loop->region->getDatas(), loop->mappingData);
            checkInternal2(rdata, "Array, on which parallel loop is mapped, must be registered in the region");
            dvmh_log(TRACE, "loop mapped on variable %s", rdata->getName());
            loop->persistentInfo->setVarId(rdata->getVarId());
        }
    } else {
        if (loop->region) {
            DvmhRegionDistribSpace *rdspace = dictFind2(*loop->region->getDspaces(), dspace);
            checkInternal2(rdspace, "Template, on which parallel loop is mapped, must be registered in the region");
        }
    }
}

static void dvmhLoopMapCommon(DvmType curLoop, const DvmType templDesc[], DvmType givenTemplRank, va_list &ap, bool fortranFlag) {
    checkInternal2(inited, "LibDVMH is not initialized");
    checkInternal2(templDesc, "NULL pointer is passed as template to dvmh_loop_map");
    DvmhLoop *loop = (DvmhLoop *)curLoop;
    checkInternal2(loop && loop == currentLoop, "Incorrect loop reference is passed to dvmh_loop_map");
    DvmhObject *templ = passOrGetOrCreateDvmh(templDesc[0], true);
    checkError2(templ, "NULL pointer is passed to dvmh_loop_map");
    checkError2(allObjects.find(templ) != allObjects.end(), "Unknown object is passed to dvmh_loop_map");
    checkInternal2(templ->isExactly<DvmhDistribSpace>() || templ->isExactly<DvmhData>(), "Loop can be mapped only on array or template");
    int templRank = templ->isExactly<DvmhDistribSpace>() ? templ->as<DvmhDistribSpace>()->getRank() : templ->as<DvmhData>()->getRank();
    checkInternal2(templRank > 0, "Rank must be positive");
    checkError3(templRank == givenTemplRank, "Rank in loop mapping must be the same as in declaration of the %s",
            (templ->isExactly<DvmhDistribSpace>() ? "template" : "array"));
#ifdef NON_CONST_AUTOS
    DvmhAxisAlignRule axisRules[templRank];
#else
    DvmhAxisAlignRule axisRules[MAX_DISTRIB_SPACE_RANK];
#endif
    extractArray(ap, templRank, axisRules, (fortranFlag ? vptPointer : vptValue));
    dvmhLoopMap(loop, templ, axisRules);
}

extern "C" void dvmh_loop_map_C(DvmType curLoop, const DvmType templDesc[], DvmType rank, /* DvmType alignmentHelper */...) {
    InterfaceFunctionGuard guard;
    va_list ap;
    va_start(ap, rank);
    dvmhLoopMapCommon(curLoop, templDesc,  rank, ap, false);
    va_end(ap);
}

extern "C" void dvmh_loop_map_(const DvmType *pCurLoop, const DvmType templDesc[], const DvmType *pRank, /* const DvmType *pAlignmentHelper */...) {
    InterfaceFunctionGuard guard;
    checkInternal(pCurLoop && pRank);
    va_list ap;
    va_start(ap, pRank);
    dvmhLoopMapCommon(*pCurLoop, templDesc, *pRank, ap, true);
    va_end(ap);
}

extern "C" void dvmh_loop_set_cuda_block_C(DvmType curLoop, DvmType xSize, DvmType ySize, DvmType zSize) {
    InterfaceFunctionGuard guard;
    checkInternal2(inited, "LibDVMH is not initialized");
    DvmhLoop *loop = (DvmhLoop *)curLoop;
    checkInternal2(loop && loop == currentLoop, "Incorrect loop reference is passed to dvmh_loop_set_cuda_block");
    loop->setCudaBlock(xSize, ySize, zSize);
}

extern "C" void dvmh_loop_set_cuda_block_(const DvmType *pCurLoop, const DvmType *pXSize, const DvmType *pYSize, const DvmType *pZSize) {
    InterfaceFunctionGuard guard;
    checkInternal(pCurLoop && pXSize && pYSize && pZSize);
    dvmh_loop_set_cuda_block_C(*pCurLoop, *pXSize, *pYSize, *pZSize);
}

extern "C" void dvmh_loop_set_stage_C(DvmType curLoop, DvmType stage) {
    InterfaceFunctionGuard guard;
    checkInternal2(inited, "LibDVMH is not initialized");
    DvmhLoop *loop = (DvmhLoop *)curLoop;
    checkInternal2(loop && loop == currentLoop, "Incorrect loop reference is passed to dvmh_loop_set_stage");
    checkError3(stage >= 0, "Stage must be non-negative. Given " DTFMT, stage);
    loop->stage = stage;
}

extern "C" void dvmh_loop_set_stage_(const DvmType *pCurLoop, const DvmType *pStage) {
    InterfaceFunctionGuard guard;
    checkInternal(pCurLoop && pStage);
    dvmh_loop_set_stage_C(*pCurLoop, *pStage);
}

extern "C" void dvmh_loop_reduction_C(DvmType curLoop, DvmType redType, void *arrayAddr, DvmType varType, DvmType arrayLength, void *locAddr, DvmType locSize) {
    InterfaceFunctionGuard guard;
    checkInternal2(inited, "LibDVMH is not initialized");
    DvmhLoop *loop = (DvmhLoop *)curLoop;
    checkInternal2(loop && loop == currentLoop, "Incorrect loop reference is passed to dvmh_loop_reduction");
    bool isLoc = redType == rf_MAXLOC || redType == rf_MINLOC;
    checkError2(!isLoc || locAddr, "NULL pointer is passed to dvmh_loop_reduction");
    int funcNumber = redType == rf_MAXLOC ? DvmhReduction::rfMax : (redType == rf_MINLOC ? DvmhReduction::rfMin : redType);
    checkInternal3(funcNumber >= DvmhReduction::rfSum && funcNumber < DvmhReduction::RED_FUNCS, "Unknown reduction function (%d)", funcNumber);
    checkInternal3(varType >= 0 && varType < DvmhData::DATA_TYPES, "Unknown data type (%d)", (int)varType);
    dvmh_get_actual_variable2_(arrayAddr);
    if (isLoc)
        dvmh_get_actual_variable2_(locAddr);
    DvmhReduction *reduction;
    if (isLoc)
        reduction = new DvmhReduction((DvmhReduction::RedFunction)funcNumber, arrayLength, (DvmhData::DataType)varType, arrayAddr, locSize, locAddr);
    else
        reduction = new DvmhReduction((DvmhReduction::RedFunction)funcNumber, arrayLength, (DvmhData::DataType)varType, arrayAddr, 0, 0);
    loop->addReduction(reduction);
}

extern "C" void dvmh_loop_reduction_(const DvmType *pCurLoop, const DvmType *pRedType, void *arrayAddr, const DvmType *pVarType, const DvmType *pArrayLength,
        void *locAddr, const DvmType *pLocSize) {
    InterfaceFunctionGuard guard;
    checkInternal(pCurLoop && pRedType && pVarType && pArrayLength && pLocSize);
    dvmh_loop_reduction_C(*pCurLoop, *pRedType, arrayAddr, *pVarType, *pArrayLength, locAddr, *pLocSize);
}

static void dvmhLoopAcrossCommon(DvmType curLoop, const DvmType dvmDesc[], DvmType givenRank, va_list &ap, bool fortranFlag) {
    checkInternal2(inited, "LibDVMH is not initialized");
    checkInternal(dvmDesc);
    DvmhLoop *loop = (DvmhLoop *)curLoop;
    checkInternal2(loop && loop == currentLoop, "Incorrect loop reference is passed to dvmh_loop_across");
    DvmhObject *obj = passOrGetOrCreateDvmh(dvmDesc[0], true);
    checkError2(obj, "NULL object is passed to dvmh_loop_across");
    checkInternal2(obj->isExactly<DvmhData>(), "Only array can be passed to dvmh_loop_across");
    DvmhData *data = obj->as<DvmhData>();
    checkError2(data->getRank() == givenRank, "Rank in across directive must be the same as in declaration of the variable");
#ifdef NON_CONST_AUTOS
    ShdWidth widths[data->getRank()];
#else
    ShdWidth widths[MAX_ARRAY_RANK];
#endif
    extractArray(ap, data->getRank(), widths, (fortranFlag ? vptPointer : vptValue));
    loop->addToAcross(data, widths);
}

extern "C" void dvmh_loop_across_C(DvmType curLoop, const DvmType dvmDesc[], DvmType rank, /* DvmType shadowLow, DvmType shadowHigh */...) {
    InterfaceFunctionGuard guard;
    va_list ap;
    va_start(ap, rank);
    dvmhLoopAcrossCommon(curLoop, dvmDesc, rank, ap, false);
    va_end(ap);
}

extern "C" void dvmh_loop_across_(const DvmType *pCurLoop, const DvmType dvmDesc[], const DvmType *pRank,
        /* const DvmType *pShadowLow, const DvmType *pShadowHigh */...) {
    InterfaceFunctionGuard guard;
    checkInternal(pCurLoop && pRank);
    va_list ap;
    va_start(ap, pRank);
    dvmhLoopAcrossCommon(*pCurLoop, dvmDesc, *pRank, ap, true);
    va_end(ap);
}

static void dvmhLoopShadowComputeInternal(DvmhLoop *loop, DvmhData *data, ShdWidth widths[]) {
    checkInternal2(loop->mappingData == data, "Only array on which loop is mapped can be passed to dvmh_loop_shadow_compute");
    DvmhDistribSpace *dspace = loop->alignRule->getDspace();
    loop->shadowComputeWidths = new ShdWidth[dspace->getRank()];
    for (int i = 1; i <= dspace->getRank(); i++) {
        const DvmhAxisAlignRule *axisRule = data->getAlignRule()->getAxisRule(i);
        if (axisRule->axisNumber > 0) {
            ShdWidth w = widths[axisRule->axisNumber - 1];
            if (axisRule->multiplier > 0) {
                loop->shadowComputeWidths[i - 1][0] = w[0] * axisRule->multiplier;
                loop->shadowComputeWidths[i - 1][1] = w[1] * axisRule->multiplier;
            } else {
                loop->shadowComputeWidths[i - 1][0] = w[1] * (-axisRule->multiplier);
                loop->shadowComputeWidths[i - 1][1] = w[0] * (-axisRule->multiplier);
            }
        } else {
            loop->shadowComputeWidths[i - 1][0] = 0;
            loop->shadowComputeWidths[i - 1][1] = 0;
        }
    }
}

static void dvmhLoopShadowComputeCommon(DvmType curLoop, const DvmType templDesc[], DvmType specifiedRank, va_list &ap, bool fortranFlag) {
    checkInternal2(inited, "LibDVMH is not initialized");
    DvmhLoop *loop = (DvmhLoop *)curLoop;
    checkInternal2(loop && loop == currentLoop, "Incorrect loop reference is passed to dvmh_loop_shadow_compute");
    DvmhObject *obj = passOrGetOrCreateDvmh(templDesc[0], true);
    checkError2(obj, "NULL object is passed to dvmh_loop_shadow_compute");
    checkInternal2(obj->isExactly<DvmhData>(), "Only array can be passed to dvmh_loop_shadow_compute");
    DvmhData *data = obj->as<DvmhData>();
    int dataRank = data->getRank();
#ifdef NON_CONST_AUTOS
    ShdWidth widths[dataRank];
#else
    ShdWidth widths[MAX_ARRAY_RANK];
#endif
    if (!specifiedRank) {
        for (int i = 0; i < dataRank; i++)
            widths[i] = data->getShdWidth(i + 1);
    } else {
        checkError2(dataRank == specifiedRank, "Rank in shadow_compute directive must be the same as in declaration of the variable");
        extractArray(ap, dataRank, widths, (fortranFlag ? vptPointer : vptValue));
    }
    dvmhLoopShadowComputeInternal(loop, data, widths);
}

extern "C" void dvmh_loop_shadow_compute_C(DvmType curLoop, const DvmType templDesc[], DvmType specifiedRank, /* DvmType shadowLow, DvmType shadowHigh */...) {
    InterfaceFunctionGuard guard;
    va_list ap;
    va_start(ap, specifiedRank);
    dvmhLoopShadowComputeCommon(curLoop, templDesc, specifiedRank, ap, false);
    va_end(ap);
}

extern "C" void dvmh_loop_shadow_compute_(const DvmType *pCurLoop, const DvmType templDesc[], const DvmType *pSpecifiedRank,
        /* const DvmType *pShadowLow, const DvmType *pShadowHigh */...) {
    InterfaceFunctionGuard guard;
    checkInternal(pCurLoop && pSpecifiedRank);
    va_list ap;
    va_start(ap, pSpecifiedRank);
    dvmhLoopShadowComputeCommon(*pCurLoop, templDesc, *pSpecifiedRank, ap, true);
    va_end(ap);
}

extern "C" void dvmh_loop_shadow_compute_array_C(DvmType curLoop, const DvmType dvmDesc[]) {
    InterfaceFunctionGuard guard;
    checkInternal2(inited, "LibDVMH is not initialized");
    DvmhLoop *loop = (DvmhLoop *)curLoop;
    checkInternal2(loop && loop == currentLoop, "Incorrect loop reference is passed to dvmh_loop_shadow_compute_array");
    DvmhObject *obj = passOrGetOrCreateDvmh(dvmDesc[0], true);
    checkError2(obj, "NULL object is passed to dvmh_loop_shadow_compute_array");
    checkInternal2(obj->isExactly<DvmhData>(), "Only array can be passed to dvmh_loop_shadow_compute_array");
    DvmhData *data = obj->as<DvmhData>();
    checkInternal2(loop->shadowComputeWidths, "Call to dvmh_loop_shadow_compute_array without call to dvmh_loop_shadow_compute");
    checkError2(data->isDistributed(), "Array must be distributed or aligned");
    checkError2(data->getAlignRule()->getDspace() == loop->alignRule->getDspace(), "Array must be aligned with the same template as loop mapped to");
    checkInternal2(loop->region, "dvmh_loop_shadow_compute_array can be called only for loops in regions");
    DvmhRegionData *rdata = dictFind2(*loop->region->getDatas(), data);
    checkInternal2(rdata, "Only previously registered array can be passed to dvmh_loop_shadow_compute_array");
    loop->addShadowComputeData(data);
}

extern "C" void dvmh_loop_shadow_compute_array_(const DvmType *pCurLoop, const DvmType dvmDesc[]) {
    InterfaceFunctionGuard guard;
    checkInternal(pCurLoop);
    dvmh_loop_shadow_compute_array_C(*pCurLoop, dvmDesc);
}

static void dvmhLoopConsistentCommon(DvmType curLoop, const DvmType dvmDesc[], DvmType givenRank, va_list &ap, bool fortranFlag) {
    checkInternal2(inited, "LibDVMH is not initialized");
    DvmhLoop *loop = (DvmhLoop *)curLoop;
    checkInternal2(loop && loop == currentLoop, "Incorrect loop reference is passed to dvmh_loop_consistent");
    checkInternal(dvmDesc);
    DvmhObject *obj = passOrGetOrCreateDvmh(dvmDesc[0], true);
    checkError2(obj, "NULL object is passed to dvmh_loop_consistent");
    checkInternal2(obj->isExactly<DvmhData>(), "Only array can be passed to dvmh_loop_consistent");
    DvmhData *data = obj->as<DvmhData>();
    checkError2(data->getRank() == givenRank, "Rank in consistent directive must be the same as in declaration of the variable");
#ifdef NON_CONST_AUTOS
    DvmhAxisAlignRule axisRules[data->getRank()];
#else
    DvmhAxisAlignRule axisRules[MAX_ARRAY_RANK];
#endif
    extractArray(ap, data->getRank(), axisRules, (fortranFlag ? vptPointer : vptValue));
    loop->addConsistent(data, axisRules);
}

extern "C" void dvmh_loop_consistent_C(DvmType curLoop, DvmType const dvmDesc[], DvmType rank, /* DvmType alignmentHelper */...) {
    InterfaceFunctionGuard guard;
    va_list ap;
    va_start(ap, rank);
    dvmhLoopConsistentCommon(curLoop, dvmDesc, rank, ap, false);
    va_end(ap);
}

extern "C" void dvmh_loop_consistent_(const DvmType *pCurLoop, DvmType const dvmDesc[], const DvmType *pRank, /* const DvmType *pAlignmentHelper */...) {
    InterfaceFunctionGuard guard;
    checkInternal(pCurLoop && pRank);
    va_list ap;
    va_start(ap, pRank);
    dvmhLoopConsistentCommon(*pCurLoop, dvmDesc, *pRank, ap, true);
    va_end(ap);
}

static void dvmhLoopRemoteAccessCommon(DvmType curLoop, const DvmType dvmDesc[], DvmType givenRank, va_list &ap, bool fortranFlag) {
    checkInternal2(inited, "LibDVMH is not initialized");
    DvmhLoop *loop = (DvmhLoop *)curLoop;
    checkInternal2(loop && loop == currentLoop, "Incorrect loop reference is passed to dvmh_loop_remote_access");
    checkInternal(dvmDesc);
    DvmhObject *obj = passOrGetOrCreateDvmh(dvmDesc[0], true);
    checkError2(obj, "NULL object is passed to dvmh_loop_remote_access");
    checkInternal2(obj->isExactly<DvmhData>(), "Only array can be passed to dvmh_loop_remote_access");
    DvmhData *data = obj->as<DvmhData>();
    checkError2(data->getRank() == givenRank, "Rank in remote_access directive must be the same as in declaration of the variable");
#ifdef NON_CONST_AUTOS
    DvmhAxisAlignRule axisRules[data->getRank()];
#else
    DvmhAxisAlignRule axisRules[MAX_ARRAY_RANK];
#endif
    extractArray(ap, data->getRank(), axisRules, (fortranFlag ? vptPointer : vptValue));
    loop->addRemoteAccess(data, axisRules);
}

extern "C" void dvmh_loop_remote_access_C(DvmType curLoop, const DvmType dvmDesc[], DvmType rank, /* DvmType alignmentHelper */...) {
    InterfaceFunctionGuard guard;
    va_list ap;
    va_start(ap, rank);
    dvmhLoopRemoteAccessCommon(curLoop, dvmDesc, rank, ap, false);
    va_end(ap);
}

extern "C" void dvmh_loop_remote_access_(const DvmType *pCurLoop, const DvmType dvmDesc[], const DvmType *pRank, /* const DvmType *pAlignmentHelper */...) {
    InterfaceFunctionGuard guard;
    checkInternal(pCurLoop && pRank);
    va_list ap;
    va_start(ap, pRank);
    dvmhLoopRemoteAccessCommon(*pCurLoop, dvmDesc, *pRank, ap, true);
    va_end(ap);
}

extern "C" void dvmh_loop_register_handler_C(DvmType curLoop, DvmType inDeviceType, DvmType handlerType, DvmType handlerHelper) {
    InterfaceFunctionGuard guard;
    checkInternal2(inited, "LibDVMH is not initialized");
    WrappedDvmHandler wrap;
    loadObject(handlerHelper, wrap);
    checkInternal(wrap.func);
    DvmhLoop *loop = (DvmhLoop *)curLoop;
    checkInternal2(loop && loop == currentLoop, "Incorrect loop reference is passed to dvmh_loop_register_handler");
    checkInternal3(inDeviceType > 0, "Incorrect device type is passed to dvmh_loop_register_handler (" DTFMT ")", inDeviceType);
    DeviceType deviceType = (DeviceType)ilog(inDeviceType);
    checkInternal3(deviceType >= 0 && deviceType < DEVICE_TYPES, "Incorrect device type is passed to dvmh_loop_register_handler (%d)", (int)deviceType);
    DvmhLoopHandler *handler = new DvmhLoopHandler((handlerType & HANDLER_TYPE_PARALLEL) != 0, (handlerType & HANDLER_TYPE_MASTER) != 0, wrap.func,
            wrap.params.size());
    for (int i = 0; i < (int)wrap.params.size(); i++)
        handler->setParam(i, wrap.params[i]);
    loop->addHandler(deviceType, handler);
    dvmh_log(TRACE, "Registered handler #%d for %s. isMaster=%d isParallel=%d paramsCount=%d", (int)loop->handlers[deviceType].size(),
            (deviceType == dtHost ? "HOST" : "CUDA"), (int)handler->isMaster(), (int)handler->isParallel(), (int)wrap.params.size());
}

extern "C" void dvmh_loop_register_handler_(const DvmType *pCurLoop, const DvmType *pDeviceType, const DvmType *pHandlerType, const DvmType *pHandlerHelper) {
    checkInternal(pCurLoop && pDeviceType && pHandlerType && pHandlerHelper);
    dvmh_loop_register_handler_C(*pCurLoop, *pDeviceType, *pHandlerType, *pHandlerHelper);
}

#ifndef NO_DVM
static RedGroupRef dvmInsred(DvmhLoop *loop) {
    checkInternal2(getDvm(loop), "Reduction is unsupported");
    RedGroupRef redGroup;
    DvmType staticSign = 1;
    DvmType tmpVar;
    tmpVar = 1;
    redGroup = crtrg_(&staticSign, &tmpVar);
    if (!loop->reductions.empty()) {
        RedRef *redRefs = new RedRef[loop->reductions.size()];
        for (int i = 0; i < (int)loop->reductions.size(); i++) {
            DvmhReduction *red = loop->reductions[i];
            DvmType redFuncNumb = (int)red->redFunc;
            DvmType redArrayType = (int)red->arrayElementType;
            DvmType redArrayLength = red->elemCount;
            if (red->isLoc()) {
                DvmType locElmLength = red->locSize;
                DvmType locIndType;
                if (red->locSize % sizeof(long) == 0)
                    locIndType = 0;
                else if (red->locSize % sizeof(int) == 0)
                    locIndType = 1;
                else if (red->locSize % sizeof(short) == 0)
                    locIndType = 2;
                else
                    locIndType = 3;
                redRefs[i] = crtred_(&redFuncNumb, red->arrayAddr, &redArrayType, &redArrayLength, red->locAddr, &locElmLength, &staticSign);
                lindtp_(&redRefs[i], &locIndType);
            } else {
                DvmType locElmLength = 0;
                redRefs[i] = crtred_(&redFuncNumb, red->arrayAddr, &redArrayType, &redArrayLength, 0, &locElmLength, &staticSign);
            }
        }
        LoopRef dvmLoop = (LoopRef)getDvm(loop);
        for (int i = 0; i < (int)loop->reductions.size(); i++) {
            tmpVar = 0;
            insred_(&redGroup, &redRefs[i], &dvmLoop, &tmpVar);
        }
        delete[] redRefs;
    }
    return redGroup;
}

static void dvmAddShd(DvmhLoop *loop) {
    if (loop->shadowComputeWidths) {
        checkInternal2(getDvm(loop), "Shadow compute is unsupported");
        DvmhData *data = loop->mappingData;
#ifdef NON_CONST_AUTOS
        DvmType lowShdWidths[data->getRank()], highShdWidths[data->getRank()];
#else
        DvmType lowShdWidths[MAX_ARRAY_RANK], highShdWidths[MAX_ARRAY_RANK];
#endif
        for (int i = 1; i <= data->getRank(); i++) {
            int ax = data->getAlignRule()->getDspaceAxis(i);
            if (ax > 0) {
                const DvmhAxisAlignRule *axisRule = data->getAlignRule()->getAxisRule(ax);
                ShdWidth w = loop->shadowComputeWidths[ax - 1];
                if (axisRule->multiplier > 0) {
                    lowShdWidths[i - 1] = w[0] / axisRule->multiplier;
                    highShdWidths[i - 1] = w[1] / axisRule->multiplier;
                } else {
                    lowShdWidths[i - 1] = w[1] / (-axisRule->multiplier);
                    highShdWidths[i - 1] = w[0] / (-axisRule->multiplier);
                }
            } else {
                lowShdWidths[i - 1] = 0;
                highShdWidths[i - 1] = 0;
            }
        }
        DvmType *dataHeader = (DvmType *)getDvm(data)->HeaderPtr;
        addshd_(dataHeader, lowShdWidths, highShdWidths);
    }
}

static void dvmMapPL(DvmhLoop *loop, long indexVars[], DvmType outInitIndices[], DvmType outLastIndices[], DvmType outSteps[]) {
    int patternRank = loop->alignRule->getDspace()->getRank();
    if (loop->mappingData && loop->mappingDataRules)
        patternRank = loop->mappingData->getRank();
#ifdef NON_CONST_AUTOS
    AddrType indexVarAddrs[loop->rank];
    DvmType indexVarTypes[loop->rank], inInitIndices[loop->rank], inLastIndices[loop->rank], inSteps[loop->rank];
    DvmType patternAxes[patternRank], patternCoeffs[patternRank], patternConsts[patternRank];
#else
    AddrType indexVarAddrs[MAX_LOOP_RANK];
    DvmType indexVarTypes[MAX_LOOP_RANK], inInitIndices[MAX_LOOP_RANK], inLastIndices[MAX_LOOP_RANK], inSteps[MAX_LOOP_RANK];
    DvmType patternAxes[MAX_DISTRIB_SPACE_RANK], patternCoeffs[MAX_DISTRIB_SPACE_RANK], patternConsts[MAX_DISTRIB_SPACE_RANK];
#endif
    for (int i = 0; i < loop->rank; i++) {
        indexVarAddrs[i] = (AddrType)(&indexVars[i]);
        indexVarTypes[i] = 0; // TODO: 0 stands for 'long', change to a new appropriate constant when LibDVM will be ready to accept it
        inInitIndices[i] = loop->loopBounds[i][0];
        inLastIndices[i] = loop->loopBounds[i][1];
        inSteps[i] = loop->loopBounds[i][2];
    }
    PatternRef patternRef;
    if (loop->mappingData) {
        // On array.
        patternRef = (PatternRef)getDvm(loop->mappingData);
        assert(loop->mappingDataRules);
        for (int i = 0; i < patternRank; i++)
            if (loop->mappingDataRules[i].axisNumber < 0) {
                patternAxes[i] = -1;
                patternCoeffs[i] = 0;
                patternConsts[i] = 0;
            } else if (loop->mappingDataRules[i].axisNumber == 0) {
                patternAxes[i] = 0;
                patternCoeffs[i] = 0;
                patternConsts[i] = loop->mappingDataRules[i].summand - loop->mappingData->getAxisSpace(i + 1).begin();
            } else {
                patternAxes[i] = loop->mappingDataRules[i].axisNumber;
                patternCoeffs[i] = loop->mappingDataRules[i].multiplier;
                patternConsts[i] = loop->mappingDataRules[i].summand - loop->mappingData->getAxisSpace(i + 1).begin();
            }
    } else {
        // On TEMPLATE.
        patternRef = (PatternRef)getDvm(loop->alignRule->getDspace());
        for (int i = 0; i < patternRank; i++)
            if (loop->alignRule->getAxisRule(i + 1)->axisNumber < 0) {
                patternAxes[i] = -1;
                patternCoeffs[i] = 0;
                patternConsts[i] = 0;
            } else if (loop->alignRule->getAxisRule(i + 1)->axisNumber == 0) {
                patternAxes[i] = 0;
                patternCoeffs[i] = 0;
                patternConsts[i] = loop->alignRule->getAxisRule(i + 1)->summand - loop->alignRule->getDspace()->getAxisSpace(i + 1).begin();
            } else {
                patternAxes[i] = loop->alignRule->getAxisRule(i + 1)->axisNumber;
                patternCoeffs[i] = loop->alignRule->getAxisRule(i + 1)->multiplier;
                patternConsts[i] = loop->alignRule->getAxisRule(i + 1)->summand - loop->alignRule->getDspace()->getAxisSpace(i + 1).begin();
            }
    }
    LoopRef dvmLoop = (LoopRef)getDvm(loop);
    mappl_(&dvmLoop, &patternRef, patternAxes, patternCoeffs, patternConsts, indexVarAddrs, indexVarTypes, inInitIndices, inLastIndices, inSteps,
            outInitIndices, outLastIndices, outSteps);
}
#endif

extern "C" void dvmh_loop_perform_C(DvmType curLoop) {
    InterfaceFunctionGuard guard;
    checkInternal2(inited, "LibDVMH is not initialized");
    DvmhLoop *loop = (DvmhLoop *)curLoop;
    checkInternal2(loop && loop == currentLoop, "Incorrect loop reference is passed to dvmh_loop_perform");
    bool doOwnReduction = true;
#ifndef NO_DVM
    RedGroupRef redGroup = 0;
#endif
    if (loop->rank > 0) {
#ifdef NON_CONST_AUTOS
        LoopBounds toExecute[loop->rank];
#else
        LoopBounds toExecute[MAX_LOOP_RANK];
#endif
#ifndef NO_DVM
        LoopRef dvmLoop = 0;
#ifdef NON_CONST_AUTOS
        long indexVars[loop->rank];
        DvmType outInitIndices[loop->rank], outLastIndices[loop->rank], outSteps[loop->rank];
#else
        long indexVars[MAX_LOOP_RANK];
        DvmType outInitIndices[MAX_LOOP_RANK], outLastIndices[MAX_LOOP_RANK], outSteps[MAX_LOOP_RANK];
#endif
        DvmType tmpVar;
        if (loop->alignRule) {
            if (!loop->alignRule->hasIndirect()) {
                tmpVar = loop->rank;
                dvmLoop = crtpl_(&tmpVar);
                tieDvm(loop, dvmLoop);
            }
            dvmAddShd(loop);
            if (dvmLoop)
                dvmMapPL(loop, indexVars, outInitIndices, outLastIndices, outSteps);
            if (dvmLoop && !loop->reductions.empty()) {
                redGroup = dvmInsred(loop);
                doOwnReduction = false;
            }
            // TODO: Handle CONSISTENT
        }
        // TODO: Handle REMOTE_ACCESS
        if (loop->alignRule) {
            loop->acrossOld.renew(loop->region, false);
            ShadowGroupRef oldBG = 0, newBG = 0;
            if (!loop->acrossOld.empty()) {
                checkInternal2(dvmLoop, "Across is unsupported");
                oldBG = dvmCreateShG(loop->acrossOld, false);
            }
            if (!loop->acrossNew.empty()) {
                checkInternal2(dvmLoop, "Across is unsupported");
                newBG = dvmCreateShG(loop->acrossNew, false);
            }
            if (oldBG && !newBG) {
                strtsh_(&oldBG);
                waitsh_(&oldBG);
            } else if (newBG) {
                // TODO: Handle ACROSS Type 1
                tmpVar = 0;
                double pipeLinePar = loop->stage;
                across_(&tmpVar, &oldBG, &newBG, &pipeLinePar);
            }
        }
#endif
        if (doOwnReduction) {
            for (int i = 0; i < (int)loop->reductions.size(); i++)
                loop->reductions[i]->initGlobalValues();
        }
        if (noLibdvm) {
            // TODO: Wait for loop->acrossNew shadows (maybe conveyorized)
            // TODO: Conveyor (both inter-process and intra-process)
        }
        // Should be after shadow renewals
        loop->prepareExecution();
        if (!loop->hasLocal) {
            handlePreAcross(loop, loop->loopBounds);
            handlePostAcross(loop, loop->loopBounds);
        }
        bool finishLoop = false;
        for (DvmType iter = 0; !finishLoop; iter++) {
            bool toExecuteFilled = false;
#ifndef NO_DVM
            if (dvmLoop) {
                if (dopl_(&dvmLoop) != 0) {
                    for (int i = 0; i < loop->rank; i++) {
                        toExecute[i][0] = outInitIndices[i];
                        toExecute[i][1] = outLastIndices[i];
                        toExecute[i][2] = outSteps[i];
                    }
                    toExecuteFilled = true;
                } else {
                    break;
                }
            }
#endif
            if (!toExecuteFilled) {
                if (loop->hasLocal) {
                    typedMemcpy(toExecute, loop->localPlusShadow, loop->rank);
                    finishLoop = true;
                } else {
                    break;
                }
            }
            handlePreAcross(loop, toExecute);
            loop->executePart(toExecute);
            handlePostAcross(loop, toExecute);
        }
#ifndef NO_DVM
        if (dvmLoop) {
            endpl_(&dvmLoop);
            untieDvm(loop);
        }
#endif
    } else {
        // Sequential part
        // TODO: Handle REMOTE_ACCESS
        loop->prepareExecution();
        loop->executePart(0);
    }

#ifndef NO_DVM
    if (redGroup) {
        strtrd_(&redGroup);
        waitrd_(&redGroup);
        delobj_(&redGroup);
    }
#endif
    if (doOwnReduction && loop->alignRule && !loop->reductions.empty()) {
        dvmh_log(TRACE, "Starting interprocess reduction");
        bool iSend = true;
        const MultiprocessorSystem *loopMPS = loop->alignRule->getDistribRule()->getMPS();
        int loopCommRank = loopMPS->getCommRank();
        if (loopCommRank >= 0) {
            int dspaceRank = loop->alignRule->getDspace()->getRank();
            int loopMpsRank = std::max(loopMPS->getRank(), loop->alignRule->getDistribRule()->getMpsAxesUsed());
#ifdef NON_CONST_AUTOS
            bool distrMpsAxes[loopMpsRank];
#else
            bool distrMpsAxes[MAX_MPS_RANK];
#endif
            for (int i = 0; i < loopMpsRank; i++)
                distrMpsAxes[i] = false;
            for (int i = 0; i < dspaceRank; i++) {
                if (loop->alignRule->getDistribRule()->getAxisRule(i + 1)->isDistributed()) {
                    distrMpsAxes[loop->alignRule->getDistribRule()->getAxisRule(i + 1)->getMPSAxis() - 1] = true;
                    if (loop->alignRule->getAxisRule(i + 1)->axisNumber == -1) {
                        if (loop->alignRule->getAxisRule(i + 1)->replicInterval[0] < loop->alignRule->getDistribRule()->getAxisRule(i + 1)->getLocalElems()[0]) {
                            iSend = false;
                            break;
                        }
                    }
                }
            }
            for (int i = 0; i < loopMpsRank; i++) {
                if (!distrMpsAxes[i] && loopMPS->getAxis(i + 1).ourProc > 0) {
                    iSend = false;
                    break;
                }
            }
        }
        for (int i = 0; i < (int)loop->reductions.size(); i++) {
            if (!iSend)
                loop->reductions[i]->initGlobalValues();
            currentMPS->allreduce(loop->reductions[i]);
        }
    }
    if (doOwnReduction) {
        for (int i = 0; i < (int)loop->reductions.size(); i++)
            loop->reductions[i]->addInitialValues();
    }
    for (int i = 0; i < (int)loop->reductions.size(); i++) {
        dvmh_actual_variable2_(loop->reductions[i]->arrayAddr);
        if (loop->reductions[i]->isLoc())
            dvmh_actual_variable2_(loop->reductions[i]->locAddr);
    }
    dvmh_log(TRACE, "loop ended async=%d", (int)(loop->region && loop->region->isAsync()));
    loop->afterExecution();
    delete loop;
    currentLoop = 0;
    if (!currentRegion) {
        for (std::set<const void *>::iterator it = autoDataExit.begin(); it != autoDataExit.end(); it++) {
            dvmh_data_exit_C(*it, 1);
        }
        autoDataExit.clear();
    }
}

extern "C" void dvmh_loop_perform_(const DvmType *pCurLoop) {
    InterfaceFunctionGuard guard;
    checkInternal(pCurLoop);
    dvmh_loop_perform_C(*pCurLoop);
}

extern "C" DvmType dvmh_loop_get_dependency_mask_C(DvmType curLoop) {
    InterfaceFunctionGuard guard;
    checkInternal2(inited, "LibDVMH is not initialized");
    DvmhObject *obj = (DvmhObject *)curLoop;
    checkInternal2(obj, "NULL pointer is passed to dvmh_loop_get_dependency_mask");
    checkInternal2(obj->is<DvmhSpecLoop>(), "Incorrect loop reference is passed to dvmh_loop_get_dependency_mask");
    DvmhSpecLoop *sloop = obj->as<DvmhSpecLoop>();
    DvmhLoop *loop = sloop->getLoop();
    assert(loop);
    return loop->dependencyMask;
}

extern "C" DvmType dvmh_loop_get_dependency_mask_(const DvmType *pCurLoop) {
    InterfaceFunctionGuard guard;
    checkInternal(pCurLoop);
    return dvmh_loop_get_dependency_mask_C(*pCurLoop);
}

extern "C" DvmType dvmh_loop_get_device_num_C(DvmType curLoop) {
    InterfaceFunctionGuard guard;
    checkInternal2(inited, "LibDVMH is not initialized");
    DvmhObject *obj = (DvmhObject *)curLoop;
    checkInternal2(obj, "NULL pointer is passed to dvmh_loop_get_device_num");
    checkInternal2(obj->is<DvmhSpecLoop>(), "Incorrect loop reference is passed to dvmh_loop_get_device_num");
    DvmhSpecLoop *sloop = obj->as<DvmhSpecLoop>();
    return sloop->getDeviceNum();
}

extern "C" DvmType dvmh_loop_get_device_num_(const DvmType *pCurLoop) {
    InterfaceFunctionGuard guard;
    checkInternal(pCurLoop);
    return dvmh_loop_get_device_num_C(*pCurLoop);
}

extern "C" DvmType dvmh_loop_autotransform_C(DvmType curLoop, DvmType dvmDesc[]) {
    InterfaceFunctionGuard guard;
    checkInternal2(inited, "LibDVMH is not initialized");
    DvmhObject *obj = (DvmhObject *)curLoop;
    checkInternal2(obj, "NULL pointer is passed to dvmh_loop_autotransform");
    checkInternal2(obj->isExactly<DvmhLoopCuda>(), "Incorrect loop reference is passed to dvmh_loop_autotransform");
    DvmhLoopCuda *cloop = obj->as<DvmhLoopCuda>();
    DvmhLoop *loop = cloop->getLoop();
    assert(loop);
    checkError2(dvmDesc, "NULL pointer is passed to dvmh_loop_autotransform");
    obj = passOrGetDvmh(dvmDesc[0]);
    checkError2(obj, "NULL pointer is passed to dvmh_loop_autotransform");
    checkInternal2(obj->isExactly<DvmhData>(), "Only DVM arrays can be passed to dvmh_loop_autotransform");
    DvmhData *data = obj->as<DvmhData>();
    return autotransformInternal(cloop, data);
}

extern "C" DvmType dvmh_loop_autotransform_(const DvmType *pCurLoop, DvmType dvmDesc[]) {
    InterfaceFunctionGuard guard;
    checkInternal(pCurLoop);
    return dvmh_loop_autotransform_C(*pCurLoop, dvmDesc);
}

extern "C" void dvmh_loop_fill_bounds_C(DvmType curLoop, DvmType boundsLow[], DvmType boundsHigh[], DvmType loopSteps[]) {
    InterfaceFunctionGuard guard;
    checkInternal2(inited, "LibDVMH is not initialized");
    DvmhObject *obj = (DvmhObject *)curLoop;
    checkInternal2(obj, "NULL pointer is passed to dvmh_loop_fill_bounds");
    checkInternal2(obj->is<DvmhSpecLoop>(), "Incorrect loop reference is passed to dvmh_loop_fill_bounds");
    DvmhSpecLoop *sloop = obj->as<DvmhSpecLoop>();
    DvmhLoop *loop = sloop->getLoop();
    assert(loop);
    for (int i = 0; i < loop->rank; i++) {
        if (boundsLow)
            boundsLow[i] = sloop->getPortion()->getLoopBounds(i + 1).begin();
        if (boundsHigh)
            boundsHigh[i] = sloop->getPortion()->getLoopBounds(i + 1).end();
        if (loopSteps)
            loopSteps[i] = sloop->getPortion()->getLoopBounds(i + 1).step();
    }
}

extern "C" void dvmh_loop_fill_bounds_(const DvmType *pCurLoop, DvmType boundsLow[], DvmType boundsHigh[], DvmType loopSteps[]) {
    InterfaceFunctionGuard guard;
    checkInternal(pCurLoop);
    dvmh_loop_fill_bounds_C(*pCurLoop, boundsLow, boundsHigh, loopSteps);
}

extern "C" DvmType dvmh_loop_get_slot_count_C(DvmType curLoop) {
    InterfaceFunctionGuard guard;
    checkInternal2(inited, "LibDVMH is not initialized");
    DvmhObject *obj = (DvmhObject *)curLoop;
    checkInternal2(obj, "NULL pointer is passed to dvmh_loop_get_slot_count");
    checkInternal2(obj->is<DvmhSpecLoop>(), "Incorrect loop reference is passed to dvmh_loop_get_slot_count");
    DvmhSpecLoop *sloop = obj->as<DvmhSpecLoop>();
    return sloop->getPortion()->getSlotsToUse();
}

extern "C" DvmType dvmh_loop_get_slot_count_(const DvmType *pCurLoop) {
    InterfaceFunctionGuard guard;
    checkInternal(pCurLoop);
    return dvmh_loop_get_slot_count_C(*pCurLoop);
}

extern "C" void dvmh_loop_fill_local_part_C(DvmType curLoop, const DvmType dvmDesc[], DvmType part[]) {
    InterfaceFunctionGuard guard;
    checkInternal2(inited, "LibDVMH is not initialized");
    checkInternal(part);
    DvmhObject *obj = (DvmhObject *)curLoop;
    checkInternal2(obj, "NULL pointer is passed to dvmh_loop_fill_local_part");
    checkInternal2(obj->is<DvmhSpecLoop>(), "Incorrect loop reference is passed to dvmh_loop_fill_local_part");
    DvmhSpecLoop *sloop = obj->as<DvmhSpecLoop>();
    checkInternal2(dvmDesc, "NULL pointer is passed to dvmh_loop_fill_local_part");
    obj = passOrGetDvmh(dvmDesc[0]);
    checkError2(obj, "NULL pointer is passed to dvmh_loop_fill_local_part");
    checkInternal2(obj->isExactly<DvmhData>(), "Only DVM arrays can be passed to dvmh_loop_fill_local_part");
    DvmhData *data = obj->as<DvmhData>();
    sloop->fillLocalPart(data, part);
}

extern "C" void dvmh_loop_fill_local_part_(const DvmType *pCurLoop, const DvmType dvmDesc[], DvmType part[]) {
    InterfaceFunctionGuard guard;
    checkInternal(pCurLoop);
    dvmh_loop_fill_local_part_C(*pCurLoop, dvmDesc, part);
}

extern "C" DvmType dvmh_loop_guess_index_type_C(DvmType curLoop) {
    InterfaceFunctionGuard guard;
    checkInternal2(inited, "LibDVMH is not initialized");
    DvmhObject *obj = (DvmhObject *)curLoop;
    checkInternal2(obj, "NULL pointer is passed to dvmh_loop_guess_index_type");
    checkInternal2(obj->is<DvmhSpecLoop>(), "Incorrect loop reference is passed to dvmh_loop_guess_index_type");
    DvmhSpecLoop *sloop = obj->as<DvmhSpecLoop>();
    DvmhLoop *loop = sloop->getLoop();
    assert(loop);
    int dev = sloop->getDeviceNum();
    int sizeNeeded = 0;
    int maxPossibleSize = sizeof(long long);
    DvmhRegion *region = loop->region;
    if (region) {
        if (region->usesDevice(dev)) {
            for (std::map<DvmhData *, DvmhRegionData *>::iterator it = region->getDatas()->begin(); it != region->getDatas()->end(); it++) {
                DvmhData *data = it->first;
                DvmhRegionData *rdata = it->second;
                if (rdata->getLocalPart(dev) || data->getRank() == 0) {
#ifdef NON_CONST_AUTOS
                    Interval exBlock[data->getRank()];
#else
                    Interval exBlock[MAX_ARRAY_RANK];
#endif
                    data->extendBlock(rdata->getLocalPart(dev), exBlock);
                    assert(data->getRepr(dev));
                    sizeNeeded = std::max(sizeNeeded, data->getBuffer(dev)->getMinIndexTypeSize(exBlock, true));
                    if (sizeNeeded >= maxPossibleSize)
                        break;
                }
            }
        }
    } else {
        for (std::set<DvmhObject *>::iterator it = allObjects.begin(); it != allObjects.end(); it++) {
            DvmhData *data = (*it)->as_s<DvmhData>();
            if (data && data->getRepr(dev)) {
                DvmhBuffer *buf = data->getBuffer(dev);
                sizeNeeded = std::max(sizeNeeded, buf->getMinIndexTypeSize(buf->getHavePortion(), true));
                if (sizeNeeded >= maxPossibleSize)
                    break;
            }
        }
    }
    int res;
    if (sizeNeeded <= (int)sizeof(int)) {
        res = rt_INT;
    } else if (sizeNeeded <= (int)sizeof(long)) {
        res = rt_LONG;
    } else if (sizeNeeded <= (int)sizeof(long long)) {
        res = rt_LLONG;
    } else {
        dvmh_log(WARNING, "There is a danger to not to be able to address arrays correctly.");
        res = rt_LLONG;
    }
    dvmh_log(TRACE, "Guessed index type for the loop on device #%d is %s", dev,
            (res == rt_INT ? "int" : (res == rt_LONG ? "long" : (res == rt_LLONG ? "long long" : "unknown"))));
    return res;
}

extern "C" DvmType dvmh_loop_guess_index_type_(const DvmType *pCurLoop) {
    InterfaceFunctionGuard guard;
    checkInternal(pCurLoop);
    return dvmh_loop_guess_index_type_C(*pCurLoop);
}

extern "C" const void *dvmh_loop_cuda_get_local_part_C(DvmType curLoop, const DvmType dvmDesc[], DvmType indexType) {
    InterfaceFunctionGuard guard;
    checkInternal2(inited, "LibDVMH is not initialized");
    DvmhObject *obj = (DvmhObject *)curLoop;
    checkInternal2(obj, "NULL pointer is passed to dvmh_loop_cuda_get_local_part");
    checkInternal2(obj->isExactly<DvmhLoopCuda>(), "Incorrect loop reference is passed to dvmh_loop_cuda_get_local_part");
    DvmhLoopCuda *cloop = obj->as<DvmhLoopCuda>();
    checkInternal2(dvmDesc, "NULL pointer is passed to dvmh_loop_cuda_get_local_part");
    obj = passOrGetDvmh(dvmDesc[0]);
    checkError2(obj, "NULL pointer is passed to dvmh_loop_cuda_get_local_part");
    checkInternal2(obj->isExactly<DvmhData>(), "Only DVM arrays can be passed to dvmh_loop_cuda_get_local_part");
    DvmhData *data = obj->as<DvmhData>();
    if (indexType == rt_INT)
        return cloop->getLocalPart<int>(data);
    else if (indexType == rt_LONG)
        return cloop->getLocalPart<long>(data);
    else if (indexType == rt_LLONG)
        return cloop->getLocalPart<long long>(data);
    else
        checkInternal2(false, "Index type could be rt_INT, rt_LONG or rt_LLONG");
    return 0;
}

// Запрос на заполнение заголовочного массива для буфера удаленных элементов. Первый параметр - новый локальный описатель цикла, который передается параметром в обработчик. Второй параметр - номер буфера удаленных элементов (нумерация с единицы). Третий параметр - массив, в который будет помещен заголовочный массив буфера удаленных элементов.
extern "C" void dvmh_loop_get_remote_buf_C(DvmType curLoop, DvmType rmaIndex, DvmType rmaDesc[]) {
    InterfaceFunctionGuard guard;
    checkInternal2(inited, "LibDVMH is not initialized");
    DvmhObject *obj = (DvmhObject *)curLoop;
    checkInternal2(obj, "NULL pointer is passed to dvmh_loop_get_remote_buf");
    checkInternal2(obj->is<DvmhSpecLoop>(), "Incorrect loop reference is passed to dvmh_loop_get_remote_buf");
    DvmhSpecLoop *sloop = obj->as<DvmhSpecLoop>();
    DvmhLoop *loop = sloop->getLoop();
    checkInternal3(rmaIndex >= 1 && rmaIndex <= (int)loop->rmas.size(), "Invalid remote access buffer index (%d) is passed to dvmh_loop_get_remote_buf",
            (int)rmaIndex);
    //loop->rmas[rmaIndex - 1]->setHeader(rmaDesc, 0);
    // TODO: Need to remember DvmhData for REMOTE_ACCESS somewhere inside the loop. Need to support multiple headers for DvmhData.
    checkInternal2(0, "Not implemented yet");
}

extern "C" void dvmh_loop_get_remote_buf_(const DvmType *pCurLoop, const DvmType *pRmaIndex, DvmType rmaDesc[]) {
    InterfaceFunctionGuard guard;
    checkInternal(pCurLoop && pRmaIndex);
    dvmh_loop_get_remote_buf_C(*pCurLoop, *pRmaIndex, rmaDesc);
}

extern "C" void dvmh_loop_cuda_register_red_C(DvmType curLoop, DvmType redIndex, void **arrayAddrPtr, void **locAddrPtr) {
    InterfaceFunctionGuard guard;
    checkInternal2(inited, "LibDVMH is not initialized");
    DvmhObject *obj = (DvmhObject *)curLoop;
    checkInternal2(obj, "NULL pointer is passed to dvmh_loop_cuda_register_red");
    checkInternal2(obj->isExactly<DvmhLoopCuda>(), "Incorrect loop reference is passed to dvmh_loop_cuda_register_red");
    DvmhLoopCuda *cloop = obj->as<DvmhLoopCuda>();
    assert(cloop);
    DvmhLoop *loop = cloop->getLoop();
    assert(loop);
    checkInternal3(redIndex >= 1 && redIndex <= (int)loop->reductions.size(), "Invalid reduction operation number (%d)", (int)redIndex);
    DvmhReductionCuda *reduction = new DvmhReductionCuda(loop->reductions[redIndex - 1], arrayAddrPtr, locAddrPtr, cloop->getDeviceNum());
    cloop->addReduction(reduction);
    checkInternal2(cloop->reductions.size() <= loop->reductions.size(), "Double registration in dvmh_loop_cuda_register_red is not allowed");
    dvmh_log(TRACE, "Reduction #%d registered for CUDA", (int)redIndex);
}

extern "C" void dvmh_loop_red_init_C(DvmType curLoop, DvmType redIndex, void *arrayAddr, void *locAddr) {
    InterfaceFunctionGuard guard;
    checkInternal2(inited, "LibDVMH is not initialized");
    checkInternal(arrayAddr);
    DvmhObject *obj = (DvmhObject *)curLoop;
    checkInternal2(obj, "NULL pointer is passed to dvmh_loop_red_init");
    checkInternal2(obj->is<DvmhSpecLoop>(), "Incorrect loop reference is passed to dvmh_loop_red_init");
    DvmhSpecLoop *sloop = obj->as<DvmhSpecLoop>();
    checkInternal3(redIndex >= 1 && redIndex <= (int)sloop->getLoop()->reductions.size(), "Invalid reduction operation number (%d)", (int)redIndex);
    DvmhReduction *red = sloop->getLoop()->reductions[redIndex - 1];
    assert(red);
    red->initValues(arrayAddr, locAddr);
}

extern "C" void dvmh_loop_red_init_(const DvmType *pCurLoop, const DvmType *pRedIndex, void *arrayAddr, void *locAddr) {
    InterfaceFunctionGuard guard;
    checkInternal(pCurLoop && pRedIndex);
    dvmh_loop_red_init_C(*pCurLoop, *pRedIndex, arrayAddr, locAddr);
}

extern "C" void dvmh_loop_cuda_red_init_C(DvmType curLoop, DvmType redIndex, void **devArrayAddrPtr, void **devLocAddrPtr) {
    InterfaceFunctionGuard guard;
    checkInternal2(inited, "LibDVMH is not initialized");
    DvmhObject *obj = (DvmhObject *)curLoop;
    checkInternal2(obj, "NULL pointer is passed to dvmh_loop_cuda_red_init");
    checkInternal2(obj->isExactly<DvmhLoopCuda>(), "Incorrect loop reference is passed to dvmh_loop_cuda_red_init");
    DvmhLoopCuda *cloop = obj->as<DvmhLoopCuda>();
    assert(cloop);
    checkInternal3(redIndex >= 1 && redIndex <= (int)cloop->getLoop()->reductions.size(), "Invalid reduction operation number (%d)", (int)redIndex);
    DvmhReduction *red = cloop->getLoop()->reductions[redIndex - 1];
    assert(red);
    char *arrayAddr = 0;
    if (devArrayAddrPtr)
        arrayAddr = new char[red->elemSize * red->elemCount];
    char *locAddr = 0;
    if (devLocAddrPtr && red->isLoc())
        locAddr = new char[red->locSize * red->elemCount];
    red->initValues(arrayAddr, locAddr);
    checkInternal(devices[cloop->getDeviceNum()]->getType() == dtCuda);
    CudaDevice *cudaDev = (CudaDevice *)devices[cloop->getDeviceNum()];
    if (devArrayAddrPtr) {
        *devArrayAddrPtr = cudaDev->allocBytes(red->elemSize * red->elemCount);
        cudaDev->setValue(*devArrayAddrPtr, arrayAddr, red->elemSize * red->elemCount);
        cloop->addToToFree(*devArrayAddrPtr);
    }
    if (devLocAddrPtr) {
        if (locAddr) {
            *devLocAddrPtr = cudaDev->allocBytes(red->locSize * red->elemCount);
            cudaDev->setValue(*devLocAddrPtr, locAddr, red->locSize * red->elemCount);
            cloop->addToToFree(*devLocAddrPtr);
        } else
            *devLocAddrPtr = 0;
    }
    delete[] locAddr;
    delete[] arrayAddr;
}

extern "C" void dvmh_loop_cuda_get_config_C(DvmType curLoop, DvmType sharedPerThread, DvmType regsPerThread, void *ainOutThreads, void *aoutStream,
        DvmType *outSharedPerBlock) {
    InterfaceFunctionGuard guard;
    dim3 *inOutThreads = (dim3 *)ainOutThreads;
    cudaStream_t *outStream = (cudaStream_t *)aoutStream;
    checkInternal2(inited, "LibDVMH is not initialized");
    DvmhObject *obj = (DvmhObject *)curLoop;
    checkInternal2(obj, "NULL pointer is passed to dvmh_loop_cuda_get_config");
    checkInternal2(obj->isExactly<DvmhLoopCuda>(), "Incorrect loop reference is passed to dvmh_loop_cuda_get_config");
    DvmhLoopCuda *cloop = obj->as<DvmhLoopCuda>();
    int block[3] = {0, 0, 0};
    if (inOutThreads && inOutThreads->x >= 1 && inOutThreads->y >= 1 && inOutThreads->z >= 1) {
        // Using outside-specified default value
        block[0] = inOutThreads->x;
        block[1] = inOutThreads->y;
        block[2] = inOutThreads->z;
    }
    if (sharedPerThread < 0)
        sharedPerThread = 0;
    if (regsPerThread <= 0)
        regsPerThread = ((CudaDevice *)devices[cloop->getDeviceNum()])->maxRegsPerThread;
    cloop->pickBlock(sharedPerThread, regsPerThread, block);
    if (inOutThreads) {
        inOutThreads->x = block[0];
        inOutThreads->y = block[1];
        inOutThreads->z = block[2];
        dvmh_log(TRACE, "Using CUDA block (%d, %d, %d)", block[0], block[1], block[2]);
    }
    if (outStream)
        *outStream = cloop->cudaStream;
    if (outSharedPerBlock)
        *outSharedPerBlock = block[0] * block[1] * block[2] * sharedPerThread;
    // XXX: Do not need it anymore
    cloop->counter = 0;
}

extern "C" void dvmh_loop_cuda_red_prepare_C(DvmType curLoop, DvmType redIndex, DvmType count, DvmType fillFlag) {
    InterfaceFunctionGuard guard;
    checkInternal2(inited, "LibDVMH is not initialized");
    DvmhObject *obj = (DvmhObject *)curLoop;
    checkInternal2(obj, "NULL pointer is passed to dvmh_loop_cuda_red_prepare");
    checkInternal2(obj->isExactly<DvmhLoopCuda>(), "Incorrect loop reference is passed to dvmh_loop_cuda_red_prepare");
    DvmhLoopCuda *cloop = obj->as<DvmhLoopCuda>();
    checkInternal3(redIndex >= 1 && redIndex <= (int)cloop->getLoop()->reductions.size(), "Invalid reduction operation number (%d)", (int)redIndex);
    DvmhReduction *red = cloop->getLoop()->reductions[redIndex - 1];
    assert(red);
    DvmhReductionCuda *cudaRed = cloop->getCudaRed(red);
    checkInternal2(cudaRed, "This reduction wasn't registered for CUDA execution");
    checkInternal3(count > 0, "Buffer length must be positive. Given " DTFMT, count);
    cudaRed->prepare(count, (fillFlag ? true : false));
}

static bool dvmhLoopHasElementCommon(DvmType curLoop, const DvmType dvmDesc[], DvmType givenRank, va_list &ap, bool fortranFlag) {
    checkInternal2(inited, "LibDVMH is not initialized");
    checkInternal(dvmDesc);
    DvmhObject *obj = (DvmhObject *)curLoop;
    checkInternal2(obj, "NULL pointer is passed to dvmh_loop_has_element");
    checkInternal2(obj->is<DvmhSpecLoop>(), "Incorrect loop reference is passed to dvmh_loop_has_element");
    DvmhSpecLoop *sloop = obj->as<DvmhSpecLoop>();
    obj = passOrGetDvmh(dvmDesc[0]);
    checkError2(obj, "NULL object is passed to dvmh_loop_has_element");
    checkInternal2(obj->isExactly<DvmhData>(), "Only array can be passed to dvmh_loop_has_element");
    DvmhData *data = obj->as<DvmhData>();
    checkError2(data->getRank() == givenRank, "Rank in indexing expression must be the same as in declaraion of the variable");
#ifdef NON_CONST_AUTOS
    DvmType indexArray[data->getRank()];
#else
    DvmType indexArray[MAX_ARRAY_RANK];
#endif
    extractArray(ap, data->getRank(), indexArray, (fortranFlag ? vptPointer : vptValue));
    bool res = data->convertGlobalToLocal2(indexArray);
    res = res && sloop->hasElement(data, indexArray);
    return res;
}

extern "C" DvmType dvmh_loop_has_element_C(DvmType curLoop, const DvmType dvmDesc[], DvmType rank, /* DvmType index */...) {
    va_list ap;
    va_start(ap, rank);
    bool res = dvmhLoopHasElementCommon(curLoop, dvmDesc, rank, ap, false);
    va_end(ap);
    return (res ? 1 : 0);
}

extern "C" DvmType dvmh_loop_has_element_(const DvmType *pCurLoop, const DvmType dvmDesc[], const DvmType *pRank, /* const DvmType *pIndex */...) {
    checkInternal(pCurLoop && pRank);
    va_list ap;
    va_start(ap, pRank);
    bool res = dvmhLoopHasElementCommon(*pCurLoop, dvmDesc, *pRank, ap, true);
    va_end(ap);
    return (res ? 1 : 0);
}

extern "C" void dvmh_loop_cuda_red_finish_C(DvmType curLoop, DvmType redIndex) {
    InterfaceFunctionGuard guard;
    checkInternal2(inited, "LibDVMH is not initialized");
    DvmhObject *obj = (DvmhObject *)curLoop;
    checkInternal2(obj, "NULL pointer is passed to dvmh_loop_cuda_red_finish");
    checkInternal2(obj->isExactly<DvmhLoopCuda>(), "Incorrect loop reference is passed to dvmh_loop_cuda_red_finish");
    DvmhLoopCuda *cloop = obj->as<DvmhLoopCuda>();
    checkInternal3(redIndex >= 1 && redIndex <= (int)cloop->getLoop()->reductions.size(), "Invalid reduction operation number (%d)", (int)redIndex);
    DvmhReduction *red = cloop->getLoop()->reductions[redIndex - 1];
    assert(red);
    cloop->finishRed(red);
}

extern "C" void dvmh_loop_red_post_C(DvmType curLoop, DvmType redIndex, const void *arrayAddr, const void *locAddr) {
    InterfaceFunctionGuard guard;
    checkInternal2(inited, "LibDVMH is not initialized");
    checkInternal(arrayAddr);
    DvmhObject *obj = (DvmhObject *)curLoop;
    checkInternal2(obj, "NULL pointer is passed to dvmh_loop_red_post");
    checkInternal2(obj->is<DvmhSpecLoop>(), "Incorrect loop reference is passed to dvmh_loop_red_post");
    DvmhSpecLoop *sloop = obj->as<DvmhSpecLoop>();
    checkInternal2(arrayAddr, "NULL pointer is passed to dvmh_loop_red_post");
    checkInternal3(redIndex >= 1 && redIndex <= (int)sloop->getLoop()->reductions.size(), "Incorrect reduction operation number (%d)", (int)redIndex);
    DvmhReduction *red = sloop->getLoop()->reductions[redIndex - 1];
    assert(red);
    if (sloop->getPortion()->getDoReduction())
        red->postValues(arrayAddr, locAddr);
}

extern "C" void dvmh_loop_red_post_(const DvmType *pCurLoop, const DvmType *pRedIndex, const void *arrayAddr, const void *locAddr) {
    InterfaceFunctionGuard guard;
    checkInternal(pCurLoop && pRedIndex);
    dvmh_loop_red_post_C(*pCurLoop, *pRedIndex, arrayAddr, locAddr);
}


static std::vector<std::vector<DvmhObject *> > scopedObjects;

extern "C" void dvmh_scope_start_() {
    checkInternal2(inited, "LibDVMH is not initialized");
    scopedObjects.push_back(std::vector<DvmhObject *>());
#ifndef NO_DVM
    begbl_();
#endif
}

extern "C" void dvmh_scope_insert_(const DvmType dvmDesc[]) {
    checkInternal2(inited, "LibDVMH is not initialized");
    checkInternal2(!scopedObjects.empty(), "No active scope present");
    DvmhObject *obj = (DvmhObject *)dvmDesc[0];
    checkInternal2(obj, "NULL pointer is passed to dvmh_scope_insert");
    checkError2(allObjects.find(obj) != allObjects.end(), "Unknown object is passed to dvmh_scope_insert");
    scopedObjects.back().push_back(obj);
}

extern "C" void dvmh_scope_end_() {
    checkInternal2(inited, "LibDVMH is not initialized");
    checkInternal2(!scopedObjects.empty(), "No active scope present");
    for (int i = (int)scopedObjects.back().size() - 1; i >= 0; i--) {
        // Inverse order of deletion
        dvmhDeleteObject(scopedObjects.back()[i], true);
    }
    scopedObjects.pop_back();
#ifndef NO_DVM
    // Extract information from libdvm
    s_ENVIRONMENT *dvmEnv = (s_ENVIRONMENT *)gEnvColl->List[gEnvColl->Count - 1];
    s_PRGBLOCK *PB = (s_PRGBLOCK *)dvmEnv->PrgBlock.List[dvmEnv->PrgBlock.Count - 1];
    std::vector<SysHandle *> toProcess;
    for (int i = dvmEnv->DisArrList.Count - 1; i >= 0; i--)
        toProcess.push_back(((s_DISARRAY *)dvmEnv->DisArrList.List[i])->HandlePtr);
    for (int i = dvmEnv->AMViewList.Count - 1; i >= 0; i--)
        toProcess.push_back(((s_AMVIEW *)dvmEnv->AMViewList.List[i])->HandlePtr);
    // Delete their mirrored objects
    for (int i = 0; i < (int)toProcess.size(); i++) {
        SysHandle *hndl = toProcess[i];
        DvmhObject *obj = 0;
        if (hndl->CrtBlockInd == PB->BlockInd && !!(obj = getDvmh(hndl)))
            dvmhDeleteObject(obj, true);
    }
    endbl_();
#endif
}


static std::vector<DvmType> openedIntervals;

extern "C" void dvmh_par_interval_start_C() {
    openedIntervals.push_back(currentLine);
    DvmType depth = openedIntervals.size();
    bploop_(&depth);
}

extern "C" void dvmh_seq_interval_start_C() {
    openedIntervals.push_back(currentLine);
    DvmType depth = openedIntervals.size();
    bsloop_(&depth);
}

extern "C" void dvmh_sp_interval_end_(){
    DvmType line = openedIntervals.back();
    openedIntervals.pop_back();
    DvmType depth = openedIntervals.size();
    enloop_(&depth, &line);
}

extern "C" void dvmh_usr_interval_start_C(DvmType userID) {
    openedIntervals.push_back(currentLine);
    DvmType depth = openedIntervals.size();
    binter_(&depth, &userID);
}

extern "C" void dvmh_usr_interval_start_(DvmType *pUserID) {
    checkInternal(pUserID);
    dvmh_usr_interval_start_C(*pUserID);
}

extern "C" void dvmh_usr_interval_end_() {
    DvmType line = openedIntervals.back();
    openedIntervals.pop_back();
    DvmType depth = openedIntervals.size();
    einter_(&depth, &line);
}


extern "C" DvmType dvmh_get_addr_(void *pVariable) {
    checkInternal(pVariable);
    return (DvmType)pVariable;
}

extern "C" DvmType dvmh_string_(const char s[], int len) {
    checkInternal(s && len >= 0);
    DvmType res = 1 + stringBuffer.size();
    stringBuffer.resize(stringBuffer.size() + (len + 1));
    char *myPlace = &stringBuffer[res - 1];
    memcpy(myPlace, s, len);
    myPlace[len] = 0;
    return res;
}

extern "C" DvmType dvmh_string_var_(char s[], int len) {
    checkInternal(s && len >= 0);
    stringVariables.push_back(std::make_pair(s, len));
    return stringVariables.size();
}

static bool fillAxisPerm(int rank, DvmType depMask, DvmType *idxPerm) {
    // idxPerm - permutation of loop axes. Numeration from one from outermost. Permutated => original
    int depCount = oneBitCount(depMask);
    int h = rank - depCount;
    int hd = 0;
    for (int i = 1; i <= rank; i++) {
        if ((depMask >> (rank - i)) & 1) {
            idxPerm[h] = i;
            h++;
        } else {
            idxPerm[hd] = i;
            hd++;
        }
    }
    return depCount > 0 && depCount < rank;
}

template <typename T>
static void applyPerm(int rank, T *vals, DvmType *idxPerm) {
#ifdef NON_CONST_AUTOS
    T tmp[rank];
#else
    T tmp[MAX_LOOP_RANK];
#endif
    for (int i = 1; i <= rank; i++)
        tmp[i - 1] = vals[idxPerm[i - 1] - 1];
    typedMemcpy(vals, tmp, rank);
}

extern "C" void dvmh_change_filled_bounds_C(DvmType boundsLow[], DvmType boundsHigh[], DvmType loopSteps[], DvmType rank, DvmType depMask, DvmType idxPerm[]) {
    InterfaceFunctionGuard guard;
    if (fillAxisPerm(rank, depMask, idxPerm)) {
        applyPerm(rank, boundsLow, idxPerm);
        applyPerm(rank, boundsHigh, idxPerm);
        applyPerm(rank, loopSteps, idxPerm);
    }
}

static bool fillAxisPerm2(int rank, DvmType depMask, DvmType *idxPerm) {
    // idxPerm - permutation of loop axes. Numeration from one from outermost. Permutated => original
    int depCount = oneBitCount(depMask);
    int h = 0;
    int hd = depCount;
    for (int i = 1; i <= rank; i++) {
        if ((depMask >> (rank - i)) & 1) {
            idxPerm[h] = i;
            h++;
        } else {
            idxPerm[hd] = i;
            hd++;
        }
    }
    return depCount > 0 && depCount < rank;
}

extern "C" void dvmh_change_filled_bounds2_C(DvmType boundsLow[], DvmType boundsHigh[], DvmType loopSteps[], DvmType rank, DvmType depMask, DvmType idxPerm[]) {
    InterfaceFunctionGuard guard;
    if (fillAxisPerm2(rank, depMask, idxPerm)) {
        applyPerm(rank, boundsLow, idxPerm);
        applyPerm(rank, boundsHigh, idxPerm);
        applyPerm(rank, loopSteps, idxPerm);
    }
    for (int i = 0; i < rank; ++i) {
        idxPerm[i]--;
    }
}

#ifndef NO_DVM
extern "C" DvmType *dvmh_get_dvm_header_C(const DvmType dvmDesc[]) {
    checkInternal2(inited, "LibDVMH is not initialized");
    checkInternal(dvmDesc);
    DvmhObject *obj = passOrGetOrCreateDvmh(dvmDesc[0], true);
    checkError2(obj, "NULL object is passed to dvmh_get_dvm_header");
    checkInternal2(obj->isExactly<DvmhData>(), "Only array can be passed to dvmh_get_dvm_header");
    DvmhData *data = obj->as<DvmhData>();
    SysHandle *dvmHandle = getDvm(data);
    checkInternal2(dvmHandle, "No tied DVM handle found");
    return (DvmType *)dvmHandle->HeaderPtr;
}
#endif
