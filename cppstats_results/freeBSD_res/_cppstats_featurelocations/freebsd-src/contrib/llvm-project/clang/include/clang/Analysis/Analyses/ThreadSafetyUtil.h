











#if !defined(LLVM_CLANG_ANALYSIS_ANALYSES_THREADSAFETYUTIL_H)
#define LLVM_CLANG_ANALYSIS_ANALYSES_THREADSAFETYUTIL_H

#include "clang/AST/Decl.h"
#include "clang/Basic/LLVM.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/iterator_range.h"
#include "llvm/Support/Allocator.h"
#include <cassert>
#include <cstddef>
#include <cstring>
#include <iterator>
#include <ostream>
#include <string>
#include <vector>

namespace clang {

class Expr;

namespace threadSafety {
namespace til {



class MemRegionRef {
private:
union AlignmentType {
double d;
void *p;
long double dd;
long long ii;
};

public:
MemRegionRef() = default;
MemRegionRef(llvm::BumpPtrAllocator *A) : Allocator(A) {}

void *allocate(size_t Sz) {
return Allocator->Allocate(Sz, alignof(AlignmentType));
}

template <typename T> T *allocateT() { return Allocator->Allocate<T>(); }

template <typename T> T *allocateT(size_t NumElems) {
return Allocator->Allocate<T>(NumElems);
}

private:
llvm::BumpPtrAllocator *Allocator = nullptr;
};

}
}

}

inline void *operator new(size_t Sz,
clang::threadSafety::til::MemRegionRef &R) {
return R.allocate(Sz);
}

namespace clang {
namespace threadSafety {

std::string getSourceLiteralString(const Expr *CE);

namespace til {



template <class T> class SimpleArray {
public:
SimpleArray() = default;
SimpleArray(T *Dat, size_t Cp, size_t Sz = 0)
: Data(Dat), Size(Sz), Capacity(Cp) {}
SimpleArray(MemRegionRef A, size_t Cp)
: Data(Cp == 0 ? nullptr : A.allocateT<T>(Cp)), Capacity(Cp) {}
SimpleArray(const SimpleArray<T> &A) = delete;

SimpleArray(SimpleArray<T> &&A)
: Data(A.Data), Size(A.Size), Capacity(A.Capacity) {
A.Data = nullptr;
A.Size = 0;
A.Capacity = 0;
}

SimpleArray &operator=(SimpleArray &&RHS) {
if (this != &RHS) {
Data = RHS.Data;
Size = RHS.Size;
Capacity = RHS.Capacity;

RHS.Data = nullptr;
RHS.Size = RHS.Capacity = 0;
}
return *this;
}


void reserve(size_t Ncp, MemRegionRef A) {
if (Ncp <= Capacity)
return;
T *Odata = Data;
Data = A.allocateT<T>(Ncp);
Capacity = Ncp;
memcpy(Data, Odata, sizeof(T) * Size);
}


void reserveCheck(size_t N, MemRegionRef A) {
if (Capacity == 0)
reserve(u_max(InitialCapacity, N), A);
else if (Size + N < Capacity)
reserve(u_max(Size + N, Capacity * 2), A);
}

using iterator = T *;
using const_iterator = const T *;
using reverse_iterator = std::reverse_iterator<iterator>;
using const_reverse_iterator = std::reverse_iterator<const_iterator>;

size_t size() const { return Size; }
size_t capacity() const { return Capacity; }

T &operator[](unsigned i) {
assert(i < Size && "Array index out of bounds.");
return Data[i];
}

const T &operator[](unsigned i) const {
assert(i < Size && "Array index out of bounds.");
return Data[i];
}

T &back() {
assert(Size && "No elements in the array.");
return Data[Size - 1];
}

const T &back() const {
assert(Size && "No elements in the array.");
return Data[Size - 1];
}

iterator begin() { return Data; }
iterator end() { return Data + Size; }

const_iterator begin() const { return Data; }
const_iterator end() const { return Data + Size; }

const_iterator cbegin() const { return Data; }
const_iterator cend() const { return Data + Size; }

reverse_iterator rbegin() { return reverse_iterator(end()); }
reverse_iterator rend() { return reverse_iterator(begin()); }

const_reverse_iterator rbegin() const {
return const_reverse_iterator(end());
}

const_reverse_iterator rend() const {
return const_reverse_iterator(begin());
}

void push_back(const T &Elem) {
assert(Size < Capacity);
Data[Size++] = Elem;
}


void drop(unsigned n = 0) {
assert(Size > n);
Size -= n;
}

void setValues(unsigned Sz, const T& C) {
assert(Sz <= Capacity);
Size = Sz;
for (unsigned i = 0; i < Sz; ++i) {
Data[i] = C;
}
}

template <class Iter> unsigned append(Iter I, Iter E) {
size_t Osz = Size;
size_t J = Osz;
for (; J < Capacity && I != E; ++J, ++I)
Data[J] = *I;
Size = J;
return J - Osz;
}

llvm::iterator_range<reverse_iterator> reverse() {
return llvm::make_range(rbegin(), rend());
}

llvm::iterator_range<const_reverse_iterator> reverse() const {
return llvm::make_range(rbegin(), rend());
}

private:


size_t u_max(size_t i, size_t j) { return (i < j) ? j : i; }

static const size_t InitialCapacity = 4;

T *Data = nullptr;
size_t Size = 0;
size_t Capacity = 0;
};

}







template<typename T>
class CopyOnWriteVector {
class VectorData {
public:
unsigned NumRefs = 1;
std::vector<T> Vect;

VectorData() = default;
VectorData(const VectorData &VD) : Vect(VD.Vect) {}
};

public:
CopyOnWriteVector() = default;
CopyOnWriteVector(CopyOnWriteVector &&V) : Data(V.Data) { V.Data = nullptr; }

CopyOnWriteVector &operator=(CopyOnWriteVector &&V) {
destroy();
Data = V.Data;
V.Data = nullptr;
return *this;
}


CopyOnWriteVector(const CopyOnWriteVector &) = delete;
CopyOnWriteVector &operator=(const CopyOnWriteVector &) = delete;

~CopyOnWriteVector() { destroy(); }


bool valid() const { return Data; }


bool writable() const { return Data && Data->NumRefs == 1; }


void init() {
if (!Data) {
Data = new VectorData();
}
}


void destroy() {
if (!Data)
return;
if (Data->NumRefs <= 1)
delete Data;
else
--Data->NumRefs;
Data = nullptr;
}


void makeWritable() {
if (!Data) {
Data = new VectorData();
return;
}
if (Data->NumRefs == 1)
return;
--Data->NumRefs;
Data = new VectorData(*Data);
}


CopyOnWriteVector clone() { return CopyOnWriteVector(Data); }

using const_iterator = typename std::vector<T>::const_iterator;

const std::vector<T> &elements() const { return Data->Vect; }

const_iterator begin() const { return elements().cbegin(); }
const_iterator end() const { return elements().cend(); }

const T& operator[](unsigned i) const { return elements()[i]; }

unsigned size() const { return Data ? elements().size() : 0; }


bool sameAs(const CopyOnWriteVector &V) const { return Data == V.Data; }


void clear() {
assert(writable() && "Vector is not writable!");
Data->Vect.clear();
}


void push_back(const T &Elem) {
assert(writable() && "Vector is not writable!");
Data->Vect.push_back(Elem);
}



T& elem(unsigned i) {
assert(writable() && "Vector is not writable!");
return Data->Vect[i];
}


void downsize(unsigned i) {
assert(writable() && "Vector is not writable!");
Data->Vect.erase(Data->Vect.begin() + i, Data->Vect.end());
}

private:
CopyOnWriteVector(VectorData *D) : Data(D) {
if (!Data)
return;
++Data->NumRefs;
}

VectorData *Data = nullptr;
};

inline std::ostream& operator<<(std::ostream& ss, const StringRef str) {
return ss.write(str.data(), str.size());
}

}
}

#endif
