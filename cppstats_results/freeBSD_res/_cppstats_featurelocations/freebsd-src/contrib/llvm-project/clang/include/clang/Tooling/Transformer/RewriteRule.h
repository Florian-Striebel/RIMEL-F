













#if !defined(LLVM_CLANG_TOOLING_TRANSFORMER_REWRITE_RULE_H_)
#define LLVM_CLANG_TOOLING_TRANSFORMER_REWRITE_RULE_H_

#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchersInternal.h"
#include "clang/Tooling/Refactoring/AtomicChange.h"
#include "clang/Tooling/Transformer/MatchConsumer.h"
#include "clang/Tooling/Transformer/RangeSelector.h"
#include "llvm/ADT/Any.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/Support/Error.h"
#include <functional>
#include <string>
#include <utility>

namespace clang {
namespace transformer {

enum class EditKind {

Range,


AddInclude,
};



struct Edit {
EditKind Kind = EditKind::Range;
CharSourceRange Range;
std::string Replacement;
llvm::Any Metadata;
};


enum class IncludeFormat {
Quoted,
Angled,
};





using EditGenerator = MatchConsumer<llvm::SmallVector<Edit, 1>>;

using TextGenerator = std::shared_ptr<MatchComputation<std::string>>;

using AnyGenerator = MatchConsumer<llvm::Any>;





































struct ASTEdit {
EditKind Kind = EditKind::Range;
RangeSelector TargetRange;
TextGenerator Replacement;
TextGenerator Note;


AnyGenerator Metadata = [](const ast_matchers::MatchFinder::MatchResult &)
-> llvm::Expected<llvm::Any> {
return llvm::Expected<llvm::Any>(llvm::Any());
};
};


EditGenerator edit(ASTEdit E);










EditGenerator editList(llvm::SmallVector<ASTEdit, 1> Edits);


inline EditGenerator noEdits() { return editList({}); }




EditGenerator noopEdit(RangeSelector Anchor);


inline EditGenerator ifBound(std::string ID, ASTEdit TrueEdit,
ASTEdit FalseEdit) {
return ifBound(std::move(ID), edit(std::move(TrueEdit)),
edit(std::move(FalseEdit)));
}



inline EditGenerator ifBound(std::string ID, ASTEdit TrueEdit) {
return ifBound(std::move(ID), edit(std::move(TrueEdit)), noEdits());
}



EditGenerator flattenVector(SmallVector<EditGenerator, 2> Generators);

namespace detail {




inline EditGenerator injectEdits(ASTEdit E) { return edit(std::move(E)); }
inline EditGenerator injectEdits(EditGenerator G) { return G; }
}

template <typename... Ts> EditGenerator flatten(Ts &&...Edits) {
return flattenVector({detail::injectEdits(std::forward<Ts>(Edits))...});
}




extern const char RootID[];


ASTEdit changeTo(RangeSelector Target, TextGenerator Replacement);

inline ASTEdit change(RangeSelector Target, TextGenerator Replacement) {
return changeTo(std::move(Target), std::move(Replacement));
}







inline ASTEdit changeTo(TextGenerator Replacement) {
return changeTo(node(RootID), std::move(Replacement));
}

inline ASTEdit change(TextGenerator Replacement) {
return changeTo(std::move(Replacement));
}



inline ASTEdit insertBefore(RangeSelector S, TextGenerator Replacement) {
return changeTo(before(std::move(S)), std::move(Replacement));
}



inline ASTEdit insertAfter(RangeSelector S, TextGenerator Replacement) {
return changeTo(after(std::move(S)), std::move(Replacement));
}


ASTEdit remove(RangeSelector S);



ASTEdit addInclude(RangeSelector Target, StringRef Header,
IncludeFormat Format = IncludeFormat::Quoted);





inline ASTEdit addInclude(StringRef Header,
IncludeFormat Format = IncludeFormat::Quoted) {
return addInclude(expansion(node(RootID)), Header, Format);
}













template <typename Callable>
inline ASTEdit withMetadata(ASTEdit Edit, Callable Metadata) {
Edit.Metadata =
[Gen = std::move(Metadata)](
const ast_matchers::MatchFinder::MatchResult &R) -> llvm::Any {
return Gen(R);
};

return Edit;
}




inline EditGenerator shrinkTo(RangeSelector outer, RangeSelector inner) {
return editList({remove(enclose(before(outer), before(inner))),
remove(enclose(after(inner), after(outer)))});
}
























struct RewriteRule {
struct Case {
ast_matchers::internal::DynTypedMatcher Matcher;
EditGenerator Edits;
TextGenerator Explanation;
};

SmallVector<Case, 1> Cases;


static const llvm::StringRef RootID;
};


RewriteRule makeRule(ast_matchers::internal::DynTypedMatcher M,
EditGenerator Edits, TextGenerator Explanation = nullptr);


inline RewriteRule makeRule(ast_matchers::internal::DynTypedMatcher M,
llvm::SmallVector<ASTEdit, 1> Edits,
TextGenerator Explanation = nullptr) {
return makeRule(std::move(M), editList(std::move(Edits)),
std::move(Explanation));
}


inline RewriteRule makeRule(ast_matchers::internal::DynTypedMatcher M,
ASTEdit Edit,
TextGenerator Explanation = nullptr) {
return makeRule(std::move(M), edit(std::move(Edit)), std::move(Explanation));
}











void addInclude(RewriteRule &Rule, llvm::StringRef Header,
IncludeFormat Format = IncludeFormat::Quoted);








































RewriteRule applyFirst(ArrayRef<RewriteRule> Rules);
















EditGenerator rewriteDescendants(std::string NodeId, RewriteRule Rule);









namespace detail {















llvm::Expected<SmallVector<Edit, 1>>
rewriteDescendants(const Decl &Node, RewriteRule Rule,
const ast_matchers::MatchFinder::MatchResult &Result);

llvm::Expected<SmallVector<Edit, 1>>
rewriteDescendants(const Stmt &Node, RewriteRule Rule,
const ast_matchers::MatchFinder::MatchResult &Result);

llvm::Expected<SmallVector<Edit, 1>>
rewriteDescendants(const TypeLoc &Node, RewriteRule Rule,
const ast_matchers::MatchFinder::MatchResult &Result);

llvm::Expected<SmallVector<Edit, 1>>
rewriteDescendants(const DynTypedNode &Node, RewriteRule Rule,
const ast_matchers::MatchFinder::MatchResult &Result);






ast_matchers::internal::DynTypedMatcher buildMatcher(const RewriteRule &Rule);








std::vector<ast_matchers::internal::DynTypedMatcher>
buildMatchers(const RewriteRule &Rule);




SourceLocation
getRuleMatchLoc(const ast_matchers::MatchFinder::MatchResult &Result);



const RewriteRule::Case &
findSelectedCase(const ast_matchers::MatchFinder::MatchResult &Result,
const RewriteRule &Rule);
}
}
}

#endif
