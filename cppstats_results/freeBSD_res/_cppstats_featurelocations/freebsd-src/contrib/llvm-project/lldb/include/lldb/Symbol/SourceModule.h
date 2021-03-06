







#if !defined(LLDB_SYMBOL_SOURCEMODULE_H)
#define LLDB_SYMBOL_SOURCEMODULE_H

#include "lldb/Utility/ConstString.h"
#include <vector>

namespace lldb_private {


struct SourceModule {

std::vector<ConstString> path;
ConstString search_path;
ConstString sysroot;
};

}

#endif
