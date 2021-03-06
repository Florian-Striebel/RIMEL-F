












#if !defined(LLVM_CLANG_INTERPRETER_PARTIALTRANSLATIONUNIT_H)
#define LLVM_CLANG_INTERPRETER_PARTIALTRANSLATIONUNIT_H

#include <memory>

namespace llvm {
class Module;
}

namespace clang {

class TranslationUnitDecl;



struct PartialTranslationUnit {
TranslationUnitDecl *TUPart = nullptr;


std::unique_ptr<llvm::Module> TheModule;
};
}

#endif
