







#if !defined(LLDB_API_SBCOMPILEUNIT_H)
#define LLDB_API_SBCOMPILEUNIT_H

#include "lldb/API/SBDefines.h"
#include "lldb/API/SBFileSpec.h"

namespace lldb {

class LLDB_API SBCompileUnit {
public:
SBCompileUnit();

SBCompileUnit(const lldb::SBCompileUnit &rhs);

~SBCompileUnit();

const lldb::SBCompileUnit &operator=(const lldb::SBCompileUnit &rhs);

explicit operator bool() const;

bool IsValid() const;

lldb::SBFileSpec GetFileSpec() const;

uint32_t GetNumLineEntries() const;

lldb::SBLineEntry GetLineEntryAtIndex(uint32_t idx) const;

uint32_t FindLineEntryIndex(uint32_t start_idx, uint32_t line,
lldb::SBFileSpec *inline_file_spec) const;

uint32_t FindLineEntryIndex(uint32_t start_idx, uint32_t line,
lldb::SBFileSpec *inline_file_spec,
bool exact) const;

SBFileSpec GetSupportFileAtIndex(uint32_t idx) const;

uint32_t GetNumSupportFiles() const;

uint32_t FindSupportFileIndex(uint32_t start_idx, const SBFileSpec &sb_file,
bool full);














lldb::SBTypeList GetTypes(uint32_t type_mask = lldb::eTypeClassAny);

lldb::LanguageType GetLanguage();

bool operator==(const lldb::SBCompileUnit &rhs) const;

bool operator!=(const lldb::SBCompileUnit &rhs) const;

bool GetDescription(lldb::SBStream &description);

private:
friend class SBAddress;
friend class SBFrame;
friend class SBSymbolContext;
friend class SBModule;

SBCompileUnit(lldb_private::CompileUnit *lldb_object_ptr);

const lldb_private::CompileUnit *operator->() const;

const lldb_private::CompileUnit &operator*() const;

lldb_private::CompileUnit *get();

void reset(lldb_private::CompileUnit *lldb_object_ptr);

lldb_private::CompileUnit *m_opaque_ptr = nullptr;
};

}

#endif
