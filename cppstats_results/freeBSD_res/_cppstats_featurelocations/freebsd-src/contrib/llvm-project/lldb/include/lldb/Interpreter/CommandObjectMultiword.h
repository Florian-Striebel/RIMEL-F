







#if !defined(LLDB_INTERPRETER_COMMANDOBJECTMULTIWORD_H)
#define LLDB_INTERPRETER_COMMANDOBJECTMULTIWORD_H

#include "lldb/Interpreter/CommandObject.h"
#include "lldb/Utility/CompletionRequest.h"

namespace lldb_private {



class CommandObjectMultiword : public CommandObject {

friend class CommandInterpreter;
friend class CommandObjectSyntax;

public:
CommandObjectMultiword(CommandInterpreter &interpreter, const char *name,
const char *help = nullptr,
const char *syntax = nullptr, uint32_t flags = 0);

~CommandObjectMultiword() override;

bool IsMultiwordObject() override { return true; }

CommandObjectMultiword *GetAsMultiwordCommand() override { return this; }

bool LoadSubCommand(llvm::StringRef cmd_name,
const lldb::CommandObjectSP &command_obj) override;

void GenerateHelpText(Stream &output_stream) override;

lldb::CommandObjectSP GetSubcommandSP(llvm::StringRef sub_cmd,
StringList *matches = nullptr) override;

CommandObject *GetSubcommandObject(llvm::StringRef sub_cmd,
StringList *matches = nullptr) override;

void AproposAllSubCommands(llvm::StringRef prefix,
llvm::StringRef search_word,
StringList &commands_found,
StringList &commands_help) override;

bool WantsRawCommandString() override { return false; }

void HandleCompletion(CompletionRequest &request) override;

const char *GetRepeatCommand(Args &current_command_args,
uint32_t index) override;

bool Execute(const char *args_string, CommandReturnObject &result) override;

bool IsRemovable() const override { return m_can_be_removed; }

void SetRemovable(bool removable) { m_can_be_removed = removable; }

protected:
CommandObject::CommandMap &GetSubcommandDictionary() {
return m_subcommand_dict;
}

CommandObject::CommandMap m_subcommand_dict;
bool m_can_be_removed;
};

class CommandObjectProxy : public CommandObject {
public:
CommandObjectProxy(CommandInterpreter &interpreter, const char *name,
const char *help = nullptr, const char *syntax = nullptr,
uint32_t flags = 0);

~CommandObjectProxy() override;



virtual CommandObject *GetProxyCommandObject() = 0;

llvm::StringRef GetSyntax() override;

llvm::StringRef GetHelp() override;

llvm::StringRef GetHelpLong() override;

bool IsRemovable() const override;

bool IsMultiwordObject() override;

CommandObjectMultiword *GetAsMultiwordCommand() override;

void GenerateHelpText(Stream &result) override;

lldb::CommandObjectSP GetSubcommandSP(llvm::StringRef sub_cmd,
StringList *matches = nullptr) override;

CommandObject *GetSubcommandObject(llvm::StringRef sub_cmd,
StringList *matches = nullptr) override;

void AproposAllSubCommands(llvm::StringRef prefix,
llvm::StringRef search_word,
StringList &commands_found,
StringList &commands_help) override;

bool LoadSubCommand(llvm::StringRef cmd_name,
const lldb::CommandObjectSP &command_obj) override;

bool WantsRawCommandString() override;

bool WantsCompletion() override;

Options *GetOptions() override;

void HandleCompletion(CompletionRequest &request) override;

void
HandleArgumentCompletion(CompletionRequest &request,
OptionElementVector &opt_element_vector) override;

const char *GetRepeatCommand(Args &current_command_args,
uint32_t index) override;




virtual llvm::StringRef GetUnsupportedError();

bool Execute(const char *args_string, CommandReturnObject &result) override;

protected:

friend class CommandInterpreter;
friend class CommandObjectSyntax;
};

}

#endif
