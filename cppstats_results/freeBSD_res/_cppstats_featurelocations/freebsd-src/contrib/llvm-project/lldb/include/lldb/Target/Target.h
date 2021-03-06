







#if !defined(LLDB_TARGET_TARGET_H)
#define LLDB_TARGET_TARGET_H

#include <list>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "lldb/Breakpoint/BreakpointList.h"
#include "lldb/Breakpoint/BreakpointName.h"
#include "lldb/Breakpoint/WatchpointList.h"
#include "lldb/Core/Architecture.h"
#include "lldb/Core/Disassembler.h"
#include "lldb/Core/ModuleList.h"
#include "lldb/Core/UserSettingsController.h"
#include "lldb/Expression/Expression.h"
#include "lldb/Host/ProcessLaunchInfo.h"
#include "lldb/Symbol/TypeSystem.h"
#include "lldb/Target/ExecutionContextScope.h"
#include "lldb/Target/PathMappingList.h"
#include "lldb/Target/SectionLoadHistory.h"
#include "lldb/Target/ThreadSpec.h"
#include "lldb/Utility/ArchSpec.h"
#include "lldb/Utility/Broadcaster.h"
#include "lldb/Utility/LLDBAssert.h"
#include "lldb/Utility/Timeout.h"
#include "lldb/lldb-public.h"

namespace lldb_private {

OptionEnumValues GetDynamicValueTypes();

enum InlineStrategy {
eInlineBreakpointsNever = 0,
eInlineBreakpointsHeaders,
eInlineBreakpointsAlways
};

enum LoadScriptFromSymFile {
eLoadScriptFromSymFileTrue,
eLoadScriptFromSymFileFalse,
eLoadScriptFromSymFileWarn
};

enum LoadCWDlldbinitFile {
eLoadCWDlldbinitTrue,
eLoadCWDlldbinitFalse,
eLoadCWDlldbinitWarn
};

enum LoadDependentFiles {
eLoadDependentsDefault,
eLoadDependentsYes,
eLoadDependentsNo,
};

enum ImportStdModule {
eImportStdModuleFalse,
eImportStdModuleFallback,
eImportStdModuleTrue,
};

class TargetExperimentalProperties : public Properties {
public:
TargetExperimentalProperties();
};

class TargetProperties : public Properties {
public:
TargetProperties(Target *target);

~TargetProperties() override;

ArchSpec GetDefaultArchitecture() const;

void SetDefaultArchitecture(const ArchSpec &arch);

bool GetMoveToNearestCode() const;

lldb::DynamicValueType GetPreferDynamicValue() const;

bool SetPreferDynamicValue(lldb::DynamicValueType d);

bool GetPreloadSymbols() const;

void SetPreloadSymbols(bool b);

bool GetDisableASLR() const;

void SetDisableASLR(bool b);

bool GetInheritTCC() const;

void SetInheritTCC(bool b);

bool GetDetachOnError() const;

void SetDetachOnError(bool b);

bool GetDisableSTDIO() const;

void SetDisableSTDIO(bool b);

const char *GetDisassemblyFlavor() const;

InlineStrategy GetInlineStrategy() const;

llvm::StringRef GetArg0() const;

void SetArg0(llvm::StringRef arg);

bool GetRunArguments(Args &args) const;

void SetRunArguments(const Args &args);

Environment GetEnvironment() const;
void SetEnvironment(Environment env);

bool GetSkipPrologue() const;

PathMappingList &GetSourcePathMap() const;

FileSpecList GetExecutableSearchPaths();

void AppendExecutableSearchPaths(const FileSpec &);

FileSpecList GetDebugFileSearchPaths();

FileSpecList GetClangModuleSearchPaths();

bool GetEnableAutoImportClangModules() const;

ImportStdModule GetImportStdModule() const;

bool GetEnableAutoApplyFixIts() const;

uint64_t GetNumberOfRetriesWithFixits() const;

bool GetEnableNotifyAboutFixIts() const;

bool GetEnableSaveObjects() const;

bool GetEnableSyntheticValue() const;

uint32_t GetMaxZeroPaddingInFloatFormat() const;

uint32_t GetMaximumNumberOfChildrenToDisplay() const;

uint32_t GetMaximumSizeOfStringSummary() const;

uint32_t GetMaximumMemReadSize() const;

FileSpec GetStandardInputPath() const;
FileSpec GetStandardErrorPath() const;
FileSpec GetStandardOutputPath() const;

void SetStandardInputPath(llvm::StringRef path);
void SetStandardOutputPath(llvm::StringRef path);
void SetStandardErrorPath(llvm::StringRef path);

void SetStandardInputPath(const char *path) = delete;
void SetStandardOutputPath(const char *path) = delete;
void SetStandardErrorPath(const char *path) = delete;

bool GetBreakpointsConsultPlatformAvoidList();

lldb::LanguageType GetLanguage() const;

llvm::StringRef GetExpressionPrefixContents();

uint64_t GetExprErrorLimit() const;

bool GetUseHexImmediates() const;

bool GetUseFastStepping() const;

bool GetDisplayExpressionsInCrashlogs() const;

LoadScriptFromSymFile GetLoadScriptFromSymbolFile() const;

LoadCWDlldbinitFile GetLoadCWDlldbinitFile() const;

Disassembler::HexImmediateStyle GetHexImmediateStyle() const;

MemoryModuleLoadLevel GetMemoryModuleLoadLevel() const;

bool GetUserSpecifiedTrapHandlerNames(Args &args) const;

void SetUserSpecifiedTrapHandlerNames(const Args &args);

bool GetNonStopModeEnabled() const;

void SetNonStopModeEnabled(bool b);

bool GetDisplayRuntimeSupportValues() const;

void SetDisplayRuntimeSupportValues(bool b);

bool GetDisplayRecognizedArguments() const;

void SetDisplayRecognizedArguments(bool b);

const ProcessLaunchInfo &GetProcessLaunchInfo() const;

void SetProcessLaunchInfo(const ProcessLaunchInfo &launch_info);

bool GetInjectLocalVariables(ExecutionContext *exe_ctx) const;

void SetInjectLocalVariables(ExecutionContext *exe_ctx, bool b);

void SetRequireHardwareBreakpoints(bool b);

bool GetRequireHardwareBreakpoints() const;

bool GetAutoInstallMainExecutable() const;

void UpdateLaunchInfoFromProperties();

void SetDebugUtilityExpression(bool debug);

bool GetDebugUtilityExpression() const;

private:

void Arg0ValueChangedCallback();
void RunArgsValueChangedCallback();
void EnvVarsValueChangedCallback();
void InputPathValueChangedCallback();
void OutputPathValueChangedCallback();
void ErrorPathValueChangedCallback();
void DetachOnErrorValueChangedCallback();
void DisableASLRValueChangedCallback();
void InheritTCCValueChangedCallback();
void DisableSTDIOValueChangedCallback();

Environment ComputeEnvironment() const;


ProcessLaunchInfo m_launch_info;
std::unique_ptr<TargetExperimentalProperties> m_experimental_properties_up;
Target *m_target;
};

class EvaluateExpressionOptions {
public:




#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 4268)
#endif
static constexpr std::chrono::milliseconds default_timeout{500};
#if defined(_MSC_VER)
#pragma warning(pop)
#endif

static constexpr ExecutionPolicy default_execution_policy =
eExecutionPolicyOnlyWhenNeeded;

EvaluateExpressionOptions() = default;

ExecutionPolicy GetExecutionPolicy() const { return m_execution_policy; }

void SetExecutionPolicy(ExecutionPolicy policy = eExecutionPolicyAlways) {
m_execution_policy = policy;
}

lldb::LanguageType GetLanguage() const { return m_language; }

void SetLanguage(lldb::LanguageType language) { m_language = language; }

bool DoesCoerceToId() const { return m_coerce_to_id; }

const char *GetPrefix() const {
return (m_prefix.empty() ? nullptr : m_prefix.c_str());
}

void SetPrefix(const char *prefix) {
if (prefix && prefix[0])
m_prefix = prefix;
else
m_prefix.clear();
}

void SetCoerceToId(bool coerce = true) { m_coerce_to_id = coerce; }

bool DoesUnwindOnError() const { return m_unwind_on_error; }

void SetUnwindOnError(bool unwind = false) { m_unwind_on_error = unwind; }

bool DoesIgnoreBreakpoints() const { return m_ignore_breakpoints; }

void SetIgnoreBreakpoints(bool ignore = false) {
m_ignore_breakpoints = ignore;
}

bool DoesKeepInMemory() const { return m_keep_in_memory; }

void SetKeepInMemory(bool keep = true) { m_keep_in_memory = keep; }

lldb::DynamicValueType GetUseDynamic() const { return m_use_dynamic; }

void
SetUseDynamic(lldb::DynamicValueType dynamic = lldb::eDynamicCanRunTarget) {
m_use_dynamic = dynamic;
}

const Timeout<std::micro> &GetTimeout() const { return m_timeout; }

void SetTimeout(const Timeout<std::micro> &timeout) { m_timeout = timeout; }

const Timeout<std::micro> &GetOneThreadTimeout() const {
return m_one_thread_timeout;
}

void SetOneThreadTimeout(const Timeout<std::micro> &timeout) {
m_one_thread_timeout = timeout;
}

bool GetTryAllThreads() const { return m_try_others; }

void SetTryAllThreads(bool try_others = true) { m_try_others = try_others; }

bool GetStopOthers() const { return m_stop_others; }

void SetStopOthers(bool stop_others = true) { m_stop_others = stop_others; }

bool GetDebug() const { return m_debug; }

void SetDebug(bool b) {
m_debug = b;
if (m_debug)
m_generate_debug_info = true;
}

bool GetGenerateDebugInfo() const { return m_generate_debug_info; }

void SetGenerateDebugInfo(bool b) { m_generate_debug_info = b; }

bool GetColorizeErrors() const { return m_ansi_color_errors; }

void SetColorizeErrors(bool b) { m_ansi_color_errors = b; }

bool GetTrapExceptions() const { return m_trap_exceptions; }

void SetTrapExceptions(bool b) { m_trap_exceptions = b; }

bool GetREPLEnabled() const { return m_repl; }

void SetREPLEnabled(bool b) { m_repl = b; }

void SetCancelCallback(lldb::ExpressionCancelCallback callback, void *baton) {
m_cancel_callback_baton = baton;
m_cancel_callback = callback;
}

bool InvokeCancelCallback(lldb::ExpressionEvaluationPhase phase) const {
return ((m_cancel_callback != nullptr)
? m_cancel_callback(phase, m_cancel_callback_baton)
: false);
}



void SetPoundLine(const char *path, uint32_t line) const {
if (path && path[0]) {
m_pound_line_file = path;
m_pound_line_line = line;
} else {
m_pound_line_file.clear();
m_pound_line_line = 0;
}
}

const char *GetPoundLineFilePath() const {
return (m_pound_line_file.empty() ? nullptr : m_pound_line_file.c_str());
}

uint32_t GetPoundLineLine() const { return m_pound_line_line; }

void SetResultIsInternal(bool b) { m_result_is_internal = b; }

bool GetResultIsInternal() const { return m_result_is_internal; }

void SetAutoApplyFixIts(bool b) { m_auto_apply_fixits = b; }

bool GetAutoApplyFixIts() const { return m_auto_apply_fixits; }

void SetRetriesWithFixIts(uint64_t number_of_retries) {
m_retries_with_fixits = number_of_retries;
}

uint64_t GetRetriesWithFixIts() const { return m_retries_with_fixits; }

bool IsForUtilityExpr() const { return m_running_utility_expression; }

void SetIsForUtilityExpr(bool b) { m_running_utility_expression = b; }

private:
ExecutionPolicy m_execution_policy = default_execution_policy;
lldb::LanguageType m_language = lldb::eLanguageTypeUnknown;
std::string m_prefix;
bool m_coerce_to_id = false;
bool m_unwind_on_error = true;
bool m_ignore_breakpoints = false;
bool m_keep_in_memory = false;
bool m_try_others = true;
bool m_stop_others = true;
bool m_debug = false;
bool m_trap_exceptions = true;
bool m_repl = false;
bool m_generate_debug_info = false;
bool m_ansi_color_errors = false;
bool m_result_is_internal = false;
bool m_auto_apply_fixits = true;
uint64_t m_retries_with_fixits = 1;


bool m_running_utility_expression = false;

lldb::DynamicValueType m_use_dynamic = lldb::eNoDynamicValues;
Timeout<std::micro> m_timeout = default_timeout;
Timeout<std::micro> m_one_thread_timeout = llvm::None;
lldb::ExpressionCancelCallback m_cancel_callback = nullptr;
void *m_cancel_callback_baton = nullptr;



mutable std::string m_pound_line_file;
mutable uint32_t m_pound_line_line;
};


class Target : public std::enable_shared_from_this<Target>,
public TargetProperties,
public Broadcaster,
public ExecutionContextScope,
public ModuleList::Notifier {
public:
friend class TargetList;
friend class Debugger;


enum {
eBroadcastBitBreakpointChanged = (1 << 0),
eBroadcastBitModulesLoaded = (1 << 1),
eBroadcastBitModulesUnloaded = (1 << 2),
eBroadcastBitWatchpointChanged = (1 << 3),
eBroadcastBitSymbolsLoaded = (1 << 4)
};



static ConstString &GetStaticBroadcasterClass();

ConstString &GetBroadcasterClass() const override {
return GetStaticBroadcasterClass();
}



class TargetEventData : public EventData {
public:
TargetEventData(const lldb::TargetSP &target_sp);

TargetEventData(const lldb::TargetSP &target_sp,
const ModuleList &module_list);

~TargetEventData() override;

static ConstString GetFlavorString();

ConstString GetFlavor() const override {
return TargetEventData::GetFlavorString();
}

void Dump(Stream *s) const override;

static const TargetEventData *GetEventDataFromEvent(const Event *event_ptr);

static lldb::TargetSP GetTargetFromEvent(const Event *event_ptr);

static ModuleList GetModuleListFromEvent(const Event *event_ptr);

const lldb::TargetSP &GetTarget() const { return m_target_sp; }

const ModuleList &GetModuleList() const { return m_module_list; }

private:
lldb::TargetSP m_target_sp;
ModuleList m_module_list;

TargetEventData(const TargetEventData &) = delete;
const TargetEventData &operator=(const TargetEventData &) = delete;
};

~Target() override;

static void SettingsInitialize();

static void SettingsTerminate();

static FileSpecList GetDefaultExecutableSearchPaths();

static FileSpecList GetDefaultDebugFileSearchPaths();

static ArchSpec GetDefaultArchitecture();

static void SetDefaultArchitecture(const ArchSpec &arch);

bool IsDummyTarget() const { return m_is_dummy_target; }
































lldb::ModuleSP GetOrCreateModule(const ModuleSpec &module_spec, bool notify,
Status *error_ptr = nullptr);



static const lldb::TargetPropertiesSP &GetGlobalProperties();

std::recursive_mutex &GetAPIMutex();

void DeleteCurrentProcess();

void CleanupProcess();











void Dump(Stream *s, lldb::DescriptionLevel description_level);



const lldb::ProcessSP &CreateProcess(lldb::ListenerSP listener_sp,
llvm::StringRef plugin_name,
const FileSpec *crash_file,
bool can_connect);

const lldb::ProcessSP &GetProcessSP() const;

bool IsValid() { return m_valid; }

void Destroy();

Status Launch(ProcessLaunchInfo &launch_info,
Stream *stream);

Status Attach(ProcessAttachInfo &attach_info,
Stream *stream);



BreakpointList &GetBreakpointList(bool internal = false);

const BreakpointList &GetBreakpointList(bool internal = false) const;

lldb::BreakpointSP GetLastCreatedBreakpoint() {
return m_last_created_breakpoint;
}

lldb::BreakpointSP GetBreakpointByID(lldb::break_id_t break_id);



lldb::BreakpointSP CreateBreakpoint(const FileSpecList *containingModules,
const FileSpec &file, uint32_t line_no,
uint32_t column, lldb::addr_t offset,
LazyBool check_inlines,
LazyBool skip_prologue, bool internal,
bool request_hardware,
LazyBool move_to_nearest_code);




lldb::BreakpointSP CreateSourceRegexBreakpoint(
const FileSpecList *containingModules,
const FileSpecList *source_file_list,
const std::unordered_set<std::string> &function_names,
RegularExpression source_regex, bool internal, bool request_hardware,
LazyBool move_to_nearest_code);


lldb::BreakpointSP CreateBreakpoint(lldb::addr_t load_addr, bool internal,
bool request_hardware);


lldb::BreakpointSP CreateAddressInModuleBreakpoint(lldb::addr_t file_addr,
bool internal,
const FileSpec *file_spec,
bool request_hardware);


lldb::BreakpointSP CreateBreakpoint(const Address &addr, bool internal,
bool request_hardware);





lldb::BreakpointSP CreateFuncRegexBreakpoint(
const FileSpecList *containingModules,
const FileSpecList *containingSourceFiles, RegularExpression func_regexp,
lldb::LanguageType requested_language, LazyBool skip_prologue,
bool internal, bool request_hardware);






lldb::BreakpointSP CreateBreakpoint(
const FileSpecList *containingModules,
const FileSpecList *containingSourceFiles, const char *func_name,
lldb::FunctionNameType func_name_type_mask, lldb::LanguageType language,
lldb::addr_t offset, LazyBool skip_prologue, bool internal,
bool request_hardware);

lldb::BreakpointSP
CreateExceptionBreakpoint(enum lldb::LanguageType language, bool catch_bp,
bool throw_bp, bool internal,
Args *additional_args = nullptr,
Status *additional_args_error = nullptr);

lldb::BreakpointSP CreateScriptedBreakpoint(
const llvm::StringRef class_name, const FileSpecList *containingModules,
const FileSpecList *containingSourceFiles, bool internal,
bool request_hardware, StructuredData::ObjectSP extra_args_sp,
Status *creation_error = nullptr);






lldb::BreakpointSP CreateBreakpoint(
const FileSpecList *containingModules,
const FileSpecList *containingSourceFiles, const char *func_names[],
size_t num_names, lldb::FunctionNameType func_name_type_mask,
lldb::LanguageType language, lldb::addr_t offset, LazyBool skip_prologue,
bool internal, bool request_hardware);

lldb::BreakpointSP
CreateBreakpoint(const FileSpecList *containingModules,
const FileSpecList *containingSourceFiles,
const std::vector<std::string> &func_names,
lldb::FunctionNameType func_name_type_mask,
lldb::LanguageType language, lldb::addr_t m_offset,
LazyBool skip_prologue, bool internal,
bool request_hardware);


lldb::BreakpointSP CreateBreakpoint(lldb::SearchFilterSP &filter_sp,
lldb::BreakpointResolverSP &resolver_sp,
bool internal, bool request_hardware,
bool resolve_indirect_symbols);


lldb::WatchpointSP CreateWatchpoint(lldb::addr_t addr, size_t size,
const CompilerType *type, uint32_t kind,
Status &error);

lldb::WatchpointSP GetLastCreatedWatchpoint() {
return m_last_created_watchpoint;
}

WatchpointList &GetWatchpointList() { return m_watchpoint_list; }


void AddNameToBreakpoint(BreakpointID &id, const char *name, Status &error);

void AddNameToBreakpoint(lldb::BreakpointSP &bp_sp, const char *name,
Status &error);

void RemoveNameFromBreakpoint(lldb::BreakpointSP &bp_sp, ConstString name);

BreakpointName *FindBreakpointName(ConstString name, bool can_create,
Status &error);

void DeleteBreakpointName(ConstString name);

void ConfigureBreakpointName(BreakpointName &bp_name,
const BreakpointOptions &options,
const BreakpointName::Permissions &permissions);
void ApplyNameToBreakpoints(BreakpointName &bp_name);


void AddBreakpointName(BreakpointName *bp_name);

void GetBreakpointNames(std::vector<std::string> &names);


void RemoveAllBreakpoints(bool internal_also = false);


void RemoveAllowedBreakpoints();

void DisableAllBreakpoints(bool internal_also = false);

void DisableAllowedBreakpoints();

void EnableAllBreakpoints(bool internal_also = false);

void EnableAllowedBreakpoints();

bool DisableBreakpointByID(lldb::break_id_t break_id);

bool EnableBreakpointByID(lldb::break_id_t break_id);

bool RemoveBreakpointByID(lldb::break_id_t break_id);




bool RemoveAllWatchpoints(bool end_to_end = true);

bool DisableAllWatchpoints(bool end_to_end = true);

bool EnableAllWatchpoints(bool end_to_end = true);

bool ClearAllWatchpointHitCounts();

bool ClearAllWatchpointHistoricValues();

bool IgnoreAllWatchpoints(uint32_t ignore_count);

bool DisableWatchpointByID(lldb::watch_id_t watch_id);

bool EnableWatchpointByID(lldb::watch_id_t watch_id);

bool RemoveWatchpointByID(lldb::watch_id_t watch_id);

bool IgnoreWatchpointByID(lldb::watch_id_t watch_id, uint32_t ignore_count);

Status SerializeBreakpointsToFile(const FileSpec &file,
const BreakpointIDList &bp_ids,
bool append);

Status CreateBreakpointsFromFile(const FileSpec &file,
BreakpointIDList &new_bps);

Status CreateBreakpointsFromFile(const FileSpec &file,
std::vector<std::string> &names,
BreakpointIDList &new_bps);










lldb::addr_t GetCallableLoadAddress(
lldb::addr_t load_addr,
AddressClass addr_class = AddressClass::eInvalid) const;











lldb::addr_t
GetOpcodeLoadAddress(lldb::addr_t load_addr,
AddressClass addr_class = AddressClass::eInvalid) const;






lldb::addr_t GetBreakableLoadAddress(lldb::addr_t addr);

void ModulesDidLoad(ModuleList &module_list);

void ModulesDidUnload(ModuleList &module_list, bool delete_locations);

void SymbolsDidLoad(ModuleList &module_list);

void ClearModules(bool delete_locations);










void DidExec();
















lldb::ModuleSP GetExecutableModule();

Module *GetExecutableModulePointer();


























void SetExecutableModule(
lldb::ModuleSP &module_sp,
LoadDependentFiles load_dependent_files = eLoadDependentsDefault);

bool LoadScriptingResources(std::list<Status> &errors,
Stream *feedback_stream = nullptr,
bool continue_on_error = true) {
return m_images.LoadScriptingResourcesInTarget(
this, errors, feedback_stream, continue_on_error);
}


















const ModuleList &GetImages() const { return m_images; }

ModuleList &GetImages() { return m_images; }
















bool ModuleIsExcludedForUnconstrainedSearches(const FileSpec &module_spec);



















bool
ModuleIsExcludedForUnconstrainedSearches(const lldb::ModuleSP &module_sp);

const ArchSpec &GetArchitecture() const { return m_arch.GetSpec(); }


























bool SetArchitecture(const ArchSpec &arch_spec, bool set_platform = false);

bool MergeArchitecture(const ArchSpec &arch_spec);

Architecture *GetArchitecturePlugin() const { return m_arch.GetPlugin(); }

Debugger &GetDebugger() { return m_debugger; }

size_t ReadMemoryFromFileCache(const Address &addr, void *dst, size_t dst_len,
Status &error);












size_t ReadMemory(const Address &addr, void *dst, size_t dst_len,
Status &error, bool force_live_memory = false,
lldb::addr_t *load_addr_ptr = nullptr);

size_t ReadCStringFromMemory(const Address &addr, std::string &out_str,
Status &error);

size_t ReadCStringFromMemory(const Address &addr, char *dst,
size_t dst_max_len, Status &result_error);

size_t ReadScalarIntegerFromMemory(const Address &addr, uint32_t byte_size,
bool is_signed, Scalar &scalar,
Status &error,
bool force_live_memory = false);

uint64_t ReadUnsignedIntegerFromMemory(const Address &addr,
size_t integer_byte_size,
uint64_t fail_value, Status &error,
bool force_live_memory = false);

bool ReadPointerFromMemory(const Address &addr, Status &error,
Address &pointer_addr,
bool force_live_memory = false);

SectionLoadList &GetSectionLoadList() {
return m_section_load_history.GetCurrentSectionLoadList();
}

static Target *GetTargetFromContexts(const ExecutionContext *exe_ctx_ptr,
const SymbolContext *sc_ptr);


lldb::TargetSP CalculateTarget() override;

lldb::ProcessSP CalculateProcess() override;

lldb::ThreadSP CalculateThread() override;

lldb::StackFrameSP CalculateStackFrame() override;

void CalculateExecutionContext(ExecutionContext &exe_ctx) override;

PathMappingList &GetImageSearchPathList();

llvm::Expected<TypeSystem &>
GetScratchTypeSystemForLanguage(lldb::LanguageType language,
bool create_on_demand = true);

std::vector<TypeSystem *> GetScratchTypeSystems(bool create_on_demand = true);

PersistentExpressionState *
GetPersistentExpressionStateForLanguage(lldb::LanguageType language);





UserExpression *
GetUserExpressionForLanguage(llvm::StringRef expr, llvm::StringRef prefix,
lldb::LanguageType language,
Expression::ResultType desired_type,
const EvaluateExpressionOptions &options,
ValueObject *ctx_obj, Status &error);









FunctionCaller *GetFunctionCallerForLanguage(lldb::LanguageType language,
const CompilerType &return_type,
const Address &function_address,
const ValueList &arg_value_list,
const char *name, Status &error);


llvm::Expected<std::unique_ptr<UtilityFunction>>
CreateUtilityFunction(std::string expression, std::string name,
lldb::LanguageType language, ExecutionContext &exe_ctx);



Status Install(ProcessLaunchInfo *launch_info);

bool ResolveFileAddress(lldb::addr_t load_addr, Address &so_addr);

bool ResolveLoadAddress(lldb::addr_t load_addr, Address &so_addr,
uint32_t stop_id = SectionLoadHistory::eStopIDNow);

bool SetSectionLoadAddress(const lldb::SectionSP &section,
lldb::addr_t load_addr,
bool warn_multiple = false);

size_t UnloadModuleSections(const lldb::ModuleSP &module_sp);

size_t UnloadModuleSections(const ModuleList &module_list);

bool SetSectionUnloaded(const lldb::SectionSP &section_sp);

bool SetSectionUnloaded(const lldb::SectionSP &section_sp,
lldb::addr_t load_addr);

void ClearAllLoadedSections();






void SetTrace(const lldb::TraceSP &trace_sp);






lldb::TraceSP GetTrace();







llvm::Expected<lldb::TraceSP> CreateTrace();



llvm::Expected<lldb::TraceSP> GetTraceOrCreate();






lldb::ExpressionResults EvaluateExpression(
llvm::StringRef expression, ExecutionContextScope *exe_scope,
lldb::ValueObjectSP &result_valobj_sp,
const EvaluateExpressionOptions &options = EvaluateExpressionOptions(),
std::string *fixed_expression = nullptr, ValueObject *ctx_obj = nullptr);

lldb::ExpressionVariableSP GetPersistentVariable(ConstString name);

lldb::addr_t GetPersistentSymbol(ConstString name);

















llvm::Expected<lldb_private::Address> GetEntryPointAddress();


class StopHook : public UserID {
public:
StopHook(const StopHook &rhs);
virtual ~StopHook() = default;

enum class StopHookKind : uint32_t { CommandBased = 0, ScriptBased };
enum class StopHookResult : uint32_t {
KeepStopped = 0,
RequestContinue,
AlreadyContinued
};

lldb::TargetSP &GetTarget() { return m_target_sp; }



void SetSpecifier(SymbolContextSpecifier *specifier);

SymbolContextSpecifier *GetSpecifier() { return m_specifier_sp.get(); }

bool ExecutionContextPasses(const ExecutionContext &exe_ctx);





virtual StopHookResult HandleStop(ExecutionContext &exe_ctx,
lldb::StreamSP output) = 0;



void SetThreadSpecifier(ThreadSpec *specifier);

ThreadSpec *GetThreadSpecifier() { return m_thread_spec_up.get(); }

bool IsActive() { return m_active; }

void SetIsActive(bool is_active) { m_active = is_active; }

void SetAutoContinue(bool auto_continue) {
m_auto_continue = auto_continue;
}

bool GetAutoContinue() const { return m_auto_continue; }

void GetDescription(Stream *s, lldb::DescriptionLevel level) const;
virtual void GetSubclassDescription(Stream *s,
lldb::DescriptionLevel level) const = 0;

protected:
lldb::TargetSP m_target_sp;
lldb::SymbolContextSpecifierSP m_specifier_sp;
std::unique_ptr<ThreadSpec> m_thread_spec_up;
bool m_active = true;
bool m_auto_continue = false;

StopHook(lldb::TargetSP target_sp, lldb::user_id_t uid);
};

class StopHookCommandLine : public StopHook {
public:
virtual ~StopHookCommandLine() = default;

StringList &GetCommands() { return m_commands; }
void SetActionFromString(const std::string &strings);
void SetActionFromStrings(const std::vector<std::string> &strings);

StopHookResult HandleStop(ExecutionContext &exc_ctx,
lldb::StreamSP output_sp) override;
void GetSubclassDescription(Stream *s,
lldb::DescriptionLevel level) const override;

private:
StringList m_commands;



StopHookCommandLine(lldb::TargetSP target_sp, lldb::user_id_t uid)
: StopHook(target_sp, uid) {}
friend class Target;
};

class StopHookScripted : public StopHook {
public:
virtual ~StopHookScripted() = default;
StopHookResult HandleStop(ExecutionContext &exc_ctx,
lldb::StreamSP output) override;

Status SetScriptCallback(std::string class_name,
StructuredData::ObjectSP extra_args_sp);

void GetSubclassDescription(Stream *s,
lldb::DescriptionLevel level) const override;

private:
std::string m_class_name;


StructuredDataImpl *m_extra_args;


StructuredData::GenericSP m_implementation_sp;




StopHookScripted(lldb::TargetSP target_sp, lldb::user_id_t uid)
: StopHook(target_sp, uid) {}
friend class Target;
};

typedef std::shared_ptr<StopHook> StopHookSP;



StopHookSP CreateStopHook(StopHook::StopHookKind kind);



void UndoCreateStopHook(lldb::user_id_t uid);



bool RunStopHooks();

size_t GetStopHookSize();

bool SetSuppresStopHooks(bool suppress) {
bool old_value = m_suppress_stop_hooks;
m_suppress_stop_hooks = suppress;
return old_value;
}

bool GetSuppressStopHooks() { return m_suppress_stop_hooks; }

bool RemoveStopHookByID(lldb::user_id_t uid);

void RemoveAllStopHooks();

StopHookSP GetStopHookByID(lldb::user_id_t uid);

bool SetStopHookActiveStateByID(lldb::user_id_t uid, bool active_state);

void SetAllStopHooksActiveState(bool active_state);

size_t GetNumStopHooks() const { return m_stop_hooks.size(); }

StopHookSP GetStopHookAtIndex(size_t index) {
if (index >= GetNumStopHooks())
return StopHookSP();
StopHookCollection::iterator pos = m_stop_hooks.begin();

while (index > 0) {
pos++;
index--;
}
return (*pos).second;
}

lldb::PlatformSP GetPlatform() { return m_platform_sp; }

void SetPlatform(const lldb::PlatformSP &platform_sp) {
m_platform_sp = platform_sp;
}

SourceManager &GetSourceManager();


lldb::SearchFilterSP
GetSearchFilterForModule(const FileSpec *containingModule);

lldb::SearchFilterSP
GetSearchFilterForModuleList(const FileSpecList *containingModuleList);

lldb::SearchFilterSP
GetSearchFilterForModuleAndCUList(const FileSpecList *containingModules,
const FileSpecList *containingSourceFiles);

lldb::REPLSP GetREPL(Status &err, lldb::LanguageType language,
const char *repl_options, bool can_create);

void SetREPL(lldb::LanguageType language, lldb::REPLSP repl_sp);

StackFrameRecognizerManager &GetFrameRecognizerManager() {
return *m_frame_recognizer_manager_up;
}

protected:


void NotifyModuleAdded(const ModuleList &module_list,
const lldb::ModuleSP &module_sp) override;

void NotifyModuleRemoved(const ModuleList &module_list,
const lldb::ModuleSP &module_sp) override;

void NotifyModuleUpdated(const ModuleList &module_list,
const lldb::ModuleSP &old_module_sp,
const lldb::ModuleSP &new_module_sp) override;

void NotifyWillClearList(const ModuleList &module_list) override;

void NotifyModulesRemoved(lldb_private::ModuleList &module_list) override;

class Arch {
public:
explicit Arch(const ArchSpec &spec);
const Arch &operator=(const ArchSpec &spec);

const ArchSpec &GetSpec() const { return m_spec; }
Architecture *GetPlugin() const { return m_plugin_up.get(); }

private:
ArchSpec m_spec;
std::unique_ptr<Architecture> m_plugin_up;
};

Debugger &m_debugger;
lldb::PlatformSP m_platform_sp;
std::recursive_mutex m_mutex;






std::recursive_mutex m_private_mutex;
Arch m_arch;
ModuleList m_images;

SectionLoadHistory m_section_load_history;
BreakpointList m_breakpoint_list;
BreakpointList m_internal_breakpoint_list;
using BreakpointNameList = std::map<ConstString, BreakpointName *>;
BreakpointNameList m_breakpoint_names;

lldb::BreakpointSP m_last_created_breakpoint;
WatchpointList m_watchpoint_list;
lldb::WatchpointSP m_last_created_watchpoint;



lldb::ProcessSP m_process_sp;
lldb::SearchFilterSP m_search_filter_sp;
PathMappingList m_image_search_paths;
TypeSystemMap m_scratch_type_system_map;

typedef std::map<lldb::LanguageType, lldb::REPLSP> REPLMap;
REPLMap m_repl_map;

lldb::SourceManagerUP m_source_manager_up;

typedef std::map<lldb::user_id_t, StopHookSP> StopHookCollection;
StopHookCollection m_stop_hooks;
lldb::user_id_t m_stop_hook_next_id;
uint32_t m_latest_stop_hook_id;

bool m_valid;
bool m_suppress_stop_hooks;
bool m_is_dummy_target;
unsigned m_next_persistent_variable_index = 0;


lldb::TraceSP m_trace_sp;

lldb::StackFrameRecognizerManagerUP m_frame_recognizer_manager_up;

static void ImageSearchPathsChanged(const PathMappingList &path_list,
void *baton);


private:
std::vector<uint32_t> m_stats_storage;
bool m_collecting_stats = false;

public:
void SetCollectingStats(bool v) { m_collecting_stats = v; }

bool GetCollectingStats() { return m_collecting_stats; }

void IncrementStats(lldb_private::StatisticKind key) {
if (!GetCollectingStats())
return;
lldbassert(key < lldb_private::StatisticKind::StatisticMax &&
"invalid statistics!");
m_stats_storage[key] += 1;
}

std::vector<uint32_t> GetStatistics() { return m_stats_storage; }

private:







Target(Debugger &debugger, const ArchSpec &target_arch,
const lldb::PlatformSP &platform_sp, bool is_dummy_target);


bool ProcessIsValid();


void PrimeFromDummyTarget(Target &target);

void AddBreakpoint(lldb::BreakpointSP breakpoint_sp, bool internal);

void FinalizeFileActions(ProcessLaunchInfo &info);

Target(const Target &) = delete;
const Target &operator=(const Target &) = delete;
};

}

#endif
