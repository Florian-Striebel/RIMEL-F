







#if !defined(LLD_COFF_DRIVER_H)
#define LLD_COFF_DRIVER_H

#include "Config.h"
#include "SymbolTable.h"
#include "lld/Common/LLVM.h"
#include "lld/Common/Reproduce.h"
#include "llvm/ADT/Optional.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/StringSet.h"
#include "llvm/Object/Archive.h"
#include "llvm/Object/COFF.h"
#include "llvm/Option/Arg.h"
#include "llvm/Option/ArgList.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/TarWriter.h"
#include <memory>
#include <set>
#include <vector>

namespace lld {
namespace coff {

class LinkerDriver;
extern LinkerDriver *driver;

using llvm::COFF::MachineTypes;
using llvm::COFF::WindowsSubsystem;
using llvm::Optional;

class COFFOptTable : public llvm::opt::OptTable {
public:
COFFOptTable();
};



extern COFFOptTable optTable;






struct ParsedDirectives {
std::vector<StringRef> exports;
std::vector<StringRef> includes;
llvm::opt::InputArgList args;
};

class ArgParser {
public:

llvm::opt::InputArgList parse(llvm::ArrayRef<const char *> args);


llvm::opt::InputArgList parse(StringRef s) { return parse(tokenize(s)); }




ParsedDirectives parseDirectives(StringRef s);

private:

void addLINK(SmallVector<const char *, 256> &argv);

std::vector<const char *> tokenize(StringRef s);
};

class LinkerDriver {
public:
void linkerMain(llvm::ArrayRef<const char *> args);


void parseDirectives(InputFile *file);


void enqueueArchiveMember(const Archive::Child &c, const Archive::Symbol &sym,
StringRef parentName);

void enqueuePDB(StringRef Path) { enqueuePath(Path, false, false); }

MemoryBufferRef takeBuffer(std::unique_ptr<MemoryBuffer> mb);

void enqueuePath(StringRef path, bool wholeArchive, bool lazy);

std::unique_ptr<llvm::TarWriter> tar;

private:

Optional<StringRef> findFile(StringRef filename);
Optional<StringRef> findLib(StringRef filename);
StringRef doFindFile(StringRef filename);
StringRef doFindLib(StringRef filename);
StringRef doFindLibMinGW(StringRef filename);


void addLibSearchPaths();


std::vector<StringRef> searchPaths;



void convertResources();

void maybeExportMinGWSymbols(const llvm::opt::InputArgList &args);



std::set<llvm::sys::fs::UniqueID> visitedFiles;

std::set<std::string> visitedLibs;

Symbol *addUndefined(StringRef sym);

StringRef mangleMaybe(Symbol *s);








StringRef findDefaultEntry();
WindowsSubsystem inferSubsystem();

void addBuffer(std::unique_ptr<MemoryBuffer> mb, bool wholeArchive,
bool lazy);
void addArchiveBuffer(MemoryBufferRef mbref, StringRef symName,
StringRef parentName, uint64_t offsetInArchive);

void enqueueTask(std::function<void()> task);
bool run();

std::list<std::function<void()>> taskQueue;
std::vector<StringRef> filePaths;
std::vector<MemoryBufferRef> resources;

llvm::StringSet<> directivesExports;
};



void printHelp(const char *argv0);


void parseNumbers(StringRef arg, uint64_t *addr, uint64_t *size = nullptr);

void parseGuard(StringRef arg);



void parseVersion(StringRef arg, uint32_t *major, uint32_t *minor);


void parseSubsystem(StringRef arg, WindowsSubsystem *sys, uint32_t *major,
uint32_t *minor, bool *gotVersion = nullptr);

void parseAlternateName(StringRef);
void parseMerge(StringRef);
void parseSection(StringRef);
void parseAligncomm(StringRef);


void parseFunctionPadMin(llvm::opt::Arg *a, llvm::COFF::MachineTypes machine);


void parseManifest(StringRef arg);


void parseManifestUAC(StringRef arg);


void parseSwaprun(StringRef arg);


std::unique_ptr<MemoryBuffer> createManifestRes();
void createSideBySideManifest();


Export parseExport(StringRef arg);
void fixupExports();
void assignExportOrdinals();





void checkFailIfMismatch(StringRef arg, InputFile *source);


MemoryBufferRef convertResToCOFF(ArrayRef<MemoryBufferRef> mbs,
ArrayRef<ObjFile *> objs);


enum {
OPT_INVALID = 0,
#define OPTION(_1, _2, ID, _4, _5, _6, _7, _8, _9, _10, _11, _12) OPT_##ID,
#include "Options.inc"
#undef OPTION
};

}
}

#endif
