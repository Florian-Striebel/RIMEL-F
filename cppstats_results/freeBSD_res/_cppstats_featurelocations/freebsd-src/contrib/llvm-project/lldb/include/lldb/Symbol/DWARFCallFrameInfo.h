







#if !defined(LLDB_SYMBOL_DWARFCALLFRAMEINFO_H)
#define LLDB_SYMBOL_DWARFCALLFRAMEINFO_H

#include <map>
#include <mutex>

#include "lldb/Core/AddressRange.h"
#include "lldb/Core/dwarf.h"
#include "lldb/Symbol/ObjectFile.h"
#include "lldb/Symbol/UnwindPlan.h"
#include "lldb/Utility/Flags.h"
#include "lldb/Utility/RangeMap.h"
#include "lldb/Utility/VMRange.h"
#include "lldb/lldb-private.h"

namespace lldb_private {








class DWARFCallFrameInfo {
public:
enum Type { EH, DWARF };

DWARFCallFrameInfo(ObjectFile &objfile, lldb::SectionSP &section, Type type);

~DWARFCallFrameInfo() = default;




bool GetAddressRange(Address addr, AddressRange &range);




bool GetUnwindPlan(const Address &addr, UnwindPlan &unwind_plan);




bool GetUnwindPlan(const AddressRange &range, UnwindPlan &unwind_plan);

typedef RangeVector<lldb::addr_t, uint32_t> FunctionAddressAndSizeVector;















void
GetFunctionAddressAndSizeVector(FunctionAddressAndSizeVector &function_info);

void ForEachFDEEntries(
const std::function<bool(lldb::addr_t, uint32_t, dw_offset_t)> &callback);

private:
enum { CFI_AUG_MAX_SIZE = 8, CFI_HEADER_SIZE = 8 };
enum CFIVersion {
CFI_VERSION1 = 1,
CFI_VERSION3 = 3,
CFI_VERSION4 = 4
};

struct CIE {
dw_offset_t cie_offset;
uint8_t version;
char augmentation[CFI_AUG_MAX_SIZE];

uint8_t address_size = sizeof(uint32_t);
uint8_t segment_size = 0;

uint32_t code_align;
int32_t data_align;
uint32_t return_addr_reg_num;
dw_offset_t inst_offset;
uint32_t inst_length;
uint8_t ptr_encoding;
uint8_t lsda_addr_encoding;

lldb::addr_t personality_loc;

lldb_private::UnwindPlan::Row initial_row;

CIE(dw_offset_t offset)
: cie_offset(offset), version(-1), code_align(0), data_align(0),
return_addr_reg_num(LLDB_INVALID_REGNUM), inst_offset(0),
inst_length(0), ptr_encoding(0), lsda_addr_encoding(DW_EH_PE_omit),
personality_loc(LLDB_INVALID_ADDRESS), initial_row() {}
};

typedef std::shared_ptr<CIE> CIESP;

typedef std::map<dw_offset_t, CIESP> cie_map_t;




typedef RangeDataVector<lldb::addr_t, uint32_t, dw_offset_t> FDEEntryMap;

bool IsEHFrame() const;

llvm::Optional<FDEEntryMap::Entry>
GetFirstFDEEntryInRange(const AddressRange &range);

void GetFDEIndex();

bool FDEToUnwindPlan(uint32_t offset, Address startaddr,
UnwindPlan &unwind_plan);

const CIE *GetCIE(dw_offset_t cie_offset);

void GetCFIData();






bool HandleCommonDwarfOpcode(uint8_t primary_opcode, uint8_t extended_opcode,
int32_t data_align, lldb::offset_t &offset,
UnwindPlan::Row &row);

ObjectFile &m_objfile;
lldb::SectionSP m_section_sp;
Flags m_flags = 0;
cie_map_t m_cie_map;

DataExtractor m_cfi_data;
bool m_cfi_data_initialized = false;

FDEEntryMap m_fde_index;
bool m_fde_index_initialized = false;
std::mutex m_fde_index_mutex;

Type m_type;

CIESP
ParseCIE(const uint32_t cie_offset);

lldb::RegisterKind GetRegisterKind() const {
return m_type == EH ? lldb::eRegisterKindEHFrame : lldb::eRegisterKindDWARF;
}
};

}

#endif
