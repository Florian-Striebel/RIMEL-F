











#if !defined(LLVM_CLANG_FRONTEND_PRECOMPILED_PREAMBLE_H)
#define LLVM_CLANG_FRONTEND_PRECOMPILED_PREAMBLE_H

#include "clang/Lex/Lexer.h"
#include "clang/Lex/Preprocessor.h"
#include "llvm/ADT/IntrusiveRefCntPtr.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/AlignOf.h"
#include "llvm/Support/MD5.h"
#include <cstddef>
#include <memory>
#include <system_error>
#include <type_traits>

namespace llvm {
class MemoryBuffer;
class MemoryBufferRef;
namespace vfs {
class FileSystem;
}
}

namespace clang {
class CompilerInstance;
class CompilerInvocation;
class Decl;
class DeclGroupRef;
class PCHContainerOperations;


PreambleBounds ComputePreambleBounds(const LangOptions &LangOpts,
const llvm::MemoryBufferRef &Buffer,
unsigned MaxLines);

class PreambleCallbacks;




class PrecompiledPreamble {
class PCHStorage;
struct PreambleFileHash;

public:
























static llvm::ErrorOr<PrecompiledPreamble>
Build(const CompilerInvocation &Invocation,
const llvm::MemoryBuffer *MainFileBuffer, PreambleBounds Bounds,
DiagnosticsEngine &Diagnostics,
IntrusiveRefCntPtr<llvm::vfs::FileSystem> VFS,
std::shared_ptr<PCHContainerOperations> PCHContainerOps,
bool StoreInMemory, PreambleCallbacks &Callbacks);

PrecompiledPreamble(PrecompiledPreamble &&) = default;
PrecompiledPreamble &operator=(PrecompiledPreamble &&) = default;


PreambleBounds getBounds() const;




std::size_t getSize() const;


llvm::StringRef getContents() const {
return {PreambleBytes.data(), PreambleBytes.size()};
}



bool CanReuse(const CompilerInvocation &Invocation,
const llvm::MemoryBufferRef &MainFileBuffer,
PreambleBounds Bounds, llvm::vfs::FileSystem &VFS) const;









void AddImplicitPreamble(CompilerInvocation &CI,
IntrusiveRefCntPtr<llvm::vfs::FileSystem> &VFS,
llvm::MemoryBuffer *MainFileBuffer) const;




void OverridePreamble(CompilerInvocation &CI,
IntrusiveRefCntPtr<llvm::vfs::FileSystem> &VFS,
llvm::MemoryBuffer *MainFileBuffer) const;

private:
PrecompiledPreamble(PCHStorage Storage, std::vector<char> PreambleBytes,
bool PreambleEndsAtStartOfLine,
llvm::StringMap<PreambleFileHash> FilesInPreamble,
llvm::StringSet<> MissingFiles);






class TempPCHFile {
public:

static llvm::ErrorOr<TempPCHFile> CreateNewPreamblePCHFile();

private:
TempPCHFile(std::string FilePath);

public:
TempPCHFile(TempPCHFile &&Other);
TempPCHFile &operator=(TempPCHFile &&Other);

TempPCHFile(const TempPCHFile &) = delete;
~TempPCHFile();


llvm::StringRef getFilePath() const;

private:
void RemoveFileIfPresent();

private:
llvm::Optional<std::string> FilePath;
};

class InMemoryPreamble {
public:
std::string Data;
};

class PCHStorage {
public:
enum class Kind { Empty, InMemory, TempFile };

PCHStorage() = default;
PCHStorage(TempPCHFile File);
PCHStorage(InMemoryPreamble Memory);

PCHStorage(const PCHStorage &) = delete;
PCHStorage &operator=(const PCHStorage &) = delete;

PCHStorage(PCHStorage &&Other);
PCHStorage &operator=(PCHStorage &&Other);

~PCHStorage();

Kind getKind() const;

TempPCHFile &asFile();
const TempPCHFile &asFile() const;

InMemoryPreamble &asMemory();
const InMemoryPreamble &asMemory() const;

private:
void destroy();
void setEmpty();

private:
Kind StorageKind = Kind::Empty;
llvm::AlignedCharArrayUnion<TempPCHFile, InMemoryPreamble> Storage = {};
};


struct PreambleFileHash {

off_t Size = 0;



time_t ModTime = 0;




llvm::MD5::MD5Result MD5 = {};

static PreambleFileHash createForFile(off_t Size, time_t ModTime);
static PreambleFileHash
createForMemoryBuffer(const llvm::MemoryBufferRef &Buffer);

friend bool operator==(const PreambleFileHash &LHS,
const PreambleFileHash &RHS) {
return LHS.Size == RHS.Size && LHS.ModTime == RHS.ModTime &&
LHS.MD5 == RHS.MD5;
}
friend bool operator!=(const PreambleFileHash &LHS,
const PreambleFileHash &RHS) {
return !(LHS == RHS);
}
};



void configurePreamble(PreambleBounds Bounds, CompilerInvocation &CI,
IntrusiveRefCntPtr<llvm::vfs::FileSystem> &VFS,
llvm::MemoryBuffer *MainFileBuffer) const;




static void
setupPreambleStorage(const PCHStorage &Storage,
PreprocessorOptions &PreprocessorOpts,
IntrusiveRefCntPtr<llvm::vfs::FileSystem> &VFS);


PCHStorage Storage;





llvm::StringMap<PreambleFileHash> FilesInPreamble;








llvm::StringSet<> MissingFiles;



std::vector<char> PreambleBytes;

bool PreambleEndsAtStartOfLine;
};


class PreambleCallbacks {
public:
virtual ~PreambleCallbacks() = default;





virtual void BeforeExecute(CompilerInstance &CI);



virtual void AfterExecute(CompilerInstance &CI);


virtual void AfterPCHEmitted(ASTWriter &Writer);



virtual void HandleTopLevelDecl(DeclGroupRef DG);


virtual std::unique_ptr<PPCallbacks> createPPCallbacks();

virtual CommentHandler *getCommentHandler();



virtual bool shouldSkipFunctionBody(Decl *D) { return true; }
};

enum class BuildPreambleError {
CouldntCreateTempFile = 1,
CouldntCreateTargetInfo,
BeginSourceFileFailed,
CouldntEmitPCH,
BadInputs
};

class BuildPreambleErrorCategory final : public std::error_category {
public:
const char *name() const noexcept override;
std::string message(int condition) const override;
};

std::error_code make_error_code(BuildPreambleError Error);
}

namespace std {
template <>
struct is_error_code_enum<clang::BuildPreambleError> : std::true_type {};
}

#endif
