







#if !defined(LLDB_SYMBOL_DECLVENDOR_H)
#define LLDB_SYMBOL_DECLVENDOR_H

#include "lldb/lldb-defines.h"

#include <vector>

namespace lldb_private {



class DeclVendor {
public:
enum DeclVendorKind {
eClangDeclVendor,
eClangModuleDeclVendor,
eAppleObjCDeclVendor,
eLastClangDeclVendor,
};

DeclVendor(DeclVendorKind kind) : m_kind(kind) {}

virtual ~DeclVendor() = default;

DeclVendorKind GetKind() const { return m_kind; }

















virtual uint32_t FindDecls(ConstString name, bool append,
uint32_t max_matches,
std::vector<CompilerDecl> &decls) = 0;












std::vector<CompilerType> FindTypes(ConstString name, uint32_t max_matches);

private:

DeclVendor(const DeclVendor &) = delete;
const DeclVendor &operator=(const DeclVendor &) = delete;

const DeclVendorKind m_kind;
};

}

#endif
