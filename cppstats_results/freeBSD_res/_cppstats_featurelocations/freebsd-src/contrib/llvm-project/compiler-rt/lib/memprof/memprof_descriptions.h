











#if !defined(MEMPROF_DESCRIPTIONS_H)
#define MEMPROF_DESCRIPTIONS_H

#include "memprof_allocator.h"
#include "memprof_thread.h"
#include "sanitizer_common/sanitizer_common.h"
#include "sanitizer_common/sanitizer_report_decorator.h"

namespace __memprof {

void DescribeThread(MemprofThreadContext *context);
inline void DescribeThread(MemprofThread *t) {
if (t)
DescribeThread(t->context());
}

class MemprofThreadIdAndName {
public:
explicit MemprofThreadIdAndName(MemprofThreadContext *t);
explicit MemprofThreadIdAndName(u32 tid);


const char *c_str() const { return &name[0]; }

private:
void Init(u32 tid, const char *tname);

char name[128];
};

}

#endif
