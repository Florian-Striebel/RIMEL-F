







#if !defined(LLVM_CLANG_INDEX_USRGENERATION_H)
#define LLVM_CLANG_INDEX_USRGENERATION_H

#include "clang/Basic/LLVM.h"
#include "llvm/ADT/StringRef.h"

namespace clang {
class ASTContext;
class Decl;
class MacroDefinitionRecord;
class Module;
class SourceLocation;
class SourceManager;
class QualType;

namespace index {

static inline StringRef getUSRSpacePrefix() {
return "c:";
}



bool generateUSRForDecl(const Decl *D, SmallVectorImpl<char> &Buf);


void generateUSRForObjCClass(StringRef Cls, raw_ostream &OS,
StringRef ExtSymbolDefinedIn = "",
StringRef CategoryContextExtSymbolDefinedIn = "");


void generateUSRForObjCCategory(StringRef Cls, StringRef Cat, raw_ostream &OS,
StringRef ClsExtSymbolDefinedIn = "",
StringRef CatExtSymbolDefinedIn = "");




void generateUSRForObjCIvar(StringRef Ivar, raw_ostream &OS);


void generateUSRForObjCMethod(StringRef Sel, bool IsInstanceMethod,
raw_ostream &OS);


void generateUSRForObjCProperty(StringRef Prop, bool isClassProp, raw_ostream &OS);


void generateUSRForObjCProtocol(StringRef Prot, raw_ostream &OS,
StringRef ExtSymbolDefinedIn = "");


void generateUSRForGlobalEnum(StringRef EnumName, raw_ostream &OS,
StringRef ExtSymbolDefinedIn = "");


void generateUSRForEnumConstant(StringRef EnumConstantName, raw_ostream &OS);




bool generateUSRForMacro(const MacroDefinitionRecord *MD,
const SourceManager &SM, SmallVectorImpl<char> &Buf);
bool generateUSRForMacro(StringRef MacroName, SourceLocation Loc,
const SourceManager &SM, SmallVectorImpl<char> &Buf);




bool generateUSRForType(QualType T, ASTContext &Ctx, SmallVectorImpl<char> &Buf);



bool generateFullUSRForModule(const Module *Mod, raw_ostream &OS);



bool generateFullUSRForTopLevelModuleName(StringRef ModName, raw_ostream &OS);



bool generateUSRFragmentForModule(const Module *Mod, raw_ostream &OS);



bool generateUSRFragmentForModuleName(StringRef ModName, raw_ostream &OS);


}
}

#endif

