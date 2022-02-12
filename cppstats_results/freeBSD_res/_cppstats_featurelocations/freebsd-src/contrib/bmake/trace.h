



































#if !defined(MAKE_TRACE_H)
#define MAKE_TRACE_H

typedef enum TrEvent {
MAKESTART,
MAKEEND,
MAKEERROR,
JOBSTART,
JOBEND,
MAKEINTR
} TrEvent;

void Trace_Init(const char *);
void Trace_Log(TrEvent, Job *);
void Trace_End(void);

#endif
