







#if !defined(LLDB_CORE_MODULECHILD_H)
#define LLDB_CORE_MODULECHILD_H

#include "lldb/lldb-forward.h"

namespace lldb_private {




class ModuleChild {
public:





ModuleChild(const lldb::ModuleSP &module_sp);


~ModuleChild();








const ModuleChild &operator=(const ModuleChild &rhs);






lldb::ModuleSP GetModule() const;






void SetModule(const lldb::ModuleSP &module_sp);

protected:

lldb::ModuleWP m_module_wp;
};

}

#endif
