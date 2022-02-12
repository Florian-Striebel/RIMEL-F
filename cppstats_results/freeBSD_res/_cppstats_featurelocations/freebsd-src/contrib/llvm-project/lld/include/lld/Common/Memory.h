



















#if !defined(LLD_COMMON_MEMORY_H)
#define LLD_COMMON_MEMORY_H

#include "llvm/Support/Allocator.h"
#include "llvm/Support/StringSaver.h"
#include <vector>

namespace lld {


extern llvm::BumpPtrAllocator bAlloc;
extern llvm::StringSaver saver;

void freeArena();



struct SpecificAllocBase {
SpecificAllocBase() { instances.push_back(this); }
virtual ~SpecificAllocBase() = default;
virtual void reset() = 0;
static std::vector<SpecificAllocBase *> instances;
};

template <class T> struct SpecificAlloc : public SpecificAllocBase {
void reset() override { alloc.DestroyAll(); }
llvm::SpecificBumpPtrAllocator<T> alloc;
};




template <typename T>
inline llvm::SpecificBumpPtrAllocator<T> &getSpecificAllocSingleton() {
static SpecificAlloc<T> instance;
return instance.alloc;
}



template <typename T, typename... U> T *make(U &&... args) {
return new (getSpecificAllocSingleton<T>().Allocate())
T(std::forward<U>(args)...);
}

}

#endif
