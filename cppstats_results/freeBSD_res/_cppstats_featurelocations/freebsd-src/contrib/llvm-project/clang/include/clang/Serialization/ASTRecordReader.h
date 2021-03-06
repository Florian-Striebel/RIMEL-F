












#if !defined(LLVM_CLANG_SERIALIZATION_ASTRECORDREADER_H)
#define LLVM_CLANG_SERIALIZATION_ASTRECORDREADER_H

#include "clang/AST/ASTContext.h"
#include "clang/AST/AbstractBasicReader.h"
#include "clang/Lex/Token.h"
#include "clang/Serialization/ASTReader.h"
#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/APInt.h"
#include "llvm/ADT/APSInt.h"

namespace clang {
class OMPTraitInfo;
class OMPChildren;


class ASTRecordReader
: public serialization::DataStreamBasicReader<ASTRecordReader> {
using ModuleFile = serialization::ModuleFile;

ASTReader *Reader;
ModuleFile *F;
unsigned Idx = 0;
ASTReader::RecordData Record;

using RecordData = ASTReader::RecordData;
using RecordDataImpl = ASTReader::RecordDataImpl;

public:

ASTRecordReader(ASTReader &Reader, ModuleFile &F)
: DataStreamBasicReader(Reader.getContext()), Reader(&Reader), F(&F) {}



Expected<unsigned> readRecord(llvm::BitstreamCursor &Cursor,
unsigned AbbrevID);


bool isModule() const { return F->isModule(); }


ASTContext &getContext() { return Reader->getContext(); }


unsigned getIdx() const { return Idx; }


size_t size() const { return Record.size(); }


const uint64_t &operator[](size_t N) { return Record[N]; }


uint64_t back() { return Record.back(); }



uint64_t readInt() { return Record[Idx++]; }

ArrayRef<uint64_t> readIntArray(unsigned Len) {
auto Array = llvm::makeArrayRef(Record).slice(Idx, Len);
Idx += Len;
return Array;
}


uint64_t peekInt() { return Record[Idx]; }


void skipInts(unsigned N) { Idx += N; }


serialization::SubmoduleID
getGlobalSubmoduleID(unsigned LocalID) {
return Reader->getGlobalSubmoduleID(*F, LocalID);
}


Module *getSubmodule(serialization::SubmoduleID GlobalID) {
return Reader->getSubmodule(GlobalID);
}


bool readLexicalDeclContextStorage(uint64_t Offset, DeclContext *DC) {
return Reader->ReadLexicalDeclContextStorage(*F, F->DeclsCursor, Offset,
DC);
}


bool readVisibleDeclContextStorage(uint64_t Offset,
serialization::DeclID ID) {
return Reader->ReadVisibleDeclContextStorage(*F, F->DeclsCursor, Offset,
ID);
}

ExplicitSpecifier readExplicitSpec() {
uint64_t Kind = readInt();
bool HasExpr = Kind & 0x1;
Kind = Kind >> 1;
return ExplicitSpecifier(HasExpr ? readExpr() : nullptr,
static_cast<ExplicitSpecKind>(Kind));
}






uint64_t getGlobalBitOffset(uint64_t LocalOffset) {
return Reader->getGlobalBitOffset(*F, LocalOffset);
}


Stmt *readStmt() { return Reader->ReadStmt(*F); }
Stmt *readStmtRef() { return readStmt(); }


Expr *readExpr() { return Reader->ReadExpr(*F); }


Stmt *readSubStmt() { return Reader->ReadSubStmt(); }


Expr *readSubExpr() { return Reader->ReadSubExpr(); }




template<typename T>
T *GetLocalDeclAs(uint32_t LocalID) {
return cast_or_null<T>(Reader->GetLocalDecl(*F, LocalID));
}



TemplateArgumentLocInfo
readTemplateArgumentLocInfo(TemplateArgument::ArgKind Kind);


TemplateArgumentLoc readTemplateArgumentLoc();

const ASTTemplateArgumentListInfo*
readASTTemplateArgumentListInfo();


TypeSourceInfo *readTypeSourceInfo();


void readTypeLoc(TypeLoc TL);



serialization::TypeID getGlobalTypeID(unsigned LocalID) const {
return Reader->getGlobalTypeID(*F, LocalID);
}

Qualifiers readQualifiers() {
return Qualifiers::fromOpaqueValue(readInt());
}


QualType readType() {
return Reader->readType(*F, Record, Idx);
}
QualType readQualType() {
return readType();
}




serialization::DeclID readDeclID() {
return Reader->ReadDeclID(*F, Record, Idx);
}



Decl *readDecl() {
return Reader->ReadDecl(*F, Record, Idx);
}
Decl *readDeclRef() {
return readDecl();
}






template<typename T>
T *readDeclAs() {
return Reader->ReadDeclAs<T>(*F, Record, Idx);
}

IdentifierInfo *readIdentifier() {
return Reader->readIdentifier(*F, Record, Idx);
}


Selector readSelector() {
return Reader->ReadSelector(*F, Record, Idx);
}



DeclarationNameLoc readDeclarationNameLoc(DeclarationName Name);
DeclarationNameInfo readDeclarationNameInfo();

void readQualifierInfo(QualifierInfo &Info);




NestedNameSpecifierLoc readNestedNameSpecifierLoc();






using DataStreamBasicReader::readTemplateArgument;
TemplateArgument readTemplateArgument(bool Canonicalize) {
TemplateArgument Arg = readTemplateArgument();
if (Canonicalize) {
Arg = getContext().getCanonicalTemplateArgument(Arg);
}
return Arg;
}


TemplateParameterList *readTemplateParameterList();


void readTemplateArgumentList(SmallVectorImpl<TemplateArgument> &TemplArgs,
bool Canonicalize = false);


void readUnresolvedSet(LazyASTUnresolvedSet &Set);


CXXBaseSpecifier readCXXBaseSpecifier();


CXXCtorInitializer **readCXXCtorInitializers();

CXXTemporary *readCXXTemporary() {
return Reader->ReadCXXTemporary(*F, Record, Idx);
}


OMPTraitInfo *readOMPTraitInfo();


OMPClause *readOMPClause();


void readOMPChildren(OMPChildren *Data);


SourceLocation readSourceLocation() {
return Reader->ReadSourceLocation(*F, Record, Idx);
}


SourceRange readSourceRange() {
return Reader->ReadSourceRange(*F, Record, Idx);
}











llvm::APFloat readAPFloat(const llvm::fltSemantics &Sem);


bool readBool() { return readInt() != 0; }


uint32_t readUInt32() {
return uint32_t(readInt());
}


uint64_t readUInt64() {
return readInt();
}


std::string readString() {
return Reader->ReadString(Record, Idx);
}


std::string readPath() {
return Reader->ReadPath(*F, Record, Idx);
}


VersionTuple readVersionTuple() {
return ASTReader::ReadVersionTuple(Record, Idx);
}


Attr *readAttr();


void readAttributes(AttrVec &Attrs);


Token readToken() {
return Reader->ReadToken(*F, Record, Idx);
}

void recordSwitchCaseID(SwitchCase *SC, unsigned ID) {
Reader->RecordSwitchCaseID(SC, ID);
}


SwitchCase *getSwitchCaseWithID(unsigned ID) {
return Reader->getSwitchCaseWithID(ID);
}
};



struct SavedStreamPosition {
explicit SavedStreamPosition(llvm::BitstreamCursor &Cursor)
: Cursor(Cursor), Offset(Cursor.GetCurrentBitNo()) {}

~SavedStreamPosition() {
if (llvm::Error Err = Cursor.JumpToBit(Offset))
llvm::report_fatal_error(
"Cursor should always be able to go back, failed: " +
toString(std::move(Err)));
}

private:
llvm::BitstreamCursor &Cursor;
uint64_t Offset;
};

inline void PCHValidator::Error(const char *Msg) {
Reader.Error(Msg);
}

}

#endif
