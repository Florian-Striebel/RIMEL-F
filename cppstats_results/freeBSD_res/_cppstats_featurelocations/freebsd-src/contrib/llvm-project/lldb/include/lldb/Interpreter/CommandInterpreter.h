







#if !defined(LLDB_INTERPRETER_COMMANDINTERPRETER_H)
#define LLDB_INTERPRETER_COMMANDINTERPRETER_H

#include "lldb/Core/Debugger.h"
#include "lldb/Core/IOHandler.h"
#include "lldb/Interpreter/CommandAlias.h"
#include "lldb/Interpreter/CommandHistory.h"
#include "lldb/Interpreter/CommandObject.h"
#include "lldb/Interpreter/ScriptInterpreter.h"
#include "lldb/Utility/Args.h"
#include "lldb/Utility/Broadcaster.h"
#include "lldb/Utility/CompletionRequest.h"
#include "lldb/Utility/Event.h"
#include "lldb/Utility/Log.h"
#include "lldb/Utility/StreamString.h"
#include "lldb/Utility/StringList.h"
#include "lldb/lldb-forward.h"
#include "lldb/lldb-private.h"

#include <mutex>
#include <stack>

namespace lldb_private {
class CommandInterpreter;

class CommandInterpreterRunResult {
public:
CommandInterpreterRunResult() = default;

uint32_t GetNumErrors() const { return m_num_errors; }

lldb::CommandInterpreterResult GetResult() const { return m_result; }

bool IsResult(lldb::CommandInterpreterResult result) {
return m_result == result;
}

protected:
friend CommandInterpreter;

void IncrementNumberOfErrors() { m_num_errors++; }

void SetResult(lldb::CommandInterpreterResult result) { m_result = result; }

private:
int m_num_errors = 0;
lldb::CommandInterpreterResult m_result =
lldb::eCommandInterpreterResultSuccess;
};

class CommandInterpreterRunOptions {
public:
































CommandInterpreterRunOptions(LazyBool stop_on_continue,
LazyBool stop_on_error, LazyBool stop_on_crash,
LazyBool echo_commands, LazyBool echo_comments,
LazyBool print_results, LazyBool print_errors,
LazyBool add_to_history)
: m_stop_on_continue(stop_on_continue), m_stop_on_error(stop_on_error),
m_stop_on_crash(stop_on_crash), m_echo_commands(echo_commands),
m_echo_comment_commands(echo_comments), m_print_results(print_results),
m_print_errors(print_errors), m_add_to_history(add_to_history) {}

CommandInterpreterRunOptions() = default;

void SetSilent(bool silent) {
LazyBool value = silent ? eLazyBoolNo : eLazyBoolYes;

m_print_results = value;
m_print_errors = value;
m_echo_commands = value;
m_echo_comment_commands = value;
m_add_to_history = value;
}





bool GetStopOnContinue() const { return DefaultToNo(m_stop_on_continue); }

void SetStopOnContinue(bool stop_on_continue) {
m_stop_on_continue = stop_on_continue ? eLazyBoolYes : eLazyBoolNo;
}

bool GetStopOnError() const { return DefaultToNo(m_stop_on_error); }

void SetStopOnError(bool stop_on_error) {
m_stop_on_error = stop_on_error ? eLazyBoolYes : eLazyBoolNo;
}

bool GetStopOnCrash() const { return DefaultToNo(m_stop_on_crash); }

void SetStopOnCrash(bool stop_on_crash) {
m_stop_on_crash = stop_on_crash ? eLazyBoolYes : eLazyBoolNo;
}

bool GetEchoCommands() const { return DefaultToYes(m_echo_commands); }

void SetEchoCommands(bool echo_commands) {
m_echo_commands = echo_commands ? eLazyBoolYes : eLazyBoolNo;
}

bool GetEchoCommentCommands() const {
return DefaultToYes(m_echo_comment_commands);
}

void SetEchoCommentCommands(bool echo_comments) {
m_echo_comment_commands = echo_comments ? eLazyBoolYes : eLazyBoolNo;
}

bool GetPrintResults() const { return DefaultToYes(m_print_results); }

void SetPrintResults(bool print_results) {
m_print_results = print_results ? eLazyBoolYes : eLazyBoolNo;
}

bool GetPrintErrors() const { return DefaultToYes(m_print_errors); }

void SetPrintErrors(bool print_errors) {
m_print_errors = print_errors ? eLazyBoolYes : eLazyBoolNo;
}

bool GetAddToHistory() const { return DefaultToYes(m_add_to_history); }

void SetAddToHistory(bool add_to_history) {
m_add_to_history = add_to_history ? eLazyBoolYes : eLazyBoolNo;
}

bool GetAutoHandleEvents() const {
return DefaultToYes(m_auto_handle_events);
}

void SetAutoHandleEvents(bool auto_handle_events) {
m_auto_handle_events = auto_handle_events ? eLazyBoolYes : eLazyBoolNo;
}

bool GetSpawnThread() const { return DefaultToNo(m_spawn_thread); }

void SetSpawnThread(bool spawn_thread) {
m_spawn_thread = spawn_thread ? eLazyBoolYes : eLazyBoolNo;
}

LazyBool m_stop_on_continue = eLazyBoolCalculate;
LazyBool m_stop_on_error = eLazyBoolCalculate;
LazyBool m_stop_on_crash = eLazyBoolCalculate;
LazyBool m_echo_commands = eLazyBoolCalculate;
LazyBool m_echo_comment_commands = eLazyBoolCalculate;
LazyBool m_print_results = eLazyBoolCalculate;
LazyBool m_print_errors = eLazyBoolCalculate;
LazyBool m_add_to_history = eLazyBoolCalculate;
LazyBool m_auto_handle_events;
LazyBool m_spawn_thread;

private:
static bool DefaultToYes(LazyBool flag) {
switch (flag) {
case eLazyBoolNo:
return false;
default:
return true;
}
}

static bool DefaultToNo(LazyBool flag) {
switch (flag) {
case eLazyBoolYes:
return true;
default:
return false;
}
}
};

class CommandInterpreter : public Broadcaster,
public Properties,
public IOHandlerDelegate {
public:
enum {
eBroadcastBitThreadShouldExit = (1 << 0),
eBroadcastBitResetPrompt = (1 << 1),
eBroadcastBitQuitCommandReceived = (1 << 2),
eBroadcastBitAsynchronousOutputData = (1 << 3),
eBroadcastBitAsynchronousErrorData = (1 << 4)
};

enum ChildrenTruncatedWarningStatus

{ eNoTruncation = 0,
eUnwarnedTruncation = 1,
eWarnedTruncation = 2
};

enum CommandTypes {
eCommandTypesBuiltin = 0x0001,
eCommandTypesUserDef = 0x0002,
eCommandTypesAliases = 0x0004,
eCommandTypesHidden = 0x0008,
eCommandTypesAllThem = 0xFFFF
};

CommandInterpreter(Debugger &debugger, bool synchronous_execution);

~CommandInterpreter() override = default;



static ConstString &GetStaticBroadcasterClass();

ConstString &GetBroadcasterClass() const override {
return GetStaticBroadcasterClass();
}

void SourceInitFileCwd(CommandReturnObject &result);
void SourceInitFileHome(CommandReturnObject &result, bool is_repl = false);

bool AddCommand(llvm::StringRef name, const lldb::CommandObjectSP &cmd_sp,
bool can_replace);

bool AddUserCommand(llvm::StringRef name, const lldb::CommandObjectSP &cmd_sp,
bool can_replace);

lldb::CommandObjectSP GetCommandSPExact(llvm::StringRef cmd,
bool include_aliases = false) const;

CommandObject *GetCommandObject(llvm::StringRef cmd,
StringList *matches = nullptr,
StringList *descriptions = nullptr) const;

bool CommandExists(llvm::StringRef cmd) const;

bool AliasExists(llvm::StringRef cmd) const;

bool UserCommandExists(llvm::StringRef cmd) const;

CommandAlias *AddAlias(llvm::StringRef alias_name,
lldb::CommandObjectSP &command_obj_sp,
llvm::StringRef args_string = llvm::StringRef());


bool RemoveCommand(llvm::StringRef cmd);

bool RemoveAlias(llvm::StringRef alias_name);

bool GetAliasFullName(llvm::StringRef cmd, std::string &full_name) const;

bool RemoveUser(llvm::StringRef alias_name);

void RemoveAllUser() { m_user_dict.clear(); }

const CommandAlias *GetAlias(llvm::StringRef alias_name) const;

CommandObject *BuildAliasResult(llvm::StringRef alias_name,
std::string &raw_input_string,
std::string &alias_result,
CommandReturnObject &result);

bool HandleCommand(const char *command_line, LazyBool add_to_history,
const ExecutionContext &override_context,
CommandReturnObject &result);

bool HandleCommand(const char *command_line, LazyBool add_to_history,
CommandReturnObject &result);

bool WasInterrupted() const;
















void HandleCommands(const StringList &commands,
const ExecutionContext &context,
const CommandInterpreterRunOptions &options,
CommandReturnObject &result);

void HandleCommands(const StringList &commands,
const CommandInterpreterRunOptions &options,
CommandReturnObject &result);
















void HandleCommandsFromFile(FileSpec &file, const ExecutionContext &context,
const CommandInterpreterRunOptions &options,
CommandReturnObject &result);

void HandleCommandsFromFile(FileSpec &file,
const CommandInterpreterRunOptions &options,
CommandReturnObject &result);

CommandObject *GetCommandObjectForCommand(llvm::StringRef &command_line);



llvm::Optional<std::string> GetAutoSuggestionForCommand(llvm::StringRef line);


void HandleCompletion(CompletionRequest &request);



void HandleCompletionMatches(CompletionRequest &request);

int GetCommandNamesMatchingPartialString(const char *cmd_cstr,
bool include_aliases,
StringList &matches,
StringList &descriptions);

void GetHelp(CommandReturnObject &result,
uint32_t types = eCommandTypesAllThem);

void GetAliasHelp(const char *alias_name, StreamString &help_string);

void OutputFormattedHelpText(Stream &strm, llvm::StringRef prefix,
llvm::StringRef help_text);

void OutputFormattedHelpText(Stream &stream, llvm::StringRef command_word,
llvm::StringRef separator,
llvm::StringRef help_text, size_t max_word_len);







void OutputHelpText(Stream &stream, llvm::StringRef command_word,
llvm::StringRef separator, llvm::StringRef help_text,
uint32_t max_word_len);

Debugger &GetDebugger() { return m_debugger; }

ExecutionContext GetExecutionContext() const;

lldb::PlatformSP GetPlatform(bool prefer_target_platform);

const char *ProcessEmbeddedScriptCommands(const char *arg);

void UpdatePrompt(llvm::StringRef prompt);

bool Confirm(llvm::StringRef message, bool default_answer);

void LoadCommandDictionary();

void Initialize();

void Clear();

bool HasCommands() const;

bool HasAliases() const;

bool HasUserCommands() const;

bool HasAliasOptions() const;

void BuildAliasCommandArgs(CommandObject *alias_cmd_obj,
const char *alias_name, Args &cmd_args,
std::string &raw_input_string,
CommandReturnObject &result);

int GetOptionArgumentPosition(const char *in_string);

void SkipLLDBInitFiles(bool skip_lldbinit_files) {
m_skip_lldbinit_files = skip_lldbinit_files;
}

void SkipAppInitFiles(bool skip_app_init_files) {
m_skip_app_init_files = skip_app_init_files;
}

bool GetSynchronous();

void FindCommandsForApropos(llvm::StringRef word, StringList &commands_found,
StringList &commands_help,
bool search_builtin_commands,
bool search_user_commands,
bool search_alias_commands);

bool GetBatchCommandMode() { return m_batch_command_mode; }

bool SetBatchCommandMode(bool value) {
const bool old_value = m_batch_command_mode;
m_batch_command_mode = value;
return old_value;
}

void ChildrenTruncated() {
if (m_truncation_warning == eNoTruncation)
m_truncation_warning = eUnwarnedTruncation;
}

bool TruncationWarningNecessary() {
return (m_truncation_warning == eUnwarnedTruncation);
}

void TruncationWarningGiven() { m_truncation_warning = eWarnedTruncation; }

const char *TruncationWarningText() {
return "*** Some of your variables have more members than the debugger "
"will show by default. To show all of them, you can either use the "
"--show-all-children option to %s or raise the limit by changing "
"the target.max-children-count setting.\n";
}

CommandHistory &GetCommandHistory() { return m_command_history; }

bool IsActive();

CommandInterpreterRunResult
RunCommandInterpreter(CommandInterpreterRunOptions &options);

void GetLLDBCommandsFromIOHandler(const char *prompt,
IOHandlerDelegate &delegate,
void *baton = nullptr);

void GetPythonCommandsFromIOHandler(const char *prompt,
IOHandlerDelegate &delegate,
void *baton = nullptr);

const char *GetCommandPrefix();


bool GetExpandRegexAliases() const;

bool GetPromptOnQuit() const;
void SetPromptOnQuit(bool enable);

bool GetSaveSessionOnQuit() const;
void SetSaveSessionOnQuit(bool enable);

FileSpec GetSaveSessionDirectory() const;
void SetSaveSessionDirectory(llvm::StringRef path);

bool GetEchoCommands() const;
void SetEchoCommands(bool enable);

bool GetEchoCommentCommands() const;
void SetEchoCommentCommands(bool enable);

bool GetRepeatPreviousCommand() const;

const CommandObject::CommandMap &GetUserCommands() const {
return m_user_dict;
}

const CommandObject::CommandMap &GetCommands() const {
return m_command_dict;
}

const CommandObject::CommandMap &GetAliases() const { return m_alias_dict; }



void AllowExitCodeOnQuit(bool allow);







LLVM_NODISCARD bool SetQuitExitCode(int exit_code);





int GetQuitExitCode(bool &exited) const;

void ResolveCommand(const char *command_line, CommandReturnObject &result);

bool GetStopCmdSourceOnError() const;

lldb::IOHandlerSP
GetIOHandler(bool force_create = false,
CommandInterpreterRunOptions *options = nullptr);

bool GetSpaceReplPrompts() const;










bool SaveTranscript(CommandReturnObject &result,
llvm::Optional<std::string> output_file = llvm::None);

FileSpec GetCurrentSourceDir();

protected:
friend class Debugger;


void IOHandlerInputComplete(IOHandler &io_handler,
std::string &line) override;

ConstString IOHandlerGetControlSequence(char ch) override {
if (ch == 'd')
return ConstString("quit\n");
return ConstString();
}

bool IOHandlerInterrupt(IOHandler &io_handler) override;

void GetProcessOutput();

bool DidProcessStopAbnormally() const;

void SetSynchronous(bool value);

lldb::CommandObjectSP GetCommandSP(llvm::StringRef cmd,
bool include_aliases = true,
bool exact = true,
StringList *matches = nullptr,
StringList *descriptions = nullptr) const;

private:
void OverrideExecutionContext(const ExecutionContext &override_context);

void RestoreExecutionContext();

Status PreprocessCommand(std::string &command);

void SourceInitFile(FileSpec file, CommandReturnObject &result);




CommandObject *ResolveCommandImpl(std::string &command_line,
CommandReturnObject &result);

void FindCommandsForApropos(llvm::StringRef word, StringList &commands_found,
StringList &commands_help,
CommandObject::CommandMap &command_map);


void PrintCommandOutput(Stream &stream, llvm::StringRef str);

bool EchoCommandNonInteractive(llvm::StringRef line,
const Flags &io_handler_flags) const;


enum class CommandHandlingState {
eIdle,
eInProgress,
eInterrupted,
};

std::atomic<CommandHandlingState> m_command_state{
CommandHandlingState::eIdle};

int m_iohandler_nesting_level = 0;

void StartHandlingCommand();
void FinishHandlingCommand();
bool InterruptCommand();

Debugger &m_debugger;



std::stack<ExecutionContext> m_overriden_exe_contexts;
bool m_synchronous_execution;
bool m_skip_lldbinit_files;
bool m_skip_app_init_files;
CommandObject::CommandMap m_command_dict;


CommandObject::CommandMap
m_alias_dict;
CommandObject::CommandMap m_user_dict;
CommandHistory m_command_history;
std::string m_repeat_command;

lldb::IOHandlerSP m_command_io_handler_sp;
char m_comment_char;
bool m_batch_command_mode;
ChildrenTruncatedWarningStatus m_truncation_warning;





uint32_t m_command_source_depth;


std::vector<FileSpec> m_command_source_dirs;
std::vector<uint32_t> m_command_source_flags;
CommandInterpreterRunResult m_result;



llvm::Optional<int> m_quit_exit_code;

bool m_allow_exit_code = false;

StreamString m_transcript_stream;
};

}

#endif
