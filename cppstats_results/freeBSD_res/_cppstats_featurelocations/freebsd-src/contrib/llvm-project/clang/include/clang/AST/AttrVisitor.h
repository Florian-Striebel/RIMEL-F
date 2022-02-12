











#if !defined(LLVM_CLANG_AST_ATTRVISITOR_H)
#define LLVM_CLANG_AST_ATTRVISITOR_H

#include "clang/AST/Attr.h"

namespace clang {

namespace attrvisitor {


template <template <typename> class Ptr, typename ImplClass,
typename RetTy = void, class... ParamTys>
class Base {
public:
#define PTR(CLASS) typename Ptr<CLASS>::type
#define DISPATCH(NAME) return static_cast<ImplClass *>(this)->Visit##NAME(static_cast<PTR(NAME)>(A))


RetTy Visit(PTR(Attr) A) {
switch (A->getKind()) {

#define ATTR(NAME) case attr::NAME: DISPATCH(NAME##Attr);


#include "clang/Basic/AttrList.inc"
}
llvm_unreachable("Attr that isn't part of AttrList.inc!");
}



#define ATTR(NAME) RetTy Visit##NAME##Attr(PTR(NAME##Attr) A) { DISPATCH(Attr); }

#include "clang/Basic/AttrList.inc"

RetTy VisitAttr(PTR(Attr)) { return RetTy(); }

#undef PTR
#undef DISPATCH
};

}





template <typename ImplClass, typename RetTy = void, typename... ParamTys>
class AttrVisitor : public attrvisitor::Base<std::add_pointer, ImplClass, RetTy,
ParamTys...> {};





template <typename ImplClass, typename RetTy = void, typename... ParamTys>
class ConstAttrVisitor
: public attrvisitor::Base<llvm::make_const_ptr, ImplClass, RetTy,
ParamTys...> {};

}

#endif
