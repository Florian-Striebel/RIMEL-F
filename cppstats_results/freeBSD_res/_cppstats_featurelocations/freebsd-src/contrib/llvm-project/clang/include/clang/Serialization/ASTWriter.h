












#if !defined(LLVM_CLANG_SERIALIZATION_ASTWRITER_H)
#define LLVM_CLANG_SERIALIZATION_ASTWRITER_H

#include "clang/AST/ASTMutationListener.h"
#include "clang/AST/Decl.h"
#include "clang/AST/Type.h"
#include "clang/Basic/LLVM.h"
#include "clang/Basic/SourceLocation.h"
#include "clang/Sema/Sema.h"
#include "clang/Sema/SemaConsumer.h"
#include "clang/Serialization/ASTBitCodes.h"
#include "clang/Serialization/ASTDeserializationListener.h"
#include "clang/Serialization/PCHContainerOperations.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/DenseSet.h"
#include "llvm/ADT/MapVector.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/SetVector.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Bitstream/BitstreamWriter.h"
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <ctime>
#include <memory>
#include <queue>
#include <string>
#include <utility>
#include <vector>

namespace llvm {

class APFloat;
class APInt;
class APSInt;

}

namespace clang {

class ASTContext;
class ASTReader;
class ASTUnresolvedSet;
class Attr;
class CXXBaseSpecifier;
class CXXCtorInitializer;
class CXXRecordDecl;
class CXXTemporary;
class FileEntry;
class FPOptions;
class FPOptionsOverride;
class FunctionDecl;
class HeaderSearch;
class HeaderSearchOptions;
class IdentifierResolver;
class LangOptions;
class MacroDefinitionRecord;
class MacroInfo;
class Module;
class InMemoryModuleCache;
class ModuleFileExtension;
class ModuleFileExtensionWriter;
class NamedDecl;
class ObjCInterfaceDecl;
class PreprocessingRecord;
class Preprocessor;
struct QualifierInfo;
class RecordDecl;
class Sema;
class SourceManager;
class Stmt;
class StoredDeclsList;
class SwitchCase;
class TemplateParameterList;
class Token;
class TypeSourceInfo;







class ASTWriter : public ASTDeserializationListener,
public ASTMutationListener {
public:
friend class ASTDeclWriter;
friend class ASTRecordWriter;

using RecordData = SmallVector<uint64_t, 64>;
using RecordDataImpl = SmallVectorImpl<uint64_t>;
using RecordDataRef = ArrayRef<uint64_t>;

private:









using TypeIdxMap = llvm::DenseMap<QualType, serialization::TypeIdx,
serialization::UnsafeQualTypeDenseMapInfo>;


llvm::BitstreamWriter &Stream;


const SmallVectorImpl<char> &Buffer;


InMemoryModuleCache &ModuleCache;


ASTContext *Context = nullptr;


Preprocessor *PP = nullptr;


ASTReader *Chain = nullptr;


Module *WritingModule = nullptr;


uint64_t ASTBlockStartOffset = 0;


std::pair<uint64_t, uint64_t> ASTBlockRange;


std::string BaseDirectory;





bool IncludeTimestamps;



bool WritingAST = false;



bool DoneWritingDeclsAndTypes = false;


bool ASTHasCompilerErrors = false;



llvm::DenseMap<const FileEntry *, uint32_t> InputFileIDs;


class DeclOrType {
public:
DeclOrType(Decl *D) : Stored(D), IsType(false) {}
DeclOrType(QualType T) : Stored(T.getAsOpaquePtr()), IsType(true) {}

bool isType() const { return IsType; }
bool isDecl() const { return !IsType; }

QualType getType() const {
assert(isType() && "Not a type!");
return QualType::getFromOpaquePtr(Stored);
}

Decl *getDecl() const {
assert(isDecl() && "Not a decl!");
return static_cast<Decl *>(Stored);
}

private:
void *Stored;
bool IsType;
};


std::queue<DeclOrType> DeclTypesToEmit;


serialization::DeclID FirstDeclID = serialization::NUM_PREDEF_DECL_IDS;


serialization::DeclID NextDeclID = FirstDeclID;







llvm::DenseMap<const Decl *, serialization::DeclID> DeclIDs;



std::vector<serialization::DeclOffset> DeclOffsets;



uint64_t DeclTypesBlockStartOffset = 0;


using LocDeclIDsTy =
SmallVector<std::pair<unsigned, serialization::DeclID>, 64>;
struct DeclIDInFileInfo {
LocDeclIDsTy DeclIDs;



unsigned FirstDeclIndex;
};
using FileDeclIDsTy =
llvm::DenseMap<FileID, std::unique_ptr<DeclIDInFileInfo>>;



FileDeclIDsTy FileDeclIDs;

void associateDeclWithFile(const Decl *D, serialization::DeclID);


serialization::TypeID FirstTypeID = serialization::NUM_PREDEF_TYPE_IDS;


serialization::TypeID NextTypeID = FirstTypeID;










TypeIdxMap TypeIdxs;



std::vector<serialization::UnderalignedInt64> TypeOffsets;


serialization::IdentID FirstIdentID = serialization::NUM_PREDEF_IDENT_IDS;


serialization::IdentID NextIdentID = FirstIdentID;







llvm::MapVector<const IdentifierInfo *, serialization::IdentID> IdentifierIDs;


serialization::MacroID FirstMacroID = serialization::NUM_PREDEF_MACRO_IDS;


serialization::MacroID NextMacroID = FirstMacroID;


llvm::DenseMap<MacroInfo *, serialization::MacroID> MacroIDs;

struct MacroInfoToEmitData {
const IdentifierInfo *Name;
MacroInfo *MI;
serialization::MacroID ID;
};


std::vector<MacroInfoToEmitData> MacroInfosToEmit;

llvm::DenseMap<const IdentifierInfo *, uint32_t>
IdentMacroDirectivesOffsetMap;





llvm::DenseSet<Stmt *> ParentStmts;



llvm::DenseMap<Stmt *, uint64_t> SubStmtEntries;





std::vector<uint32_t> IdentifierOffsets;


serialization::SubmoduleID FirstSubmoduleID =
serialization::NUM_PREDEF_SUBMODULE_IDS;


serialization::SubmoduleID NextSubmoduleID = FirstSubmoduleID;


serialization::SelectorID FirstSelectorID =
serialization::NUM_PREDEF_SELECTOR_IDS;


serialization::SelectorID NextSelectorID = FirstSelectorID;


llvm::MapVector<Selector, serialization::SelectorID> SelectorIDs;



std::vector<uint32_t> SelectorOffsets;



llvm::DenseMap<const MacroDefinitionRecord *,
serialization::PreprocessedEntityID> MacroDefinitions;



llvm::DenseMap<const Decl *, unsigned> AnonymousDeclarationNumbers;


class DeclUpdate {

unsigned Kind;
union {
const Decl *Dcl;
void *Type;
SourceLocation::UIntTy Loc;
unsigned Val;
Module *Mod;
const Attr *Attribute;
};

public:
DeclUpdate(unsigned Kind) : Kind(Kind), Dcl(nullptr) {}
DeclUpdate(unsigned Kind, const Decl *Dcl) : Kind(Kind), Dcl(Dcl) {}
DeclUpdate(unsigned Kind, QualType Type)
: Kind(Kind), Type(Type.getAsOpaquePtr()) {}
DeclUpdate(unsigned Kind, SourceLocation Loc)
: Kind(Kind), Loc(Loc.getRawEncoding()) {}
DeclUpdate(unsigned Kind, unsigned Val) : Kind(Kind), Val(Val) {}
DeclUpdate(unsigned Kind, Module *M) : Kind(Kind), Mod(M) {}
DeclUpdate(unsigned Kind, const Attr *Attribute)
: Kind(Kind), Attribute(Attribute) {}

unsigned getKind() const { return Kind; }
const Decl *getDecl() const { return Dcl; }
QualType getType() const { return QualType::getFromOpaquePtr(Type); }

SourceLocation getLoc() const {
return SourceLocation::getFromRawEncoding(Loc);
}

unsigned getNumber() const { return Val; }
Module *getModule() const { return Mod; }
const Attr *getAttr() const { return Attribute; }
};

using UpdateRecord = SmallVector<DeclUpdate, 1>;
using DeclUpdateMap = llvm::MapVector<const Decl *, UpdateRecord>;



DeclUpdateMap DeclUpdates;

using FirstLatestDeclMap = llvm::DenseMap<Decl *, Decl *>;



FirstLatestDeclMap FirstLatestDecls;












SmallVector<serialization::DeclID, 16> EagerlyDeserializedDecls;
SmallVector<serialization::DeclID, 16> ModularCodegenDecls;








llvm::SmallSetVector<const DeclContext *, 16> UpdatedDeclContexts;




SmallVector<const Decl *, 16> DeclsToEmitEvenIfUnreferenced;



llvm::SetVector<ObjCInterfaceDecl *> ObjCClassesWithCategories;



llvm::SmallVector<const Decl *, 16> Redeclarations;



llvm::DenseMap<const Decl *, const Decl *> FirstLocalDeclCache;


llvm::DenseMap<SwitchCase *, unsigned> SwitchCaseIDs;


unsigned NumStatements = 0;


unsigned NumMacros = 0;



unsigned NumLexicalDeclContexts = 0;



unsigned NumVisibleDeclContexts = 0;



llvm::DenseMap<const Module *, unsigned> SubmoduleIDs;


std::vector<std::unique_ptr<ModuleFileExtensionWriter>>
ModuleFileExtensionWriters;


unsigned getSubmoduleID(Module *Mod);


void WriteSubStmt(Stmt *S);

void WriteBlockInfoBlock();
void WriteControlBlock(Preprocessor &PP, ASTContext &Context,
StringRef isysroot, const std::string &OutputFile);


ASTFileSignature writeUnhashedControlBlock(Preprocessor &PP,
ASTContext &Context);


static std::pair<ASTFileSignature, ASTFileSignature>
createSignature(StringRef AllBytes, StringRef ASTBlockBytes);

void WriteInputFiles(SourceManager &SourceMgr, HeaderSearchOptions &HSOpts,
bool Modules);
void WriteSourceManagerBlock(SourceManager &SourceMgr,
const Preprocessor &PP);
void WritePreprocessor(const Preprocessor &PP, bool IsModule);
void WriteHeaderSearch(const HeaderSearch &HS);
void WritePreprocessorDetail(PreprocessingRecord &PPRec,
uint64_t MacroOffsetsBase);
void WriteSubmodules(Module *WritingModule);

void WritePragmaDiagnosticMappings(const DiagnosticsEngine &Diag,
bool isModule);

unsigned TypeExtQualAbbrev = 0;
unsigned TypeFunctionProtoAbbrev = 0;
void WriteTypeAbbrevs();
void WriteType(QualType T);

bool isLookupResultExternal(StoredDeclsList &Result, DeclContext *DC);
bool isLookupResultEntirelyExternal(StoredDeclsList &Result, DeclContext *DC);

void GenerateNameLookupTable(const DeclContext *DC,
llvm::SmallVectorImpl<char> &LookupTable);
uint64_t WriteDeclContextLexicalBlock(ASTContext &Context, DeclContext *DC);
uint64_t WriteDeclContextVisibleBlock(ASTContext &Context, DeclContext *DC);
void WriteTypeDeclOffsets();
void WriteFileDeclIDsMap();
void WriteComments();
void WriteSelectors(Sema &SemaRef);
void WriteReferencedSelectorsPool(Sema &SemaRef);
void WriteIdentifierTable(Preprocessor &PP, IdentifierResolver &IdResolver,
bool IsModule);
void WriteDeclUpdatesBlocks(RecordDataImpl &OffsetsRecord);
void WriteDeclContextVisibleUpdate(const DeclContext *DC);
void WriteFPPragmaOptions(const FPOptionsOverride &Opts);
void WriteOpenCLExtensions(Sema &SemaRef);
void WriteCUDAPragmas(Sema &SemaRef);
void WriteObjCCategories();
void WriteLateParsedTemplates(Sema &SemaRef);
void WriteOptimizePragmaOptions(Sema &SemaRef);
void WriteMSStructPragmaOptions(Sema &SemaRef);
void WriteMSPointersToMembersPragmaOptions(Sema &SemaRef);
void WritePackPragmaOptions(Sema &SemaRef);
void WriteFloatControlPragmaOptions(Sema &SemaRef);
void WriteModuleFileExtension(Sema &SemaRef,
ModuleFileExtensionWriter &Writer);

unsigned DeclParmVarAbbrev = 0;
unsigned DeclContextLexicalAbbrev = 0;
unsigned DeclContextVisibleLookupAbbrev = 0;
unsigned UpdateVisibleAbbrev = 0;
unsigned DeclRecordAbbrev = 0;
unsigned DeclTypedefAbbrev = 0;
unsigned DeclVarAbbrev = 0;
unsigned DeclFieldAbbrev = 0;
unsigned DeclEnumAbbrev = 0;
unsigned DeclObjCIvarAbbrev = 0;
unsigned DeclCXXMethodAbbrev = 0;

unsigned DeclRefExprAbbrev = 0;
unsigned CharacterLiteralAbbrev = 0;
unsigned IntegerLiteralAbbrev = 0;
unsigned ExprImplicitCastAbbrev = 0;

void WriteDeclAbbrevs();
void WriteDecl(ASTContext &Context, Decl *D);

ASTFileSignature WriteASTCore(Sema &SemaRef, StringRef isysroot,
const std::string &OutputFile,
Module *WritingModule);

public:


ASTWriter(llvm::BitstreamWriter &Stream, SmallVectorImpl<char> &Buffer,
InMemoryModuleCache &ModuleCache,
ArrayRef<std::shared_ptr<ModuleFileExtension>> Extensions,
bool IncludeTimestamps = true);
~ASTWriter() override;

ASTContext &getASTContext() const {
assert(Context && "requested AST context when not writing AST");
return *Context;
}

const LangOptions &getLangOpts() const;




time_t getTimestampForOutput(const FileEntry *E) const;















ASTFileSignature WriteAST(Sema &SemaRef, const std::string &OutputFile,
Module *WritingModule, StringRef isysroot,
bool hasErrors = false,
bool ShouldCacheASTInMemory = false);


void AddToken(const Token &Tok, RecordDataImpl &Record);


void AddAlignPackInfo(const Sema::AlignPackInfo &Info,
RecordDataImpl &Record);


void AddSourceLocation(SourceLocation Loc, RecordDataImpl &Record);


void AddSourceRange(SourceRange Range, RecordDataImpl &Record);


void AddIdentifierRef(const IdentifierInfo *II, RecordDataImpl &Record);


serialization::SelectorID getSelectorRef(Selector Sel);


serialization::IdentID getIdentifierRef(const IdentifierInfo *II);


serialization::MacroID getMacroRef(MacroInfo *MI, const IdentifierInfo *Name);


serialization::MacroID getMacroID(MacroInfo *MI);

uint32_t getMacroDirectivesOffset(const IdentifierInfo *Name);


void AddTypeRef(QualType T, RecordDataImpl &Record);


serialization::TypeID GetOrCreateTypeID(QualType T);


serialization::TypeID getTypeID(QualType T) const;



const Decl *getFirstLocalDecl(const Decl *D);




bool IsLocalDecl(const Decl *D) {
if (D->isFromASTFile())
return false;
auto I = DeclIDs.find(D);
return (I == DeclIDs.end() ||
I->second >= serialization::NUM_PREDEF_DECL_IDS);
};


void AddDeclRef(const Decl *D, RecordDataImpl &Record);


serialization::DeclID GetDeclRef(const Decl *D);



serialization::DeclID getDeclID(const Decl *D);

unsigned getAnonymousDeclarationNumber(const NamedDecl *D);


void AddString(StringRef Str, RecordDataImpl &Record);



bool PreparePathForOutput(SmallVectorImpl<char> &Path);


void AddPath(StringRef Path, RecordDataImpl &Record);


void EmitRecordWithPath(unsigned Abbrev, RecordDataRef Record,
StringRef Path);


void AddVersionTuple(const VersionTuple &Version, RecordDataImpl &Record);




unsigned getLocalOrImportedSubmoduleID(const Module *Mod);



void SetIdentifierOffset(const IdentifierInfo *II, uint32_t Offset);



void SetSelectorOffset(Selector Sel, uint32_t Offset);


unsigned RecordSwitchCaseID(SwitchCase *S);


unsigned getSwitchCaseID(SwitchCase *S);

void ClearSwitchCaseIDs();

unsigned getTypeExtQualAbbrev() const {
return TypeExtQualAbbrev;
}

unsigned getTypeFunctionProtoAbbrev() const {
return TypeFunctionProtoAbbrev;
}

unsigned getDeclParmVarAbbrev() const { return DeclParmVarAbbrev; }
unsigned getDeclRecordAbbrev() const { return DeclRecordAbbrev; }
unsigned getDeclTypedefAbbrev() const { return DeclTypedefAbbrev; }
unsigned getDeclVarAbbrev() const { return DeclVarAbbrev; }
unsigned getDeclFieldAbbrev() const { return DeclFieldAbbrev; }
unsigned getDeclEnumAbbrev() const { return DeclEnumAbbrev; }
unsigned getDeclObjCIvarAbbrev() const { return DeclObjCIvarAbbrev; }
unsigned getDeclCXXMethodAbbrev() const { return DeclCXXMethodAbbrev; }

unsigned getDeclRefExprAbbrev() const { return DeclRefExprAbbrev; }
unsigned getCharacterLiteralAbbrev() const { return CharacterLiteralAbbrev; }
unsigned getIntegerLiteralAbbrev() const { return IntegerLiteralAbbrev; }
unsigned getExprImplicitCastAbbrev() const { return ExprImplicitCastAbbrev; }

bool hasChain() const { return Chain; }
ASTReader *getChain() const { return Chain; }

private:

void ReaderInitialized(ASTReader *Reader) override;
void IdentifierRead(serialization::IdentID ID, IdentifierInfo *II) override;
void MacroRead(serialization::MacroID ID, MacroInfo *MI) override;
void TypeRead(serialization::TypeIdx Idx, QualType T) override;
void SelectorRead(serialization::SelectorID ID, Selector Sel) override;
void MacroDefinitionRead(serialization::PreprocessedEntityID ID,
MacroDefinitionRecord *MD) override;
void ModuleRead(serialization::SubmoduleID ID, Module *Mod) override;


void CompletedTagDefinition(const TagDecl *D) override;
void AddedVisibleDecl(const DeclContext *DC, const Decl *D) override;
void AddedCXXImplicitMember(const CXXRecordDecl *RD, const Decl *D) override;
void AddedCXXTemplateSpecialization(
const ClassTemplateDecl *TD,
const ClassTemplateSpecializationDecl *D) override;
void AddedCXXTemplateSpecialization(
const VarTemplateDecl *TD,
const VarTemplateSpecializationDecl *D) override;
void AddedCXXTemplateSpecialization(const FunctionTemplateDecl *TD,
const FunctionDecl *D) override;
void ResolvedExceptionSpec(const FunctionDecl *FD) override;
void DeducedReturnType(const FunctionDecl *FD, QualType ReturnType) override;
void ResolvedOperatorDelete(const CXXDestructorDecl *DD,
const FunctionDecl *Delete,
Expr *ThisArg) override;
void CompletedImplicitDefinition(const FunctionDecl *D) override;
void InstantiationRequested(const ValueDecl *D) override;
void VariableDefinitionInstantiated(const VarDecl *D) override;
void FunctionDefinitionInstantiated(const FunctionDecl *D) override;
void DefaultArgumentInstantiated(const ParmVarDecl *D) override;
void DefaultMemberInitializerInstantiated(const FieldDecl *D) override;
void AddedObjCCategoryToInterface(const ObjCCategoryDecl *CatD,
const ObjCInterfaceDecl *IFD) override;
void DeclarationMarkedUsed(const Decl *D) override;
void DeclarationMarkedOpenMPThreadPrivate(const Decl *D) override;
void DeclarationMarkedOpenMPDeclareTarget(const Decl *D,
const Attr *Attr) override;
void DeclarationMarkedOpenMPAllocate(const Decl *D, const Attr *A) override;
void RedefinedHiddenDefinition(const NamedDecl *D, Module *M) override;
void AddedAttributeToRecord(const Attr *Attr,
const RecordDecl *Record) override;
};



class PCHGenerator : public SemaConsumer {
const Preprocessor &PP;
std::string OutputFile;
std::string isysroot;
Sema *SemaPtr;
std::shared_ptr<PCHBuffer> Buffer;
llvm::BitstreamWriter Stream;
ASTWriter Writer;
bool AllowASTWithErrors;
bool ShouldCacheASTInMemory;

protected:
ASTWriter &getWriter() { return Writer; }
const ASTWriter &getWriter() const { return Writer; }
SmallVectorImpl<char> &getPCH() const { return Buffer->Data; }

public:
PCHGenerator(const Preprocessor &PP, InMemoryModuleCache &ModuleCache,
StringRef OutputFile, StringRef isysroot,
std::shared_ptr<PCHBuffer> Buffer,
ArrayRef<std::shared_ptr<ModuleFileExtension>> Extensions,
bool AllowASTWithErrors = false, bool IncludeTimestamps = true,
bool ShouldCacheASTInMemory = false);
~PCHGenerator() override;

void InitializeSema(Sema &S) override { SemaPtr = &S; }
void HandleTranslationUnit(ASTContext &Ctx) override;
ASTMutationListener *GetASTMutationListener() override;
ASTDeserializationListener *GetASTDeserializationListener() override;
bool hasEmittedPCH() const { return Buffer->IsComplete; }
};

}

#endif
