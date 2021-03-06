







#if !defined(LLD_ELF_RELOCATIONS_H)
#define LLD_ELF_RELOCATIONS_H

#include "lld/Common/LLVM.h"
#include "llvm/ADT/DenseMap.h"
#include <map>
#include <vector>

namespace lld {
namespace elf {
class Symbol;
class InputSection;
class InputSectionBase;
class OutputSection;
class SectionBase;


using RelType = uint32_t;
using JumpModType = uint32_t;




enum RelExpr {
R_ABS,
R_ADDEND,
R_DTPREL,
R_GOT,
R_GOT_OFF,
R_GOT_PC,
R_GOTONLY_PC,
R_GOTPLTONLY_PC,
R_GOTPLT,
R_GOTPLTREL,
R_GOTREL,
R_NONE,
R_PC,
R_PLT,
R_PLT_PC,
R_RELAX_GOT_PC,
R_RELAX_GOT_PC_NOPIC,
R_RELAX_TLS_GD_TO_IE,
R_RELAX_TLS_GD_TO_IE_ABS,
R_RELAX_TLS_GD_TO_IE_GOT_OFF,
R_RELAX_TLS_GD_TO_IE_GOTPLT,
R_RELAX_TLS_GD_TO_LE,
R_RELAX_TLS_GD_TO_LE_NEG,
R_RELAX_TLS_IE_TO_LE,
R_RELAX_TLS_LD_TO_LE,
R_RELAX_TLS_LD_TO_LE_ABS,
R_SIZE,
R_TPREL,
R_TPREL_NEG,
R_TLSDESC,
R_TLSDESC_CALL,
R_TLSDESC_PC,
R_TLSGD_GOT,
R_TLSGD_GOTPLT,
R_TLSGD_PC,
R_TLSIE_HINT,
R_TLSLD_GOT,
R_TLSLD_GOTPLT,
R_TLSLD_GOT_OFF,
R_TLSLD_HINT,
R_TLSLD_PC,






R_AARCH64_GOT_PAGE_PC,
R_AARCH64_GOT_PAGE,
R_AARCH64_PAGE_PC,
R_AARCH64_RELAX_TLS_GD_TO_IE_PAGE_PC,
R_AARCH64_TLSDESC_PAGE,
R_ARM_PCA,
R_ARM_SBREL,
R_MIPS_GOTREL,
R_MIPS_GOT_GP,
R_MIPS_GOT_GP_PC,
R_MIPS_GOT_LOCAL_PAGE,
R_MIPS_GOT_OFF,
R_MIPS_GOT_OFF32,
R_MIPS_TLSGD,
R_MIPS_TLSLD,
R_PPC32_PLTREL,
R_PPC64_CALL,
R_PPC64_CALL_PLT,
R_PPC64_RELAX_TOC,
R_PPC64_TOCBASE,
R_PPC64_RELAX_GOT_PC,
R_RISCV_ADD,
R_RISCV_PC_INDIRECT,
};


struct Relocation {
RelExpr expr;
RelType type;
uint64_t offset;
int64_t addend;
Symbol *sym;
};




struct JumpInstrMod {
JumpModType original;
uint64_t offset;
unsigned size;
};




template <class ELFT> void scanRelocations(InputSectionBase &);

template <class ELFT> void reportUndefinedSymbols();

void hexagonTLSSymbolUpdate(ArrayRef<OutputSection *> outputSections);
bool hexagonNeedsTLSSymbol(ArrayRef<OutputSection *> outputSections);

class ThunkSection;
class Thunk;
class InputSectionDescription;

class ThunkCreator {
public:

bool createThunks(ArrayRef<OutputSection *> outputSections);




uint32_t pass = 0;

private:
void mergeThunks(ArrayRef<OutputSection *> outputSections);

ThunkSection *getISDThunkSec(OutputSection *os, InputSection *isec,
InputSectionDescription *isd,
const Relocation &rel, uint64_t src);

ThunkSection *getISThunkSec(InputSection *isec);

void createInitialThunkSections(ArrayRef<OutputSection *> outputSections);

std::pair<Thunk *, bool> getThunk(InputSection *isec, Relocation &rel,
uint64_t src);

ThunkSection *addThunkSection(OutputSection *os, InputSectionDescription *,
uint64_t off);

bool normalizeExistingThunk(Relocation &rel, uint64_t src);







llvm::DenseMap<std::pair<std::pair<SectionBase *, uint64_t>, int64_t>,
std::vector<Thunk *>>
thunkedSymbolsBySectionAndAddend;
llvm::DenseMap<std::pair<Symbol *, int64_t>, std::vector<Thunk *>>
thunkedSymbols;



llvm::DenseMap<Symbol *, Thunk *> thunks;





llvm::DenseMap<InputSection *, ThunkSection *> thunkedSections;
};



template <class ELFT>
static inline int64_t getAddend(const typename ELFT::Rel &rel) {
return 0;
}
template <class ELFT>
static inline int64_t getAddend(const typename ELFT::Rela &rel) {
return rel.r_addend;
}

template <typename RelTy>
ArrayRef<RelTy> sortRels(ArrayRef<RelTy> rels, SmallVector<RelTy, 0> &storage) {
auto cmp = [](const RelTy &a, const RelTy &b) {
return a.r_offset < b.r_offset;
};
if (!llvm::is_sorted(rels, cmp)) {
storage.assign(rels.begin(), rels.end());
llvm::stable_sort(storage, cmp);
rels = storage;
}
return rels;
}
}
}

#endif
