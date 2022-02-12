


















#if !defined(LLD_COFF_LTO_H)
#define LLD_COFF_LTO_H

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
namespace coff {

class BitcodeFile;
class InputFile;

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
std::unique_ptr<llvm::raw_fd_ostream> indexFile;
llvm::DenseSet<StringRef> thinIndices;
};
}
}

#endif
