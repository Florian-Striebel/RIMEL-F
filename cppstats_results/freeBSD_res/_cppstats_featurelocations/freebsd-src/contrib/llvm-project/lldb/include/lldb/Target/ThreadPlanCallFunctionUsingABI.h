








#if !defined(LLDB_TARGET_THREADPLANCALLFUNCTIONUSINGABI_H)
#define LLDB_TARGET_THREADPLANCALLFUNCTIONUSINGABI_H

#include "lldb/Target/ABI.h"
#include "lldb/Target/Thread.h"
#include "lldb/Target/ThreadPlanCallFunction.h"
#include "lldb/lldb-private.h"

#include "llvm/ADT/ArrayRef.h"
#include "llvm/IR/DerivedTypes.h"

namespace lldb_private {

class ThreadPlanCallFunctionUsingABI : public ThreadPlanCallFunction {






public:
ThreadPlanCallFunctionUsingABI(Thread &thread,
const Address &function_address,
llvm::Type &function_prototype,
llvm::Type &return_type,
llvm::ArrayRef<ABI::CallArgument> args,
const EvaluateExpressionOptions &options);

~ThreadPlanCallFunctionUsingABI() override;

void GetDescription(Stream *s, lldb::DescriptionLevel level) override;

protected:
void SetReturnValue() override;

private:
llvm::Type &m_return_type;
ThreadPlanCallFunctionUsingABI(const ThreadPlanCallFunctionUsingABI &) =
delete;
const ThreadPlanCallFunctionUsingABI &
operator=(const ThreadPlanCallFunctionUsingABI &) = delete;
};

}

#endif
