







#if !defined(LLVM_CLANG_STATICANALYZER_CORE_BUGREPORTER_COMMONBUGCATEGORIES_H)
#define LLVM_CLANG_STATICANALYZER_CORE_BUGREPORTER_COMMONBUGCATEGORIES_H


namespace clang {
namespace ento {
namespace categories {
extern const char *const CoreFoundationObjectiveC;
extern const char *const LogicError;
extern const char *const MemoryRefCount;
extern const char *const MemoryError;
extern const char *const UnixAPI;
extern const char *const CXXObjectLifecycle;
extern const char *const CXXMoveSemantics;
extern const char *const SecurityError;
extern const char *const UnusedCode;
}
}
}
#endif
