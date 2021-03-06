




































#if defined(HAVE_CONFIG_H)
#include <config.h>
#endif

#include "ftmacros.h"

#include <stddef.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <pcap/pcap.h>

#include "portability.h"

#include "fmtutils.h"





void
pcap_fmt_errmsg_for_errno(char *errbuf, size_t errbuflen, int errnum,
const char *fmt, ...)
{
va_list ap;
size_t msglen;
char *p;
size_t errbuflen_remaining;

va_start(ap, fmt);
pcap_vsnprintf(errbuf, errbuflen, fmt, ap);
va_end(ap);
msglen = strlen(errbuf);





if (msglen + 3 > errbuflen) {

return;
}
p = errbuf + msglen;
errbuflen_remaining = errbuflen - msglen;
*p++ = ':';
*p++ = ' ';
*p = '\0';
msglen += 2;
errbuflen_remaining -= 2;




#if defined(HAVE_STRERROR_S)



errno_t err = strerror_s(p, errbuflen_remaining, errnum);
if (err != 0) {




pcap_snprintf(p, errbuflen_remaining, "Error %d", errnum);
}
#elif defined(HAVE_GNU_STRERROR_R)








char strerror_buf[PCAP_ERRBUF_SIZE];
char *errstring = strerror_r(errnum, strerror_buf, PCAP_ERRBUF_SIZE);
pcap_snprintf(p, errbuflen_remaining, "%s", errstring);
#elif defined(HAVE_POSIX_STRERROR_R)




int err = strerror_r(errnum, p, errbuflen_remaining);
if (err == EINVAL) {




pcap_snprintf(p, errbuflen_remaining, "Unknown error: %d",
errnum);
} else if (err == ERANGE) {




pcap_snprintf(p, errbuflen_remaining,
"Message for error %d is too long", errnum);
}
#else




pcap_snprintf(p, errbuflen_remaining, "%s", pcap_strerror(errnum));
#endif
}

#if defined(_WIN32)




void
pcap_fmt_errmsg_for_win32_err(char *errbuf, size_t errbuflen, DWORD errnum,
const char *fmt, ...)
{
va_list ap;
size_t msglen;
char *p;
size_t errbuflen_remaining;
DWORD retval;
char win32_errbuf[PCAP_ERRBUF_SIZE+1];

va_start(ap, fmt);
pcap_vsnprintf(errbuf, errbuflen, fmt, ap);
va_end(ap);
msglen = strlen(errbuf);





if (msglen + 3 > errbuflen) {

return;
}
p = errbuf + msglen;
errbuflen_remaining = errbuflen - msglen;
*p++ = ':';
*p++ = ' ';
*p = '\0';
msglen += 2;
errbuflen_remaining -= 2;

















retval = FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_IGNORE_INSERTS|FORMAT_MESSAGE_MAX_WIDTH_MASK,
NULL, errnum, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
win32_errbuf, PCAP_ERRBUF_SIZE, NULL);
if (retval == 0) {



pcap_snprintf(p, errbuflen_remaining,
"Couldn't get error message for error (%lu)", errnum);
return;
}

pcap_snprintf(p, errbuflen_remaining, "%s (%lu)", win32_errbuf, errnum);
}
#endif
