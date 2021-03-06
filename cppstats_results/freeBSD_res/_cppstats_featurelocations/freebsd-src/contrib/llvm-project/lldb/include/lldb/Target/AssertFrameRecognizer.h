







#if !defined(LLDB_TARGET_ASSERTFRAMERECOGNIZER_H)
#define LLDB_TARGET_ASSERTFRAMERECOGNIZER_H

#include "lldb/Target/Process.h"
#include "lldb/Target/StackFrameRecognizer.h"
#include "lldb/Utility/ConstString.h"
#include "lldb/Utility/FileSpec.h"

#include <tuple>

namespace lldb_private {






void RegisterAssertFrameRecognizer(Process *process);




class AssertRecognizedStackFrame : public RecognizedStackFrame {
public:
AssertRecognizedStackFrame(lldb::StackFrameSP most_relevant_frame_sp);
lldb::StackFrameSP GetMostRelevantFrame() override;

private:
lldb::StackFrameSP m_most_relevant_frame;
};






class AssertFrameRecognizer : public StackFrameRecognizer {
public:
std::string GetName() override { return "Assert StackFrame Recognizer"; }
lldb::RecognizedStackFrameSP
RecognizeFrame(lldb::StackFrameSP frame_sp) override;
};

}

#endif
