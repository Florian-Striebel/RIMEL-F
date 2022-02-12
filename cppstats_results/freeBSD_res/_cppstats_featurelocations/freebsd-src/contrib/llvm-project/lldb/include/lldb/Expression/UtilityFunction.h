








#if !defined(LLDB_EXPRESSION_UTILITYFUNCTION_H)
#define LLDB_EXPRESSION_UTILITYFUNCTION_H

#include <memory>
#include <string>

#include "lldb/Expression/Expression.h"
#include "lldb/lldb-forward.h"
#include "lldb/lldb-private.h"

namespace lldb_private {









class UtilityFunction : public Expression {

static char ID;

public:
bool isA(const void *ClassID) const override { return ClassID == &ID; }
static bool classof(const Expression *obj) { return obj->isA(&ID); }











UtilityFunction(ExecutionContextScope &exe_scope, std::string text,
std::string name, bool enable_debugging);

~UtilityFunction() override;











virtual bool Install(DiagnosticManager &diagnostic_manager,
ExecutionContext &exe_ctx) = 0;












bool ContainsAddress(lldb::addr_t address) {


return (address >= m_jit_start_addr && address < m_jit_end_addr);
}



const char *Text() override { return m_function_text.c_str(); }



const char *FunctionName() override { return m_function_name.c_str(); }



ExpressionVariableList *LocalVariables() { return nullptr; }


bool NeedsValidation() override { return false; }


bool NeedsVariableResolution() override { return false; }




FunctionCaller *MakeFunctionCaller(const CompilerType &return_type,
const ValueList &arg_value_list,
lldb::ThreadSP compilation_thread,
Status &error);



FunctionCaller *GetFunctionCaller() { return m_caller_up.get(); }

protected:
std::shared_ptr<IRExecutionUnit> m_execution_unit_sp;
lldb::ModuleWP m_jit_module_wp;

std::string m_function_text;

std::string m_function_name;
std::unique_ptr<FunctionCaller> m_caller_up;
};

}

#endif
