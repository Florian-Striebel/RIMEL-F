







#if !defined(LLD_ELF_SCRIPT_PARSER_H)
#define LLD_ELF_SCRIPT_PARSER_H

#include "lld/Common/LLVM.h"
#include "llvm/Support/MemoryBuffer.h"

namespace lld {
namespace elf {



void readLinkerScript(MemoryBufferRef mb);


void readVersionScript(MemoryBufferRef mb);

void readDynamicList(MemoryBufferRef mb);


void readDefsym(StringRef name, MemoryBufferRef mb);

bool hasWildcard(StringRef s);

}
}

#endif
