







#if !defined(LLDB_SYMBOL_LINEENTRY_H)
#define LLDB_SYMBOL_LINEENTRY_H

#include "lldb/Core/AddressRange.h"
#include "lldb/Utility/FileSpec.h"
#include "lldb/lldb-private.h"

namespace lldb_private {



struct LineEntry {



LineEntry();

LineEntry(const lldb::SectionSP &section_sp, lldb::addr_t section_offset,
lldb::addr_t byte_size, const FileSpec &file, uint32_t _line,
uint16_t _column, bool _is_start_of_statement,
bool _is_start_of_basic_block, bool _is_prologue_end,
bool _is_epilogue_begin, bool _is_terminal_entry);




void Clear();

























bool Dump(Stream *s, Target *target, bool show_file, Address::DumpStyle style,
Address::DumpStyle fallback_style, bool show_range) const;

bool GetDescription(Stream *s, lldb::DescriptionLevel level, CompileUnit *cu,
Target *target, bool show_address_only) const;










bool DumpStopContext(Stream *s, bool show_fullpaths) const;







bool IsValid() const;













static int Compare(const LineEntry &lhs, const LineEntry &rhs);






























AddressRange
GetSameLineContiguousAddressRange(bool include_inlined_functions) const;





void ApplyFileMappings(lldb::TargetSP target_sp);


AddressRange range;
FileSpec file;

FileSpec original_file;
uint32_t line = LLDB_INVALID_LINE_NUMBER;


uint16_t column =
0;

uint16_t is_start_of_statement : 1,

is_start_of_basic_block : 1,

is_prologue_end : 1,


is_epilogue_begin : 1,


is_terminal_entry : 1;


};











bool operator<(const LineEntry &lhs, const LineEntry &rhs);

}

#endif
