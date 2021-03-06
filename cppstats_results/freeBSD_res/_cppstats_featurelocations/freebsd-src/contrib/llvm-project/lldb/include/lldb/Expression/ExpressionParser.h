







#if !defined(LLDB_EXPRESSION_EXPRESSIONPARSER_H)
#define LLDB_EXPRESSION_EXPRESSIONPARSER_H

#include "lldb/Utility/CompletionRequest.h"
#include "lldb/Utility/Status.h"
#include "lldb/lldb-private-enumerations.h"
#include "lldb/lldb-public.h"

namespace lldb_private {

class IRExecutionUnit;






class ExpressionParser {
public:











ExpressionParser(ExecutionContextScope *exe_scope, Expression &expr,
bool generate_debug_info)
: m_expr(expr), m_generate_debug_info(generate_debug_info) {}


virtual ~ExpressionParser() = default;































virtual bool Complete(CompletionRequest &request, unsigned line, unsigned pos,
unsigned typed_pos) = 0;










virtual bool RewriteExpression(DiagnosticManager &diagnostic_manager) {
return false;
}































virtual Status
PrepareForExecution(lldb::addr_t &func_addr, lldb::addr_t &func_end,
std::shared_ptr<IRExecutionUnit> &execution_unit_sp,
ExecutionContext &exe_ctx, bool &can_interpret,
lldb_private::ExecutionPolicy execution_policy) = 0;

bool GetGenerateDebugInfo() const { return m_generate_debug_info; }

protected:
Expression &m_expr;
bool m_generate_debug_info;
};
}

#endif
