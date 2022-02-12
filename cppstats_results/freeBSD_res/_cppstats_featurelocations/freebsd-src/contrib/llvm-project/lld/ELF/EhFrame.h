







#if !defined(LLD_ELF_EHFRAME_H)
#define LLD_ELF_EHFRAME_H

#include "lld/Common/LLVM.h"

namespace lld {
namespace elf {
class InputSectionBase;
struct EhSectionPiece;

size_t readEhRecordSize(InputSectionBase *s, size_t off);
uint8_t getFdeEncoding(EhSectionPiece *p);
bool hasLSDA(const EhSectionPiece &p);
}
}

#endif
