







#if !defined(LLVM_CLANG_DRIVER_PHASES_H)
#define LLVM_CLANG_DRIVER_PHASES_H

namespace clang {
namespace driver {
namespace phases {


enum ID {
Preprocess,
Precompile,
Compile,
Backend,
Assemble,
Link,
IfsMerge,
LastPhase = IfsMerge,
};

enum {
MaxNumberOfPhases = LastPhase + 1
};

const char *getPhaseName(ID Id);

}
}
}

#endif
