







#if !defined(LLD_COFF_DEBUGTYPES_H)
#define LLD_COFF_DEBUGTYPES_H

#include "lld/Common/LLVM.h"
#include "llvm/ADT/BitVector.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/DebugInfo/CodeView/TypeIndexDiscovery.h"
#include "llvm/DebugInfo/CodeView/TypeRecord.h"
#include "llvm/Support/Error.h"
#include "llvm/Support/MemoryBuffer.h"

namespace llvm {
namespace codeview {
struct GloballyHashedType;
}
namespace pdb {
class NativeSession;
class TpiStream;
}
}

namespace lld {
namespace coff {

using llvm::codeview::GloballyHashedType;
using llvm::codeview::TypeIndex;

class ObjFile;
class PDBInputFile;
class TypeMerger;
struct GHashState;

class TpiSource {
public:
enum TpiKind : uint8_t { Regular, PCH, UsingPCH, PDB, PDBIpi, UsingPDB };

TpiSource(TpiKind k, ObjFile *f);
virtual ~TpiSource();












virtual Error mergeDebugT(TypeMerger *m);



virtual void loadGHashes();


virtual void remapTpiWithGHashes(GHashState *g);


bool remapTypeIndex(TypeIndex &ti, llvm::codeview::TiRefKind refKind) const;

protected:
void remapRecord(MutableArrayRef<uint8_t> rec,
ArrayRef<llvm::codeview::TiReference> typeRefs);

void mergeTypeRecord(TypeIndex curIndex, llvm::codeview::CVType ty);




void mergeUniqueTypeRecords(
ArrayRef<uint8_t> debugTypes,
TypeIndex beginIndex = TypeIndex(TypeIndex::FirstNonSimpleIndex));



void fillMapFromGHashes(GHashState *m);




void assignGHashesFromVector(std::vector<GloballyHashedType> &&hashVec);


void fillIsItemIndexFromDebugT();

public:
bool remapTypesInSymbolRecord(MutableArrayRef<uint8_t> rec);

void remapTypesInTypeRecord(MutableArrayRef<uint8_t> rec);



virtual bool isDependency() const { return false; }




bool shouldOmitFromPdb(uint32_t ghashIdx) {
return ghashIdx == endPrecompGHashIdx;
}


static std::vector<TpiSource *> instances;




static ArrayRef<TpiSource *> dependencySources;


static ArrayRef<TpiSource *> objectSources;


static void sortDependencies();

static uint32_t countTypeServerPDBs();
static uint32_t countPrecompObjs();


static void clearGHashes();


static void clear();

const TpiKind kind;
bool ownedGHashes = true;
uint32_t tpiSrcIdx = 0;

protected:



uint32_t endPrecompGHashIdx = ~0U;

public:
ObjFile *file;


Error typeMergingError = Error::success();


llvm::SmallVector<TypeIndex, 0> indexMapStorage;




llvm::ArrayRef<TypeIndex> tpiMap;
llvm::ArrayRef<TypeIndex> ipiMap;



llvm::ArrayRef<llvm::codeview::GloballyHashedType> ghashes;



std::vector<std::pair<TypeIndex, TypeIndex>> funcIdToType;


llvm::BitVector isItemIndex;




std::vector<uint32_t> uniqueTypes;

struct MergedInfo {
std::vector<uint8_t> recs;
std::vector<uint16_t> recSizes;
std::vector<uint32_t> recHashes;
};

MergedInfo mergedTpi;
MergedInfo mergedIpi;

uint64_t nbTypeRecords = 0;
uint64_t nbTypeRecordsBytes = 0;
};

TpiSource *makeTpiSource(ObjFile *file);
TpiSource *makeTypeServerSource(PDBInputFile *pdbInputFile);
TpiSource *makeUseTypeServerSource(ObjFile *file,
llvm::codeview::TypeServer2Record ts);
TpiSource *makePrecompSource(ObjFile *file);
TpiSource *makeUsePrecompSource(ObjFile *file,
llvm::codeview::PrecompRecord ts);

}
}

#endif
