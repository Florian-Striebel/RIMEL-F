







#if !defined(LLDB_API_SBBLOCK_H)
#define LLDB_API_SBBLOCK_H

#include "lldb/API/SBDefines.h"
#include "lldb/API/SBFrame.h"
#include "lldb/API/SBTarget.h"
#include "lldb/API/SBValueList.h"

namespace lldb {

class LLDB_API SBBlock {
public:
SBBlock();

SBBlock(const lldb::SBBlock &rhs);

~SBBlock();

const lldb::SBBlock &operator=(const lldb::SBBlock &rhs);

bool IsInlined() const;

explicit operator bool() const;

bool IsValid() const;

const char *GetInlinedName() const;

lldb::SBFileSpec GetInlinedCallSiteFile() const;

uint32_t GetInlinedCallSiteLine() const;

uint32_t GetInlinedCallSiteColumn() const;

lldb::SBBlock GetParent();

lldb::SBBlock GetSibling();

lldb::SBBlock GetFirstChild();

uint32_t GetNumRanges();

lldb::SBAddress GetRangeStartAddress(uint32_t idx);

lldb::SBAddress GetRangeEndAddress(uint32_t idx);

uint32_t GetRangeIndexForBlockAddress(lldb::SBAddress block_addr);

lldb::SBValueList GetVariables(lldb::SBFrame &frame, bool arguments,
bool locals, bool statics,
lldb::DynamicValueType use_dynamic);

lldb::SBValueList GetVariables(lldb::SBTarget &target, bool arguments,
bool locals, bool statics);








lldb::SBBlock GetContainingInlinedBlock();

bool GetDescription(lldb::SBStream &description);

private:
friend class SBAddress;
friend class SBFrame;
friend class SBFunction;
friend class SBSymbolContext;

lldb_private::Block *GetPtr();

void SetPtr(lldb_private::Block *lldb_object_ptr);

SBBlock(lldb_private::Block *lldb_object_ptr);

void AppendVariables(bool can_create, bool get_parent_variables,
lldb_private::VariableList *var_list);

lldb_private::Block *m_opaque_ptr = nullptr;
};

}

#endif
