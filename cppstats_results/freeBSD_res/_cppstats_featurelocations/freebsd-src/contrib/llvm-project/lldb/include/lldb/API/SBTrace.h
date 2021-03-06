







#if !defined(LLDB_API_SBTRACE_H)
#define LLDB_API_SBTRACE_H

#include "lldb/API/SBDefines.h"
#include "lldb/API/SBError.h"

class TraceImpl;

namespace lldb {

class LLDB_API SBTrace {
public:

SBTrace();

SBTrace(const lldb::TraceSP &trace_sp);




const char *GetStartConfigurationHelp();























SBError Start(const SBStructuredData &configuration);



















SBError Start(const SBThread &thread, const SBStructuredData &configuration);











SBError Stop();








SBError Stop(const SBThread &thread);

explicit operator bool() const;

bool IsValid();

protected:
lldb::TraceSP m_opaque_sp;

lldb::ProcessWP m_opaque_wp;
};
}

#endif
