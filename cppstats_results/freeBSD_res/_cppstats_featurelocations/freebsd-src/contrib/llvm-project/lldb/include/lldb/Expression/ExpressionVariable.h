







#if !defined(LLDB_EXPRESSION_EXPRESSIONVARIABLE_H)
#define LLDB_EXPRESSION_EXPRESSIONVARIABLE_H

#include <memory>
#include <vector>

#include "llvm/ADT/DenseMap.h"

#include "lldb/Core/ValueObject.h"
#include "lldb/Utility/ConstString.h"
#include "lldb/lldb-public.h"

namespace lldb_private {

class ExpressionVariable
: public std::enable_shared_from_this<ExpressionVariable> {
public:

enum LLVMCastKind { eKindClang, eKindSwift, eKindGo, kNumKinds };

LLVMCastKind getKind() const { return m_kind; }

ExpressionVariable(LLVMCastKind kind) : m_flags(0), m_kind(kind) {}

virtual ~ExpressionVariable();

llvm::Optional<uint64_t> GetByteSize() { return m_frozen_sp->GetByteSize(); }

ConstString GetName() { return m_frozen_sp->GetName(); }

lldb::ValueObjectSP GetValueObject() { return m_frozen_sp; }

uint8_t *GetValueBytes();

void ValueUpdated() { m_frozen_sp->ValueUpdated(); }

RegisterInfo *GetRegisterInfo() {
return m_frozen_sp->GetValue().GetRegisterInfo();
}

void SetRegisterInfo(const RegisterInfo *reg_info) {
return m_frozen_sp->GetValue().SetContext(
Value::ContextType::RegisterInfo, const_cast<RegisterInfo *>(reg_info));
}

CompilerType GetCompilerType() { return m_frozen_sp->GetCompilerType(); }

void SetCompilerType(const CompilerType &compiler_type) {
m_frozen_sp->GetValue().SetCompilerType(compiler_type);
}

void SetName(ConstString name) { m_frozen_sp->SetName(name); }








void TransferAddress(bool force = false) {
if (m_live_sp.get() == nullptr)
return;

if (m_frozen_sp.get() == nullptr)
return;

if (force || (m_frozen_sp->GetLiveAddress() == LLDB_INVALID_ADDRESS))
m_frozen_sp->SetLiveAddress(m_live_sp->GetLiveAddress());
}

enum Flags {
EVNone = 0,
EVIsLLDBAllocated = 1 << 0,


EVIsProgramReference = 1 << 1,


EVNeedsAllocation = 1 << 2,

EVIsFreezeDried = 1 << 3,


EVNeedsFreezeDry =
1 << 4,
EVKeepInTarget = 1 << 5,


EVTypeIsReference = 1 << 6,


EVBareRegister = 1 << 7

};

typedef uint16_t FlagType;

FlagType m_flags;


lldb::ValueObjectSP m_frozen_sp;
lldb::ValueObjectSP m_live_sp;
LLVMCastKind m_kind;
};






class ExpressionVariableList {
public:

size_t GetSize() { return m_variables.size(); }

lldb::ExpressionVariableSP GetVariableAtIndex(size_t index) {
lldb::ExpressionVariableSP var_sp;
if (index < m_variables.size())
var_sp = m_variables[index];
return var_sp;
}

size_t AddVariable(const lldb::ExpressionVariableSP &var_sp) {
m_variables.push_back(var_sp);
return m_variables.size() - 1;
}

lldb::ExpressionVariableSP
AddNewlyConstructedVariable(ExpressionVariable *var) {
lldb::ExpressionVariableSP var_sp(var);
m_variables.push_back(var_sp);
return m_variables.back();
}

bool ContainsVariable(const lldb::ExpressionVariableSP &var_sp) {
const size_t size = m_variables.size();
for (size_t index = 0; index < size; ++index) {
if (m_variables[index].get() == var_sp.get())
return true;
}
return false;
}









lldb::ExpressionVariableSP GetVariable(ConstString name) {
lldb::ExpressionVariableSP var_sp;
for (size_t index = 0, size = GetSize(); index < size; ++index) {
var_sp = GetVariableAtIndex(index);
if (var_sp->GetName() == name)
return var_sp;
}
var_sp.reset();
return var_sp;
}

lldb::ExpressionVariableSP GetVariable(llvm::StringRef name) {
if (name.empty())
return nullptr;

for (size_t index = 0, size = GetSize(); index < size; ++index) {
auto var_sp = GetVariableAtIndex(index);
llvm::StringRef var_name_str = var_sp->GetName().GetStringRef();
if (var_name_str == name)
return var_sp;
}
return nullptr;
}

void RemoveVariable(lldb::ExpressionVariableSP var_sp) {
for (std::vector<lldb::ExpressionVariableSP>::iterator
vi = m_variables.begin(),
ve = m_variables.end();
vi != ve; ++vi) {
if (vi->get() == var_sp.get()) {
m_variables.erase(vi);
return;
}
}
}

void Clear() { m_variables.clear(); }

private:
std::vector<lldb::ExpressionVariableSP> m_variables;
};

class PersistentExpressionState : public ExpressionVariableList {
public:

enum LLVMCastKind { eKindClang, eKindSwift, eKindGo, kNumKinds };

LLVMCastKind getKind() const { return m_kind; }

PersistentExpressionState(LLVMCastKind kind) : m_kind(kind) {}

virtual ~PersistentExpressionState();

virtual lldb::ExpressionVariableSP
CreatePersistentVariable(const lldb::ValueObjectSP &valobj_sp) = 0;

virtual lldb::ExpressionVariableSP
CreatePersistentVariable(ExecutionContextScope *exe_scope,
ConstString name, const CompilerType &type,
lldb::ByteOrder byte_order,
uint32_t addr_byte_size) = 0;


virtual ConstString GetNextPersistentVariableName(bool is_error = false) = 0;

virtual void
RemovePersistentVariable(lldb::ExpressionVariableSP variable) = 0;

virtual llvm::Optional<CompilerType>
GetCompilerTypeFromPersistentDecl(ConstString type_name) = 0;

virtual lldb::addr_t LookupSymbol(ConstString name);

void RegisterExecutionUnit(lldb::IRExecutionUnitSP &execution_unit_sp);

protected:
virtual llvm::StringRef
GetPersistentVariablePrefix(bool is_error = false) const = 0;

private:
LLVMCastKind m_kind;

typedef std::set<lldb::IRExecutionUnitSP> ExecutionUnitSet;
ExecutionUnitSet
m_execution_units;

typedef llvm::DenseMap<const char *, lldb::addr_t> SymbolMap;
SymbolMap
m_symbol_map;
};

}

#endif
