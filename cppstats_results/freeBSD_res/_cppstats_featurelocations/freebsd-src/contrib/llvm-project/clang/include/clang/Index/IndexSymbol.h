







#if !defined(LLVM_CLANG_INDEX_INDEXSYMBOL_H)
#define LLVM_CLANG_INDEX_INDEXSYMBOL_H

#include "clang/Basic/LLVM.h"
#include "clang/Lex/MacroInfo.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/Support/DataTypes.h"

namespace clang {
class Decl;
class LangOptions;

namespace index {

enum class SymbolKind : uint8_t {
Unknown,

Module,
Namespace,
NamespaceAlias,
Macro,

Enum,
Struct,
Class,
Protocol,
Extension,
Union,
TypeAlias,

Function,
Variable,
Field,
EnumConstant,

InstanceMethod,
ClassMethod,
StaticMethod,
InstanceProperty,
ClassProperty,
StaticProperty,

Constructor,
Destructor,
ConversionFunction,

Parameter,
Using,
TemplateTypeParm,
TemplateTemplateParm,
NonTypeTemplateParm,
};

enum class SymbolLanguage : uint8_t {
C,
ObjC,
CXX,
Swift,
};


enum class SymbolSubKind : uint8_t {
None,
CXXCopyConstructor,
CXXMoveConstructor,
AccessorGetter,
AccessorSetter,
UsingTypename,
UsingValue,
UsingEnum,
};

typedef uint16_t SymbolPropertySet;

enum class SymbolProperty : SymbolPropertySet {
Generic = 1 << 0,
TemplatePartialSpecialization = 1 << 1,
TemplateSpecialization = 1 << 2,
UnitTest = 1 << 3,
IBAnnotated = 1 << 4,
IBOutletCollection = 1 << 5,
GKInspectable = 1 << 6,
Local = 1 << 7,

ProtocolInterface = 1 << 8,
};
static const unsigned SymbolPropertyBitNum = 9;




enum class SymbolRole : uint32_t {
Declaration = 1 << 0,
Definition = 1 << 1,
Reference = 1 << 2,
Read = 1 << 3,
Write = 1 << 4,
Call = 1 << 5,
Dynamic = 1 << 6,
AddressOf = 1 << 7,
Implicit = 1 << 8,


Undefinition = 1 << 9,


RelationChildOf = 1 << 10,
RelationBaseOf = 1 << 11,
RelationOverrideOf = 1 << 12,
RelationReceivedBy = 1 << 13,
RelationCalledBy = 1 << 14,
RelationExtendedBy = 1 << 15,
RelationAccessorOf = 1 << 16,
RelationContainedBy = 1 << 17,
RelationIBTypeOf = 1 << 18,
RelationSpecializationOf = 1 << 19,



NameReference = 1 << 20,
};
static const unsigned SymbolRoleBitNum = 21;
typedef unsigned SymbolRoleSet;


struct SymbolRelation {
SymbolRoleSet Roles;
const Decl *RelatedSymbol;

SymbolRelation(SymbolRoleSet Roles, const Decl *Sym)
: Roles(Roles), RelatedSymbol(Sym) {}
};

struct SymbolInfo {
SymbolKind Kind;
SymbolSubKind SubKind;
SymbolLanguage Lang;
SymbolPropertySet Properties;
};

SymbolInfo getSymbolInfo(const Decl *D);

SymbolInfo getSymbolInfoForMacro(const MacroInfo &MI);

bool isFunctionLocalSymbol(const Decl *D);

void applyForEachSymbolRole(SymbolRoleSet Roles,
llvm::function_ref<void(SymbolRole)> Fn);
bool applyForEachSymbolRoleInterruptible(SymbolRoleSet Roles,
llvm::function_ref<bool(SymbolRole)> Fn);
void printSymbolRoles(SymbolRoleSet Roles, raw_ostream &OS);


bool printSymbolName(const Decl *D, const LangOptions &LO, raw_ostream &OS);

StringRef getSymbolKindString(SymbolKind K);
StringRef getSymbolSubKindString(SymbolSubKind K);
StringRef getSymbolLanguageString(SymbolLanguage K);

void applyForEachSymbolProperty(SymbolPropertySet Props,
llvm::function_ref<void(SymbolProperty)> Fn);
void printSymbolProperties(SymbolPropertySet Props, raw_ostream &OS);

}
}

#endif
