







#if !defined(LLD_COFF_TYPEMERGER_H)
#define LLD_COFF_TYPEMERGER_H

#include "Config.h"
#include "llvm/DebugInfo/CodeView/MergingTypeTableBuilder.h"
#include "llvm/DebugInfo/CodeView/TypeHashing.h"
#include "llvm/Support/Allocator.h"
#include <atomic>

namespace lld {
namespace coff {

using llvm::codeview::GloballyHashedType;
using llvm::codeview::TypeIndex;

struct GHashState;

class TypeMerger {
public:
TypeMerger(llvm::BumpPtrAllocator &alloc);

~TypeMerger();


inline llvm::codeview::TypeCollection &getTypeTable() {
assert(!config->debugGHashes);
return typeTable;
}


inline llvm::codeview::TypeCollection &getIDTable() {
assert(!config->debugGHashes);
return idTable;
}



void mergeTypesWithGHash();



llvm::DenseMap<TypeIndex, TypeIndex> funcIdToType;


llvm::codeview::MergingTypeTableBuilder typeTable;


llvm::codeview::MergingTypeTableBuilder idTable;



SmallVector<uint32_t, 0> tpiCounts;
SmallVector<uint32_t, 0> ipiCounts;
};

}
}

#endif
