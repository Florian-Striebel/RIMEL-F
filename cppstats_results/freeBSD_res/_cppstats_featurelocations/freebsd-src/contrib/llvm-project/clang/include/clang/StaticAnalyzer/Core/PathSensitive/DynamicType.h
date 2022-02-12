













#if !defined(LLVM_CLANG_STATICANALYZER_CORE_PATHSENSITIVE_DYNAMICTYPE_H)
#define LLVM_CLANG_STATICANALYZER_CORE_PATHSENSITIVE_DYNAMICTYPE_H

#include "clang/AST/Type.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/DynamicCastInfo.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/DynamicTypeInfo.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/ProgramState.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/ProgramStateTrait.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/ProgramState_Fwd.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/SymbolManager.h"
#include "llvm/ADT/ImmutableMap.h"
#include "llvm/ADT/Optional.h"

namespace clang {
namespace ento {


DynamicTypeInfo getDynamicTypeInfo(ProgramStateRef State, const MemRegion *MR);


const DynamicTypeInfo *getRawDynamicTypeInfo(ProgramStateRef State,
const MemRegion *MR);


DynamicTypeInfo getClassObjectDynamicTypeInfo(ProgramStateRef State,
SymbolRef Sym);


const DynamicCastInfo *getDynamicCastInfo(ProgramStateRef State,
const MemRegion *MR,
QualType CastFromTy,
QualType CastToTy);


ProgramStateRef setDynamicTypeInfo(ProgramStateRef State, const MemRegion *MR,
DynamicTypeInfo NewTy);


ProgramStateRef setDynamicTypeInfo(ProgramStateRef State, const MemRegion *MR,
QualType NewTy, bool CanBeSubClassed = true);


ProgramStateRef setClassObjectDynamicTypeInfo(ProgramStateRef State,
SymbolRef Sym,
DynamicTypeInfo NewTy);


ProgramStateRef setClassObjectDynamicTypeInfo(ProgramStateRef State,
SymbolRef Sym, QualType NewTy,
bool CanBeSubClassed = true);


ProgramStateRef setDynamicTypeAndCastInfo(ProgramStateRef State,
const MemRegion *MR,
QualType CastFromTy,
QualType CastToTy,
bool IsCastSucceeds);


ProgramStateRef removeDeadTypes(ProgramStateRef State, SymbolReaper &SR);


ProgramStateRef removeDeadCasts(ProgramStateRef State, SymbolReaper &SR);


ProgramStateRef removeDeadClassObjectTypes(ProgramStateRef State,
SymbolReaper &SR);

void printDynamicTypeInfoJson(raw_ostream &Out, ProgramStateRef State,
const char *NL = "\n", unsigned int Space = 0,
bool IsDot = false);

}
}

#endif
