












#if !defined(LLVM_CLANG_AST_EXTERNALASTSOURCE_H)
#define LLVM_CLANG_AST_EXTERNALASTSOURCE_H

#include "clang/AST/CharUnits.h"
#include "clang/AST/DeclBase.h"
#include "clang/Basic/LLVM.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/IntrusiveRefCntPtr.h"
#include "llvm/ADT/Optional.h"
#include "llvm/ADT/PointerUnion.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/iterator.h"
#include "llvm/Support/PointerLikeTypeTraits.h"
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <utility>

namespace clang {

class ASTConsumer;
class ASTContext;
class ASTSourceDescriptor;
class CXXBaseSpecifier;
class CXXCtorInitializer;
class CXXRecordDecl;
class DeclarationName;
class FieldDecl;
class IdentifierInfo;
class NamedDecl;
class ObjCInterfaceDecl;
class RecordDecl;
class Selector;
class Stmt;
class TagDecl;








class ExternalASTSource : public RefCountedBase<ExternalASTSource> {
friend class ExternalSemaSource;



uint32_t CurrentGeneration = 0;


static char ID;

public:
ExternalASTSource() = default;
virtual ~ExternalASTSource();



class Deserializing {
ExternalASTSource *Source;

public:
explicit Deserializing(ExternalASTSource *source) : Source(source) {
assert(Source);
Source->StartedDeserializing();
}

~Deserializing() {
Source->FinishedDeserializing();
}
};




uint32_t getGeneration() const { return CurrentGeneration; }








virtual Decl *GetExternalDecl(uint32_t ID);







virtual Selector GetExternalSelector(uint32_t ID);





virtual uint32_t GetNumExternalSelectors();









virtual Stmt *GetExternalDeclStmt(uint64_t Offset);





virtual CXXCtorInitializer **GetExternalCXXCtorInitializers(uint64_t Offset);





virtual CXXBaseSpecifier *GetExternalCXXBaseSpecifiers(uint64_t Offset);


virtual void updateOutOfDateIdentifier(IdentifierInfo &II) {}








virtual bool
FindExternalVisibleDeclsByName(const DeclContext *DC, DeclarationName Name);





virtual void completeVisibleDeclsMap(const DeclContext *DC);


virtual Module *getModule(unsigned ID) { return nullptr; }


virtual llvm::Optional<ASTSourceDescriptor> getSourceDescriptor(unsigned ID);

enum ExtKind { EK_Always, EK_Never, EK_ReplyHazy };

virtual ExtKind hasExternalDefinitions(const Decl *D);








virtual void
FindExternalLexicalDecls(const DeclContext *DC,
llvm::function_ref<bool(Decl::Kind)> IsKindWeWant,
SmallVectorImpl<Decl *> &Result);



void FindExternalLexicalDecls(const DeclContext *DC,
SmallVectorImpl<Decl *> &Result) {
FindExternalLexicalDecls(DC, [](Decl::Kind) { return true; }, Result);
}




virtual void FindFileRegionDecls(FileID File, unsigned Offset,
unsigned Length,
SmallVectorImpl<Decl *> &Decls);





virtual void CompleteRedeclChain(const Decl *D);



virtual void CompleteType(TagDecl *Tag);







virtual void CompleteType(ObjCInterfaceDecl *Class);


virtual void ReadComments();






virtual void StartedDeserializing();





virtual void FinishedDeserializing();





virtual void StartTranslationUnit(ASTConsumer *Consumer);





virtual void PrintStats();




























virtual bool layoutRecordType(
const RecordDecl *Record, uint64_t &Size, uint64_t &Alignment,
llvm::DenseMap<const FieldDecl *, uint64_t> &FieldOffsets,
llvm::DenseMap<const CXXRecordDecl *, CharUnits> &BaseOffsets,
llvm::DenseMap<const CXXRecordDecl *, CharUnits> &VirtualBaseOffsets);





struct MemoryBufferSizes {
size_t malloc_bytes;
size_t mmap_bytes;

MemoryBufferSizes(size_t malloc_bytes, size_t mmap_bytes)
: malloc_bytes(malloc_bytes), mmap_bytes(mmap_bytes) {}
};



MemoryBufferSizes getMemoryBufferSizes() const {
MemoryBufferSizes sizes(0, 0);
getMemoryBufferSizes(sizes);
return sizes;
}

virtual void getMemoryBufferSizes(MemoryBufferSizes &sizes) const;



virtual bool isA(const void *ClassID) const { return ClassID == &ID; }
static bool classof(const ExternalASTSource *S) { return S->isA(&ID); }


protected:
static DeclContextLookupResult
SetExternalVisibleDeclsForName(const DeclContext *DC,
DeclarationName Name,
ArrayRef<NamedDecl*> Decls);

static DeclContextLookupResult
SetNoExternalVisibleDeclsForName(const DeclContext *DC,
DeclarationName Name);


uint32_t incrementGeneration(ASTContext &C);
};







template<typename T, typename OffsT, T* (ExternalASTSource::*Get)(OffsT Offset)>
struct LazyOffsetPtr {





mutable uint64_t Ptr = 0;

public:
LazyOffsetPtr() = default;
explicit LazyOffsetPtr(T *Ptr) : Ptr(reinterpret_cast<uint64_t>(Ptr)) {}

explicit LazyOffsetPtr(uint64_t Offset) : Ptr((Offset << 1) | 0x01) {
assert((Offset << 1 >> 1) == Offset && "Offsets must require < 63 bits");
if (Offset == 0)
Ptr = 0;
}

LazyOffsetPtr &operator=(T *Ptr) {
this->Ptr = reinterpret_cast<uint64_t>(Ptr);
return *this;
}

LazyOffsetPtr &operator=(uint64_t Offset) {
assert((Offset << 1 >> 1) == Offset && "Offsets must require < 63 bits");
if (Offset == 0)
Ptr = 0;
else
Ptr = (Offset << 1) | 0x01;

return *this;
}




explicit operator bool() const { return Ptr != 0; }




bool isValid() const { return Ptr != 0; }


bool isOffset() const { return Ptr & 0x01; }






T* get(ExternalASTSource *Source) const {
if (isOffset()) {
assert(Source &&
"Cannot deserialize a lazy pointer without an AST source");
Ptr = reinterpret_cast<uint64_t>((Source->*Get)(Ptr >> 1));
}
return reinterpret_cast<T*>(Ptr);
}
};




template<typename Owner, typename T, void (ExternalASTSource::*Update)(Owner)>
struct LazyGenerationalUpdatePtr {


struct LazyData {
ExternalASTSource *ExternalSource;
uint32_t LastGeneration = 0;
T LastValue;

LazyData(ExternalASTSource *Source, T Value)
: ExternalSource(Source), LastValue(Value) {}
};


using ValueType = llvm::PointerUnion<T, LazyData*>;
ValueType Value;

LazyGenerationalUpdatePtr(ValueType V) : Value(V) {}


static ValueType makeValue(const ASTContext &Ctx, T Value);

public:
explicit LazyGenerationalUpdatePtr(const ASTContext &Ctx, T Value = T())
: Value(makeValue(Ctx, Value)) {}



enum NotUpdatedTag { NotUpdated };
LazyGenerationalUpdatePtr(NotUpdatedTag, T Value = T())
: Value(Value) {}


void markIncomplete() {
Value.template get<LazyData *>()->LastGeneration = 0;
}


void set(T NewValue) {
if (auto *LazyVal = Value.template dyn_cast<LazyData *>()) {
LazyVal->LastValue = NewValue;
return;
}
Value = NewValue;
}


void setNotUpdated(T NewValue) { Value = NewValue; }


T get(Owner O) {
if (auto *LazyVal = Value.template dyn_cast<LazyData *>()) {
if (LazyVal->LastGeneration != LazyVal->ExternalSource->getGeneration()) {
LazyVal->LastGeneration = LazyVal->ExternalSource->getGeneration();
(LazyVal->ExternalSource->*Update)(O);
}
return LazyVal->LastValue;
}
return Value.template get<T>();
}


T getNotUpdated() const {
if (auto *LazyVal = Value.template dyn_cast<LazyData *>())
return LazyVal->LastValue;
return Value.template get<T>();
}

void *getOpaqueValue() { return Value.getOpaqueValue(); }
static LazyGenerationalUpdatePtr getFromOpaqueValue(void *Ptr) {
return LazyGenerationalUpdatePtr(ValueType::getFromOpaqueValue(Ptr));
}
};

}

namespace llvm {



template<typename Owner, typename T,
void (clang::ExternalASTSource::*Update)(Owner)>
struct PointerLikeTypeTraits<
clang::LazyGenerationalUpdatePtr<Owner, T, Update>> {
using Ptr = clang::LazyGenerationalUpdatePtr<Owner, T, Update>;

static void *getAsVoidPointer(Ptr P) { return P.getOpaqueValue(); }
static Ptr getFromVoidPointer(void *P) { return Ptr::getFromOpaqueValue(P); }

static constexpr int NumLowBitsAvailable =
PointerLikeTypeTraits<T>::NumLowBitsAvailable - 1;
};

}

namespace clang {







template<typename T, typename Source,
void (Source::*Loader)(SmallVectorImpl<T>&),
unsigned LoadedStorage = 2, unsigned LocalStorage = 4>
class LazyVector {
SmallVector<T, LoadedStorage> Loaded;
SmallVector<T, LocalStorage> Local;

public:
















class iterator
: public llvm::iterator_adaptor_base<
iterator, int, std::random_access_iterator_tag, T, int, T *, T &> {
friend class LazyVector;

LazyVector *Self;

iterator(LazyVector *Self, int Position)
: iterator::iterator_adaptor_base(Position), Self(Self) {}

bool isLoaded() const { return this->I < 0; }

public:
iterator() : iterator(nullptr, 0) {}

typename iterator::reference operator*() const {
if (isLoaded())
return Self->Loaded.end()[this->I];
return Self->Local.begin()[this->I];
}
};

iterator begin(Source *source, bool LocalOnly = false) {
if (LocalOnly)
return iterator(this, 0);

if (source)
(source->*Loader)(Loaded);
return iterator(this, -(int)Loaded.size());
}

iterator end() {
return iterator(this, Local.size());
}

void push_back(const T& LocalValue) {
Local.push_back(LocalValue);
}

void erase(iterator From, iterator To) {
if (From.isLoaded() && To.isLoaded()) {
Loaded.erase(&*From, &*To);
return;
}

if (From.isLoaded()) {
Loaded.erase(&*From, Loaded.end());
From = begin(nullptr, true);
}

Local.erase(&*From, &*To);
}
};


using LazyDeclStmtPtr =
LazyOffsetPtr<Stmt, uint64_t, &ExternalASTSource::GetExternalDeclStmt>;


using LazyDeclPtr =
LazyOffsetPtr<Decl, uint32_t, &ExternalASTSource::GetExternalDecl>;


using LazyCXXCtorInitializersPtr =
LazyOffsetPtr<CXXCtorInitializer *, uint64_t,
&ExternalASTSource::GetExternalCXXCtorInitializers>;


using LazyCXXBaseSpecifiersPtr =
LazyOffsetPtr<CXXBaseSpecifier, uint64_t,
&ExternalASTSource::GetExternalCXXBaseSpecifiers>;

}

#endif
