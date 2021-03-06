















#if !defined(LLVM_CLANG_LEX_DEPENDENCY_DIRECTIVES_SOURCE_MINIMIZER_H)
#define LLVM_CLANG_LEX_DEPENDENCY_DIRECTIVES_SOURCE_MINIMIZER_H

#include "clang/Basic/SourceLocation.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"

namespace clang {

class DiagnosticsEngine;

namespace minimize_source_to_dependency_directives {



enum TokenKind {
pp_none,
pp_include,
pp___include_macros,
pp_define,
pp_undef,
pp_import,
pp_pragma_import,
pp_pragma_once,
pp_include_next,
pp_if,
pp_ifdef,
pp_ifndef,
pp_elif,
pp_elifdef,
pp_elifndef,
pp_else,
pp_endif,
decl_at_import,
cxx_export_decl,
cxx_module_decl,
cxx_import_decl,
pp_eof,
};




struct Token {

TokenKind K = pp_none;


int Offset = -1;

Token(TokenKind K, int Offset) : K(K), Offset(Offset) {}
};



struct SkippedRange {

int Offset;



int Length;
};





bool computeSkippedRanges(ArrayRef<Token> Input,
llvm::SmallVectorImpl<SkippedRange> &Range);

}













bool minimizeSourceToDependencyDirectives(
llvm::StringRef Input, llvm::SmallVectorImpl<char> &Output,
llvm::SmallVectorImpl<minimize_source_to_dependency_directives::Token>
&Tokens,
DiagnosticsEngine *Diags = nullptr,
SourceLocation InputSourceLoc = SourceLocation());

}

#endif
