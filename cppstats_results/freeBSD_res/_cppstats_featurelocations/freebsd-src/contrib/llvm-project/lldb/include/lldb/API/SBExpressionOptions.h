







#if !defined(LLDB_API_SBEXPRESSIONOPTIONS_H)
#define LLDB_API_SBEXPRESSIONOPTIONS_H

#include "lldb/API/SBDefines.h"

#include <vector>

namespace lldb {

class LLDB_API SBExpressionOptions {
public:
SBExpressionOptions();

SBExpressionOptions(const lldb::SBExpressionOptions &rhs);

~SBExpressionOptions();

const SBExpressionOptions &operator=(const lldb::SBExpressionOptions &rhs);

bool GetCoerceResultToId() const;

void SetCoerceResultToId(bool coerce = true);

bool GetUnwindOnError() const;

void SetUnwindOnError(bool unwind = true);

bool GetIgnoreBreakpoints() const;

void SetIgnoreBreakpoints(bool ignore = true);

lldb::DynamicValueType GetFetchDynamicValue() const;

void SetFetchDynamicValue(
lldb::DynamicValueType dynamic = lldb::eDynamicCanRunTarget);

uint32_t GetTimeoutInMicroSeconds() const;


void SetTimeoutInMicroSeconds(uint32_t timeout = 0);

uint32_t GetOneThreadTimeoutInMicroSeconds() const;




void SetOneThreadTimeoutInMicroSeconds(uint32_t timeout = 0);

bool GetTryAllThreads() const;

void SetTryAllThreads(bool run_others = true);

bool GetStopOthers() const;

void SetStopOthers(bool stop_others = true);

bool GetTrapExceptions() const;

void SetTrapExceptions(bool trap_exceptions = true);

void SetLanguage(lldb::LanguageType language);

void SetCancelCallback(lldb::ExpressionCancelCallback callback, void *baton);

bool GetGenerateDebugInfo();

void SetGenerateDebugInfo(bool b = true);

bool GetSuppressPersistentResult();

void SetSuppressPersistentResult(bool b = false);

const char *GetPrefix() const;

void SetPrefix(const char *prefix);

void SetAutoApplyFixIts(bool b = true);

bool GetAutoApplyFixIts();

void SetRetriesWithFixIts(uint64_t retries);

uint64_t GetRetriesWithFixIts();

bool GetTopLevel();

void SetTopLevel(bool b = true);


bool GetAllowJIT();


void SetAllowJIT(bool allow);

protected:
lldb_private::EvaluateExpressionOptions *get() const;

lldb_private::EvaluateExpressionOptions &ref() const;

friend class SBFrame;
friend class SBValue;
friend class SBTarget;

private:

mutable std::unique_ptr<lldb_private::EvaluateExpressionOptions> m_opaque_up;
};

}

#endif
