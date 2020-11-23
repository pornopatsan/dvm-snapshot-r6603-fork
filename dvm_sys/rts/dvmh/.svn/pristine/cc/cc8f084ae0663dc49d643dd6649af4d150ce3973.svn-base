#pragma once

#include "dvmh_data.h"
#include "dvmh_types.h"
#include "dvmlib_incs.h"
#include "loop.h"

#include <map>
#include <vector>

namespace libdvmh {

class DvmhData;

class DvmhDebugReduction {
public:
    typedef DvmhReduction::RedFunction RedFunction;
public:
    RedFunction redFunc; // reduction function
    DvmhData::DataType arrayElementType; // reduction element type
    RedGroupRef redGroup; // reduction group
    DvmType redIndex; // Index in global reduction storage
    void *arrayAddr;
    std::string globalName;
public:
    void *localAddr;
    std::string currentName;
public:
    explicit DvmhDebugReduction(RedFunction aRedFunc, DvmhData::DataType VType, void *Mem, std::string name);
    ~DvmhDebugReduction() { free(localAddr); }
public:
    std::string description();
};

}
