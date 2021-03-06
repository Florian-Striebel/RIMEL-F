







#if !defined(LLDB_SYMBOL_COMPACTUNWINDINFO_H)
#define LLDB_SYMBOL_COMPACTUNWINDINFO_H

#include "lldb/Symbol/ObjectFile.h"
#include "lldb/Symbol/UnwindPlan.h"
#include "lldb/Utility/DataExtractor.h"
#include "lldb/Utility/RangeMap.h"
#include "lldb/lldb-private.h"
#include <mutex>
#include <vector>

namespace lldb_private {















class CompactUnwindInfo {
public:
CompactUnwindInfo(ObjectFile &objfile, lldb::SectionSP &section);

~CompactUnwindInfo();

bool GetUnwindPlan(Target &target, Address addr, UnwindPlan &unwind_plan);

bool IsValid(const lldb::ProcessSP &process_sp);

private:





struct UnwindIndex {
uint32_t function_offset = 0;

uint32_t second_level = 0;


uint32_t lsda_array_start = 0;

uint32_t lsda_array_end =
0;
bool sentinal_entry = false;



UnwindIndex() = default;

bool operator<(const CompactUnwindInfo::UnwindIndex &rhs) const {
return function_offset < rhs.function_offset;
}

bool operator==(const CompactUnwindInfo::UnwindIndex &rhs) const {
return function_offset == rhs.function_offset;
}
};



struct FunctionInfo {
uint32_t encoding = 0;
Address lsda_address;
Address personality_ptr_address;


uint32_t valid_range_offset_start = 0;

uint32_t valid_range_offset_end =
0;
FunctionInfo() : lsda_address(), personality_ptr_address() {}
};

struct UnwindHeader {
uint32_t version;
uint32_t common_encodings_array_offset = 0;
uint32_t common_encodings_array_count = 0;
uint32_t personality_array_offset = 0;
uint32_t personality_array_count = 0;

UnwindHeader() = default;
};

void ScanIndex(const lldb::ProcessSP &process_sp);

bool GetCompactUnwindInfoForFunction(Target &target, Address address,
FunctionInfo &unwind_info);

lldb::offset_t
BinarySearchRegularSecondPage(uint32_t entry_page_offset,
uint32_t entry_count, uint32_t function_offset,
uint32_t *entry_func_start_offset,
uint32_t *entry_func_end_offset);

uint32_t BinarySearchCompressedSecondPage(uint32_t entry_page_offset,
uint32_t entry_count,
uint32_t function_offset_to_find,
uint32_t function_offset_base,
uint32_t *entry_func_start_offset,
uint32_t *entry_func_end_offset);

uint32_t GetLSDAForFunctionOffset(uint32_t lsda_offset, uint32_t lsda_count,
uint32_t function_offset);

bool CreateUnwindPlan_x86_64(Target &target, FunctionInfo &function_info,
UnwindPlan &unwind_plan,
Address pc_or_function_start);

bool CreateUnwindPlan_i386(Target &target, FunctionInfo &function_info,
UnwindPlan &unwind_plan,
Address pc_or_function_start);

bool CreateUnwindPlan_arm64(Target &target, FunctionInfo &function_info,
UnwindPlan &unwind_plan,
Address pc_or_function_start);

bool CreateUnwindPlan_armv7(Target &target, FunctionInfo &function_info,
UnwindPlan &unwind_plan,
Address pc_or_function_start);

ObjectFile &m_objfile;
lldb::SectionSP m_section_sp;
lldb::DataBufferSP m_section_contents_if_encrypted;



std::mutex m_mutex;
std::vector<UnwindIndex> m_indexes;

LazyBool m_indexes_computed;




DataExtractor m_unwindinfo_data;
bool m_unwindinfo_data_computed;


UnwindHeader m_unwind_header;
};

}

#endif
