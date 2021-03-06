







#if !defined(LLVM_CLANG_BASIC_PRAGMA_KINDS_H)
#define LLVM_CLANG_BASIC_PRAGMA_KINDS_H

namespace clang {

enum PragmaMSCommentKind {
PCK_Unknown,
PCK_Linker,
PCK_Lib,
PCK_Compiler,
PCK_ExeStr,
PCK_User
};

enum PragmaMSStructKind {
PMSST_OFF,
PMSST_ON
};

enum PragmaFloatControlKind {
PFC_Unknown,
PFC_Precise,
PFC_NoPrecise,
PFC_Except,
PFC_NoExcept,
PFC_Push,
PFC_Pop
};
}

#endif
