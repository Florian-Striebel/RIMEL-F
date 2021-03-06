












#if !defined(LLVM_CLANG_SERIALIZATION_CONTINUOUSRANGEMAP_H)
#define LLVM_CLANG_SERIALIZATION_CONTINUOUSRANGEMAP_H

#include "clang/Basic/LLVM.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/SmallVector.h"
#include <algorithm>
#include <cassert>
#include <utility>

namespace clang {











template <typename Int, typename V, unsigned InitialCapacity>
class ContinuousRangeMap {
public:
using value_type = std::pair<Int, V>;
using reference = value_type &;
using const_reference = const value_type &;
using pointer = value_type *;
using const_pointer = const value_type *;

private:
using Representation = SmallVector<value_type, InitialCapacity>;

Representation Rep;

struct Compare {
bool operator ()(const_reference L, Int R) const {
return L.first < R;
}
bool operator ()(Int L, const_reference R) const {
return L < R.first;
}
bool operator ()(Int L, Int R) const {
return L < R;
}
bool operator ()(const_reference L, const_reference R) const {
return L.first < R.first;
}
};

public:
void insert(const value_type &Val) {
if (!Rep.empty() && Rep.back() == Val)
return;

assert((Rep.empty() || Rep.back().first < Val.first) &&
"Must insert keys in order.");
Rep.push_back(Val);
}

void insertOrReplace(const value_type &Val) {
iterator I = llvm::lower_bound(Rep, Val, Compare());
if (I != Rep.end() && I->first == Val.first) {
I->second = Val.second;
return;
}

Rep.insert(I, Val);
}

using iterator = typename Representation::iterator;
using const_iterator = typename Representation::const_iterator;

iterator begin() { return Rep.begin(); }
iterator end() { return Rep.end(); }
const_iterator begin() const { return Rep.begin(); }
const_iterator end() const { return Rep.end(); }

iterator find(Int K) {
iterator I = llvm::upper_bound(Rep, K, Compare());


if (I == Rep.begin())
return Rep.end();
--I;
return I;
}
const_iterator find(Int K) const {
return const_cast<ContinuousRangeMap*>(this)->find(K);
}

reference back() { return Rep.back(); }
const_reference back() const { return Rep.back(); }



class Builder {
ContinuousRangeMap &Self;

public:
explicit Builder(ContinuousRangeMap &Self) : Self(Self) {}
Builder(const Builder&) = delete;
Builder &operator=(const Builder&) = delete;

~Builder() {
llvm::sort(Self.Rep, Compare());
Self.Rep.erase(
std::unique(
Self.Rep.begin(), Self.Rep.end(),
[](const_reference A, const_reference B) {


assert((A == B || A.first != B.first) &&
"ContinuousRangeMap::Builder given non-unique keys");
return A == B;
}),
Self.Rep.end());
}

void insert(const value_type &Val) {
Self.Rep.push_back(Val);
}
};

friend class Builder;
};

}

#endif
