











#if !defined(LLVM_CLANG_AST_TYPEVISITOR_H)
#define LLVM_CLANG_AST_TYPEVISITOR_H

#include "clang/AST/Type.h"

namespace clang {

#define DISPATCH(CLASS) return static_cast<ImplClass*>(this)-> Visit##CLASS(static_cast<const CLASS*>(T))










































template<typename ImplClass, typename RetTy=void>
class TypeVisitor {
public:


RetTy Visit(const Type *T) {

switch (T->getTypeClass()) {
#define ABSTRACT_TYPE(CLASS, PARENT)
#define TYPE(CLASS, PARENT) case Type::CLASS: DISPATCH(CLASS##Type);
#include "clang/AST/TypeNodes.inc"
}
llvm_unreachable("Unknown type class!");
}



#define TYPE(CLASS, PARENT) RetTy Visit##CLASS##Type(const CLASS##Type *T) { DISPATCH(PARENT); }


#include "clang/AST/TypeNodes.inc"



RetTy VisitType(const Type*) { return RetTy(); }
};

#undef DISPATCH

}

#endif
