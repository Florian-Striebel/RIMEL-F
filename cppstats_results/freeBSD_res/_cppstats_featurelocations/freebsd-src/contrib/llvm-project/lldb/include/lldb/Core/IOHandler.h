







#if !defined(LLDB_CORE_IOHANDLER_H)
#define LLDB_CORE_IOHANDLER_H

#include "lldb/Core/ValueObjectList.h"
#include "lldb/Host/Config.h"
#include "lldb/Utility/CompletionRequest.h"
#include "lldb/Utility/ConstString.h"
#include "lldb/Utility/Flags.h"
#include "lldb/Utility/Predicate.h"
#include "lldb/Utility/Stream.h"
#include "lldb/Utility/StringList.h"
#include "lldb/lldb-defines.h"
#include "lldb/lldb-forward.h"
#include "llvm/ADT/StringRef.h"

#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include <cstdint>
#include <cstdio>

namespace lldb_private {
class Debugger;
namespace repro {
class DataRecorder;
}
}

namespace curses {
class Application;
typedef std::unique_ptr<Application> ApplicationAP;
}

namespace lldb_private {

class IOHandler {
public:
enum class Type {
CommandInterpreter,
CommandList,
Confirm,
Curses,
Expression,
REPL,
ProcessIO,
PythonInterpreter,
LuaInterpreter,
PythonCode,
Other
};

IOHandler(Debugger &debugger, IOHandler::Type type);

IOHandler(Debugger &debugger, IOHandler::Type type,
const lldb::FileSP &input_sp, const lldb::StreamFileSP &output_sp,
const lldb::StreamFileSP &error_sp, uint32_t flags,
repro::DataRecorder *data_recorder);

virtual ~IOHandler();



virtual void Run() = 0;





virtual void Cancel() = 0;




virtual bool Interrupt() = 0;

virtual void GotEOF() = 0;

virtual bool IsActive() { return m_active && !m_done; }

virtual void SetIsDone(bool b) { m_done = b; }

virtual bool GetIsDone() { return m_done; }

Type GetType() const { return m_type; }

virtual void Activate() { m_active = true; }

virtual void Deactivate() { m_active = false; }

virtual void TerminalSizeChanged() {}

virtual const char *GetPrompt() {

return nullptr;
}

virtual bool SetPrompt(llvm::StringRef prompt) {

return false;
}
bool SetPrompt(const char *) = delete;

virtual ConstString GetControlSequence(char ch) { return ConstString(); }

virtual const char *GetCommandPrefix() { return nullptr; }

virtual const char *GetHelpPrologue() { return nullptr; }

int GetInputFD();

int GetOutputFD();

int GetErrorFD();

FILE *GetInputFILE();

FILE *GetOutputFILE();

FILE *GetErrorFILE();

lldb::FileSP GetInputFileSP();

lldb::StreamFileSP GetOutputStreamFileSP();

lldb::StreamFileSP GetErrorStreamFileSP();

Debugger &GetDebugger() { return m_debugger; }

void *GetUserData() { return m_user_data; }

void SetUserData(void *user_data) { m_user_data = user_data; }

Flags &GetFlags() { return m_flags; }

const Flags &GetFlags() const { return m_flags; }






bool GetIsInteractive();







bool GetIsRealTerminal();

void SetPopped(bool b);

void WaitForPop();

virtual void PrintAsync(Stream *stream, const char *s, size_t len) {
stream->Write(s, len);
stream->Flush();
}

protected:
Debugger &m_debugger;
lldb::FileSP m_input_sp;
lldb::StreamFileSP m_output_sp;
lldb::StreamFileSP m_error_sp;
repro::DataRecorder *m_data_recorder;
Predicate<bool> m_popped;
Flags m_flags;
Type m_type;
void *m_user_data;
bool m_done;
bool m_active;

private:
IOHandler(const IOHandler &) = delete;
const IOHandler &operator=(const IOHandler &) = delete;
};







class IOHandlerDelegate {
public:
enum class Completion { None, LLDBCommand, Expression };

IOHandlerDelegate(Completion completion = Completion::None)
: m_completion(completion) {}

virtual ~IOHandlerDelegate() = default;

virtual void IOHandlerActivated(IOHandler &io_handler, bool interactive) {}

virtual void IOHandlerDeactivated(IOHandler &io_handler) {}

virtual llvm::Optional<std::string> IOHandlerSuggestion(IOHandler &io_handler,
llvm::StringRef line);

virtual void IOHandlerComplete(IOHandler &io_handler,
CompletionRequest &request);

virtual const char *IOHandlerGetFixIndentationCharacters() { return nullptr; }























virtual int IOHandlerFixIndentation(IOHandler &io_handler,
const StringList &lines,
int cursor_position) {
return 0;
}








virtual void IOHandlerInputComplete(IOHandler &io_handler,
std::string &data) = 0;

virtual void IOHandlerInputInterrupted(IOHandler &io_handler,
std::string &data) {}















virtual bool IOHandlerIsInputComplete(IOHandler &io_handler,
StringList &lines) {


return true;
}

virtual ConstString IOHandlerGetControlSequence(char ch) {
return ConstString();
}

virtual const char *IOHandlerGetCommandPrefix() { return nullptr; }

virtual const char *IOHandlerGetHelpPrologue() { return nullptr; }





virtual bool IOHandlerInterrupt(IOHandler &io_handler) { return false; }

protected:
Completion m_completion;
};





class IOHandlerDelegateMultiline : public IOHandlerDelegate {
public:
IOHandlerDelegateMultiline(const char *end_line,
Completion completion = Completion::None)
: IOHandlerDelegate(completion),
m_end_line((end_line && end_line[0]) ? end_line : "") {}

~IOHandlerDelegateMultiline() override = default;

ConstString IOHandlerGetControlSequence(char ch) override {
if (ch == 'd')
return ConstString(m_end_line + "\n");
return ConstString();
}

bool IOHandlerIsInputComplete(IOHandler &io_handler,
StringList &lines) override {

const size_t num_lines = lines.GetSize();
if (num_lines > 0 && lines[num_lines - 1] == m_end_line) {


lines.PopBack();
return true;
}
return false;
}

protected:
const std::string m_end_line;
};

class IOHandlerEditline : public IOHandler {
public:
IOHandlerEditline(Debugger &debugger, IOHandler::Type type,
const char *editline_name,
llvm::StringRef prompt, llvm::StringRef continuation_prompt,
bool multi_line, bool color_prompts,
uint32_t line_number_start,


IOHandlerDelegate &delegate,
repro::DataRecorder *data_recorder);

IOHandlerEditline(Debugger &debugger, IOHandler::Type type,
const lldb::FileSP &input_sp,
const lldb::StreamFileSP &output_sp,
const lldb::StreamFileSP &error_sp, uint32_t flags,
const char *editline_name,
llvm::StringRef prompt, llvm::StringRef continuation_prompt,
bool multi_line, bool color_prompts,
uint32_t line_number_start,


IOHandlerDelegate &delegate,
repro::DataRecorder *data_recorder);

IOHandlerEditline(Debugger &, IOHandler::Type, const char *, const char *,
const char *, bool, bool, uint32_t,
IOHandlerDelegate &) = delete;

IOHandlerEditline(Debugger &, IOHandler::Type, const lldb::FileSP &,
const lldb::StreamFileSP &, const lldb::StreamFileSP &,
uint32_t, const char *, const char *, const char *, bool,
bool, uint32_t, IOHandlerDelegate &) = delete;

~IOHandlerEditline() override;

void Run() override;

void Cancel() override;

bool Interrupt() override;

void GotEOF() override;

void Activate() override;

void Deactivate() override;

void TerminalSizeChanged() override;

ConstString GetControlSequence(char ch) override {
return m_delegate.IOHandlerGetControlSequence(ch);
}

const char *GetCommandPrefix() override {
return m_delegate.IOHandlerGetCommandPrefix();
}

const char *GetHelpPrologue() override {
return m_delegate.IOHandlerGetHelpPrologue();
}

const char *GetPrompt() override;

bool SetPrompt(llvm::StringRef prompt) override;
bool SetPrompt(const char *prompt) = delete;

const char *GetContinuationPrompt();

void SetContinuationPrompt(llvm::StringRef prompt);
void SetContinuationPrompt(const char *) = delete;

bool GetLine(std::string &line, bool &interrupted);

bool GetLines(StringList &lines, bool &interrupted);

void SetBaseLineNumber(uint32_t line);

bool GetInterruptExits() { return m_interrupt_exits; }

void SetInterruptExits(bool b) { m_interrupt_exits = b; }

const StringList *GetCurrentLines() const { return m_current_lines_ptr; }

uint32_t GetCurrentLineIndex() const;

void PrintAsync(Stream *stream, const char *s, size_t len) override;

private:
#if LLDB_ENABLE_LIBEDIT
bool IsInputCompleteCallback(Editline *editline, StringList &lines);

int FixIndentationCallback(Editline *editline, const StringList &lines,
int cursor_position);

llvm::Optional<std::string> SuggestionCallback(llvm::StringRef line);

void AutoCompleteCallback(CompletionRequest &request);
#endif

protected:
#if LLDB_ENABLE_LIBEDIT
std::unique_ptr<Editline> m_editline_up;
#endif
IOHandlerDelegate &m_delegate;
std::string m_prompt;
std::string m_continuation_prompt;
StringList *m_current_lines_ptr;
uint32_t m_base_line_number;
uint32_t m_curr_line_idx;
bool m_multi_line;
bool m_color_prompts;
bool m_interrupt_exits;
bool m_editing;

std::string m_line_buffer;
};



class IOHandlerConfirm : public IOHandlerDelegate, public IOHandlerEditline {
public:
IOHandlerConfirm(Debugger &debugger, llvm::StringRef prompt,
bool default_response);

~IOHandlerConfirm() override;

bool GetResponse() const { return m_user_response; }

void IOHandlerComplete(IOHandler &io_handler,
CompletionRequest &request) override;

void IOHandlerInputComplete(IOHandler &io_handler,
std::string &data) override;

protected:
const bool m_default_response;
bool m_user_response;
};

class IOHandlerStack {
public:
IOHandlerStack() = default;

size_t GetSize() const {
std::lock_guard<std::recursive_mutex> guard(m_mutex);
return m_stack.size();
}

void Push(const lldb::IOHandlerSP &sp) {
if (sp) {
std::lock_guard<std::recursive_mutex> guard(m_mutex);
sp->SetPopped(false);
m_stack.push_back(sp);

m_top = sp.get();
}
}

bool IsEmpty() const {
std::lock_guard<std::recursive_mutex> guard(m_mutex);
return m_stack.empty();
}

lldb::IOHandlerSP Top() {
lldb::IOHandlerSP sp;
{
std::lock_guard<std::recursive_mutex> guard(m_mutex);
if (!m_stack.empty())
sp = m_stack.back();
}
return sp;
}

void Pop() {
std::lock_guard<std::recursive_mutex> guard(m_mutex);
if (!m_stack.empty()) {
lldb::IOHandlerSP sp(m_stack.back());
m_stack.pop_back();
sp->SetPopped(true);
}


m_top = (m_stack.empty() ? nullptr : m_stack.back().get());
}

std::recursive_mutex &GetMutex() { return m_mutex; }

bool IsTop(const lldb::IOHandlerSP &io_handler_sp) const {
return m_top == io_handler_sp.get();
}

bool CheckTopIOHandlerTypes(IOHandler::Type top_type,
IOHandler::Type second_top_type) {
std::lock_guard<std::recursive_mutex> guard(m_mutex);
const size_t num_io_handlers = m_stack.size();
return (num_io_handlers >= 2 &&
m_stack[num_io_handlers - 1]->GetType() == top_type &&
m_stack[num_io_handlers - 2]->GetType() == second_top_type);
}

ConstString GetTopIOHandlerControlSequence(char ch) {
return ((m_top != nullptr) ? m_top->GetControlSequence(ch) : ConstString());
}

const char *GetTopIOHandlerCommandPrefix() {
return ((m_top != nullptr) ? m_top->GetCommandPrefix() : nullptr);
}

const char *GetTopIOHandlerHelpPrologue() {
return ((m_top != nullptr) ? m_top->GetHelpPrologue() : nullptr);
}

void PrintAsync(Stream *stream, const char *s, size_t len);

protected:
typedef std::vector<lldb::IOHandlerSP> collection;
collection m_stack;
mutable std::recursive_mutex m_mutex;
IOHandler *m_top = nullptr;

private:
IOHandlerStack(const IOHandlerStack &) = delete;
const IOHandlerStack &operator=(const IOHandlerStack &) = delete;
};

}

#endif
