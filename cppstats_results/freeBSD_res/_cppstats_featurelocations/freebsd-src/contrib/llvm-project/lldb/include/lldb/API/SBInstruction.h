







#if !defined(LLDB_API_SBINSTRUCTION_H)
#define LLDB_API_SBINSTRUCTION_H

#include "lldb/API/SBData.h"
#include "lldb/API/SBDefines.h"

#include <cstdio>




class InstructionImpl;

namespace lldb {

class LLDB_API SBInstruction {
public:
SBInstruction();

SBInstruction(const SBInstruction &rhs);

const SBInstruction &operator=(const SBInstruction &rhs);

~SBInstruction();

explicit operator bool() const;

bool IsValid();

SBAddress GetAddress();

const char *GetMnemonic(lldb::SBTarget target);

const char *GetOperands(lldb::SBTarget target);

const char *GetComment(lldb::SBTarget target);

lldb::SBData GetData(lldb::SBTarget target);

size_t GetByteSize();

bool DoesBranch();

bool HasDelaySlot();

bool CanSetBreakpoint();

void Print(FILE *out);

void Print(SBFile out);

void Print(FileSP out);

bool GetDescription(lldb::SBStream &description);

bool EmulateWithFrame(lldb::SBFrame &frame, uint32_t evaluate_options);

bool DumpEmulation(const char *triple);



bool TestEmulation(lldb::SBStream &output_stream, const char *test_file);

protected:
friend class SBInstructionList;

SBInstruction(const lldb::DisassemblerSP &disasm_sp,
const lldb::InstructionSP &inst_sp);

void SetOpaque(const lldb::DisassemblerSP &disasm_sp,
const lldb::InstructionSP &inst_sp);

lldb::InstructionSP GetOpaque();

private:
std::shared_ptr<InstructionImpl> m_opaque_sp;
};

}

#endif
