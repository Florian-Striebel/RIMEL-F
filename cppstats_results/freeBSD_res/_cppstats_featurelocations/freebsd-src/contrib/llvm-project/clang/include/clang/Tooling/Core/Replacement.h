
















#if !defined(LLVM_CLANG_TOOLING_CORE_REPLACEMENT_H)
#define LLVM_CLANG_TOOLING_CORE_REPLACEMENT_H

#include "clang/Basic/LangOptions.h"
#include "clang/Basic/SourceLocation.h"
#include "llvm/ADT/Optional.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/Compiler.h"
#include "llvm/Support/Error.h"
#include "llvm/Support/raw_ostream.h"
#include <map>
#include <set>
#include <string>
#include <system_error>
#include <utility>
#include <vector>

namespace clang {

class FileManager;
class Rewriter;
class SourceManager;

namespace tooling {


class Range {
public:
Range() = default;
Range(unsigned Offset, unsigned Length) : Offset(Offset), Length(Length) {}



unsigned getOffset() const { return Offset; }
unsigned getLength() const { return Length; }





bool overlapsWith(Range RHS) const {
return Offset + Length > RHS.Offset && Offset < RHS.Offset + RHS.Length;
}


bool contains(Range RHS) const {
return RHS.Offset >= Offset &&
(RHS.Offset + RHS.Length) <= (Offset + Length);
}


bool operator==(const Range &RHS) const {
return Offset == RHS.getOffset() && Length == RHS.getLength();
}


private:
unsigned Offset = 0;
unsigned Length = 0;
};





class Replacement {
public:

Replacement();







Replacement(StringRef FilePath, unsigned Offset, unsigned Length,
StringRef ReplacementText);



Replacement(const SourceManager &Sources, SourceLocation Start,
unsigned Length, StringRef ReplacementText);


Replacement(const SourceManager &Sources, const CharSourceRange &Range,
StringRef ReplacementText,
const LangOptions &LangOpts = LangOptions());


template <typename Node>
Replacement(const SourceManager &Sources, const Node &NodeToReplace,
StringRef ReplacementText,
const LangOptions &LangOpts = LangOptions());




bool isApplicable() const;



StringRef getFilePath() const { return FilePath; }
unsigned getOffset() const { return ReplacementRange.getOffset(); }
unsigned getLength() const { return ReplacementRange.getLength(); }
StringRef getReplacementText() const { return ReplacementText; }



bool apply(Rewriter &Rewrite) const;


std::string toString() const;

private:
void setFromSourceLocation(const SourceManager &Sources, SourceLocation Start,
unsigned Length, StringRef ReplacementText);
void setFromSourceRange(const SourceManager &Sources,
const CharSourceRange &Range,
StringRef ReplacementText,
const LangOptions &LangOpts);

std::string FilePath;
Range ReplacementRange;
std::string ReplacementText;
};

enum class replacement_error {
fail_to_apply = 0,
wrong_file_path,
overlap_conflict,
insert_conflict,
};



class ReplacementError : public llvm::ErrorInfo<ReplacementError> {
public:
ReplacementError(replacement_error Err) : Err(Err) {}


ReplacementError(replacement_error Err, Replacement Existing)
: Err(Err), ExistingReplacement(std::move(Existing)) {}



ReplacementError(replacement_error Err, Replacement New, Replacement Existing)
: Err(Err), NewReplacement(std::move(New)),
ExistingReplacement(std::move(Existing)) {}

std::string message() const override;

void log(raw_ostream &OS) const override { OS << message(); }

replacement_error get() const { return Err; }

static char ID;

const llvm::Optional<Replacement> &getNewReplacement() const {
return NewReplacement;
}

const llvm::Optional<Replacement> &getExistingReplacement() const {
return ExistingReplacement;
}

private:

std::error_code convertToErrorCode() const override {
return llvm::inconvertibleErrorCode();
}

replacement_error Err;



llvm::Optional<Replacement> NewReplacement;


llvm::Optional<Replacement> ExistingReplacement;
};


bool operator<(const Replacement &LHS, const Replacement &RHS);


bool operator==(const Replacement &LHS, const Replacement &RHS);




class Replacements {
private:
using ReplacementsImpl = std::set<Replacement>;

public:
using const_iterator = ReplacementsImpl::const_iterator;
using const_reverse_iterator = ReplacementsImpl::const_reverse_iterator;

Replacements() = default;

explicit Replacements(const Replacement &R) { Replaces.insert(R); }






































llvm::Error add(const Replacement &R);



LLVM_NODISCARD Replacements merge(const Replacements &Replaces) const;


std::vector<Range> getAffectedRanges() const;




unsigned getShiftedCodePosition(unsigned Position) const;

unsigned size() const { return Replaces.size(); }

void clear() { Replaces.clear(); }

bool empty() const { return Replaces.empty(); }

const_iterator begin() const { return Replaces.begin(); }

const_iterator end() const { return Replaces.end(); }

const_reverse_iterator rbegin() const { return Replaces.rbegin(); }

const_reverse_iterator rend() const { return Replaces.rend(); }

bool operator==(const Replacements &RHS) const {
return Replaces == RHS.Replaces;
}

private:
Replacements(const_iterator Begin, const_iterator End)
: Replaces(Begin, End) {}



Replacement getReplacementInChangedCode(const Replacement &R) const;





Replacements getCanonicalReplacements() const;




llvm::Expected<Replacements>
mergeIfOrderIndependent(const Replacement &R) const;

ReplacementsImpl Replaces;
};







bool applyAllReplacements(const Replacements &Replaces, Rewriter &Rewrite);








llvm::Expected<std::string> applyAllReplacements(StringRef Code,
const Replacements &Replaces);


struct TranslationUnitReplacements {

std::string MainSourceFile;

std::vector<Replacement> Replacements;
};









std::vector<Range>
calculateRangesAfterReplacements(const Replacements &Replaces,
const std::vector<Range> &Ranges);




std::map<std::string, Replacements> groupReplacementsByFile(
FileManager &FileMgr,
const std::map<std::string, Replacements> &FileToReplaces);

template <typename Node>
Replacement::Replacement(const SourceManager &Sources,
const Node &NodeToReplace, StringRef ReplacementText,
const LangOptions &LangOpts) {
const CharSourceRange Range =
CharSourceRange::getTokenRange(NodeToReplace->getSourceRange());
setFromSourceRange(Sources, Range, ReplacementText, LangOpts);
}

}

}

#endif
