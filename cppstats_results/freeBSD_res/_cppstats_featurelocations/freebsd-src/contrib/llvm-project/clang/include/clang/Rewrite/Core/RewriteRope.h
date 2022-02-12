











#if !defined(LLVM_CLANG_REWRITE_CORE_REWRITEROPE_H)
#define LLVM_CLANG_REWRITE_CORE_REWRITEROPE_H

#include "llvm/ADT/IntrusiveRefCntPtr.h"
#include "llvm/ADT/StringRef.h"
#include <cassert>
#include <cstddef>
#include <iterator>
#include <utility>

namespace clang {









struct RopeRefCountString {
unsigned RefCount;
char Data[1];

void Retain() { ++RefCount; }

void Release() {
assert(RefCount > 0 && "Reference count is already zero.");
if (--RefCount == 0)
delete [] (char*)this;
}
};













struct RopePiece {
llvm::IntrusiveRefCntPtr<RopeRefCountString> StrData;
unsigned StartOffs = 0;
unsigned EndOffs = 0;

RopePiece() = default;
RopePiece(llvm::IntrusiveRefCntPtr<RopeRefCountString> Str, unsigned Start,
unsigned End)
: StrData(std::move(Str)), StartOffs(Start), EndOffs(End) {}

const char &operator[](unsigned Offset) const {
return StrData->Data[Offset+StartOffs];
}
char &operator[](unsigned Offset) {
return StrData->Data[Offset+StartOffs];
}

unsigned size() const { return EndOffs-StartOffs; }
};









class RopePieceBTreeIterator {

const void *CurNode = nullptr;



const RopePiece *CurPiece = nullptr;


unsigned CurChar = 0;

public:
using iterator_category = std::forward_iterator_tag;
using value_type = const char;
using difference_type = std::ptrdiff_t;
using pointer = value_type *;
using reference = value_type &;

RopePieceBTreeIterator() = default;
RopePieceBTreeIterator(const void *N);

char operator*() const {
return (*CurPiece)[CurChar];
}

bool operator==(const RopePieceBTreeIterator &RHS) const {
return CurPiece == RHS.CurPiece && CurChar == RHS.CurChar;
}
bool operator!=(const RopePieceBTreeIterator &RHS) const {
return !operator==(RHS);
}

RopePieceBTreeIterator& operator++() {
if (CurChar+1 < CurPiece->size())
++CurChar;
else
MoveToNextPiece();
return *this;
}

RopePieceBTreeIterator operator++(int) {
RopePieceBTreeIterator tmp = *this; ++*this; return tmp;
}

llvm::StringRef piece() const {
return llvm::StringRef(&(*CurPiece)[0], CurPiece->size());
}

void MoveToNextPiece();
};





class RopePieceBTree {
void *Root;

public:
RopePieceBTree();
RopePieceBTree(const RopePieceBTree &RHS);
RopePieceBTree &operator=(const RopePieceBTree &) = delete;
~RopePieceBTree();

using iterator = RopePieceBTreeIterator;

iterator begin() const { return iterator(Root); }
iterator end() const { return iterator(); }
unsigned size() const;
unsigned empty() const { return size() == 0; }

void clear();

void insert(unsigned Offset, const RopePiece &R);

void erase(unsigned Offset, unsigned NumBytes);
};








class RewriteRope {
RopePieceBTree Chunks;



llvm::IntrusiveRefCntPtr<RopeRefCountString> AllocBuffer;
enum { AllocChunkSize = 4080 };
unsigned AllocOffs = AllocChunkSize;

public:
RewriteRope() = default;
RewriteRope(const RewriteRope &RHS) : Chunks(RHS.Chunks) {}

using iterator = RopePieceBTree::iterator;
using const_iterator = RopePieceBTree::iterator;

iterator begin() const { return Chunks.begin(); }
iterator end() const { return Chunks.end(); }
unsigned size() const { return Chunks.size(); }

void clear() {
Chunks.clear();
}

void assign(const char *Start, const char *End) {
clear();
if (Start != End)
Chunks.insert(0, MakeRopeString(Start, End));
}

void insert(unsigned Offset, const char *Start, const char *End) {
assert(Offset <= size() && "Invalid position to insert!");
if (Start == End) return;
Chunks.insert(Offset, MakeRopeString(Start, End));
}

void erase(unsigned Offset, unsigned NumBytes) {
assert(Offset+NumBytes <= size() && "Invalid region to erase!");
if (NumBytes == 0) return;
Chunks.erase(Offset, NumBytes);
}

private:
RopePiece MakeRopeString(const char *Start, const char *End);
};

}

#endif
