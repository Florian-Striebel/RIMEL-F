










































#if defined(_KERNEL) || defined(_STANDALONE) || defined(HAVE_NBTOOL_CONFIG_H) || defined(BCS_ONLY)
__INT
_FUNCNAME(const char *nptr, char **endptr, int base)
#else
#include <locale.h>
#include "setlocale_local.h"
#define INT_FUNCNAME_(pre, name, post) pre ##name ##post
#define INT_FUNCNAME(pre, name, post) INT_FUNCNAME_(pre, name, post)

static __INT
INT_FUNCNAME(_int_, _FUNCNAME, _l)(const char *nptr, char **endptr,
int base, locale_t loc)
#endif
{
const char *s;
__INT acc, cutoff;
unsigned char c;
int i, neg, any, cutlim;

_DIAGASSERT(nptr != NULL);



if (base && (base < 2 || base > 36)) {
#if !defined(_KERNEL) && !defined(_STANDALONE)
errno = EINVAL;
if (endptr != NULL)

*endptr = __UNCONST(nptr);
return 0;
#else
panic("%s: invalid base %d", __func__, base);
#endif
}






s = nptr;
#if defined(_KERNEL) || defined(_STANDALONE) || defined(HAVE_NBTOOL_CONFIG_H) || defined(BCS_ONLY)

do {
c = *s++;
} while (isspace(c));
#else
do {
c = *s++;
} while (isspace_l(c, loc));
#endif
if (c == '-') {
neg = 1;
c = *s++;
} else {
neg = 0;
if (c == '+')
c = *s++;
}
if ((base == 0 || base == 16) &&
c == '0' && (*s == 'x' || *s == 'X') &&
((s[1] >= '0' && s[1] <= '9') ||
(s[1] >= 'a' && s[1] <= 'f') ||
(s[1] >= 'A' && s[1] <= 'F'))) {
c = s[1];
s += 2;
base = 16;
#if 0
} else if ((base == 0 || base == 2) &&
c == '0' && (*s == 'b' || *s == 'B') &&
(s[1] >= '0' && s[1] <= '1')) {
c = s[1];
s += 2;
base = 2;
#endif
} else if (base == 0)
base = (c == '0' ? 8 : 10);


















cutoff = (__INT)(neg ? __INT_MIN : __INT_MAX);
cutlim = (int)(cutoff % base);
cutoff /= base;
if (neg) {
if (cutlim > 0) {
cutlim -= base;
cutoff += 1;
}
cutlim = -cutlim;
}
for (acc = 0, any = 0;; c = *s++) {
if (c >= '0' && c <= '9')
i = c - '0';
else if (c >= 'a' && c <= 'z')
i = (c - 'a') + 10;
else if (c >= 'A' && c <= 'Z')
i = (c - 'A') + 10;
else
break;
if (i >= base)
break;
if (any < 0)
continue;
if (neg) {
if (acc < cutoff || (acc == cutoff && i > cutlim)) {
acc = __INT_MIN;
#if !defined(_KERNEL) && !defined(_STANDALONE)
any = -1;
errno = ERANGE;
#else
any = 0;
break;
#endif
} else {
any = 1;
acc *= base;
acc -= i;
}
} else {
if (acc > cutoff || (acc == cutoff && i > cutlim)) {
acc = __INT_MAX;
#if !defined(_KERNEL) && !defined(_STANDALONE)
any = -1;
errno = ERANGE;
#else
any = 0;
break;
#endif
} else {
any = 1;
acc *= base;
acc += i;
}
}
}
if (endptr != NULL)

*endptr = __UNCONST(any ? s - 1 : nptr);
return(acc);
}

#if !defined(_KERNEL) && !defined(_STANDALONE) && !defined(HAVE_NBTOOL_CONFIG_H) && !defined(BCS_ONLY)

__INT
_FUNCNAME(const char *nptr, char **endptr, int base)
{
return INT_FUNCNAME(_int_, _FUNCNAME, _l)(nptr, endptr, base, _current_locale());
}

__INT
INT_FUNCNAME(, _FUNCNAME, _l)(const char *nptr, char **endptr, int base, locale_t loc)
{
return INT_FUNCNAME(_int_, _FUNCNAME, _l)(nptr, endptr, base, loc);
}
#endif
