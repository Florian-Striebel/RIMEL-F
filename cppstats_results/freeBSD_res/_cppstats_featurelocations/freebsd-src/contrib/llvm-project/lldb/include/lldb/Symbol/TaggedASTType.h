







#if !defined(LLDB_SYMBOL_TAGGEDASTTYPE_H)
#define LLDB_SYMBOL_TAGGEDASTTYPE_H

#include "lldb/Symbol/CompilerType.h"

namespace lldb_private {



template <unsigned int C> class TaggedASTType : public CompilerType {
public:
TaggedASTType(const CompilerType &compiler_type)
: CompilerType(compiler_type) {}

TaggedASTType(lldb::opaque_compiler_type_t type, TypeSystem *type_system)
: CompilerType(type_system, type) {}

TaggedASTType(const TaggedASTType<C> &tw) : CompilerType(tw) {}

TaggedASTType() : CompilerType() {}

virtual ~TaggedASTType() = default;

TaggedASTType<C> &operator=(const TaggedASTType<C> &tw) {
CompilerType::operator=(tw);
return *this;
}
};


typedef TaggedASTType<0> TypeFromParser;
typedef TaggedASTType<1> TypeFromUser;
}

#endif
