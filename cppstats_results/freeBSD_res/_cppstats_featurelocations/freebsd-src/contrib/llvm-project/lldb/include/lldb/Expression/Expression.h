







#if !defined(LLDB_EXPRESSION_EXPRESSION_H)
#define LLDB_EXPRESSION_EXPRESSION_H

#include <map>
#include <string>
#include <vector>


#include "lldb/Expression/ExpressionTypeSystemHelper.h"
#include "lldb/lldb-forward.h"
#include "lldb/lldb-private.h"

namespace lldb_private {

class RecordingMemoryManager;









class Expression {
public:
enum ResultType { eResultTypeAny, eResultTypeId };

Expression(Target &target);

Expression(ExecutionContextScope &exe_scope);


virtual ~Expression() = default;



virtual const char *Text() = 0;



virtual const char *FunctionName() = 0;



virtual lldb::LanguageType Language() const {
return lldb::eLanguageTypeUnknown;
}



virtual Materializer *GetMaterializer() { return nullptr; }



virtual ResultType DesiredResultType() { return eResultTypeAny; }




virtual bool NeedsValidation() = 0;


virtual bool NeedsVariableResolution() = 0;

virtual EvaluateExpressionOptions *GetOptions() { return nullptr; };



lldb::addr_t StartAddress() { return m_jit_start_addr; }


virtual void WillStartExecuting() {}


virtual void DidFinishExecuting() {}

virtual ExpressionTypeSystemHelper *GetTypeSystemHelper() { return nullptr; }


virtual bool isA(const void *ClassID) const = 0;

protected:
lldb::TargetWP m_target_wp;
lldb::ProcessWP m_jit_process_wp;


lldb::addr_t m_jit_start_addr;


lldb::addr_t m_jit_end_addr;


};

}

#endif
