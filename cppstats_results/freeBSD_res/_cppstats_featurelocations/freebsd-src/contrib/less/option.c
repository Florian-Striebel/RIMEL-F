


















#include "less.h"
#include "option.h"

static struct loption *pendopt;
public int plusoption = FALSE;

static char *optstring LESSPARAMS((char *s, char **p_str, char *printopt, char *validchars));
static int flip_triple LESSPARAMS((int val, int lc));

extern int screen_trashed;
extern int less_is_more;
extern int quit_at_eof;
extern char *every_first_cmd;
extern int opt_use_backslash;




static char *
opt_desc(o)
struct loption *o;
{
static char buf[OPTNAME_MAX + 10];
if (o->oletter == OLETTER_NONE)
SNPRINTF1(buf, sizeof(buf), "--%s", o->onames->oname);
else
SNPRINTF2(buf, sizeof(buf), "-%c (--%s)", o->oletter, o->onames->oname);
return (buf);
}





public char *
propt(c)
int c;
{
static char buf[8];

sprintf(buf, "-%s", prchar(c));
return (buf);
}





public void
scan_option(s)
char *s;
{
struct loption *o;
int optc;
char *optname;
char *printopt;
char *str;
int set_default;
int lc;
int err;
PARG parg;

if (s == NULL)
return;








if (pendopt != NULL)
{
switch (pendopt->otype & OTYPE)
{
case STRING:
(*pendopt->ofunc)(INIT, s);
break;
case NUMBER:
printopt = opt_desc(pendopt);
*(pendopt->ovar) = getnum(&s, printopt, (int*)NULL);
break;
}
pendopt = NULL;
return;
}

set_default = FALSE;
optname = NULL;

while (*s != '\0')
{



switch (optc = *s++)
{
case ' ':
case '\t':
case END_OPTION_STRING:
continue;
case '-':



if (*s == '-')
{
optname = ++s;
break;
}





set_default = (*s == '+');
if (set_default)
s++;
continue;
case '+':







plusoption = TRUE;
s = optstring(s, &str, propt('+'), NULL);
if (s == NULL)
return;
if (*str == '+')
{
if (every_first_cmd != NULL)
free(every_first_cmd);
every_first_cmd = save(str+1);
} else
{
ungetsc(str);
ungetcc_back(CHAR_END_COMMAND);
}
free(str);
continue;
case '0': case '1': case '2': case '3': case '4':
case '5': case '6': case '7': case '8': case '9':





s--;
optc = 'z';
break;
case 'n':
if (less_is_more)
optc = 'z';
break;
}





err = 0;
if (optname == NULL)
{
printopt = propt(optc);
lc = ASCII_IS_LOWER(optc);
o = findopt(optc);
} else
{
printopt = optname;
lc = ASCII_IS_LOWER(optname[0]);
o = findopt_name(&optname, NULL, &err);
s = optname;
optname = NULL;
if (*s == '\0' || *s == ' ')
{



;
} else if (*s == '=')
{



if (o != NULL &&
(o->otype & OTYPE) != STRING &&
(o->otype & OTYPE) != NUMBER)
{
parg.p_string = printopt;
error("The %s option should not be followed by =",
&parg);
return;
}
s++;
} else
{




o = NULL;
}
}
if (o == NULL)
{
parg.p_string = printopt;
if (err == OPT_AMBIG)
error("%s is an ambiguous abbreviation (\"less --help\" for help)",
&parg);
else
error("There is no %s option (\"less --help\" for help)",
&parg);
return;
}

str = NULL;
switch (o->otype & OTYPE)
{
case BOOL:
if (set_default)
*(o->ovar) = o->odefault;
else
*(o->ovar) = ! o->odefault;
break;
case TRIPLE:
if (set_default)
*(o->ovar) = o->odefault;
else
*(o->ovar) = flip_triple(o->odefault, lc);
break;
case STRING:
if (*s == '\0')
{





pendopt = o;
return;
}





while (*s == ' ')
s++;
s = optstring(s, &str, printopt, o->odesc[1]);
if (s == NULL)
return;
break;
case NUMBER:
if (*s == '\0')
{
pendopt = o;
return;
}
*(o->ovar) = getnum(&s, printopt, (int*)NULL);
break;
}



if (o->ofunc != NULL)
(*o->ofunc)(INIT, str);
if (str != NULL)
free(str);
}
}










public void
toggle_option(o, lower, s, how_toggle)
struct loption *o;
int lower;
char *s;
int how_toggle;
{
int num;
int no_prompt;
int err;
PARG parg;

no_prompt = (how_toggle & OPT_NO_PROMPT);
how_toggle &= ~OPT_NO_PROMPT;

if (o == NULL)
{
error("No such option", NULL_PARG);
return;
}

if (how_toggle == OPT_TOGGLE && (o->otype & NO_TOGGLE))
{
parg.p_string = opt_desc(o);
error("Cannot change the %s option", &parg);
return;
}

if (how_toggle == OPT_NO_TOGGLE && (o->otype & NO_QUERY))
{
parg.p_string = opt_desc(o);
error("Cannot query the %s option", &parg);
return;
}







switch (o->otype & OTYPE)
{
case STRING:
case NUMBER:
if (how_toggle == OPT_TOGGLE && *s == '\0')
how_toggle = OPT_NO_TOGGLE;
break;
}

#if HILITE_SEARCH
if (how_toggle != OPT_NO_TOGGLE && (o->otype & HL_REPAINT))
repaint_hilite(0);
#endif




if (how_toggle != OPT_NO_TOGGLE)
{
switch (o->otype & OTYPE)
{
case BOOL:



switch (how_toggle)
{
case OPT_TOGGLE:
*(o->ovar) = ! *(o->ovar);
break;
case OPT_UNSET:
*(o->ovar) = o->odefault;
break;
case OPT_SET:
*(o->ovar) = ! o->odefault;
break;
}
break;
case TRIPLE:







switch (how_toggle)
{
case OPT_TOGGLE:
*(o->ovar) = flip_triple(*(o->ovar), lower);
break;
case OPT_UNSET:
*(o->ovar) = o->odefault;
break;
case OPT_SET:
*(o->ovar) = flip_triple(o->odefault, lower);
break;
}
break;
case STRING:




switch (how_toggle)
{
case OPT_SET:
case OPT_UNSET:
error("Cannot use \"-+\" or \"--\" for a string option",
NULL_PARG);
return;
}
break;
case NUMBER:



switch (how_toggle)
{
case OPT_TOGGLE:
num = getnum(&s, NULL, &err);
if (!err)
*(o->ovar) = num;
break;
case OPT_UNSET:
*(o->ovar) = o->odefault;
break;
case OPT_SET:
error("Can't use \"-!\" for a numeric option",
NULL_PARG);
return;
}
break;
}
}





if (o->ofunc != NULL)
(*o->ofunc)((how_toggle==OPT_NO_TOGGLE) ? QUERY : TOGGLE, s);

#if HILITE_SEARCH
if (how_toggle != OPT_NO_TOGGLE && (o->otype & HL_REPAINT))
chg_hilite();
#endif

if (!no_prompt)
{



switch (o->otype & OTYPE)
{
case BOOL:
case TRIPLE:



error(o->odesc[*(o->ovar)], NULL_PARG);
break;
case NUMBER:




parg.p_int = *(o->ovar);
error(o->odesc[1], &parg);
break;
case STRING:



break;
}
}

if (how_toggle != OPT_NO_TOGGLE && (o->otype & REPAINT))
screen_trashed = TRUE;
}




static int
flip_triple(val, lc)
int val;
int lc;
{
if (lc)
return ((val == OPT_ON) ? OPT_OFF : OPT_ON);
else
return ((val == OPT_ONPLUS) ? OPT_OFF : OPT_ONPLUS);
}




public int
opt_has_param(o)
struct loption *o;
{
if (o == NULL)
return (0);
if (o->otype & (BOOL|TRIPLE|NOVAR|NO_TOGGLE))
return (0);
return (1);
}





public char *
opt_prompt(o)
struct loption *o;
{
if (o == NULL || (o->otype & (STRING|NUMBER)) == 0)
return ("?");
return (o->odesc[0]);
}





public char *
opt_toggle_disallowed(c)
int c;
{
switch (c)
{
case 'o':
if (ch_getflags() & CH_CANSEEK)
return "Input is not a pipe";
break;
}
return NULL;
}








public int
isoptpending(VOID_PARAM)
{
return (pendopt != NULL);
}




static void
nostring(printopt)
char *printopt;
{
PARG parg;
parg.p_string = printopt;
error("Value is required after %s", &parg);
}




public void
nopendopt(VOID_PARAM)
{
nostring(opt_desc(pendopt));
}






static char *
optstring(s, p_str, printopt, validchars)
char *s;
char **p_str;
char *printopt;
char *validchars;
{
char *p;
char *out;

if (*s == '\0')
{
nostring(printopt);
return (NULL);
}

*p_str = (char *) ecalloc(strlen(s)+1, sizeof(char));
out = *p_str;

for (p = s; *p != '\0'; p++)
{
if (opt_use_backslash && *p == '\\' && p[1] != '\0')
{

++p;
} else
{
if (*p == END_OPTION_STRING ||
(validchars != NULL && strchr(validchars, *p) == NULL))

break;
}
*out++ = *p;
}
*out = '\0';
return (p);
}



static int
num_error(printopt, errp)
char *printopt;
int *errp;
{
PARG parg;

if (errp != NULL)
{
*errp = TRUE;
return (-1);
}
if (printopt != NULL)
{
parg.p_string = printopt;
error("Number is required after %s", &parg);
}
return (-1);
}






public int
getnum(sp, printopt, errp)
char **sp;
char *printopt;
int *errp;
{
char *s;
int n;
int neg;

s = skipsp(*sp);
neg = FALSE;
if (*s == '-')
{
neg = TRUE;
s++;
}
if (*s < '0' || *s > '9')
return (num_error(printopt, errp));

n = 0;
while (*s >= '0' && *s <= '9')
n = 10 * n + *s++ - '0';
*sp = s;
if (errp != NULL)
*errp = FALSE;
if (neg)
n = -n;
return (n);
}







public long
getfraction(sp, printopt, errp)
char **sp;
char *printopt;
int *errp;
{
char *s;
long frac = 0;
int fraclen = 0;

s = skipsp(*sp);
if (*s < '0' || *s > '9')
return (num_error(printopt, errp));

for ( ; *s >= '0' && *s <= '9'; s++)
{
frac = (frac * 10) + (*s - '0');
fraclen++;
}
if (fraclen > NUM_LOG_FRAC_DENOM)
while (fraclen-- > NUM_LOG_FRAC_DENOM)
frac /= 10;
else
while (fraclen++ < NUM_LOG_FRAC_DENOM)
frac *= 10;
*sp = s;
if (errp != NULL)
*errp = FALSE;
return (frac);
}





public int
get_quit_at_eof(VOID_PARAM)
{
if (!less_is_more)
return quit_at_eof;

return quit_at_eof ? OPT_ONPLUS : OPT_ON;
}