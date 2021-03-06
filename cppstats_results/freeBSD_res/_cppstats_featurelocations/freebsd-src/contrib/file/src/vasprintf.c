










































































































#include "file.h"

#if !defined(lint)
FILE_RCSID("@(#)$File: vasprintf.c,v 1.19 2021/02/23 00:51:11 christos Exp $")
#endif

#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <limits.h>
#include <stddef.h>

#define ALLOC_CHUNK 2048
#define ALLOC_SECURITY_MARGIN 1024
#if ALLOC_CHUNK < ALLOC_SECURITY_MARGIN
#error !!! ALLOC_CHUNK < ALLOC_SECURITY_MARGIN !!!
#endif






typedef struct {
const char * src_string;
char * buffer_base;
char * dest_string;
size_t buffer_len;
size_t real_len;
size_t pseudo_len;
size_t maxlen;
va_list vargs;
char * sprintf_string;
FILE * fprintf_file;
} xprintf_struct;






static int realloc_buff(xprintf_struct *s, size_t len)
{
char * ptr;

if (len + ALLOC_SECURITY_MARGIN + s->real_len > s->buffer_len) {
len += s->real_len + ALLOC_CHUNK;
ptr = (char *)realloc((void *)(s->buffer_base), len);
if (ptr == NULL) {
s->buffer_base = NULL;
return EOF;
}

s->dest_string = ptr + (size_t)(s->dest_string - s->buffer_base);
s->buffer_base = ptr;
s->buffer_len = len;

(s->buffer_base)[s->buffer_len - 1] = 1;
}

return 0;
}





static int usual_char(xprintf_struct * s)
{
size_t len;

len = strcspn(s->src_string, "%");



if (realloc_buff(s,len) == EOF)
return EOF;

memcpy(s->dest_string, s->src_string, len);
s->src_string += len;
s->dest_string += len;
s->real_len += len;
s->pseudo_len += len;

return 0;
}





static int print_it(xprintf_struct *s, size_t approx_len,
const char *format_string, ...)
{
va_list varg;
int vsprintf_len;
size_t len;

if (realloc_buff(s,approx_len) == EOF)
return EOF;

va_start(varg, format_string);
vsprintf_len = vsprintf(s->dest_string, format_string, varg);
va_end(varg);


assert((s->buffer_base)[s->buffer_len - 1] == 1);

if (vsprintf_len == EOF)
return EOF;

s->pseudo_len += vsprintf_len;
len = strlen(s->dest_string);
s->real_len += len;
s->dest_string += len;

return 0;
}












static int type_s(xprintf_struct *s, int width, int prec,
const char *format_string, const char *arg_string)
{
size_t string_len;

if (arg_string == NULL)
return print_it(s, (size_t)6, "(null)", 0);



string_len = 0;
while (arg_string[string_len] != 0 && (size_t)prec != string_len)
string_len++;

if (width != -1 && string_len < (size_t)width)
string_len = (size_t)width;

return print_it(s, string_len, format_string, arg_string);
}








static int getint(const char **string)
{
int i = 0;

while (isdigit((unsigned char)**string) != 0) {
i = i * 10 + (**string - '0');
(*string)++;
}

if (i < 0 || i > 32767)
i = 32767;

return i;
}













static int dispatch(xprintf_struct *s)
{
const char *initial_ptr;
char format_string[24];
char *format_ptr;
int flag_plus, flag_minus, flag_space, flag_sharp, flag_zero;
int width, prec, modifier, approx_width;
char type;


#define SRCTXT (s->src_string)
#define DESTTXT (s->dest_string)


#define INCOHERENT() do {SRCTXT=initial_ptr; return 0;} while (0)
#define INCOHERENT_TEST() do {if(*SRCTXT==0) INCOHERENT();} while (0)


if (*SRCTXT != '%')
return usual_char(s);


SRCTXT++;



if (*SRCTXT == '%') {
if (realloc_buff(s, (size_t)1) == EOF)
return EOF;
*DESTTXT = '%';
DESTTXT++;
SRCTXT++;
(s->real_len)++;
(s->pseudo_len)++;
return 0;
}


initial_ptr = SRCTXT;




flag_plus = flag_minus = flag_space = flag_sharp = flag_zero = 0;

for (;; SRCTXT++) {
if (*SRCTXT == ' ')
flag_space = 1;
else if (*SRCTXT == '+')
flag_plus = 1;
else if (*SRCTXT == '-')
flag_minus = 1;
else if (*SRCTXT == '#')
flag_sharp = 1;
else if (*SRCTXT == '0')
flag_zero = 1;
else
break;
}

INCOHERENT_TEST();


if (*SRCTXT == '*') {
SRCTXT++;
width = va_arg(s->vargs, int);
if ((size_t)width > 0x3fffU)
width = 0x3fff;
} else if (isdigit((unsigned char)*SRCTXT))
width = getint(&SRCTXT);
else
width = -1;

INCOHERENT_TEST();


if (*SRCTXT == '.') {
SRCTXT++;
if (*SRCTXT == '*') {
SRCTXT++;
prec = va_arg(s->vargs, int);
if ((size_t)prec >= 0x3fffU)
prec = 0x3fff;
} else {
if (isdigit((unsigned char)*SRCTXT) == 0)
INCOHERENT();
prec = getint(&SRCTXT);
}
INCOHERENT_TEST();
} else
prec = -1;


switch (*SRCTXT) {
case 'L':
case 'h':
case 'l':
case 'z':
case 't':
modifier = *SRCTXT;
SRCTXT++;
if (modifier=='l' && *SRCTXT=='l') {
SRCTXT++;
modifier = 'L';
}
INCOHERENT_TEST();
break;
default:
modifier = -1;
break;
}


type = *SRCTXT;
if (strchr("diouxXfegEGcspn",type) == NULL)
INCOHERENT();
SRCTXT++;


format_string[0] = '%';
format_ptr = &(format_string[1]);

if (flag_plus) {
*format_ptr = '+';
format_ptr++;
}
if (flag_minus) {
*format_ptr = '-';
format_ptr++;
}
if (flag_space) {
*format_ptr = ' ';
format_ptr++;
}
if (flag_sharp) {
*format_ptr = '#';
format_ptr++;
}
if (flag_zero) {
*format_ptr = '0';
format_ptr++;
}

if (width != -1) {
sprintf(format_ptr, "%i", width);
format_ptr += strlen(format_ptr);
}

if (prec != -1) {
*format_ptr = '.';
format_ptr++;
sprintf(format_ptr, "%i", prec);
format_ptr += strlen(format_ptr);
}

if (modifier != -1) {
if (modifier == 'L' && strchr("diouxX",type) != NULL) {
*format_ptr = 'l';
format_ptr++;
*format_ptr = 'l';
format_ptr++;
} else {
*format_ptr = modifier;
format_ptr++;
}
}

*format_ptr = type;
format_ptr++;
*format_ptr = 0;


approx_width = width + prec;
if (approx_width < 0)
approx_width = 0;

switch (type) {

case 'd':
case 'i':
case 'o':
case 'u':
case 'x':
case 'X':
switch (modifier) {
case -1 :
return print_it(s, (size_t)approx_width, format_string, va_arg(s->vargs, int));
case 'L':
return print_it(s, (size_t)approx_width, format_string, va_arg(s->vargs, long long int));
case 'l':
return print_it(s, (size_t)approx_width, format_string, va_arg(s->vargs, long int));
case 'h':
return print_it(s, (size_t)approx_width, format_string, va_arg(s->vargs, int));
case 'z':
return print_it(s, (size_t)approx_width, format_string, va_arg(s->vargs, size_t));
case 't':
return print_it(s, (size_t)approx_width, format_string, va_arg(s->vargs, ptrdiff_t));

default:
INCOHERENT();
}


case 'c':
if (modifier != -1)
INCOHERENT();
return print_it(s, (size_t)approx_width, format_string, va_arg(s->vargs, int));



case 'e':
case 'f':
case 'g':
case 'E':
case 'G':
switch (modifier) {
case -1 :
case 'l':
return print_it(s, (size_t)approx_width, format_string, va_arg(s->vargs, double));
case 'L':
return print_it(s, (size_t)approx_width, format_string, va_arg(s->vargs, long double));
default:
INCOHERENT();
}


case 's':
return type_s(s, width, prec, format_string, va_arg(s->vargs, const char*));


case 'p':
if (modifier == -1)
return print_it(s, (size_t)approx_width, format_string, va_arg(s->vargs, void *));
INCOHERENT();


case 'n':
if (modifier == -1) {
int * p;
p = va_arg(s->vargs, int *);
if (p != NULL) {
*p = s->pseudo_len;
return 0;
}
return EOF;
}
INCOHERENT();

}

INCOHERENT();

#undef INCOHERENT
#undef INCOHERENT_TEST
#undef SRCTXT
#undef DESTTXT
}





static int core(xprintf_struct *s)
{
size_t save_len;
char *dummy_base;


if ((int)(s->maxlen) <= 0)
return EOF;
s->maxlen--;


if (s->src_string == NULL)
s->src_string = "(null)";


s->buffer_base = NULL;
s->buffer_len = 0;
s->real_len = 0;
s->pseudo_len = 0;
if (realloc_buff(s, (size_t)0) == EOF)
return EOF;
s->dest_string = s->buffer_base;


for (;;) {

if (*(s->src_string) == 0) {
*(s->dest_string) = '\0';
break;
}

if (dispatch(s) == EOF)
goto free_EOF;


if (s->real_len >= s->maxlen) {
(s->buffer_base)[s->maxlen] = '\0';
break;
}
}


dummy_base = s->buffer_base;

dummy_base = s->buffer_base + s->real_len;
save_len = s->real_len;




while(*(s->src_string) != 0) {
s->real_len = 0;
s->dest_string = dummy_base;
if (dispatch(s) == EOF)
goto free_EOF;
}

s->buffer_base = (char *)realloc((void *)(s->buffer_base), save_len + 1);
if (s->buffer_base == NULL)
return EOF;
return s->pseudo_len;

free_EOF:
free(s->buffer_base);
return EOF;
}

int vasprintf(char **ptr, const char *format_string, va_list vargs)
{
xprintf_struct s;
int retval;

s.src_string = format_string;
#if defined(va_copy)
va_copy (s.vargs, vargs);
#else
#if defined(__va_copy)
__va_copy (s.vargs, vargs);
#else
#if defined(WIN32)
s.vargs = vargs;
#else
memcpy (&s.vargs, &vargs, sizeof (s.va_args));
#endif
#endif
#endif
s.maxlen = (size_t)INT_MAX;

retval = core(&s);
va_end(s.vargs);
if (retval == EOF) {
*ptr = NULL;
return EOF;
}

*ptr = s.buffer_base;
return retval;
}
