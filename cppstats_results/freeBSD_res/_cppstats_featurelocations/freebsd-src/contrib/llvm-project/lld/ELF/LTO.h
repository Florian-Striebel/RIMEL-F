


















#if !defined(LLD_ELF_LTO_H)
#define LLD_ELF_LTO_H

#include "lld/Common/LLVM.h"
#include "llvm/ADT/DenseSet.h"
#include "llvm/ADT/SmallString.h"
#include "llvm/Support/raw_ostream.h"
#include <memory>
#include <vector>

namespace llvm {
namespace lto {
class LTO;
}
}

namespace lld {
namespace elf {

class BitcodeFile;
class InputFile;
class LazyObjFile;

class BitcodeCompiler {
public:
BitcodeCompiler();
~BitcodeCompiler();

void add(BitcodeFile &f);
std::vector<InputFile *> compile();

private:
std::unique_ptr<llvm::lto::LTO> ltoObj;
std::vector<SmallString<0>> buf;
std::vector<std::unique_ptr<MemoryBuffer>> files;
llvm::DenseSet<StringRef> usedStartStop;
std::unique_ptr<llvm::raw_fd_ostream> indexFile;
llvm::DenseSet<StringRef> thinIndices;
};
}
}

#endif
