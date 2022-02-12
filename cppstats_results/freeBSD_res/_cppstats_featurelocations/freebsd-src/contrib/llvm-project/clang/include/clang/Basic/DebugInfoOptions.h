







#if !defined(LLVM_CLANG_BASIC_DEBUGINFOOPTIONS_H)
#define LLVM_CLANG_BASIC_DEBUGINFOOPTIONS_H

namespace clang {
namespace codegenoptions {

enum DebugInfoFormat {
DIF_DWARF,
DIF_CodeView,
};

enum DebugInfoKind {

NoDebugInfo,





LocTrackingOnly,


DebugDirectivesOnly,



DebugLineTablesOnly,





DebugInfoConstructor,





LimitedDebugInfo,


FullDebugInfo,



UnusedTypeInfo,
};

}
}

#endif
