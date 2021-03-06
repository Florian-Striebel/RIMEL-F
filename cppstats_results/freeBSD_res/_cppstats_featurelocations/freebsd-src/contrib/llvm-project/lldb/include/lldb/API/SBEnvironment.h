







#if !defined(LLDB_API_SBENVIRONMENT_H)
#define LLDB_API_SBENVIRONMENT_H

#include "lldb/API/SBDefines.h"

namespace lldb {

class LLDB_API SBEnvironment {
public:
SBEnvironment();

SBEnvironment(const lldb::SBEnvironment &rhs);

~SBEnvironment();

const lldb::SBEnvironment &operator=(const lldb::SBEnvironment &rhs);










const char *Get(const char *name);



size_t GetNumValues();









const char *GetNameAtIndex(size_t index);











const char *GetValueAtIndex(size_t index);







SBStringList GetEntries();







void PutEntry(const char *name_and_value);













void SetEntries(const SBStringList &entries, bool append);
















bool Set(const char *name, const char *value, bool overwrite);








bool Unset(const char *name);


void Clear();

protected:
friend class SBPlatform;
friend class SBTarget;
friend class SBLaunchInfo;

SBEnvironment(lldb_private::Environment rhs);

lldb_private::Environment &ref() const;

private:
std::unique_ptr<lldb_private::Environment> m_opaque_up;
};

}

#endif
