







#if !defined(LLD_ELF_ARMA8ERRATAFIX_H)
#define LLD_ELF_ARMA8ERRATAFIX_H

#include "lld/Common/LLVM.h"
#include "llvm/ADT/DenseMap.h"
#include <map>
#include <vector>

namespace lld {
namespace elf {

class Defined;
class InputSection;
class InputSectionDescription;
class OutputSection;
class Patch657417Section;

class ARMErr657417Patcher {
public:

bool createFixes();

private:
std::vector<Patch657417Section *>
patchInputSectionDescription(InputSectionDescription &isd);

void insertPatches(InputSectionDescription &isd,
std::vector<Patch657417Section *> &patches);

void init();




llvm::DenseMap<InputSection *, std::vector<const Defined *>> sectionMap;

bool initialized = false;
};

}
}

#endif
