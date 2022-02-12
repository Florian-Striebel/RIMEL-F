












#if !defined(LLVM_CLANG_SEMA_TEMPLATEDEDUCTION_H)
#define LLVM_CLANG_SEMA_TEMPLATEDEDUCTION_H

#include "clang/Sema/Ownership.h"
#include "clang/Sema/SemaConcept.h"
#include "clang/AST/ASTConcept.h"
#include "clang/AST/DeclAccessPair.h"
#include "clang/AST/DeclTemplate.h"
#include "clang/AST/TemplateBase.h"
#include "clang/Basic/PartialDiagnostic.h"
#include "clang/Basic/SourceLocation.h"
#include "llvm/ADT/Optional.h"
#include "llvm/ADT/SmallVector.h"
#include <cassert>
#include <cstddef>
#include <utility>

namespace clang {

class Decl;
struct DeducedPack;
class Sema;

namespace sema {




class TemplateDeductionInfo {

TemplateArgumentList *Deduced = nullptr;



SourceLocation Loc;


bool HasSFINAEDiagnostic = false;


unsigned DeducedDepth;



unsigned ExplicitArgs = 0;



SmallVector<PartialDiagnosticAt, 4> SuppressedDiagnostics;

public:
TemplateDeductionInfo(SourceLocation Loc, unsigned DeducedDepth = 0)
: Loc(Loc), DeducedDepth(DeducedDepth) {}
TemplateDeductionInfo(const TemplateDeductionInfo &) = delete;
TemplateDeductionInfo &operator=(const TemplateDeductionInfo &) = delete;

enum ForBaseTag { ForBase };


TemplateDeductionInfo(ForBaseTag, const TemplateDeductionInfo &Info)
: Deduced(Info.Deduced), Loc(Info.Loc), DeducedDepth(Info.DeducedDepth),
ExplicitArgs(Info.ExplicitArgs) {}



SourceLocation getLocation() const {
return Loc;
}



unsigned getDeducedDepth() const {
return DeducedDepth;
}


unsigned getNumExplicitArgs() const {
return ExplicitArgs;
}


TemplateArgumentList *take() {
TemplateArgumentList *Result = Deduced;
Deduced = nullptr;
return Result;
}


void takeSFINAEDiagnostic(PartialDiagnosticAt &PD) {
assert(HasSFINAEDiagnostic);
PD.first = SuppressedDiagnostics.front().first;
PD.second.swap(SuppressedDiagnostics.front().second);
clearSFINAEDiagnostic();
}


void clearSFINAEDiagnostic() {
SuppressedDiagnostics.clear();
HasSFINAEDiagnostic = false;
}


const PartialDiagnosticAt &peekSFINAEDiagnostic() const {
assert(HasSFINAEDiagnostic);
return SuppressedDiagnostics.front();
}



void setExplicitArgs(TemplateArgumentList *NewDeduced) {
Deduced = NewDeduced;
ExplicitArgs = Deduced->size();
}



void reset(TemplateArgumentList *NewDeduced) {
Deduced = NewDeduced;
}


bool hasSFINAEDiagnostic() const {
return HasSFINAEDiagnostic;
}


void addSFINAEDiagnostic(SourceLocation Loc, PartialDiagnostic PD) {

if (HasSFINAEDiagnostic)
return;
SuppressedDiagnostics.clear();
SuppressedDiagnostics.emplace_back(Loc, std::move(PD));
HasSFINAEDiagnostic = true;
}


void addSuppressedDiagnostic(SourceLocation Loc,
PartialDiagnostic PD) {
if (HasSFINAEDiagnostic)
return;
SuppressedDiagnostics.emplace_back(Loc, std::move(PD));
}


using diag_iterator = SmallVectorImpl<PartialDiagnosticAt>::const_iterator;



diag_iterator diag_begin() const { return SuppressedDiagnostics.begin(); }



diag_iterator diag_end() const { return SuppressedDiagnostics.end(); }















TemplateParameter Param;





















TemplateArgument FirstArg;













TemplateArgument SecondArg;






unsigned CallArgIndex = 0;




SmallVector<DeducedPack *, 8> PendingDeducedPacks;



ConstraintSatisfaction AssociatedConstraintsSatisfaction;
};

}



struct DeductionFailureInfo {

unsigned Result : 8;


unsigned HasDiagnostic : 1;



void *Data;


alignas(PartialDiagnosticAt) char Diagnostic[sizeof(PartialDiagnosticAt)];



PartialDiagnosticAt *getSFINAEDiagnostic();



TemplateParameter getTemplateParameter();



TemplateArgumentList *getTemplateArgumentList();



const TemplateArgument *getFirstArg();



const TemplateArgument *getSecondArg();



llvm::Optional<unsigned> getCallArgIndex();


void Destroy();
};








struct TemplateSpecCandidate {


DeclAccessPair FoundDecl;



Decl *Specialization;


DeductionFailureInfo DeductionFailure;

void set(DeclAccessPair Found, Decl *Spec, DeductionFailureInfo Info) {
FoundDecl = Found;
Specialization = Spec;
DeductionFailure = Info;
}


void NoteDeductionFailure(Sema &S, bool ForTakingAddress);
};





class TemplateSpecCandidateSet {
SmallVector<TemplateSpecCandidate, 16> Candidates;
SourceLocation Loc;




bool ForTakingAddress;

void destroyCandidates();

public:
TemplateSpecCandidateSet(SourceLocation Loc, bool ForTakingAddress = false)
: Loc(Loc), ForTakingAddress(ForTakingAddress) {}
TemplateSpecCandidateSet(const TemplateSpecCandidateSet &) = delete;
TemplateSpecCandidateSet &
operator=(const TemplateSpecCandidateSet &) = delete;
~TemplateSpecCandidateSet() { destroyCandidates(); }

SourceLocation getLocation() const { return Loc; }



void clear();

using iterator = SmallVector<TemplateSpecCandidate, 16>::iterator;

iterator begin() { return Candidates.begin(); }
iterator end() { return Candidates.end(); }

size_t size() const { return Candidates.size(); }
bool empty() const { return Candidates.empty(); }



TemplateSpecCandidate &addCandidate() {
Candidates.emplace_back();
return Candidates.back();
}

void NoteCandidates(Sema &S, SourceLocation Loc);

void NoteCandidates(Sema &S, SourceLocation Loc) const {
const_cast<TemplateSpecCandidateSet *>(this)->NoteCandidates(S, Loc);
}
};

}

#endif
