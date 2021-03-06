








#if !defined(LLDB_TARGET_LANGUAGERUNTIME_H)
#define LLDB_TARGET_LANGUAGERUNTIME_H

#include "lldb/Breakpoint/BreakpointResolver.h"
#include "lldb/Breakpoint/BreakpointResolverName.h"
#include "lldb/Core/PluginInterface.h"
#include "lldb/Core/Value.h"
#include "lldb/Core/ValueObject.h"
#include "lldb/Expression/LLVMUserExpression.h"
#include "lldb/Symbol/DeclVendor.h"
#include "lldb/Target/ExecutionContextScope.h"
#include "lldb/Target/Runtime.h"
#include "lldb/lldb-private.h"
#include "lldb/lldb-public.h"

namespace lldb_private {

class ExceptionSearchFilter : public SearchFilter {
public:
ExceptionSearchFilter(const lldb::TargetSP &target_sp,
lldb::LanguageType language,
bool update_module_list = true);

~ExceptionSearchFilter() override = default;

bool ModulePasses(const lldb::ModuleSP &module_sp) override;

bool ModulePasses(const FileSpec &spec) override;

void Search(Searcher &searcher) override;

void GetDescription(Stream *s) override;

static SearchFilter *
CreateFromStructuredData(Target &target,
const StructuredData::Dictionary &data_dict,
Status &error);

StructuredData::ObjectSP SerializeToStructuredData() override;

protected:
lldb::LanguageType m_language;
LanguageRuntime *m_language_runtime;
lldb::SearchFilterSP m_filter_sp;

lldb::SearchFilterSP DoCreateCopy() override;

void UpdateModuleListIfNeeded();
};

class LanguageRuntime : public Runtime, public PluginInterface {
public:
static LanguageRuntime *FindPlugin(Process *process,
lldb::LanguageType language);

static void InitializeCommands(CommandObject *parent);

virtual lldb::LanguageType GetLanguageType() const = 0;

virtual bool GetObjectDescription(Stream &str, ValueObject &object) = 0;

virtual bool GetObjectDescription(Stream &str, Value &value,
ExecutionContextScope *exe_scope) = 0;


virtual bool GetDynamicTypeAndAddress(ValueObject &in_value,
lldb::DynamicValueType use_dynamic,
TypeAndOrName &class_type_or_name,
Address &address,
Value::ValueType &value_type) = 0;




virtual CompilerType GetConcreteType(ExecutionContextScope *exe_scope,
ConstString abstract_type_name) {
return CompilerType();
}



virtual bool CouldHaveDynamicValue(ValueObject &in_value) = 0;






virtual TypeAndOrName FixUpDynamicType(const TypeAndOrName &type_and_or_name,
ValueObject &static_value) = 0;

virtual void SetExceptionBreakpoints() {}

virtual void ClearExceptionBreakpoints() {}

virtual bool ExceptionBreakpointsAreSet() { return false; }

virtual bool ExceptionBreakpointsExplainStop(lldb::StopInfoSP stop_reason) {
return false;
}

static lldb::BreakpointSP
CreateExceptionBreakpoint(Target &target, lldb::LanguageType language,
bool catch_bp, bool throw_bp,
bool is_internal = false);

static lldb::BreakpointPreconditionSP
GetExceptionPrecondition(lldb::LanguageType language, bool throw_bp);

virtual lldb::ValueObjectSP GetExceptionObjectForThread(
lldb::ThreadSP thread_sp) {
return lldb::ValueObjectSP();
}

virtual lldb::ThreadSP GetBacktraceThreadFromException(
lldb::ValueObjectSP thread_sp) {
return lldb::ThreadSP();
}

virtual DeclVendor *GetDeclVendor() { return nullptr; }

virtual lldb::BreakpointResolverSP
CreateExceptionResolver(const lldb::BreakpointSP &bkpt,
bool catch_bp, bool throw_bp) = 0;

virtual lldb::SearchFilterSP CreateExceptionSearchFilter() {
return m_process->GetTarget().GetSearchFilterForModule(nullptr);
}

virtual bool GetTypeBitSize(const CompilerType &compiler_type,
uint64_t &size) {
return false;
}

virtual void SymbolsDidLoad(const ModuleList &module_list) { return; }

virtual lldb::ThreadPlanSP GetStepThroughTrampolinePlan(Thread &thread,
bool stop_others) = 0;



virtual bool IsAllowedRuntimeValue(ConstString name) { return false; }

virtual llvm::Optional<CompilerType> GetRuntimeType(CompilerType base_type) {
return llvm::None;
}

virtual void ModulesDidLoad(const ModuleList &module_list) override {}




virtual bool GetIRPasses(LLVMUserExpression::IRPasses &custom_passes) {
return false;
}




virtual lldb::addr_t LookupRuntimeSymbol(ConstString name) {
return LLDB_INVALID_ADDRESS;
}

virtual bool isA(const void *ClassID) const { return ClassID == &ID; }
static char ID;






























static lldb::UnwindPlanSP
GetRuntimeUnwindPlan(lldb_private::Thread &thread,
lldb_private::RegisterContext *regctx,
bool &behaves_like_zeroth_frame);

protected:



virtual lldb::UnwindPlanSP
GetRuntimeUnwindPlan(lldb::ProcessSP process_sp,
lldb_private::RegisterContext *regctx,
bool &behaves_like_zeroth_frame) {
return lldb::UnwindPlanSP();
}

LanguageRuntime(Process *process);
};

}

#endif
