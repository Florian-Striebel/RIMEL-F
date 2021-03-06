











#if !defined(LLVM_CLANG_LIB_INTERPRETER_INCREMENTALPARSER_H)
#define LLVM_CLANG_LIB_INTERPRETER_INCREMENTALPARSER_H

#include "clang/Interpreter/PartialTranslationUnit.h"

#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/Error.h"

#include <list>
#include <memory>
namespace llvm {
class LLVMContext;
}

namespace clang {
class ASTConsumer;
class CompilerInstance;
class CodeGenerator;
class DeclGroupRef;
class FrontendAction;
class IncrementalAction;
class Parser;




class IncrementalParser {

std::unique_ptr<IncrementalAction> Act;


std::unique_ptr<CompilerInstance> CI;


std::unique_ptr<Parser> P;


ASTConsumer *Consumer = nullptr;


unsigned InputCount = 0;



std::list<PartialTranslationUnit> PTUs;

public:
IncrementalParser(std::unique_ptr<CompilerInstance> Instance,
llvm::LLVMContext &LLVMCtx, llvm::Error &Err);
~IncrementalParser();

const CompilerInstance *getCI() const { return CI.get(); }




llvm::Expected<PartialTranslationUnit &> Parse(llvm::StringRef Input);

private:
llvm::Expected<PartialTranslationUnit &> ParseOrWrapTopLevelDecl();
};
}

#endif
