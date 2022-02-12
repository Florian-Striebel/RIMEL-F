














#if !defined(LLVM_CLANG_BASIC_LAMBDA_H)
#define LLVM_CLANG_BASIC_LAMBDA_H

namespace clang {


enum LambdaCaptureDefault {
LCD_None,
LCD_ByCopy,
LCD_ByRef
};






enum LambdaCaptureKind {
LCK_This,
LCK_StarThis,
LCK_ByCopy,
LCK_ByRef,
LCK_VLAType
};

}

#endif
