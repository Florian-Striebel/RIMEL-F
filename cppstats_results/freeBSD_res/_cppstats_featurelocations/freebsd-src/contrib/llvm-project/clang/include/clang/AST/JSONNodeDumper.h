













#if !defined(LLVM_CLANG_AST_JSONNODEDUMPER_H)
#define LLVM_CLANG_AST_JSONNODEDUMPER_H

#include "clang/AST/ASTContext.h"
#include "clang/AST/ASTDumperUtils.h"
#include "clang/AST/ASTNodeTraverser.h"
#include "clang/AST/AttrVisitor.h"
#include "clang/AST/CommentCommandTraits.h"
#include "clang/AST/CommentVisitor.h"
#include "clang/AST/ExprConcepts.h"
#include "clang/AST/ExprCXX.h"
#include "clang/AST/Mangle.h"
#include "clang/AST/Type.h"
#include "llvm/Support/JSON.h"

namespace clang {

class APValue;

class NodeStreamer {
bool FirstChild = true;
bool TopLevel = true;
llvm::SmallVector<std::function<void(bool IsLastChild)>, 32> Pending;

protected:
llvm::json::OStream JOS;

public:

template <typename Fn> void AddChild(Fn DoAddChild) {
return AddChild("", DoAddChild);
}



template <typename Fn> void AddChild(StringRef Label, Fn DoAddChild) {


if (TopLevel) {
TopLevel = false;
JOS.objectBegin();

DoAddChild();

while (!Pending.empty()) {
Pending.back()(true);
Pending.pop_back();
}

JOS.objectEnd();
TopLevel = true;
return;
}



std::string LabelStr(!Label.empty() ? Label : "inner");
bool WasFirstChild = FirstChild;
auto DumpWithIndent = [=](bool IsLastChild) {
if (WasFirstChild) {
JOS.attributeBegin(LabelStr);
JOS.arrayBegin();
}

FirstChild = true;
unsigned Depth = Pending.size();
JOS.objectBegin();

DoAddChild();



while (Depth < Pending.size()) {
Pending.back()(true);
this->Pending.pop_back();
}

JOS.objectEnd();

if (IsLastChild) {
JOS.arrayEnd();
JOS.attributeEnd();
}
};

if (FirstChild) {
Pending.push_back(std::move(DumpWithIndent));
} else {
Pending.back()(false);
Pending.back() = std::move(DumpWithIndent);
}
FirstChild = false;
}

NodeStreamer(raw_ostream &OS) : JOS(OS, 2) {}
};






class JSONNodeDumper
: public ConstAttrVisitor<JSONNodeDumper>,
public comments::ConstCommentVisitor<JSONNodeDumper, void,
const comments::FullComment *>,
public ConstTemplateArgumentVisitor<JSONNodeDumper>,
public ConstStmtVisitor<JSONNodeDumper>,
public TypeVisitor<JSONNodeDumper>,
public ConstDeclVisitor<JSONNodeDumper>,
public NodeStreamer {
friend class JSONDumper;

const SourceManager &SM;
ASTContext& Ctx;
ASTNameGenerator ASTNameGen;
PrintingPolicy PrintPolicy;
const comments::CommandTraits *Traits;
StringRef LastLocFilename, LastLocPresumedFilename;
unsigned LastLocLine, LastLocPresumedLine;

using InnerAttrVisitor = ConstAttrVisitor<JSONNodeDumper>;
using InnerCommentVisitor =
comments::ConstCommentVisitor<JSONNodeDumper, void,
const comments::FullComment *>;
using InnerTemplateArgVisitor = ConstTemplateArgumentVisitor<JSONNodeDumper>;
using InnerStmtVisitor = ConstStmtVisitor<JSONNodeDumper>;
using InnerTypeVisitor = TypeVisitor<JSONNodeDumper>;
using InnerDeclVisitor = ConstDeclVisitor<JSONNodeDumper>;

void attributeOnlyIfTrue(StringRef Key, bool Value) {
if (Value)
JOS.attribute(Key, Value);
}

void writeIncludeStack(PresumedLoc Loc, bool JustFirst = false);


void writeBareSourceLocation(SourceLocation Loc, bool IsSpelling);





void writeSourceLocation(SourceLocation Loc);
void writeSourceRange(SourceRange R);
std::string createPointerRepresentation(const void *Ptr);
llvm::json::Object createQualType(QualType QT, bool Desugar = true);
llvm::json::Object createBareDeclRef(const Decl *D);
void writeBareDeclRef(const Decl *D);
llvm::json::Object createCXXRecordDefinitionData(const CXXRecordDecl *RD);
llvm::json::Object createCXXBaseSpecifier(const CXXBaseSpecifier &BS);
std::string createAccessSpecifier(AccessSpecifier AS);
llvm::json::Array createCastPath(const CastExpr *C);

void writePreviousDeclImpl(...) {}

template <typename T> void writePreviousDeclImpl(const Mergeable<T> *D) {
const T *First = D->getFirstDecl();
if (First != D)
JOS.attribute("firstRedecl", createPointerRepresentation(First));
}

template <typename T> void writePreviousDeclImpl(const Redeclarable<T> *D) {
const T *Prev = D->getPreviousDecl();
if (Prev)
JOS.attribute("previousDecl", createPointerRepresentation(Prev));
}
void addPreviousDeclaration(const Decl *D);

StringRef getCommentCommandName(unsigned CommandID) const;

public:
JSONNodeDumper(raw_ostream &OS, const SourceManager &SrcMgr, ASTContext &Ctx,
const PrintingPolicy &PrintPolicy,
const comments::CommandTraits *Traits)
: NodeStreamer(OS), SM(SrcMgr), Ctx(Ctx), ASTNameGen(Ctx),
PrintPolicy(PrintPolicy), Traits(Traits), LastLocLine(0),
LastLocPresumedLine(0) {}

void Visit(const Attr *A);
void Visit(const Stmt *Node);
void Visit(const Type *T);
void Visit(QualType T);
void Visit(const Decl *D);

void Visit(const comments::Comment *C, const comments::FullComment *FC);
void Visit(const TemplateArgument &TA, SourceRange R = {},
const Decl *From = nullptr, StringRef Label = {});
void Visit(const CXXCtorInitializer *Init);
void Visit(const OMPClause *C);
void Visit(const BlockDecl::Capture &C);
void Visit(const GenericSelectionExpr::ConstAssociation &A);
void Visit(const concepts::Requirement *R);
void Visit(const APValue &Value, QualType Ty);

void VisitTypedefType(const TypedefType *TT);
void VisitFunctionType(const FunctionType *T);
void VisitFunctionProtoType(const FunctionProtoType *T);
void VisitRValueReferenceType(const ReferenceType *RT);
void VisitArrayType(const ArrayType *AT);
void VisitConstantArrayType(const ConstantArrayType *CAT);
void VisitDependentSizedExtVectorType(const DependentSizedExtVectorType *VT);
void VisitVectorType(const VectorType *VT);
void VisitUnresolvedUsingType(const UnresolvedUsingType *UUT);
void VisitUnaryTransformType(const UnaryTransformType *UTT);
void VisitTagType(const TagType *TT);
void VisitTemplateTypeParmType(const TemplateTypeParmType *TTPT);
void VisitAutoType(const AutoType *AT);
void VisitTemplateSpecializationType(const TemplateSpecializationType *TST);
void VisitInjectedClassNameType(const InjectedClassNameType *ICNT);
void VisitObjCInterfaceType(const ObjCInterfaceType *OIT);
void VisitPackExpansionType(const PackExpansionType *PET);
void VisitElaboratedType(const ElaboratedType *ET);
void VisitMacroQualifiedType(const MacroQualifiedType *MQT);
void VisitMemberPointerType(const MemberPointerType *MPT);

void VisitNamedDecl(const NamedDecl *ND);
void VisitTypedefDecl(const TypedefDecl *TD);
void VisitTypeAliasDecl(const TypeAliasDecl *TAD);
void VisitNamespaceDecl(const NamespaceDecl *ND);
void VisitUsingDirectiveDecl(const UsingDirectiveDecl *UDD);
void VisitNamespaceAliasDecl(const NamespaceAliasDecl *NAD);
void VisitUsingDecl(const UsingDecl *UD);
void VisitUsingEnumDecl(const UsingEnumDecl *UED);
void VisitUsingShadowDecl(const UsingShadowDecl *USD);
void VisitVarDecl(const VarDecl *VD);
void VisitFieldDecl(const FieldDecl *FD);
void VisitFunctionDecl(const FunctionDecl *FD);
void VisitEnumDecl(const EnumDecl *ED);
void VisitEnumConstantDecl(const EnumConstantDecl *ECD);
void VisitRecordDecl(const RecordDecl *RD);
void VisitCXXRecordDecl(const CXXRecordDecl *RD);
void VisitTemplateTypeParmDecl(const TemplateTypeParmDecl *D);
void VisitNonTypeTemplateParmDecl(const NonTypeTemplateParmDecl *D);
void VisitTemplateTemplateParmDecl(const TemplateTemplateParmDecl *D);
void VisitLinkageSpecDecl(const LinkageSpecDecl *LSD);
void VisitAccessSpecDecl(const AccessSpecDecl *ASD);
void VisitFriendDecl(const FriendDecl *FD);

void VisitObjCIvarDecl(const ObjCIvarDecl *D);
void VisitObjCMethodDecl(const ObjCMethodDecl *D);
void VisitObjCTypeParamDecl(const ObjCTypeParamDecl *D);
void VisitObjCCategoryDecl(const ObjCCategoryDecl *D);
void VisitObjCCategoryImplDecl(const ObjCCategoryImplDecl *D);
void VisitObjCProtocolDecl(const ObjCProtocolDecl *D);
void VisitObjCInterfaceDecl(const ObjCInterfaceDecl *D);
void VisitObjCImplementationDecl(const ObjCImplementationDecl *D);
void VisitObjCCompatibleAliasDecl(const ObjCCompatibleAliasDecl *D);
void VisitObjCPropertyDecl(const ObjCPropertyDecl *D);
void VisitObjCPropertyImplDecl(const ObjCPropertyImplDecl *D);
void VisitBlockDecl(const BlockDecl *D);

void VisitDeclRefExpr(const DeclRefExpr *DRE);
void VisitSYCLUniqueStableNameExpr(const SYCLUniqueStableNameExpr *E);
void VisitPredefinedExpr(const PredefinedExpr *PE);
void VisitUnaryOperator(const UnaryOperator *UO);
void VisitBinaryOperator(const BinaryOperator *BO);
void VisitCompoundAssignOperator(const CompoundAssignOperator *CAO);
void VisitMemberExpr(const MemberExpr *ME);
void VisitCXXNewExpr(const CXXNewExpr *NE);
void VisitCXXDeleteExpr(const CXXDeleteExpr *DE);
void VisitCXXThisExpr(const CXXThisExpr *TE);
void VisitCastExpr(const CastExpr *CE);
void VisitImplicitCastExpr(const ImplicitCastExpr *ICE);
void VisitCallExpr(const CallExpr *CE);
void VisitUnaryExprOrTypeTraitExpr(const UnaryExprOrTypeTraitExpr *TTE);
void VisitSizeOfPackExpr(const SizeOfPackExpr *SOPE);
void VisitUnresolvedLookupExpr(const UnresolvedLookupExpr *ULE);
void VisitAddrLabelExpr(const AddrLabelExpr *ALE);
void VisitCXXTypeidExpr(const CXXTypeidExpr *CTE);
void VisitConstantExpr(const ConstantExpr *CE);
void VisitInitListExpr(const InitListExpr *ILE);
void VisitGenericSelectionExpr(const GenericSelectionExpr *GSE);
void VisitCXXUnresolvedConstructExpr(const CXXUnresolvedConstructExpr *UCE);
void VisitCXXConstructExpr(const CXXConstructExpr *CE);
void VisitExprWithCleanups(const ExprWithCleanups *EWC);
void VisitCXXBindTemporaryExpr(const CXXBindTemporaryExpr *BTE);
void VisitMaterializeTemporaryExpr(const MaterializeTemporaryExpr *MTE);
void VisitCXXDependentScopeMemberExpr(const CXXDependentScopeMemberExpr *ME);
void VisitRequiresExpr(const RequiresExpr *RE);

void VisitObjCEncodeExpr(const ObjCEncodeExpr *OEE);
void VisitObjCMessageExpr(const ObjCMessageExpr *OME);
void VisitObjCBoxedExpr(const ObjCBoxedExpr *OBE);
void VisitObjCSelectorExpr(const ObjCSelectorExpr *OSE);
void VisitObjCProtocolExpr(const ObjCProtocolExpr *OPE);
void VisitObjCPropertyRefExpr(const ObjCPropertyRefExpr *OPRE);
void VisitObjCSubscriptRefExpr(const ObjCSubscriptRefExpr *OSRE);
void VisitObjCIvarRefExpr(const ObjCIvarRefExpr *OIRE);
void VisitObjCBoolLiteralExpr(const ObjCBoolLiteralExpr *OBLE);

void VisitIntegerLiteral(const IntegerLiteral *IL);
void VisitCharacterLiteral(const CharacterLiteral *CL);
void VisitFixedPointLiteral(const FixedPointLiteral *FPL);
void VisitFloatingLiteral(const FloatingLiteral *FL);
void VisitStringLiteral(const StringLiteral *SL);
void VisitCXXBoolLiteralExpr(const CXXBoolLiteralExpr *BLE);

void VisitIfStmt(const IfStmt *IS);
void VisitSwitchStmt(const SwitchStmt *SS);
void VisitCaseStmt(const CaseStmt *CS);
void VisitLabelStmt(const LabelStmt *LS);
void VisitGotoStmt(const GotoStmt *GS);
void VisitWhileStmt(const WhileStmt *WS);
void VisitObjCAtCatchStmt(const ObjCAtCatchStmt *OACS);

void VisitNullTemplateArgument(const TemplateArgument &TA);
void VisitTypeTemplateArgument(const TemplateArgument &TA);
void VisitDeclarationTemplateArgument(const TemplateArgument &TA);
void VisitNullPtrTemplateArgument(const TemplateArgument &TA);
void VisitIntegralTemplateArgument(const TemplateArgument &TA);
void VisitTemplateTemplateArgument(const TemplateArgument &TA);
void VisitTemplateExpansionTemplateArgument(const TemplateArgument &TA);
void VisitExpressionTemplateArgument(const TemplateArgument &TA);
void VisitPackTemplateArgument(const TemplateArgument &TA);

void visitTextComment(const comments::TextComment *C,
const comments::FullComment *);
void visitInlineCommandComment(const comments::InlineCommandComment *C,
const comments::FullComment *);
void visitHTMLStartTagComment(const comments::HTMLStartTagComment *C,
const comments::FullComment *);
void visitHTMLEndTagComment(const comments::HTMLEndTagComment *C,
const comments::FullComment *);
void visitBlockCommandComment(const comments::BlockCommandComment *C,
const comments::FullComment *);
void visitParamCommandComment(const comments::ParamCommandComment *C,
const comments::FullComment *FC);
void visitTParamCommandComment(const comments::TParamCommandComment *C,
const comments::FullComment *FC);
void visitVerbatimBlockComment(const comments::VerbatimBlockComment *C,
const comments::FullComment *);
void
visitVerbatimBlockLineComment(const comments::VerbatimBlockLineComment *C,
const comments::FullComment *);
void visitVerbatimLineComment(const comments::VerbatimLineComment *C,
const comments::FullComment *);
};

class JSONDumper : public ASTNodeTraverser<JSONDumper, JSONNodeDumper> {
JSONNodeDumper NodeDumper;

template <typename SpecializationDecl>
void writeTemplateDeclSpecialization(const SpecializationDecl *SD,
bool DumpExplicitInst,
bool DumpRefOnly) {
bool DumpedAny = false;
for (const auto *RedeclWithBadType : SD->redecls()) {



const auto *Redecl = dyn_cast<SpecializationDecl>(RedeclWithBadType);
if (!Redecl) {



assert(isa<CXXRecordDecl>(RedeclWithBadType) &&
"expected an injected-class-name");
continue;
}

switch (Redecl->getTemplateSpecializationKind()) {
case TSK_ExplicitInstantiationDeclaration:
case TSK_ExplicitInstantiationDefinition:
if (!DumpExplicitInst)
break;
LLVM_FALLTHROUGH;
case TSK_Undeclared:
case TSK_ImplicitInstantiation:
if (DumpRefOnly)
NodeDumper.AddChild([=] { NodeDumper.writeBareDeclRef(Redecl); });
else
Visit(Redecl);
DumpedAny = true;
break;
case TSK_ExplicitSpecialization:
break;
}
}


if (!DumpedAny)
NodeDumper.AddChild([=] { NodeDumper.writeBareDeclRef(SD); });
}

template <typename TemplateDecl>
void writeTemplateDecl(const TemplateDecl *TD, bool DumpExplicitInst) {






dumpTemplateParameters(TD->getTemplateParameters());

Visit(TD->getTemplatedDecl());

for (const auto *Child : TD->specializations())
writeTemplateDeclSpecialization(Child, DumpExplicitInst,
!TD->isCanonicalDecl());
}

public:
JSONDumper(raw_ostream &OS, const SourceManager &SrcMgr, ASTContext &Ctx,
const PrintingPolicy &PrintPolicy,
const comments::CommandTraits *Traits)
: NodeDumper(OS, SrcMgr, Ctx, PrintPolicy, Traits) {}

JSONNodeDumper &doGetNodeDelegate() { return NodeDumper; }

void VisitFunctionTemplateDecl(const FunctionTemplateDecl *FTD) {
writeTemplateDecl(FTD, true);
}
void VisitClassTemplateDecl(const ClassTemplateDecl *CTD) {
writeTemplateDecl(CTD, false);
}
void VisitVarTemplateDecl(const VarTemplateDecl *VTD) {
writeTemplateDecl(VTD, false);
}
};

}

#endif
