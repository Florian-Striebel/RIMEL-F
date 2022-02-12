








#if !defined(LLDB_TARGET_THREADPLANCALLUSEREXPRESSION_H)
#define LLDB_TARGET_THREADPLANCALLUSEREXPRESSION_H

#include "lldb/Target/Thread.h"
#include "lldb/Target/ThreadPlan.h"
#include "lldb/Target/ThreadPlanCallFunction.h"
#include "lldb/lldb-private.h"

#include "llvm/ADT/ArrayRef.h"

namespace lldb_private {

class ThreadPlanCallUserExpression : public ThreadPlanCallFunction {
public:
ThreadPlanCallUserExpression(Thread &thread, Address &function,
llvm::ArrayRef<lldb::addr_t> args,
const EvaluateExpressionOptions &options,
lldb::UserExpressionSP &user_expression_sp);

~ThreadPlanCallUserExpression() override;

void GetDescription(Stream *s, lldb::DescriptionLevel level) override;

void DidPush() override;

void WillPop() override;

lldb::StopInfoSP GetRealStopInfo() override;

bool MischiefManaged() override;

void TransferExpressionOwnership() { m_manage_materialization = true; }

lldb::ExpressionVariableSP GetExpressionVariable() override {
return m_result_var_sp;
}

protected:
void DoTakedown(bool success) override;
private:
lldb::UserExpressionSP
m_user_expression_sp;


bool m_manage_materialization = false;
lldb::ExpressionVariableSP
m_result_var_sp;


ThreadPlanCallUserExpression(const ThreadPlanCallUserExpression &) = delete;
const ThreadPlanCallUserExpression &
operator=(const ThreadPlanCallUserExpression &) = delete;
};

}

#endif
