












#if !defined(LLVM_CLANG_AST_DECLFRIEND_H)
#define LLVM_CLANG_AST_DECLFRIEND_H

#include "clang/AST/Decl.h"
#include "clang/AST/DeclBase.h"
#include "clang/AST/DeclCXX.h"
#include "clang/AST/DeclTemplate.h"
#include "clang/AST/ExternalASTSource.h"
#include "clang/AST/TypeLoc.h"
#include "clang/Basic/LLVM.h"
#include "clang/Basic/SourceLocation.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/None.h"
#include "llvm/ADT/PointerUnion.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/Compiler.h"
#include "llvm/Support/TrailingObjects.h"
#include <cassert>
#include <iterator>

namespace clang {

class ASTContext;
















class FriendDecl final
: public Decl,
private llvm::TrailingObjects<FriendDecl, TemplateParameterList *> {
virtual void anchor();

public:
using FriendUnion = llvm::PointerUnion<NamedDecl *, TypeSourceInfo *>;

private:
friend class CXXRecordDecl;
friend class CXXRecordDecl::friend_iterator;


FriendUnion Friend;


LazyDeclPtr NextFriend;


SourceLocation FriendLoc;




unsigned UnsupportedFriend : 1;




unsigned NumTPLists : 31;

FriendDecl(DeclContext *DC, SourceLocation L, FriendUnion Friend,
SourceLocation FriendL,
ArrayRef<TemplateParameterList *> FriendTypeTPLists)
: Decl(Decl::Friend, DC, L), Friend(Friend), FriendLoc(FriendL),
UnsupportedFriend(false), NumTPLists(FriendTypeTPLists.size()) {
for (unsigned i = 0; i < NumTPLists; ++i)
getTrailingObjects<TemplateParameterList *>()[i] = FriendTypeTPLists[i];
}

FriendDecl(EmptyShell Empty, unsigned NumFriendTypeTPLists)
: Decl(Decl::Friend, Empty), UnsupportedFriend(false),
NumTPLists(NumFriendTypeTPLists) {}

FriendDecl *getNextFriend() {
if (!NextFriend.isOffset())
return cast_or_null<FriendDecl>(NextFriend.get(nullptr));
return getNextFriendSlowCase();
}

FriendDecl *getNextFriendSlowCase();

public:
friend class ASTDeclReader;
friend class ASTDeclWriter;
friend class ASTNodeImporter;
friend TrailingObjects;

static FriendDecl *Create(ASTContext &C, DeclContext *DC,
SourceLocation L, FriendUnion Friend_,
SourceLocation FriendL,
ArrayRef<TemplateParameterList*> FriendTypeTPLists
= None);
static FriendDecl *CreateDeserialized(ASTContext &C, unsigned ID,
unsigned FriendTypeNumTPLists);





TypeSourceInfo *getFriendType() const {
return Friend.dyn_cast<TypeSourceInfo*>();
}

unsigned getFriendTypeNumTemplateParameterLists() const {
return NumTPLists;
}

TemplateParameterList *getFriendTypeTemplateParameterList(unsigned N) const {
assert(N < NumTPLists);
return getTrailingObjects<TemplateParameterList *>()[N];
}



NamedDecl *getFriendDecl() const {
return Friend.dyn_cast<NamedDecl *>();
}


SourceLocation getFriendLoc() const {
return FriendLoc;
}


SourceRange getSourceRange() const override LLVM_READONLY {
if (NamedDecl *ND = getFriendDecl()) {
if (const auto *FD = dyn_cast<FunctionDecl>(ND))
return FD->getSourceRange();
if (const auto *FTD = dyn_cast<FunctionTemplateDecl>(ND))
return FTD->getSourceRange();
if (const auto *CTD = dyn_cast<ClassTemplateDecl>(ND))
return CTD->getSourceRange();
if (const auto *DD = dyn_cast<DeclaratorDecl>(ND)) {
if (DD->getOuterLocStart() != DD->getInnerLocStart())
return DD->getSourceRange();
}
return SourceRange(getFriendLoc(), ND->getEndLoc());
}
else if (TypeSourceInfo *TInfo = getFriendType()) {
SourceLocation StartL =
(NumTPLists == 0) ? getFriendLoc()
: getTrailingObjects<TemplateParameterList *>()[0]
->getTemplateLoc();
return SourceRange(StartL, TInfo->getTypeLoc().getEndLoc());
}
else
return SourceRange(getFriendLoc(), getLocation());
}


bool isUnsupportedFriend() const {
return UnsupportedFriend;
}
void setUnsupportedFriend(bool Unsupported) {
UnsupportedFriend = Unsupported;
}


static bool classof(const Decl *D) { return classofKind(D->getKind()); }
static bool classofKind(Kind K) { return K == Decl::Friend; }
};


class CXXRecordDecl::friend_iterator {
friend class CXXRecordDecl;

FriendDecl *Ptr;

explicit friend_iterator(FriendDecl *Ptr) : Ptr(Ptr) {}

public:
friend_iterator() = default;

using value_type = FriendDecl *;
using reference = FriendDecl *;
using pointer = FriendDecl *;
using difference_type = int;
using iterator_category = std::forward_iterator_tag;

reference operator*() const { return Ptr; }

friend_iterator &operator++() {
assert(Ptr && "attempt to increment past end of friend list");
Ptr = Ptr->getNextFriend();
return *this;
}

friend_iterator operator++(int) {
friend_iterator tmp = *this;
++*this;
return tmp;
}

bool operator==(const friend_iterator &Other) const {
return Ptr == Other.Ptr;
}

bool operator!=(const friend_iterator &Other) const {
return Ptr != Other.Ptr;
}

friend_iterator &operator+=(difference_type N) {
assert(N >= 0 && "cannot rewind a CXXRecordDecl::friend_iterator");
while (N--)
++*this;
return *this;
}

friend_iterator operator+(difference_type N) const {
friend_iterator tmp = *this;
tmp += N;
return tmp;
}
};

inline CXXRecordDecl::friend_iterator CXXRecordDecl::friend_begin() const {
return friend_iterator(getFirstFriend());
}

inline CXXRecordDecl::friend_iterator CXXRecordDecl::friend_end() const {
return friend_iterator(nullptr);
}

inline CXXRecordDecl::friend_range CXXRecordDecl::friends() const {
return friend_range(friend_begin(), friend_end());
}

inline void CXXRecordDecl::pushFriendDecl(FriendDecl *FD) {
assert(!FD->NextFriend && "friend already has next friend?");
FD->NextFriend = data().FirstFriend;
data().FirstFriend = FD;
}

}

#endif
