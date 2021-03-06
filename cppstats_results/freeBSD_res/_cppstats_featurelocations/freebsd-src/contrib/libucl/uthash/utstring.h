
























#if !defined(UTSTRING_H)
#define UTSTRING_H

#define UTSTRING_VERSION 1.9.8

#if defined(__GNUC__)
#define _UNUSED_ __attribute__ ((__unused__))
#else
#define _UNUSED_
#endif

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#if !defined(oom)
#define oom abort
#endif

typedef struct {
char *d;
void **pd;
size_t n;
size_t i;
} UT_string;

#define utstring_reserve(s,amt) do { if (((s)->n - (s)->i) < (size_t)(amt)) { (s)->d = (char*)realloc((s)->d, (s)->n + amt); if ((s)->d == NULL) oom(); else {(s)->n += amt; if ((s)->pd) *((s)->pd) = (s)->d;} } } while(0)









#define utstring_init(s) do { (s)->n = 0; (s)->i = 0; (s)->d = NULL; utstring_reserve(s,128); (s)->d[0] = '\0'; } while(0)






#define utstring_done(s) do { if ((s)->d != NULL) free((s)->d); (s)->n = 0; } while(0)





#define utstring_free(s) do { utstring_done(s); free(s); } while(0)





#define utstring_new(s) do { s = (UT_string*)calloc(1, sizeof(UT_string)); if (!s) oom(); else utstring_init(s); } while(0)






#define utstring_renew(s) do { if (s) { utstring_clear(s); } else { utstring_new(s); } } while(0)








#define utstring_clear(s) do { (s)->i = 0; (s)->d[0] = '\0'; } while(0)





#define utstring_bincpy(s,b,l) do { utstring_reserve((s),(l)+1); if (l) memcpy(&(s)->d[(s)->i], b, l); (s)->i += l; (s)->d[(s)->i]='\0'; } while(0)







#define utstring_concat(dst,src) do { utstring_reserve((dst),((src)->i)+1); if ((src)->i) memcpy(&(dst)->d[(dst)->i], (src)->d, (src)->i); (dst)->i += (src)->i; (dst)->d[(dst)->i]='\0'; } while(0)







#define utstring_len(s) ((unsigned)((s)->i))

#define utstring_body(s) ((s)->d)

_UNUSED_ static void utstring_printf_va(UT_string *s, const char *fmt, va_list ap) {
int n;
va_list cp;
while (1) {
#if defined(_WIN32)
cp = ap;
#else
va_copy(cp, ap);
#endif
n = vsnprintf (&s->d[s->i], s->n-s->i, fmt, cp);
va_end(cp);

if ((n > -1) && (n < (int)(s->n-s->i))) {
s->i += n;
return;
}


if (n > -1) utstring_reserve(s,n+1);
else utstring_reserve(s,(s->n)*2);
}
}
#if defined(__GNUC__)

static void utstring_printf(UT_string *s, const char *fmt, ...)
__attribute__ (( format( printf, 2, 3) ));
#endif
_UNUSED_ static void utstring_printf(UT_string *s, const char *fmt, ...) {
va_list ap;
va_start(ap,fmt);
utstring_printf_va(s,fmt,ap);
va_end(ap);
}

#define utstring_append_len(dst, src, len) do { while ((dst)->n-(dst)->i <= (len)) utstring_reserve((dst),((dst)->n)*2); memcpy(&(dst)->d[(dst)->i], (src), (len)); (dst)->i+=(len); (dst)->d[(dst)->i]='\0'; } while(0)







#define utstring_append_c(dst, c) do { if ((dst)->n-(dst)->i < 2) utstring_reserve((dst),((dst)->n)*2); (dst)->d[(dst)->i++] = (c); (dst)->d[(dst)->i]='\0'; } while(0)










_UNUSED_ static void _utstring_BuildTable(
const char *P_Needle,
ssize_t P_NeedleLen,
long *P_KMP_Table)
{
long i, j;

i = 0;
j = i - 1;
P_KMP_Table[i] = j;
while (i < P_NeedleLen)
{
while ( (j > -1) && (P_Needle[i] != P_Needle[j]) )
{
j = P_KMP_Table[j];
}
i++;
j++;
if (i < P_NeedleLen)
{
if (P_Needle[i] == P_Needle[j])
{
P_KMP_Table[i] = P_KMP_Table[j];
}
else
{
P_KMP_Table[i] = j;
}
}
else
{
P_KMP_Table[i] = j;
}
}

return;
}



_UNUSED_ static void _utstring_BuildTableR(
const char *P_Needle,
ssize_t P_NeedleLen,
long *P_KMP_Table)
{
long i, j;

i = P_NeedleLen - 1;
j = i + 1;
P_KMP_Table[i + 1] = j;
while (i >= 0)
{
while ( (j < P_NeedleLen) && (P_Needle[i] != P_Needle[j]) )
{
j = P_KMP_Table[j + 1];
}
i--;
j--;
if (i >= 0)
{
if (P_Needle[i] == P_Needle[j])
{
P_KMP_Table[i + 1] = P_KMP_Table[j + 1];
}
else
{
P_KMP_Table[i + 1] = j;
}
}
else
{
P_KMP_Table[i + 1] = j;
}
}

return;
}



_UNUSED_ static long _utstring_find(
const char *P_Haystack,
size_t P_HaystackLen,
const char *P_Needle,
size_t P_NeedleLen,
long *P_KMP_Table)
{
long i, j;
long V_FindPosition = -1;


i = j = 0;
while ( (j < (int)P_HaystackLen) && (((P_HaystackLen - j) + i) >= P_NeedleLen) )
{
while ( (i > -1) && (P_Needle[i] != P_Haystack[j]) )
{
i = P_KMP_Table[i];
}
i++;
j++;
if (i >= (int)P_NeedleLen)
{

V_FindPosition = j - i;
break;
}
}

return V_FindPosition;
}



_UNUSED_ static long _utstring_findR(
const char *P_Haystack,
size_t P_HaystackLen,
const char *P_Needle,
size_t P_NeedleLen,
long *P_KMP_Table)
{
long i, j;
long V_FindPosition = -1;


j = (P_HaystackLen - 1);
i = (P_NeedleLen - 1);
while ( (j >= 0) && (j >= i) )
{
while ( (i < (int)P_NeedleLen) && (P_Needle[i] != P_Haystack[j]) )
{
i = P_KMP_Table[i + 1];
}
i--;
j--;
if (i < 0)
{

V_FindPosition = j + 1;
break;
}
}

return V_FindPosition;
}



_UNUSED_ static long utstring_find(
UT_string *s,
long P_StartPosition,
const char *P_Needle,
ssize_t P_NeedleLen)
{
long V_StartPosition;
long V_HaystackLen;
long *V_KMP_Table;
long V_FindPosition = -1;

if (P_StartPosition < 0)
{
V_StartPosition = s->i + P_StartPosition;
}
else
{
V_StartPosition = P_StartPosition;
}
V_HaystackLen = s->i - V_StartPosition;
if ( (V_HaystackLen >= P_NeedleLen) && (P_NeedleLen > 0) )
{
V_KMP_Table = (long *)malloc(sizeof(long) * (P_NeedleLen + 1));
if (V_KMP_Table != NULL)
{
_utstring_BuildTable(P_Needle, P_NeedleLen, V_KMP_Table);

V_FindPosition = _utstring_find(s->d + V_StartPosition,
V_HaystackLen,
P_Needle,
P_NeedleLen,
V_KMP_Table);
if (V_FindPosition >= 0)
{
V_FindPosition += V_StartPosition;
}

free(V_KMP_Table);
}
}

return V_FindPosition;
}



_UNUSED_ static long utstring_findR(
UT_string *s,
long P_StartPosition,
const char *P_Needle,
ssize_t P_NeedleLen)
{
long V_StartPosition;
long V_HaystackLen;
long *V_KMP_Table;
long V_FindPosition = -1;

if (P_StartPosition < 0)
{
V_StartPosition = s->i + P_StartPosition;
}
else
{
V_StartPosition = P_StartPosition;
}
V_HaystackLen = V_StartPosition + 1;
if ( (V_HaystackLen >= P_NeedleLen) && (P_NeedleLen > 0) )
{
V_KMP_Table = (long *)malloc(sizeof(long) * (P_NeedleLen + 1));
if (V_KMP_Table != NULL)
{
_utstring_BuildTableR(P_Needle, P_NeedleLen, V_KMP_Table);

V_FindPosition = _utstring_findR(s->d,
V_HaystackLen,
P_Needle,
P_NeedleLen,
V_KMP_Table);

free(V_KMP_Table);
}
}

return V_FindPosition;
}




#endif
