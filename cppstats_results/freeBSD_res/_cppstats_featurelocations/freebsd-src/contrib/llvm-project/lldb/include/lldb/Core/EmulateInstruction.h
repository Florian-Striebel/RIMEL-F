







#if !defined(LLDB_CORE_EMULATEINSTRUCTION_H)
#define LLDB_CORE_EMULATEINSTRUCTION_H

#include <string>

#include "lldb/Core/Address.h"
#include "lldb/Core/Opcode.h"
#include "lldb/Core/PluginInterface.h"
#include "lldb/Utility/ArchSpec.h"
#include "lldb/lldb-defines.h"
#include "lldb/lldb-enumerations.h"
#include "lldb/lldb-private-enumerations.h"
#include "lldb/lldb-private-types.h"
#include "lldb/lldb-types.h"

#include <cstddef>
#include <cstdint>

namespace lldb_private {
class OptionValueDictionary;
class RegisterContext;
class RegisterValue;
class Stream;
class Target;
class UnwindPlan;



























































class EmulateInstruction : public PluginInterface {
public:
static EmulateInstruction *FindPlugin(const ArchSpec &arch,
InstructionType supported_inst_type,
const char *plugin_name);

enum ContextType {
eContextInvalid = 0,

eContextReadOpcode,



eContextImmediate,



eContextPushRegisterOnStack,



eContextPopRegisterOffStack,


eContextAdjustStackPointer,


eContextSetFramePointer,



eContextRestoreStackPointer,


eContextAdjustBaseRegister,


eContextAdjustPC,


eContextRegisterPlusOffset,


eContextRegisterStore,

eContextRegisterLoad,


eContextRelativeBranchImmediate,


eContextAbsoluteBranchRegister,



eContextSupervisorCall,



eContextTableBranchReadMemory,


eContextWriteRegisterRandomBits,


eContextWriteMemoryRandomBits,

eContextArithmetic,

eContextAdvancePC,

eContextReturnFromException
};

enum InfoType {
eInfoTypeRegisterPlusOffset,
eInfoTypeRegisterPlusIndirectOffset,
eInfoTypeRegisterToRegisterPlusOffset,
eInfoTypeRegisterToRegisterPlusIndirectOffset,
eInfoTypeRegisterRegisterOperands,
eInfoTypeOffset,
eInfoTypeRegister,
eInfoTypeImmediate,
eInfoTypeImmediateSigned,
eInfoTypeAddress,
eInfoTypeISAAndImmediate,
eInfoTypeISAAndImmediateSigned,
eInfoTypeISA,
eInfoTypeNoArgs
} InfoType;

struct Context {
ContextType type = eContextInvalid;
enum InfoType info_type = eInfoTypeNoArgs;
union {
struct RegisterPlusOffset {
RegisterInfo reg;
int64_t signed_offset;
} RegisterPlusOffset;

struct RegisterPlusIndirectOffset {
RegisterInfo base_reg;
RegisterInfo offset_reg;
} RegisterPlusIndirectOffset;

struct RegisterToRegisterPlusOffset {
RegisterInfo data_reg;
RegisterInfo base_reg;
int64_t offset;
} RegisterToRegisterPlusOffset;

struct RegisterToRegisterPlusIndirectOffset {
RegisterInfo base_reg;
RegisterInfo offset_reg;
RegisterInfo data_reg;
} RegisterToRegisterPlusIndirectOffset;

struct RegisterRegisterOperands {
RegisterInfo
operand1;
RegisterInfo
operand2;
} RegisterRegisterOperands;

int64_t signed_offset;


RegisterInfo reg;

uint64_t unsigned_immediate;
int64_t signed_immediate;

lldb::addr_t address;

struct ISAAndImmediate {
uint32_t isa;
uint32_t unsigned_data32;
} ISAAndImmediate;

struct ISAAndImmediateSigned {
uint32_t isa;
int32_t signed_data32;
} ISAAndImmediateSigned;

uint32_t isa;
} info;

Context() = default;

void SetRegisterPlusOffset(RegisterInfo base_reg, int64_t signed_offset) {
info_type = eInfoTypeRegisterPlusOffset;
info.RegisterPlusOffset.reg = base_reg;
info.RegisterPlusOffset.signed_offset = signed_offset;
}

void SetRegisterPlusIndirectOffset(RegisterInfo base_reg,
RegisterInfo offset_reg) {
info_type = eInfoTypeRegisterPlusIndirectOffset;
info.RegisterPlusIndirectOffset.base_reg = base_reg;
info.RegisterPlusIndirectOffset.offset_reg = offset_reg;
}

void SetRegisterToRegisterPlusOffset(RegisterInfo data_reg,
RegisterInfo base_reg,
int64_t offset) {
info_type = eInfoTypeRegisterToRegisterPlusOffset;
info.RegisterToRegisterPlusOffset.data_reg = data_reg;
info.RegisterToRegisterPlusOffset.base_reg = base_reg;
info.RegisterToRegisterPlusOffset.offset = offset;
}

void SetRegisterToRegisterPlusIndirectOffset(RegisterInfo base_reg,
RegisterInfo offset_reg,
RegisterInfo data_reg) {
info_type = eInfoTypeRegisterToRegisterPlusIndirectOffset;
info.RegisterToRegisterPlusIndirectOffset.base_reg = base_reg;
info.RegisterToRegisterPlusIndirectOffset.offset_reg = offset_reg;
info.RegisterToRegisterPlusIndirectOffset.data_reg = data_reg;
}

void SetRegisterRegisterOperands(RegisterInfo op1_reg,
RegisterInfo op2_reg) {
info_type = eInfoTypeRegisterRegisterOperands;
info.RegisterRegisterOperands.operand1 = op1_reg;
info.RegisterRegisterOperands.operand2 = op2_reg;
}

void SetOffset(int64_t signed_offset) {
info_type = eInfoTypeOffset;
info.signed_offset = signed_offset;
}

void SetRegister(RegisterInfo reg) {
info_type = eInfoTypeRegister;
info.reg = reg;
}

void SetImmediate(uint64_t immediate) {
info_type = eInfoTypeImmediate;
info.unsigned_immediate = immediate;
}

void SetImmediateSigned(int64_t signed_immediate) {
info_type = eInfoTypeImmediateSigned;
info.signed_immediate = signed_immediate;
}

void SetAddress(lldb::addr_t address) {
info_type = eInfoTypeAddress;
info.address = address;
}
void SetISAAndImmediate(uint32_t isa, uint32_t data) {
info_type = eInfoTypeISAAndImmediate;
info.ISAAndImmediate.isa = isa;
info.ISAAndImmediate.unsigned_data32 = data;
}

void SetISAAndImmediateSigned(uint32_t isa, int32_t data) {
info_type = eInfoTypeISAAndImmediateSigned;
info.ISAAndImmediateSigned.isa = isa;
info.ISAAndImmediateSigned.signed_data32 = data;
}

void SetISA(uint32_t isa) {
info_type = eInfoTypeISA;
info.isa = isa;
}

void SetNoArgs() { info_type = eInfoTypeNoArgs; }

void Dump(Stream &s, EmulateInstruction *instruction) const;
};

typedef size_t (*ReadMemoryCallback)(EmulateInstruction *instruction,
void *baton, const Context &context,
lldb::addr_t addr, void *dst,
size_t length);

typedef size_t (*WriteMemoryCallback)(EmulateInstruction *instruction,
void *baton, const Context &context,
lldb::addr_t addr, const void *dst,
size_t length);

typedef bool (*ReadRegisterCallback)(EmulateInstruction *instruction,
void *baton,
const RegisterInfo *reg_info,
RegisterValue &reg_value);

typedef bool (*WriteRegisterCallback)(EmulateInstruction *instruction,
void *baton, const Context &context,
const RegisterInfo *reg_info,
const RegisterValue &reg_value);




typedef uint32_t InstructionCondition;
static const InstructionCondition UnconditionalCondition = UINT32_MAX;

EmulateInstruction(const ArchSpec &arch);

~EmulateInstruction() override = default;


virtual bool
SupportsEmulatingInstructionsOfType(InstructionType inst_type) = 0;

virtual bool SetTargetTriple(const ArchSpec &arch) = 0;

virtual bool ReadInstruction() = 0;

virtual bool EvaluateInstruction(uint32_t evaluate_options) = 0;

virtual InstructionCondition GetInstructionCondition() {
return UnconditionalCondition;
}

virtual bool TestEmulation(Stream *out_stream, ArchSpec &arch,
OptionValueDictionary *test_data) = 0;

virtual bool GetRegisterInfo(lldb::RegisterKind reg_kind, uint32_t reg_num,
RegisterInfo &reg_info) = 0;


virtual bool SetInstruction(const Opcode &insn_opcode,
const Address &inst_addr, Target *target);

virtual bool CreateFunctionEntryUnwind(UnwindPlan &unwind_plan);

static const char *TranslateRegister(lldb::RegisterKind reg_kind,
uint32_t reg_num, std::string &reg_name);


bool ReadRegister(const RegisterInfo *reg_info, RegisterValue &reg_value);

uint64_t ReadRegisterUnsigned(const RegisterInfo *reg_info,
uint64_t fail_value, bool *success_ptr);

bool WriteRegister(const Context &context, const RegisterInfo *ref_info,
const RegisterValue &reg_value);

bool WriteRegisterUnsigned(const Context &context,
const RegisterInfo *reg_info, uint64_t reg_value);


bool ReadRegister(lldb::RegisterKind reg_kind, uint32_t reg_num,
RegisterValue &reg_value);

bool WriteRegister(const Context &context, lldb::RegisterKind reg_kind,
uint32_t reg_num, const RegisterValue &reg_value);

uint64_t ReadRegisterUnsigned(lldb::RegisterKind reg_kind, uint32_t reg_num,
uint64_t fail_value, bool *success_ptr);

bool WriteRegisterUnsigned(const Context &context,
lldb::RegisterKind reg_kind, uint32_t reg_num,
uint64_t reg_value);

size_t ReadMemory(const Context &context, lldb::addr_t addr, void *dst,
size_t dst_len);

uint64_t ReadMemoryUnsigned(const Context &context, lldb::addr_t addr,
size_t byte_size, uint64_t fail_value,
bool *success_ptr);

bool WriteMemory(const Context &context, lldb::addr_t addr, const void *src,
size_t src_len);

bool WriteMemoryUnsigned(const Context &context, lldb::addr_t addr,
uint64_t uval, size_t uval_byte_size);

uint32_t GetAddressByteSize() const { return m_arch.GetAddressByteSize(); }

lldb::ByteOrder GetByteOrder() const { return m_arch.GetByteOrder(); }

const Opcode &GetOpcode() const { return m_opcode; }

lldb::addr_t GetAddress() const { return m_addr; }

const ArchSpec &GetArchitecture() const { return m_arch; }

static size_t ReadMemoryFrame(EmulateInstruction *instruction, void *baton,
const Context &context, lldb::addr_t addr,
void *dst, size_t length);

static size_t WriteMemoryFrame(EmulateInstruction *instruction, void *baton,
const Context &context, lldb::addr_t addr,
const void *dst, size_t length);

static bool ReadRegisterFrame(EmulateInstruction *instruction, void *baton,
const RegisterInfo *reg_info,
RegisterValue &reg_value);

static bool WriteRegisterFrame(EmulateInstruction *instruction, void *baton,
const Context &context,
const RegisterInfo *reg_info,
const RegisterValue &reg_value);

static size_t ReadMemoryDefault(EmulateInstruction *instruction, void *baton,
const Context &context, lldb::addr_t addr,
void *dst, size_t length);

static size_t WriteMemoryDefault(EmulateInstruction *instruction, void *baton,
const Context &context, lldb::addr_t addr,
const void *dst, size_t length);

static bool ReadRegisterDefault(EmulateInstruction *instruction, void *baton,
const RegisterInfo *reg_info,
RegisterValue &reg_value);

static bool WriteRegisterDefault(EmulateInstruction *instruction, void *baton,
const Context &context,
const RegisterInfo *reg_info,
const RegisterValue &reg_value);

void SetBaton(void *baton);

void SetCallbacks(ReadMemoryCallback read_mem_callback,
WriteMemoryCallback write_mem_callback,
ReadRegisterCallback read_reg_callback,
WriteRegisterCallback write_reg_callback);

void SetReadMemCallback(ReadMemoryCallback read_mem_callback);

void SetWriteMemCallback(WriteMemoryCallback write_mem_callback);

void SetReadRegCallback(ReadRegisterCallback read_reg_callback);

void SetWriteRegCallback(WriteRegisterCallback write_reg_callback);

static bool GetBestRegisterKindAndNumber(const RegisterInfo *reg_info,
lldb::RegisterKind &reg_kind,
uint32_t &reg_num);

static uint32_t GetInternalRegisterNumber(RegisterContext *reg_ctx,
const RegisterInfo &reg_info);

protected:
ArchSpec m_arch;
void *m_baton = nullptr;
ReadMemoryCallback m_read_mem_callback = &ReadMemoryDefault;
WriteMemoryCallback m_write_mem_callback = &WriteMemoryDefault;
ReadRegisterCallback m_read_reg_callback = &ReadRegisterDefault;
WriteRegisterCallback m_write_reg_callback = &WriteRegisterDefault;
lldb::addr_t m_addr = LLDB_INVALID_ADDRESS;
Opcode m_opcode;

private:

EmulateInstruction(const EmulateInstruction &) = delete;
const EmulateInstruction &operator=(const EmulateInstruction &) = delete;
};

}

#endif
