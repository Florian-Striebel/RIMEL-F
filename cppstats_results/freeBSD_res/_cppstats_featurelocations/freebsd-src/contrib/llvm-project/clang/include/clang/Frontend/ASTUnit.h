











#if !defined(LLVM_CLANG_FRONTEND_ASTUNIT_H)
#define LLVM_CLANG_FRONTEND_ASTUNIT_H

#include "clang-c/Index.h"
#include "clang/AST/ASTContext.h"
#include "clang/Basic/Diagnostic.h"
#include "clang/Basic/FileSystemOptions.h"
#include "clang/Basic/LLVM.h"
#include "clang/Basic/LangOptions.h"
#include "clang/Basic/SourceLocation.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Basic/TargetOptions.h"
#include "clang/Lex/HeaderSearchOptions.h"
#include "clang/Lex/ModuleLoader.h"
#include "clang/Lex/PreprocessingRecord.h"
#include "clang/Sema/CodeCompleteConsumer.h"
#include "clang/Serialization/ASTBitCodes.h"
#include "clang/Frontend/PrecompiledPreamble.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/IntrusiveRefCntPtr.h"
#include "llvm/ADT/None.h"
#include "llvm/ADT/Optional.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/iterator_range.h"
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace llvm {

class MemoryBuffer;

namespace vfs {

class FileSystem;

}
}

namespace clang {

class ASTContext;
class ASTDeserializationListener;
class ASTMutationListener;
class ASTReader;
class CompilerInstance;
class CompilerInvocation;
class Decl;
class FileEntry;
class FileManager;
class FrontendAction;
class HeaderSearch;
class InputKind;
class InMemoryModuleCache;
class PCHContainerOperations;
class PCHContainerReader;
class Preprocessor;
class PreprocessorOptions;
class Sema;
class TargetInfo;


enum class SkipFunctionBodiesScope { None, Preamble, PreambleAndMainFile };


enum class CaptureDiagsKind { None, All, AllWithoutNonErrorsFromIncludes };


class ASTUnit {
public:
struct StandaloneFixIt {
std::pair<unsigned, unsigned> RemoveRange;
std::pair<unsigned, unsigned> InsertFromRange;
std::string CodeToInsert;
bool BeforePreviousInsertions;
};

struct StandaloneDiagnostic {
unsigned ID;
DiagnosticsEngine::Level Level;
std::string Message;
std::string Filename;
unsigned LocOffset;
std::vector<std::pair<unsigned, unsigned>> Ranges;
std::vector<StandaloneFixIt> FixIts;
};

private:
std::shared_ptr<LangOptions> LangOpts;
IntrusiveRefCntPtr<DiagnosticsEngine> Diagnostics;
IntrusiveRefCntPtr<FileManager> FileMgr;
IntrusiveRefCntPtr<SourceManager> SourceMgr;
IntrusiveRefCntPtr<InMemoryModuleCache> ModuleCache;
std::unique_ptr<HeaderSearch> HeaderInfo;
IntrusiveRefCntPtr<TargetInfo> Target;
std::shared_ptr<Preprocessor> PP;
IntrusiveRefCntPtr<ASTContext> Ctx;
std::shared_ptr<TargetOptions> TargetOpts;
std::shared_ptr<HeaderSearchOptions> HSOpts;
std::shared_ptr<PreprocessorOptions> PPOpts;
IntrusiveRefCntPtr<ASTReader> Reader;
bool HadModuleLoaderFatalFailure = false;

struct ASTWriterData;
std::unique_ptr<ASTWriterData> WriterData;

FileSystemOptions FileSystemOpts;



std::unique_ptr<ASTConsumer> Consumer;



std::unique_ptr<Sema> TheSema;



std::shared_ptr<CompilerInvocation> Invocation;


TrivialModuleLoader ModuleLoader;




bool OnlyLocalDecls = false;


CaptureDiagsKind CaptureDiagnostics = CaptureDiagsKind::None;


bool MainFileIsAST;


TranslationUnitKind TUKind = TU_Complete;


bool WantTiming;


bool OwnsRemappedFileBuffers = true;








std::vector<Decl*> TopLevelDecls;


using LocDeclsTy = SmallVector<std::pair<unsigned, Decl *>, 64>;
using FileDeclsTy = llvm::DenseMap<FileID, std::unique_ptr<LocDeclsTy>>;



FileDeclsTy FileDecls;


std::string OriginalSourceFile;


SmallVector<StandaloneDiagnostic, 4> PreambleDiagnostics;



SmallVector<StoredDiagnostic, 4> StoredDiagnostics;



SmallVector<StoredDiagnostic, 4> FailedParseDiagnostics;






unsigned NumStoredDiagnosticsFromDriver = 0;










unsigned PreambleRebuildCountdown = 0;


unsigned PreambleCounter = 0;







llvm::StringMap<SourceLocation> PreambleSrcLocCache;


llvm::Optional<PrecompiledPreamble> Preamble;




std::unique_ptr<llvm::MemoryBuffer> SavedMainFileBuffer;







unsigned NumWarningsInPreamble = 0;



std::vector<serialization::DeclID> TopLevelDeclsInPreamble;


bool ShouldCacheCodeCompletionResults : 1;



bool IncludeBriefCommentsInCodeCompletion : 1;



bool UserFilesAreVolatile : 1;

static void ConfigureDiags(IntrusiveRefCntPtr<DiagnosticsEngine> Diags,
ASTUnit &AST, CaptureDiagsKind CaptureDiagnostics);

void TranslateStoredDiagnostics(FileManager &FileMgr,
SourceManager &SrcMan,
const SmallVectorImpl<StandaloneDiagnostic> &Diags,
SmallVectorImpl<StoredDiagnostic> &Out);

void clearFileLevelDecls();

public:


struct CachedCodeCompletionResult {


CodeCompletionString *Completion;








uint64_t ShowInContexts;


unsigned Priority;



CXCursorKind Kind;


CXAvailabilityKind Availability;


SimplifiedTypeClass TypeClass;







unsigned Type;
};



llvm::StringMap<unsigned> &getCachedCompletionTypes() {
return CachedCompletionTypes;
}


std::shared_ptr<GlobalCodeCompletionAllocator>
getCachedCompletionAllocator() {
return CachedCompletionAllocator;
}

CodeCompletionTUInfo &getCodeCompletionTUInfo() {
if (!CCTUInfo)
CCTUInfo = std::make_unique<CodeCompletionTUInfo>(
std::make_shared<GlobalCodeCompletionAllocator>());
return *CCTUInfo;
}

private:

std::shared_ptr<GlobalCodeCompletionAllocator> CachedCompletionAllocator;

std::unique_ptr<CodeCompletionTUInfo> CCTUInfo;


std::vector<CachedCodeCompletionResult> CachedCompletionResults;



llvm::StringMap<unsigned> CachedCompletionTypes;






unsigned CompletionCacheTopLevelHashValue = 0;






unsigned PreambleTopLevelHashValue = 0;



unsigned CurrentTopLevelHashValue = 0;



unsigned UnsafeToFree : 1;


SkipFunctionBodiesScope SkipFunctionBodies = SkipFunctionBodiesScope::None;



void CacheCodeCompletionResults();


void ClearCachedCompletionResults();

explicit ASTUnit(bool MainFileIsAST);

bool Parse(std::shared_ptr<PCHContainerOperations> PCHContainerOps,
std::unique_ptr<llvm::MemoryBuffer> OverrideMainBuffer,
IntrusiveRefCntPtr<llvm::vfs::FileSystem> VFS);

std::unique_ptr<llvm::MemoryBuffer> getMainBufferWithPrecompiledPreamble(
std::shared_ptr<PCHContainerOperations> PCHContainerOps,
CompilerInvocation &PreambleInvocationIn,
IntrusiveRefCntPtr<llvm::vfs::FileSystem> VFS, bool AllowRebuild = true,
unsigned MaxLines = 0);
void RealizeTopLevelDeclsFromPreamble();



void transferASTDataFromCompilerInstance(CompilerInstance &CI);








class ConcurrencyState {
void *Mutex;

public:
ConcurrencyState();
~ConcurrencyState();

void start();
void finish();
};
ConcurrencyState ConcurrencyCheckValue;

public:
friend class ConcurrencyCheck;

class ConcurrencyCheck {
ASTUnit &Self;

public:
explicit ConcurrencyCheck(ASTUnit &Self) : Self(Self) {
Self.ConcurrencyCheckValue.start();
}

~ConcurrencyCheck() {
Self.ConcurrencyCheckValue.finish();
}
};

ASTUnit(const ASTUnit &) = delete;
ASTUnit &operator=(const ASTUnit &) = delete;
~ASTUnit();

bool isMainFileAST() const { return MainFileIsAST; }

bool isUnsafeToFree() const { return UnsafeToFree; }
void setUnsafeToFree(bool Value) { UnsafeToFree = Value; }

const DiagnosticsEngine &getDiagnostics() const { return *Diagnostics; }
DiagnosticsEngine &getDiagnostics() { return *Diagnostics; }

const SourceManager &getSourceManager() const { return *SourceMgr; }
SourceManager &getSourceManager() { return *SourceMgr; }

const Preprocessor &getPreprocessor() const { return *PP; }
Preprocessor &getPreprocessor() { return *PP; }
std::shared_ptr<Preprocessor> getPreprocessorPtr() const { return PP; }

const ASTContext &getASTContext() const { return *Ctx; }
ASTContext &getASTContext() { return *Ctx; }

void setASTContext(ASTContext *ctx) { Ctx = ctx; }
void setPreprocessor(std::shared_ptr<Preprocessor> pp);








void enableSourceFileDiagnostics();

bool hasSema() const { return (bool)TheSema; }

Sema &getSema() const {
assert(TheSema && "ASTUnit does not have a Sema object!");
return *TheSema;
}

const LangOptions &getLangOpts() const {
assert(LangOpts && "ASTUnit does not have language options");
return *LangOpts;
}

const HeaderSearchOptions &getHeaderSearchOpts() const {
assert(HSOpts && "ASTUnit does not have header search options");
return *HSOpts;
}

const PreprocessorOptions &getPreprocessorOpts() const {
assert(PPOpts && "ASTUnit does not have preprocessor options");
return *PPOpts;
}

const FileManager &getFileManager() const { return *FileMgr; }
FileManager &getFileManager() { return *FileMgr; }

const FileSystemOptions &getFileSystemOpts() const { return FileSystemOpts; }

IntrusiveRefCntPtr<ASTReader> getASTReader() const;

StringRef getOriginalSourceFileName() const {
return OriginalSourceFile;
}

ASTMutationListener *getASTMutationListener();
ASTDeserializationListener *getDeserializationListener();

bool getOnlyLocalDecls() const { return OnlyLocalDecls; }

bool getOwnsRemappedFileBuffers() const { return OwnsRemappedFileBuffers; }
void setOwnsRemappedFileBuffers(bool val) { OwnsRemappedFileBuffers = val; }

StringRef getMainFileName() const;


StringRef getASTFileName() const;

using top_level_iterator = std::vector<Decl *>::iterator;

top_level_iterator top_level_begin() {
assert(!isMainFileAST() && "Invalid call for AST based ASTUnit!");
if (!TopLevelDeclsInPreamble.empty())
RealizeTopLevelDeclsFromPreamble();
return TopLevelDecls.begin();
}

top_level_iterator top_level_end() {
assert(!isMainFileAST() && "Invalid call for AST based ASTUnit!");
if (!TopLevelDeclsInPreamble.empty())
RealizeTopLevelDeclsFromPreamble();
return TopLevelDecls.end();
}

std::size_t top_level_size() const {
assert(!isMainFileAST() && "Invalid call for AST based ASTUnit!");
return TopLevelDeclsInPreamble.size() + TopLevelDecls.size();
}

bool top_level_empty() const {
assert(!isMainFileAST() && "Invalid call for AST based ASTUnit!");
return TopLevelDeclsInPreamble.empty() && TopLevelDecls.empty();
}


void addTopLevelDecl(Decl *D) {
TopLevelDecls.push_back(D);
}


void addFileLevelDecl(Decl *D);




void findFileRegionDecls(FileID File, unsigned Offset, unsigned Length,
SmallVectorImpl<Decl *> &Decls);




unsigned &getCurrentTopLevelHashValue() { return CurrentTopLevelHashValue; }






SourceLocation getLocation(const FileEntry *File,
unsigned Line, unsigned Col) const;


SourceLocation getLocation(const FileEntry *File, unsigned Offset) const;




SourceLocation mapLocationFromPreamble(SourceLocation Loc) const;




SourceLocation mapLocationToPreamble(SourceLocation Loc) const;

bool isInPreambleFileID(SourceLocation Loc) const;
bool isInMainFileID(SourceLocation Loc) const;
SourceLocation getStartOfMainFileID() const;
SourceLocation getEndOfPreambleFileID() const;


SourceRange mapRangeFromPreamble(SourceRange R) const {
return SourceRange(mapLocationFromPreamble(R.getBegin()),
mapLocationFromPreamble(R.getEnd()));
}


SourceRange mapRangeToPreamble(SourceRange R) const {
return SourceRange(mapLocationToPreamble(R.getBegin()),
mapLocationToPreamble(R.getEnd()));
}

unsigned getPreambleCounterForTests() const { return PreambleCounter; }


using stored_diag_iterator = StoredDiagnostic *;
using stored_diag_const_iterator = const StoredDiagnostic *;

stored_diag_const_iterator stored_diag_begin() const {
return StoredDiagnostics.begin();
}

stored_diag_iterator stored_diag_begin() {
return StoredDiagnostics.begin();
}

stored_diag_const_iterator stored_diag_end() const {
return StoredDiagnostics.end();
}

stored_diag_iterator stored_diag_end() {
return StoredDiagnostics.end();
}

unsigned stored_diag_size() const { return StoredDiagnostics.size(); }

stored_diag_iterator stored_diag_afterDriver_begin() {
if (NumStoredDiagnosticsFromDriver > StoredDiagnostics.size())
NumStoredDiagnosticsFromDriver = 0;
return StoredDiagnostics.begin() + NumStoredDiagnosticsFromDriver;
}

using cached_completion_iterator =
std::vector<CachedCodeCompletionResult>::iterator;

cached_completion_iterator cached_completion_begin() {
return CachedCompletionResults.begin();
}

cached_completion_iterator cached_completion_end() {
return CachedCompletionResults.end();
}

unsigned cached_completion_size() const {
return CachedCompletionResults.size();
}




llvm::iterator_range<PreprocessingRecord::iterator>
getLocalPreprocessingEntities() const;



using DeclVisitorFn = bool (*)(void *context, const Decl *D);





bool visitLocalTopLevelDecls(void *context, DeclVisitorFn Fn);


const FileEntry *getPCHFile();



bool isModuleFile() const;

std::unique_ptr<llvm::MemoryBuffer>
getBufferForFile(StringRef Filename, std::string *ErrorStr = nullptr);


TranslationUnitKind getTranslationUnitKind() const { return TUKind; }


InputKind getInputKind() const;



using RemappedFile = std::pair<std::string, llvm::MemoryBuffer *>;


static std::unique_ptr<ASTUnit>
create(std::shared_ptr<CompilerInvocation> CI,
IntrusiveRefCntPtr<DiagnosticsEngine> Diags,
CaptureDiagsKind CaptureDiagnostics, bool UserFilesAreVolatile);

enum WhatToLoad {

LoadPreprocessorOnly,


LoadASTOnly,


LoadEverything
};











static std::unique_ptr<ASTUnit>
LoadFromASTFile(const std::string &Filename,
const PCHContainerReader &PCHContainerRdr, WhatToLoad ToLoad,
IntrusiveRefCntPtr<DiagnosticsEngine> Diags,
const FileSystemOptions &FileSystemOpts,
bool UseDebugInfo = false, bool OnlyLocalDecls = false,
CaptureDiagsKind CaptureDiagnostics = CaptureDiagsKind::None,
bool AllowASTWithCompilerErrors = false,
bool UserFilesAreVolatile = false);

private:














bool LoadFromCompilerInvocation(
std::shared_ptr<PCHContainerOperations> PCHContainerOps,
unsigned PrecompilePreambleAfterNParses,
IntrusiveRefCntPtr<llvm::vfs::FileSystem> VFS);

public:




























static ASTUnit *LoadFromCompilerInvocationAction(
std::shared_ptr<CompilerInvocation> CI,
std::shared_ptr<PCHContainerOperations> PCHContainerOps,
IntrusiveRefCntPtr<DiagnosticsEngine> Diags,
FrontendAction *Action = nullptr, ASTUnit *Unit = nullptr,
bool Persistent = true, StringRef ResourceFilesPath = StringRef(),
bool OnlyLocalDecls = false,
CaptureDiagsKind CaptureDiagnostics = CaptureDiagsKind::None,
unsigned PrecompilePreambleAfterNParses = 0,
bool CacheCodeCompletionResults = false,
bool UserFilesAreVolatile = false,
std::unique_ptr<ASTUnit> *ErrAST = nullptr);















static std::unique_ptr<ASTUnit> LoadFromCompilerInvocation(
std::shared_ptr<CompilerInvocation> CI,
std::shared_ptr<PCHContainerOperations> PCHContainerOps,
IntrusiveRefCntPtr<DiagnosticsEngine> Diags, FileManager *FileMgr,
bool OnlyLocalDecls = false,
CaptureDiagsKind CaptureDiagnostics = CaptureDiagsKind::None,
unsigned PrecompilePreambleAfterNParses = 0,
TranslationUnitKind TUKind = TU_Complete,
bool CacheCodeCompletionResults = false,
bool IncludeBriefCommentsInCodeCompletion = false,
bool UserFilesAreVolatile = false);






























static ASTUnit *LoadFromCommandLine(
const char **ArgBegin, const char **ArgEnd,
std::shared_ptr<PCHContainerOperations> PCHContainerOps,
IntrusiveRefCntPtr<DiagnosticsEngine> Diags, StringRef ResourceFilesPath,
bool OnlyLocalDecls = false,
CaptureDiagsKind CaptureDiagnostics = CaptureDiagsKind::None,
ArrayRef<RemappedFile> RemappedFiles = None,
bool RemappedFilesKeepOriginalName = true,
unsigned PrecompilePreambleAfterNParses = 0,
TranslationUnitKind TUKind = TU_Complete,
bool CacheCodeCompletionResults = false,
bool IncludeBriefCommentsInCodeCompletion = false,
bool AllowPCHWithCompilerErrors = false,
SkipFunctionBodiesScope SkipFunctionBodies =
SkipFunctionBodiesScope::None,
bool SingleFileParse = false, bool UserFilesAreVolatile = false,
bool ForSerialization = false,
bool RetainExcludedConditionalBlocks = false,
llvm::Optional<StringRef> ModuleFormat = llvm::None,
std::unique_ptr<ASTUnit> *ErrAST = nullptr,
IntrusiveRefCntPtr<llvm::vfs::FileSystem> VFS = nullptr);












bool Reparse(std::shared_ptr<PCHContainerOperations> PCHContainerOps,
ArrayRef<RemappedFile> RemappedFiles = None,
IntrusiveRefCntPtr<llvm::vfs::FileSystem> VFS = nullptr);




void ResetForParse();





















void CodeComplete(StringRef File, unsigned Line, unsigned Column,
ArrayRef<RemappedFile> RemappedFiles, bool IncludeMacros,
bool IncludeCodePatterns, bool IncludeBriefComments,
CodeCompleteConsumer &Consumer,
std::shared_ptr<PCHContainerOperations> PCHContainerOps,
DiagnosticsEngine &Diag, LangOptions &LangOpts,
SourceManager &SourceMgr, FileManager &FileMgr,
SmallVectorImpl<StoredDiagnostic> &StoredDiagnostics,
SmallVectorImpl<const llvm::MemoryBuffer *> &OwnedBuffers);





bool Save(StringRef File);




bool serialize(raw_ostream &OS);
};

}

#endif
