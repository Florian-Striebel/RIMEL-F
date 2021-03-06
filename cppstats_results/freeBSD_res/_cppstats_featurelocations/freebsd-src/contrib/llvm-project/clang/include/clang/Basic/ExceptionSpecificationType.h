












#if !defined(LLVM_CLANG_BASIC_EXCEPTIONSPECIFICATIONTYPE_H)
#define LLVM_CLANG_BASIC_EXCEPTIONSPECIFICATIONTYPE_H

namespace clang {


enum ExceptionSpecificationType {
EST_None,
EST_DynamicNone,
EST_Dynamic,
EST_MSAny,
EST_NoThrow,
EST_BasicNoexcept,
EST_DependentNoexcept,
EST_NoexceptFalse,
EST_NoexceptTrue,
EST_Unevaluated,
EST_Uninstantiated,
EST_Unparsed
};

inline bool isDynamicExceptionSpec(ExceptionSpecificationType ESpecType) {
return ESpecType >= EST_DynamicNone && ESpecType <= EST_MSAny;
}

inline bool isComputedNoexcept(ExceptionSpecificationType ESpecType) {
return ESpecType >= EST_DependentNoexcept &&
ESpecType <= EST_NoexceptTrue;
}

inline bool isNoexceptExceptionSpec(ExceptionSpecificationType ESpecType) {
return ESpecType == EST_BasicNoexcept || ESpecType == EST_NoThrow ||
isComputedNoexcept(ESpecType);
}

inline bool isUnresolvedExceptionSpec(ExceptionSpecificationType ESpecType) {
return ESpecType == EST_Unevaluated || ESpecType == EST_Uninstantiated;
}


enum CanThrowResult {
CT_Cannot,
CT_Dependent,
CT_Can
};

inline CanThrowResult mergeCanThrow(CanThrowResult CT1, CanThrowResult CT2) {


return CT1 > CT2 ? CT1 : CT2;
}

}

#endif
