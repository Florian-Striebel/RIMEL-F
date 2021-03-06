







#if !defined(LLDB_CORE_DISASSEMBLER_H)
#define LLDB_CORE_DISASSEMBLER_H

#include "lldb/Core/Address.h"
#include "lldb/Core/EmulateInstruction.h"
#include "lldb/Core/FormatEntity.h"
#include "lldb/Core/Opcode.h"
#include "lldb/Core/PluginInterface.h"
#include "lldb/Interpreter/OptionValue.h"
#include "lldb/Symbol/LineEntry.h"
#include "lldb/Target/ExecutionContext.h"
#include "lldb/Utility/ArchSpec.h"
#include "lldb/Utility/ConstString.h"
#include "lldb/Utility/FileSpec.h"
#include "lldb/lldb-defines.h"
#include "lldb/lldb-forward.h"
#include "lldb/lldb-private-enumerations.h"
#include "lldb/lldb-types.h"

#include "llvm/ADT/StringRef.h"

#include <functional>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include <cstddef>
#include <cstdint>
#include <cstdio>

namespace llvm {
template <typename T> class SmallVectorImpl;
}

namespace lldb_private {
class AddressRange;
class DataExtractor;
class Debugger;
class Disassembler;
class Module;
class StackFrame;
class Stream;
class SymbolContext;
class SymbolContextList;
class Target;
struct RegisterInfo;

class Instruction {
public:
Instruction(const Address &address,
AddressClass addr_class = AddressClass::eInvalid);

virtual ~Instruction();

const Address &GetAddress() const { return m_address; }

const char *GetMnemonic(const ExecutionContext *exe_ctx) {
CalculateMnemonicOperandsAndCommentIfNeeded(exe_ctx);
return m_opcode_name.c_str();
}

const char *GetOperands(const ExecutionContext *exe_ctx) {
CalculateMnemonicOperandsAndCommentIfNeeded(exe_ctx);
return m_mnemonics.c_str();
}

const char *GetComment(const ExecutionContext *exe_ctx) {
CalculateMnemonicOperandsAndCommentIfNeeded(exe_ctx);
return m_comment.c_str();
}

virtual void
CalculateMnemonicOperandsAndComment(const ExecutionContext *exe_ctx) = 0;

AddressClass GetAddressClass();

void SetAddress(const Address &addr) {

m_address_class = AddressClass::eInvalid;
m_address = addr;
}


















































virtual void Dump(Stream *s, uint32_t max_opcode_byte_size, bool show_address,
bool show_bytes, const ExecutionContext *exe_ctx,
const SymbolContext *sym_ctx,
const SymbolContext *prev_sym_ctx,
const FormatEntity::Entry *disassembly_addr_format,
size_t max_address_text_size);

virtual bool DoesBranch() = 0;

virtual bool HasDelaySlot();

bool CanSetBreakpoint ();

virtual size_t Decode(const Disassembler &disassembler,
const DataExtractor &data,
lldb::offset_t data_offset) = 0;

virtual void SetDescription(llvm::StringRef) {
}

lldb::OptionValueSP ReadArray(FILE *in_file, Stream *out_stream,
OptionValue::Type data_type);

lldb::OptionValueSP ReadDictionary(FILE *in_file, Stream *out_stream);

bool DumpEmulation(const ArchSpec &arch);

virtual bool TestEmulation(Stream *stream, const char *test_file_name);

bool Emulate(const ArchSpec &arch, uint32_t evaluate_options, void *baton,
EmulateInstruction::ReadMemoryCallback read_mem_callback,
EmulateInstruction::WriteMemoryCallback write_mem_calback,
EmulateInstruction::ReadRegisterCallback read_reg_callback,
EmulateInstruction::WriteRegisterCallback write_reg_callback);

const Opcode &GetOpcode() const { return m_opcode; }

uint32_t GetData(DataExtractor &data);

struct Operand {
enum class Type {
Invalid = 0,
Register,
Immediate,
Dereference,
Sum,
Product
} m_type = Type::Invalid;
std::vector<Operand> m_children;
lldb::addr_t m_immediate = 0;
ConstString m_register;
bool m_negative = false;
bool m_clobbered = false;

bool IsValid() { return m_type != Type::Invalid; }

static Operand BuildRegister(ConstString &r);
static Operand BuildImmediate(lldb::addr_t imm, bool neg);
static Operand BuildImmediate(int64_t imm);
static Operand BuildDereference(const Operand &ref);
static Operand BuildSum(const Operand &lhs, const Operand &rhs);
static Operand BuildProduct(const Operand &lhs, const Operand &rhs);
};

virtual bool ParseOperands(llvm::SmallVectorImpl<Operand> &operands) {
return false;
}

virtual bool IsCall() { return false; }

protected:
Address m_address;







private:
AddressClass m_address_class;

protected:
Opcode m_opcode;
std::string m_opcode_name;
std::string m_mnemonics;
std::string m_comment;
bool m_calculated_strings;

void
CalculateMnemonicOperandsAndCommentIfNeeded(const ExecutionContext *exe_ctx) {
if (!m_calculated_strings) {
m_calculated_strings = true;
CalculateMnemonicOperandsAndComment(exe_ctx);
}
}
};

namespace OperandMatchers {
std::function<bool(const Instruction::Operand &)>
MatchBinaryOp(std::function<bool(const Instruction::Operand &)> base,
std::function<bool(const Instruction::Operand &)> left,
std::function<bool(const Instruction::Operand &)> right);

std::function<bool(const Instruction::Operand &)>
MatchUnaryOp(std::function<bool(const Instruction::Operand &)> base,
std::function<bool(const Instruction::Operand &)> child);

std::function<bool(const Instruction::Operand &)>
MatchRegOp(const RegisterInfo &info);

std::function<bool(const Instruction::Operand &)> FetchRegOp(ConstString &reg);

std::function<bool(const Instruction::Operand &)> MatchImmOp(int64_t imm);

std::function<bool(const Instruction::Operand &)> FetchImmOp(int64_t &imm);

std::function<bool(const Instruction::Operand &)>
MatchOpType(Instruction::Operand::Type type);
}

class InstructionList {
public:
InstructionList();
~InstructionList();

size_t GetSize() const;

uint32_t GetMaxOpcocdeByteSize() const;

lldb::InstructionSP GetInstructionAtIndex(size_t idx) const;






lldb::InstructionSP GetInstructionAtAddress(const Address &addr);

























uint32_t GetIndexOfNextBranchInstruction(uint32_t start,
bool ignore_calls,
bool *found_calls) const;

uint32_t GetIndexOfInstructionAtLoadAddress(lldb::addr_t load_addr,
Target &target);

uint32_t GetIndexOfInstructionAtAddress(const Address &addr);

void Clear();

void Append(lldb::InstructionSP &inst_sp);

void Dump(Stream *s, bool show_address, bool show_bytes,
const ExecutionContext *exe_ctx);

private:
typedef std::vector<lldb::InstructionSP> collection;
typedef collection::iterator iterator;
typedef collection::const_iterator const_iterator;

collection m_instructions;
};

class PseudoInstruction : public Instruction {
public:
PseudoInstruction();

~PseudoInstruction() override;

bool DoesBranch() override;

bool HasDelaySlot() override;

void CalculateMnemonicOperandsAndComment(
const ExecutionContext *exe_ctx) override {



}

size_t Decode(const Disassembler &disassembler, const DataExtractor &data,
lldb::offset_t data_offset) override;

void SetOpcode(size_t opcode_size, void *opcode_data);

void SetDescription(llvm::StringRef description) override;

protected:
std::string m_description;

PseudoInstruction(const PseudoInstruction &) = delete;
const PseudoInstruction &operator=(const PseudoInstruction &) = delete;
};

class Disassembler : public std::enable_shared_from_this<Disassembler>,
public PluginInterface {
public:
enum {
eOptionNone = 0u,
eOptionShowBytes = (1u << 0),
eOptionRawOuput = (1u << 1),
eOptionMarkPCSourceLine = (1u << 2),

eOptionMarkPCAddress =
(1u << 3)
};

enum HexImmediateStyle {
eHexStyleC,
eHexStyleAsm,
};






static lldb::DisassemblerSP
FindPlugin(const ArchSpec &arch, const char *flavor, const char *plugin_name);


static lldb::DisassemblerSP FindPluginForTarget(const Target &target,
const ArchSpec &arch,
const char *flavor,
const char *plugin_name);

struct Limit {
enum { Bytes, Instructions } kind;
lldb::addr_t value;
};

static lldb::DisassemblerSP DisassembleRange(const ArchSpec &arch,
const char *plugin_name,
const char *flavor,
Target &target,
const AddressRange &disasm_range,
bool force_live_memory = false);

static lldb::DisassemblerSP
DisassembleBytes(const ArchSpec &arch, const char *plugin_name,
const char *flavor, const Address &start, const void *bytes,
size_t length, uint32_t max_num_instructions,
bool data_from_file);

static bool Disassemble(Debugger &debugger, const ArchSpec &arch,
const char *plugin_name, const char *flavor,
const ExecutionContext &exe_ctx, const Address &start,
Limit limit, bool mixed_source_and_assembly,
uint32_t num_mixed_context_lines, uint32_t options,
Stream &strm);

static bool Disassemble(Debugger &debugger, const ArchSpec &arch,
StackFrame &frame, Stream &strm);


Disassembler(const ArchSpec &arch, const char *flavor);
~Disassembler() override;

void PrintInstructions(Debugger &debugger, const ArchSpec &arch,
const ExecutionContext &exe_ctx,
bool mixed_source_and_assembly,
uint32_t num_mixed_context_lines, uint32_t options,
Stream &strm);

size_t ParseInstructions(Target &target, Address address, Limit limit,
Stream *error_strm_ptr,
bool force_live_memory = false);

virtual size_t DecodeInstructions(const Address &base_addr,
const DataExtractor &data,
lldb::offset_t data_offset,
size_t num_instructions, bool append,
bool data_from_file) = 0;

InstructionList &GetInstructionList();

const InstructionList &GetInstructionList() const;

const ArchSpec &GetArchitecture() const { return m_arch; }

const char *GetFlavor() const { return m_flavor.c_str(); }

virtual bool FlavorValidForArchSpec(const lldb_private::ArchSpec &arch,
const char *flavor) = 0;

protected:



struct SourceLine {
FileSpec file;
uint32_t line = LLDB_INVALID_LINE_NUMBER;
uint32_t column = 0;

SourceLine() : file() {}

bool operator==(const SourceLine &rhs) const {
return file == rhs.file && line == rhs.line && rhs.column == column;
}

bool operator!=(const SourceLine &rhs) const {
return file != rhs.file || line != rhs.line || column != rhs.column;
}

bool IsValid() const { return line != LLDB_INVALID_LINE_NUMBER; }
};

struct SourceLinesToDisplay {
std::vector<SourceLine> lines;




size_t current_source_line = -1;


bool print_source_context_end_eol = true;

SourceLinesToDisplay() : lines() {}
};



static SourceLine GetFunctionDeclLineEntry(const SymbolContext &sc);


static void AddLineToSourceLineTables(
SourceLine &line,
std::map<FileSpec, std::set<uint32_t>> &source_lines_seen);









static bool
ElideMixedSourceAndDisassemblyLine(const ExecutionContext &exe_ctx,
const SymbolContext &sc, SourceLine &line);

static bool
ElideMixedSourceAndDisassemblyLine(const ExecutionContext &exe_ctx,
const SymbolContext &sc, LineEntry &line) {
SourceLine sl;
sl.file = line.file;
sl.line = line.line;
sl.column = line.column;
return ElideMixedSourceAndDisassemblyLine(exe_ctx, sc, sl);
};


ArchSpec m_arch;
InstructionList m_instruction_list;
lldb::addr_t m_base_addr;
std::string m_flavor;

private:

Disassembler(const Disassembler &) = delete;
const Disassembler &operator=(const Disassembler &) = delete;
};

}

#endif
