






















#include "flexdef.h"
static const char * check_4_gnu_m4 =
"m4_dnl ifdef(`__gnu__', ,"
"`errprint(Flex requires GNU M4. Set the PATH or set the M4 environment variable to its path name.)"
" m4exit(2)')\n";



struct filter *output_chain = NULL;








struct filter *filter_create_ext (struct filter *chain, const char *cmd,
...)
{
struct filter *f;
int max_args;
const char *s;
va_list ap;


f = malloc(sizeof(struct filter));
if (!f)
flexerror(_("malloc failed (f) in filter_create_ext"));
memset (f, 0, sizeof (*f));
f->filter_func = NULL;
f->extra = NULL;
f->next = NULL;
f->argc = 0;

if (chain != NULL) {

while (chain->next)
chain = chain->next;
chain->next = f;
}



max_args = 8;
f->argv = malloc(sizeof(char *) * (size_t) (max_args + 1));
if (!f->argv)
flexerror(_("malloc failed (f->argv) in filter_create_ext"));
f->argv[f->argc++] = cmd;

va_start (ap, cmd);
while ((s = va_arg (ap, const char *)) != NULL) {
if (f->argc >= max_args) {
max_args += 8;
f->argv = realloc(f->argv, sizeof(char*) * (size_t) (max_args + 1));
}
f->argv[f->argc++] = s;
}
f->argv[f->argc] = NULL;

va_end (ap);
return f;
}









struct filter *filter_create_int (struct filter *chain,
int (*filter_func) (struct filter *),
void *extra)
{
struct filter *f;


f = malloc(sizeof(struct filter));
if (!f)
flexerror(_("malloc failed in filter_create_int"));
memset (f, 0, sizeof (*f));
f->next = NULL;
f->argc = 0;
f->argv = NULL;

f->filter_func = filter_func;
f->extra = extra;

if (chain != NULL) {

while (chain->next)
chain = chain->next;
chain->next = f;
}

return f;
}





bool filter_apply_chain (struct filter * chain)
{
int pid, pipes[2];






if (chain)
filter_apply_chain (chain->next);
else
return true;




fflush (stdout);
fflush (stderr);


if (pipe (pipes) == -1)
flexerror (_("pipe failed"));

if ((pid = fork ()) == -1)
flexerror (_("fork failed"));

if (pid == 0) {








close (pipes[1]);
clearerr(stdin);
if (dup2 (pipes[0], fileno (stdin)) == -1)
flexfatal (_("dup2(pipes[0],0)"));
close (pipes[0]);
fseek (stdin, 0, SEEK_CUR);
ungetc(' ', stdin);
(void)fgetc(stdin);


if (chain->filter_func) {
int r;

if ((r = chain->filter_func (chain)) == -1)
flexfatal (_("filter_func failed"));
FLEX_EXIT (0);
}
else {
execvp (chain->argv[0],
(char **const) (chain->argv));
lerr_fatal ( _("exec of %s failed"),
chain->argv[0]);
}

FLEX_EXIT (1);
}


close (pipes[0]);
if (dup2 (pipes[1], fileno (stdout)) == -1)
flexfatal (_("dup2(pipes[1],1)"));
close (pipes[1]);
fseek (stdout, 0, SEEK_CUR);

return true;
}






int filter_truncate (struct filter *chain, int max_len)
{
int len = 1;

if (!chain)
return 0;

while (chain->next && len < max_len) {
chain = chain->next;
++len;
}

chain->next = NULL;
return len;
}






int filter_tee_header (struct filter *chain)
{




const int readsz = 512;
char *buf;
int to_cfd = -1;
FILE *to_c = NULL, *to_h = NULL;
bool write_header;

write_header = (chain->extra != NULL);






if ((to_cfd = dup (1)) == -1)
flexfatal (_("dup(1) failed"));
to_c = fdopen (to_cfd, "w");

if (write_header) {
if (freopen ((char *) chain->extra, "w", stdout) == NULL)
flexfatal (_("freopen(headerfilename) failed"));

filter_apply_chain (chain->next);
to_h = stdout;
}




if (write_header) {
fputs (check_4_gnu_m4, to_h);
fputs ("m4_changecom`'m4_dnl\n", to_h);
fputs ("m4_changequote`'m4_dnl\n", to_h);
fputs ("m4_changequote([[,]])[[]]m4_dnl\n", to_h);
fputs ("m4_define([[M4_YY_NOOP]])[[]]m4_dnl\n", to_h);
fputs ("m4_define( [[M4_YY_IN_HEADER]],[[]])m4_dnl\n",
to_h);
fprintf (to_h, "#ifndef %sHEADER_H\n", prefix);
fprintf (to_h, "#define %sHEADER_H 1\n", prefix);
fprintf (to_h, "#define %sIN_HEADER 1\n\n", prefix);
fprintf (to_h,
"m4_define( [[M4_YY_OUTFILE_NAME]],[[%s]])m4_dnl\n",
headerfilename ? headerfilename : "<stdout>");

}

fputs (check_4_gnu_m4, to_c);
fputs ("m4_changecom`'m4_dnl\n", to_c);
fputs ("m4_changequote`'m4_dnl\n", to_c);
fputs ("m4_changequote([[,]])[[]]m4_dnl\n", to_c);
fputs ("m4_define([[M4_YY_NOOP]])[[]]m4_dnl\n", to_c);
fprintf (to_c, "m4_define( [[M4_YY_OUTFILE_NAME]],[[%s]])m4_dnl\n",
outfilename ? outfilename : "<stdout>");

buf = malloc((size_t) readsz);
if (!buf)
flexerror(_("malloc failed in filter_tee_header"));
while (fgets (buf, readsz, stdin)) {
fputs (buf, to_c);
if (write_header)
fputs (buf, to_h);
}

if (write_header) {
fprintf (to_h, "\n");


if (gen_line_dirs)
fprintf (to_h, "#line 4000 \"M4_YY_OUTFILE_NAME\"\n");

fprintf (to_h, "#undef %sIN_HEADER\n", prefix);
fprintf (to_h, "#endif /* %sHEADER_H */\n", prefix);
fputs ("m4_undefine( [[M4_YY_IN_HEADER]])m4_dnl\n", to_h);

fflush (to_h);
if (ferror (to_h))
lerr (_("error writing output file %s"),
(char *) chain->extra);

else if (fclose (to_h))
lerr (_("error closing output file %s"),
(char *) chain->extra);
}

fflush (to_c);
if (ferror (to_c))
lerr (_("error writing output file %s"),
outfilename ? outfilename : "<stdout>");

else if (fclose (to_c))
lerr (_("error closing output file %s"),
outfilename ? outfilename : "<stdout>");

while (wait (0) > 0) ;

FLEX_EXIT (0);
return 0;
}







int filter_fix_linedirs (struct filter *chain)
{
char *buf;
const size_t readsz = 512;
int lineno = 1;
bool in_gen = true;
bool last_was_blank = false;

if (!chain)
return 0;

buf = malloc(readsz);
if (!buf)
flexerror(_("malloc failed in filter_fix_linedirs"));

while (fgets (buf, (int) readsz, stdin)) {

regmatch_t m[10];


if (buf[0] == '#'
&& regexec (&regex_linedir, buf, 3, m, 0) == 0) {

char *fname;


fname = regmatch_dup (&m[2], buf);

if (strcmp (fname,
outfilename ? outfilename : "<stdout>")
== 0
|| strcmp (fname,
headerfilename ? headerfilename : "<stdout>")
== 0) {

char *s1, *s2;
char filename[MAXLINE];

s1 = fname;
s2 = filename;

while ((s2 - filename) < (MAXLINE - 1) && *s1) {

if (*s1 == '\\')
*s2++ = '\\';

if (*s1 == '\"')
*s2++ = '\\';

*s2++ = *s1++;
}

*s2 = '\0';


in_gen = true;
snprintf (buf, readsz, "#line %d \"%s\"\n",
lineno, filename);
}
else {

in_gen = false;
}

free (fname);
last_was_blank = false;
}


else if (in_gen
&& regexec (&regex_blank_line, buf, 0, NULL,
0) == 0) {
if (last_was_blank)
continue;
else
last_was_blank = true;
}

else {

last_was_blank = false;
}

fputs (buf, stdout);
lineno++;
}
fflush (stdout);
if (ferror (stdout))
lerr (_("error writing output file %s"),
outfilename ? outfilename : "<stdout>");

else if (fclose (stdout))
lerr (_("error closing output file %s"),
outfilename ? outfilename : "<stdout>");

return 0;
}


