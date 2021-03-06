












#if !defined(LLVM_CLANG_BASIC_MACROBUILDER_H)
#define LLVM_CLANG_BASIC_MACROBUILDER_H

#include "clang/Basic/LLVM.h"
#include "llvm/ADT/Twine.h"
#include "llvm/Support/raw_ostream.h"

namespace clang {

class MacroBuilder {
raw_ostream &Out;
public:
MacroBuilder(raw_ostream &Output) : Out(Output) {}


void defineMacro(const Twine &Name, const Twine &Value = "1") {
Out << "#define " << Name << ' ' << Value << '\n';
}



void undefineMacro(const Twine &Name) {
Out << "#undef " << Name << '\n';
}


void append(const Twine &Str) {
Out << Str << '\n';
}
};

}

#endif
