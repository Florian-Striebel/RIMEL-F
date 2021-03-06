











#if !defined(LLVM_CLANG_AST_INTERP_INTERPSTACK_H)
#define LLVM_CLANG_AST_INTERP_INTERPSTACK_H

#include <memory>

namespace clang {
namespace interp {


class InterpStack final {
public:
InterpStack() {}


~InterpStack();


template <typename T, typename... Tys> void push(Tys &&... Args) {
new (grow(aligned_size<T>())) T(std::forward<Tys>(Args)...);
}


template <typename T> T pop() {
auto *Ptr = &peek<T>();
auto Value = std::move(*Ptr);
Ptr->~T();
shrink(aligned_size<T>());
return Value;
}


template <typename T> void discard() {
auto *Ptr = &peek<T>();
Ptr->~T();
shrink(aligned_size<T>());
}


template <typename T> T &peek() {
return *reinterpret_cast<T *>(peek(aligned_size<T>()));
}


void *top() { return Chunk ? peek(0) : nullptr; }


size_t size() const { return StackSize; }


void clear();

private:


template <typename T> constexpr size_t aligned_size() const {
constexpr size_t PtrAlign = alignof(void *);
return ((sizeof(T) + PtrAlign - 1) / PtrAlign) * PtrAlign;
}


void *grow(size_t Size);

void *peek(size_t Size);

void shrink(size_t Size);


static constexpr size_t ChunkSize = 1024 * 1024;







struct StackChunk {
StackChunk *Next;
StackChunk *Prev;
char *End;

StackChunk(StackChunk *Prev = nullptr)
: Next(nullptr), Prev(Prev), End(reinterpret_cast<char *>(this + 1)) {}


size_t size() { return End - start(); }


char *start() { return reinterpret_cast<char *>(this + 1); }
};
static_assert(sizeof(StackChunk) < ChunkSize, "Invalid chunk size");


StackChunk *Chunk = nullptr;

size_t StackSize = 0;
};

}
}

#endif
