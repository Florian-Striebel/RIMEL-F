







#if !defined(LLDB_SYMBOL_COMPILERDECL_H)
#define LLDB_SYMBOL_COMPILERDECL_H

#include "lldb/Symbol/CompilerType.h"
#include "lldb/Utility/ConstString.h"
#include "lldb/lldb-private.h"

namespace lldb_private {











class CompilerDecl {
public:

CompilerDecl() = default;





CompilerDecl(TypeSystem *type_system, void *decl)
: m_type_system(type_system), m_opaque_decl(decl) {}



explicit operator bool() const { return IsValid(); }

bool operator<(const CompilerDecl &rhs) const {
if (m_type_system == rhs.m_type_system)
return m_opaque_decl < rhs.m_opaque_decl;
return m_type_system < rhs.m_type_system;
}

bool IsValid() const {
return m_type_system != nullptr && m_opaque_decl != nullptr;
}



TypeSystem *GetTypeSystem() const { return m_type_system; }

void *GetOpaqueDecl() const { return m_opaque_decl; }

void SetDecl(TypeSystem *type_system, void *decl) {
m_type_system = type_system;
m_opaque_decl = decl;
}

void Clear() {
m_type_system = nullptr;
m_opaque_decl = nullptr;
}

ConstString GetName() const;

ConstString GetMangledName() const;

CompilerDeclContext GetDeclContext() const;


CompilerType GetFunctionReturnType() const;



size_t GetNumFunctionArguments() const;



CompilerType GetFunctionArgumentType(size_t arg_idx) const;

private:
TypeSystem *m_type_system = nullptr;
void *m_opaque_decl = nullptr;
};

bool operator==(const CompilerDecl &lhs, const CompilerDecl &rhs);
bool operator!=(const CompilerDecl &lhs, const CompilerDecl &rhs);

}

#endif
