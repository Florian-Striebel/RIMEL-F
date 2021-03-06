







#if !defined(LLDB_API_SBLANGUAGERUNTIME_H)
#define LLDB_API_SBLANGUAGERUNTIME_H

#include "lldb/API/SBDefines.h"

namespace lldb {

class SBLanguageRuntime {
public:
static lldb::LanguageType GetLanguageTypeFromString(const char *string);

static const char *GetNameForLanguageType(lldb::LanguageType language);
};

}

#endif
