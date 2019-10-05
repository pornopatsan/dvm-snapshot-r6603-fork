#pragma once

#include <map>
#include <vector>
#include <string>

#include <clang/AST/ASTConsumer.h>
#include <clang/AST/ASTContext.h>
#include <clang/AST/RecursiveASTVisitor.h>
#include <clang/Basic/Version.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Lex/Preprocessor.h>
#include <clang/Rewrite/Core/Rewriter.h>

#include "pragmas.h"
#include "file_ctx.h"

using namespace clang; // Sad, but there are too many used from this namespace

namespace cdvmh {

class CollectNamesVisitor: public RecursiveASTVisitor<CollectNamesVisitor> {
    typedef RecursiveASTVisitor<CollectNamesVisitor> base;
public:
    explicit CollectNamesVisitor(CompilerInstance &aComp): comp(aComp), ignoreDepth(0) {}
public:
    bool VisitVarDecl(VarDecl *vd);
    bool VisitTypedefDecl(TypedefDecl *td);
    bool VisitFunctionDecl(FunctionDecl *f);
    bool TraverseFunctionDecl(FunctionDecl *f);
    bool VisitRecordDecl(RecordDecl *d);
    bool TraverseRecordDecl(RecordDecl *d);
    bool VisitDeclRefExpr(DeclRefExpr *e);
public:
    std::set<std::string> getNames() { return names; }
protected:
    CompilerInstance &comp;

    int ignoreDepth;
    std::set<std::string> names;
};

class MacroCollector: public PPCallbacks {
public:
    explicit MacroCollector(SourceFileContext &aFileCtx): fileCtx(aFileCtx) {}
public:
    virtual void MacroDefined(const Token &MacroNameTok, const MacroDirective *MD) {
        fileCtx.seenMacroNames.insert(MacroNameTok.getIdentifierInfo()->getName());
    }
protected:
    SourceFileContext &fileCtx;
};

struct IncludedFile {
    int inclusionCount;
    bool isSystem;
};

struct Inclusion {
    bool valid;
    SourceLocation hashLoc;
    std::pair<int, int> where;
    std::string what;
    FileID whatFID;
    bool isAngled;
    bool isImport;
    bool isIncludeNext;
    bool isSkipped;
    std::string spellingFN;
    std::vector<Inclusion> nested;
    Inclusion(): valid(false) {}
};

class IncludeCollector: public PPCallbacks {
public:
    std::map<std::string, IncludedFile> includedFiles;
    std::vector<Inclusion> inclusions;
public:
    explicit IncludeCollector(SourceFileContext &fileCtx, Preprocessor &PP): fileCtx(fileCtx), PP(PP), ignoreLevel(-1) {}
public:
    virtual void FileChanged(SourceLocation Loc, FileChangeReason Reason, SrcMgr::CharacteristicKind FileType, FileID PrevFID);
    virtual void FileSkipped(const FileEntry &ParentFile, const Token &FilenameTok, SrcMgr::CharacteristicKind FileType);
#if CLANG_VERSION_MAJOR < 7
    virtual void InclusionDirective(SourceLocation HashLoc, const Token &IncludeTok, StringRef FileName, bool IsAngled, CharSourceRange FilenameRange,
            const FileEntry *File, StringRef SearchPath, StringRef RelativePath, const Module *Imported);
#else
    virtual void InclusionDirective(SourceLocation HashLoc, const Token &IncludeTok, StringRef FileName, bool IsAngled, CharSourceRange FilenameRange,
            const FileEntry *File, StringRef SearchPath, StringRef RelativePath, const Module *Imported, SrcMgr::CharacteristicKind FileType) override;
#endif
protected:
    void commitInclusion(SrcMgr::CharacteristicKind FileType);
protected:
    SourceFileContext &fileCtx;
    Preprocessor &PP;

    std::vector<int> activeInclusion;
    Inclusion pendingInclusion;
    int ignoreLevel;
    std::set<FileID> seenFIDs;
};

struct PragmaExpander: public PragmaHandler {
public:
    explicit PragmaExpander(llvm::raw_string_ostream &OS, const char *name = "dvm"): PragmaHandler(name), OS(OS), name(name) {}
    virtual void HandlePragma(Preprocessor &PP, PragmaIntroducerKind Introducer, Token &PragmaTok);
protected:
    llvm::raw_string_ostream &OS;
    std::string name;
};

class IncludeExpanderAndRewriter {
public:
    explicit IncludeExpanderAndRewriter(SourceFileContext &fileCtx, const std::vector<Inclusion> &inclusions, Rewriter &rewr): fileCtx(fileCtx),
            projectCtx(fileCtx.getProjectCtx()), inclusions(inclusions), rewr(rewr), srcMgr(rewr.getSourceMgr()) {
        mainOutFN = fileCtx.getInputFile().canonicalConverted;
        mainFID = srcMgr.getMainFileID();
    }
public:
    void work() {
        bool isChanged, isHard;
        processInclusions(mainFID, inclusions, isChanged, isHard);
    }
protected:
    void processInclusions(FileID parentFID, const std::vector<Inclusion> &incs, bool &isChanged, bool &isHard);
    void processInclusion(const Inclusion &inc, bool &isChanged, bool &isHard);
protected:
    SourceFileContext &fileCtx;
    ProjectContext &projectCtx;
    const std::vector<Inclusion> &inclusions;
    Rewriter &rewr;
    SourceManager &srcMgr;

    FileID mainFID;
    std::string mainOutFN;
};

}
