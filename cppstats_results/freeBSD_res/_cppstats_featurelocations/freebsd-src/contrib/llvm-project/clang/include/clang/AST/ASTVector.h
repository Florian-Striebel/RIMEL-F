















#if !defined(LLVM_CLANG_AST_ASTVECTOR_H)
#define LLVM_CLANG_AST_ASTVECTOR_H

#include "clang/AST/ASTContextAllocate.h"
#include "llvm/ADT/PointerIntPair.h"
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstring>
#include <iterator>
#include <memory>
#include <type_traits>
#include <utility>

namespace clang {

class ASTContext;

template<typename T>
class ASTVector {
private:
T *Begin = nullptr;
T *End = nullptr;
llvm::PointerIntPair<T *, 1, bool> Capacity;

void setEnd(T *P) { this->End = P; }

protected:


bool getTag() const { return Capacity.getInt(); }
void setTag(bool B) { Capacity.setInt(B); }

public:

ASTVector() : Capacity(nullptr, false) {}

ASTVector(ASTVector &&O) : Begin(O.Begin), End(O.End), Capacity(O.Capacity) {
O.Begin = O.End = nullptr;
O.Capacity.setPointer(nullptr);
O.Capacity.setInt(false);
}

ASTVector(const ASTContext &C, unsigned N) : Capacity(nullptr, false) {
reserve(C, N);
}

ASTVector &operator=(ASTVector &&RHS) {
ASTVector O(std::move(RHS));

using std::swap;

swap(Begin, O.Begin);
swap(End, O.End);
swap(Capacity, O.Capacity);
return *this;
}

~ASTVector() {
if (std::is_class<T>::value) {

destroy_range(Begin, End);
}
}

using size_type = size_t;
using difference_type = ptrdiff_t;
using value_type = T;
using iterator = T *;
using const_iterator = const T *;

using const_reverse_iterator = std::reverse_iterator<const_iterator>;
using reverse_iterator = std::reverse_iterator<iterator>;

using reference = T &;
using const_reference = const T &;
using pointer = T *;
using const_pointer = const T *;


iterator begin() { return Begin; }
const_iterator begin() const { return Begin; }
iterator end() { return End; }
const_iterator end() const { return End; }


reverse_iterator rbegin() { return reverse_iterator(end()); }
const_reverse_iterator rbegin() const{ return const_reverse_iterator(end()); }
reverse_iterator rend() { return reverse_iterator(begin()); }
const_reverse_iterator rend() const { return const_reverse_iterator(begin());}

bool empty() const { return Begin == End; }
size_type size() const { return End-Begin; }

reference operator[](unsigned idx) {
assert(Begin + idx < End);
return Begin[idx];
}
const_reference operator[](unsigned idx) const {
assert(Begin + idx < End);
return Begin[idx];
}

reference front() {
return begin()[0];
}
const_reference front() const {
return begin()[0];
}

reference back() {
return end()[-1];
}
const_reference back() const {
return end()[-1];
}

void pop_back() {
--End;
End->~T();
}

T pop_back_val() {
T Result = back();
pop_back();
return Result;
}

void clear() {
if (std::is_class<T>::value) {
destroy_range(Begin, End);
}
End = Begin;
}


pointer data() {
return pointer(Begin);
}


const_pointer data() const {
return const_pointer(Begin);
}

void push_back(const_reference Elt, const ASTContext &C) {
if (End < this->capacity_ptr()) {
Retry:
new (End) T(Elt);
++End;
return;
}
grow(C);
goto Retry;
}

void reserve(const ASTContext &C, unsigned N) {
if (unsigned(this->capacity_ptr()-Begin) < N)
grow(C, N);
}



size_t capacity() const { return this->capacity_ptr() - Begin; }


template<typename in_iter>
void append(const ASTContext &C, in_iter in_start, in_iter in_end) {
size_type NumInputs = std::distance(in_start, in_end);

if (NumInputs == 0)
return;


if (NumInputs > size_type(this->capacity_ptr()-this->end()))
this->grow(C, this->size()+NumInputs);




std::uninitialized_copy(in_start, in_end, this->end());
this->setEnd(this->end() + NumInputs);
}


void append(const ASTContext &C, size_type NumInputs, const T &Elt) {

if (NumInputs > size_type(this->capacity_ptr()-this->end()))
this->grow(C, this->size()+NumInputs);


std::uninitialized_fill_n(this->end(), NumInputs, Elt);
this->setEnd(this->end() + NumInputs);
}



template<typename It1, typename It2>
static void uninitialized_copy(It1 I, It1 E, It2 Dest) {
std::uninitialized_copy(I, E, Dest);
}

iterator insert(const ASTContext &C, iterator I, const T &Elt) {
if (I == this->end()) {
push_back(Elt, C);
return this->end()-1;
}

if (this->End < this->capacity_ptr()) {
Retry:
new (this->end()) T(this->back());
this->setEnd(this->end()+1);

std::copy_backward(I, this->end()-1, this->end());
*I = Elt;
return I;
}
size_t EltNo = I-this->begin();
this->grow(C);
I = this->begin()+EltNo;
goto Retry;
}

iterator insert(const ASTContext &C, iterator I, size_type NumToInsert,
const T &Elt) {

size_t InsertElt = I - this->begin();

if (I == this->end()) {
append(C, NumToInsert, Elt);
return this->begin() + InsertElt;
}


reserve(C, static_cast<unsigned>(this->size() + NumToInsert));


I = this->begin()+InsertElt;





if (size_t(this->end()-I) >= NumToInsert) {
T *OldEnd = this->end();
append(C, this->end()-NumToInsert, this->end());


std::copy_backward(I, OldEnd-NumToInsert, OldEnd);

std::fill_n(I, NumToInsert, Elt);
return I;
}





T *OldEnd = this->end();
this->setEnd(this->end() + NumToInsert);
size_t NumOverwritten = OldEnd-I;
this->uninitialized_copy(I, OldEnd, this->end()-NumOverwritten);


std::fill_n(I, NumOverwritten, Elt);


std::uninitialized_fill_n(OldEnd, NumToInsert-NumOverwritten, Elt);
return I;
}

template<typename ItTy>
iterator insert(const ASTContext &C, iterator I, ItTy From, ItTy To) {

size_t InsertElt = I - this->begin();

if (I == this->end()) {
append(C, From, To);
return this->begin() + InsertElt;
}

size_t NumToInsert = std::distance(From, To);


reserve(C, static_cast<unsigned>(this->size() + NumToInsert));


I = this->begin()+InsertElt;





if (size_t(this->end()-I) >= NumToInsert) {
T *OldEnd = this->end();
append(C, this->end()-NumToInsert, this->end());


std::copy_backward(I, OldEnd-NumToInsert, OldEnd);

std::copy(From, To, I);
return I;
}





T *OldEnd = this->end();
this->setEnd(this->end() + NumToInsert);
size_t NumOverwritten = OldEnd-I;
this->uninitialized_copy(I, OldEnd, this->end()-NumOverwritten);


for (; NumOverwritten > 0; --NumOverwritten) {
*I = *From;
++I; ++From;
}


this->uninitialized_copy(From, To, OldEnd);
return I;
}

void resize(const ASTContext &C, unsigned N, const T &NV) {
if (N < this->size()) {
this->destroy_range(this->begin()+N, this->end());
this->setEnd(this->begin()+N);
} else if (N > this->size()) {
if (this->capacity() < N)
this->grow(C, N);
construct_range(this->end(), this->begin()+N, NV);
this->setEnd(this->begin()+N);
}
}

private:


void grow(const ASTContext &C, size_type MinSize = 1);

void construct_range(T *S, T *E, const T &Elt) {
for (; S != E; ++S)
new (S) T(Elt);
}

void destroy_range(T *S, T *E) {
while (S != E) {
--E;
E->~T();
}
}

protected:
const_iterator capacity_ptr() const {
return (iterator) Capacity.getPointer();
}

iterator capacity_ptr() { return (iterator)Capacity.getPointer(); }
};


template <typename T>
void ASTVector<T>::grow(const ASTContext &C, size_t MinSize) {
size_t CurCapacity = this->capacity();
size_t CurSize = size();
size_t NewCapacity = 2*CurCapacity;
if (NewCapacity < MinSize)
NewCapacity = MinSize;


T *NewElts = new (C, alignof(T)) T[NewCapacity];


if (Begin != End) {
if (std::is_class<T>::value) {
std::uninitialized_copy(Begin, End, NewElts);

destroy_range(Begin, End);
} else {

memcpy(NewElts, Begin, CurSize * sizeof(T));
}
}


Begin = NewElts;
End = NewElts+CurSize;
Capacity.setPointer(Begin+NewCapacity);
}

}

#endif
