












#if !defined(LLVM_CLANG_BASIC_LINKAGE_H)
#define LLVM_CLANG_BASIC_LINKAGE_H

#include <utility>

namespace clang {



enum Linkage : unsigned char {


NoLinkage = 0,




InternalLinkage,








UniqueExternalLinkage,



VisibleNoLinkage,




ModuleInternalLinkage,





ModuleLinkage,



ExternalLinkage
};



enum LanguageLinkage {
CLanguageLinkage,
CXXLanguageLinkage,
NoLanguageLinkage
};




enum GVALinkage {
GVA_Internal,
GVA_AvailableExternally,
GVA_DiscardableODR,
GVA_StrongExternal,
GVA_StrongODR
};

inline bool isDiscardableGVALinkage(GVALinkage L) {
return L <= GVA_DiscardableODR;
}



inline bool isUniqueGVALinkage(GVALinkage L) {
return L == GVA_Internal || L == GVA_StrongExternal;
}

inline bool isExternallyVisible(Linkage L) {
return L >= VisibleNoLinkage;
}

inline Linkage getFormalLinkage(Linkage L) {
switch (L) {
case UniqueExternalLinkage:
return ExternalLinkage;
case VisibleNoLinkage:
return NoLinkage;
case ModuleInternalLinkage:
return InternalLinkage;
default:
return L;
}
}

inline bool isExternalFormalLinkage(Linkage L) {
return getFormalLinkage(L) == ExternalLinkage;
}









inline Linkage minLinkage(Linkage L1, Linkage L2) {
if (L2 == VisibleNoLinkage)
std::swap(L1, L2);
if (L1 == VisibleNoLinkage) {
if (L2 == InternalLinkage)
return NoLinkage;
if (L2 == UniqueExternalLinkage)
return NoLinkage;
}
return L1 < L2 ? L1 : L2;
}

}

#endif
