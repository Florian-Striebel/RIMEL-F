







#if !defined(LLDB_TARGET_MEMORY_H)
#define LLDB_TARGET_MEMORY_H

#include "lldb/Utility/RangeMap.h"
#include "lldb/lldb-private.h"
#include <map>
#include <mutex>
#include <vector>

namespace lldb_private {


class MemoryCache {
public:

MemoryCache(Process &process);

~MemoryCache();

void Clear(bool clear_invalid_ranges = false);

void Flush(lldb::addr_t addr, size_t size);

size_t Read(lldb::addr_t addr, void *dst, size_t dst_len, Status &error);

uint32_t GetMemoryCacheLineSize() const { return m_L2_cache_line_byte_size; }

void AddInvalidRange(lldb::addr_t base_addr, lldb::addr_t byte_size);

bool RemoveInvalidRange(lldb::addr_t base_addr, lldb::addr_t byte_size);


void AddL1CacheData(lldb::addr_t addr, const void *src, size_t src_len);

void AddL1CacheData(lldb::addr_t addr,
const lldb::DataBufferSP &data_buffer_sp);

protected:
typedef std::map<lldb::addr_t, lldb::DataBufferSP> BlockMap;
typedef RangeVector<lldb::addr_t, lldb::addr_t, 4> InvalidRanges;
typedef Range<lldb::addr_t, lldb::addr_t> AddrRange;

std::recursive_mutex m_mutex;
BlockMap m_L1_cache;


BlockMap m_L2_cache;

InvalidRanges m_invalid_ranges;
Process &m_process;
uint32_t m_L2_cache_line_byte_size;

private:
MemoryCache(const MemoryCache &) = delete;
const MemoryCache &operator=(const MemoryCache &) = delete;
};



class AllocatedBlock {
public:
AllocatedBlock(lldb::addr_t addr, uint32_t byte_size, uint32_t permissions,
uint32_t chunk_size);

~AllocatedBlock();

lldb::addr_t ReserveBlock(uint32_t size);

bool FreeBlock(lldb::addr_t addr);

lldb::addr_t GetBaseAddress() const { return m_range.GetRangeBase(); }

uint32_t GetByteSize() const { return m_range.GetByteSize(); }

uint32_t GetPermissions() const { return m_permissions; }

uint32_t GetChunkSize() const { return m_chunk_size; }

bool Contains(lldb::addr_t addr) const {
return m_range.Contains(addr);
}

protected:
uint32_t TotalChunks() const { return GetByteSize() / GetChunkSize(); }

uint32_t CalculateChunksNeededForSize(uint32_t size) const {
return (size + m_chunk_size - 1) / m_chunk_size;
}

Range<lldb::addr_t, uint32_t> m_range;

const uint32_t m_permissions;

const uint32_t m_chunk_size;

RangeVector<lldb::addr_t, uint32_t> m_free_blocks;

RangeVector<lldb::addr_t, uint32_t> m_reserved_blocks;
};




class AllocatedMemoryCache {
public:

AllocatedMemoryCache(Process &process);

~AllocatedMemoryCache();

void Clear();

lldb::addr_t AllocateMemory(size_t byte_size, uint32_t permissions,
Status &error);

bool DeallocateMemory(lldb::addr_t ptr);

protected:
typedef std::shared_ptr<AllocatedBlock> AllocatedBlockSP;

AllocatedBlockSP AllocatePage(uint32_t byte_size, uint32_t permissions,
uint32_t chunk_size, Status &error);


Process &m_process;
std::recursive_mutex m_mutex;
typedef std::multimap<uint32_t, AllocatedBlockSP> PermissionsToBlockMap;
PermissionsToBlockMap m_memory_map;

private:
AllocatedMemoryCache(const AllocatedMemoryCache &) = delete;
const AllocatedMemoryCache &operator=(const AllocatedMemoryCache &) = delete;
};

}

#endif
