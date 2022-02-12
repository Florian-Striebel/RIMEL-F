







#if !defined(LLDB_API_SBSYMBOL_H)
#define LLDB_API_SBSYMBOL_H

#include "lldb/API/SBAddress.h"
#include "lldb/API/SBDefines.h"
#include "lldb/API/SBInstructionList.h"
#include "lldb/API/SBTarget.h"

namespace lldb {

class LLDB_API SBSymbol {
public:
SBSymbol();

~SBSymbol();

SBSymbol(const lldb::SBSymbol &rhs);

const lldb::SBSymbol &operator=(const lldb::SBSymbol &rhs);

explicit operator bool() const;

bool IsValid() const;

const char *GetName() const;

const char *GetDisplayName() const;

const char *GetMangledName() const;

lldb::SBInstructionList GetInstructions(lldb::SBTarget target);

lldb::SBInstructionList GetInstructions(lldb::SBTarget target,
const char *flavor_string);

SBAddress GetStartAddress();

SBAddress GetEndAddress();

uint32_t GetPrologueByteSize();

SymbolType GetType();

bool operator==(const lldb::SBSymbol &rhs) const;

bool operator!=(const lldb::SBSymbol &rhs) const;

bool GetDescription(lldb::SBStream &description);



bool IsExternal();



bool IsSynthetic();

protected:
lldb_private::Symbol *get();

void reset(lldb_private::Symbol *);

private:
friend class SBAddress;
friend class SBFrame;
friend class SBModule;
friend class SBSymbolContext;

SBSymbol(lldb_private::Symbol *lldb_object_ptr);

void SetSymbol(lldb_private::Symbol *lldb_object_ptr);

lldb_private::Symbol *m_opaque_ptr = nullptr;
};

}

#endif
