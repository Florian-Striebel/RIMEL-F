







#if !defined(LLDB_EXPRESSION_REPL_H)
#define LLDB_EXPRESSION_REPL_H

#include <string>

#include "lldb/Core/IOHandler.h"
#include "lldb/Interpreter/OptionGroupFormat.h"
#include "lldb/Interpreter/OptionGroupValueObjectDisplay.h"
#include "lldb/Target/Target.h"

namespace lldb_private {

class REPL : public IOHandlerDelegate {
public:

enum LLVMCastKind { eKindClang, eKindSwift, eKindGo, kNumKinds };

LLVMCastKind getKind() const { return m_kind; }

REPL(LLVMCastKind kind, Target &target);

~REPL() override;
























static lldb::REPLSP Create(Status &Status, lldb::LanguageType language,
Debugger *debugger, Target *target,
const char *repl_options);

void SetFormatOptions(const OptionGroupFormat &options) {
m_format_options = options;
}

void
SetValueObjectDisplayOptions(const OptionGroupValueObjectDisplay &options) {
m_varobj_options = options;
}

void SetEvaluateOptions(const EvaluateExpressionOptions &options) {
m_expr_options = options;
}

void SetCompilerOptions(const char *options) {
if (options)
m_compiler_options = options;
}

lldb::IOHandlerSP GetIOHandler();

Status RunLoop();


void IOHandlerActivated(IOHandler &io_handler, bool interactive) override;

bool IOHandlerInterrupt(IOHandler &io_handler) override;

void IOHandlerInputInterrupted(IOHandler &io_handler,
std::string &line) override;

const char *IOHandlerGetFixIndentationCharacters() override;

ConstString IOHandlerGetControlSequence(char ch) override;

const char *IOHandlerGetCommandPrefix() override;

const char *IOHandlerGetHelpPrologue() override;

bool IOHandlerIsInputComplete(IOHandler &io_handler,
StringList &lines) override;

int IOHandlerFixIndentation(IOHandler &io_handler, const StringList &lines,
int cursor_position) override;

void IOHandlerInputComplete(IOHandler &io_handler,
std::string &line) override;

void IOHandlerComplete(IOHandler &io_handler,
CompletionRequest &request) override;

protected:
static int CalculateActualIndentation(const StringList &lines);



virtual Status DoInitialization() = 0;

virtual ConstString GetSourceFileBasename() = 0;

virtual const char *GetAutoIndentCharacters() = 0;

virtual bool SourceIsComplete(const std::string &source) = 0;

virtual lldb::offset_t GetDesiredIndentation(
const StringList &lines, int cursor_position,
int tab_size) = 0;

virtual lldb::LanguageType GetLanguage() = 0;

virtual bool PrintOneVariable(Debugger &debugger,
lldb::StreamFileSP &output_sp,
lldb::ValueObjectSP &valobj_sp,
ExpressionVariable *var = nullptr) = 0;

virtual void CompleteCode(const std::string &current_code,
CompletionRequest &request) = 0;

OptionGroupFormat m_format_options = OptionGroupFormat(lldb::eFormatDefault);
OptionGroupValueObjectDisplay m_varobj_options;
EvaluateExpressionOptions m_expr_options;
std::string m_compiler_options;

bool m_enable_auto_indent = true;
std::string m_indent_str;
std::string m_current_indent_str;
uint32_t m_current_indent_level = 0;

std::string m_repl_source_path;
bool m_dedicated_repl_mode = false;

StringList m_code;

Target &m_target;
lldb::IOHandlerSP m_io_handler_sp;
LLVMCastKind m_kind;

private:
std::string GetSourcePath();
};

}

#endif
