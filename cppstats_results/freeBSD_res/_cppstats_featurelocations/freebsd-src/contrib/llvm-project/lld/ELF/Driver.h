







#if !defined(LLD_ELF_DRIVER_H)
#define LLD_ELF_DRIVER_H

#include "LTO.h"
#include "SymbolTable.h"
#include "lld/Common/LLVM.h"
#include "lld/Common/Reproduce.h"
#include "llvm/ADT/Optional.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/StringSet.h"
#include "llvm/Option/ArgList.h"
#include "llvm/Support/raw_ostream.h"

namespace lld {
namespace elf {

extern class LinkerDriver *driver;

class LinkerDriver {
public:
void linkerMain(ArrayRef<const char *> args);
void addFile(StringRef path, bool withLOption);
void addLibrary(StringRef name);

private:
void createFiles(llvm::opt::InputArgList &args);
void inferMachineType();
template <class ELFT> void link(llvm::opt::InputArgList &args);
template <class ELFT> void compileBitcodeFiles();


bool inWholeArchive = false;


bool inLib = false;


std::unique_ptr<BitcodeCompiler> lto;

std::vector<InputFile *> files;
};


class ELFOptTable : public llvm::opt::OptTable {
public:
ELFOptTable();
llvm::opt::InputArgList parse(ArrayRef<const char *> argv);
};


enum {
OPT_INVALID = 0,
#define OPTION(_1, _2, ID, _4, _5, _6, _7, _8, _9, _10, _11, _12) OPT_##ID,
#include "Options.inc"
#undef OPTION
};

void printHelp();
std::string createResponseFile(const llvm::opt::InputArgList &args);

llvm::Optional<std::string> findFromSearchPaths(StringRef path);
llvm::Optional<std::string> searchScript(StringRef path);
llvm::Optional<std::string> searchLibraryBaseName(StringRef path);
llvm::Optional<std::string> searchLibrary(StringRef path);

}
}

#endif
