












#if !defined(LLVM_CLANG_LIB_SEMA_TYPELOCBUILDER_H)
#define LLVM_CLANG_LIB_SEMA_TYPELOCBUILDER_H

#include "clang/AST/ASTContext.h"
#include "clang/AST/TypeLoc.h"

namespace clang {

class TypeLocBuilder {
enum { InlineCapacity = 8 * sizeof(SourceLocation) };



char *Buffer;


size_t Capacity;


size_t Index;

#if !defined(NDEBUG)

QualType LastTy;
#endif


enum { BufferMaxAlignment = alignof(void *) };
alignas(BufferMaxAlignment) char InlineBuffer[InlineCapacity];
unsigned NumBytesAtAlign4, NumBytesAtAlign8;

public:
TypeLocBuilder()
: Buffer(InlineBuffer), Capacity(InlineCapacity), Index(InlineCapacity),
NumBytesAtAlign4(0), NumBytesAtAlign8(0) {}

~TypeLocBuilder() {
if (Buffer != InlineBuffer)
delete[] Buffer;
}


void reserve(size_t Requested) {
if (Requested > Capacity)

grow(Requested);
}



void pushFullCopy(TypeLoc L);



TypeSpecTypeLoc pushTypeSpec(QualType T) {
size_t LocalSize = TypeSpecTypeLoc::LocalDataSize;
unsigned LocalAlign = TypeSpecTypeLoc::LocalDataAlignment;
return pushImpl(T, LocalSize, LocalAlign).castAs<TypeSpecTypeLoc>();
}


void clear() {
#if !defined(NDEBUG)
LastTy = QualType();
#endif
Index = Capacity;
NumBytesAtAlign4 = NumBytesAtAlign8 = 0;
}



void TypeWasModifiedSafely(QualType T) {
#if !defined(NDEBUG)
LastTy = T;
#endif
}



template <class TyLocType> TyLocType push(QualType T) {
TyLocType Loc = TypeLoc(T, nullptr).castAs<TyLocType>();
size_t LocalSize = Loc.getLocalDataSize();
unsigned LocalAlign = Loc.getLocalDataAlignment();
return pushImpl(T, LocalSize, LocalAlign).castAs<TyLocType>();
}


TypeSourceInfo *getTypeSourceInfo(ASTContext& Context, QualType T) {
#if !defined(NDEBUG)
assert(T == LastTy && "type doesn't match last type pushed!");
#endif

size_t FullDataSize = Capacity - Index;
TypeSourceInfo *DI = Context.CreateTypeSourceInfo(T, FullDataSize);
memcpy(DI->getTypeLoc().getOpaqueData(), &Buffer[Index], FullDataSize);
return DI;
}



TypeLoc getTypeLocInContext(ASTContext &Context, QualType T) {
#if !defined(NDEBUG)
assert(T == LastTy && "type doesn't match last type pushed!");
#endif

size_t FullDataSize = Capacity - Index;
void *Mem = Context.Allocate(FullDataSize);
memcpy(Mem, &Buffer[Index], FullDataSize);
return TypeLoc(T, Mem);
}

private:

TypeLoc pushImpl(QualType T, size_t LocalSize, unsigned LocalAlignment);


void grow(size_t NewCapacity);







TypeLoc getTemporaryTypeLoc(QualType T) {
#if !defined(NDEBUG)
assert(LastTy == T && "type doesn't match last type pushed!");
#endif
return TypeLoc(T, &Buffer[Index]);
}
};

}

#endif
