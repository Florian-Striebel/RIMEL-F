






































#if !defined(LLVM_CLANG_ASTMATCHERS_ASTMATCHFINDER_H)
#define LLVM_CLANG_ASTMATCHERS_ASTMATCHFINDER_H

#include "clang/ASTMatchers/ASTMatchers.h"
#include "llvm/ADT/SmallPtrSet.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/Support/Timer.h"

namespace clang {

namespace ast_matchers {

















class MatchFinder {
public:




struct MatchResult {
MatchResult(const BoundNodes &Nodes, clang::ASTContext *Context);




const BoundNodes Nodes;



clang::ASTContext * const Context;
clang::SourceManager * const SourceManager;

};



class MatchCallback {
public:
virtual ~MatchCallback();


virtual void run(const MatchResult &Result) = 0;




virtual void onStartOfTranslationUnit() {}




virtual void onEndOfTranslationUnit() {}





virtual StringRef getID() const;





virtual llvm::Optional<TraversalKind> getCheckTraversalKind() const;
};


class ParsingDoneTestCallback {
public:
virtual ~ParsingDoneTestCallback();
virtual void run() = 0;
};

struct MatchFinderOptions {
struct Profiling {
Profiling(llvm::StringMap<llvm::TimeRecord> &Records)
: Records(Records) {}


llvm::StringMap<llvm::TimeRecord> &Records;
};




llvm::Optional<Profiling> CheckProfiling;
};

MatchFinder(MatchFinderOptions Options = MatchFinderOptions());
~MatchFinder();









void addMatcher(const DeclarationMatcher &NodeMatch,
MatchCallback *Action);
void addMatcher(const TypeMatcher &NodeMatch,
MatchCallback *Action);
void addMatcher(const StatementMatcher &NodeMatch,
MatchCallback *Action);
void addMatcher(const NestedNameSpecifierMatcher &NodeMatch,
MatchCallback *Action);
void addMatcher(const NestedNameSpecifierLocMatcher &NodeMatch,
MatchCallback *Action);
void addMatcher(const TypeLocMatcher &NodeMatch,
MatchCallback *Action);
void addMatcher(const CXXCtorInitializerMatcher &NodeMatch,
MatchCallback *Action);
void addMatcher(const TemplateArgumentLocMatcher &NodeMatch,
MatchCallback *Action);










bool addDynamicMatcher(const internal::DynTypedMatcher &NodeMatch,
MatchCallback *Action);


std::unique_ptr<clang::ASTConsumer> newASTConsumer();







template <typename T> void match(const T &Node, ASTContext &Context) {
match(clang::DynTypedNode::create(Node), Context);
}
void match(const clang::DynTypedNode &Node, ASTContext &Context);



void matchAST(ASTContext &Context);






void registerTestCallbackAfterParsing(ParsingDoneTestCallback *ParsingDone);



struct MatchersByType {
std::vector<std::pair<internal::DynTypedMatcher, MatchCallback *>>
DeclOrStmt;
std::vector<std::pair<TypeMatcher, MatchCallback *>> Type;
std::vector<std::pair<NestedNameSpecifierMatcher, MatchCallback *>>
NestedNameSpecifier;
std::vector<std::pair<NestedNameSpecifierLocMatcher, MatchCallback *>>
NestedNameSpecifierLoc;
std::vector<std::pair<TypeLocMatcher, MatchCallback *>> TypeLoc;
std::vector<std::pair<CXXCtorInitializerMatcher, MatchCallback *>> CtorInit;
std::vector<std::pair<TemplateArgumentLocMatcher, MatchCallback *>>
TemplateArgumentLoc;

llvm::SmallPtrSet<MatchCallback *, 16> AllCallbacks;
};

private:
MatchersByType Matchers;

MatchFinderOptions Options;


ParsingDoneTestCallback *ParsingDone;
};















template <typename MatcherT, typename NodeT>
SmallVector<BoundNodes, 1>
match(MatcherT Matcher, const NodeT &Node, ASTContext &Context);

template <typename MatcherT>
SmallVector<BoundNodes, 1> match(MatcherT Matcher, const DynTypedNode &Node,
ASTContext &Context);




template <typename MatcherT>
SmallVector<BoundNodes, 1> match(MatcherT Matcher, ASTContext &Context);











template <typename NodeT>
const NodeT *
selectFirst(StringRef BoundTo, const SmallVectorImpl<BoundNodes> &Results) {
for (const BoundNodes &N : Results) {
if (const NodeT *Node = N.getNodeAs<NodeT>(BoundTo))
return Node;
}
return nullptr;
}

namespace internal {
class CollectMatchesCallback : public MatchFinder::MatchCallback {
public:
void run(const MatchFinder::MatchResult &Result) override {
Nodes.push_back(Result.Nodes);
}

llvm::Optional<TraversalKind> getCheckTraversalKind() const override {
return llvm::None;
}

SmallVector<BoundNodes, 1> Nodes;
};
}

template <typename MatcherT>
SmallVector<BoundNodes, 1> match(MatcherT Matcher, const DynTypedNode &Node,
ASTContext &Context) {
internal::CollectMatchesCallback Callback;
MatchFinder Finder;
Finder.addMatcher(Matcher, &Callback);
Finder.match(Node, Context);
return std::move(Callback.Nodes);
}

template <typename MatcherT, typename NodeT>
SmallVector<BoundNodes, 1>
match(MatcherT Matcher, const NodeT &Node, ASTContext &Context) {
return match(Matcher, DynTypedNode::create(Node), Context);
}

template <typename MatcherT>
SmallVector<BoundNodes, 1>
match(MatcherT Matcher, ASTContext &Context) {
internal::CollectMatchesCallback Callback;
MatchFinder Finder;
Finder.addMatcher(Matcher, &Callback);
Finder.matchAST(Context);
return std::move(Callback.Nodes);
}

inline SmallVector<BoundNodes, 1>
matchDynamic(internal::DynTypedMatcher Matcher, const DynTypedNode &Node,
ASTContext &Context) {
internal::CollectMatchesCallback Callback;
MatchFinder Finder;
Finder.addDynamicMatcher(Matcher, &Callback);
Finder.match(Node, Context);
return std::move(Callback.Nodes);
}

template <typename NodeT>
SmallVector<BoundNodes, 1> matchDynamic(internal::DynTypedMatcher Matcher,
const NodeT &Node,
ASTContext &Context) {
return matchDynamic(Matcher, DynTypedNode::create(Node), Context);
}

inline SmallVector<BoundNodes, 1>
matchDynamic(internal::DynTypedMatcher Matcher, ASTContext &Context) {
internal::CollectMatchesCallback Callback;
MatchFinder Finder;
Finder.addDynamicMatcher(Matcher, &Callback);
Finder.matchAST(Context);
return std::move(Callback.Nodes);
}

}
}

#endif
