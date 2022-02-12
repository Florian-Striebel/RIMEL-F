







#if !defined(LLDB_SYMBOL_LINETABLE_H)
#define LLDB_SYMBOL_LINETABLE_H

#include "lldb/Core/Address.h"
#include "lldb/Core/ModuleChild.h"
#include "lldb/Core/Section.h"
#include "lldb/Core/SourceLocationSpec.h"
#include "lldb/Symbol/LineEntry.h"
#include "lldb/Utility/RangeMap.h"
#include "lldb/lldb-private.h"
#include <vector>

namespace lldb_private {



class LineSequence {
public:
LineSequence();

virtual ~LineSequence() = default;

virtual void Clear() = 0;

private:
LineSequence(const LineSequence &) = delete;
const LineSequence &operator=(const LineSequence &) = delete;
};



class LineTable {
public:




LineTable(CompileUnit *comp_unit);





LineTable(CompileUnit *comp_unit,
std::vector<std::unique_ptr<LineSequence>> &&sequences);


~LineTable();














void InsertLineEntry(lldb::addr_t file_addr, uint32_t line, uint16_t column,
uint16_t file_idx, bool is_start_of_statement,
bool is_start_of_basic_block, bool is_prologue_end,
bool is_epilogue_begin, bool is_terminal_entry);


static std::unique_ptr<LineSequence> CreateLineSequenceContainer();



static void AppendLineEntryToSequence(LineSequence *sequence, lldb::addr_t file_addr,
uint32_t line, uint16_t column,
uint16_t file_idx, bool is_start_of_statement,
bool is_start_of_basic_block,
bool is_prologue_end, bool is_epilogue_begin,
bool is_terminal_entry);


void InsertSequence(LineSequence *sequence);










void Dump(Stream *s, Target *target, Address::DumpStyle style,
Address::DumpStyle fallback_style, bool show_line_ranges);

void GetDescription(Stream *s, Target *target, lldb::DescriptionLevel level);


















bool FindLineEntryByAddress(const Address &so_addr, LineEntry &line_entry,
uint32_t *index_ptr = nullptr);































uint32_t
FindLineEntryIndexByFileIndex(uint32_t start_idx, uint32_t file_idx,
const SourceLocationSpec &src_location_spec,
LineEntry *line_entry_ptr);

uint32_t FindLineEntryIndexByFileIndex(
uint32_t start_idx, const std::vector<uint32_t> &file_idx,
const SourceLocationSpec &src_location_spec, LineEntry *line_entry_ptr);

size_t FineLineEntriesForFileIndex(uint32_t file_idx, bool append,
SymbolContextList &sc_list);












bool GetLineEntryAtIndex(uint32_t idx, LineEntry &line_entry);





uint32_t GetSize() const;

typedef lldb_private::RangeVector<lldb::addr_t, lldb::addr_t, 32>
FileAddressRanges;













size_t GetContiguousFileAddressRanges(FileAddressRanges &file_ranges,
bool append);

typedef RangeDataVector<lldb::addr_t, lldb::addr_t, lldb::addr_t>
FileRangeMap;

LineTable *LinkLineTable(const FileRangeMap &file_range_map);

protected:
struct Entry {
Entry()
: line(0), is_start_of_statement(false), is_start_of_basic_block(false),
is_prologue_end(false), is_epilogue_begin(false),
is_terminal_entry(false) {}

Entry(lldb::addr_t _file_addr, uint32_t _line, uint16_t _column,
uint16_t _file_idx, bool _is_start_of_statement,
bool _is_start_of_basic_block, bool _is_prologue_end,
bool _is_epilogue_begin, bool _is_terminal_entry)
: file_addr(_file_addr), line(_line),
is_start_of_statement(_is_start_of_statement),
is_start_of_basic_block(_is_start_of_basic_block),
is_prologue_end(_is_prologue_end),
is_epilogue_begin(_is_epilogue_begin),
is_terminal_entry(_is_terminal_entry), column(_column),
file_idx(_file_idx) {}

int bsearch_compare(const void *key, const void *arrmem);

void Clear() {
file_addr = LLDB_INVALID_ADDRESS;
line = 0;
column = 0;
file_idx = 0;
is_start_of_statement = false;
is_start_of_basic_block = false;
is_prologue_end = false;
is_epilogue_begin = false;
is_terminal_entry = false;
}

static int Compare(const Entry &lhs, const Entry &rhs) {

#define SCALAR_COMPARE(a, b) if (a < b) return -1; if (a > b) return +1




SCALAR_COMPARE(lhs.file_addr, rhs.file_addr);
SCALAR_COMPARE(lhs.line, rhs.line);
SCALAR_COMPARE(lhs.column, rhs.column);
SCALAR_COMPARE(lhs.is_start_of_statement, rhs.is_start_of_statement);
SCALAR_COMPARE(lhs.is_start_of_basic_block, rhs.is_start_of_basic_block);

SCALAR_COMPARE(rhs.is_prologue_end, lhs.is_prologue_end);
SCALAR_COMPARE(lhs.is_epilogue_begin, rhs.is_epilogue_begin);

SCALAR_COMPARE(rhs.is_terminal_entry, lhs.is_terminal_entry);
SCALAR_COMPARE(lhs.file_idx, rhs.file_idx);
#undef SCALAR_COMPARE
return 0;
}

class LessThanBinaryPredicate {
public:
LessThanBinaryPredicate(LineTable *line_table);
bool operator()(const LineTable::Entry &, const LineTable::Entry &) const;
bool operator()(const std::unique_ptr<LineSequence> &,
const std::unique_ptr<LineSequence> &) const;

protected:
LineTable *m_line_table;
};

static bool EntryAddressLessThan(const Entry &lhs, const Entry &rhs) {
return lhs.file_addr < rhs.file_addr;
}



lldb::addr_t file_addr = LLDB_INVALID_ADDRESS;


uint32_t line : 27;

uint32_t is_start_of_statement : 1;

uint32_t is_start_of_basic_block : 1;


uint32_t is_prologue_end : 1;


uint32_t is_epilogue_begin : 1;


uint32_t is_terminal_entry : 1;


uint16_t column = 0;


uint16_t file_idx = 0;
};

struct EntrySearchInfo {
LineTable *line_table;
lldb_private::Section *a_section;
Entry *a_entry;
};


typedef std::vector<lldb_private::Section *>
section_collection;
typedef std::vector<Entry>
entry_collection;

CompileUnit
*m_comp_unit;
entry_collection
m_entries;


class LineSequenceImpl : public LineSequence {
public:
LineSequenceImpl() = default;

~LineSequenceImpl() override = default;

void Clear() override;

entry_collection
m_entries;
};

bool ConvertEntryAtIndexToLineEntry(uint32_t idx, LineEntry &line_entry);

private:
LineTable(const LineTable &) = delete;
const LineTable &operator=(const LineTable &) = delete;

template <typename T>
uint32_t FindLineEntryIndexByFileIndexImpl(
uint32_t start_idx, T file_idx,
const SourceLocationSpec &src_location_spec, LineEntry *line_entry_ptr,
std::function<bool(T, uint16_t)> file_idx_matcher) {
const size_t count = m_entries.size();
size_t best_match = UINT32_MAX;

if (!line_entry_ptr)
return best_match;

const uint32_t line = src_location_spec.GetLine().getValueOr(0);
const uint16_t column =
src_location_spec.GetColumn().getValueOr(LLDB_INVALID_COLUMN_NUMBER);
const bool exact_match = src_location_spec.GetExactMatch();

for (size_t idx = start_idx; idx < count; ++idx) {


if (m_entries[idx].is_terminal_entry)
continue;

if (!file_idx_matcher(file_idx, m_entries[idx].file_idx))
continue;






if (column == LLDB_INVALID_COLUMN_NUMBER) {
if (m_entries[idx].line < line) {
continue;
} else if (m_entries[idx].line == line) {
ConvertEntryAtIndexToLineEntry(idx, *line_entry_ptr);
return idx;
} else if (!exact_match) {
if (best_match == UINT32_MAX ||
m_entries[idx].line < m_entries[best_match].line)
best_match = idx;
}
} else {
if (m_entries[idx].line < line) {
continue;
} else if (m_entries[idx].line == line &&
m_entries[idx].column == column) {
ConvertEntryAtIndexToLineEntry(idx, *line_entry_ptr);
return idx;
} else if (!exact_match) {
if (best_match == UINT32_MAX)
best_match = idx;
else if (m_entries[idx].line < m_entries[best_match].line)
best_match = idx;
else if (m_entries[idx].line == m_entries[best_match].line)
if (m_entries[idx].column &&
m_entries[idx].column < m_entries[best_match].column)
best_match = idx;
}
}
}

if (best_match != UINT32_MAX) {
if (line_entry_ptr)
ConvertEntryAtIndexToLineEntry(best_match, *line_entry_ptr);
return best_match;
}
return UINT32_MAX;
}
};

}

#endif
