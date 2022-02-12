







#if !defined(LLDB_CORE_DUMPDATAEXTRACTOR_H)
#define LLDB_CORE_DUMPDATAEXTRACTOR_H

#include "lldb/lldb-enumerations.h"
#include "lldb/lldb-types.h"

#include <cstddef>
#include <cstdint>

namespace lldb_private {
class DataExtractor;
class ExecutionContextScope;
class Stream;



























































lldb::offset_t
DumpDataExtractor(const DataExtractor &DE, Stream *s, lldb::offset_t offset,
lldb::Format item_format, size_t item_byte_size,
size_t item_count, size_t num_per_line, uint64_t base_addr,
uint32_t item_bit_size, uint32_t item_bit_offset,
ExecutionContextScope *exe_scope = nullptr);

void DumpHexBytes(Stream *s, const void *src, size_t src_len,
uint32_t bytes_per_line, lldb::addr_t base_addr);
}

#endif
