











#if !defined(LLVM_CLANG_AST_ASTDUMPERUTILS_H)
#define LLVM_CLANG_AST_ASTDUMPERUTILS_H

#include "llvm/Support/raw_ostream.h"

namespace clang {


enum ASTDumpOutputFormat {
ADOF_Default,
ADOF_JSON
};




struct TerminalColor {
llvm::raw_ostream::Colors Color;
bool Bold;
};












static const TerminalColor DeclKindNameColor = {llvm::raw_ostream::GREEN, true};

static const TerminalColor AttrColor = {llvm::raw_ostream::BLUE, true};

static const TerminalColor StmtColor = {llvm::raw_ostream::MAGENTA, true};

static const TerminalColor CommentColor = {llvm::raw_ostream::BLUE, false};


static const TerminalColor TypeColor = {llvm::raw_ostream::GREEN, false};


static const TerminalColor AddressColor = {llvm::raw_ostream::YELLOW, false};

static const TerminalColor LocationColor = {llvm::raw_ostream::YELLOW, false};


static const TerminalColor ValueKindColor = {llvm::raw_ostream::CYAN, false};

static const TerminalColor ObjectKindColor = {llvm::raw_ostream::CYAN, false};

static const TerminalColor ErrorsColor = {llvm::raw_ostream::RED, true};


static const TerminalColor NullColor = {llvm::raw_ostream::BLUE, false};


static const TerminalColor UndeserializedColor = {llvm::raw_ostream::GREEN,
true};


static const TerminalColor CastColor = {llvm::raw_ostream::RED, false};


static const TerminalColor ValueColor = {llvm::raw_ostream::CYAN, true};

static const TerminalColor DeclNameColor = {llvm::raw_ostream::CYAN, true};


static const TerminalColor IndentColor = {llvm::raw_ostream::BLUE, false};

class ColorScope {
llvm::raw_ostream &OS;
const bool ShowColors;

public:
ColorScope(llvm::raw_ostream &OS, bool ShowColors, TerminalColor Color)
: OS(OS), ShowColors(ShowColors) {
if (ShowColors)
OS.changeColor(Color.Color, Color.Bold);
}
~ColorScope() {
if (ShowColors)
OS.resetColor();
}
};

}

#endif
