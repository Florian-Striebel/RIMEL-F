
































#if !defined(fmtutils_h)
#define fmtutils_h

#include "pcap/funcattrs.h"

#if defined(__cplusplus)
extern "C" {
#endif

void pcap_fmt_errmsg_for_errno(char *, size_t, int,
PCAP_FORMAT_STRING(const char *), ...) PCAP_PRINTFLIKE(4, 5);

#if defined(_WIN32)
void pcap_fmt_errmsg_for_win32_err(char *, size_t, DWORD,
PCAP_FORMAT_STRING(const char *), ...) PCAP_PRINTFLIKE(4, 5);
#endif

#if defined(__cplusplus)
}
#endif

#endif
