







#if !defined(LLDB_API_SBMEMORYREGIONINFO_H)
#define LLDB_API_SBMEMORYREGIONINFO_H

#include "lldb/API/SBData.h"
#include "lldb/API/SBDefines.h"

namespace lldb {

class LLDB_API SBMemoryRegionInfo {
public:
SBMemoryRegionInfo();

SBMemoryRegionInfo(const lldb::SBMemoryRegionInfo &rhs);

~SBMemoryRegionInfo();

const lldb::SBMemoryRegionInfo &
operator=(const lldb::SBMemoryRegionInfo &rhs);

void Clear();





lldb::addr_t GetRegionBase();





lldb::addr_t GetRegionEnd();





bool IsReadable();





bool IsWritable();





bool IsExecutable();






bool IsMapped();








const char *GetName();






bool HasDirtyMemoryPageList();










uint32_t GetNumDirtyPages();








addr_t GetDirtyPageAddressAtIndex(uint32_t idx);






int GetPageSize();

bool operator==(const lldb::SBMemoryRegionInfo &rhs) const;

bool operator!=(const lldb::SBMemoryRegionInfo &rhs) const;

bool GetDescription(lldb::SBStream &description);

private:
friend class SBProcess;
friend class SBMemoryRegionInfoList;

lldb_private::MemoryRegionInfo &ref();

const lldb_private::MemoryRegionInfo &ref() const;


SBMemoryRegionInfo(const lldb_private::MemoryRegionInfo *lldb_object_ptr);

lldb::MemoryRegionInfoUP m_opaque_up;
};

}

#endif
