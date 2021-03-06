






















#include "flexdef.h"


static const char* REGEXP_LINEDIR = "^#line ([[:digit:]]+) \"(.*)\"";
static const char* REGEXP_BLANK_LINE = "^[[:space:]]*$";

regex_t regex_linedir;
regex_t regex_blank_line;





bool flex_init_regex(void)
{
flex_regcomp(&regex_linedir, REGEXP_LINEDIR, REG_EXTENDED);
flex_regcomp(&regex_blank_line, REGEXP_BLANK_LINE, REG_EXTENDED);

return true;
}






void flex_regcomp(regex_t *preg, const char *regex, int cflags)
{
int err;

memset (preg, 0, sizeof (regex_t));

if ((err = regcomp (preg, regex, cflags)) != 0) {
const size_t errbuf_sz = 200;
char *errbuf;
int n;

errbuf = malloc(errbuf_sz * sizeof(char));
if (!errbuf)
flexfatal(_("Unable to allocate buffer to report regcomp"));
n = snprintf(errbuf, errbuf_sz, "regcomp for \"%s\" failed: ", regex);
regerror(err, preg, errbuf+n, errbuf_sz-(size_t)n);

flexfatal (errbuf);
}
}






char *regmatch_dup (regmatch_t * m, const char *src)
{
char *str;
size_t len;

if (m == NULL || m->rm_so < 0 || m->rm_eo < m->rm_so)
return NULL;
len = (size_t) (m->rm_eo - m->rm_so);
str = malloc((len + 1) * sizeof(char));
if (!str)
flexfatal(_("Unable to allocate a copy of the match"));
strncpy (str, src + m->rm_so, len);
str[len] = 0;
return str;
}







char *regmatch_cpy (regmatch_t * m, char *dest, const char *src)
{
if (m == NULL || m->rm_so < 0) {
if (dest)
dest[0] = '\0';
return dest;
}

snprintf (dest, (size_t) regmatch_len(m), "%s", src + m->rm_so);
return dest;
}





int regmatch_len (regmatch_t * m)
{
if (m == NULL || m->rm_so < 0) {
return 0;
}

return m->rm_eo - m->rm_so;
}










int regmatch_strtol (regmatch_t * m, const char *src, char **endptr,
int base)
{
int n = 0;

#define bufsz 20
char buf[bufsz];
char *s;

if (m == NULL || m->rm_so < 0)
return 0;

if (regmatch_len (m) < bufsz)
s = regmatch_cpy (m, buf, src);
else
s = regmatch_dup (m, src);

n = (int) strtol (s, endptr, base);

if (s != buf)
free (s);

return n;
}






bool regmatch_empty (regmatch_t * m)
{
return (m == NULL || m->rm_so < 0 || m->rm_so == m->rm_eo);
}


