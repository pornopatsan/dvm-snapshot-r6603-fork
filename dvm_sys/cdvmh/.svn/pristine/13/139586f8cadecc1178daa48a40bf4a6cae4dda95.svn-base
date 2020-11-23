#pragma once

#include <map>
#include <vector>
#include <string>

#include "utils.h"

namespace cdvmh {

typedef unsigned char DebugLevel;
const DebugLevel dlWriteArrays = 1 << 0;
const DebugLevel dlReadArrays = 1 << 1;
const DebugLevel dlWriteVariables = 1 << 2;
const DebugLevel dlReadVariables = 1 << 3;

struct ConverterOptions {
    bool autoTfm;
    bool oneThread;
    bool noCuda;
    bool noH;
    bool emitBlankHandlers;
    bool lessDvmLines;
    bool savePragmas;
    bool extraComments;
    bool displayWarnings;
    bool verbose;
    bool seqOutput;
    bool doOpenMP;
    bool paralOutside;
    bool enableIndirect;
    bool linearRefs;
    bool useBlank;
    bool useOmpReduction;
    int perfDbgLvl;
    DebugLevel dvmDebugLvl;
    bool useDvmhStdio;
    bool useVoidStdio;
    std::vector<std::string> includeDirs;
    std::vector<std::pair<std::string, std::string> > addDefines;
    std::vector<std::string> removeDefines;
    std::string dvmhLibraryEntry;
    std::vector<std::string> inputFiles;
    std::vector<std::string> outputFiles;
    std::vector<std::string> languages;
    ConverterOptions() {
        init();
    }
    void init();
    void setFromArgs(int argc, char *argv[]);
protected:
    void addDefine(std::string def);
};

struct InputFile {
    std::string fileName;
    std::string canonicalName;
    std::string baseName;
    std::string shortName;
    bool isCompilable;
    bool CPlusPlus;
    std::string debugPreConvFileName;
    std::string canonicalDebugPreConv;
    std::string convertedFileName;
    std::string canonicalConverted;
    std::string outCXXName;
    std::string outBlankName;
    std::string outHostName;
    std::string outCudaName;
    std::string outCudaInfoName;
    explicit InputFile(const std::string &aFileName, std::string forcedLanguage = std::string(), std::string convName = std::string());
};

class ProjectContext {
public:
    const ConverterOptions &getOptions() const { return options; }
    int getFileCount() const { return inputFiles.size(); }
    const InputFile &getInputFile(int idx) const { return inputFiles[idx]; }
public:
    explicit ProjectContext(const ConverterOptions &opts);
public:
    bool hasInputFile(std::string fullName, bool isCanonical = false) const {
        return nameToIdx.find(isCanonical ? fullName : getCanonicalFileName(fullName)) != nameToIdx.end();
    }
    const InputFile &getInputFile(std::string fullName, bool isCanonical = false) {
        return inputFiles[nameToIdx[(isCanonical ? fullName : getCanonicalFileName(fullName))]];
    }
    bool hasCudaReplacement(const std::string &name) const {
        return cudaReplacementNames.find(name) != cudaReplacementNames.end();
    }
    std::string getCudaReplacement(const std::string &name) const {
        std::map<std::string, std::string>::const_iterator it = cudaReplacementNames.find(name);
        return (it != cudaReplacementNames.end() ? it->second : "");
    }
    bool hasDvmhReplacement(const std::string &name) const {
        return dvmhReplacementNames.find(name) != dvmhReplacementNames.end();
    }
    std::string getDvmhReplacement(const std::string &name, bool canBeVoid = false, bool forDistribArrays = false) const;
protected:
    ConverterOptions options;

    std::vector<InputFile> inputFiles;
    std::map<std::string, int> nameToIdx;
    std::map<std::string, std::string> cudaReplacementNames; // All the functions and data types which are available in CUDA from device functions, like fabs (which is originally from math.h (kind of)).
    std::map<std::string, std::string> dvmhReplacementNames; // All the functions, global variables and data types which have own implementation in LibDVMH.
};

}
