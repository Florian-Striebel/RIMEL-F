







#if !defined(LLDB_SYMBOL_ARMUNWINDINFO_H)
#define LLDB_SYMBOL_ARMUNWINDINFO_H

#include "lldb/Symbol/ObjectFile.h"
#include "lldb/Utility/DataExtractor.h"
#include "lldb/Utility/RangeMap.h"
#include "lldb/lldb-private.h"
#include <vector>











namespace lldb_private {

class ArmUnwindInfo {
public:
ArmUnwindInfo(ObjectFile &objfile, lldb::SectionSP &arm_exidx,
lldb::SectionSP &arm_extab);

~ArmUnwindInfo();

bool GetUnwindPlan(Target &target, const Address &addr,
UnwindPlan &unwind_plan);

private:
struct ArmExidxEntry {
ArmExidxEntry(uint32_t f, lldb::addr_t a, uint32_t d);

bool operator<(const ArmExidxEntry &other) const;

uint32_t file_address;
lldb::addr_t address;
uint32_t data;
};

const uint8_t *GetExceptionHandlingTableEntry(const Address &addr);

uint8_t GetByteAtOffset(const uint32_t *data, uint16_t offset) const;

uint64_t GetULEB128(const uint32_t *data, uint16_t &offset,
uint16_t max_offset) const;

const lldb::ByteOrder m_byte_order;
lldb::SectionSP m_arm_exidx_sp;
lldb::SectionSP m_arm_extab_sp;
DataExtractor m_arm_exidx_data;
DataExtractor m_arm_extab_data;
std::vector<ArmExidxEntry> m_exidx_entries;
};

}

#endif
