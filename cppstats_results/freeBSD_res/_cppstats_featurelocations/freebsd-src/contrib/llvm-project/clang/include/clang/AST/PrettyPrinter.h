











#if !defined(LLVM_CLANG_AST_PRETTYPRINTER_H)
#define LLVM_CLANG_AST_PRETTYPRINTER_H

#include "clang/Basic/LLVM.h"
#include "clang/Basic/LangOptions.h"

namespace clang {

class DeclContext;
class LangOptions;
class SourceManager;
class Stmt;
class TagDecl;

class PrinterHelper {
public:
virtual ~PrinterHelper();
virtual bool handledStmt(Stmt* E, raw_ostream& OS) = 0;
};


class PrintingCallbacks {
protected:
~PrintingCallbacks() = default;

public:

virtual std::string remapPath(StringRef Path) const {
return std::string(Path);
}








virtual bool isScopeVisible(const DeclContext *DC) const { return false; }
};






struct PrintingPolicy {

PrintingPolicy(const LangOptions &LO)
: Indentation(2), SuppressSpecifiers(false),
SuppressTagKeyword(LO.CPlusPlus), IncludeTagDefinition(false),
SuppressScope(false), SuppressUnwrittenScope(false),
SuppressInlineNamespace(true), SuppressInitializers(false),
ConstantArraySizeAsWritten(false), AnonymousTagLocations(true),
SuppressStrongLifetime(false), SuppressLifetimeQualifiers(false),
SuppressTemplateArgsInCXXConstructors(false),
SuppressDefaultTemplateArgs(true), Bool(LO.Bool),
Nullptr(LO.CPlusPlus11), Restrict(LO.C99), Alignof(LO.CPlusPlus11),
UnderscoreAlignof(LO.C11), UseVoidForZeroParams(!LO.CPlusPlus),
SplitTemplateClosers(!LO.CPlusPlus11), TerseOutput(false),
PolishForDeclaration(false), Half(LO.Half),
MSWChar(LO.MicrosoftExt && !LO.WChar), IncludeNewlines(true),
MSVCFormatting(false), ConstantsAsWritten(false),
SuppressImplicitBase(false), FullyQualifiedName(false),
PrintCanonicalTypes(false), PrintInjectedClassNameWithArguments(true) {}





void adjustForCPlusPlus() {
SuppressTagKeyword = true;
Bool = true;
UseVoidForZeroParams = false;
}


unsigned Indentation : 8;















unsigned SuppressSpecifiers : 1;









unsigned SuppressTagKeyword : 1;









unsigned IncludeTagDefinition : 1;


unsigned SuppressScope : 1;



unsigned SuppressUnwrittenScope : 1;




unsigned SuppressInlineNamespace : 1;












unsigned SuppressInitializers : 1;

















unsigned ConstantArraySizeAsWritten : 1;




unsigned AnonymousTagLocations : 1;


unsigned SuppressStrongLifetime : 1;


unsigned SuppressLifetimeQualifiers : 1;



unsigned SuppressTemplateArgsInCXXConstructors : 1;



unsigned SuppressDefaultTemplateArgs : 1;



unsigned Bool : 1;



unsigned Nullptr : 1;


unsigned Restrict : 1;


unsigned Alignof : 1;


unsigned UnderscoreAlignof : 1;



unsigned UseVoidForZeroParams : 1;



unsigned SplitTemplateClosers : 1;






unsigned TerseOutput : 1;




unsigned PolishForDeclaration : 1;



unsigned Half : 1;



unsigned MSWChar : 1;


unsigned IncludeNewlines : 1;




unsigned MSVCFormatting : 1;

















unsigned ConstantsAsWritten : 1;


unsigned SuppressImplicitBase : 1;



unsigned FullyQualifiedName : 1;


unsigned PrintCanonicalTypes : 1;




unsigned PrintInjectedClassNameWithArguments : 1;


const PrintingCallbacks *Callbacks = nullptr;
};

}

#endif
