







#if !defined(LLVM_CLANG_FRONTEND_PREPROCESSOROUTPUTOPTIONS_H)
#define LLVM_CLANG_FRONTEND_PREPROCESSOROUTPUTOPTIONS_H

namespace clang {



class PreprocessorOutputOptions {
public:
unsigned ShowCPP : 1;
unsigned ShowComments : 1;
unsigned ShowLineMarkers : 1;
unsigned UseLineDirectives : 1;
unsigned ShowMacroComments : 1;
unsigned ShowMacros : 1;
unsigned ShowIncludeDirectives : 1;
unsigned RewriteIncludes : 1;
unsigned RewriteImports : 1;

public:
PreprocessorOutputOptions() {
ShowCPP = 0;
ShowComments = 0;
ShowLineMarkers = 1;
UseLineDirectives = 0;
ShowMacroComments = 0;
ShowMacros = 0;
ShowIncludeDirectives = 0;
RewriteIncludes = 0;
RewriteImports = 0;
}
};

}

#endif
