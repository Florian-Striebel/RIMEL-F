










#if !defined(TSAN_STACK_TRACE_H)
#define TSAN_STACK_TRACE_H

#include "sanitizer_common/sanitizer_stacktrace.h"
#include "tsan_defs.h"

namespace __tsan {



struct VarSizeStackTrace : public StackTrace {
uptr *trace_buffer;

VarSizeStackTrace();
~VarSizeStackTrace();
void Init(const uptr *pcs, uptr cnt, uptr extra_top_pc = 0);



void ReverseOrder();

private:
void ResizeBuffer(uptr new_size);

VarSizeStackTrace(const VarSizeStackTrace &);
void operator=(const VarSizeStackTrace &);
};

}

#endif
