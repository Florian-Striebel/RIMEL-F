







#if !defined(LLDB_CORE_VALUEOBJECTLIST_H)
#define LLDB_CORE_VALUEOBJECTLIST_H

#include "lldb/lldb-forward.h"
#include "lldb/lldb-types.h"

#include <vector>

#include <cstddef>

namespace lldb_private {
class ValueObject;


class ValueObjectList {
public:
const ValueObjectList &operator=(const ValueObjectList &rhs);

void Append(const lldb::ValueObjectSP &val_obj_sp);

void Append(const ValueObjectList &valobj_list);

lldb::ValueObjectSP FindValueObjectByPointer(ValueObject *valobj);

size_t GetSize() const;

void Resize(size_t size);

lldb::ValueObjectSP GetValueObjectAtIndex(size_t idx);

lldb::ValueObjectSP RemoveValueObjectAtIndex(size_t idx);

void SetValueObjectAtIndex(size_t idx, const lldb::ValueObjectSP &valobj_sp);

lldb::ValueObjectSP FindValueObjectByValueName(const char *name);

lldb::ValueObjectSP FindValueObjectByUID(lldb::user_id_t uid);

void Swap(ValueObjectList &value_object_list);

void Clear() { m_value_objects.clear(); }

const std::vector<lldb::ValueObjectSP> &GetObjects() const {
return m_value_objects;
}
protected:
typedef std::vector<lldb::ValueObjectSP> collection;

collection m_value_objects;
};

}

#endif
