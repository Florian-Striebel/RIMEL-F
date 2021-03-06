







#if !defined(LLDB_TARGET_STACKFRAMELIST_H)
#define LLDB_TARGET_STACKFRAMELIST_H

#include <memory>
#include <mutex>
#include <vector>

#include "lldb/Target/StackFrame.h"

namespace lldb_private {

class StackFrameList {
public:

StackFrameList(Thread &thread, const lldb::StackFrameListSP &prev_frames_sp,
bool show_inline_frames);

~StackFrameList();




uint32_t GetNumFrames(bool can_create = true);


lldb::StackFrameSP GetFrameAtIndex(uint32_t idx);



lldb::StackFrameSP GetFrameWithConcreteFrameIndex(uint32_t unwind_idx);


lldb::StackFrameSP GetFrameWithStackID(const StackID &stack_id);


uint32_t SetSelectedFrame(lldb_private::StackFrame *frame);


uint32_t GetSelectedFrameIndex() const;



bool SetSelectedFrameByIndex(uint32_t idx);



uint32_t GetVisibleStackFrameIndex(uint32_t idx) {
if (m_current_inlined_depth < UINT32_MAX)
return idx - m_current_inlined_depth;
else
return idx;
}




void CalculateCurrentInlinedDepth();




void SetDefaultFileAndLineToSelectedFrame();


void Clear();

void Dump(Stream *s);



lldb::StackFrameSP
GetStackFrameSPForStackFramePtr(StackFrame *stack_frame_ptr);

size_t GetStatus(Stream &strm, uint32_t first_frame, uint32_t num_frames,
bool show_frame_info, uint32_t num_frames_with_source,
bool show_unique = false,
const char *frame_marker = nullptr);

protected:
friend class Thread;

bool SetFrameAtIndex(uint32_t idx, lldb::StackFrameSP &frame_sp);

void GetFramesUpTo(uint32_t end_idx);

void GetOnlyConcreteFramesUpTo(uint32_t end_idx, Unwind &unwinder);

void SynthesizeTailCallFrames(StackFrame &next_frame);

bool GetAllFramesFetched() { return m_concrete_frames_fetched == UINT32_MAX; }

void SetAllFramesFetched() { m_concrete_frames_fetched = UINT32_MAX; }

bool DecrementCurrentInlinedDepth();

void ResetCurrentInlinedDepth();

uint32_t GetCurrentInlinedDepth();

void SetCurrentInlinedDepth(uint32_t new_depth);

typedef std::vector<lldb::StackFrameSP> collection;
typedef collection::iterator iterator;
typedef collection::const_iterator const_iterator;


Thread &m_thread;






lldb::StackFrameListSP m_prev_frames_sp;





mutable std::recursive_mutex m_mutex;



collection m_frames;


uint32_t m_selected_frame_idx;



uint32_t m_concrete_frames_fetched;




uint32_t m_current_inlined_depth;




lldb::addr_t m_current_inlined_pc;


const bool m_show_inlined_frames;

private:
StackFrameList(const StackFrameList &) = delete;
const StackFrameList &operator=(const StackFrameList &) = delete;
};

}

#endif
