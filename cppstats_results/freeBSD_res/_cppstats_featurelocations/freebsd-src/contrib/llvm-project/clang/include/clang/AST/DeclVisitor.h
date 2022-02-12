











#if !defined(LLVM_CLANG_AST_DECLVISITOR_H)
#define LLVM_CLANG_AST_DECLVISITOR_H

#include "clang/AST/Decl.h"
#include "clang/AST/DeclBase.h"
#include "clang/AST/DeclCXX.h"
#include "clang/AST/DeclFriend.h"
#include "clang/AST/DeclObjC.h"
#include "clang/AST/DeclOpenMP.h"
#include "clang/AST/DeclTemplate.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/Support/ErrorHandling.h"

namespace clang {

namespace declvisitor {

template<template <typename> class Ptr, typename ImplClass, typename RetTy=void>
class Base {
public:
#define PTR(CLASS) typename Ptr<CLASS>::type
#define DISPATCH(NAME, CLASS) return static_cast<ImplClass*>(this)->Visit##NAME(static_cast<PTR(CLASS)>(D))


RetTy Visit(PTR(Decl) D) {
switch (D->getKind()) {
#define DECL(DERIVED, BASE) case Decl::DERIVED: DISPATCH(DERIVED##Decl, DERIVED##Decl);

#define ABSTRACT_DECL(DECL)
#include "clang/AST/DeclNodes.inc"
}
llvm_unreachable("Decl that isn't part of DeclNodes.inc!");
}



#define DECL(DERIVED, BASE) RetTy Visit##DERIVED##Decl(PTR(DERIVED##Decl) D) { DISPATCH(BASE, BASE); }

#include "clang/AST/DeclNodes.inc"

RetTy VisitDecl(PTR(Decl) D) { return RetTy(); }

#undef PTR
#undef DISPATCH
};

}





template <typename ImplClass, typename RetTy = void>
class DeclVisitor
: public declvisitor::Base<std::add_pointer, ImplClass, RetTy> {};




template <typename ImplClass, typename RetTy = void>
class ConstDeclVisitor
: public declvisitor::Base<llvm::make_const_ptr, ImplClass, RetTy> {};

}

#endif
