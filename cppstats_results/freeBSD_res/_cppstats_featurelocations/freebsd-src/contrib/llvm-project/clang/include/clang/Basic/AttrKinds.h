












#if !defined(LLVM_CLANG_BASIC_ATTRKINDS_H)
#define LLVM_CLANG_BASIC_ATTRKINDS_H

namespace clang {

namespace attr {


enum Kind {
#define ATTR(X) X,
#define ATTR_RANGE(CLASS, FIRST_NAME, LAST_NAME) First##CLASS = FIRST_NAME, Last##CLASS = LAST_NAME,


#include "clang/Basic/AttrList.inc"
};

}
}

#endif
