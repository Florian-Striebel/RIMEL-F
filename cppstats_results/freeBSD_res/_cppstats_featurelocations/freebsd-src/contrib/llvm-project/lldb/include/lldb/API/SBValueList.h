







#if !defined(LLDB_API_SBVALUELIST_H)
#define LLDB_API_SBVALUELIST_H

#include "lldb/API/SBDefines.h"

class ValueListImpl;

namespace lldb {

class LLDB_API SBValueList {
public:
SBValueList();

SBValueList(const lldb::SBValueList &rhs);

~SBValueList();

explicit operator bool() const;

bool IsValid() const;

void Clear();

void Append(const lldb::SBValue &val_obj);

void Append(const lldb::SBValueList &value_list);

uint32_t GetSize() const;

lldb::SBValue GetValueAtIndex(uint32_t idx) const;

lldb::SBValue GetFirstValueByName(const char *name) const;

lldb::SBValue FindValueObjectByUID(lldb::user_id_t uid);

const lldb::SBValueList &operator=(const lldb::SBValueList &rhs);

protected:


void *opaque_ptr();

private:
friend class SBFrame;

SBValueList(const ValueListImpl *lldb_object_ptr);

void Append(lldb::ValueObjectSP &val_obj_sp);

void CreateIfNeeded();

ValueListImpl *operator->();

ValueListImpl &operator*();

const ValueListImpl *operator->() const;

const ValueListImpl &operator*() const;

ValueListImpl &ref();

std::unique_ptr<ValueListImpl> m_opaque_up;
};

}

#endif
