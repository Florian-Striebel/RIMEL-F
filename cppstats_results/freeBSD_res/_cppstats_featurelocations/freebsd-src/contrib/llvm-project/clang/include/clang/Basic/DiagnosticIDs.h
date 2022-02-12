












#if !defined(LLVM_CLANG_BASIC_DIAGNOSTICIDS_H)
#define LLVM_CLANG_BASIC_DIAGNOSTICIDS_H

#include "clang/Basic/LLVM.h"
#include "llvm/ADT/IntrusiveRefCntPtr.h"
#include "llvm/ADT/StringRef.h"
#include <vector>

namespace clang {
class DiagnosticsEngine;
class SourceLocation;


namespace diag {

enum {
DIAG_SIZE_COMMON = 300,
DIAG_SIZE_DRIVER = 250,
DIAG_SIZE_FRONTEND = 150,
DIAG_SIZE_SERIALIZATION = 120,
DIAG_SIZE_LEX = 400,
DIAG_SIZE_PARSE = 600,
DIAG_SIZE_AST = 250,
DIAG_SIZE_COMMENT = 100,
DIAG_SIZE_CROSSTU = 100,
DIAG_SIZE_SEMA = 4500,
DIAG_SIZE_ANALYSIS = 100,
DIAG_SIZE_REFACTORING = 1000,
};

enum {
DIAG_START_COMMON = 0,
DIAG_START_DRIVER = DIAG_START_COMMON + DIAG_SIZE_COMMON,
DIAG_START_FRONTEND = DIAG_START_DRIVER + DIAG_SIZE_DRIVER,
DIAG_START_SERIALIZATION = DIAG_START_FRONTEND + DIAG_SIZE_FRONTEND,
DIAG_START_LEX = DIAG_START_SERIALIZATION + DIAG_SIZE_SERIALIZATION,
DIAG_START_PARSE = DIAG_START_LEX + DIAG_SIZE_LEX,
DIAG_START_AST = DIAG_START_PARSE + DIAG_SIZE_PARSE,
DIAG_START_COMMENT = DIAG_START_AST + DIAG_SIZE_AST,
DIAG_START_CROSSTU = DIAG_START_COMMENT + DIAG_SIZE_COMMENT,
DIAG_START_SEMA = DIAG_START_CROSSTU + DIAG_SIZE_CROSSTU,
DIAG_START_ANALYSIS = DIAG_START_SEMA + DIAG_SIZE_SEMA,
DIAG_START_REFACTORING = DIAG_START_ANALYSIS + DIAG_SIZE_ANALYSIS,
DIAG_UPPER_LIMIT = DIAG_START_REFACTORING + DIAG_SIZE_REFACTORING
};

class CustomDiagInfo;


typedef unsigned kind;


enum {
#define DIAG(ENUM, FLAGS, DEFAULT_MAPPING, DESC, GROUP, SFINAE, CATEGORY, NOWERROR, SHOWINSYSHEADER, DEFFERABLE) ENUM,


#define COMMONSTART
#include "clang/Basic/DiagnosticCommonKinds.inc"
NUM_BUILTIN_COMMON_DIAGNOSTICS
#undef DIAG
};





enum class Severity {

Ignored = 1,
Remark = 2,
Warning = 3,
Error = 4,
Fatal = 5
};



enum class Flavor {
WarningOrError,

Remark

};
}

class DiagnosticMapping {
unsigned Severity : 3;
unsigned IsUser : 1;
unsigned IsPragma : 1;
unsigned HasNoWarningAsError : 1;
unsigned HasNoErrorAsFatal : 1;
unsigned WasUpgradedFromWarning : 1;

public:
static DiagnosticMapping Make(diag::Severity Severity, bool IsUser,
bool IsPragma) {
DiagnosticMapping Result;
Result.Severity = (unsigned)Severity;
Result.IsUser = IsUser;
Result.IsPragma = IsPragma;
Result.HasNoWarningAsError = 0;
Result.HasNoErrorAsFatal = 0;
Result.WasUpgradedFromWarning = 0;
return Result;
}

diag::Severity getSeverity() const { return (diag::Severity)Severity; }
void setSeverity(diag::Severity Value) { Severity = (unsigned)Value; }

bool isUser() const { return IsUser; }
bool isPragma() const { return IsPragma; }

bool isErrorOrFatal() const {
return getSeverity() == diag::Severity::Error ||
getSeverity() == diag::Severity::Fatal;
}

bool hasNoWarningAsError() const { return HasNoWarningAsError; }
void setNoWarningAsError(bool Value) { HasNoWarningAsError = Value; }

bool hasNoErrorAsFatal() const { return HasNoErrorAsFatal; }
void setNoErrorAsFatal(bool Value) { HasNoErrorAsFatal = Value; }




bool wasUpgradedFromWarning() const { return WasUpgradedFromWarning; }
void setUpgradedFromWarning(bool Value) { WasUpgradedFromWarning = Value; }


unsigned serialize() const {
return (IsUser << 7) | (IsPragma << 6) | (HasNoWarningAsError << 5) |
(HasNoErrorAsFatal << 4) | (WasUpgradedFromWarning << 3) | Severity;
}

static DiagnosticMapping deserialize(unsigned Bits) {
DiagnosticMapping Result;
Result.IsUser = (Bits >> 7) & 1;
Result.IsPragma = (Bits >> 6) & 1;
Result.HasNoWarningAsError = (Bits >> 5) & 1;
Result.HasNoErrorAsFatal = (Bits >> 4) & 1;
Result.WasUpgradedFromWarning = (Bits >> 3) & 1;
Result.Severity = Bits & 0x7;
return Result;
}
};




class DiagnosticIDs : public RefCountedBase<DiagnosticIDs> {
public:

enum Level {
Ignored, Note, Remark, Warning, Error, Fatal
};

private:

std::unique_ptr<diag::CustomDiagInfo> CustomDiagInfo;

public:
DiagnosticIDs();
~DiagnosticIDs();










unsigned getCustomDiagID(Level L, StringRef FormatString);






StringRef getDescription(unsigned DiagID) const;






static bool isBuiltinWarningOrExtension(unsigned DiagID);



static bool isDefaultMappingAsError(unsigned DiagID);


static bool isBuiltinNote(unsigned DiagID);



static bool isBuiltinExtensionDiag(unsigned DiagID) {
bool ignored;
return isBuiltinExtensionDiag(DiagID, ignored);
}








static bool isBuiltinExtensionDiag(unsigned DiagID, bool &EnabledByDefault);






static StringRef getWarningOptionForDiag(unsigned DiagID);



static unsigned getCategoryNumberForDiag(unsigned DiagID);


static unsigned getNumberOfCategories();


static StringRef getCategoryNameFromID(unsigned CategoryID);



static bool isARCDiagnostic(unsigned DiagID);



enum SFINAEResponse {





SFINAE_SubstitutionFailure,




SFINAE_Suppress,





SFINAE_Report,



SFINAE_AccessControl
};









static SFINAEResponse getDiagnosticSFINAEResponse(unsigned DiagID);






static bool isDeferrable(unsigned DiagID);






static std::vector<std::string> getDiagnosticFlags();





bool getDiagnosticsInGroup(diag::Flavor Flavor, StringRef Group,
SmallVectorImpl<diag::kind> &Diags) const;


static void getAllDiagnostics(diag::Flavor Flavor,
std::vector<diag::kind> &Diags);



static StringRef getNearestOption(diag::Flavor Flavor, StringRef Group);

private:








DiagnosticIDs::Level
getDiagnosticLevel(unsigned DiagID, SourceLocation Loc,
const DiagnosticsEngine &Diag) const LLVM_READONLY;

diag::Severity
getDiagnosticSeverity(unsigned DiagID, SourceLocation Loc,
const DiagnosticsEngine &Diag) const LLVM_READONLY;





bool ProcessDiag(DiagnosticsEngine &Diag) const;



void EmitDiag(DiagnosticsEngine &Diag, Level DiagLevel) const;



bool isUnrecoverable(unsigned DiagID) const;

friend class DiagnosticsEngine;
};

}

#endif
