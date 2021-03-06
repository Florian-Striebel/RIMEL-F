








#if !defined(LLDB_API_SBDECLARATION_H)
#define LLDB_API_SBDECLARATION_H

#include "lldb/API/SBDefines.h"
#include "lldb/API/SBFileSpec.h"

namespace lldb {

class LLDB_API SBDeclaration {
public:
SBDeclaration();

SBDeclaration(const lldb::SBDeclaration &rhs);

~SBDeclaration();

const lldb::SBDeclaration &operator=(const lldb::SBDeclaration &rhs);

explicit operator bool() const;

bool IsValid() const;

lldb::SBFileSpec GetFileSpec() const;

uint32_t GetLine() const;

uint32_t GetColumn() const;

void SetFileSpec(lldb::SBFileSpec filespec);

void SetLine(uint32_t line);

void SetColumn(uint32_t column);

bool operator==(const lldb::SBDeclaration &rhs) const;

bool operator!=(const lldb::SBDeclaration &rhs) const;

bool GetDescription(lldb::SBStream &description);

protected:
lldb_private::Declaration *get();

private:
friend class SBValue;

const lldb_private::Declaration *operator->() const;

lldb_private::Declaration &ref();

const lldb_private::Declaration &ref() const;

SBDeclaration(const lldb_private::Declaration *lldb_object_ptr);

void SetDeclaration(const lldb_private::Declaration &lldb_object_ref);

std::unique_ptr<lldb_private::Declaration> m_opaque_up;
};

}

#endif
