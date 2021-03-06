







#if !defined(LLD_MACHO_DRIVER_H)
#define LLD_MACHO_DRIVER_H

#include "lld/Common/LLVM.h"
#include "llvm/ADT/Optional.h"
#include "llvm/ADT/SetVector.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Option/OptTable.h"
#include "llvm/Support/MemoryBuffer.h"

#include <set>
#include <type_traits>

namespace llvm {
namespace MachO {
class InterfaceFile;
enum class PlatformKind : unsigned;
}
}

namespace lld {
namespace macho {

class DylibFile;
class InputFile;

class MachOOptTable : public llvm::opt::OptTable {
public:
MachOOptTable();
llvm::opt::InputArgList parse(ArrayRef<const char *> argv);
void printHelp(const char *argv0, bool showHidden) const;
};


enum {
OPT_INVALID = 0,
#define OPTION(_1, _2, ID, _4, _5, _6, _7, _8, _9, _10, _11, _12) OPT_##ID,
#include "Options.inc"
#undef OPTION
};

void parseLCLinkerOption(InputFile *, unsigned argc, StringRef data);

std::string createResponseFile(const llvm::opt::InputArgList &args);


llvm::Optional<std::string> resolveDylibPath(llvm::StringRef path);

DylibFile *loadDylib(llvm::MemoryBufferRef mbref, DylibFile *umbrella = nullptr,
bool isBundleLoader = false);



llvm::Optional<llvm::StringRef>
findPathCombination(const llvm::Twine &name,
const std::vector<llvm::StringRef> &roots,
ArrayRef<llvm::StringRef> extensions = {""});



llvm::StringRef rerootPath(llvm::StringRef path);

llvm::Optional<InputFile *> loadArchiveMember(MemoryBufferRef, uint32_t modTime,
StringRef archiveName,
bool objCOnly,
uint64_t offsetInArchive);

uint32_t getModTime(llvm::StringRef path);

void printArchiveMemberLoad(StringRef reason, const InputFile *);


llvm::MachO::PlatformKind removeSimulator(llvm::MachO::PlatformKind platform);


class DependencyTracker {
public:
explicit DependencyTracker(llvm::StringRef path);


inline void logFileNotFound(const Twine &path) {
if (active)
notFounds.insert(path.str());
}




void write(llvm::StringRef version,
const llvm::SetVector<InputFile *> &inputs,
llvm::StringRef output);

private:
enum DepOpCode : uint8_t {

Version = 0x00,

Input = 0x10,

NotFound = 0x11,

Output = 0x40,
};

const llvm::StringRef path;
bool active;




std::set<std::string> notFounds;
};

extern DependencyTracker *depTracker;

}
}

#endif
