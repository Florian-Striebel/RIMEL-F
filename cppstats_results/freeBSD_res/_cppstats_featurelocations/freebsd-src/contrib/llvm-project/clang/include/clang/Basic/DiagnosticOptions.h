







#if !defined(LLVM_CLANG_BASIC_DIAGNOSTICOPTIONS_H)
#define LLVM_CLANG_BASIC_DIAGNOSTICOPTIONS_H

#include "clang/Basic/LLVM.h"
#include "llvm/ADT/IntrusiveRefCntPtr.h"
#include <string>
#include <type_traits>
#include <vector>

namespace llvm {
namespace opt {
class ArgList;
}
}

namespace clang {
class DiagnosticsEngine;



enum OverloadsShown : unsigned {

Ovl_All,


Ovl_Best
};



enum class DiagnosticLevelMask : unsigned {
None = 0,
Note = 1 << 0,
Remark = 1 << 1,
Warning = 1 << 2,
Error = 1 << 3,
All = Note | Remark | Warning | Error
};

inline DiagnosticLevelMask operator~(DiagnosticLevelMask M) {
using UT = std::underlying_type<DiagnosticLevelMask>::type;
return static_cast<DiagnosticLevelMask>(~static_cast<UT>(M));
}

inline DiagnosticLevelMask operator|(DiagnosticLevelMask LHS,
DiagnosticLevelMask RHS) {
using UT = std::underlying_type<DiagnosticLevelMask>::type;
return static_cast<DiagnosticLevelMask>(
static_cast<UT>(LHS) | static_cast<UT>(RHS));
}

inline DiagnosticLevelMask operator&(DiagnosticLevelMask LHS,
DiagnosticLevelMask RHS) {
using UT = std::underlying_type<DiagnosticLevelMask>::type;
return static_cast<DiagnosticLevelMask>(
static_cast<UT>(LHS) & static_cast<UT>(RHS));
}

raw_ostream& operator<<(raw_ostream& Out, DiagnosticLevelMask M);


class DiagnosticOptions : public RefCountedBase<DiagnosticOptions>{
friend bool ParseDiagnosticArgs(DiagnosticOptions &, llvm::opt::ArgList &,
clang::DiagnosticsEngine *, bool);

friend class CompilerInvocation;

public:
enum TextDiagnosticFormat { Clang, MSVC, Vi };


enum {
DefaultTabStop = 8,
MaxTabStop = 100,
DefaultMacroBacktraceLimit = 6,
DefaultTemplateBacktraceLimit = 10,
DefaultConstexprBacktraceLimit = 10,
DefaultSpellCheckingLimit = 50,
DefaultSnippetLineLimit = 1,
};


#define DIAGOPT(Name, Bits, Default) unsigned Name : Bits;
#define ENUM_DIAGOPT(Name, Type, Bits, Default)
#include "clang/Basic/DiagnosticOptions.def"

protected:


#define DIAGOPT(Name, Bits, Default)
#define ENUM_DIAGOPT(Name, Type, Bits, Default) unsigned Name : Bits;
#include "clang/Basic/DiagnosticOptions.def"

public:

std::string DiagnosticLogFile;


std::string DiagnosticSerializationFile;



std::vector<std::string> Warnings;



std::vector<std::string> UndefPrefixes;



std::vector<std::string> Remarks;



std::vector<std::string> VerifyPrefixes;

public:

#define DIAGOPT(Name, Bits, Default)
#define ENUM_DIAGOPT(Name, Type, Bits, Default) Type get##Name() const { return static_cast<Type>(Name); } void set##Name(Type Value) { Name = static_cast<unsigned>(Value); }


#include "clang/Basic/DiagnosticOptions.def"

DiagnosticOptions() {
#define DIAGOPT(Name, Bits, Default) Name = Default;
#define ENUM_DIAGOPT(Name, Type, Bits, Default) set##Name(Default);
#include "clang/Basic/DiagnosticOptions.def"
}
};

using TextDiagnosticFormat = DiagnosticOptions::TextDiagnosticFormat;

}

#endif
