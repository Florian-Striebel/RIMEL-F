



#if !defined(_PROV_H)
#define _PROV_H

#include <unistd.h>

#if defined(__cplusplus)
extern "C" {
#endif

#if _DTRACE_VERSION

#define TESTER_ENTRY() __dtrace_tester___entry()

#if !defined(__sparc)
#define TESTER_ENTRY_ENABLED() __dtraceenabled_tester___entry()

#else
#define TESTER_ENTRY_ENABLED() __dtraceenabled_tester___entry(0)

#endif


extern void __dtrace_tester___entry(void);
#if !defined(__sparc)
extern int __dtraceenabled_tester___entry(void);
#else
extern int __dtraceenabled_tester___entry(long);
#endif

#else

#define TESTER_ENTRY()
#define TESTER_ENTRY_ENABLED() (0)

#endif


#if defined(__cplusplus)
}
#endif

#endif
