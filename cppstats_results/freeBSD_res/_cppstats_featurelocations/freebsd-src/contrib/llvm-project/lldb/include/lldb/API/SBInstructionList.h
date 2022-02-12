







#if !defined(LLDB_API_SBINSTRUCTIONLIST_H)
#define LLDB_API_SBINSTRUCTIONLIST_H

#include "lldb/API/SBDefines.h"

#include <cstdio>

namespace lldb {

class LLDB_API SBInstructionList {
public:
SBInstructionList();

SBInstructionList(const SBInstructionList &rhs);

const SBInstructionList &operator=(const SBInstructionList &rhs);

~SBInstructionList();

explicit operator bool() const;

bool IsValid() const;

size_t GetSize();

lldb::SBInstruction GetInstructionAtIndex(uint32_t idx);




size_t GetInstructionsCount(const SBAddress &start,
const SBAddress &end,
bool canSetBreakpoint = false);

void Clear();

void AppendInstruction(lldb::SBInstruction inst);

void Print(FILE *out);

void Print(SBFile out);

void Print(FileSP out);

bool GetDescription(lldb::SBStream &description);

bool DumpEmulationForAllInstructions(const char *triple);

protected:
friend class SBFunction;
friend class SBSymbol;
friend class SBTarget;

void SetDisassembler(const lldb::DisassemblerSP &opaque_sp);
bool GetDescription(lldb_private::Stream &description);


private:
lldb::DisassemblerSP m_opaque_sp;
};

}

#endif
