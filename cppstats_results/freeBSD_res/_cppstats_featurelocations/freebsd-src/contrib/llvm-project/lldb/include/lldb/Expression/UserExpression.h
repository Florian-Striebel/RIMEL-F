







#if !defined(LLDB_EXPRESSION_USEREXPRESSION_H)
#define LLDB_EXPRESSION_USEREXPRESSION_H

#include <memory>
#include <string>
#include <vector>

#include "lldb/Core/Address.h"
#include "lldb/Expression/Expression.h"
#include "lldb/Expression/Materializer.h"
#include "lldb/Target/ExecutionContext.h"
#include "lldb/Target/Target.h"
#include "lldb/lldb-forward.h"
#include "lldb/lldb-private.h"

namespace lldb_private {










class UserExpression : public Expression {

static char ID;

public:
bool isA(const void *ClassID) const override { return ClassID == &ID; }
static bool classof(const Expression *obj) { return obj->isA(&ID); }

enum { kDefaultTimeout = 500000u };














UserExpression(ExecutionContextScope &exe_scope, llvm::StringRef expr,
llvm::StringRef prefix, lldb::LanguageType language,
ResultType desired_type,
const EvaluateExpressionOptions &options);


~UserExpression() override;




















virtual bool Parse(DiagnosticManager &diagnostic_manager,
ExecutionContext &exe_ctx,
lldb_private::ExecutionPolicy execution_policy,
bool keep_result_in_memory, bool generate_debug_info) = 0;






















virtual bool Complete(ExecutionContext &exe_ctx, CompletionRequest &request,
unsigned complete_pos) {
return false;
}

virtual bool CanInterpret() = 0;

bool MatchesContext(ExecutionContext &exe_ctx);



























lldb::ExpressionResults Execute(DiagnosticManager &diagnostic_manager,
ExecutionContext &exe_ctx,
const EvaluateExpressionOptions &options,
lldb::UserExpressionSP &shared_ptr_to_me,
lldb::ExpressionVariableSP &result);




























virtual bool FinalizeJITExecution(
DiagnosticManager &diagnostic_manager, ExecutionContext &exe_ctx,
lldb::ExpressionVariableSP &result,
lldb::addr_t function_stack_bottom = LLDB_INVALID_ADDRESS,
lldb::addr_t function_stack_top = LLDB_INVALID_ADDRESS) = 0;


const char *Text() override { return m_expr_text.c_str(); }


const char *GetUserText() { return m_expr_text.c_str(); }



const char *FunctionName() override { return "$__lldb_expr"; }



lldb::LanguageType Language() const override { return m_language; }



ResultType DesiredResultType() override { return m_desired_type; }


bool NeedsValidation() override { return true; }


bool NeedsVariableResolution() override { return true; }

EvaluateExpressionOptions *GetOptions() override { return &m_options; }

virtual lldb::ExpressionVariableSP
GetResultAfterDematerialization(ExecutionContextScope *exe_scope) {
return lldb::ExpressionVariableSP();
}











































static lldb::ExpressionResults
Evaluate(ExecutionContext &exe_ctx, const EvaluateExpressionOptions &options,
llvm::StringRef expr_cstr, llvm::StringRef expr_prefix,
lldb::ValueObjectSP &result_valobj_sp, Status &error,
std::string *fixed_expression = nullptr,
ValueObject *ctx_obj = nullptr);

static const Status::ValueType kNoResult =
0x1001;


const char *GetFixedText() {
if (m_fixed_text.empty())
return nullptr;
return m_fixed_text.c_str();
}

protected:
virtual lldb::ExpressionResults
DoExecute(DiagnosticManager &diagnostic_manager, ExecutionContext &exe_ctx,
const EvaluateExpressionOptions &options,
lldb::UserExpressionSP &shared_ptr_to_me,
lldb::ExpressionVariableSP &result) = 0;

static lldb::addr_t GetObjectPointer(lldb::StackFrameSP frame_sp,
ConstString &object_name, Status &err);




void InstallContext(ExecutionContext &exe_ctx);

bool LockAndCheckContext(ExecutionContext &exe_ctx, lldb::TargetSP &target_sp,
lldb::ProcessSP &process_sp,
lldb::StackFrameSP &frame_sp);

Address m_address;
std::string m_expr_text;
std::string m_expr_prefix;

std::string m_fixed_text;


lldb::LanguageType m_language;

ResultType m_desired_type;

EvaluateExpressionOptions
m_options;
};

}

#endif
