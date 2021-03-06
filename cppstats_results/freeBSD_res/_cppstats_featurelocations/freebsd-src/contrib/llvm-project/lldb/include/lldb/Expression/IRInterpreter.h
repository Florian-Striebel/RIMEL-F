







#if !defined(LLDB_EXPRESSION_IRINTERPRETER_H)
#define LLDB_EXPRESSION_IRINTERPRETER_H

#include "lldb/Utility/ConstString.h"
#include "lldb/Utility/Stream.h"
#include "lldb/lldb-public.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/Pass.h"

namespace llvm {
class Function;
class Module;
}

namespace lldb_private {

class IRMemoryMap;
}








class IRInterpreter {
public:
static bool CanInterpret(llvm::Module &module, llvm::Function &function,
lldb_private::Status &error,
const bool support_function_calls);

static bool Interpret(llvm::Module &module, llvm::Function &function,
llvm::ArrayRef<lldb::addr_t> args,
lldb_private::IRExecutionUnit &execution_unit,
lldb_private::Status &error,
lldb::addr_t stack_frame_bottom,
lldb::addr_t stack_frame_top,
lldb_private::ExecutionContext &exe_ctx);

private:
static bool supportsFunction(llvm::Function &llvm_function,
lldb_private::Status &err);
};

#endif
