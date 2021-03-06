







#if !defined(LLD_MACHO_DWARF_H)
#define LLD_MACHO_DWARF_H

#include "llvm/ADT/StringRef.h"
#include "llvm/DebugInfo/DWARF/DWARFObject.h"

namespace lld {
namespace macho {

class ObjFile;



class DwarfObject final : public llvm::DWARFObject {
public:
bool isLittleEndian() const override { return true; }

llvm::Optional<llvm::RelocAddrEntry> find(const llvm::DWARFSection &sec,
uint64_t pos) const override {

return llvm::None;
}

void forEachInfoSections(
llvm::function_ref<void(const llvm::DWARFSection &)> f) const override {
f(infoSection);
}

llvm::StringRef getAbbrevSection() const override { return abbrevSection; }
llvm::StringRef getStrSection() const override { return strSection; }



static std::unique_ptr<DwarfObject> create(ObjFile *);

private:
llvm::DWARFSection infoSection;
llvm::StringRef abbrevSection;
llvm::StringRef strSection;
};

}
}

#endif
