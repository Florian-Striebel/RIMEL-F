













#if !defined(LLVM_CLANG_BASIC_PARTIALDIAGNOSTIC_H)
#define LLVM_CLANG_BASIC_PARTIALDIAGNOSTIC_H

#include "clang/Basic/Diagnostic.h"
#include "clang/Basic/LLVM.h"
#include "clang/Basic/SourceLocation.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"
#include <cassert>
#include <cstdint>
#include <string>
#include <type_traits>
#include <utility>

namespace clang {

class DeclContext;
class IdentifierInfo;

class PartialDiagnostic : public StreamingDiagnostic {
private:





mutable unsigned DiagID = 0;
public:
struct NullDiagnostic {};



PartialDiagnostic(NullDiagnostic) {}

PartialDiagnostic(unsigned DiagID, DiagStorageAllocator &Allocator_)
: StreamingDiagnostic(Allocator_), DiagID(DiagID) {}

PartialDiagnostic(const PartialDiagnostic &Other)
: StreamingDiagnostic(), DiagID(Other.DiagID) {
Allocator = Other.Allocator;
if (Other.DiagStorage) {
DiagStorage = getStorage();
*DiagStorage = *Other.DiagStorage;
}
}

template <typename T> const PartialDiagnostic &operator<<(const T &V) const {
const StreamingDiagnostic &DB = *this;
DB << V;
return *this;
}




template <typename T, typename = typename std::enable_if<
!std::is_lvalue_reference<T>::value>::type>
const PartialDiagnostic &operator<<(T &&V) const {
const StreamingDiagnostic &DB = *this;
DB << std::move(V);
return *this;
}

PartialDiagnostic(PartialDiagnostic &&Other) : DiagID(Other.DiagID) {
Allocator = Other.Allocator;
DiagStorage = Other.DiagStorage;
Other.DiagStorage = nullptr;
}

PartialDiagnostic(const PartialDiagnostic &Other,
DiagnosticStorage *DiagStorage_)
: DiagID(Other.DiagID) {
Allocator = reinterpret_cast<DiagStorageAllocator *>(~uintptr_t(0));
DiagStorage = DiagStorage_;
if (Other.DiagStorage)
*this->DiagStorage = *Other.DiagStorage;
}

PartialDiagnostic(const Diagnostic &Other, DiagStorageAllocator &Allocator_)
: DiagID(Other.getID()) {
Allocator = &Allocator_;

for (unsigned I = 0, N = Other.getNumArgs(); I != N; ++I) {
if (Other.getArgKind(I) == DiagnosticsEngine::ak_std_string)
AddString(Other.getArgStdStr(I));
else
AddTaggedVal(Other.getRawArg(I), Other.getArgKind(I));
}


for (unsigned I = 0, N = Other.getNumRanges(); I != N; ++I)
AddSourceRange(Other.getRange(I));


for (unsigned I = 0, N = Other.getNumFixItHints(); I != N; ++I)
AddFixItHint(Other.getFixItHint(I));
}

PartialDiagnostic &operator=(const PartialDiagnostic &Other) {
DiagID = Other.DiagID;
if (Other.DiagStorage) {
if (!DiagStorage)
DiagStorage = getStorage();

*DiagStorage = *Other.DiagStorage;
} else {
freeStorage();
}

return *this;
}

PartialDiagnostic &operator=(PartialDiagnostic &&Other) {
freeStorage();

DiagID = Other.DiagID;
DiagStorage = Other.DiagStorage;
Allocator = Other.Allocator;

Other.DiagStorage = nullptr;
return *this;
}

void swap(PartialDiagnostic &PD) {
std::swap(DiagID, PD.DiagID);
std::swap(DiagStorage, PD.DiagStorage);
std::swap(Allocator, PD.Allocator);
}

unsigned getDiagID() const { return DiagID; }
void setDiagID(unsigned ID) { DiagID = ID; }

void Emit(const DiagnosticBuilder &DB) const {
if (!DiagStorage)
return;


for (unsigned i = 0, e = DiagStorage->NumDiagArgs; i != e; ++i) {
if ((DiagnosticsEngine::ArgumentKind)DiagStorage->DiagArgumentsKind[i]
== DiagnosticsEngine::ak_std_string)
DB.AddString(DiagStorage->DiagArgumentsStr[i]);
else
DB.AddTaggedVal(DiagStorage->DiagArgumentsVal[i],
(DiagnosticsEngine::ArgumentKind)DiagStorage->DiagArgumentsKind[i]);
}


for (const CharSourceRange &Range : DiagStorage->DiagRanges)
DB.AddSourceRange(Range);


for (const FixItHint &Fix : DiagStorage->FixItHints)
DB.AddFixItHint(Fix);
}

void EmitToString(DiagnosticsEngine &Diags,
SmallVectorImpl<char> &Buf) const {


DiagnosticBuilder DB(Diags.Report(getDiagID()));
Emit(DB);
Diagnostic(&Diags).FormatDiagnostic(Buf);
DB.Clear();
Diags.Clear();
}



void Reset(unsigned DiagID = 0) {
this->DiagID = DiagID;
freeStorage();
}

bool hasStorage() const { return DiagStorage != nullptr; }


StringRef getStringArg(unsigned I) {
assert(DiagStorage && "No diagnostic storage?");
assert(I < DiagStorage->NumDiagArgs && "Not enough diagnostic args");
assert(DiagStorage->DiagArgumentsKind[I]
== DiagnosticsEngine::ak_std_string && "Not a string arg");
return DiagStorage->DiagArgumentsStr[I];
}
};

inline const DiagnosticBuilder &operator<<(const DiagnosticBuilder &DB,
const PartialDiagnostic &PD) {
PD.Emit(DB);
return DB;
}



using PartialDiagnosticAt = std::pair<SourceLocation, PartialDiagnostic>;

}

#endif
