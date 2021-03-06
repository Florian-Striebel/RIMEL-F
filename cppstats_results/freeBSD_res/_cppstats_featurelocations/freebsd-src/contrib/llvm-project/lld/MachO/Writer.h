







#if !defined(LLD_MACHO_WRITER_H)
#define LLD_MACHO_WRITER_H

#include <cstdint>

namespace lld {
namespace macho {

class OutputSection;
class InputSection;
class Symbol;

class LoadCommand {
public:
virtual ~LoadCommand() = default;
virtual uint32_t getSize() const = 0;
virtual void writeTo(uint8_t *buf) const = 0;
};

template <class LP> void writeResult();

void createSyntheticSections();


void addNonLazyBindingEntries(const Symbol *, const InputSection *,
uint64_t offset, int64_t addend = 0);

extern OutputSection *firstTLVDataSection;

}
}

#endif
