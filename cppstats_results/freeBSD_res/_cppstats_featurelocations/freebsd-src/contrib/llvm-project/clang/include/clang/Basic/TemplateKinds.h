











#if !defined(LLVM_CLANG_BASIC_TEMPLATEKINDS_H)
#define LLVM_CLANG_BASIC_TEMPLATEKINDS_H

namespace clang {



enum TemplateNameKind {

TNK_Non_template = 0,



TNK_Function_template,



TNK_Type_template,


TNK_Var_template,












TNK_Dependent_template_name,



TNK_Undeclared_template,

TNK_Concept_template,
};

}
#endif
