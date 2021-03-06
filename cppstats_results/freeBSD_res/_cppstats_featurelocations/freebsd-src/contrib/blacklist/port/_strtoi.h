













































__TYPE
_FUNCNAME(const char * __restrict nptr, char ** __restrict endptr, int base,
__TYPE lo, __TYPE hi, int * rstatus)
{
int serrno;
__TYPE im;
char *ep;
int rep;



if (endptr == NULL)
endptr = &ep;

if (rstatus == NULL)
rstatus = &rep;

serrno = errno;
errno = 0;

im = __WRAPPED(nptr, endptr, base);

*rstatus = errno;
errno = serrno;

if (*rstatus == 0) {

if (nptr == *endptr)
*rstatus = ECANCELED;

else if (**endptr != '\0')
*rstatus = ENOTSUP;
}

if (im < lo) {
if (*rstatus == 0)
*rstatus = ERANGE;
return lo;
}
if (im > hi) {
if (*rstatus == 0)
*rstatus = ERANGE;
return hi;
}

return im;
}
