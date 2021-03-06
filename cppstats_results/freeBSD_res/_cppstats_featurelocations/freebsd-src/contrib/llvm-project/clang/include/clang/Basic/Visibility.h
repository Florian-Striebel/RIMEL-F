












#if !defined(LLVM_CLANG_BASIC_VISIBILITY_H)
#define LLVM_CLANG_BASIC_VISIBILITY_H

#include "clang/Basic/Linkage.h"
#include <cassert>
#include <cstdint>

namespace clang {











enum Visibility {


HiddenVisibility,




ProtectedVisibility,



DefaultVisibility
};

inline Visibility minVisibility(Visibility L, Visibility R) {
return L < R ? L : R;
}

class LinkageInfo {
uint8_t linkage_ : 3;
uint8_t visibility_ : 2;
uint8_t explicit_ : 1;

void setVisibility(Visibility V, bool E) { visibility_ = V; explicit_ = E; }
public:
LinkageInfo() : linkage_(ExternalLinkage), visibility_(DefaultVisibility),
explicit_(false) {}
LinkageInfo(Linkage L, Visibility V, bool E)
: linkage_(L), visibility_(V), explicit_(E) {
assert(getLinkage() == L && getVisibility() == V &&
isVisibilityExplicit() == E && "Enum truncated!");
}

static LinkageInfo external() {
return LinkageInfo();
}
static LinkageInfo internal() {
return LinkageInfo(InternalLinkage, DefaultVisibility, false);
}
static LinkageInfo uniqueExternal() {
return LinkageInfo(UniqueExternalLinkage, DefaultVisibility, false);
}
static LinkageInfo none() {
return LinkageInfo(NoLinkage, DefaultVisibility, false);
}
static LinkageInfo visible_none() {
return LinkageInfo(VisibleNoLinkage, DefaultVisibility, false);
}

Linkage getLinkage() const { return (Linkage)linkage_; }
Visibility getVisibility() const { return (Visibility)visibility_; }
bool isVisibilityExplicit() const { return explicit_; }

void setLinkage(Linkage L) { linkage_ = L; }

void mergeLinkage(Linkage L) {
setLinkage(minLinkage(getLinkage(), L));
}
void mergeLinkage(LinkageInfo other) {
mergeLinkage(other.getLinkage());
}

void mergeExternalVisibility(Linkage L) {
Linkage ThisL = getLinkage();
if (!isExternallyVisible(L)) {
if (ThisL == VisibleNoLinkage)
ThisL = NoLinkage;
else if (ThisL == ExternalLinkage)
ThisL = UniqueExternalLinkage;
}
setLinkage(ThisL);
}
void mergeExternalVisibility(LinkageInfo Other) {
mergeExternalVisibility(Other.getLinkage());
}


void mergeVisibility(Visibility newVis, bool newExplicit) {
Visibility oldVis = getVisibility();


if (oldVis < newVis)
return;



if (oldVis == newVis && !newExplicit)
return;



setVisibility(newVis, newExplicit);
}
void mergeVisibility(LinkageInfo other) {
mergeVisibility(other.getVisibility(), other.isVisibilityExplicit());
}


void merge(LinkageInfo other) {
mergeLinkage(other);
mergeVisibility(other);
}


void mergeMaybeWithVisibility(LinkageInfo other, bool withVis) {
mergeLinkage(other);
if (withVis) mergeVisibility(other);
}
};
}

#endif
