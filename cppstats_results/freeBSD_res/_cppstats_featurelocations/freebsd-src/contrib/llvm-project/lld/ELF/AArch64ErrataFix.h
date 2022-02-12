







#if !defined(LLD_ELF_AARCH64ERRATAFIX_H)
#define LLD_ELF_AARCH64ERRATAFIX_H

#include "lld/Common/LLVM.h"
#include <map>
#include <vector>

namespace lld {
namespace elf {

class Defined;
class InputSection;
class InputSectionDescription;
class OutputSection;
class Patch843419Section;

class AArch64Err843419Patcher {
public:

bool createFixes();

private:
std::vector<Patch843419Section *>
patchInputSectionDescription(InputSectionDescription &isd);

void insertPatches(InputSectionDescription &isd,
std::vector<Patch843419Section *> &patches);

void init();




std::map<InputSection *, std::vector<const Defined *>> sectionMap;

bool initialized = false;
};

}
}

#endif
