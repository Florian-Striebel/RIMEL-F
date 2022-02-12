







#if !defined(LLVM_CLANG_INDEX_INDEXINGACTION_H)
#define LLVM_CLANG_INDEX_INDEXINGACTION_H

#include "clang/AST/ASTConsumer.h"
#include "clang/Basic/LLVM.h"
#include "clang/Index/IndexingOptions.h"
#include "clang/Lex/PPCallbacks.h"
#include "clang/Lex/Preprocessor.h"
#include "llvm/ADT/ArrayRef.h"
#include <memory>

namespace clang {
class ASTContext;
class ASTConsumer;
class ASTReader;
class ASTUnit;
class Decl;
class FrontendAction;

namespace serialization {
class ModuleFile;
}

namespace index {
class IndexDataConsumer;


std::unique_ptr<ASTConsumer>
createIndexingASTConsumer(std::shared_ptr<IndexDataConsumer> DataConsumer,
const IndexingOptions &Opts,
std::shared_ptr<Preprocessor> PP);

std::unique_ptr<ASTConsumer> createIndexingASTConsumer(
std::shared_ptr<IndexDataConsumer> DataConsumer,
const IndexingOptions &Opts, std::shared_ptr<Preprocessor> PP,


std::function<bool(const Decl *)> ShouldSkipFunctionBody);


std::unique_ptr<FrontendAction>
createIndexingAction(std::shared_ptr<IndexDataConsumer> DataConsumer,
const IndexingOptions &Opts);


void indexASTUnit(ASTUnit &Unit, IndexDataConsumer &DataConsumer,
IndexingOptions Opts);


void indexTopLevelDecls(ASTContext &Ctx, Preprocessor &PP,
ArrayRef<const Decl *> Decls,
IndexDataConsumer &DataConsumer, IndexingOptions Opts);



std::unique_ptr<PPCallbacks> indexMacrosCallback(IndexDataConsumer &Consumer,
IndexingOptions Opts);


void indexModuleFile(serialization::ModuleFile &Mod, ASTReader &Reader,
IndexDataConsumer &DataConsumer, IndexingOptions Opts);

}
}

#endif
