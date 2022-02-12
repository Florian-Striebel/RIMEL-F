







#if !defined(LLDB_EXPRESSION_DYNAMICCHECKERFUNCTIONS_H)
#define LLDB_EXPRESSION_DYNAMICCHECKERFUNCTIONS_H

#include "lldb/lldb-types.h"

namespace lldb_private {

class DiagnosticManager;
class ExecutionContext;












class DynamicCheckerFunctions {
public:
enum DynamicCheckerFunctionsKind {
DCF_Clang,
};

DynamicCheckerFunctions(DynamicCheckerFunctionsKind kind) : m_kind(kind) {}
virtual ~DynamicCheckerFunctions() = default;













virtual bool Install(DiagnosticManager &diagnostic_manager,
ExecutionContext &exe_ctx) = 0;
virtual bool DoCheckersExplainStop(lldb::addr_t addr, Stream &message) = 0;

DynamicCheckerFunctionsKind GetKind() const { return m_kind; }

private:
const DynamicCheckerFunctionsKind m_kind;
};
}

#endif
