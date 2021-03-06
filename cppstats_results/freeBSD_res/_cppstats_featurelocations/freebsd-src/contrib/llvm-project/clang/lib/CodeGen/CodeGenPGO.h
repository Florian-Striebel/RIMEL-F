











#if !defined(LLVM_CLANG_LIB_CODEGEN_CODEGENPGO_H)
#define LLVM_CLANG_LIB_CODEGEN_CODEGENPGO_H

#include "CGBuilder.h"
#include "CodeGenModule.h"
#include "CodeGenTypes.h"
#include "llvm/ProfileData/InstrProfReader.h"
#include <array>
#include <memory>

namespace clang {
namespace CodeGen {


class CodeGenPGO {
private:
CodeGenModule &CGM;
std::string FuncName;
llvm::GlobalVariable *FuncNameVar;

std::array <unsigned, llvm::IPVK_Last + 1> NumValueSites;
unsigned NumRegionCounters;
uint64_t FunctionHash;
std::unique_ptr<llvm::DenseMap<const Stmt *, unsigned>> RegionCounterMap;
std::unique_ptr<llvm::DenseMap<const Stmt *, uint64_t>> StmtCountMap;
std::unique_ptr<llvm::InstrProfRecord> ProfRecord;
std::vector<uint64_t> RegionCounts;
uint64_t CurrentRegionCount;

public:
CodeGenPGO(CodeGenModule &CGModule)
: CGM(CGModule), FuncNameVar(nullptr), NumValueSites({{0}}),
NumRegionCounters(0), FunctionHash(0), CurrentRegionCount(0) {}




bool haveRegionCounts() const { return !RegionCounts.empty(); }


uint64_t getCurrentRegionCount() const { return CurrentRegionCount; }




void setCurrentRegionCount(uint64_t Count) { CurrentRegionCount = Count; }



Optional<uint64_t> getStmtCount(const Stmt *S) const {
if (!StmtCountMap)
return None;
auto I = StmtCountMap->find(S);
if (I == StmtCountMap->end())
return None;
return I->second;
}



void setCurrentStmt(const Stmt *S) {
if (auto Count = getStmtCount(S))
setCurrentRegionCount(*Count);
}





void assignRegionCounters(GlobalDecl GD, llvm::Function *Fn);


void emitEmptyCounterMapping(const Decl *D, StringRef FuncName,
llvm::GlobalValue::LinkageTypes Linkage);

void valueProfile(CGBuilderTy &Builder, uint32_t ValueKind,
llvm::Instruction *ValueSite, llvm::Value *ValuePtr);


void setValueProfilingFlag(llvm::Module &M);

private:
void setFuncName(llvm::Function *Fn);
void setFuncName(StringRef Name, llvm::GlobalValue::LinkageTypes Linkage);
void mapRegionCounters(const Decl *D);
void computeRegionCounts(const Decl *D);
void applyFunctionAttributes(llvm::IndexedInstrProfReader *PGOReader,
llvm::Function *Fn);
void loadRegionCounts(llvm::IndexedInstrProfReader *PGOReader,
bool IsInMainFile);
bool skipRegionMappingForDecl(const Decl *D);
void emitCounterRegionMapping(const Decl *D);

public:
void emitCounterIncrement(CGBuilderTy &Builder, const Stmt *S,
llvm::Value *StepV);


uint64_t getRegionCount(const Stmt *S) {
if (!RegionCounterMap)
return 0;
if (!haveRegionCounts())
return 0;
return RegionCounts[(*RegionCounterMap)[S]];
}
};

}
}

#endif
