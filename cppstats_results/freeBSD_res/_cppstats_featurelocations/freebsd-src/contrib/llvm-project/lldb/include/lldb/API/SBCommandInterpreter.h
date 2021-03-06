







#if !defined(LLDB_API_SBCOMMANDINTERPRETER_H)
#define LLDB_API_SBCOMMANDINTERPRETER_H

#include <memory>

#include "lldb/API/SBDebugger.h"
#include "lldb/API/SBDefines.h"

namespace lldb {

class SBCommandInterpreter {
public:
enum {
eBroadcastBitThreadShouldExit = (1 << 0),
eBroadcastBitResetPrompt = (1 << 1),
eBroadcastBitQuitCommandReceived = (1 << 2),
eBroadcastBitAsynchronousOutputData = (1 << 3),
eBroadcastBitAsynchronousErrorData = (1 << 4)
};

SBCommandInterpreter(const lldb::SBCommandInterpreter &rhs);

~SBCommandInterpreter();

const lldb::SBCommandInterpreter &
operator=(const lldb::SBCommandInterpreter &rhs);

static const char *
GetArgumentTypeAsCString(const lldb::CommandArgumentType arg_type);

static const char *
GetArgumentDescriptionAsCString(const lldb::CommandArgumentType arg_type);

static bool EventIsCommandInterpreterEvent(const lldb::SBEvent &event);

explicit operator bool() const;

bool IsValid() const;

bool CommandExists(const char *cmd);

bool AliasExists(const char *cmd);

lldb::SBBroadcaster GetBroadcaster();

static const char *GetBroadcasterClass();

bool HasCommands();

bool HasAliases();

bool HasAliasOptions();

lldb::SBProcess GetProcess();

lldb::SBDebugger GetDebugger();

lldb::SBCommand AddMultiwordCommand(const char *name, const char *help);



















lldb::SBCommand AddCommand(const char *name,
lldb::SBCommandPluginInterface *impl,
const char *help);
























lldb::SBCommand AddCommand(const char *name,
lldb::SBCommandPluginInterface *impl,
const char *help, const char *syntax);



























lldb::SBCommand AddCommand(const char *name,
lldb::SBCommandPluginInterface *impl,
const char *help, const char *syntax,
const char *auto_repeat_command);

void SourceInitFileInHomeDirectory(lldb::SBCommandReturnObject &result);
void SourceInitFileInHomeDirectory(lldb::SBCommandReturnObject &result,
bool is_repl);

void
SourceInitFileInCurrentWorkingDirectory(lldb::SBCommandReturnObject &result);

lldb::ReturnStatus HandleCommand(const char *command_line,
lldb::SBCommandReturnObject &result,
bool add_to_history = false);

lldb::ReturnStatus HandleCommand(const char *command_line,
SBExecutionContext &exe_ctx,
SBCommandReturnObject &result,
bool add_to_history = false);

void HandleCommandsFromFile(lldb::SBFileSpec &file,
lldb::SBExecutionContext &override_context,
lldb::SBCommandInterpreterRunOptions &options,
lldb::SBCommandReturnObject result);


















int HandleCompletion(const char *current_line, const char *cursor,
const char *last_char, int match_start_point,
int max_return_elements, lldb::SBStringList &matches);

int HandleCompletion(const char *current_line, uint32_t cursor_pos,
int match_start_point, int max_return_elements,
lldb::SBStringList &matches);



int HandleCompletionWithDescriptions(
const char *current_line, const char *cursor, const char *last_char,
int match_start_point, int max_return_elements,
lldb::SBStringList &matches, lldb::SBStringList &descriptions);

int HandleCompletionWithDescriptions(const char *current_line,
uint32_t cursor_pos,
int match_start_point,
int max_return_elements,
lldb::SBStringList &matches,
lldb::SBStringList &descriptions);

bool WasInterrupted() const;




bool SetCommandOverrideCallback(const char *command_name,
lldb::CommandOverrideCallback callback,
void *baton);

SBCommandInterpreter(
lldb_private::CommandInterpreter *interpreter_ptr =
nullptr);






bool IsActive();
















const char *GetIOHandlerControlSequence(char ch);

bool GetPromptOnQuit();

void SetPromptOnQuit(bool b);



void AllowExitCodeOnQuit(bool allow);



bool HasCustomQuitExitCode();




int GetQuitStatus();



void ResolveCommand(const char *command_line, SBCommandReturnObject &result);

protected:
lldb_private::CommandInterpreter &ref();

lldb_private::CommandInterpreter *get();

void reset(lldb_private::CommandInterpreter *);

private:
friend class SBDebugger;

lldb_private::CommandInterpreter *m_opaque_ptr;
};

class SBCommandPluginInterface {
public:
virtual ~SBCommandPluginInterface() = default;

virtual bool DoExecute(lldb::SBDebugger , char ** ,
lldb::SBCommandReturnObject & ) {
return false;
}
};

class SBCommand {
public:
SBCommand();

explicit operator bool() const;

bool IsValid();

const char *GetName();

const char *GetHelp();

const char *GetHelpLong();

void SetHelp(const char *);

void SetHelpLong(const char *);

uint32_t GetFlags();

void SetFlags(uint32_t flags);

lldb::SBCommand AddMultiwordCommand(const char *name,
const char *help = nullptr);



















lldb::SBCommand AddCommand(const char *name,
lldb::SBCommandPluginInterface *impl,
const char *help = nullptr);
























lldb::SBCommand AddCommand(const char *name,
lldb::SBCommandPluginInterface *impl,
const char *help, const char *syntax);































lldb::SBCommand AddCommand(const char *name,
lldb::SBCommandPluginInterface *impl,
const char *help, const char *syntax,
const char *auto_repeat_command);

private:
friend class SBDebugger;
friend class SBCommandInterpreter;

SBCommand(lldb::CommandObjectSP cmd_sp);

lldb::CommandObjectSP m_opaque_sp;
};

}

#endif
