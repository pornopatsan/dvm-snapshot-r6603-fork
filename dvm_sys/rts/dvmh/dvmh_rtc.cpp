#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <map>
#include <string>

#ifdef HAVE_CUDA
#pragma GCC visibility push(default)
#ifdef HAVE_NVRTC
#include <nvrtc.h>
#endif
#include <cuda.h>
#include <cuda_runtime.h>
#pragma GCC visibility pop
#endif

#include "include/dvmhlib_const.h"

#include "dvmh.h"
#include "dvmh_data.h"
#include "dvmh_device.h"
#include "dvmh_log.h"
#include "dvmh_rts.h"
#include "dvmh_stat.h"
#include "loop.h"
#include "mps.h"
#include "settings.h"

using namespace libdvmh;

namespace libdvmh {

#include "dynamic_include.h"

#ifdef HAVE_NVRTC

static const std::string dvmTypes = "typedef " DVMTYPE_STR " DvmType;\n"
        "typedef unsigned " DVMTYPE_STR " UDvmType;\n";

static const std::string typedefs = "typedef int __indexTypeInt;\n"
                                    "typedef long long __indexTypeLLong;\n";

#define checkInternalNvrtc(expr) do { \
    nvrtcResult _err = (expr); \
    checkInternal3(_err == NVRTC_SUCCESS, "NVRTC error occured in \"%s\": Error #%d - \"%s\"", #expr, _err, nvrtcGetErrorString(_err)); \
} while (0)

static const std::string rtcIncludes = dvmTypes + typedefs + dvmhlib_f2c + dvmhlib_device + dvmhlib_warp_red + dvmhlib_block_red;
static const std::string rtcIncludesWithPGI = rtcIncludes + pgi_include;

struct ValueContainer {
    union {
        void *ptr;
        char c;
        unsigned char uc;
        short sh;
        unsigned short ush;
        int i;
        unsigned int ui;
        long l;
        unsigned long ul;
        long long ll;
        unsigned long long ull;
        float f;
        double d;
        float_complex fc;
        double_complex dc;
    } data;
    DvmhData::DataType type;
public:
    ValueContainer(): type(DvmhData::dtUnknown) { data.ptr = 0; }
    explicit ValueContainer(DvmhData::DataType type, void *value): type(type) {
        if (type == DvmhData::dtUnknown)
            data.ptr = value;
        else
            memcpy(&data, value, DvmhData::getTypeSize(type));
    }
    explicit ValueContainer(va_list &vl) {
        type = (DvmhData::DataType)va_arg(vl, int);
        switch (type) {
            case DvmhData::dtUnknown: data.ptr = va_arg(vl, void *); break;
            case DvmhData::dtChar: data.c = (char)va_arg(vl, int); break;
            case DvmhData::dtUChar: data.uc = (unsigned char)va_arg(vl, unsigned int); break;
            case DvmhData::dtShort: data.sh = (short)va_arg(vl, int); break;
            case DvmhData::dtUShort: data.ush = (unsigned short)va_arg(vl, unsigned int); break;
            case DvmhData::dtInt: data.i = va_arg(vl, int); break;
            case DvmhData::dtUInt: data.ui = va_arg(vl, unsigned int); break;
            case DvmhData::dtLong: data.l = va_arg(vl, long); break;
            case DvmhData::dtULong: data.ul = va_arg(vl, unsigned long); break;
            case DvmhData::dtLongLong: data.ll = va_arg(vl, long long); break;
            case DvmhData::dtULongLong: data.ull = va_arg(vl, unsigned long long); break;
            case DvmhData::dtFloat: data.f = (float)va_arg(vl, double); break;
            case DvmhData::dtDouble: data.d = va_arg(vl, double); break;
            case DvmhData::dtFloatComplex:
                data.fc.re = (float)va_arg(vl, double);
                data.fc.im = (float)va_arg(vl, double);
                break;
            case DvmhData::dtDoubleComplex:
                data.dc.re = va_arg(vl, double);
                data.dc.im = va_arg(vl, double);
                break;
            case DvmhData::dtPointer:
                data.ptr = va_arg(vl, void *);
                break;
            default:
                checkInternal3(false, "Unknown data type %d encountered", (int)type);
        }
    }
public:
    bool operator==(const ValueContainer &other) const {
        return type == other.type && memcmp(&data, &other.data, DvmhData::getTypeSize(type)) == 0;
    }
    bool operator!=(const ValueContainer &other) const {
        return !(*this == other);
    }
};

class SpecializedNvrtcProgram: private Uncopyable {
public:
    CUmodule getModule() const { return module; }
public:
    explicit SpecializedNvrtcProgram(const std::string &srcCode, const std::vector<std::pair<int, ValueContainer> > &spArgs, bool usePGI): specsArgs(spArgs) {
        nvrtcProgram prog;
        const char progName[] = "rtc.cu";
        int devNum;
        cudaDeviceProp prop;
        cudaGetDevice(&devNum);
        cudaGetDeviceProperties(&prop, devNum);

        char compCap[32];
        compCap[sprintf(compCap, "-arch=compute_%d%d", prop.major, prop.minor)] = 0;
        const char *opts[4];
        int optCount = 0;
        opts[optCount++] = compCap;
        opts[optCount++] = "-default-device";
        if (prop.major <= 2)
            opts[optCount++] = "-DCUDA_FERMI_ARCH";
        if (usePGI)
            opts[optCount++] = "-DCUDA_NO_SM_20_INTRINSICS";

        bool possibleKernelPrint = false;
        char buf[1024];
        buf[0] = 0;
        int myGlobalRank = currentMPS->getCommRank();

        if (!dvmhSettings.logFile.empty()) {
            if (strchr(dvmhSettings.logFile.c_str(), '%')) {
                sprintf(buf, dvmhSettings.logFile.c_str(), myGlobalRank);
                possibleKernelPrint = true;
            }
        }

        if (dvmhSettings.logLevel >= DEBUG) {
            if (possibleKernelPrint) {
                static int kernel_num = 0;
                char tmp[1024];
                tmp[sprintf(tmp, "%s_kernel_%d.cu", buf, kernel_num)] = 0;
                std::ofstream file;
                file.open(tmp, std::ofstream::out);
                if (file.is_open()) {
                    file << "#include \"rtc_include.h\"" << std::endl;
                    file << srcCode.substr(rtcIncludes.size()).c_str() << std::endl;
                    file.close();
                    dvmh_log(DEBUG, "NVRTC: write kernel to '%s' file", tmp);
                } else {
                    dvmh_log(DEBUG, "NVRTC: can not open '%s' file", tmp);
                }

                if (myGlobalRank == 0 && kernel_num == 0) {
                    strcpy(tmp, "rtc_include.h");
                    std::ofstream file_inc;
                    file_inc.open(tmp, std::ofstream::out);
                    if (file_inc.is_open()) {
                        file_inc << rtcIncludes.c_str() << std::endl;
                        file_inc.close();
                        dvmh_log(DEBUG, "NVRTC: write include for RTC to '%s' file", tmp);
                    } else {
                        dvmh_log(DEBUG, "NVRTC: can not open '%s' file", tmp);
                    }
                }
                kernel_num++;
            } else {
                dvmh_log(DEBUG, "NVRTC: can not print kernels: log file is undefined");
            }
        }
        checkInternalNvrtc(nvrtcCreateProgram(&prog, srcCode.c_str(), progName, 0, NULL, NULL));
        //XXX: get reg count info in future CUDA toolkit
        nvrtcResult compileResult = nvrtcCompileProgram(prog, optCount, opts);
        size_t logSize = 0;
        checkInternalNvrtc(nvrtcGetProgramLogSize(prog, &logSize));
        if (logSize > 1 && (compileResult != NVRTC_SUCCESS || dvmhSettings.logLevel >= DEBUG)) {
            char *log = new char[logSize];
            if (logSize < 1024)
                dvmh_log(DEBUG, "NVRTC: log size = %.2f Bytes", float(logSize));
            else if (logSize < 1024 * 1024)
                dvmh_log(DEBUG, "NVRTC: log size = %.2f KB", logSize / 1024.0f);
            else
                dvmh_log(DEBUG, "NVRTC: log size = %.2f MB", logSize / (1024.0f * 1024.0f));

            checkInternalNvrtc(nvrtcGetProgramLog(prog, log));

            dvmh_log(compileResult == NVRTC_SUCCESS ? DEBUG : FATAL, "NVRTC: Compilation log:");
            char *newLog = new char[logSize];
            for (int i = 0, k = 0; i < (int)logSize; i++) {
                if (log[i] == '\n') {
                    newLog[k] = '\0';
                    dvmh_log(compileResult == NVRTC_SUCCESS ? DEBUG : FATAL, "%s", newLog);
                    k = 0;
                    i++;
                } else {
                    newLog[k] = log[i];
                    k++;
                }
            }
            delete[] newLog;
            delete[] log;

            if (compileResult != NVRTC_SUCCESS) {
                char tmp[1024];
                tmp[sprintf(tmp, "error_kernel_comp_%d.cu", myGlobalRank)] = 0;
                std::ofstream file;
                file.open(tmp, std::ofstream::out);
                if (file.is_open()) {
                    file << srcCode.c_str() << std::endl;
                    file.close();
                    dvmh_log(DEBUG, "NVRTC: write kernel to '%s' file", tmp);
                } else {
                    dvmh_log(DEBUG, "NVRTC: can not open '%s' file", tmp);
                }
            }
        }

        checkInternal(compileResult == NVRTC_SUCCESS);
        size_t ptxSize;
        checkInternalNvrtc(nvrtcGetPTXSize(prog, &ptxSize));
        ptxCode.resize(ptxSize - 1);
        checkInternalNvrtc(nvrtcGetPTX(prog, &ptxCode[0]));
        nvrtcDestroyProgram(&prog);
        checkInternalCU(cuModuleLoadData(&module, ptxCode.c_str()));
    }
public:
    bool isEqSpecArgs(std::vector<std::pair<int, ValueContainer> > &inArgs) {
        bool answer = true;
        if (inArgs.size() == specsArgs.size()) {
            for (unsigned k = 0; k < specsArgs.size(); k++) {
                if (inArgs[k] != specsArgs[k]) {
                    answer = false;
                    break;
                }
            }
        } else {
            answer = false;
        }

        return answer;
    }
public:
    ~SpecializedNvrtcProgram() {
        checkInternalCU(cuModuleUnload(module));
    }
protected:
    std::vector<std::pair<int, ValueContainer> > specsArgs; // Array of pairs (Number of arg, Value)
    std::string ptxCode;
    CUmodule module;
};

class NvrtcProgram: private Uncopyable {
public:
    int getArgType(int idx) const { assert(idx >= 0 && idx < (int)argDescs.size()); return argDescs[idx].first; }
public:
    explicit NvrtcProgram(const std::string &kerName, const std::string &srcText, const int numPar, bool PGI): kernelName(kerName), srcCode(srcText),
            argsPlace(2 * numPar), argDescs(numPar), numOfChanges(numPar, 0), dontReplace(numPar, false), actualValues(numPar), lastSpecsSelect(-1),
            fullDeprecate(false), usePGI(PGI)
    {
        int idx = 0;
        int place = srcCode.find(kernelName.c_str()) + strlen(kernelName.c_str()) + 1;
        argsPlace[idx] = place;
        idx++;

        // XXX: ',' appears in templates < > only ?? No, also in function types, anonymous struct types.
        while (srcCode[place] != ')') {
            if (srcCode[place] == '<') {
                while (srcCode[place] != '>')
                    place++;
            }

            if (srcCode[place] == ',') {
                argsPlace[idx] = place - argsPlace[idx - 1] + 1;
                argsPlace[idx + 1] = place + 1;
                idx += 2;
            }
            place++;
        }
        argsPlace[idx] = place - argsPlace[idx - 1];
        for (int i = 0; i < numPar; i++)
            actualValues[i].reserve(16);
        specs.reserve(16);
    }
public:
    void addArg(const int num, const std::string &argName, ValueContainer value) {
        argDescs[num] = std::make_pair(value.type, argName);
        numOfChanges[num] = 0;
        dontReplace[num] = argName.empty() || value.type == DvmhData::dtUnknown || value.type == DvmhData::dtPointer;
        actualValues[num].push_back(value);
    }
    bool checkArg(const int num, const std::string &argName, ValueContainer value) const {
        return num >= 0 && num < (int)argDescs.size() && argDescs[num].first == value.type && argDescs[num].second == argName;
    }
    void deprecateArgs(ValueContainer *args) {
        fullDeprecate = true;
        for (unsigned i = 0; i < argDescs.size(); i++) {
            if (dontReplace[i])
                continue;
            fullDeprecate = false;
            bool deprecate = true;
            if (args[i].type != DvmhData::dtUnknown) {
                for (unsigned k = 0; k < actualValues[i].size(); k++) {
                    if (actualValues[i][k] != args[i]) {
                        deprecate = false;
                        break;
                    }
                }
            } else {
                deprecate = false;
                dontReplace[i] = true;
                dvmh_log(DEBUG, "NVRTC: deprecate <%d> parameter", i);
            }

            if (deprecate) {
                numOfChanges[i]++;
                actualValues[i].push_back(args[i]);
                if (numOfChanges[i] > dvmhSettings.numVariantsForVarRtc) {
                    dontReplace[i] = true;
                    if (dvmhSettings.logLevel >= DEBUG) {
                        char oldVal[256] = {0};
                        char newVal[256] = {0};
                        std::string oldVals = "";
                        for (unsigned z = 0; z < actualValues[i].size(); z++) {
                            printByType(oldVal, actualValues[i][z]);
                            oldVals += oldVal;
                        }
                        printByType(newVal, args[i]);
                        dvmh_log(DEBUG, "NVRTC: deprecate <%d> parameter with name '%s' with oldVals: %s, newVal: %s", i, argDescs[i].second.c_str(),
                                oldVals.c_str(), newVal);
                    }
                }
            }
        }
        if (fullDeprecate)
            dvmh_log(INFO, "NVRTC: all parameters were deprecated");
    }
    SpecializedNvrtcProgram *getSpecialization(ValueContainer *args, std::vector<int> &excludeArgs) {
        std::vector<std::pair<int, ValueContainer> > specsArgs;
        if (dvmhSettings.specializeRtc) {
            if (!fullDeprecate)
                deprecateArgs(args);
            getSpecArgs(specsArgs, args);
        }

        if (specs.empty()) {
            // create first specialized version
            specs.push_back(new SpecializedNvrtcProgram(replaceArgsWithConstDecl(specsArgs), specsArgs, usePGI));
            lastSpecsSelect = specs.size() - 1;
            dvmh_log(INFO, "NVRTC: created <%d> specialization with %d arguments in kernel '%s'", lastSpecsSelect, (int)specsArgs.size(), kernelName.c_str());
        } else {
            // try to use previously specialized version
            if (!specs[lastSpecsSelect]->isEqSpecArgs(specsArgs)) {
                lastSpecsSelect = -1;
                // try to find specialized version
                for (unsigned k = 0; k < specs.size(); k++) {
                    if (specs[k]->isEqSpecArgs(specsArgs)) {
                        lastSpecsSelect = k;
                        break;
                    }
                }

                if (lastSpecsSelect == -1) {
                    specs.push_back(new SpecializedNvrtcProgram(replaceArgsWithConstDecl(specsArgs), specsArgs, usePGI));
                    lastSpecsSelect = specs.size() - 1;
                    dvmh_log(INFO, "NVRTC: created <%d> specialization with %d arguments in kernel '%s'", lastSpecsSelect, (int)specsArgs.size(), kernelName.c_str());
                }
            } else {
                dvmh_log(INFO, "NVRTC: used <%d> specialization with %d arguments in kernel '%s'", lastSpecsSelect, (int)specsArgs.size(), kernelName.c_str());
            }
        }

        for (unsigned i = 0; i < specsArgs.size(); i++)
            excludeArgs.push_back(specsArgs[i].first);

        return specs[lastSpecsSelect];
    }
public:
    ~NvrtcProgram() {
        while (!specs.empty()) {
            delete specs.back();
            specs.pop_back();
        }
    }
protected:
    void printByType(char *valStr, const ValueContainer &val, const char **pTypeStr = 0) {
        const char *typeStr;
        switch (val.type) {
            case DvmhData::dtChar:
                typeStr = "char";
                sprintf(valStr, "%d", val.data.c);
                break;
            case DvmhData::dtUChar:
                typeStr = "unsigned char";
                sprintf(valStr, "%uU", val.data.uc);
                break;
            case DvmhData::dtShort:
                typeStr = "short";
                sprintf(valStr, "%d", val.data.sh);
                break;
            case DvmhData::dtUShort:
                typeStr = "unsigned short";
                sprintf(valStr, "%uU", val.data.ush);
                break;
            case DvmhData::dtInt:
                typeStr = "int";
                sprintf(valStr, "%d", val.data.i);
                break;
            case DvmhData::dtUInt:
                typeStr = "unsigned int";
                sprintf(valStr, "%uU", val.data.ui);
                break;
            case DvmhData::dtLong:
                typeStr = "long";
                sprintf(valStr, "%ldL", val.data.l);
                break;
            case DvmhData::dtULong:
                typeStr = "unsigned long";
                sprintf(valStr, "%luUL", val.data.ul);
                break;
            case DvmhData::dtLongLong:
                typeStr = "long long";
                sprintf(valStr, "%lldLL", val.data.ll);
                break;
            case DvmhData::dtULongLong:
                typeStr = "unsigned long long";
                sprintf(valStr, "%lluULL", val.data.ull);
                break;
            case DvmhData::dtFloat:
                typeStr = "float";
                sprintf(valStr, "%.8ef", val.data.f);
                break;
            case DvmhData::dtDouble:
                typeStr = "double";
                sprintf(valStr, "%.17e", val.data.d);
                break;
            case DvmhData::dtFloatComplex:
                typeStr = "cmplx2";
                sprintf(valStr, "cmplx2(%.8ef, %.8ef)", val.data.fc.re, val.data.fc.im);
                break;
            case DvmhData::dtDoubleComplex:
                typeStr = "dcmplx2";
                sprintf(valStr, "dcmplx2(%.17e, %.17e)", val.data.dc.re, val.data.dc.im);
                break;
            default:
                assert(false);
        }
        if (pTypeStr)
            *pTypeStr = typeStr;
    }
    void printArgToBuf(std::string &printedArgs, std::vector<std::pair<int, ValueContainer> > &specsArgs) {
        for (unsigned k = 0; k < specsArgs.size(); k++) {
            char valStr[256] = {0};
            int num = specsArgs[k].first;
            const ValueContainer &val = specsArgs[k].second;
            const char *typeStr;
            const std::string &name = argDescs[num].second;

            printByType(valStr, val, &typeStr);
            printedArgs += std::string("const ") + typeStr + " " + name + " = " + valStr + ";\n";
        }
    }
    void getSpecArgs(std::vector<std::pair<int, ValueContainer> > &outArgs, ValueContainer *args) {
        for (unsigned i = 0; i < argDescs.size(); i++) {
            if (dontReplace[i] == false)
                outArgs.push_back(std::make_pair(i, args[i]));
        }
    }
    std::string replaceArgsWithConstDecl(std::vector<std::pair<int, ValueContainer> > &specsArgs)
    {
        std::string newCode(srcCode);
        if (specsArgs.size() != 0) {
            // remove args in kernel that should be replaced
            for (unsigned k = 0; k < specsArgs.size(); k++) {
                int place = argsPlace[2 * specsArgs[k].first];
                int len = argsPlace[2 * specsArgs[k].first + 1];
                newCode.replace(place, len, len, ' ');
            }

            // print to buffer new declarations of args that will be placed in kernel
            std::string printedArgs;
            printArgToBuf(printedArgs, specsArgs);

            // find and erase unnecessary ','
            int place = argsPlace[argsPlace.size() - 2] + argsPlace[argsPlace.size() - 1] - 1;
            while (newCode[place] == ' ')
                place--;
            if (newCode[place] == ',')
                newCode[place] = ' ';

            // insert new declarations
            place = argsPlace[argsPlace.size() - 2] + argsPlace[argsPlace.size() - 1] - 1;
            while (newCode[place] != '{')
                place++;
            place++;
            newCode.insert(place, printedArgs);
        }
        return newCode;
    }
protected:
    std::string kernelName;
    std::string srcCode;

    std::vector<int> argsPlace; // 2*i array of [from, len]
    std::vector<std::pair<DvmhData::DataType, std::string> > argDescs; // Array of pairs (Type, Name)
    std::vector<char> numOfChanges;
    std::vector<bool> dontReplace;
    std::vector<std::vector<ValueContainer> > actualValues;

    std::vector<SpecializedNvrtcProgram *> specs;
    int lastSpecsSelect;
    bool fullDeprecate;
    bool usePGI;
};

static std::map<std::string, NvrtcProgram *> *rtcDicts = 0;
static DvmhSpinLock dictsLock;

#endif

}

extern "C" void loop_cuda_rtc_set_lang(DvmType *InDvmhLoop, DvmType lang) {
#ifdef HAVE_NVRTC
    if (lang == C_CUDA)
        ((DvmhLoopCuda *)*InDvmhLoop)->kernelsUsePGI = false;
    else if (lang == FORTRAN_CUDA)
        ((DvmhLoopCuda *)*InDvmhLoop)->kernelsUsePGI = true;
    else
        checkInternal3(false, "NVRTC: CUDA-handler selected unknown CUDA-language with number '" DTFMT "'", lang);
#else
    dvmh_log(DEBUG, "RTS is compiled without NVRTC support");
#endif
}

extern "C" void loop_cuda_rtc_launch(DvmType *InDvmhLoop, const char *kernelName, const char *src, void *ablocks, DvmType numPar, ...) {
#ifdef HAVE_NVRTC
    dim3 *blocks = (dim3 *)ablocks;
    {
        SpinLockGuard guard(dictsLock);
        if (!rtcDicts) {
            int devCount = devicesCount;
            rtcDicts = new std::map<std::string, NvrtcProgram *>[devCount];
        }
    }
    DvmhTimer tm(true);
    va_list vl;
    va_start(vl, numPar);

    DvmhLoopCuda *cloop = (DvmhLoopCuda *)*InDvmhLoop;
    NvrtcProgram *prog = rtcDicts[cloop->getDeviceNum()][kernelName];
    bool newProgFlag = prog == 0;
    if (!prog) {
        if (cloop->kernelsUsePGI)
            prog = new NvrtcProgram(kernelName, rtcIncludesWithPGI + std::string(src), numPar, cloop->kernelsUsePGI);
        else
            prog = new NvrtcProgram(kernelName, rtcIncludes + std::string(src), numPar, cloop->kernelsUsePGI);
        rtcDicts[cloop->getDeviceNum()][kernelName] = prog;
    }

    ValueContainer *gotValues = new ValueContainer[numPar];
    for (int i = 0; i < numPar; i++) {
        const char *name = va_arg(vl, const char *);
        gotValues[i] = ValueContainer(vl);

        if (newProgFlag)
            prog->addArg(i, name, gotValues[i]);
        else
            checkInternal3(prog->checkArg(i, name, gotValues[i]), "Argument type or name has changed. Argument #%d, type %d, name %s.", i + 1,
                    (int)gotValues[i].type, name);
    }
    va_end(vl);

    std::vector<int> excludeArgs;
    SpecializedNvrtcProgram *spec = prog->getSpecialization(gotValues, excludeArgs);

    double rtcTime = tm.total();
    dvmh_log(INFO, "NVRTC: time of additional work to compile or select kernel - %.4f ms", rtcTime * 1000.0);
    dvmh_stat_add_measurement(cloop->cudaDev->index, DVMH_STAT_METRIC_UTIL_RTC_COMPILATION, rtcTime, 0.0, rtcTime);

    CUfunction kernel;
    checkInternalCU(cuModuleGetFunction(&kernel, spec->getModule(), kernelName));

    int block[3];
    ((CudaHandlerOptimizationParams *)cloop->getPortion()->getOptParams())->getBlock(block);

    void **passParams = new void *[numPar - excludeArgs.size()];
    {
        std::sort(excludeArgs.begin(), excludeArgs.end());
        int toSkip = 0;
        int nextExclIdx = 0;
        int nextExcl = (excludeArgs.empty() ? -1 : excludeArgs.front());
        for (int i = 0; i < numPar; i++) {
            if (i != nextExcl) {
                passParams[i - toSkip] = (gotValues[i].type == DvmhData::dtUnknown ? gotValues[i].data.ptr : &gotValues[i].data);
            } else {
                toSkip++;
                nextExclIdx++;
                nextExcl = (nextExclIdx < (int)excludeArgs.size() ? excludeArgs[nextExclIdx] : -1);
            }
        }
    }
    checkInternalCU(cuLaunchKernel(kernel, blocks->x, blocks->y, blocks->z, block[0], block[1], block[2], cloop->dynSharedPerBlock, cloop->cudaStream,
            passParams, 0));
    delete[] passParams;

    delete[] gotValues;
    // XXX: sync there needed since CUDA runtime's cudaDeviceSynchronize do not catch errors from such launch
    checkInternalCU(cuCtxSynchronize());
#else
    checkInternal2(0, "RTS is compiled without NVRTC support");
#endif
}
