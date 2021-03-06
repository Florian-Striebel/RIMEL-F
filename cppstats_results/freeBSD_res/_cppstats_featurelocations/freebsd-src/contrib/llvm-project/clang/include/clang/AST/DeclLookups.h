











#if !defined(LLVM_CLANG_AST_DECLLOOKUPS_H)
#define LLVM_CLANG_AST_DECLLOOKUPS_H

#include "clang/AST/ASTContext.h"
#include "clang/AST/DeclBase.h"
#include "clang/AST/DeclContextInternals.h"
#include "clang/AST/DeclarationName.h"
#include "clang/AST/ExternalASTSource.h"
#include <cstddef>
#include <iterator>

namespace clang {



class DeclContext::all_lookups_iterator {
StoredDeclsMap::iterator It, End;

public:
using value_type = lookup_result;
using reference = lookup_result;
using pointer = lookup_result;
using iterator_category = std::forward_iterator_tag;
using difference_type = std::ptrdiff_t;

all_lookups_iterator() = default;
all_lookups_iterator(StoredDeclsMap::iterator It,
StoredDeclsMap::iterator End)
: It(It), End(End) {}

DeclarationName getLookupName() const { return It->first; }

reference operator*() const { return It->second.getLookupResult(); }
pointer operator->() const { return It->second.getLookupResult(); }

all_lookups_iterator& operator++() {



do {
++It;
} while (It != End &&
It->first == DeclarationName::getUsingDirectiveName());

return *this;
}

all_lookups_iterator operator++(int) {
all_lookups_iterator tmp(*this);
++(*this);
return tmp;
}

friend bool operator==(all_lookups_iterator x, all_lookups_iterator y) {
return x.It == y.It;
}

friend bool operator!=(all_lookups_iterator x, all_lookups_iterator y) {
return x.It != y.It;
}
};

inline DeclContext::lookups_range DeclContext::lookups() const {
DeclContext *Primary = const_cast<DeclContext*>(this)->getPrimaryContext();
if (Primary->hasExternalVisibleStorage())
getParentASTContext().getExternalSource()->completeVisibleDeclsMap(Primary);
if (StoredDeclsMap *Map = Primary->buildLookup())
return lookups_range(all_lookups_iterator(Map->begin(), Map->end()),
all_lookups_iterator(Map->end(), Map->end()));



return lookups_range(all_lookups_iterator(), all_lookups_iterator());
}

inline DeclContext::lookups_range
DeclContext::noload_lookups(bool PreserveInternalState) const {
DeclContext *Primary = const_cast<DeclContext*>(this)->getPrimaryContext();
if (!PreserveInternalState)
Primary->loadLazyLocalLexicalLookups();
if (StoredDeclsMap *Map = Primary->getLookupPtr())
return lookups_range(all_lookups_iterator(Map->begin(), Map->end()),
all_lookups_iterator(Map->end(), Map->end()));



return lookups_range(all_lookups_iterator(), all_lookups_iterator());
}

}

#endif
