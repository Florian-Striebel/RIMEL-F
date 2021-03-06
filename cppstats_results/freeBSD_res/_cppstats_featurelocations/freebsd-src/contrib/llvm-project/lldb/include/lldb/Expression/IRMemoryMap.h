







#if !defined(LLDB_EXPRESSION_IRMEMORYMAP_H)
#define LLDB_EXPRESSION_IRMEMORYMAP_H

#include "lldb/Utility/DataBufferHeap.h"
#include "lldb/Utility/UserID.h"
#include "lldb/lldb-public.h"

#include <map>

namespace lldb_private {















class IRMemoryMap {
public:
IRMemoryMap(lldb::TargetSP target_sp);
~IRMemoryMap();

enum AllocationPolicy : uint8_t {
eAllocationPolicyInvalid =
0,
eAllocationPolicyHostOnly,



eAllocationPolicyMirror,


eAllocationPolicyProcessOnly

};

lldb::addr_t Malloc(size_t size, uint8_t alignment, uint32_t permissions,
AllocationPolicy policy, bool zero_memory, Status &error);
void Leak(lldb::addr_t process_address, Status &error);
void Free(lldb::addr_t process_address, Status &error);

void WriteMemory(lldb::addr_t process_address, const uint8_t *bytes,
size_t size, Status &error);
void WriteScalarToMemory(lldb::addr_t process_address, Scalar &scalar,
size_t size, Status &error);
void WritePointerToMemory(lldb::addr_t process_address, lldb::addr_t address,
Status &error);
void ReadMemory(uint8_t *bytes, lldb::addr_t process_address, size_t size,
Status &error);
void ReadScalarFromMemory(Scalar &scalar, lldb::addr_t process_address,
size_t size, Status &error);
void ReadPointerFromMemory(lldb::addr_t *address,
lldb::addr_t process_address, Status &error);
bool GetAllocSize(lldb::addr_t address, size_t &size);
void GetMemoryData(DataExtractor &extractor, lldb::addr_t process_address,
size_t size, Status &error);

lldb::ByteOrder GetByteOrder();
uint32_t GetAddressByteSize();


ExecutionContextScope *GetBestExecutionContextScope() const;

lldb::TargetSP GetTarget() { return m_target_wp.lock(); }

protected:



lldb::ProcessWP &GetProcessWP() { return m_process_wp; }

private:
struct Allocation {
lldb::addr_t
m_process_alloc;
lldb::addr_t
m_process_start;
size_t m_size;
DataBufferHeap m_data;


AllocationPolicy m_policy;
bool m_leak;
uint8_t m_permissions;


uint8_t m_alignment;

public:
Allocation(lldb::addr_t process_alloc, lldb::addr_t process_start,
size_t size, uint32_t permissions, uint8_t alignment,
AllocationPolicy m_policy);

Allocation(const Allocation &) = delete;
const Allocation &operator=(const Allocation &) = delete;
};

static_assert(sizeof(Allocation) <=
(4 * sizeof(lldb::addr_t)) + sizeof(DataBufferHeap),
"IRMemoryMap::Allocation is larger than expected");

lldb::ProcessWP m_process_wp;
lldb::TargetWP m_target_wp;
typedef std::map<lldb::addr_t, Allocation> AllocationMap;
AllocationMap m_allocations;

lldb::addr_t FindSpace(size_t size);
bool ContainsHostOnlyAllocations();
AllocationMap::iterator FindAllocation(lldb::addr_t addr, size_t size);



bool IntersectsAllocation(lldb::addr_t addr, size_t size) const;


static bool AllocationsIntersect(lldb::addr_t addr1, size_t size1,
lldb::addr_t addr2, size_t size2);
};
}

#endif
