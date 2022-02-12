







#if !defined(LLDB_INTERPRETER_COMMANDOBJECT_H)
#define LLDB_INTERPRETER_COMMANDOBJECT_H

#include <map>
#include <string>
#include <vector>

#include "lldb/Utility/Flags.h"

#include "lldb/Interpreter/CommandCompletions.h"
#include "lldb/Interpreter/Options.h"
#include "lldb/Target/ExecutionContext.h"
#include "lldb/Utility/Args.h"
#include "lldb/Utility/CompletionRequest.h"
#include "lldb/Utility/StringList.h"
#include "lldb/lldb-private.h"

namespace lldb_private {







template <typename ValueType>
int AddNamesMatchingPartialString(
const std::map<std::string, ValueType> &in_map, llvm::StringRef cmd_str,
StringList &matches, StringList *descriptions = nullptr) {
int number_added = 0;

const bool add_all = cmd_str.empty();

for (auto iter = in_map.begin(), end = in_map.end(); iter != end; iter++) {
if (add_all || (iter->first.find(std::string(cmd_str), 0) == 0)) {
++number_added;
matches.AppendString(iter->first.c_str());
if (descriptions)
descriptions->AppendString(iter->second->GetHelp());
}
}

return number_added;
}

template <typename ValueType>
size_t FindLongestCommandWord(std::map<std::string, ValueType> &dict) {
auto end = dict.end();
size_t max_len = 0;

for (auto pos = dict.begin(); pos != end; ++pos) {
size_t len = pos->first.size();
if (max_len < len)
max_len = len;
}
return max_len;
}

class CommandObject {
public:
typedef llvm::StringRef(ArgumentHelpCallbackFunction)();

struct ArgumentHelpCallback {
ArgumentHelpCallbackFunction *help_callback;
bool self_formatting;

llvm::StringRef operator()() const { return (*help_callback)(); }

explicit operator bool() const { return (help_callback != nullptr); }
};

struct ArgumentTableEntry
{
lldb::CommandArgumentType arg_type;
const char *arg_name;
CommandCompletions::CommonCompletionTypes completion_type;
ArgumentHelpCallback help_function;
const char *help_text;
};

struct CommandArgumentData
{
lldb::CommandArgumentType arg_type;
ArgumentRepetitionType arg_repetition;


uint32_t arg_opt_set_association;

CommandArgumentData(lldb::CommandArgumentType type = lldb::eArgTypeNone,
ArgumentRepetitionType repetition = eArgRepeatPlain,
uint32_t opt_set = LLDB_OPT_SET_ALL)
: arg_type(type), arg_repetition(repetition),
arg_opt_set_association(opt_set) {}
};

typedef std::vector<CommandArgumentData>
CommandArgumentEntry;

static ArgumentTableEntry g_arguments_data
[lldb::eArgTypeLastArg];

typedef std::map<std::string, lldb::CommandObjectSP> CommandMap;

CommandObject(CommandInterpreter &interpreter, llvm::StringRef name,
llvm::StringRef help = "", llvm::StringRef syntax = "",
uint32_t flags = 0);

virtual ~CommandObject() = default;

static const char *
GetArgumentTypeAsCString(const lldb::CommandArgumentType arg_type);

static const char *
GetArgumentDescriptionAsCString(const lldb::CommandArgumentType arg_type);

CommandInterpreter &GetCommandInterpreter() { return m_interpreter; }
Debugger &GetDebugger();

virtual llvm::StringRef GetHelp();

virtual llvm::StringRef GetHelpLong();

virtual llvm::StringRef GetSyntax();

llvm::StringRef GetCommandName() const;

virtual void SetHelp(llvm::StringRef str);

virtual void SetHelpLong(llvm::StringRef str);

void SetSyntax(llvm::StringRef str);




virtual bool IsRemovable() const { return false; }

virtual bool IsMultiwordObject() { return false; }

virtual CommandObjectMultiword *GetAsMultiwordCommand() { return nullptr; }

virtual bool IsAlias() { return false; }




virtual bool IsDashDashCommand() { return false; }

virtual lldb::CommandObjectSP GetSubcommandSP(llvm::StringRef sub_cmd,
StringList *matches = nullptr) {
return lldb::CommandObjectSP();
}

virtual CommandObject *GetSubcommandObject(llvm::StringRef sub_cmd,
StringList *matches = nullptr) {
return nullptr;
}

virtual void AproposAllSubCommands(llvm::StringRef prefix,
llvm::StringRef search_word,
StringList &commands_found,
StringList &commands_help) {}

void FormatLongHelpText(Stream &output_strm, llvm::StringRef long_help);

void GenerateHelpText(CommandReturnObject &result);

virtual void GenerateHelpText(Stream &result);




virtual bool LoadSubCommand(llvm::StringRef cmd_name,
const lldb::CommandObjectSP &command_obj) {
return false;
}

virtual bool WantsRawCommandString() = 0;




virtual bool WantsCompletion() { return !WantsRawCommandString(); }

virtual Options *GetOptions();

static const ArgumentTableEntry *GetArgumentTable();

static lldb::CommandArgumentType LookupArgumentName(llvm::StringRef arg_name);

static const ArgumentTableEntry *
FindArgumentDataByType(lldb::CommandArgumentType arg_type);

int GetNumArgumentEntries();

CommandArgumentEntry *GetArgumentEntryAtIndex(int idx);

static void GetArgumentHelp(Stream &str, lldb::CommandArgumentType arg_type,
CommandInterpreter &interpreter);

static const char *GetArgumentName(lldb::CommandArgumentType arg_type);






void GetFormattedCommandArguments(Stream &str,
uint32_t opt_set_mask = LLDB_OPT_SET_ALL);

bool IsPairType(ArgumentRepetitionType arg_repeat_type);

bool ParseOptions(Args &args, CommandReturnObject &result);

void SetCommandName(llvm::StringRef name);








virtual void HandleCompletion(CompletionRequest &request);








virtual void
HandleArgumentCompletion(CompletionRequest &request,
OptionElementVector &opt_element_vector) {}

bool HelpTextContainsWord(llvm::StringRef search_word,
bool search_short_help = true,
bool search_long_help = true,
bool search_syntax = true,
bool search_options = true);





Flags &GetFlags() { return m_flags; }





const Flags &GetFlags() const { return m_flags; }












virtual const char *GetRepeatCommand(Args &current_command_args,
uint32_t index) {
return nullptr;
}

bool HasOverrideCallback() const {
return m_command_override_callback ||
m_deprecated_command_override_callback;
}

void SetOverrideCallback(lldb::CommandOverrideCallback callback,
void *baton) {
m_deprecated_command_override_callback = callback;
m_command_override_baton = baton;
}

void SetOverrideCallback(lldb::CommandOverrideCallbackWithResult callback,
void *baton) {
m_command_override_callback = callback;
m_command_override_baton = baton;
}

bool InvokeOverrideCallback(const char **argv, CommandReturnObject &result) {
if (m_command_override_callback)
return m_command_override_callback(m_command_override_baton, argv,
result);
else if (m_deprecated_command_override_callback)
return m_deprecated_command_override_callback(m_command_override_baton,
argv);
else
return false;
}

virtual bool Execute(const char *args_string,
CommandReturnObject &result) = 0;

protected:
bool ParseOptionsAndNotify(Args &args, CommandReturnObject &result,
OptionGroupOptions &group_options,
ExecutionContext &exe_ctx);

virtual const char *GetInvalidTargetDescription() {
return "invalid target, create a target using the 'target create' command";
}

virtual const char *GetInvalidProcessDescription() {
return "invalid process";
}

virtual const char *GetInvalidThreadDescription() { return "invalid thread"; }

virtual const char *GetInvalidFrameDescription() { return "invalid frame"; }

virtual const char *GetInvalidRegContextDescription() {
return "invalid frame, no registers";
}




Target &GetSelectedOrDummyTarget(bool prefer_dummy = false);
Target &GetSelectedTarget();
Target &GetDummyTarget();






Thread *GetDefaultThread();










bool CheckRequirements(CommandReturnObject &result);

void Cleanup();

CommandInterpreter &m_interpreter;
ExecutionContext m_exe_ctx;
std::unique_lock<std::recursive_mutex> m_api_locker;
std::string m_cmd_name;
std::string m_cmd_help_short;
std::string m_cmd_help_long;
std::string m_cmd_syntax;
Flags m_flags;
std::vector<CommandArgumentEntry> m_arguments;
lldb::CommandOverrideCallback m_deprecated_command_override_callback;
lldb::CommandOverrideCallbackWithResult m_command_override_callback;
void *m_command_override_baton;



static void AddIDsArgumentData(CommandArgumentEntry &arg,
lldb::CommandArgumentType ID,
lldb::CommandArgumentType IDRange);
};

class CommandObjectParsed : public CommandObject {
public:
CommandObjectParsed(CommandInterpreter &interpreter, const char *name,
const char *help = nullptr, const char *syntax = nullptr,
uint32_t flags = 0)
: CommandObject(interpreter, name, help, syntax, flags) {}

~CommandObjectParsed() override = default;

bool Execute(const char *args_string, CommandReturnObject &result) override;

protected:
virtual bool DoExecute(Args &command, CommandReturnObject &result) = 0;

bool WantsRawCommandString() override { return false; }
};

class CommandObjectRaw : public CommandObject {
public:
CommandObjectRaw(CommandInterpreter &interpreter, llvm::StringRef name,
llvm::StringRef help = "", llvm::StringRef syntax = "",
uint32_t flags = 0)
: CommandObject(interpreter, name, help, syntax, flags) {}

~CommandObjectRaw() override = default;

bool Execute(const char *args_string, CommandReturnObject &result) override;

protected:
virtual bool DoExecute(llvm::StringRef command,
CommandReturnObject &result) = 0;

bool WantsRawCommandString() override { return true; }
};

}

#endif
