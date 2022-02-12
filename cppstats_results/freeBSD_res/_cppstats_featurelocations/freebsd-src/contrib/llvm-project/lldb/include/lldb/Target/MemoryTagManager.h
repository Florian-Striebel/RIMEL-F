







#if !defined(LLDB_TARGET_MEMORYTAGMANAGER_H)
#define LLDB_TARGET_MEMORYTAGMANAGER_H

#include "lldb/Target/MemoryRegionInfo.h"
#include "lldb/Utility/RangeMap.h"
#include "lldb/lldb-private.h"
#include "llvm/Support/Error.h"

namespace lldb_private {










class MemoryTagManager {
public:
typedef Range<lldb::addr_t, lldb::addr_t> TagRange;





virtual lldb::addr_t GetLogicalTag(lldb::addr_t addr) const = 0;


virtual lldb::addr_t RemoveNonAddressBits(lldb::addr_t addr) const = 0;




virtual ptrdiff_t AddressDiff(lldb::addr_t addr1,
lldb::addr_t addr2) const = 0;


virtual lldb::addr_t GetGranuleSize() const = 0;










virtual TagRange ExpandToGranule(TagRange range) const = 0;











virtual llvm::Expected<TagRange> MakeTaggedRange(
lldb::addr_t addr, lldb::addr_t end_addr,
const lldb_private::MemoryRegionInfos &memory_regions) const = 0;








virtual int32_t GetAllocationTagType() const = 0;




virtual size_t GetTagSizeInBytes() const = 0;







virtual llvm::Expected<std::vector<lldb::addr_t>>
UnpackTagsData(const std::vector<uint8_t> &tags,
size_t granules = 0) const = 0;





virtual llvm::Expected<std::vector<uint8_t>>
PackTags(const std::vector<lldb::addr_t> &tags) const = 0;





















virtual llvm::Expected<std::vector<lldb::addr_t>>
RepeatTagsForRange(const std::vector<lldb::addr_t> &tags,
TagRange range) const = 0;

virtual ~MemoryTagManager() {}
};

}

#endif
