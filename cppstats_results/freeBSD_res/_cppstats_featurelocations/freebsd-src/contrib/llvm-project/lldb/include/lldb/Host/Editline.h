


























#if !defined(LLDB_HOST_EDITLINE_H)
#define LLDB_HOST_EDITLINE_H
#if defined(__cplusplus)

#include "lldb/Host/Config.h"

#if LLDB_EDITLINE_USE_WCHAR
#include <codecvt>
#endif
#include <locale>
#include <sstream>
#include <vector>

#include "lldb/lldb-private.h"

#if !defined(_WIN32) && !defined(__ANDROID__)
#include <histedit.h>
#endif

#include <csignal>
#include <mutex>
#include <string>
#include <vector>

#include "lldb/Host/ConnectionFileDescriptor.h"
#include "lldb/Utility/CompletionRequest.h"
#include "lldb/Utility/FileSpec.h"
#include "lldb/Utility/Predicate.h"
#include "lldb/Utility/StringList.h"

#include "llvm/ADT/FunctionExtras.h"

namespace lldb_private {
namespace line_editor {


#if LLDB_EDITLINE_USE_WCHAR
using EditLineStringType = std::wstring;
using EditLineStringStreamType = std::wstringstream;
using EditLineCharType = wchar_t;
#else
using EditLineStringType = std::string;
using EditLineStringStreamType = std::stringstream;
using EditLineCharType = char;
#endif





#if LLDB_EDITLINE_USE_WCHAR || defined(EL_CLIENTDATA) || LLDB_HAVE_EL_RFUNC_T
using EditLineGetCharType = wchar_t;
#else
using EditLineGetCharType = char;
#endif

using EditlineGetCharCallbackType = int (*)(::EditLine *editline,
EditLineGetCharType *c);
using EditlineCommandCallbackType = unsigned char (*)(::EditLine *editline,
int ch);
using EditlinePromptCallbackType = const char *(*)(::EditLine *editline);

class EditlineHistory;

using EditlineHistorySP = std::shared_ptr<EditlineHistory>;

using IsInputCompleteCallbackType =
llvm::unique_function<bool(Editline *, StringList &)>;

using FixIndentationCallbackType =
llvm::unique_function<int(Editline *, StringList &, int)>;

using SuggestionCallbackType =
llvm::unique_function<llvm::Optional<std::string>(llvm::StringRef)>;

using CompleteCallbackType = llvm::unique_function<void(CompletionRequest &)>;



enum class EditorStatus {


Editing,


Complete,


EndOfInput,


Interrupted
};


enum class CursorLocation {

BlockStart,


EditingPrompt,



EditingCursor,



BlockEnd
};


enum class HistoryOperation {
Oldest,
Older,
Current,
Newer,
Newest
};
}

using namespace line_editor;




class Editline {
public:
Editline(const char *editor_name, FILE *input_file, FILE *output_file,
FILE *error_file, bool color_prompts);

~Editline();



static Editline *InstanceFor(::EditLine *editline);



void SetPrompt(const char *prompt);




void SetContinuationPrompt(const char *continuation_prompt);


void TerminalSizeChanged();


const char *GetPrompt();


uint32_t GetCurrentLine();


bool Interrupt();


bool Cancel();


void SetSuggestionCallback(SuggestionCallbackType callback) {
m_suggestion_callback = std::move(callback);
}


void SetAutoCompleteCallback(CompleteCallbackType callback) {
m_completion_callback = std::move(callback);
}


void SetIsInputCompleteCallback(IsInputCompleteCallbackType callback) {
m_is_input_complete_callback = std::move(callback);
}




void SetFixIndentationCallback(FixIndentationCallbackType callback,
const char *indent_chars) {
m_fix_indentation_callback = std::move(callback);
m_fix_indentation_callback_chars = indent_chars;
}


bool GetLine(std::string &line, bool &interrupted);


bool GetLines(int first_line_number, StringList &lines, bool &interrupted);

void PrintAsync(Stream *stream, const char *s, size_t len);

private:



void SetBaseLineNumber(int line_number);





std::string PromptForIndex(int line_index);




void SetCurrentLine(int line_index);




int GetPromptWidth();




bool IsEmacs();



bool IsOnlySpaces();


int GetLineIndexForLocation(CursorLocation location, int cursor_row);




void MoveCursor(CursorLocation from, CursorLocation to);






void DisplayInput(int firstIndex = 0);





int CountRowsForLine(const EditLineStringType &content);


void SaveEditedLine();


StringList GetInputAsStringList(int line_count = UINT32_MAX);


unsigned char RecallHistory(HistoryOperation op);



int GetCharacter(EditLineGetCharType *c);


const char *Prompt();


unsigned char BreakLineCommand(int ch);


unsigned char EndOrAddLineCommand(int ch);


unsigned char DeleteNextCharCommand(int ch);


unsigned char DeletePreviousCharCommand(int ch);



unsigned char PreviousLineCommand(int ch);



unsigned char NextLineCommand(int ch);



unsigned char PreviousHistoryCommand(int ch);



unsigned char NextHistoryCommand(int ch);


unsigned char BufferStartCommand(int ch);


unsigned char BufferEndCommand(int ch);



unsigned char TabCommand(int ch);


unsigned char ApplyAutosuggestCommand(int ch);


unsigned char TypedCharacter(int ch);


unsigned char FixIndentationCommand(int ch);


unsigned char RevertLineCommand(int ch);



void ConfigureEditor(bool multiline);

bool CompleteCharacter(char ch, EditLineGetCharType &out);

void ApplyTerminalSizeChange();





void AddFunctionToEditLine(const EditLineCharType *command,
const EditLineCharType *helptext,
EditlineCommandCallbackType callbackFn);
void SetEditLinePromptCallback(EditlinePromptCallbackType callbackFn);
void SetGetCharacterFunction(EditlineGetCharCallbackType callbackFn);

#if LLDB_EDITLINE_USE_WCHAR
std::wstring_convert<std::codecvt_utf8<wchar_t>> m_utf8conv;
#endif
::EditLine *m_editline = nullptr;
EditlineHistorySP m_history_sp;
bool m_in_history = false;
std::vector<EditLineStringType> m_live_history_lines;
bool m_multiline_enabled = false;
std::vector<EditLineStringType> m_input_lines;
EditorStatus m_editor_status;
bool m_color_prompts = true;
int m_terminal_width = 0;
int m_base_line_number = 0;
unsigned m_current_line_index = 0;
int m_current_line_rows = -1;
int m_revert_cursor_index = 0;
int m_line_number_digits = 3;
std::string m_set_prompt;
std::string m_set_continuation_prompt;
std::string m_current_prompt;
bool m_needs_prompt_repaint = false;
volatile std::sig_atomic_t m_terminal_size_has_changed = 0;
std::string m_editor_name;
FILE *m_input_file;
FILE *m_output_file;
FILE *m_error_file;
ConnectionFileDescriptor m_input_connection;

IsInputCompleteCallbackType m_is_input_complete_callback;

FixIndentationCallbackType m_fix_indentation_callback;
const char *m_fix_indentation_callback_chars = nullptr;

CompleteCallbackType m_completion_callback;

SuggestionCallbackType m_suggestion_callback;

std::size_t m_previous_autosuggestion_size = 0;
std::mutex m_output_mutex;
};
}

#endif
#endif
