







#if !defined(LLDB_TARGET_STACKFRAMERECOGNIZER_H)
#define LLDB_TARGET_STACKFRAMERECOGNIZER_H

#include "lldb/Core/ValueObject.h"
#include "lldb/Core/ValueObjectList.h"
#include "lldb/Symbol/VariableList.h"
#include "lldb/Target/StopInfo.h"
#include "lldb/Utility/StructuredData.h"
#include "lldb/lldb-private-forward.h"
#include "lldb/lldb-public.h"

#include <vector>

namespace lldb_private {







class RecognizedStackFrame
: public std::enable_shared_from_this<RecognizedStackFrame> {
public:
virtual lldb::ValueObjectListSP GetRecognizedArguments() {
return m_arguments;
}
virtual lldb::ValueObjectSP GetExceptionObject() {
return lldb::ValueObjectSP();
}
virtual lldb::StackFrameSP GetMostRelevantFrame() { return nullptr; };
virtual ~RecognizedStackFrame() = default;

std::string GetStopDescription() { return m_stop_desc; }

protected:
lldb::ValueObjectListSP m_arguments;
std::string m_stop_desc;
};







class StackFrameRecognizer
: public std::enable_shared_from_this<StackFrameRecognizer> {
public:
virtual lldb::RecognizedStackFrameSP RecognizeFrame(
lldb::StackFrameSP frame) {
return lldb::RecognizedStackFrameSP();
};
virtual std::string GetName() {
return "";
}

virtual ~StackFrameRecognizer() = default;
};







class ScriptedStackFrameRecognizer : public StackFrameRecognizer {
lldb_private::ScriptInterpreter *m_interpreter;
lldb_private::StructuredData::ObjectSP m_python_object_sp;
std::string m_python_class;

public:
ScriptedStackFrameRecognizer(lldb_private::ScriptInterpreter *interpreter,
const char *pclass);
~ScriptedStackFrameRecognizer() override = default;

std::string GetName() override {
return GetPythonClassName();
}

const char *GetPythonClassName() { return m_python_class.c_str(); }

lldb::RecognizedStackFrameSP RecognizeFrame(
lldb::StackFrameSP frame) override;

private:
ScriptedStackFrameRecognizer(const ScriptedStackFrameRecognizer &) = delete;
const ScriptedStackFrameRecognizer &
operator=(const ScriptedStackFrameRecognizer &) = delete;
};


class StackFrameRecognizerManager {
public:
void AddRecognizer(lldb::StackFrameRecognizerSP recognizer,
ConstString module, llvm::ArrayRef<ConstString> symbols,
bool first_instruction_only = true);

void AddRecognizer(lldb::StackFrameRecognizerSP recognizer,
lldb::RegularExpressionSP module,
lldb::RegularExpressionSP symbol,
bool first_instruction_only = true);

void ForEach(std::function<
void(uint32_t recognizer_id, std::string recognizer_name,
std::string module, llvm::ArrayRef<ConstString> symbols,
bool regexp)> const &callback);

bool RemoveRecognizerWithID(uint32_t recognizer_id);

void RemoveAllRecognizers();

lldb::StackFrameRecognizerSP GetRecognizerForFrame(lldb::StackFrameSP frame);

lldb::RecognizedStackFrameSP RecognizeFrame(lldb::StackFrameSP frame);

private:
struct RegisteredEntry {
uint32_t recognizer_id;
lldb::StackFrameRecognizerSP recognizer;
bool is_regexp;
ConstString module;
lldb::RegularExpressionSP module_regexp;
std::vector<ConstString> symbols;
lldb::RegularExpressionSP symbol_regexp;
bool first_instruction_only;
};

std::deque<RegisteredEntry> m_recognizers;
};







class ValueObjectRecognizerSynthesizedValue : public ValueObject {
public:
static lldb::ValueObjectSP Create(ValueObject &parent, lldb::ValueType type) {
return (new ValueObjectRecognizerSynthesizedValue(parent, type))->GetSP();
}
ValueObjectRecognizerSynthesizedValue(ValueObject &parent,
lldb::ValueType type)
: ValueObject(parent), m_type(type) {
SetName(parent.GetName());
}

llvm::Optional<uint64_t> GetByteSize() override {
return m_parent->GetByteSize();
}
lldb::ValueType GetValueType() const override { return m_type; }
bool UpdateValue() override {
if (!m_parent->UpdateValueIfNeeded()) return false;
m_value = m_parent->GetValue();
return true;
}
size_t CalculateNumChildren(uint32_t max = UINT32_MAX) override {
return m_parent->GetNumChildren(max);
}
CompilerType GetCompilerTypeImpl() override {
return m_parent->GetCompilerType();
}
bool IsSynthetic() override { return true; }

private:
lldb::ValueType m_type;
};

}

#endif
