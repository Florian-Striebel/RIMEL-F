








#if !defined(LLDB_TARGET_MEMORYREGIONINFO_H)
#define LLDB_TARGET_MEMORYREGIONINFO_H

#include <vector>

#include "lldb/Utility/ConstString.h"
#include "lldb/Utility/RangeMap.h"
#include "llvm/ADT/Optional.h"
#include "llvm/Support/FormatProviders.h"

namespace lldb_private {
class MemoryRegionInfo {
public:
typedef Range<lldb::addr_t, lldb::addr_t> RangeType;

enum OptionalBool { eDontKnow = -1, eNo = 0, eYes = 1 };

MemoryRegionInfo() = default;
MemoryRegionInfo(RangeType range, OptionalBool read, OptionalBool write,
OptionalBool execute, OptionalBool mapped, ConstString name,
OptionalBool flash, lldb::offset_t blocksize,
OptionalBool memory_tagged)
: m_range(range), m_read(read), m_write(write), m_execute(execute),
m_mapped(mapped), m_name(name), m_flash(flash), m_blocksize(blocksize),
m_memory_tagged(memory_tagged) {}

RangeType &GetRange() { return m_range; }

void Clear() { *this = MemoryRegionInfo(); }

const RangeType &GetRange() const { return m_range; }

OptionalBool GetReadable() const { return m_read; }

OptionalBool GetWritable() const { return m_write; }

OptionalBool GetExecutable() const { return m_execute; }

OptionalBool GetMapped() const { return m_mapped; }

ConstString GetName() const { return m_name; }

OptionalBool GetMemoryTagged() const { return m_memory_tagged; }

void SetReadable(OptionalBool val) { m_read = val; }

void SetWritable(OptionalBool val) { m_write = val; }

void SetExecutable(OptionalBool val) { m_execute = val; }

void SetMapped(OptionalBool val) { m_mapped = val; }

void SetName(const char *name) { m_name = ConstString(name); }

OptionalBool GetFlash() const { return m_flash; }

void SetFlash(OptionalBool val) { m_flash = val; }

lldb::offset_t GetBlocksize() const { return m_blocksize; }

void SetBlocksize(lldb::offset_t blocksize) { m_blocksize = blocksize; }

void SetMemoryTagged(OptionalBool val) { m_memory_tagged = val; }



uint32_t GetLLDBPermissions() const {
uint32_t permissions = 0;
if (m_read)
permissions |= lldb::ePermissionsReadable;
if (m_write)
permissions |= lldb::ePermissionsWritable;
if (m_execute)
permissions |= lldb::ePermissionsExecutable;
return permissions;
}



void SetLLDBPermissions(uint32_t permissions) {
m_read = (permissions & lldb::ePermissionsReadable) ? eYes : eNo;
m_write = (permissions & lldb::ePermissionsWritable) ? eYes : eNo;
m_execute = (permissions & lldb::ePermissionsExecutable) ? eYes : eNo;
}

bool operator==(const MemoryRegionInfo &rhs) const {
return m_range == rhs.m_range && m_read == rhs.m_read &&
m_write == rhs.m_write && m_execute == rhs.m_execute &&
m_mapped == rhs.m_mapped && m_name == rhs.m_name &&
m_flash == rhs.m_flash && m_blocksize == rhs.m_blocksize &&
m_memory_tagged == rhs.m_memory_tagged &&
m_pagesize == rhs.m_pagesize;
}

bool operator!=(const MemoryRegionInfo &rhs) const { return !(*this == rhs); }




int GetPageSize() { return m_pagesize; }





llvm::Optional<std::vector<lldb::addr_t>> &GetDirtyPageList() {
return m_dirty_pages;
}

void SetPageSize(int pagesize) { m_pagesize = pagesize; }

void SetDirtyPageList(std::vector<lldb::addr_t> pagelist) {
if (m_dirty_pages.hasValue())
m_dirty_pages.getValue().clear();
m_dirty_pages = std::move(pagelist);
}

protected:
RangeType m_range;
OptionalBool m_read = eDontKnow;
OptionalBool m_write = eDontKnow;
OptionalBool m_execute = eDontKnow;
OptionalBool m_mapped = eDontKnow;
ConstString m_name;
OptionalBool m_flash = eDontKnow;
lldb::offset_t m_blocksize = 0;
OptionalBool m_memory_tagged = eDontKnow;
int m_pagesize = 0;
llvm::Optional<std::vector<lldb::addr_t>> m_dirty_pages;
};

inline bool operator<(const MemoryRegionInfo &lhs,
const MemoryRegionInfo &rhs) {
return lhs.GetRange() < rhs.GetRange();
}

inline bool operator<(const MemoryRegionInfo &lhs, lldb::addr_t rhs) {
return lhs.GetRange().GetRangeBase() < rhs;
}

inline bool operator<(lldb::addr_t lhs, const MemoryRegionInfo &rhs) {
return lhs < rhs.GetRange().GetRangeBase();
}

llvm::raw_ostream &operator<<(llvm::raw_ostream &OS,
const MemoryRegionInfo &Info);


class MemoryRegionInfos : public std::vector<lldb_private::MemoryRegionInfo> {
public:

};

}

namespace llvm {
template <>




struct format_provider<lldb_private::MemoryRegionInfo::OptionalBool> {
static void format(const lldb_private::MemoryRegionInfo::OptionalBool &B,
raw_ostream &OS, StringRef Options);
};
}

#endif
