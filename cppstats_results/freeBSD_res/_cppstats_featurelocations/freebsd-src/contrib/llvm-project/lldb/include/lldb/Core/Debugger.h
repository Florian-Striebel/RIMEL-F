







#if !defined(LLDB_CORE_DEBUGGER_H)
#define LLDB_CORE_DEBUGGER_H

#include <cstdint>

#include <memory>
#include <vector>

#include "lldb/Core/FormatEntity.h"
#include "lldb/Core/IOHandler.h"
#include "lldb/Core/SourceManager.h"
#include "lldb/Core/StreamFile.h"
#include "lldb/Core/UserSettingsController.h"
#include "lldb/Host/HostThread.h"
#include "lldb/Host/Terminal.h"
#include "lldb/Target/ExecutionContext.h"
#include "lldb/Target/Platform.h"
#include "lldb/Target/TargetList.h"
#include "lldb/Utility/Broadcaster.h"
#include "lldb/Utility/ConstString.h"
#include "lldb/Utility/FileSpec.h"
#include "lldb/Utility/Status.h"
#include "lldb/Utility/UserID.h"
#include "lldb/lldb-defines.h"
#include "lldb/lldb-enumerations.h"
#include "lldb/lldb-forward.h"
#include "lldb/lldb-private-enumerations.h"
#include "lldb/lldb-private-types.h"
#include "lldb/lldb-types.h"

#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/DynamicLibrary.h"
#include "llvm/Support/Threading.h"

#include <cassert>
#include <cstddef>
#include <cstdio>

namespace llvm {
class raw_ostream;
}

namespace lldb_private {
class Address;
class CommandInterpreter;
class Process;
class Stream;
class SymbolContext;
class Target;

namespace repro {
class DataRecorder;
}






class Debugger : public std::enable_shared_from_this<Debugger>,
public UserID,
public Properties {
friend class SourceManager;

public:

enum {
eBroadcastBitProgress = (1 << 0),
};

static ConstString GetStaticBroadcasterClass();


Broadcaster &GetBroadcaster() { return m_broadcaster; }
const Broadcaster &GetBroadcaster() const { return m_broadcaster; }

class ProgressEventData : public EventData {

public:
ProgressEventData(uint64_t progress_id, const std::string &message,
uint64_t completed, uint64_t total,
bool debugger_specific)
: m_message(message), m_id(progress_id), m_completed(completed),
m_total(total), m_debugger_specific(debugger_specific) {}

static ConstString GetFlavorString();

ConstString GetFlavor() const override;

void Dump(Stream *s) const override;

static const ProgressEventData *
GetEventDataFromEvent(const Event *event_ptr);
uint64_t GetID() const { return m_id; }
uint64_t GetCompleted() const { return m_completed; }
uint64_t GetTotal() const { return m_total; }
const std::string &GetMessage() const { return m_message; }
bool IsDebuggerSpecific() const { return m_debugger_specific; }

private:
std::string m_message;
const uint64_t m_id;
uint64_t m_completed;
const uint64_t m_total;
const bool m_debugger_specific;
ProgressEventData(const ProgressEventData &) = delete;
const ProgressEventData &operator=(const ProgressEventData &) = delete;
};

~Debugger() override;

static lldb::DebuggerSP
CreateInstance(lldb::LogOutputCallback log_callback = nullptr,
void *baton = nullptr);

static lldb::TargetSP FindTargetWithProcessID(lldb::pid_t pid);

static lldb::TargetSP FindTargetWithProcess(Process *process);

static void Initialize(LoadPluginCallbackType load_plugin_callback);

static void Terminate();

static void SettingsInitialize();

static void SettingsTerminate();

static void Destroy(lldb::DebuggerSP &debugger_sp);

static lldb::DebuggerSP FindDebuggerWithID(lldb::user_id_t id);

static lldb::DebuggerSP
FindDebuggerWithInstanceName(ConstString instance_name);

static size_t GetNumDebuggers();

static lldb::DebuggerSP GetDebuggerAtIndex(size_t index);

static bool FormatDisassemblerAddress(const FormatEntity::Entry *format,
const SymbolContext *sc,
const SymbolContext *prev_sc,
const ExecutionContext *exe_ctx,
const Address *addr, Stream &s);

void Clear();

bool GetAsyncExecution();

void SetAsyncExecution(bool async);

lldb::FileSP GetInputFileSP() { return m_input_file_sp; }

lldb::StreamFileSP GetOutputStreamSP() { return m_output_stream_sp; }

lldb::StreamFileSP GetErrorStreamSP() { return m_error_stream_sp; }

File &GetInputFile() { return *m_input_file_sp; }

File &GetOutputFile() { return m_output_stream_sp->GetFile(); }

File &GetErrorFile() { return m_error_stream_sp->GetFile(); }

StreamFile &GetOutputStream() { return *m_output_stream_sp; }

StreamFile &GetErrorStream() { return *m_error_stream_sp; }

repro::DataRecorder *GetInputRecorder();

void SetInputFile(lldb::FileSP file, repro::DataRecorder *recorder = nullptr);

void SetOutputFile(lldb::FileSP file);

void SetErrorFile(lldb::FileSP file);

void SaveInputTerminalState();

void RestoreInputTerminalState();

lldb::StreamSP GetAsyncOutputStream();

lldb::StreamSP GetAsyncErrorStream();

CommandInterpreter &GetCommandInterpreter() {
assert(m_command_interpreter_up.get());
return *m_command_interpreter_up;
}

ScriptInterpreter *
GetScriptInterpreter(bool can_create = true,
llvm::Optional<lldb::ScriptLanguage> language = {});

lldb::ListenerSP GetListener() { return m_listener_sp; }





SourceManager &GetSourceManager();

lldb::TargetSP GetSelectedTarget() {
return m_target_list.GetSelectedTarget();
}

ExecutionContext GetSelectedExecutionContext();








TargetList &GetTargetList() { return m_target_list; }

PlatformList &GetPlatformList() { return m_platform_list; }

void DispatchInputInterrupt();

void DispatchInputEndOfFile();



void AdoptTopIOHandlerFilesIfInvalid(lldb::FileSP &in,
lldb::StreamFileSP &out,
lldb::StreamFileSP &err);


void RunIOHandlerAsync(const lldb::IOHandlerSP &reader_sp,
bool cancel_top_handler = true);


void RunIOHandlerSync(const lldb::IOHandlerSP &reader_sp);


bool RemoveIOHandler(const lldb::IOHandlerSP &reader_sp);

bool IsTopIOHandler(const lldb::IOHandlerSP &reader_sp);

bool CheckTopIOHandlerTypes(IOHandler::Type top_type,
IOHandler::Type second_top_type);

void PrintAsync(const char *s, size_t len, bool is_stdout);

ConstString GetTopIOHandlerControlSequence(char ch);

const char *GetIOHandlerCommandPrefix();

const char *GetIOHandlerHelpPrologue();

void ClearIOHandlers();

bool GetCloseInputOnEOF() const;

void SetCloseInputOnEOF(bool b);

bool EnableLog(llvm::StringRef channel,
llvm::ArrayRef<const char *> categories,
llvm::StringRef log_file, uint32_t log_options,
llvm::raw_ostream &error_stream);

void SetLoggingCallback(lldb::LogOutputCallback log_callback, void *baton);


enum StopDisassemblyType {
eStopDisassemblyTypeNever = 0,
eStopDisassemblyTypeNoDebugInfo,
eStopDisassemblyTypeNoSource,
eStopDisassemblyTypeAlways
};

Status SetPropertyValue(const ExecutionContext *exe_ctx,
VarSetOperationType op, llvm::StringRef property_path,
llvm::StringRef value) override;

bool GetAutoConfirm() const;

const FormatEntity::Entry *GetDisassemblyFormat() const;

const FormatEntity::Entry *GetFrameFormat() const;

const FormatEntity::Entry *GetFrameFormatUnique() const;

uint32_t GetStopDisassemblyMaxSize() const;

const FormatEntity::Entry *GetThreadFormat() const;

const FormatEntity::Entry *GetThreadStopFormat() const;

lldb::ScriptLanguage GetScriptLanguage() const;

bool SetScriptLanguage(lldb::ScriptLanguage script_lang);

uint32_t GetTerminalWidth() const;

bool SetTerminalWidth(uint32_t term_width);

llvm::StringRef GetPrompt() const;

void SetPrompt(llvm::StringRef p);
void SetPrompt(const char *) = delete;

llvm::StringRef GetReproducerPath() const;

bool GetUseExternalEditor() const;

bool SetUseExternalEditor(bool use_external_editor_p);

bool GetUseColor() const;

bool SetUseColor(bool use_color);

bool GetUseAutosuggestion() const;

bool GetUseSourceCache() const;

bool SetUseSourceCache(bool use_source_cache);

bool GetHighlightSource() const;

lldb::StopShowColumn GetStopShowColumn() const;

llvm::StringRef GetStopShowColumnAnsiPrefix() const;

llvm::StringRef GetStopShowColumnAnsiSuffix() const;

uint32_t GetStopSourceLineCount(bool before) const;

StopDisassemblyType GetStopDisassemblyDisplay() const;

uint32_t GetDisassemblyLineCount() const;

llvm::StringRef GetStopShowLineMarkerAnsiPrefix() const;

llvm::StringRef GetStopShowLineMarkerAnsiSuffix() const;

bool GetAutoOneLineSummaries() const;

bool GetAutoIndent() const;

bool SetAutoIndent(bool b);

bool GetPrintDecls() const;

bool SetPrintDecls(bool b);

uint32_t GetTabSize() const;

bool SetTabSize(uint32_t tab_size);

bool GetEscapeNonPrintables() const;

bool GetNotifyVoid() const;

ConstString GetInstanceName() { return m_instance_name; }

bool LoadPlugin(const FileSpec &spec, Status &error);

void RunIOHandlers();

bool IsForwardingEvents();

void EnableForwardEvents(const lldb::ListenerSP &listener_sp);

void CancelForwardEvents(const lldb::ListenerSP &listener_sp);

bool IsHandlingEvents() const { return m_event_handler_thread.IsJoinable(); }

Status RunREPL(lldb::LanguageType language, const char *repl_options);




Target &GetSelectedOrDummyTarget(bool prefer_dummy = false);
Target &GetDummyTarget() { return *m_dummy_target_sp; }

lldb::BroadcasterManagerSP GetBroadcasterManager() {
return m_broadcaster_manager_sp;
}

protected:
friend class CommandInterpreter;
friend class REPL;
friend class Progress;






























static void ReportProgress(uint64_t progress_id, const std::string &message,
uint64_t completed, uint64_t total,
llvm::Optional<lldb::user_id_t> debugger_id);

bool StartEventHandlerThread();

void StopEventHandlerThread();

static lldb::thread_result_t EventHandlerThread(lldb::thread_arg_t arg);

void PushIOHandler(const lldb::IOHandlerSP &reader_sp,
bool cancel_top_handler = true);

bool PopIOHandler(const lldb::IOHandlerSP &reader_sp);

bool HasIOHandlerThread();

bool StartIOHandlerThread();

void StopIOHandlerThread();

void JoinIOHandlerThread();

static lldb::thread_result_t IOHandlerThread(lldb::thread_arg_t arg);

void DefaultEventHandler();

void HandleBreakpointEvent(const lldb::EventSP &event_sp);

void HandleProcessEvent(const lldb::EventSP &event_sp);

void HandleThreadEvent(const lldb::EventSP &event_sp);


std::mutex m_output_flush_mutex;
void FlushProcessOutput(Process &process, bool flush_stdout,
bool flush_stderr);

SourceManager::SourceFileCache &GetSourceFileCache() {
return m_source_file_cache;
}

void InstanceInitialize();


lldb::FileSP m_input_file_sp;
lldb::StreamFileSP m_output_stream_sp;
lldb::StreamFileSP m_error_stream_sp;


repro::DataRecorder *m_input_recorder;

lldb::BroadcasterManagerSP m_broadcaster_manager_sp;





TerminalState m_terminal_state;
TargetList m_target_list;

PlatformList m_platform_list;
lldb::ListenerSP m_listener_sp;
std::unique_ptr<SourceManager> m_source_manager_up;



SourceManager::SourceFileCache m_source_file_cache;




std::unique_ptr<CommandInterpreter> m_command_interpreter_up;

std::recursive_mutex m_script_interpreter_mutex;
std::array<lldb::ScriptInterpreterSP, lldb::eScriptLanguageUnknown>
m_script_interpreters;

IOHandlerStack m_io_handler_stack;
std::recursive_mutex m_io_handler_synchronous_mutex;

llvm::StringMap<std::weak_ptr<llvm::raw_ostream>> m_log_streams;
std::shared_ptr<llvm::raw_ostream> m_log_callback_stream_sp;
ConstString m_instance_name;
static LoadPluginCallbackType g_load_plugin_callback;
typedef std::vector<llvm::sys::DynamicLibrary> LoadedPluginsList;
LoadedPluginsList m_loaded_plugins;
HostThread m_event_handler_thread;
HostThread m_io_handler_thread;
Broadcaster m_sync_broadcaster;
Broadcaster m_broadcaster;
lldb::ListenerSP m_forward_listener_sp;
llvm::once_flag m_clear_once;
lldb::TargetSP m_dummy_target_sp;


enum {
eBroadcastBitEventThreadIsListening = (1 << 0),
};

private:


Debugger(lldb::LogOutputCallback m_log_callback, void *baton);

Debugger(const Debugger &) = delete;
const Debugger &operator=(const Debugger &) = delete;
};

}

#endif
