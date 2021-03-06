







#if !defined(LLDB_EXPRESSION_FUNCTIONCALLER_H)
#define LLDB_EXPRESSION_FUNCTIONCALLER_H

#include <list>
#include <memory>
#include <string>
#include <vector>

#include "lldb/Core/Address.h"
#include "lldb/Core/Value.h"
#include "lldb/Expression/Expression.h"
#include "lldb/Expression/ExpressionParser.h"
#include "lldb/Symbol/CompilerType.h"

namespace lldb_private {
































class FunctionCaller : public Expression {

static char ID;

public:
bool isA(const void *ClassID) const override { return ClassID == &ID; }
static bool classof(const Expression *obj) { return obj->isA(&ID); }

















FunctionCaller(ExecutionContextScope &exe_scope,
const CompilerType &return_type,
const Address &function_address,
const ValueList &arg_value_list, const char *name);


~FunctionCaller() override;













virtual unsigned CompileFunction(lldb::ThreadSP thread_to_use_sp,
DiagnosticManager &diagnostic_manager) = 0;

















bool InsertFunction(ExecutionContext &exe_ctx, lldb::addr_t &args_addr_ref,
DiagnosticManager &diagnostic_manager);












bool WriteFunctionWrapper(ExecutionContext &exe_ctx,
DiagnosticManager &diagnostic_manager);

















bool WriteFunctionArguments(ExecutionContext &exe_ctx,
lldb::addr_t &args_addr_ref,
DiagnosticManager &diagnostic_manager);





















bool WriteFunctionArguments(ExecutionContext &exe_ctx,
lldb::addr_t &args_addr_ref,
ValueList &arg_values,
DiagnosticManager &diagnostic_manager);
































lldb::ExpressionResults
ExecuteFunction(ExecutionContext &exe_ctx, lldb::addr_t *args_addr_ptr,
const EvaluateExpressionOptions &options,
DiagnosticManager &diagnostic_manager, Value &results);
















lldb::ThreadPlanSP
GetThreadPlanToCallFunction(ExecutionContext &exe_ctx, lldb::addr_t args_addr,
const EvaluateExpressionOptions &options,
DiagnosticManager &diagnostic_manager);














bool FetchFunctionResults(ExecutionContext &exe_ctx, lldb::addr_t args_addr,
Value &ret_value);









void DeallocateFunctionResults(ExecutionContext &exe_ctx,
lldb::addr_t args_addr);





const char *Text() override { return m_wrapper_function_text.c_str(); }



const char *FunctionName() override {
return m_wrapper_function_name.c_str();
}



ExpressionVariableList *LocalVariables() { return nullptr; }


bool NeedsValidation() override { return false; }


bool NeedsVariableResolution() override { return false; }

ValueList GetArgumentValues() const { return m_arg_values; }

protected:


std::shared_ptr<IRExecutionUnit> m_execution_unit_sp;
std::unique_ptr<ExpressionParser>
m_parser;



lldb::ModuleWP m_jit_module_wp;
std::string
m_name;

Function *m_function_ptr;


Address m_function_addr;

CompilerType m_function_return_type;


std::string m_wrapper_function_name;
std::string
m_wrapper_function_text;
std::string m_wrapper_struct_name;


std::list<lldb::addr_t> m_wrapper_args_addrs;



bool m_struct_valid;



size_t m_struct_size;
std::vector<uint64_t>
m_member_offsets;
uint64_t m_return_size;
uint64_t m_return_offset;


ValueList m_arg_values;

bool m_compiled;
bool
m_JITted;
};

}

#endif
