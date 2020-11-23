#include "project_ctx.h"

#include <cassert>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <algorithm>

#include "cdvmh_log.h"
#include "messages.h"

namespace cdvmh {

// ConverterOptions

void ConverterOptions::init() {
    autoTfm = false;
    oneThread = false;
    noCuda = false;
    noH = false;
    emitBlankHandlers = false;
    lessDvmLines = false;
    savePragmas = false;
    extraComments = false;
    displayWarnings = false;
    verbose = false;
    seqOutput = false;
    doOpenMP = true;
    paralOutside = false;
    enableIndirect = false;
    linearRefs = true;
    useBlank = false;
    useOmpReduction = false;
    perfDbgLvl = 0;
    dvmDebugLvl = 0;
    useDvmhStdio = true;
    useVoidStdio = true;
    includeDirs.clear();
    addDefines.clear();
    removeDefines.clear();
    dvmhLibraryEntry.clear();
    inputFiles.clear();
    outputFiles.clear();
    languages.clear();
}

void ConverterOptions::setFromArgs(int argc, char *argv[]) {
    init();
    std::string outputFile;
    std::string language;
    for (int i = 0; i < argc; i++) {
        if (!strcmp(argv[i], "-autoTfm")) {
            autoTfm = true;
        } else if (!strcmp(argv[i], "-oneThread")) {
            oneThread = true;
        } else if (!strcmp(argv[i], "-noCuda")) {
            noCuda = true;
        } else if (!strcmp(argv[i], "-I")) {
            i++;
            includeDirs.push_back(argv[i]);
        } else if (!strncmp(argv[i], "-I", 2)) {
            includeDirs.push_back(argv[i] + 2);
        } else if (!strcmp(argv[i], "-D")) {
            i++;
            addDefine(argv[i]);
        } else if (!strncmp(argv[i], "-D", 2)) {
            addDefine(argv[i] + 2);
        } else if (!strcmp(argv[i], "-U")) {
            i++;
            removeDefines.push_back(argv[i]);
        } else if (!strncmp(argv[i], "-U", 2)) {
            removeDefines.push_back(argv[i] + 2);
        } else if (!strcmp(argv[i], "-o")) {
            i++;
            outputFile = argv[i];
        } else if (!strcmp(argv[i], "-x")) {
            i++;
            language = argv[i];
            checkUserErrN(language == "c" || language == "cxx", "", 1, 13, language.c_str());
        } else if (!strcmp(argv[i], "-s")) {
            seqOutput = true;
        } else if (!strcmp(argv[i], "-p")) {
            seqOutput = false;
        } else if (!strcmp(argv[i], "-no-omp")) {
            doOpenMP = false;
        } else if (!strcmp(argv[i], "-Opl")) {
            paralOutside = true;
        } else if (!strcmp(argv[i], "-enable-indirect")) {
            enableIndirect = true;
        } else if (!strcmp(argv[i], "-Ohost")) {
            linearRefs = false;
        } else if (!strcmp(argv[i], "-use-blank")) {
            useBlank = true;
        } else if (!strcmp(argv[i], "-omp-reduction")) {
            useOmpReduction = true;
        } else if (!strcmp(argv[i], "-d1")) {
            dvmDebugLvl = dlWriteArrays;
        } else if (!strcmp(argv[i], "-d2")) {
            dvmDebugLvl = dlWriteArrays | dlReadArrays;
        } else if (!strcmp(argv[i], "-d3")) {
            dvmDebugLvl = dlWriteArrays | dlWriteVariables;
        } else if (!strcmp(argv[i], "-d4")) {
            dvmDebugLvl = dlWriteArrays | dlReadArrays | dlWriteVariables | dlReadVariables;
        } else if (!strcmp(argv[i], "-e1")) {
            perfDbgLvl = 1;
        } else if (!strcmp(argv[i], "-e2")) {
            perfDbgLvl = 2;
        } else if (!strcmp(argv[i], "-e3")) {
            perfDbgLvl = 3;
        } else if (!strcmp(argv[i], "-e4")) {
            perfDbgLvl = 4;
        } else if (!strcmp(argv[i], "-v")) {
            verbose = true;
        } else if (!strcmp(argv[i], "-w")) {
            displayWarnings = true;
        } else if (!strcmp(argv[i], "-dvm-stdio")) {
            useDvmhStdio = false;
        } else if (!strcmp(argv[i], "-no-void-stdio")) {
            useVoidStdio = false;
        } else if (!strcmp(argv[i], "-noH")) {
            noH = true;
        } else if (!strcmp(argv[i], "-dvm-entry")) {
            i++;
            dvmhLibraryEntry = argv[i];
        } else if (!strcmp(argv[i], "-emit-blank-handlers")) {
            emitBlankHandlers = true;
        } else if (!strcmp(argv[i], "-less-dvmlines")) {
            lessDvmLines = true;
        } else if (!strcmp(argv[i], "-save-pragmas")) {
            savePragmas = true;
        } else if (!strcmp(argv[i], "-extra-comments")) {
            extraComments = true;
        } else if (!strncmp(argv[i], "-", 1)) {
            userErrN("", 1, 12, argv[i]);
        } else {
            // If no option recognized - it is an input file
            inputFiles.push_back(argv[i]);
            outputFiles.push_back(outputFile);
            languages.push_back(language);
            outputFile.clear();
        }
    }
    if (!outputFile.empty() && !outputFiles.empty())
        outputFiles.back() = outputFile;
    if (seqOutput || (noH && !paralOutside))
        doOpenMP = false;
    if (dvmDebugLvl > 0) {
        noH = true;
    }
}

void ConverterOptions::addDefine(std::string def) {
    if (def.find('=') != def.npos)
        addDefines.push_back(std::make_pair(def.substr(0, def.find('=')), def.substr(def.find('=') + 1)));
    else
        addDefines.push_back(std::make_pair(def, ""));
}

// InputFile

InputFile::InputFile(const std::string &aFileName, std::string forcedLanguage, std::string convName) {
    fileName = aFileName;
    canonicalName = getCanonicalFileName(fileName);
    baseName = getBaseName(fileName);
    std::string ext = toLower(getExtension(baseName));
    checkUserErrN(!ext.empty(), "", 1, 14, fileName.c_str());
    shortName = baseName.substr(0, baseName.size() - ext.size() - 1);
    isCompilable = ext != "h" && ext != "hpp";
    if (forcedLanguage.empty())
        CPlusPlus = ext == "cpp" || ext == "cc" || ext == "cxx" || ext == "hpp";
    else
        CPlusPlus = forcedLanguage == "cxx";
    if (convName.empty())
        convName = shortName + ".DVMH." + (isCompilable ? "c" : "h") + (CPlusPlus ? "pp" : "");
    std::string convExt = toLower(getExtension(convName));
    if (isCompilable) {
        if (CPlusPlus)
            checkUserErrN(convExt == "cpp" || convExt == "cc" || convExt == "cxx", "", 1, 15, convExt.c_str());
        else
            checkUserErrN(convExt == "c", "", 1, 16, convExt.c_str());
    } else {
        if (CPlusPlus)
            checkUserErrN(convExt == "h" || convExt == "hpp", "", 1, 17, convExt.c_str());
        else
            checkUserErrN(convExt == "h", "", 1, 18, convExt.c_str());
    }
    convertedFileName = convName;
    canonicalConverted = getCanonicalFileName(convertedFileName);
    debugPreConvFileName = shortName + ".DVMHDEBUG." + (isCompilable ? "c" : "h") + (CPlusPlus ? "pp" : "");
    canonicalDebugPreConv = getCanonicalFileName(debugPreConvFileName);
    if (isCompilable) {
        std::string convShort = convName.substr(0, convName.size() - convExt.size() - 1);
        outCXXName = convShort + "_cxx.cpp";
        outBlankName = convShort + "_blank." + (CPlusPlus ? "cpp" : "c");
        outHostName = convShort + "_host." + (CPlusPlus ? "cpp" : "c");
        outCudaName = convShort + "_cuda.cu";
        outCudaInfoName = convShort + "_cuda_info.c";
    }
}

// ProjectContext

static void addSame(std::map<std::string, std::string> &m, const std::string &name) {
    m[name] = name;
}

static void addPrefix(std::map<std::string, std::string> &m, const std::string &prefix, const std::string &name, bool voidOption = false,
        bool distribOption = false) {
    m[name] = prefix + name;
    if (voidOption)
        m["void " + name] = prefix + "void_" + name;
    if (distribOption)
        m["distrib " + name] = prefix + name + "_distrib";
    if (voidOption && distribOption)
        m[std::string("void ") + "distrib " + name] = prefix + "void_" + name + "_distrib";
}

ProjectContext::ProjectContext(const ConverterOptions &opts) {
    options = opts;
    for (int i = 0; i < (int)opts.inputFiles.size(); i++) {
        std::string fileName(opts.inputFiles[i]);
        checkUserErrN(fileExists(fileName), "", 1, 19, fileName.c_str());
        InputFile file(fileName, opts.languages[i], opts.outputFiles[i]);
        inputFiles.push_back(file);
        nameToIdx[file.canonicalName] = inputFiles.size() - 1;
    }

    addSame(cudaReplacementNames, "char");
    addSame(cudaReplacementNames, "signed char");
    addSame(cudaReplacementNames, "unsigned char");
    addSame(cudaReplacementNames, "short");
    addSame(cudaReplacementNames, "unsigned short");
    addSame(cudaReplacementNames, "int");
    addSame(cudaReplacementNames, "unsigned int");
    addSame(cudaReplacementNames, "long");
    addSame(cudaReplacementNames, "unsigned long");
    addSame(cudaReplacementNames, "long long");
    addSame(cudaReplacementNames, "unsigned long long");
    addSame(cudaReplacementNames, "float");
    // not in CUDA addSame(cudaReplacementNames, "float_t");
    addSame(cudaReplacementNames, "double");
    // not in CUDA addSame(cudaReplacementNames, "double_t");
    addSame(cudaReplacementNames, "long double");
    addSame(cudaReplacementNames, "size_t");
    addSame(cudaReplacementNames, "bool");

    // not in CUDA addSame(cudaReplacementNames, "fpclassify");
    addSame(cudaReplacementNames, "isfinite");
    addSame(cudaReplacementNames, "isinf");
    addSame(cudaReplacementNames, "isnan");
    // not in CUDA addSame(cudaReplacementNames, "isnormal");
    addSame(cudaReplacementNames, "signbit");
    addSame(cudaReplacementNames, "acos");
    addSame(cudaReplacementNames, "acosf");
    // not in CUDA addSame(cudaReplacementNames, "acosl");
    addSame(cudaReplacementNames, "asin");
    addSame(cudaReplacementNames, "asinf");
    // not in CUDA addSame(cudaReplacementNames, "asinl");
    addSame(cudaReplacementNames, "atan");
    addSame(cudaReplacementNames, "atanf");
    // not in CUDA addSame(cudaReplacementNames, "atanl");
    addSame(cudaReplacementNames, "atan2");
    addSame(cudaReplacementNames, "atan2f");
    // not in CUDA addSame(cudaReplacementNames, "atan2l");
    addSame(cudaReplacementNames, "cos");
    addSame(cudaReplacementNames, "cosf");
    // not in CUDA addSame(cudaReplacementNames, "cosl");
    addSame(cudaReplacementNames, "sin");
    addSame(cudaReplacementNames, "sinf");
    // not in CUDA addSame(cudaReplacementNames, "sinl");
    addSame(cudaReplacementNames, "tan");
    addSame(cudaReplacementNames, "tanf");
    // not in CUDA addSame(cudaReplacementNames, "tanl");
    addSame(cudaReplacementNames, "acosh");
    addSame(cudaReplacementNames, "acoshf");
    // not in CUDA addSame(cudaReplacementNames, "acoshl");
    addSame(cudaReplacementNames, "asinh");
    addSame(cudaReplacementNames, "asinhf");
    // not in CUDA addSame(cudaReplacementNames, "asinhl");
    addSame(cudaReplacementNames, "atanh");
    addSame(cudaReplacementNames, "atanhf");
    // not in CUDA addSame(cudaReplacementNames, "atanhl");
    addSame(cudaReplacementNames, "cosh");
    addSame(cudaReplacementNames, "coshf");
    // not in CUDA addSame(cudaReplacementNames, "coshl");
    addSame(cudaReplacementNames, "sinh");
    addSame(cudaReplacementNames, "sinhf");
    // not in CUDA addSame(cudaReplacementNames, "sinhl");
    addSame(cudaReplacementNames, "tanh");
    addSame(cudaReplacementNames, "tanhf");
    // not in CUDA addSame(cudaReplacementNames, "tanhl");
    addSame(cudaReplacementNames, "exp");
    addSame(cudaReplacementNames, "expf");
    // not in CUDA addSame(cudaReplacementNames, "expl");
    addSame(cudaReplacementNames, "exp2");
    addSame(cudaReplacementNames, "exp2f");
    // not in CUDA addSame(cudaReplacementNames, "exp2l");
    addSame(cudaReplacementNames, "expm1");
    addSame(cudaReplacementNames, "expm1f");
    // not in CUDA addSame(cudaReplacementNames, "expm1l");
    addSame(cudaReplacementNames, "frexp");
    addSame(cudaReplacementNames, "frexpf");
    // not in CUDA addSame(cudaReplacementNames, "frexpl");
    addSame(cudaReplacementNames, "ilogb");
    addSame(cudaReplacementNames, "ilogbf");
    // not in CUDA addSame(cudaReplacementNames, "ilogbl");
    addSame(cudaReplacementNames, "ldexp");
    addSame(cudaReplacementNames, "ldexpf");
    // not in CUDA addSame(cudaReplacementNames, "ldexpl");
    addSame(cudaReplacementNames, "log");
    addSame(cudaReplacementNames, "logf");
    // not in CUDA addSame(cudaReplacementNames, "logl");
    addSame(cudaReplacementNames, "log10");
    addSame(cudaReplacementNames, "log10f");
    // not in CUDA addSame(cudaReplacementNames, "log10l");
    addSame(cudaReplacementNames, "log1p");
    addSame(cudaReplacementNames, "log1pf");
    // not in CUDA addSame(cudaReplacementNames, "log1pl");
    addSame(cudaReplacementNames, "log2");
    addSame(cudaReplacementNames, "log2f");
    // not in CUDA addSame(cudaReplacementNames, "log2l");
    addSame(cudaReplacementNames, "logb");
    addSame(cudaReplacementNames, "logbf");
    // not in CUDA addSame(cudaReplacementNames, "logbl");
    addSame(cudaReplacementNames, "modf");
    addSame(cudaReplacementNames, "modff");
    // not in CUDA addSame(cudaReplacementNames, "modfl");
    addSame(cudaReplacementNames, "scalbn");
    addSame(cudaReplacementNames, "scalbnf");
    // not in CUDA addSame(cudaReplacementNames, "scalbnl");
    addSame(cudaReplacementNames, "scalbln");
    addSame(cudaReplacementNames, "scalblnf");
    // not in CUDA addSame(cudaReplacementNames, "scalblnl");
    addSame(cudaReplacementNames, "cbrt");
    addSame(cudaReplacementNames, "cbrtf");
    // not in CUDA addSame(cudaReplacementNames, "cbrtl");
    addSame(cudaReplacementNames, "fabs");
    addSame(cudaReplacementNames, "fabsf");
    // not in CUDA addSame(cudaReplacementNames, "fabsl");
    addSame(cudaReplacementNames, "hypot");
    addSame(cudaReplacementNames, "hypotf");
    // not in CUDA addSame(cudaReplacementNames, "hypotl");
    addSame(cudaReplacementNames, "pow");
    addSame(cudaReplacementNames, "powf");
    // not in CUDA addSame(cudaReplacementNames, "powl");
    addSame(cudaReplacementNames, "sqrt");
    addSame(cudaReplacementNames, "sqrtf");
    // not in CUDA addSame(cudaReplacementNames, "sqrtl");
    addSame(cudaReplacementNames, "erf");
    addSame(cudaReplacementNames, "erff");
    // not in CUDA addSame(cudaReplacementNames, "erfl");
    addSame(cudaReplacementNames, "erfc");
    addSame(cudaReplacementNames, "erfcf");
    // not in CUDA addSame(cudaReplacementNames, "erfcl");
    addSame(cudaReplacementNames, "lgamma");
    addSame(cudaReplacementNames, "lgammaf");
    // not in CUDA addSame(cudaReplacementNames, "lgammal");
    addSame(cudaReplacementNames, "tgamma");
    addSame(cudaReplacementNames, "tgammaf");
    // not in CUDA addSame(cudaReplacementNames, "tgammal");
    addSame(cudaReplacementNames, "ceil");
    addSame(cudaReplacementNames, "ceilf");
    // not in CUDA addSame(cudaReplacementNames, "ceill");
    addSame(cudaReplacementNames, "floor");
    addSame(cudaReplacementNames, "floorf");
    // not in CUDA addSame(cudaReplacementNames, "floorl");
    addSame(cudaReplacementNames, "nearbyint");
    addSame(cudaReplacementNames, "nearbyintf");
    // not in CUDA addSame(cudaReplacementNames, "nearbyintl");
    addSame(cudaReplacementNames, "rint");
    addSame(cudaReplacementNames, "rintf");
    // not in CUDA addSame(cudaReplacementNames, "rintl");
    addSame(cudaReplacementNames, "lrint");
    addSame(cudaReplacementNames, "lrintf");
    // not in CUDA addSame(cudaReplacementNames, "lrintl");
    addSame(cudaReplacementNames, "llrint");
    addSame(cudaReplacementNames, "llrintf");
    // not in CUDA addSame(cudaReplacementNames, "llrintl");
    addSame(cudaReplacementNames, "round");
    addSame(cudaReplacementNames, "roundf");
    // not in CUDA addSame(cudaReplacementNames, "roundl");
    addSame(cudaReplacementNames, "lround");
    addSame(cudaReplacementNames, "lroundf");
    // not in CUDA addSame(cudaReplacementNames, "lroundl");
    addSame(cudaReplacementNames, "llround");
    addSame(cudaReplacementNames, "llroundf");
    // not in CUDA addSame(cudaReplacementNames, "llroundl");
    addSame(cudaReplacementNames, "trunc");
    addSame(cudaReplacementNames, "truncf");
    // not in CUDA addSame(cudaReplacementNames, "truncl");
    addSame(cudaReplacementNames, "fmod");
    addSame(cudaReplacementNames, "fmodf");
    // not in CUDA addSame(cudaReplacementNames, "fmodl");
    addSame(cudaReplacementNames, "remainder");
    addSame(cudaReplacementNames, "remainderf");
    // not in CUDA addSame(cudaReplacementNames, "remainderl");
    addSame(cudaReplacementNames, "remquo");
    addSame(cudaReplacementNames, "remquof");
    // not in CUDA addSame(cudaReplacementNames, "remquol");
    addSame(cudaReplacementNames, "copysign");
    addSame(cudaReplacementNames, "copysignf");
    // not in CUDA addSame(cudaReplacementNames, "copysignl");
    addSame(cudaReplacementNames, "nan");
    addSame(cudaReplacementNames, "nanf");
    // not in CUDA addSame(cudaReplacementNames, "nanl");
    addSame(cudaReplacementNames, "nextafter");
    addSame(cudaReplacementNames, "nextafterf");
    // not in CUDA addSame(cudaReplacementNames, "nextafterl");
    // not in CUDA addSame(cudaReplacementNames, "nexttoward");
    // not in CUDA addSame(cudaReplacementNames, "nexttowardf");
    // not in CUDA addSame(cudaReplacementNames, "nexttowardl");
    addSame(cudaReplacementNames, "fdim");
    addSame(cudaReplacementNames, "fdimf");
    // not in CUDA addSame(cudaReplacementNames, "fdiml");
    addSame(cudaReplacementNames, "fmax");
    addSame(cudaReplacementNames, "fmaxf");
    // not in CUDA addSame(cudaReplacementNames, "fmaxl");
    addSame(cudaReplacementNames, "fmin");
    addSame(cudaReplacementNames, "fminf");
    // not in CUDA addSame(cudaReplacementNames, "fminl");
    addSame(cudaReplacementNames, "fma");
    addSame(cudaReplacementNames, "fmaf");
    // not in CUDA addSame(cudaReplacementNames, "fmal");
    // not in CUDA addSame(cudaReplacementNames, "isgreater");
    // not in CUDA addSame(cudaReplacementNames, "isgreaterequal");
    // not in CUDA addSame(cudaReplacementNames, "isless");
    // not in CUDA addSame(cudaReplacementNames, "islessequal");
    // not in CUDA addSame(cudaReplacementNames, "islessgreater");
    // not in CUDA addSame(cudaReplacementNames, "isunordered");
    // TODO: Add more CUDA-available functions and types

    std::string stdioPrefix(options.useDvmhStdio ? "dvmh_" : "dvm_");
    // stdio.h functions
    //  Operations on files
    addPrefix(dvmhReplacementNames, stdioPrefix, "remove");
    addPrefix(dvmhReplacementNames, stdioPrefix, "rename");
    addPrefix(dvmhReplacementNames, stdioPrefix, "tmpfile");
    addPrefix(dvmhReplacementNames, stdioPrefix, "tmpnam");
    //  File access functions
    addPrefix(dvmhReplacementNames, stdioPrefix, "fclose");
    addPrefix(dvmhReplacementNames, stdioPrefix, "fflush");
    addPrefix(dvmhReplacementNames, stdioPrefix, "fopen");
    addPrefix(dvmhReplacementNames, stdioPrefix, "freopen");
    addPrefix(dvmhReplacementNames, stdioPrefix, "setbuf");
    addPrefix(dvmhReplacementNames, stdioPrefix, "setvbuf");
    //  Formatted input/output functions
    addPrefix(dvmhReplacementNames, stdioPrefix, "fprintf", options.useVoidStdio);
    addPrefix(dvmhReplacementNames, stdioPrefix, "fscanf", options.useVoidStdio && options.useDvmhStdio);
    addPrefix(dvmhReplacementNames, stdioPrefix, "printf", options.useVoidStdio);
    addPrefix(dvmhReplacementNames, stdioPrefix, "scanf", options.useVoidStdio && options.useDvmhStdio);
    addPrefix(dvmhReplacementNames, stdioPrefix, "vfprintf", options.useVoidStdio);
    addPrefix(dvmhReplacementNames, stdioPrefix, "vfscanf", options.useVoidStdio && options.useDvmhStdio);
    addPrefix(dvmhReplacementNames, stdioPrefix, "vprintf", options.useVoidStdio);
    addPrefix(dvmhReplacementNames, stdioPrefix, "vscanf", options.useVoidStdio && options.useDvmhStdio);
    //  Character input/output functions
    addPrefix(dvmhReplacementNames, stdioPrefix, "fgetc");
    addPrefix(dvmhReplacementNames, stdioPrefix, "fgets", options.useVoidStdio && options.useDvmhStdio);
    addPrefix(dvmhReplacementNames, stdioPrefix, "fputc", options.useVoidStdio && options.useDvmhStdio);
    addPrefix(dvmhReplacementNames, stdioPrefix, "fputs", options.useVoidStdio && options.useDvmhStdio);
    addPrefix(dvmhReplacementNames, stdioPrefix, "getc");
    addPrefix(dvmhReplacementNames, stdioPrefix, "getchar");
    addPrefix(dvmhReplacementNames, stdioPrefix, "gets", options.useVoidStdio && options.useDvmhStdio);
    addPrefix(dvmhReplacementNames, stdioPrefix, "putc", options.useVoidStdio && options.useDvmhStdio);
    addPrefix(dvmhReplacementNames, stdioPrefix, "putchar", options.useVoidStdio && options.useDvmhStdio);
    addPrefix(dvmhReplacementNames, stdioPrefix, "puts", options.useVoidStdio && options.useDvmhStdio);
    addPrefix(dvmhReplacementNames, stdioPrefix, "ungetc", options.useVoidStdio && options.useDvmhStdio);
    //  Direct input/output functions
    addPrefix(dvmhReplacementNames, stdioPrefix, "fread", options.useVoidStdio && options.useDvmhStdio, options.useDvmhStdio);
    addPrefix(dvmhReplacementNames, stdioPrefix, "fwrite", options.useVoidStdio && options.useDvmhStdio, options.useDvmhStdio);
    //  File positioning functions
    addPrefix(dvmhReplacementNames, stdioPrefix, "fgetpos");
    addPrefix(dvmhReplacementNames, stdioPrefix, "fseek", options.useVoidStdio && options.useDvmhStdio); // beware of errno checks, maybe
    addPrefix(dvmhReplacementNames, stdioPrefix, "fsetpos");
    addPrefix(dvmhReplacementNames, stdioPrefix, "ftell");
    addPrefix(dvmhReplacementNames, stdioPrefix, "rewind");
    //  Error-handling functions
    addPrefix(dvmhReplacementNames, stdioPrefix, "clearerr");
    addPrefix(dvmhReplacementNames, stdioPrefix, "feof");
    addPrefix(dvmhReplacementNames, stdioPrefix, "ferror");
    addPrefix(dvmhReplacementNames, stdioPrefix, "perror");
    // stdio.h global variables
    // TODO: Actually, they ain't variables, but macros. Make their replacement on another level - in PPCallbacks.
    if (options.useDvmhStdio) {
        addPrefix(dvmhReplacementNames, stdioPrefix, "stderr");
        addPrefix(dvmhReplacementNames, stdioPrefix, "stdin");
        addPrefix(dvmhReplacementNames, stdioPrefix, "stdout");
    } else {
        dvmhReplacementNames["stderr"] = "DVMSTDERR";
        dvmhReplacementNames["stdin"] = "DVMSTDIN";
        dvmhReplacementNames["stdout"] = "DVMSTDOUT";
    }
    if (!options.useDvmhStdio) {
        // stdio.h types
        dvmhReplacementNames["FILE"] = "DVMFILE";
    }

    // stdlib.h functions
    dvmhReplacementNames["calloc"] = "dvmh_calloc_C";
    dvmhReplacementNames["exit"] = "dvmh_exit_C";
    dvmhReplacementNames["free"] = "dvmh_free_C";
    dvmhReplacementNames["malloc"] = "dvmh_malloc_C";
    dvmhReplacementNames["realloc"] = "dvmh_realloc_C";
    dvmhReplacementNames["strdup"] = "dvmh_strdup_C";

    // XXX: Functions not from C Standard Libraries are not included: open, close, fstat, lseek, read, write, access, unlink, stat
}

std::string ProjectContext::getDvmhReplacement(const std::string &name, bool canBeVoid, bool forDistribArrays) const {
    std::string toSeek = forDistribArrays ? "distrib " + name : name;
    std::map<std::string, std::string>::const_iterator it1 = dvmhReplacementNames.find(toSeek);
    std::map<std::string, std::string>::const_iterator it2 = dvmhReplacementNames.find("void " + toSeek);
    if (canBeVoid && it2 != dvmhReplacementNames.end())
        return it2->second;
    else if (it1 != dvmhReplacementNames.end())
        return it1->second;
    assert(false);
    return "UNKNOWN";
}

}
