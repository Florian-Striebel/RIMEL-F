







#if !defined(LLD_DWARF_H)
#define LLD_DWARF_H

#include "lld/Common/LLVM.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/DebugInfo/DWARF/DWARFContext.h"
#include "llvm/DebugInfo/DWARF/DWARFDebugLine.h"
#include <memory>
#include <string>

namespace llvm {
struct DILineInfo;
}

namespace lld {

class DWARFCache {
public:
DWARFCache(std::unique_ptr<llvm::DWARFContext> dwarf);
llvm::Optional<llvm::DILineInfo> getDILineInfo(uint64_t offset,
uint64_t sectionIndex);
llvm::Optional<std::pair<std::string, unsigned>>
getVariableLoc(StringRef name);

llvm::DWARFContext *getContext() { return dwarf.get(); }

private:
std::unique_ptr<llvm::DWARFContext> dwarf;
std::vector<const llvm::DWARFDebugLine::LineTable *> lineTables;
struct VarLoc {
const llvm::DWARFDebugLine::LineTable *lt;
unsigned file;
unsigned line;
};
llvm::DenseMap<StringRef, VarLoc> variableLoc;
};

}

#endif
