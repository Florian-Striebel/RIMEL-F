







#if !defined(LLDB_API_SBSTRINGLIST_H)
#define LLDB_API_SBSTRINGLIST_H

#include "lldb/API/SBDefines.h"

namespace lldb {

class LLDB_API SBStringList {
public:
SBStringList();

SBStringList(const lldb::SBStringList &rhs);

const SBStringList &operator=(const SBStringList &rhs);

~SBStringList();

explicit operator bool() const;

bool IsValid() const;

void AppendString(const char *str);

void AppendList(const char **strv, int strc);

void AppendList(const lldb::SBStringList &strings);

uint32_t GetSize() const;

const char *GetStringAtIndex(size_t idx);

const char *GetStringAtIndex(size_t idx) const;

void Clear();

protected:
friend class SBCommandInterpreter;
friend class SBDebugger;
friend class SBBreakpoint;
friend class SBBreakpointLocation;
friend class SBBreakpointName;

SBStringList(const lldb_private::StringList *lldb_strings);

void AppendList(const lldb_private::StringList &strings);

const lldb_private::StringList *operator->() const;

const lldb_private::StringList &operator*() const;

private:
std::unique_ptr<lldb_private::StringList> m_opaque_up;
};

}

#endif
