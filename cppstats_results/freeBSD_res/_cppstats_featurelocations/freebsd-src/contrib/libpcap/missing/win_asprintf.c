#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "portability.h"

int
pcap_vasprintf(char **strp, const char *format, va_list args)
{
int len;
size_t str_size;
char *str;
int ret;

len = _vscprintf(format, args);
if (len == -1) {
*strp = NULL;
return (-1);
}
str_size = len + 1;
str = malloc(str_size);
if (str == NULL) {
*strp = NULL;
return (-1);
}
ret = pcap_vsnprintf(str, str_size, format, args);
if (ret == -1) {
free(str);
*strp = NULL;
return (-1);
}
*strp = str;





return (ret);
}

int
pcap_asprintf(char **strp, const char *format, ...)
{
va_list args;
int ret;

va_start(args, format);
ret = pcap_vasprintf(strp, format, args);
va_end(args);
return (ret);
}
