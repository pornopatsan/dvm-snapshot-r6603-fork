#include <cstdio>
#include <ctime>
#include <cstdlib>
#include <fstream>

#include <clang/Frontend/Utils.h>
#include <clang/Basic/Version.h>

#include "converter.h"
#include "pass_ctx.h"
#include "pragma_parser.h"
#include "cdvmh_log.h"
#include "handler_gen.h"
#include "messages.h"

#if CLANG_VERSION_MINOR >= 5
#include <system_error>
#include <memory>
#endif

#define CDVMH_VERSION "1.0"

using namespace cdvmh;

static void usage(FILE *f) {
    fprintf(f, MSG(11));
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        usage(stderr);
        return 1;
    }
    logLevel = ERROR;
    const char *sett = getenv("DVMH_LOGLEVEL");
    if (sett) {
        logLevel = (LogLevel)atoi(sett);
        if (logLevel < ERROR)
            logLevel = ERROR;
        if (logLevel > TRACE)
            logLevel = TRACE;
    }
    srand(time(0));

    // Parse parameters
    ConverterOptions opts;
    opts.setFromArgs(argc - 1, argv + 1);
    if (opts.displayWarnings)
        if (logLevel < WARNING)
            logLevel = WARNING;
    ProjectContext projectCtx(opts);

    for (int i = 0; i < (int)opts.includeDirs.size(); i++)
        cdvmhLog(DEBUG, "", 1, "Using include directory: %s", opts.includeDirs[i].c_str());

    for (int i = 0; i < projectCtx.getFileCount(); i++) {
        // Convert one file
        int inputFileIdx = i;
        SourceFileContext fileCtx(projectCtx, inputFileIdx);
        const InputFile &file = fileCtx.getInputFile();
        if (file.isCompilable) {
            cdvmhLog(INFO, "", 1, 110, MSG(110), file.fileName.c_str(), file.convertedFileName.c_str(), file.outCXXName.c_str(), file.outHostName.c_str(),
                    file.outCudaName.c_str(), file.outCudaInfoName.c_str());
            if (opts.emitBlankHandlers)
                cdvmhLog(INFO, "", 1, 111, MSG(111), file.outBlankName.c_str());
        } else {
            cdvmhLog(INFO, "", 1, 112, MSG(112), file.fileName.c_str(), file.convertedFileName.c_str());
        }

        if (0) {
            // Preprocess with expansion of DVM pragmas. Now turned off.
            PassContext passCtx(fileCtx);
            std::string res;
            llvm::raw_string_ostream OS(res);
            passCtx.getPP()->AddPragmaHandler(new PragmaExpander(OS));
            clang::DoPrintPreprocessedInput(*passCtx.getPP(), &OS, passCtx.getCompiler()->getPreprocessorOutputOpts());
            std::string msg;
#if CLANG_VERSION_MAJOR < 4 && CLANG_VERSION_MINOR < 5
            llvm::raw_fd_ostream f("/dev/null", msg);
#elif CLANG_VERSION_MAJOR < 4 && CLANG_VERSION_MINOR < 6
            llvm::raw_fd_ostream f("/dev/null", msg, llvm::sys::fs::F_Text);
#else
            std::error_code EC;
            llvm::raw_fd_ostream f("/dev/null", EC, llvm::sys::fs::F_Text);
            msg = EC.message();
#endif
            assert(msg.empty());
            f << OS.str();
        }
        // Perform the pass - convert the file
        {

            // Debug prepass
            if (opts.dvmDebugLvl > 0) {

                fileCtx.isDebugPass = true;

                PassContext debugPassCtx(fileCtx);
                Rewriter *rewr = debugPassCtx.getRewr();
                debugPassCtx.getPP()->AddPragmaHandler(new DvmPragmaHandler(fileCtx, *debugPassCtx.getCompiler(), *rewr));
                DebugConsumer dbgConsumer(fileCtx, *debugPassCtx.getCompiler(), *rewr);
                debugPassCtx.parse(&dbgConsumer);
                const RewriteBuffer *debugBuf = rewr->getRewriteBufferFor(
                                                debugPassCtx.getCompiler()->getSourceManager().getMainFileID());  
                std::stringstream originalFile;
                originalFile << readFile(file.fileName);

                remove(file.debugPreConvFileName.c_str());
                std::ofstream dbgFile(file.debugPreConvFileName.c_str());
                if (!debugBuf)
                    dbgFile << readFile(file.fileName);
                else {                    
                    dbgFile << "#include <cdvmh_debug_helpers.h>\n";
                    dbgFile << std::string(debugBuf->begin(), debugBuf->end()); 
                }
                dbgFile.close();

                fileCtx.isDebugPass = false; 
            }

            PassContext passCtx(fileCtx);

            Rewriter *mainRewr = passCtx.getRewr("main");
            //Rewriter *blankHandlersRewr = passCtx.getRewr("blankHandlers");
            IncludeCollector *includeCollector = new IncludeCollector(fileCtx, *passCtx.getPP());
            MacroCollector *macroCollector = new MacroCollector(fileCtx);
            //PPDirectiveCollector *ppDirectiveCollector = new PPDirectiveCollector(*passCtx.getCompiler());
#if CLANG_VERSION_MAJOR < 4 && CLANG_VERSION_MINOR < 6
            passCtx.getPP()->addPPCallbacks(includeCollector);
            passCtx.getPP()->addPPCallbacks(macroCollector);
            //passCtx.getPP()->addPPCallbacks(ppDirectiveCollector);
#else
            passCtx.getPP()->addPPCallbacks(std::unique_ptr<IncludeCollector>(includeCollector));
            passCtx.getPP()->addPPCallbacks(std::unique_ptr<MacroCollector>(macroCollector));
            //passCtx.getPP()->addPPCallbacks(std::unique_ptr<PPDirectiveCollector>(ppDirectiveCollector));
#endif
            passCtx.getPP()->AddPragmaHandler(new DvmPragmaHandler(fileCtx, *passCtx.getCompiler(), *mainRewr));
            //passCtx.getPP()->addPPCallbacks(new IncludeRewriter(fileCtx, *mainRewr));

            ConverterConsumer astConsumer(fileCtx, *passCtx.getCompiler(), *mainRewr);
            passCtx.parse(&astConsumer);

            // Figure out which files to expand, which to leave included, and which inclusion directives to alter, merge all to the main rewrite buffer
            const RewriteBuffer *rewriteBuf = IncludeExpanderAndRewriter(fileCtx, includeCollector->inclusions, *mainRewr).work();
            if (!rewriteBuf)
                fileCtx.setConvertedText(readFile(file.fileName), false);
            else
                fileCtx.setConvertedText(std::string(rewriteBuf->begin(), rewriteBuf->end()), true);
        }
        // Preliminary remove all the possible output files
        remove(file.convertedFileName.c_str());
        if (file.isCompilable) {
            remove(file.outCXXName.c_str());
            remove(file.outBlankName.c_str());
            remove(file.outHostName.c_str());
            remove(file.outCudaName.c_str());
            remove(file.outCudaInfoName.c_str());
        }
        // Write output files
        {
            std::ofstream outFile(file.convertedFileName.c_str());

            if (fileCtx.isTextChanged() || !fileCtx.getInitGlobText().empty()) {
                // Output heading
                if (opts.useDvmhStdio) {
                    if (opts.extraComments)
                        outFile << "/* CDVMH include */\n";
                    if (fileCtx.needsAllocator())
                        outFile << "#define DVMH_NEEDS_ALLOCATOR 1\n";
                    outFile << "#include <cdvmh_helpers.h>\n";
                } else {
                    if (opts.extraComments)
                        outFile << "/* CDVMH includes */\n";
                    if (fileCtx.needsAllocator())
                        outFile << "#define DVMH_NEEDS_ALLOCATOR 1\n";
                    outFile << "#include <dvmhlib_types.h>\n";
                    outFile << "#include <dvmlib.h>\n";
                    outFile << "#include <cdvmh_helpers.h>\n";
                }
                outFile << "\n";
                if (file.isCompilable) {
                    std::string tmp = fileCtx.genDvm0cText() + fileCtx.genOmpHandlerTypeText();
                    if (!tmp.empty()) {
                        if (opts.extraComments)
                            outFile << "/* Supplementary macros */\n";
                        outFile << tmp;
                        outFile << "\n";
                    }
                    if (!fileCtx.getHandlersForwardDecls().empty()) {
                        if (opts.extraComments)
                            outFile << "/* Forward declaraions for the handlers for this file */\n";
                        outFile << fileCtx.getHandlersForwardDecls();
                        outFile << "\n";
                    }
                }

                // Now output rewritten source code
                if (opts.extraComments)
                    outFile << "/* Rest of file, converted */\n";
            }

            outFile << fileCtx.getConvertedText();

            if (!fileCtx.getInitGlobText().empty()) {
                // Output tail
                outFile << "\n" << (file.CPlusPlus ? "static " : "") << "void " << fileCtx.getInitGlobName() << "() {\n";
                outFile << fileCtx.getInitGlobText() << "}\n";
                outFile << "\n" << (file.CPlusPlus ? "static " : "") << "void " << fileCtx.getFinishGlobName() << "() {\n";
                outFile << fileCtx.getFinishGlobText() << "}\n";
                outFile << "\n";
                if (file.CPlusPlus) {
                    outFile << "static DvmhModuleInitializer " << fileCtx.getInitGlobName() + "_initializer(" + fileCtx.getInitGlobName() + ", " +
                            fileCtx.getFinishGlobName() + ");\n";
                    outFile << "\n";
                } else {
                    std::string txt;
                    txt += "extern \"C\" void " + fileCtx.getInitGlobName() + "();\n";
                    txt += "extern \"C\" void " + fileCtx.getFinishGlobName() + "();\n";
                    txt += "static DvmhModuleInitializer " + fileCtx.getInitGlobName() + "_initializer(" + fileCtx.getInitGlobName() + ", " +
                            fileCtx.getFinishGlobName() + ");\n";
                    if (fileCtx.hasCudaHandlers() || !fileCtx.getCudaReqMap().empty()) {
                        fileCtx.addToCudaTail(txt);
                    } else {
                        std::ofstream cxxOut(file.outCXXName.c_str());
                        if (opts.extraComments)
                            cxxOut << "/* CDVMH include */\n";
                        cxxOut << "#include <cdvmh_helpers.h>\n";
                        cxxOut << "\n";
                        cxxOut << txt;
                        cxxOut << "\n";
                    }
                }
            }
            outFile.close();

            {
                // Handlers
                std::string blankBeforeRma = fileCtx.genBlankHandlersText();
                if (opts.emitBlankHandlers) {
                    std::ofstream outFile((file.outBlankName + "_before_rma").c_str());
                    outFile << blankBeforeRma;
                }

                BlankHandlerConverter conv(fileCtx);
                std::string blankHandlers;
                if (opts.emitBlankHandlers || opts.useBlank) {
                    blankHandlers = conv.genRmas(blankBeforeRma);
                }

                // Output blank handlers
                if (opts.emitBlankHandlers) {
                    std::ofstream outFile(file.outBlankName.c_str());
                    outFile << blankHandlers;
                }

                // Output HOST handlers
                {
                    std::string hostHandlers;
                    if (!opts.useBlank && fileCtx.hasHostHandlers())
                        hostHandlers = fileCtx.genHostHandlersText(false);
                    else if (opts.useBlank && !fileCtx.getHostReqMap().empty())
                        hostHandlers = conv.genHostHandlers(blankHandlers, false);
                    if (!hostHandlers.empty()) {
                        std::ofstream outFile(file.convertedFileName.c_str(), std::ios_base::app | std::ios_base::out);
                        outFile << "\n";
                        if (opts.extraComments)
                            outFile << "/* Host handlers placed in the same file, maybe they will be moved to separate file in future */\n";
                        outFile << hostHandlers;
                    }
                }

                // Output CUDA handlers
                {
                    std::string cudaHandlers, cudaInfo;
                    if (!opts.useBlank && fileCtx.hasCudaHandlers()) {
                        cudaHandlers = fileCtx.genCudaHandlersText();
                        cudaInfo = fileCtx.genCudaInfoText();
                    } else if (opts.useBlank && !fileCtx.getCudaReqMap().empty()) {
                        cudaHandlers = conv.genCudaHandlers(blankHandlers);
                    }
                    if (!cudaHandlers.empty()) {
                        std::ofstream outFile(file.outCudaName.c_str());
                        // TODO: Exclude <cassert> include, maybe
                        outFile << "#include <cassert>\n";
                        outFile << "\n";
                        if (opts.extraComments)
                            outFile << "/* CDVMH include */\n";
                        outFile << "#include <cdvmh_helpers.h>\n";
                        outFile << "\n";
                        outFile << cudaHandlers;
                    }
                    if (!cudaInfo.empty()) {
                        std::ofstream outFile(file.outCudaInfoName.c_str());
                        outFile << "\n";
                        outFile << "#include <dvmhlib2.h>\n";
                        outFile << "\n";
                        outFile << cudaInfo;
                    }
                }
            }
        }
    }

    return 0;
}
