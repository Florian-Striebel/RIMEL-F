


















































#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdint.h>
#include <ctype.h>
#include <stdlib.h>
#include <limits.h>

#include "xo.h"
#include "xo_encoder.h"
#include "xo_buf.h"

#if !defined(UNUSED)
#define UNUSED __attribute__ ((__unused__))
#endif























typedef struct path_frame_s {
char *pf_name;
uint32_t pf_flags;
} path_frame_t;

typedef struct stack_frame_s {
ssize_t sf_off;
uint32_t sf_flags;
} stack_frame_t;



typedef struct leaf_s {
ssize_t f_name;
ssize_t f_value;
uint32_t f_flags;
#if defined(CSV_STACK_IS_NEEDED)
ssize_t f_depth;
#endif
} leaf_t;


#define LF_KEY (1<<0)
#define LF_HAS_VALUE (1<<1)

typedef struct csv_private_s {
uint32_t c_flags;


char *c_path_buf;
path_frame_t *c_path;
ssize_t c_path_max;
ssize_t c_path_cur;


#if CSV_STACK_IS_NEEDED
xo_buffer_t c_stack_buf;
stack_frame_t *c_stack;
ssize_t c_stack_max;
#endif
ssize_t c_stack_depth;


xo_buffer_t c_name_buf;
xo_buffer_t c_value_buf;
leaf_t *c_leaf;
ssize_t c_leaf_depth;
ssize_t c_leaf_max;

xo_buffer_t c_data;
} csv_private_t;

#define C_STACK_MAX 32
#define C_LEAF_MAX 32


#define CF_HEADER_DONE (1<<0)
#define CF_NO_HEADER (1<<1)
#define CF_NO_KEYS (1<<2)
#define CF_VALUE_ONLY (1<<3)

#define CF_DOS_NEWLINE (1<<4)
#define CF_LEAFS_DONE (1<<5)
#define CF_NO_QUOTES (1<<6)
#define CF_RECORD_DATA (1<<7)

#define CF_DEBUG (1<<8)
#define CF_HAS_PATH (1<<9)





static void
csv_dbg (xo_handle_t *xop UNUSED, csv_private_t *csv UNUSED,
const char *fmt, ...)
{
if (csv == NULL || !(csv->c_flags & CF_DEBUG))
return;

va_list vap;

va_start(vap, fmt);
vfprintf(stderr, fmt, vap);
va_end(vap);
}





static int
csv_create (xo_handle_t *xop)
{
csv_private_t *csv = xo_realloc(NULL, sizeof(*csv));
if (csv == NULL)
return -1;

bzero(csv, sizeof(*csv));
xo_buf_init(&csv->c_data);
xo_buf_init(&csv->c_name_buf);
xo_buf_init(&csv->c_value_buf);
#if defined(CSV_STACK_IS_NEEDED)
xo_buf_init(&csv->c_stack_buf);
#endif

xo_set_private(xop, csv);

return 0;
}




static void
csv_destroy (xo_handle_t *xop UNUSED, csv_private_t *csv)
{

xo_buf_cleanup(&csv->c_data);
xo_buf_cleanup(&csv->c_name_buf);
xo_buf_cleanup(&csv->c_value_buf);
#if defined(CSV_STACK_IS_NEEDED)
xo_buf_cleanup(&csv->c_stack_buf);
#endif

if (csv->c_leaf)
xo_free(csv->c_leaf);
if (csv->c_path_buf)
xo_free(csv->c_path_buf);
}





static const char *
csv_path_top (csv_private_t *csv, ssize_t delta)
{
if (!(csv->c_flags & CF_HAS_PATH) || csv->c_path == NULL)
return NULL;

ssize_t cur = csv->c_path_cur + delta;

if (cur < 0)
return NULL;

return csv->c_path[cur].pf_name;
}




static inline void
csv_stack_push (csv_private_t *csv UNUSED, const char *name UNUSED)
{
#if defined(CSV_STACK_IS_NEEDED)
csv->c_stack_depth += 1;
#endif
}




static inline void
csv_stack_pop (csv_private_t *csv UNUSED, const char *name UNUSED)
{
#if defined(CSV_STACK_IS_NEEDED)
csv->c_stack_depth -= 1;
#endif
}


#define QF_NEEDS_QUOTES (1<<0)
#define QF_NEEDS_ESCAPE (1<<1)






static uint32_t
csv_quote_flags (xo_handle_t *xop UNUSED, csv_private_t *csv UNUSED,
const char *value)
{
static const char quoted[] = "\n\r\",";
static const char escaped[] = "\"";

if (csv->c_flags & CF_NO_QUOTES)
return 0;

size_t len = strlen(value);
uint32_t rc = 0;

if (strcspn(value, quoted) != len)
rc |= QF_NEEDS_QUOTES;
else if (isspace((int) value[0]))
rc |= QF_NEEDS_QUOTES;
else if (isspace((int) value[len - 1]))
rc |= QF_NEEDS_QUOTES;

if (strcspn(value, escaped) != len)
rc |= QF_NEEDS_ESCAPE;

csv_dbg(xop, csv, "csv: quote flags [%s] -> %x (%zu/%zu)\n",
value, rc, len, strcspn(value, quoted));

return rc;
}




static void
csv_escape (xo_buffer_t *xbp, const char *value, size_t len)
{
const char *cp, *ep, *np;

for (cp = value, ep = value + len; cp && cp < ep; cp = np) {
np = strchr(cp, '"');
if (np) {
np += 1;
xo_buf_append(xbp, cp, np - cp);
xo_buf_append(xbp, "\"", 1);
} else
xo_buf_append(xbp, cp, ep - cp);
}
}





static void
csv_append_newline (xo_buffer_t *xbp, csv_private_t *csv)
{
if (csv->c_flags & CF_DOS_NEWLINE)
xo_buf_append(xbp, "\r\n", 2);
else
xo_buf_append(xbp, "\n", 1);
}






static void
csv_emit_record (xo_handle_t *xop, csv_private_t *csv)
{
csv_dbg(xop, csv, "csv: emit: ...\n");

ssize_t fnum;
uint32_t quote_flags;
leaf_t *lp;


if (csv->c_leaf_depth == 0)
return;

if (!(csv->c_flags & (CF_HEADER_DONE | CF_NO_HEADER))) {
csv->c_flags |= CF_HEADER_DONE;

for (fnum = 0; fnum < csv->c_leaf_depth; fnum++) {
lp = &csv->c_leaf[fnum];
const char *name = xo_buf_data(&csv->c_name_buf, lp->f_name);

if (fnum != 0)
xo_buf_append(&csv->c_data, ",", 1);

xo_buf_append(&csv->c_data, name, strlen(name));
}

csv_append_newline(&csv->c_data, csv);
}

for (fnum = 0; fnum < csv->c_leaf_depth; fnum++) {
lp = &csv->c_leaf[fnum];
const char *value;

if (lp->f_flags & LF_HAS_VALUE) {
value = xo_buf_data(&csv->c_value_buf, lp->f_value);
} else {
value = "";
}

quote_flags = csv_quote_flags(xop, csv, value);

if (fnum != 0)
xo_buf_append(&csv->c_data, ",", 1);

if (quote_flags & QF_NEEDS_QUOTES)
xo_buf_append(&csv->c_data, "\"", 1);

if (quote_flags & QF_NEEDS_ESCAPE)
csv_escape(&csv->c_data, value, strlen(value));
else
xo_buf_append(&csv->c_data, value, strlen(value));

if (quote_flags & QF_NEEDS_QUOTES)
xo_buf_append(&csv->c_data, "\"", 1);
}

csv_append_newline(&csv->c_data, csv);


if (xo_get_flags(xop) & (XOF_FLUSH | XOF_FLUSH_LINE))
xo_flush_h(xop);


for (fnum = 0; fnum < csv->c_leaf_depth; fnum++) {
lp = &csv->c_leaf[fnum];

lp->f_flags &= ~LF_HAS_VALUE;
lp->f_value = 0;
}

xo_buf_reset(&csv->c_value_buf);





csv->c_flags |= CF_LEAFS_DONE;
}






static int
csv_open_level (xo_handle_t *xop UNUSED, csv_private_t *csv,
const char *name, int instance)
{

if (csv->c_flags & CF_RECORD_DATA) {
csv->c_flags &= ~CF_RECORD_DATA;
csv_emit_record(xop, csv);
return 0;
}

const char *path_top = csv_path_top(csv, 0);


if (path_top == NULL) {
if (instance && !(csv->c_flags & CF_HAS_PATH)) {
csv_dbg(xop, csv, "csv: recording (no-path) ...\n");
csv->c_flags |= CF_RECORD_DATA;
}

} else if (xo_streq(path_top, name)) {
csv->c_path_cur += 1;

csv_dbg(xop, csv, "csv: match: [%s] (%zd/%zd)\n", name,
csv->c_path_cur, csv->c_path_max);


if (csv->c_path_cur == csv->c_path_max) {
csv_dbg(xop, csv, "csv: recording ...\n");
csv->c_flags |= CF_RECORD_DATA;
}
}


csv_stack_push(csv, name);

return 0;
}




static int
csv_close_level (xo_handle_t *xop UNUSED, csv_private_t *csv, const char *name)
{

if (csv->c_flags & CF_RECORD_DATA) {
csv->c_flags &= ~CF_RECORD_DATA;
csv_emit_record(xop, csv);
}

const char *path_top = csv_path_top(csv, -1);
csv_dbg(xop, csv, "csv: close: [%s] [%s] (%zd)\n", name,
path_top ?: "", csv->c_path_cur);


if (path_top != NULL && xo_streq(path_top, name)) {
csv->c_path_cur -= 1;
return 0;
}


csv_stack_pop(csv, name);

return 0;
}






static int
csv_leaf_num (xo_handle_t *xop UNUSED, csv_private_t *csv,
const char *name, xo_xff_flags_t flags)
{
ssize_t fnum;
leaf_t *lp;
xo_buffer_t *xbp = &csv->c_name_buf;

for (fnum = 0; fnum < csv->c_leaf_depth; fnum++) {
lp = &csv->c_leaf[fnum];

const char *fname = xo_buf_data(xbp, lp->f_name);
if (xo_streq(fname, name))
return fnum;
}


if (csv->c_flags & CF_LEAFS_DONE)
return -1;



if (csv->c_leaf_depth + 1 >= csv->c_leaf_max) {

ssize_t new_max = csv->c_leaf_max * 2;
if (new_max == 0)
new_max = C_LEAF_MAX;

lp = xo_realloc(csv->c_leaf, new_max * sizeof(*lp));
if (lp == NULL)
return -1;


bzero(&lp[csv->c_leaf_max], csv->c_leaf_max * sizeof(*lp));


csv->c_leaf = lp;
csv->c_leaf_max = new_max;
}

lp = &csv->c_leaf[csv->c_leaf_depth++];
#if defined(CSV_STACK_IS_NEEDED)
lp->f_depth = csv->c_stack_depth;
#endif

lp->f_name = xo_buf_offset(xbp);

char *cp = xo_buf_cur(xbp);
xo_buf_append(xbp, name, strlen(name) + 1);

if (flags & XFF_KEY)
lp->f_flags |= LF_KEY;

csv_dbg(xop, csv, "csv: leaf: name: %zd [%s] [%s] %x\n",
fnum, name, cp, lp->f_flags);

return fnum;
}




static void
csv_leaf_set (xo_handle_t *xop UNUSED, csv_private_t *csv, leaf_t *lp,
const char *value)
{
xo_buffer_t *xbp = &csv->c_value_buf;

lp->f_value = xo_buf_offset(xbp);
lp->f_flags |= LF_HAS_VALUE;

char *cp = xo_buf_cur(xbp);
xo_buf_append(xbp, value, strlen(value) + 1);

csv_dbg(xop, csv, "csv: leaf: value: [%s] [%s] %x\n",
value, cp, lp->f_flags);
}





static int
csv_record_leafs (xo_handle_t *xop, csv_private_t *csv, const char *leafs_raw)
{
char *cp, *ep, *np;
ssize_t len = strlen(leafs_raw);
char *leafs_buf = alloca(len + 1);

memcpy(leafs_buf, leafs_raw, len + 1);

for (cp = leafs_buf, ep = leafs_buf + len; cp && cp < ep; cp = np) {
np = strchr(cp, '.');
if (np)
*np++ = '\0';

if (*cp == '\0')
continue;

csv_dbg(xop, csv, "adding leaf: [%s]\n", cp);
csv_leaf_num(xop, csv, cp, 0);
}




csv->c_flags |= CF_LEAFS_DONE;

return 0;
}





static int
csv_record_path (xo_handle_t *xop, csv_private_t *csv, const char *path_raw)
{
int count;
char *cp, *ep, *np;
ssize_t len = strlen(path_raw);
char *path_buf = xo_realloc(NULL, len + 1);

memcpy(path_buf, path_raw, len + 1);

for (cp = path_buf, ep = path_buf + len, count = 2;
cp && cp < ep; cp = np) {
np = strchr(cp, '/');
if (np) {
np += 1;
count += 1;
}
}

path_frame_t *path = xo_realloc(NULL, sizeof(path[0]) * count);
if (path == NULL) {
xo_failure(xop, "allocation failure for path '%s'", path_buf);
return -1;
}

bzero(path, sizeof(path[0]) * count);

for (count = 0, cp = path_buf; cp && cp < ep; cp = np) {
path[count++].pf_name = cp;

np = strchr(cp, '/');
if (np)
*np++ = '\0';
csv_dbg(xop, csv, "path: [%s]\n", cp);
}

path[count].pf_name = NULL;

if (csv->c_path)
xo_free(csv->c_path);
if (csv->c_path_buf)
xo_free(csv->c_path_buf);

csv->c_path_buf = path_buf;
csv->c_path = path;
csv->c_path_max = count;
csv->c_path_cur = 0;

return 0;
}






static int
csv_options (xo_handle_t *xop, csv_private_t *csv,
const char *raw_opts, char opts_char)
{
ssize_t len = strlen(raw_opts);
char *options = alloca(len + 1);
memcpy(options, raw_opts, len);
options[len] = '\0';

char *cp, *ep, *np, *vp;
for (cp = options, ep = options + len + 1; cp && cp < ep; cp = np) {
np = strchr(cp, opts_char);
if (np)
*np++ = '\0';

vp = strchr(cp, '=');
if (vp)
*vp++ = '\0';

if (xo_streq(cp, "path")) {

if (vp != NULL && csv_record_path(xop, csv, vp))
return -1;

csv->c_flags |= CF_HAS_PATH;

} else if (xo_streq(cp, "leafs")
|| xo_streq(cp, "leaf")
|| xo_streq(cp, "leaves")) {

if (vp != NULL && csv_record_leafs(xop, csv, vp))
return -1;

} else if (xo_streq(cp, "no-keys")) {
csv->c_flags |= CF_NO_KEYS;
} else if (xo_streq(cp, "no-header")) {
csv->c_flags |= CF_NO_HEADER;
} else if (xo_streq(cp, "value-only")) {
csv->c_flags |= CF_VALUE_ONLY;
} else if (xo_streq(cp, "dos")) {
csv->c_flags |= CF_DOS_NEWLINE;
} else if (xo_streq(cp, "no-quotes")) {
csv->c_flags |= CF_NO_QUOTES;
} else if (xo_streq(cp, "debug")) {
csv->c_flags |= CF_DEBUG;
} else {
xo_warn_hc(xop, -1,
"unknown encoder option value: '%s'", cp);
return -1;
}
}

return 0;
}





static int
csv_data (xo_handle_t *xop UNUSED, csv_private_t *csv UNUSED,
const char *name, const char *value,
xo_xof_flags_t flags)
{
csv_dbg(xop, csv, "data: [%s]=[%s] %llx\n", name, value, (unsigned long long) flags);

if (!(csv->c_flags & CF_RECORD_DATA))
return 0;


int fnum = csv_leaf_num(xop, csv, name, flags);
if (fnum < 0)
return 0;

leaf_t *lp = &csv->c_leaf[fnum];
csv_leaf_set(xop, csv, lp, value);

return 0;
}





static int
csv_handler (XO_ENCODER_HANDLER_ARGS)
{
int rc = 0;
csv_private_t *csv = private;
xo_buffer_t *xbp = csv ? &csv->c_data : NULL;

csv_dbg(xop, csv, "op %s: [%s] [%s]\n", xo_encoder_op_name(op),
name ?: "", value ?: "");
fflush(stdout);


if (csv == NULL && op != XO_OP_CREATE)
return -1;

switch (op) {
case XO_OP_CREATE:
rc = csv_create(xop);
break;

case XO_OP_OPTIONS:
rc = csv_options(xop, csv, value, ':');
break;

case XO_OP_OPTIONS_PLUS:
rc = csv_options(xop, csv, value, '+');
break;

case XO_OP_OPEN_LIST:
case XO_OP_CLOSE_LIST:
break;

case XO_OP_OPEN_CONTAINER:
case XO_OP_OPEN_LEAF_LIST:
rc = csv_open_level(xop, csv, name, 0);
break;

case XO_OP_OPEN_INSTANCE:
rc = csv_open_level(xop, csv, name, 1);
break;

case XO_OP_CLOSE_CONTAINER:
case XO_OP_CLOSE_LEAF_LIST:
case XO_OP_CLOSE_INSTANCE:
rc = csv_close_level(xop, csv, name);
break;

case XO_OP_STRING:
case XO_OP_CONTENT:
rc = csv_data(xop, csv, name, value, flags);
break;

case XO_OP_FINISH:
break;

case XO_OP_FLUSH:
rc = write(1, xbp->xb_bufp, xbp->xb_curp - xbp->xb_bufp);
if (rc > 0)
rc = 0;

xo_buf_reset(xbp);
break;

case XO_OP_DESTROY:
csv_destroy(xop, csv);
break;

case XO_OP_ATTRIBUTE:
break;

case XO_OP_VERSION:
break;
}

return rc;
}




int
xo_encoder_library_init (XO_ENCODER_INIT_ARGS)
{
arg->xei_handler = csv_handler;
arg->xei_version = XO_ENCODER_VERSION;

return 0;
}
