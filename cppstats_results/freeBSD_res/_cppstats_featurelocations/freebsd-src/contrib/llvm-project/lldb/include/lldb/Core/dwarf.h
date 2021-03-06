







#if !defined(LLDB_CORE_DWARF_H)
#define LLDB_CORE_DWARF_H

#include "lldb/Utility/RangeMap.h"
#include <cstdint>


#include "llvm/BinaryFormat/Dwarf.h"


using namespace llvm::dwarf;

typedef uint32_t dw_uleb128_t;
typedef int32_t dw_sleb128_t;
typedef uint16_t dw_attr_t;
typedef uint16_t dw_form_t;
typedef llvm::dwarf::Tag dw_tag_t;
typedef uint64_t dw_addr_t;



typedef uint32_t dw_offset_t;



#define DW_INVALID_OFFSET (~(dw_offset_t)0)
#define DW_INVALID_INDEX 0xFFFFFFFFul



#define DW_EH_PE_MASK_ENCODING 0x0F

typedef lldb_private::RangeVector<dw_addr_t, dw_addr_t, 2> DWARFRangeList;

#endif
