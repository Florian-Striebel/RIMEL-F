







#if !defined(LLDB_API_SBCOMMANDINTERPRETERRUNOPTIONS_H)
#define LLDB_API_SBCOMMANDINTERPRETERRUNOPTIONS_H

#include <memory>

#include "lldb/API/SBDefines.h"

namespace lldb_private {
class CommandInterpreterRunOptions;
class CommandInterpreterRunResult;
}

namespace lldb {

class LLDB_API SBCommandInterpreterRunOptions {
friend class SBDebugger;
friend class SBCommandInterpreter;

public:
SBCommandInterpreterRunOptions();
SBCommandInterpreterRunOptions(const SBCommandInterpreterRunOptions &rhs);
~SBCommandInterpreterRunOptions();

SBCommandInterpreterRunOptions &
operator=(const SBCommandInterpreterRunOptions &rhs);

bool GetStopOnContinue() const;

void SetStopOnContinue(bool);

bool GetStopOnError() const;

void SetStopOnError(bool);

bool GetStopOnCrash() const;

void SetStopOnCrash(bool);

bool GetEchoCommands() const;

void SetEchoCommands(bool);

bool GetEchoCommentCommands() const;

void SetEchoCommentCommands(bool echo);

bool GetPrintResults() const;

void SetPrintResults(bool);

bool GetPrintErrors() const;

void SetPrintErrors(bool);

bool GetAddToHistory() const;

void SetAddToHistory(bool);

bool GetAutoHandleEvents() const;

void SetAutoHandleEvents(bool);

bool GetSpawnThread() const;

void SetSpawnThread(bool);

private:
lldb_private::CommandInterpreterRunOptions *get() const;

lldb_private::CommandInterpreterRunOptions &ref() const;


mutable std::unique_ptr<lldb_private::CommandInterpreterRunOptions>
m_opaque_up;
};

class LLDB_API SBCommandInterpreterRunResult {
friend class SBDebugger;
friend class SBCommandInterpreter;

public:
SBCommandInterpreterRunResult();
SBCommandInterpreterRunResult(const SBCommandInterpreterRunResult &rhs);
~SBCommandInterpreterRunResult();

SBCommandInterpreterRunResult &
operator=(const SBCommandInterpreterRunResult &rhs);

int GetNumberOfErrors() const;
lldb::CommandInterpreterResult GetResult() const;

private:
SBCommandInterpreterRunResult(
const lldb_private::CommandInterpreterRunResult &rhs);


std::unique_ptr<lldb_private::CommandInterpreterRunResult> m_opaque_up;
};

}

#endif
