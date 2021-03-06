











#if !defined(LLVM_CLANG_AST_EXTERNALASTMERGER_H)
#define LLVM_CLANG_AST_EXTERNALASTMERGER_H

#include "clang/AST/ASTImporter.h"
#include "clang/AST/ASTImporterSharedState.h"
#include "clang/AST/ExternalASTSource.h"
#include "llvm/Support/raw_ostream.h"

namespace clang {


























class ExternalASTMerger : public ExternalASTSource {
public:


struct DCOrigin {
DeclContext *DC;
ASTContext *AST;
};

typedef std::map<const DeclContext *, DCOrigin> OriginMap;
typedef std::vector<std::unique_ptr<ASTImporter>> ImporterVector;
private:

ImporterVector Importers;


OriginMap Origins;

llvm::raw_ostream *LogStream;

public:




struct ImporterTarget {
ASTContext &AST;
FileManager &FM;
};






class ImporterSource {
ASTContext &AST;
FileManager &FM;
const OriginMap &OM;


bool Temporary;


ExternalASTMerger *Merger;

public:
ImporterSource(ASTContext &AST, FileManager &FM, const OriginMap &OM,
bool Temporary = false, ExternalASTMerger *Merger = nullptr)
: AST(AST), FM(FM), OM(OM), Temporary(Temporary), Merger(Merger) {}
ASTContext &getASTContext() const { return AST; }
FileManager &getFileManager() const { return FM; }
const OriginMap &getOriginMap() const { return OM; }
bool isTemporary() const { return Temporary; }
ExternalASTMerger *getMerger() const { return Merger; }
};

private:

ImporterTarget Target;




std::shared_ptr<ASTImporterSharedState> SharedState;

public:
ExternalASTMerger(const ImporterTarget &Target,
llvm::ArrayRef<ImporterSource> Sources);





Decl *FindOriginalDecl(Decl *D);








void AddSources(llvm::ArrayRef<ImporterSource> Sources);








void RemoveSources(llvm::ArrayRef<ImporterSource> Sources);


bool FindExternalVisibleDeclsByName(const DeclContext *DC,
DeclarationName Name) override;


void
FindExternalLexicalDecls(const DeclContext *DC,
llvm::function_ref<bool(Decl::Kind)> IsKindWeWant,
SmallVectorImpl<Decl *> &Result) override;


void CompleteType(TagDecl *Tag) override;


void CompleteType(ObjCInterfaceDecl *Interface) override;


bool CanComplete(DeclContext *DC);



void MaybeRecordOrigin(const DeclContext *ToDC, DCOrigin Origin);


void ForceRecordOrigin(const DeclContext *ToDC, DCOrigin Origin);



const OriginMap &GetOrigins() { return Origins; }



bool HasImporterForOrigin(ASTContext &OriginContext);




ASTImporter &ImporterForOrigin(ASTContext &OriginContext);


void SetLogStream(llvm::raw_string_ostream &Stream) { LogStream = &Stream; }
private:

void RecordOriginImpl(const DeclContext *ToDC, DCOrigin Origin,
ASTImporter &importer);



template <typename CallbackType>
void ForEachMatchingDC(const DeclContext *DC, CallbackType Callback);

public:

llvm::raw_ostream &logs() { return *LogStream; }


bool LoggingEnabled() { return LogStream != &llvm::nulls(); }
};

}

#endif
