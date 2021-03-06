











#if !defined(LLVM_CLANG_AST_ATTRITERATOR_H)
#define LLVM_CLANG_AST_ATTRITERATOR_H

#include "clang/Basic/LLVM.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/Support/Casting.h"
#include <cassert>
#include <cstddef>
#include <iterator>

namespace clang {

class ASTContext;
class Attr;


using AttrVec = SmallVector<Attr *, 4>;



template <typename SpecificAttr, typename Container = AttrVec>
class specific_attr_iterator {
using Iterator = typename Container::const_iterator;








mutable Iterator Current;

void AdvanceToNext() const {
while (!isa<SpecificAttr>(*Current))
++Current;
}

void AdvanceToNext(Iterator I) const {
while (Current != I && !isa<SpecificAttr>(*Current))
++Current;
}

public:
using value_type = SpecificAttr *;
using reference = SpecificAttr *;
using pointer = SpecificAttr *;
using iterator_category = std::forward_iterator_tag;
using difference_type = std::ptrdiff_t;

specific_attr_iterator() = default;
explicit specific_attr_iterator(Iterator i) : Current(i) {}

reference operator*() const {
AdvanceToNext();
return cast<SpecificAttr>(*Current);
}
pointer operator->() const {
AdvanceToNext();
return cast<SpecificAttr>(*Current);
}

specific_attr_iterator& operator++() {
++Current;
return *this;
}
specific_attr_iterator operator++(int) {
specific_attr_iterator Tmp(*this);
++(*this);
return Tmp;
}

friend bool operator==(specific_attr_iterator Left,
specific_attr_iterator Right) {
assert((Left.Current == nullptr) == (Right.Current == nullptr));
if (Left.Current < Right.Current)
Left.AdvanceToNext(Right.Current);
else
Right.AdvanceToNext(Left.Current);
return Left.Current == Right.Current;
}
friend bool operator!=(specific_attr_iterator Left,
specific_attr_iterator Right) {
return !(Left == Right);
}
};

template <typename SpecificAttr, typename Container>
inline specific_attr_iterator<SpecificAttr, Container>
specific_attr_begin(const Container& container) {
return specific_attr_iterator<SpecificAttr, Container>(container.begin());
}
template <typename SpecificAttr, typename Container>
inline specific_attr_iterator<SpecificAttr, Container>
specific_attr_end(const Container& container) {
return specific_attr_iterator<SpecificAttr, Container>(container.end());
}

template <typename SpecificAttr, typename Container>
inline bool hasSpecificAttr(const Container& container) {
return specific_attr_begin<SpecificAttr>(container) !=
specific_attr_end<SpecificAttr>(container);
}
template <typename SpecificAttr, typename Container>
inline SpecificAttr *getSpecificAttr(const Container& container) {
specific_attr_iterator<SpecificAttr, Container> i =
specific_attr_begin<SpecificAttr>(container);
if (i != specific_attr_end<SpecificAttr>(container))
return *i;
else
return nullptr;
}

}

#endif
