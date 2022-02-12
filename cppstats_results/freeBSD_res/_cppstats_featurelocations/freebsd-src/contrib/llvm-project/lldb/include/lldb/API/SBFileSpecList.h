








#if !defined(LLDB_API_SBFILESPECLIST_H)
#define LLDB_API_SBFILESPECLIST_H

#include "lldb/API/SBDefines.h"

namespace lldb {

class LLDB_API SBFileSpecList {
public:
SBFileSpecList();

SBFileSpecList(const lldb::SBFileSpecList &rhs);

~SBFileSpecList();

const SBFileSpecList &operator=(const lldb::SBFileSpecList &rhs);

uint32_t GetSize() const;

bool GetDescription(SBStream &description) const;

void Append(const SBFileSpec &sb_file);

bool AppendIfUnique(const SBFileSpec &sb_file);

void Clear();

uint32_t FindFileIndex(uint32_t idx, const SBFileSpec &sb_file, bool full);

const SBFileSpec GetFileSpecAtIndex(uint32_t idx) const;

private:
friend class SBTarget;

const lldb_private::FileSpecList *operator->() const;

const lldb_private::FileSpecList *get() const;

const lldb_private::FileSpecList &operator*() const;

const lldb_private::FileSpecList &ref() const;

std::unique_ptr<lldb_private::FileSpecList> m_opaque_up;
};

}

#endif
