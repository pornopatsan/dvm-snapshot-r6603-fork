#include "dvmh_debug.h"

#include "include/dvmhlib2.h"
#include "dvmlib_incs.h"

#include "dvmh_log.h"
#include "dvmh_data.h"
#include "dvmh_rts.h"
#include "loop.h"

#include <stdlib.h>
#include <sstream>

using namespace libdvmh;

namespace libdvmh {

typedef std::map<DvmType, DvmhDebugReduction*> ReductionsMap;
typedef std::map<std::string, DvmhDebugReduction*> MappingsMap;

struct DebugLoopData {
    DvmType no;
    UDvmType line;
    std::string filename;

    DebugLoopData(DvmType aNo, UDvmType aLine, const char *aFilename)
    : no(aNo), line(aLine), filename(std::string(aFilename)) { }
};


std::map<DvmhLoop*, RedGroupRef> debugRedGroups;

std::pair<void*, DvmhDebugReduction*> currentWriting = std::make_pair((void*)NULL, (DvmhDebugReduction*)NULL);

ReductionsMap currentReductions;
MappingsMap currentMappings;
RedGroupRef currentRedGroup;

std::vector<DebugLoopData> debugLoopsStack;

template<typename T>
inline std::string toStr(T v) {
    std::stringstream ss;
    ss << v;
    return ss.str();
}

DvmhDebugReduction::DvmhDebugReduction(RedFunction aRedFunc, DvmhData::DataType VType, void *Mem, std::string name) {
    redFunc = aRedFunc;
    arrayElementType = VType;
    arrayAddr = Mem;
    globalName = name;

    localAddr = calloc(1, DvmhData::getTypeSize(VType));
    currentName = "";
    redGroup = 0;
}

std::string DvmhDebugReduction::description() {
    switch (arrayElementType) {
        case DvmhData::dtUnknown: return "Unknown: " + toStr(*((int*)localAddr));
        case DvmhData::dtChar: return "Char: " + toStr(*((char*)localAddr));
        case DvmhData::dtUChar: return "UChar: " + toStr(*((char*)localAddr));
        case DvmhData::dtShort: return "Short: " + toStr(*((short*)localAddr));
        case DvmhData::dtUShort: return "UShort: " + toStr(*((short*)localAddr));
        case DvmhData::dtInt: return "Int: " + toStr(*((int*)localAddr));
        case DvmhData::dtUInt: return "UInt: " + toStr(*((int*)localAddr));
        case DvmhData::dtLong: return "Long: " + toStr(*((long*)localAddr));
        case DvmhData::dtULong: return "ULong: " + toStr(*((long*)localAddr));
        case DvmhData::dtLongLong: return "LongLong: " + toStr(*((long long*)localAddr));
        case DvmhData::dtULongLong: return "ULongLong: " + toStr(*((long long*)localAddr));
        case DvmhData::dtFloat: return "Float: " + toStr(*((float*)localAddr));
        case DvmhData::dtDouble: return "Double: " + toStr(*((double*)localAddr));
        case DvmhData::dtFloatComplex: return "FloatComplex: <some value>";
        case DvmhData::dtDoubleComplex: return "DoubleComplex: <some value>";
        case DvmhData::dtLogical: return "Logical: " + toStr(*((int*)localAddr));
        case DvmhData::dtPointer: return "Pointer: " + toStr(*((void**)localAddr));
        default: assert(false);
    }
}

}

// Debug runtime API functions

// Before writing to the variable
DvmType dvmh_dbg_before_write_var_C(DvmType plType, DvmType addr, DvmType handle, char* szOperand) {
    AddrType varAddr = addr;

    MappingsMap::iterator redVar = currentMappings.find(std::string(szOperand));
    if (redVar != currentMappings.end()) {
        currentWriting = std::make_pair((void*)varAddr, redVar->second);
        varAddr = (AddrType)redVar->second->localAddr;
    }

    return dprstv_(&plType, &varAddr, &handle, szOperand, -1);
}

// After writing to the variable
DvmType dvmh_dbg_after_write_var_C() {
    if (currentWriting.first != NULL) {
        size_t typeSize = DvmhData::getTypeSize(currentWriting.second->arrayElementType);
        memcpy(currentWriting.second->localAddr, currentWriting.first, typeSize);
        currentWriting = std::make_pair((void*)NULL, (DvmhDebugReduction*)NULL);
    }

    return dstv_();
}

// Reading from variable
DvmType dvmh_dbg_read_var_C(DvmType plType, DvmType addr, DvmType handle, char *szOperand) {
    AddrType _addr = addr;
    return dldv_(&plType, &_addr, &handle, szOperand, -1);
}

// Sequental loop beginning
DvmType dvmh_dbg_loop_seq_start_C(DvmType no) {
    debugLoopsStack.push_back(DebugLoopData(no, (UDvmType)currentLine, currentFile));
    return dbegsl_(&no);
}

// End of loop
DvmType dvmh_dbg_loop_end_C() {
    assert(!debugLoopsStack.empty());
    DebugLoopData data = debugLoopsStack.back();
    debugLoopsStack.pop_back();

    // NOTE: Record end of the loop with the same line number as beggining
    //       (this logic is more stable with several converter phases)
    dvmh_line_C((DvmType)data.line, data.filename.c_str());
    return dendl_(&data.no, &data.line);
}

// Next iteration beginning
DvmType dvmh_dbg_loop_iter_C(DvmType rank, ...) {
#ifdef NON_CONST_AUTOS
    AddrType indexes[rank];
    DvmType index_types[rank];
#else
    AddrType indexes[MAX_LOOP_RANK];
    DvmType index_types[MAX_LOOP_RANK];
#endif

    va_list pars;
    va_start(pars, rank);
    for (int i = 0; i < rank; ++i) {
        indexes[i] = va_arg(pars, AddrType);
    }
    for (int i = 0; i < rank; i++) {
        index_types[i] = va_arg(pars, DvmType);
    }
    va_end(pars);

    return diter_(indexes, index_types);
}

DvmType dvmh_dbg_loop_par_start_C(DvmType no, DvmType rank, ...) {
#ifdef NON_CONST_AUTOS
    DvmType init[rank], last[rank], step[rank];
#else
    DvmType init[MAX_LOOP_RANK], last[MAX_LOOP_RANK], step[MAX_LOOP_RANK];
#endif

    va_list pars;
    va_start(pars, rank);
    for (int i = 0; i < rank; i++) {
        init[i] = va_arg(pars, DvmType);
    }
    for (int i = 0; i < rank; i++) {
        last[i] = va_arg(pars, DvmType);
    }
    for (int i = 0; i < rank; i++) {
        step[i] = va_arg(pars, DvmType);
    }
    va_end(pars);

    debugLoopsStack.push_back(DebugLoopData(no, (UDvmType)currentLine, currentFile));

    return dbegpl_(&rank, &no, init, last, step);
}

void dvmh_dbg_loop_red_group_create_C(DvmType curLoop) {
    DvmhObject *obj = (DvmhObject *)curLoop;
    checkInternal2(obj, "NULL pointer is passed to dvmh_dbg_loop_red_group_create");
    checkInternal2(obj->is<DvmhLoop>(), "Incorrect loop reference is passed to dvmh_dbg_loop_red_group_create");
    DvmhLoop *loop = obj->as<DvmhLoop>();
    assert(loop);
    debugRedGroups[loop] = dcrtrg_();

    currentRedGroup = debugRedGroups[loop];
}

void dvmh_dbg_loop_global_red_init_C(DvmType curLoop, DvmType redIndex, void *arrayAddr, char *name) {
    checkInternal(arrayAddr);
    DvmhObject *obj = (DvmhObject *)curLoop;
    checkInternal2(obj, "NULL pointer is passed to dvmh_dbg_loop_global_red_init");
    checkInternal2(obj->is<DvmhLoop>(), "Incorrect loop reference is passed to dvmh_dbg_loop_global_red_init");

    DvmhLoop *loop = obj->as<DvmhLoop>();
    DvmhReduction *red = loop->reductions[redIndex - 1];
    assert(red);

    DvmType redFuncNumb = (int)red->redFunc;
    DvmType redArrayType = (int)red->arrayElementType;
    DvmType redArrayLength = red->elemCount;

    DvmType locElmLength = 0;
    DvmType locIndType = 0;

    if (red->isLoc()) {
        locElmLength = red->locSize;
        if (red->locSize % sizeof(long) == 0)
            locIndType = 0;
        else if (red->locSize % sizeof(int) == 0)
            locIndType = 1;
        else if (red->locSize % sizeof(short) == 0)
            locIndType = 2;
        else
            locIndType = 3;
    }

    currentReductions[redIndex] = new DvmhDebugReduction(red->redFunc, red->arrayElementType, red->arrayAddr, std::string(name));
    currentReductions[redIndex]->redGroup = debugRedGroups[loop];

    dinsrd_(&debugRedGroups[loop], &redFuncNumb, currentReductions[redIndex]->localAddr, &redArrayType, &redArrayLength, red->locAddr, &locElmLength, &locIndType);
}

extern "C" void dvmh_dbg_loop_handler_red_init_C(DvmType curLoop, DvmType redIndex, void *arrayAddr, char *name) {
    checkInternal(arrayAddr);
    DvmhObject *obj = (DvmhObject *)curLoop;
    checkInternal2(obj, "NULL pointer is passed to dvmh_dbg_loop_handler_red_init");
    checkInternal2(obj->is<DvmhSpecLoop>(), "Incorrect loop reference is passed to dvmh_dbg_loop_handler_red_init");

    DvmhSpecLoop *sloop = obj->as<DvmhSpecLoop>();
    DvmhLoop *loop = sloop->getLoop();
    DvmhReduction *red = loop->reductions[redIndex - 1];
    assert(red);

    assert(currentReductions[redIndex] != NULL);
    currentReductions[redIndex]->currentName = std::string(name);
    currentMappings[std::string(name)] = currentReductions[redIndex];

    // Fill internal reduction variable with value from previous handler call
    size_t typeSize = DvmhData::getTypeSize(currentReductions[redIndex]->arrayElementType);
    memcpy(arrayAddr, currentReductions[redIndex]->localAddr, typeSize);
}

void dvmh_dbg_loop_red_group_delete_C(DvmType curLoop) {
    DvmhObject *obj = (DvmhObject *)curLoop;
    checkInternal2(obj, "NULL pointer is passed to dvmh_dbg_loop_red_group_delete");
    checkInternal2(obj->is<DvmhLoop>(), "Incorrect loop reference is passed to dvmh_dbg_loop_red_group_delete");
    DvmhLoop *loop = obj->as<DvmhLoop>();
    assert(loop);

    assert(debugRedGroups[loop] == currentRedGroup);

    for (ReductionsMap::iterator it = currentReductions.begin(); it != currentReductions.end(); ++it) {
        if (it->second->redGroup == currentRedGroup) {
            // Fill internal reduction variable with calculated value
            size_t typeSize = DvmhData::getTypeSize(it->second->arrayElementType);
            memcpy(it->second->localAddr, it->second->arrayAddr, typeSize);
        }
    }

    dclcrg_(&debugRedGroups[loop]);
    ddelrg_(&debugRedGroups[loop]);

    //return;

    for (MappingsMap::iterator it = currentMappings.begin(); it != currentMappings.end();) {
        if (it->second->redGroup == currentRedGroup) {
            currentMappings.erase(it++);
        } else {
            ++it;
        }
    }

    for (ReductionsMap::iterator it = currentReductions.begin(); it != currentReductions.end();) {
        if (it->second->redGroup == currentRedGroup) {
            delete it->second;
            currentReductions.erase(it++);
        } else {
            ++it;
        }
    }

    debugRedGroups.erase(loop);
    currentRedGroup = 0;
}
