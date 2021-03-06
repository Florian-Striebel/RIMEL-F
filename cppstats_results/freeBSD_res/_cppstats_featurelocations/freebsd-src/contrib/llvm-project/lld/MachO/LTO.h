







#if !defined(LLD_MACHO_LTO_H)
#define LLD_MACHO_LTO_H

#include "llvm/ADT/SmallString.h"
#include "llvm/Support/MemoryBuffer.h"
#include <memory>
#include <vector>

namespace llvm {
namespace lto {
class LTO;
}
}

namespace lld {
namespace macho {

class BitcodeFile;
class ObjFile;

class BitcodeCompiler {
public:
BitcodeCompiler();

void add(BitcodeFile &f);
std::vector<ObjFile *> compile();

private:
std::unique_ptr<llvm::lto::LTO> ltoObj;
std::vector<llvm::SmallString<0>> buf;
std::vector<std::unique_ptr<llvm::MemoryBuffer>> files;
};

}
}

#endif
