







#if !defined(LLDB_SYMBOL_COMPILERDECLCONTEXT_H)
#define LLDB_SYMBOL_COMPILERDECLCONTEXT_H

#include <vector>

#include "lldb/Utility/ConstString.h"
#include "lldb/lldb-private.h"

namespace lldb_private {












class CompilerDeclContext {
public:

CompilerDeclContext() = default;








CompilerDeclContext(TypeSystem *type_system, void *decl_ctx)
: m_type_system(type_system), m_opaque_decl_ctx(decl_ctx) {}



explicit operator bool() const { return IsValid(); }

bool operator<(const CompilerDeclContext &rhs) const {
if (m_type_system == rhs.m_type_system)
return m_opaque_decl_ctx < rhs.m_opaque_decl_ctx;
return m_type_system < rhs.m_type_system;
}

bool IsValid() const {
return m_type_system != nullptr && m_opaque_decl_ctx != nullptr;
}

std::vector<CompilerDecl> FindDeclByName(ConstString name,
const bool ignore_using_decls);





















bool IsClassMethod(lldb::LanguageType *language_ptr,
bool *is_instance_method_ptr,
ConstString *language_object_name_ptr);












bool IsContainedInLookup(CompilerDeclContext other) const;



TypeSystem *GetTypeSystem() const { return m_type_system; }

void *GetOpaqueDeclContext() const { return m_opaque_decl_ctx; }

void SetDeclContext(TypeSystem *type_system, void *decl_ctx) {
m_type_system = type_system;
m_opaque_decl_ctx = decl_ctx;
}

void Clear() {
m_type_system = nullptr;
m_opaque_decl_ctx = nullptr;
}

ConstString GetName() const;

ConstString GetScopeQualifiedName() const;

private:
TypeSystem *m_type_system = nullptr;
void *m_opaque_decl_ctx = nullptr;
};

bool operator==(const CompilerDeclContext &lhs, const CompilerDeclContext &rhs);
bool operator!=(const CompilerDeclContext &lhs, const CompilerDeclContext &rhs);

}

#endif
