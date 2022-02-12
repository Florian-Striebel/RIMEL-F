







#if !defined(LLDB_CORE_VALUEOBJECTUPDATER_H)
#define LLDB_CORE_VALUEOBJECTUPDATER_H

#include "lldb/Core/ValueObject.h"

namespace lldb_private {






class ValueObjectUpdater {

lldb::ValueObjectSP m_root_valobj_sp;

lldb::ValueObjectSP m_user_valobj_sp;

uint32_t m_stop_id = UINT32_MAX;

public:
ValueObjectUpdater(lldb::ValueObjectSP in_valobj_sp);





lldb::ValueObjectSP GetSP();

lldb::ProcessSP GetProcessSP() const;
};

}

#endif
