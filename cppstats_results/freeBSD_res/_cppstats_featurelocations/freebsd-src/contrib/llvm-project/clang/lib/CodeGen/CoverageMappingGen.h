











#if !defined(LLVM_CLANG_LIB_CODEGEN_COVERAGEMAPPINGGEN_H)
#define LLVM_CLANG_LIB_CODEGEN_COVERAGEMAPPINGGEN_H

#include "clang/Basic/LLVM.h"
#include "clang/Basic/SourceLocation.h"
#include "clang/Lex/PPCallbacks.h"
#include "clang/Lex/Preprocessor.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/IR/GlobalValue.h"
#include "llvm/Support/raw_ostream.h"

namespace clang {

class LangOptions;
class SourceManager;
class FileEntry;
class Preprocessor;
class Decl;
class Stmt;

struct SkippedRange {
SourceRange Range;

SourceLocation PrevTokLoc;

SourceLocation NextTokLoc;

SkippedRange(SourceRange Range, SourceLocation PrevTokLoc = SourceLocation(),
SourceLocation NextTokLoc = SourceLocation())
: Range(Range), PrevTokLoc(PrevTokLoc), NextTokLoc(NextTokLoc) {}
};




class CoverageSourceInfo : public PPCallbacks,
public CommentHandler,
public EmptylineHandler {

std::vector<SkippedRange> SkippedRanges;

SourceManager &SourceMgr;

public:


SourceLocation PrevTokLoc;

CoverageSourceInfo(SourceManager &SourceMgr) : SourceMgr(SourceMgr) {}

std::vector<SkippedRange> &getSkippedRanges() { return SkippedRanges; }

void AddSkippedRange(SourceRange Range);

void SourceRangeSkipped(SourceRange Range, SourceLocation EndifLoc) override;

void HandleEmptyline(SourceRange Range) override;

bool HandleComment(Preprocessor &PP, SourceRange Range) override;

void updateNextTokLoc(SourceLocation Loc);
};

namespace CodeGen {

class CodeGenModule;



class CoverageMappingModuleGen {

struct FunctionInfo {
uint64_t NameHash;
uint64_t FuncHash;
std::string CoverageMapping;
bool IsUsed;
};

CodeGenModule &CGM;
CoverageSourceInfo &SourceInfo;
llvm::SmallDenseMap<const FileEntry *, unsigned, 8> FileEntries;
std::vector<llvm::Constant *> FunctionNames;
std::vector<FunctionInfo> FunctionRecords;
std::map<std::string, std::string> CoveragePrefixMap;

std::string getCurrentDirname();
std::string normalizeFilename(StringRef Filename);


void emitFunctionMappingRecord(const FunctionInfo &Info,
uint64_t FilenamesRef);

public:
static CoverageSourceInfo *setUpCoverageCallbacks(Preprocessor &PP);

CoverageMappingModuleGen(CodeGenModule &CGM, CoverageSourceInfo &SourceInfo);

CoverageSourceInfo &getSourceInfo() const {
return SourceInfo;
}



void addFunctionMappingRecord(llvm::GlobalVariable *FunctionName,
StringRef FunctionNameValue,
uint64_t FunctionHash,
const std::string &CoverageMapping,
bool IsUsed = true);


void emit();



unsigned getFileID(const FileEntry *File);


CodeGenModule &getCodeGenModule() { return CGM; }
};



class CoverageMappingGen {
CoverageMappingModuleGen &CVM;
SourceManager &SM;
const LangOptions &LangOpts;
llvm::DenseMap<const Stmt *, unsigned> *CounterMap;

public:
CoverageMappingGen(CoverageMappingModuleGen &CVM, SourceManager &SM,
const LangOptions &LangOpts)
: CVM(CVM), SM(SM), LangOpts(LangOpts), CounterMap(nullptr) {}

CoverageMappingGen(CoverageMappingModuleGen &CVM, SourceManager &SM,
const LangOptions &LangOpts,
llvm::DenseMap<const Stmt *, unsigned> *CounterMap)
: CVM(CVM), SM(SM), LangOpts(LangOpts), CounterMap(CounterMap) {}




void emitCounterMapping(const Decl *D, llvm::raw_ostream &OS);



void emitEmptyMapping(const Decl *D, llvm::raw_ostream &OS);
};

}
}

#endif
