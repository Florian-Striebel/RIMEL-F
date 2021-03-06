












#if !defined(LLVM_CLANG_ASTMATCHERS_GTESTMATCHERS_H)
#define LLVM_CLANG_ASTMATCHERS_GTESTMATCHERS_H

#include "clang/AST/Stmt.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "llvm/ADT/StringRef.h"

namespace clang {
namespace ast_matchers {


enum class GtestCmp {
Eq,
Ne,
Ge,
Gt,
Le,
Lt,
};





enum class MockArgs {
None,
Some,
};



internal::BindableMatcher<Stmt> gtestAssert(GtestCmp Cmp, StatementMatcher Left,
StatementMatcher Right);


internal::BindableMatcher<Stmt> gtestAssertThat(StatementMatcher Actual,
StatementMatcher Matcher);



internal::BindableMatcher<Stmt> gtestExpect(GtestCmp Cmp, StatementMatcher Left,
StatementMatcher Right);


internal::BindableMatcher<Stmt> gtestExpectThat(StatementMatcher Actual,
StatementMatcher Matcher);




internal::BindableMatcher<Stmt> gtestExpectCall(StatementMatcher MockObject,
llvm::StringRef MockMethodName,
MockArgs Args);




internal::BindableMatcher<Stmt> gtestExpectCall(StatementMatcher MockCall,
MockArgs Args);


internal::BindableMatcher<Stmt> gtestOnCall(StatementMatcher MockObject,
llvm::StringRef MockMethodName,
MockArgs Args);


internal::BindableMatcher<Stmt> gtestOnCall(StatementMatcher MockCall,
MockArgs Args);

}
}

#endif

