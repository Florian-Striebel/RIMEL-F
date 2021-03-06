







#if !defined(LLDB_HOST_LZMA_H)
#define LLDB_HOST_LZMA_H

#include "llvm/ADT/ArrayRef.h"

namespace llvm {
class Error;
}

namespace lldb_private {

namespace lzma {

bool isAvailable();

llvm::Expected<uint64_t>
getUncompressedSize(llvm::ArrayRef<uint8_t> InputBuffer);

llvm::Error uncompress(llvm::ArrayRef<uint8_t> InputBuffer,
llvm::SmallVectorImpl<uint8_t> &Uncompressed);

}

}

#endif
