







#if !defined(LLDB_EXPRESSION_DWARFEXPRESSION_H)
#define LLDB_EXPRESSION_DWARFEXPRESSION_H

#include "lldb/Core/Address.h"
#include "lldb/Core/Disassembler.h"
#include "lldb/Utility/DataExtractor.h"
#include "lldb/Utility/Scalar.h"
#include "lldb/Utility/Status.h"
#include "lldb/lldb-private.h"
#include <functional>

class DWARFUnit;

namespace lldb_private {












class DWARFExpression {
public:
DWARFExpression();






DWARFExpression(lldb::ModuleSP module, const DataExtractor &data,
const DWARFUnit *dwarf_cu);


virtual ~DWARFExpression();



















void GetDescription(Stream *s, lldb::DescriptionLevel level,
lldb::addr_t location_list_base_addr, ABI *abi) const;


bool IsValid() const;


bool IsLocationList() const;
















bool LocationListContainsAddress(lldb::addr_t func_load_addr,
lldb::addr_t addr) const;




















lldb::addr_t GetLocation_DW_OP_addr(uint32_t op_addr_idx, bool &error) const;

bool Update_DW_OP_addr(lldb::addr_t file_addr);

void UpdateValue(uint64_t const_value, lldb::offset_t const_value_byte_size,
uint8_t addr_byte_size);

void SetModule(const lldb::ModuleSP &module) { m_module_wp = module; }

bool ContainsThreadLocalStorage() const;

bool LinkThreadLocalStorage(
lldb::ModuleSP new_module_sp,
std::function<lldb::addr_t(lldb::addr_t file_addr)> const
&link_address_callback);










void SetLocationListAddresses(lldb::addr_t cu_file_addr,
lldb::addr_t func_file_addr);


int GetRegisterKind();





void SetRegisterKind(lldb::RegisterKind reg_kind);




bool Evaluate(ExecutionContextScope *exe_scope, lldb::addr_t func_load_addr,
const Value *initial_value_ptr, const Value *object_address_ptr,
Value &result, Status *error_ptr) const;



bool Evaluate(ExecutionContext *exe_ctx, RegisterContext *reg_ctx,
lldb::addr_t loclist_base_load_addr,
const Value *initial_value_ptr, const Value *object_address_ptr,
Value &result, Status *error_ptr) const;







































static bool Evaluate(ExecutionContext *exe_ctx, RegisterContext *reg_ctx,
lldb::ModuleSP opcode_ctx, const DataExtractor &opcodes,
const DWARFUnit *dwarf_cu,
const lldb::RegisterKind reg_set,
const Value *initial_value_ptr,
const Value *object_address_ptr, Value &result,
Status *error_ptr);

bool GetExpressionData(DataExtractor &data) const {
data = m_data;
return data.GetByteSize() > 0;
}

bool DumpLocationForAddress(Stream *s, lldb::DescriptionLevel level,
lldb::addr_t func_load_addr, lldb::addr_t address,
ABI *abi);

bool MatchesOperand(StackFrame &frame, const Instruction::Operand &op);

llvm::Optional<DataExtractor>
GetLocationExpression(lldb::addr_t load_function_start,
lldb::addr_t addr) const;

private:














void DumpLocation(Stream *s, const DataExtractor &data,
lldb::DescriptionLevel level, ABI *abi) const;


lldb::ModuleWP m_module_wp;


DataExtractor m_data;




const DWARFUnit *m_dwarf_cu = nullptr;


lldb::RegisterKind m_reg_kind = lldb::eRegisterKindDWARF;

struct LoclistAddresses {
lldb::addr_t cu_file_addr;
lldb::addr_t func_file_addr;
};
llvm::Optional<LoclistAddresses> m_loclist_addresses;
};

}

#endif
