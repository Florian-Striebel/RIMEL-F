#include <stdio.h>
#include <stdarg.h>

#include "portability.h"

int
pcap_vsnprintf(char *str, size_t str_size, const char *format, va_list args)
{
int ret;

ret = _vsnprintf_s(str, str_size, _TRUNCATE, format, args);

















str[str_size - 1] = '\0';
return (ret);
}

int
pcap_snprintf(char *str, size_t str_size, const char *format, ...)
{
va_list args;
int ret;

va_start(args, format);
ret = pcap_vsnprintf(str, str_size, format, args);
va_end(args);
return (ret);
}
