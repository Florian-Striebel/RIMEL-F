







#if !defined(LLDB_API_SBTHREADCOLLECTION_H)
#define LLDB_API_SBTHREADCOLLECTION_H

#include "lldb/API/SBDefines.h"

namespace lldb {

class LLDB_API SBThreadCollection {
public:
SBThreadCollection();

SBThreadCollection(const SBThreadCollection &rhs);

const SBThreadCollection &operator=(const SBThreadCollection &rhs);

~SBThreadCollection();

explicit operator bool() const;

bool IsValid() const;

size_t GetSize();

lldb::SBThread GetThreadAtIndex(size_t idx);

protected:

lldb_private::ThreadCollection *get() const;

lldb_private::ThreadCollection *operator->() const;

lldb::ThreadCollectionSP &operator*();

const lldb::ThreadCollectionSP &operator*() const;

SBThreadCollection(const lldb::ThreadCollectionSP &threads);

void SetOpaque(const lldb::ThreadCollectionSP &threads);

private:
friend class SBProcess;
friend class SBThread;

lldb::ThreadCollectionSP m_opaque_sp;
};

}

#endif
