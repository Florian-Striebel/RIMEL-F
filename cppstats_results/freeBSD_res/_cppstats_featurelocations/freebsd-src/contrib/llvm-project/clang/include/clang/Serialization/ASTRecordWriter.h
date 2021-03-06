












#if !defined(LLVM_CLANG_SERIALIZATION_ASTRECORDWRITER_H)
#define LLVM_CLANG_SERIALIZATION_ASTRECORDWRITER_H

#include "clang/AST/AbstractBasicWriter.h"
#include "clang/AST/OpenMPClause.h"
#include "clang/Serialization/ASTWriter.h"

namespace clang {

class TypeLoc;


class ASTRecordWriter
: public serialization::DataStreamBasicWriter<ASTRecordWriter> {
ASTWriter *Writer;
ASTWriter::RecordDataImpl *Record;



SmallVector<Stmt *, 16> StmtsToEmit;




SmallVector<unsigned, 8> OffsetIndices;



void FlushStmts();
void FlushSubStmts();

void PrepareToEmit(uint64_t MyOffset) {

for (unsigned I : OffsetIndices) {
auto &StoredOffset = (*Record)[I];
assert(StoredOffset < MyOffset && "invalid offset");
if (StoredOffset)
StoredOffset = MyOffset - StoredOffset;
}
OffsetIndices.clear();
}

public:

ASTRecordWriter(ASTWriter &W, ASTWriter::RecordDataImpl &Record)
: DataStreamBasicWriter(W.getASTContext()), Writer(&W), Record(&Record) {}



ASTRecordWriter(ASTRecordWriter &Parent, ASTWriter::RecordDataImpl &Record)
: DataStreamBasicWriter(Parent.getASTContext()), Writer(Parent.Writer),
Record(&Record) {}


ASTRecordWriter(const ASTRecordWriter &) = delete;
ASTRecordWriter &operator=(const ASTRecordWriter &) = delete;


ASTWriter::RecordDataImpl &getRecordData() const { return *Record; }



void push_back(uint64_t N) { Record->push_back(N); }
template<typename InputIterator>
void append(InputIterator begin, InputIterator end) {
Record->append(begin, end);
}
bool empty() const { return Record->empty(); }
size_t size() const { return Record->size(); }
uint64_t &operator[](size_t N) { return (*Record)[N]; }





uint64_t Emit(unsigned Code, unsigned Abbrev = 0) {
uint64_t Offset = Writer->Stream.GetCurrentBitNo();
PrepareToEmit(Offset);
Writer->Stream.EmitRecord(Code, *Record, Abbrev);
FlushStmts();
return Offset;
}


uint64_t EmitStmt(unsigned Code, unsigned Abbrev = 0) {
FlushSubStmts();
PrepareToEmit(Writer->Stream.GetCurrentBitNo());
Writer->Stream.EmitRecord(Code, *Record, Abbrev);
return Writer->Stream.GetCurrentBitNo();
}



void AddOffset(uint64_t BitOffset) {
OffsetIndices.push_back(Record->size());
Record->push_back(BitOffset);
}








void AddStmt(Stmt *S) {
StmtsToEmit.push_back(S);
}
void writeStmtRef(const Stmt *S) {
AddStmt(const_cast<Stmt*>(S));
}



void AddFunctionDefinition(const FunctionDecl *FD);


void AddSourceLocation(SourceLocation Loc) {
return Writer->AddSourceLocation(Loc, *Record);
}
void writeSourceLocation(SourceLocation Loc) {
AddSourceLocation(Loc);
}


void AddSourceRange(SourceRange Range) {
return Writer->AddSourceRange(Range, *Record);
}

void writeBool(bool Value) {
Record->push_back(Value);
}

void writeUInt32(uint32_t Value) {
Record->push_back(Value);
}

void writeUInt64(uint64_t Value) {
Record->push_back(Value);
}


void AddAPInt(const llvm::APInt &Value) {
writeAPInt(Value);
}


void AddAPSInt(const llvm::APSInt &Value) {
writeAPSInt(Value);
}


void AddAPFloat(const llvm::APFloat &Value);


void AddAPValue(const APValue &Value) { writeAPValue(Value); }


void AddIdentifierRef(const IdentifierInfo *II) {
return Writer->AddIdentifierRef(II, *Record);
}
void writeIdentifier(const IdentifierInfo *II) {
AddIdentifierRef(II);
}


void AddSelectorRef(Selector S);
void writeSelector(Selector sel) {
AddSelectorRef(sel);
}


void AddCXXTemporary(const CXXTemporary *Temp);


void AddCXXBaseSpecifier(const CXXBaseSpecifier &Base);


void AddCXXBaseSpecifiers(ArrayRef<CXXBaseSpecifier> Bases);


void AddTypeRef(QualType T) {
return Writer->AddTypeRef(T, *Record);
}
void writeQualType(QualType T) {
AddTypeRef(T);
}


void AddTypeSourceInfo(TypeSourceInfo *TInfo);


void AddTypeLoc(TypeLoc TL);


void AddTemplateArgumentLocInfo(TemplateArgument::ArgKind Kind,
const TemplateArgumentLocInfo &Arg);


void AddTemplateArgumentLoc(const TemplateArgumentLoc &Arg);


void AddASTTemplateArgumentListInfo(
const ASTTemplateArgumentListInfo *ASTTemplArgList);


void AddDeclRef(const Decl *D) {
return Writer->AddDeclRef(D, *Record);
}
void writeDeclRef(const Decl *D) {
AddDeclRef(D);
}


void AddDeclarationName(DeclarationName Name) {
writeDeclarationName(Name);
}

void AddDeclarationNameLoc(const DeclarationNameLoc &DNLoc,
DeclarationName Name);
void AddDeclarationNameInfo(const DeclarationNameInfo &NameInfo);

void AddQualifierInfo(const QualifierInfo &Info);


void AddNestedNameSpecifier(NestedNameSpecifier *NNS) {
writeNestedNameSpecifier(NNS);
}


void AddNestedNameSpecifierLoc(NestedNameSpecifierLoc NNS);


void AddTemplateName(TemplateName Name) {
writeTemplateName(Name);
}


void AddTemplateArgument(const TemplateArgument &Arg) {
writeTemplateArgument(Arg);
}


void AddTemplateParameterList(const TemplateParameterList *TemplateParams);


void AddTemplateArgumentList(const TemplateArgumentList *TemplateArgs);


void AddUnresolvedSet(const ASTUnresolvedSet &Set);


void AddCXXCtorInitializers(ArrayRef<CXXCtorInitializer *> CtorInits);

void AddCXXDefinitionData(const CXXRecordDecl *D);


void AddVarDeclInit(const VarDecl *VD);


void writeOMPTraitInfo(const OMPTraitInfo *TI);

void writeOMPClause(OMPClause *C);


void writeOMPChildren(OMPChildren *Data);


void AddString(StringRef Str) {
return Writer->AddString(Str, *Record);
}


void AddPath(StringRef Path) {
return Writer->AddPath(Path, *Record);
}


void AddVersionTuple(const VersionTuple &Version) {
return Writer->AddVersionTuple(Version, *Record);
}


void AddAttr(const Attr *A);


void AddAttributes(ArrayRef<const Attr*> Attrs);
};

}

#endif
