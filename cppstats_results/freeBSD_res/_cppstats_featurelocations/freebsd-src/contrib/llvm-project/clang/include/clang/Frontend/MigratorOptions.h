












#if !defined(LLVM_CLANG_FRONTEND_MIGRATOROPTIONS_H)
#define LLVM_CLANG_FRONTEND_MIGRATOROPTIONS_H

namespace clang {

class MigratorOptions {
public:
unsigned NoNSAllocReallocError : 1;
unsigned NoFinalizeRemoval : 1;
MigratorOptions() {
NoNSAllocReallocError = 0;
NoFinalizeRemoval = 0;
}
};

}
#endif
