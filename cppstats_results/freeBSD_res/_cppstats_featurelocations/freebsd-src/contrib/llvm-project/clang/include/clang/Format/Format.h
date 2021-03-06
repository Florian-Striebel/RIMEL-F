












#if !defined(LLVM_CLANG_FORMAT_FORMAT_H)
#define LLVM_CLANG_FORMAT_FORMAT_H

#include "clang/Basic/LangOptions.h"
#include "clang/Tooling/Core/Replacement.h"
#include "clang/Tooling/Inclusions/IncludeStyle.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/Support/Regex.h"
#include "llvm/Support/SourceMgr.h"
#include <system_error>

namespace llvm {
namespace vfs {
class FileSystem;
}
}

namespace clang {

class Lexer;
class SourceManager;
class DiagnosticConsumer;

namespace format {

enum class ParseError {
Success = 0,
Error,
Unsuitable,
BinPackTrailingCommaConflict
};
class ParseErrorCategory final : public std::error_category {
public:
const char *name() const noexcept override;
std::string message(int EV) const override;
};
const std::error_category &getParseCategory();
std::error_code make_error_code(ParseError e);



struct FormatStyle {



bool InheritsParentConfig;


int AccessModifierOffset;


enum BracketAlignmentStyle : unsigned char {





BAS_Align,





BAS_DontAlign,






BAS_AlwaysBreak,
};





BracketAlignmentStyle AlignAfterOpenBracket;


enum ArrayInitializerAlignmentStyle {









AIAS_Left,









AIAS_Right,

AIAS_None
};


ArrayInitializerAlignmentStyle AlignArrayOfStructures;







enum AlignConsecutiveStyle {
ACS_None,
ACS_Consecutive,
ACS_AcrossEmptyLines,
ACS_AcrossComments,
ACS_AcrossEmptyLinesAndComments
};





































































AlignConsecutiveStyle AlignConsecutiveMacros;



































































AlignConsecutiveStyle AlignConsecutiveAssignments;




































































AlignConsecutiveStyle AlignConsecutiveBitFields;




































































AlignConsecutiveStyle AlignConsecutiveDeclarations;


enum EscapedNewlineAlignmentStyle : unsigned char {







ENAS_DontAlign,










ENAS_Left,







ENAS_Right,
};


EscapedNewlineAlignmentStyle AlignEscapedNewlines;


enum OperandAlignmentStyle : unsigned char {



OAS_DontAlign,















OAS_Align,









OAS_AlignAfterOperator,
};



OperandAlignmentStyle AlignOperands;







bool AlignTrailingComments;















bool AllowAllArgumentsOnNextLine;















bool AllowAllConstructorInitializersOnNextLine;
















bool AllowAllParametersOfDeclarationOnNextLine;













bool AllowShortEnumsOnASingleLine;



enum ShortBlockStyle : unsigned char {








SBS_Never,







SBS_Empty,





SBS_Always,
};



ShortBlockStyle AllowShortBlocksOnASingleLine;












bool AllowShortCaseLabelsOnASingleLine;



enum ShortFunctionStyle : unsigned char {

SFS_None,













SFS_InlineOnly,







SFS_Empty,










SFS_Inline,







SFS_All,
};



ShortFunctionStyle AllowShortFunctionsOnASingleLine;


enum ShortIfStyle : unsigned char {
















SIS_Never,















SIS_WithoutElse,















SIS_OnlyFirstIf,













SIS_AllIfsAndElse,
};


ShortIfStyle AllowShortIfStatementsOnASingleLine;



enum ShortLambdaStyle : unsigned char {

SLS_None,







SLS_Empty,







SLS_Inline,





SLS_All,
};



ShortLambdaStyle AllowShortLambdasOnASingleLine;



bool AllowShortLoopsOnASingleLine;



enum DefinitionReturnTypeBreakingStyle : unsigned char {


DRTBS_None,

DRTBS_All,

DRTBS_TopLevel,
};



enum ReturnTypeBreakingStyle : unsigned char {









RTBS_None,















RTBS_All,












RTBS_TopLevel,














RTBS_AllDefinitions,











RTBS_TopLevelDefinitions,
};



DefinitionReturnTypeBreakingStyle AlwaysBreakAfterDefinitionReturnType;


ReturnTypeBreakingStyle AlwaysBreakAfterReturnType;













bool AlwaysBreakBeforeMultilineStrings;


enum BreakTemplateDeclarationsStyle : unsigned char {









BTDS_No,










BTDS_MultiLine,










BTDS_Yes
};


BreakTemplateDeclarationsStyle AlwaysBreakTemplateDeclarations;

















std::vector<std::string> AttributeMacros;

















bool BinPackArguments;


enum TrailingCommaStyle : unsigned char {

TCS_None,





TCS_Wrapped,
};
















TrailingCommaStyle InsertTrailingCommas;













bool BinPackParameters;



enum BinPackStyle : unsigned char {

BPS_Auto,

BPS_Always,

BPS_Never,
};


enum BinaryOperatorStyle : unsigned char {











BOS_None,











BOS_NonAssignment,











BOS_All,
};


BinaryOperatorStyle BreakBeforeBinaryOperators;


enum BraceBreakingStyle : unsigned char {












































BS_Attach,

















































BS_Linux,

















































BS_Mozilla,

















































BS_Stroustrup,



























































BS_Allman,



























































BS_Whitesmiths,






























































BS_GNU,














































BS_WebKit,

BS_Custom
};


BraceBreakingStyle BreakBeforeBraces;


enum BraceWrappingAfterControlStatementStyle : unsigned char {








BWACS_Never,










BWACS_MultiLine,









BWACS_Always
};








struct BraceWrappingFlags {















bool AfterCaseLabel;









bool AfterClass;


BraceWrappingAfterControlStatementStyle AfterControlStatement;











bool AfterEnum;















bool AfterFunction;















bool AfterNamespace;



bool AfterObjCDeclaration;













bool AfterStruct;













bool AfterUnion;













bool AfterExternBlock;















bool BeforeCatch;













bool BeforeElse;
















bool BeforeLambdaBody;













bool BeforeWhile;

bool IndentBraces;











bool SplitEmptyFunction;










bool SplitEmptyRecord;










bool SplitEmptyNamespace;
};













BraceWrappingFlags BraceWrapping;










bool BreakBeforeConceptDeclarations;













bool BreakBeforeTernaryOperators;


enum BreakConstructorInitializersStyle : unsigned char {






BCIS_BeforeColon,







BCIS_BeforeComma,






BCIS_AfterColon
};


BreakConstructorInitializersStyle BreakConstructorInitializers;








bool BreakAfterJavaFieldAnnotations;












bool BreakStringLiterals;






unsigned ColumnLimit;








std::string CommentPragmas;


enum BreakInheritanceListStyle : unsigned char {







BILS_BeforeColon,








BILS_BeforeComma,







BILS_AfterColon,






BILS_AfterComma,
};


BreakInheritanceListStyle BreakInheritanceList;






















bool CompactNamespaces;


















bool ConstructorInitializerAllOnOneLineOrOnePerLine;




unsigned ConstructorInitializerIndentWidth;









unsigned ContinuationIndentWidth;





















bool Cpp11BracedListStyle;



bool DeriveLineEnding;






bool DerivePointerAlignment;


bool DisableFormat;




enum EmptyLineAfterAccessModifierStyle : unsigned char {














ELAAMS_Never,


ELAAMS_Leave,




















ELAAMS_Always,
};




EmptyLineAfterAccessModifierStyle EmptyLineAfterAccessModifier;


enum EmptyLineBeforeAccessModifierStyle : unsigned char {














ELBAMS_Never,

ELBAMS_Leave,

















ELBAMS_LogicalBlock,



















ELBAMS_Always,
};


EmptyLineBeforeAccessModifierStyle EmptyLineBeforeAccessModifier;











bool ExperimentalAutoDetectBinPacking;











bool FixNamespaceComments;
















std::vector<std::string> ForEachMacros;



















std::vector<std::string> IfMacros;















std::vector<std::string> TypenameMacros;









std::vector<std::string> StatementMacros;











std::vector<std::string> NamespaceMacros;















std::vector<std::string> WhitespaceSensitiveMacros;

tooling::IncludeStyle IncludeStyle;

























bool IndentAccessModifiers;

















bool IndentCaseLabels;





















bool IndentCaseBlocks;















bool IndentGotoLabels;


enum PPDirectiveIndentStyle : unsigned char {








PPDIS_None,








PPDIS_AfterHash,








PPDIS_BeforeHash
};


PPDirectiveIndentStyle IndentPPDirectives;


enum IndentExternBlockStyle : unsigned char {

















IEBS_AfterExternBlock,






IEBS_NoIndent,






IEBS_Indent,
};


IndentExternBlockStyle IndentExternBlock;

















bool IndentRequires;












unsigned IndentWidth;












bool IndentWrappedFunctionNames;
































std::vector<std::string> JavaImportGroups;



enum JavaScriptQuoteStyle : unsigned char {





JSQS_Leave,





JSQS_Single,





JSQS_Double
};


JavaScriptQuoteStyle JavaScriptQuotes;














bool JavaScriptWrapImports;










bool KeepEmptyLinesAtTheStartOfBlocks;






enum LanguageKind : unsigned char {

LK_None,

LK_Cpp,

LK_CSharp,

LK_Java,

LK_JavaScript,

LK_Json,

LK_ObjC,


LK_Proto,

LK_TableGen,


LK_TextProto
};
bool isCpp() const { return Language == LK_Cpp || Language == LK_ObjC; }
bool isCSharp() const { return Language == LK_CSharp; }
bool isJson() const { return Language == LK_Json; }


LanguageKind Language;


enum LambdaBodyIndentationKind : unsigned char {







LBI_Signature,








LBI_OuterScope,
};










LambdaBodyIndentationKind LambdaBodyIndentation;



























std::string MacroBlockBegin;


std::string MacroBlockEnd;












unsigned MaxEmptyLinesToKeep;


enum NamespaceIndentationKind : unsigned char {









NI_None,









NI_Inner,









NI_All
};


NamespaceIndentationKind NamespaceIndentation;































BinPackStyle ObjCBinPackProtocolList;









unsigned ObjCBlockIndentWidth;



bool ObjCSpaceAfterProperty;






















bool ObjCBreakBeforeNestedBlockParam;



bool ObjCSpaceBeforeProtocolList;


unsigned PenaltyBreakAssignment;


unsigned PenaltyBreakBeforeFirstCallParameter;


unsigned PenaltyBreakComment;


unsigned PenaltyBreakFirstLessLess;


unsigned PenaltyBreakString;


unsigned PenaltyBreakTemplateDeclaration;


unsigned PenaltyExcessCharacter;



unsigned PenaltyReturnTypeOnItsOwnLine;



unsigned PenaltyIndentedWhitespace;


enum PointerAlignmentStyle : unsigned char {




PAS_Left,




PAS_Right,




PAS_Middle
};


PointerAlignmentStyle PointerAlignment;













int PPIndentWidth;


struct RawStringFormat {

LanguageKind Language;

std::vector<std::string> Delimiters;

std::vector<std::string> EnclosingFunctions;

std::string CanonicalDelimiter;



std::string BasedOnStyle;
bool operator==(const RawStringFormat &Other) const {
return Language == Other.Language && Delimiters == Other.Delimiters &&
EnclosingFunctions == Other.EnclosingFunctions &&
CanonicalDelimiter == Other.CanonicalDelimiter &&
BasedOnStyle == Other.BasedOnStyle;
}
};




































std::vector<RawStringFormat> RawStringFormats;


enum ReferenceAlignmentStyle {

RAS_Pointer,




RAS_Left,




RAS_Right,




RAS_Middle
};



ReferenceAlignmentStyle ReferenceAlignment;














bool ReflowComments;





















unsigned ShortNamespaceLines;


enum SortIncludesOptions : unsigned char {








SI_Never,








SI_CaseSensitive,








SI_CaseInsensitive,
};







SortIncludesOptions SortIncludes;


enum SortJavaStaticImportOptions : unsigned char {






SJSIO_Before,






SJSIO_After,
};




SortJavaStaticImportOptions SortJavaStaticImport;















bool SortUsingDeclarations;






bool SpaceAfterCStyleCast;






bool SpaceAfterLogicalNot;






bool SpaceAfterTemplateKeyword;


enum SpaceAroundPointerQualifiersStyle : unsigned char {






SAPQ_Default,





SAPQ_Before,





SAPQ_After,





SAPQ_Both,
};


SpaceAroundPointerQualifiersStyle SpaceAroundPointerQualifiers;







bool SpaceBeforeAssignmentOperators;








bool SpaceBeforeCaseColon;










bool SpaceBeforeCpp11BracedList;







bool SpaceBeforeCtorInitializerColon;






bool SpaceBeforeInheritanceColon;


enum SpaceBeforeParensOptions : unsigned char {








SBPO_Never,









SBPO_ControlStatements,












SBPO_ControlStatementsExceptControlMacros,










SBPO_NonEmptyParentheses,











SBPO_Always
};


SpaceBeforeParensOptions SpaceBeforeParens;







bool SpaceBeforeRangeBasedForLoopColon;







bool SpaceInEmptyBlock;











bool SpaceInEmptyParentheses;















unsigned SpacesBeforeTrailingComments;



enum SpacesInAnglesStyle : unsigned char {





SIAS_Never,





SIAS_Always,


SIAS_Leave
};

SpacesInAnglesStyle SpacesInAngles;








bool SpacesInConditionalStatement;








bool SpacesInContainerLiterals;






bool SpacesInCStyleCastParentheses;


struct SpacesInLineComment {

unsigned Minimum;

unsigned Maximum;
};




























SpacesInLineComment SpacesInLineCommentPrefix;






bool SpacesInParentheses;









bool SpacesInSquareBrackets;








bool SpaceBeforeSquareBrackets;


enum BitFieldColonSpacingStyle : unsigned char {




BFCS_Both,





BFCS_None,




BFCS_Before,





BFCS_After
};

BitFieldColonSpacingStyle BitFieldColonSpacing;









enum LanguageStandard : unsigned char {


LS_Cpp03,

LS_Cpp11,

LS_Cpp14,

LS_Cpp17,

LS_Cpp20,


LS_Latest,

LS_Auto,
};






LanguageStandard Standard;















std::vector<std::string> StatementAttributeLikeMacros;


unsigned TabWidth;


enum UseTabStyle : unsigned char {

UT_Never,

UT_ForIndentation,


UT_ForContinuationAndIndentation,


UT_AlignWithSpaces,


UT_Always
};



bool UseCRLF;


UseTabStyle UseTab;

bool operator==(const FormatStyle &R) const {
return AccessModifierOffset == R.AccessModifierOffset &&
AlignAfterOpenBracket == R.AlignAfterOpenBracket &&
AlignArrayOfStructures == R.AlignArrayOfStructures &&
AlignConsecutiveAssignments == R.AlignConsecutiveAssignments &&
AlignConsecutiveBitFields == R.AlignConsecutiveBitFields &&
AlignConsecutiveDeclarations == R.AlignConsecutiveDeclarations &&
AlignConsecutiveMacros == R.AlignConsecutiveMacros &&
AlignEscapedNewlines == R.AlignEscapedNewlines &&
AlignOperands == R.AlignOperands &&
AlignTrailingComments == R.AlignTrailingComments &&
AllowAllArgumentsOnNextLine == R.AllowAllArgumentsOnNextLine &&
AllowAllConstructorInitializersOnNextLine ==
R.AllowAllConstructorInitializersOnNextLine &&
AllowAllParametersOfDeclarationOnNextLine ==
R.AllowAllParametersOfDeclarationOnNextLine &&
AllowShortEnumsOnASingleLine == R.AllowShortEnumsOnASingleLine &&
AllowShortBlocksOnASingleLine == R.AllowShortBlocksOnASingleLine &&
AllowShortCaseLabelsOnASingleLine ==
R.AllowShortCaseLabelsOnASingleLine &&
AllowShortFunctionsOnASingleLine ==
R.AllowShortFunctionsOnASingleLine &&
AllowShortIfStatementsOnASingleLine ==
R.AllowShortIfStatementsOnASingleLine &&
AllowShortLambdasOnASingleLine == R.AllowShortLambdasOnASingleLine &&
AllowShortLoopsOnASingleLine == R.AllowShortLoopsOnASingleLine &&
AlwaysBreakAfterReturnType == R.AlwaysBreakAfterReturnType &&
AlwaysBreakBeforeMultilineStrings ==
R.AlwaysBreakBeforeMultilineStrings &&
AlwaysBreakTemplateDeclarations ==
R.AlwaysBreakTemplateDeclarations &&
AttributeMacros == R.AttributeMacros &&
BinPackArguments == R.BinPackArguments &&
BinPackParameters == R.BinPackParameters &&
BreakBeforeBinaryOperators == R.BreakBeforeBinaryOperators &&
BreakBeforeBraces == R.BreakBeforeBraces &&
BreakBeforeConceptDeclarations == R.BreakBeforeConceptDeclarations &&
BreakBeforeTernaryOperators == R.BreakBeforeTernaryOperators &&
BreakConstructorInitializers == R.BreakConstructorInitializers &&
CompactNamespaces == R.CompactNamespaces &&
BreakAfterJavaFieldAnnotations == R.BreakAfterJavaFieldAnnotations &&
BreakStringLiterals == R.BreakStringLiterals &&
ColumnLimit == R.ColumnLimit && CommentPragmas == R.CommentPragmas &&
BreakInheritanceList == R.BreakInheritanceList &&
ConstructorInitializerAllOnOneLineOrOnePerLine ==
R.ConstructorInitializerAllOnOneLineOrOnePerLine &&
ConstructorInitializerIndentWidth ==
R.ConstructorInitializerIndentWidth &&
ContinuationIndentWidth == R.ContinuationIndentWidth &&
Cpp11BracedListStyle == R.Cpp11BracedListStyle &&
DeriveLineEnding == R.DeriveLineEnding &&
DerivePointerAlignment == R.DerivePointerAlignment &&
DisableFormat == R.DisableFormat &&
EmptyLineAfterAccessModifier == R.EmptyLineAfterAccessModifier &&
EmptyLineBeforeAccessModifier == R.EmptyLineBeforeAccessModifier &&
ExperimentalAutoDetectBinPacking ==
R.ExperimentalAutoDetectBinPacking &&
FixNamespaceComments == R.FixNamespaceComments &&
ForEachMacros == R.ForEachMacros &&
IncludeStyle.IncludeBlocks == R.IncludeStyle.IncludeBlocks &&
IncludeStyle.IncludeCategories == R.IncludeStyle.IncludeCategories &&
IncludeStyle.IncludeIsMainRegex ==
R.IncludeStyle.IncludeIsMainRegex &&
IncludeStyle.IncludeIsMainSourceRegex ==
R.IncludeStyle.IncludeIsMainSourceRegex &&
IndentAccessModifiers == R.IndentAccessModifiers &&
IndentCaseLabels == R.IndentCaseLabels &&
IndentCaseBlocks == R.IndentCaseBlocks &&
IndentGotoLabels == R.IndentGotoLabels &&
IndentPPDirectives == R.IndentPPDirectives &&
IndentExternBlock == R.IndentExternBlock &&
IndentRequires == R.IndentRequires && IndentWidth == R.IndentWidth &&
Language == R.Language &&
IndentWrappedFunctionNames == R.IndentWrappedFunctionNames &&
JavaImportGroups == R.JavaImportGroups &&
JavaScriptQuotes == R.JavaScriptQuotes &&
JavaScriptWrapImports == R.JavaScriptWrapImports &&
KeepEmptyLinesAtTheStartOfBlocks ==
R.KeepEmptyLinesAtTheStartOfBlocks &&
LambdaBodyIndentation == R.LambdaBodyIndentation &&
MacroBlockBegin == R.MacroBlockBegin &&
MacroBlockEnd == R.MacroBlockEnd &&
MaxEmptyLinesToKeep == R.MaxEmptyLinesToKeep &&
NamespaceIndentation == R.NamespaceIndentation &&
NamespaceMacros == R.NamespaceMacros &&
ObjCBinPackProtocolList == R.ObjCBinPackProtocolList &&
ObjCBlockIndentWidth == R.ObjCBlockIndentWidth &&
ObjCBreakBeforeNestedBlockParam ==
R.ObjCBreakBeforeNestedBlockParam &&
ObjCSpaceAfterProperty == R.ObjCSpaceAfterProperty &&
ObjCSpaceBeforeProtocolList == R.ObjCSpaceBeforeProtocolList &&
PenaltyBreakAssignment == R.PenaltyBreakAssignment &&
PenaltyBreakBeforeFirstCallParameter ==
R.PenaltyBreakBeforeFirstCallParameter &&
PenaltyBreakComment == R.PenaltyBreakComment &&
PenaltyBreakFirstLessLess == R.PenaltyBreakFirstLessLess &&
PenaltyBreakString == R.PenaltyBreakString &&
PenaltyExcessCharacter == R.PenaltyExcessCharacter &&
PenaltyReturnTypeOnItsOwnLine == R.PenaltyReturnTypeOnItsOwnLine &&
PenaltyBreakTemplateDeclaration ==
R.PenaltyBreakTemplateDeclaration &&
PointerAlignment == R.PointerAlignment &&
RawStringFormats == R.RawStringFormats &&
ReferenceAlignment == R.ReferenceAlignment &&
ShortNamespaceLines == R.ShortNamespaceLines &&
SortIncludes == R.SortIncludes &&
SortJavaStaticImport == R.SortJavaStaticImport &&
SpaceAfterCStyleCast == R.SpaceAfterCStyleCast &&
SpaceAfterLogicalNot == R.SpaceAfterLogicalNot &&
SpaceAfterTemplateKeyword == R.SpaceAfterTemplateKeyword &&
SpaceBeforeAssignmentOperators == R.SpaceBeforeAssignmentOperators &&
SpaceBeforeCaseColon == R.SpaceBeforeCaseColon &&
SpaceBeforeCpp11BracedList == R.SpaceBeforeCpp11BracedList &&
SpaceBeforeCtorInitializerColon ==
R.SpaceBeforeCtorInitializerColon &&
SpaceBeforeInheritanceColon == R.SpaceBeforeInheritanceColon &&
SpaceBeforeParens == R.SpaceBeforeParens &&
SpaceAroundPointerQualifiers == R.SpaceAroundPointerQualifiers &&
SpaceBeforeRangeBasedForLoopColon ==
R.SpaceBeforeRangeBasedForLoopColon &&
SpaceInEmptyBlock == R.SpaceInEmptyBlock &&
SpaceInEmptyParentheses == R.SpaceInEmptyParentheses &&
SpacesBeforeTrailingComments == R.SpacesBeforeTrailingComments &&
SpacesInAngles == R.SpacesInAngles &&
SpacesInConditionalStatement == R.SpacesInConditionalStatement &&
SpacesInContainerLiterals == R.SpacesInContainerLiterals &&
SpacesInCStyleCastParentheses == R.SpacesInCStyleCastParentheses &&
SpacesInLineCommentPrefix.Minimum ==
R.SpacesInLineCommentPrefix.Minimum &&
SpacesInLineCommentPrefix.Maximum ==
R.SpacesInLineCommentPrefix.Maximum &&
SpacesInParentheses == R.SpacesInParentheses &&
SpacesInSquareBrackets == R.SpacesInSquareBrackets &&
SpaceBeforeSquareBrackets == R.SpaceBeforeSquareBrackets &&
BitFieldColonSpacing == R.BitFieldColonSpacing &&
Standard == R.Standard &&
StatementAttributeLikeMacros == R.StatementAttributeLikeMacros &&
StatementMacros == R.StatementMacros && TabWidth == R.TabWidth &&
UseTab == R.UseTab && UseCRLF == R.UseCRLF &&
TypenameMacros == R.TypenameMacros;
}

llvm::Optional<FormatStyle> GetLanguageStyle(LanguageKind Language) const;








struct FormatStyleSet {
typedef std::map<FormatStyle::LanguageKind, FormatStyle> MapType;

llvm::Optional<FormatStyle> Get(FormatStyle::LanguageKind Language) const;






void Add(FormatStyle Style);


void Clear();

private:
std::shared_ptr<MapType> Styles;
};

static FormatStyleSet BuildStyleSetFromConfiguration(
const FormatStyle &MainStyle,
const std::vector<FormatStyle> &ConfigurationStyles);

private:
FormatStyleSet StyleSet;

friend std::error_code
parseConfiguration(llvm::MemoryBufferRef Config, FormatStyle *Style,
bool AllowUnknownOptions,
llvm::SourceMgr::DiagHandlerTy DiagHandler,
void *DiagHandlerCtxt);
};



FormatStyle getLLVMStyle(
FormatStyle::LanguageKind Language = FormatStyle::LanguageKind::LK_Cpp);





FormatStyle getGoogleStyle(FormatStyle::LanguageKind Language);



FormatStyle getChromiumStyle(FormatStyle::LanguageKind Language);



FormatStyle getMozillaStyle();



FormatStyle getWebKitStyle();



FormatStyle getGNUStyle();



FormatStyle getMicrosoftStyle(FormatStyle::LanguageKind Language);


FormatStyle getNoStyle();







bool getPredefinedStyle(StringRef Name, FormatStyle::LanguageKind Language,
FormatStyle *Style);















std::error_code
parseConfiguration(llvm::MemoryBufferRef Config, FormatStyle *Style,
bool AllowUnknownOptions = false,
llvm::SourceMgr::DiagHandlerTy DiagHandler = nullptr,
void *DiagHandlerCtx = nullptr);


inline std::error_code parseConfiguration(StringRef Config, FormatStyle *Style,
bool AllowUnknownOptions = false) {
return parseConfiguration(llvm::MemoryBufferRef(Config, "YAML"), Style,
AllowUnknownOptions);
}


std::string configurationAsText(const FormatStyle &Style);



tooling::Replacements sortIncludes(const FormatStyle &Style, StringRef Code,
ArrayRef<tooling::Range> Ranges,
StringRef FileName,
unsigned *Cursor = nullptr);




llvm::Expected<tooling::Replacements>
formatReplacements(StringRef Code, const tooling::Replacements &Replaces,
const FormatStyle &Style);














llvm::Expected<tooling::Replacements>
cleanupAroundReplacements(StringRef Code, const tooling::Replacements &Replaces,
const FormatStyle &Style);


struct FormattingAttemptStatus {


bool FormatComplete = true;




unsigned Line = 0;
};












tooling::Replacements reformat(const FormatStyle &Style, StringRef Code,
ArrayRef<tooling::Range> Ranges,
StringRef FileName = "<stdin>",
FormattingAttemptStatus *Status = nullptr);




tooling::Replacements reformat(const FormatStyle &Style, StringRef Code,
ArrayRef<tooling::Range> Ranges,
StringRef FileName, bool *IncompleteFormat);





tooling::Replacements cleanup(const FormatStyle &Style, StringRef Code,
ArrayRef<tooling::Range> Ranges,
StringRef FileName = "<stdin>");





tooling::Replacements fixNamespaceEndComments(const FormatStyle &Style,
StringRef Code,
ArrayRef<tooling::Range> Ranges,
StringRef FileName = "<stdin>");






tooling::Replacements sortUsingDeclarations(const FormatStyle &Style,
StringRef Code,
ArrayRef<tooling::Range> Ranges,
StringRef FileName = "<stdin>");




LangOptions getFormattingLangOpts(const FormatStyle &Style = getLLVMStyle());




extern const char *StyleOptionHelpDescription;




extern const char *DefaultFormatStyle;



extern const char *DefaultFallbackStyle;




























llvm::Expected<FormatStyle> getStyle(StringRef StyleName, StringRef FileName,
StringRef FallbackStyle,
StringRef Code = "",
llvm::vfs::FileSystem *FS = nullptr,
bool AllowUnknownOptions = false);



FormatStyle::LanguageKind guessLanguage(StringRef FileName, StringRef Code);


inline StringRef getLanguageName(FormatStyle::LanguageKind Language) {
switch (Language) {
case FormatStyle::LK_Cpp:
return "C++";
case FormatStyle::LK_CSharp:
return "CSharp";
case FormatStyle::LK_ObjC:
return "Objective-C";
case FormatStyle::LK_Java:
return "Java";
case FormatStyle::LK_JavaScript:
return "JavaScript";
case FormatStyle::LK_Json:
return "Json";
case FormatStyle::LK_Proto:
return "Proto";
case FormatStyle::LK_TableGen:
return "TableGen";
case FormatStyle::LK_TextProto:
return "TextProto";
default:
return "Unknown";
}
}

}
}

namespace std {
template <>
struct is_error_code_enum<clang::format::ParseError> : std::true_type {};
}

#endif
