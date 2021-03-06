











#if !defined(LLVM_CLANG_STATICANALYZER_CORE_PATHSENSITIVE_STOREREF_H)
#define LLVM_CLANG_STATICANALYZER_CORE_PATHSENSITIVE_STOREREF_H

#include <cassert>

namespace clang {
namespace ento {

class StoreManager;





using Store = const void *;

class StoreRef {
Store store;
StoreManager &mgr;

public:
StoreRef(Store store, StoreManager &smgr);
StoreRef(const StoreRef &sr);
StoreRef &operator=(StoreRef const &newStore);
~StoreRef();

bool operator==(const StoreRef &x) const {
assert(&mgr == &x.mgr);
return x.store == store;
}

bool operator!=(const StoreRef &x) const { return !operator==(x); }

Store getStore() const { return store; }
const StoreManager &getStoreManager() const { return mgr; }
};

}
}

#endif
