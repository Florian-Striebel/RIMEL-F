







#if !defined(LLDB_API_SBSTRUCTUREDDATA_H)
#define LLDB_API_SBSTRUCTUREDDATA_H

#include "lldb/API/SBDefines.h"
#include "lldb/API/SBModule.h"

namespace lldb {

class SBStructuredData {
public:
SBStructuredData();

SBStructuredData(const lldb::SBStructuredData &rhs);

SBStructuredData(const lldb::EventSP &event_sp);

SBStructuredData(lldb_private::StructuredDataImpl *impl);

~SBStructuredData();

lldb::SBStructuredData &operator=(const lldb::SBStructuredData &rhs);

explicit operator bool() const;

bool IsValid() const;

lldb::SBError SetFromJSON(lldb::SBStream &stream);

lldb::SBError SetFromJSON(const char *json);

void Clear();

lldb::SBError GetAsJSON(lldb::SBStream &stream) const;

lldb::SBError GetDescription(lldb::SBStream &stream) const;


lldb::StructuredDataType GetType() const;




size_t GetSize() const;



bool GetKeys(lldb::SBStringList &keys) const;



lldb::SBStructuredData GetValueForKey(const char *key) const;



lldb::SBStructuredData GetItemAtIndex(size_t idx) const;


uint64_t GetIntegerValue(uint64_t fail_value = 0) const;



double GetFloatValue(double fail_value = 0.0) const;


bool GetBooleanValue(bool fail_value = false) const;
















size_t GetStringValue(char *dst, size_t dst_len) const;

protected:
friend class SBLaunchInfo;
friend class SBDebugger;
friend class SBTarget;
friend class SBProcess;
friend class SBThread;
friend class SBThreadPlan;
friend class SBBreakpoint;
friend class SBBreakpointLocation;
friend class SBBreakpointName;
friend class SBTrace;

StructuredDataImplUP m_impl_up;
};
}

#endif
