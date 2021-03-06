







#if !defined(LLDB_EXPRESSION_LLVMUSEREXPRESSION_H)
#define LLDB_EXPRESSION_LLVMUSEREXPRESSION_H

#include <map>
#include <string>
#include <vector>

#include "llvm/IR/LegacyPassManager.h"

#include "lldb/Expression/UserExpression.h"

namespace lldb_private {











class LLVMUserExpression : public UserExpression {

static char ID;

public:
bool isA(const void *ClassID) const override {
return ClassID == &ID || UserExpression::isA(ClassID);
}
static bool classof(const Expression *obj) { return obj->isA(&ID); }







struct IRPasses {
IRPasses() : EarlyPasses(nullptr), LatePasses(nullptr){};
std::shared_ptr<llvm::legacy::PassManager> EarlyPasses;
std::shared_ptr<llvm::legacy::PassManager> LatePasses;
};

LLVMUserExpression(ExecutionContextScope &exe_scope, llvm::StringRef expr,
llvm::StringRef prefix, lldb::LanguageType language,
ResultType desired_type,
const EvaluateExpressionOptions &options);
~LLVMUserExpression() override;

bool FinalizeJITExecution(
DiagnosticManager &diagnostic_manager, ExecutionContext &exe_ctx,
lldb::ExpressionVariableSP &result,
lldb::addr_t function_stack_bottom = LLDB_INVALID_ADDRESS,
lldb::addr_t function_stack_top = LLDB_INVALID_ADDRESS) override;

bool CanInterpret() override { return m_can_interpret; }

Materializer *GetMaterializer() override { return m_materializer_up.get(); }



const char *Text() override { return m_transformed_text.c_str(); }

protected:
lldb::ExpressionResults
DoExecute(DiagnosticManager &diagnostic_manager, ExecutionContext &exe_ctx,
const EvaluateExpressionOptions &options,
lldb::UserExpressionSP &shared_ptr_to_me,
lldb::ExpressionVariableSP &result) override;

virtual void ScanContext(ExecutionContext &exe_ctx,
lldb_private::Status &err) = 0;

bool PrepareToExecuteJITExpression(DiagnosticManager &diagnostic_manager,
ExecutionContext &exe_ctx,
lldb::addr_t &struct_address);

virtual bool AddArguments(ExecutionContext &exe_ctx,
std::vector<lldb::addr_t> &args,
lldb::addr_t struct_address,
DiagnosticManager &diagnostic_manager) = 0;

lldb::addr_t
m_stack_frame_bottom;
lldb::addr_t m_stack_frame_top;

bool m_allow_cxx;
bool m_allow_objc;
std::string
m_transformed_text;

std::shared_ptr<IRExecutionUnit>
m_execution_unit_sp;
std::unique_ptr<Materializer> m_materializer_up;


lldb::ModuleWP m_jit_module_wp;
Target *m_target;


bool m_can_interpret;

lldb::addr_t m_materialized_address;


Materializer::DematerializerSP m_dematerializer_sp;
};

}
#endif
