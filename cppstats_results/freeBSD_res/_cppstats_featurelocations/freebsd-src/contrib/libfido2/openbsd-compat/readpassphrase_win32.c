
































#define UMDF_USING_NTSTATUS
#define SECURITY_WIN32
#include <windows.h>
#include <stdio.h>
#include <time.h>
#include <shlwapi.h>
#include <conio.h>
#include <lm.h>
#include <sddl.h>
#include <aclapi.h>
#include <ntsecapi.h>
#include <security.h>
#include <ntstatus.h>
#include <wchar.h>

#include "openbsd-compat.h"

#if !defined(HAVE_READPASSPHRASE)


static wchar_t *
utf8_to_utf16(const char *utf8)
{
int needed = 0;
wchar_t* utf16 = NULL;
if ((needed = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, NULL, 0)) == 0 ||
(utf16 = malloc(needed * sizeof(wchar_t))) == NULL ||
MultiByteToWideChar(CP_UTF8, 0, utf8, -1, utf16, needed) == 0) {

errno = ENOMEM;
return NULL;
}

return utf16;
}

char *
readpassphrase(const char *prompt, char *outBuf, size_t outBufLen, int flags)
{
size_t current_index = 0;
char ch;
wchar_t* wtmp = NULL;

if (outBufLen == 0) {
errno = EINVAL;
return NULL;
}

while (_kbhit()) (void)_getch();

wtmp = utf8_to_utf16(prompt);
if (wtmp == NULL)
errx(1, "unable to alloc memory");

_cputws(wtmp);
free(wtmp);

while (current_index < outBufLen - 1) {
ch = (char)_getch();

if (ch == '\r') {
if (_kbhit()) (void)_getch();
break;
} else if (ch == '\n') {
break;
} else if (ch == '\b') {
if (current_index > 0) {
if (flags & RPP_ECHO_ON)
printf_s("%c \b", ch);

current_index--;
}
} else if (ch == '\003') {
errx(1, "");
} else {
if (flags & RPP_SEVENBIT)
ch &= 0x7f;

if (isalpha((unsigned char)ch)) {
if(flags & RPP_FORCELOWER)
ch = (char)tolower((unsigned char)ch);
if(flags & RPP_FORCEUPPER)
ch = (char)toupper((unsigned char)ch);
}

outBuf[current_index++] = ch;
if(flags & RPP_ECHO_ON)
printf_s("%c", ch);
}
}

outBuf[current_index] = '\0';
_cputs("\n");

return outBuf;
}

#endif
