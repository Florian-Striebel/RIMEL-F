







#if !defined(LLDB_TARGET_POSTMORTEMPROCESS_H)
#define LLDB_TARGET_POSTMORTEMPROCESS_H

#include "lldb/Target/Process.h"

namespace lldb_private {








class PostMortemProcess : public Process {
public:
using Process::Process;

bool IsLiveDebugSession() const override { return false; }
};

}

#endif
