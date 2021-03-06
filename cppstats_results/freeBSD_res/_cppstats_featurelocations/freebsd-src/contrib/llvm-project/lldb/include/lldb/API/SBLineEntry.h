







#if !defined(LLDB_API_SBLINEENTRY_H)
#define LLDB_API_SBLINEENTRY_H

#include "lldb/API/SBAddress.h"
#include "lldb/API/SBDefines.h"
#include "lldb/API/SBFileSpec.h"

namespace lldb {

class LLDB_API SBLineEntry {
public:
SBLineEntry();

SBLineEntry(const lldb::SBLineEntry &rhs);

~SBLineEntry();

const lldb::SBLineEntry &operator=(const lldb::SBLineEntry &rhs);

lldb::SBAddress GetStartAddress() const;

lldb::SBAddress GetEndAddress() const;

explicit operator bool() const;

bool IsValid() const;

lldb::SBFileSpec GetFileSpec() const;

uint32_t GetLine() const;

uint32_t GetColumn() const;

void SetFileSpec(lldb::SBFileSpec filespec);

void SetLine(uint32_t line);

void SetColumn(uint32_t column);

bool operator==(const lldb::SBLineEntry &rhs) const;

bool operator!=(const lldb::SBLineEntry &rhs) const;

bool GetDescription(lldb::SBStream &description);

protected:
lldb_private::LineEntry *get();

private:
friend class SBAddress;
friend class SBCompileUnit;
friend class SBFrame;
friend class SBSymbolContext;

const lldb_private::LineEntry *operator->() const;

lldb_private::LineEntry &ref();

const lldb_private::LineEntry &ref() const;

SBLineEntry(const lldb_private::LineEntry *lldb_object_ptr);

void SetLineEntry(const lldb_private::LineEntry &lldb_object_ref);

std::unique_ptr<lldb_private::LineEntry> m_opaque_up;
};

}

#endif
