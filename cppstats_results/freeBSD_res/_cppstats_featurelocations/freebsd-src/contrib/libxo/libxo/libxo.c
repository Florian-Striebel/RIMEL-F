




























#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <stddef.h>
#include <wchar.h>
#include <locale.h>
#include <sys/types.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <ctype.h>
#include <wctype.h>
#include <getopt.h>

#include "xo_config.h"
#include "xo.h"
#include "xo_encoder.h"
#include "xo_buf.h"
#include "xo_explicit.h"























#if defined(LIBXO_WCWIDTH)
#include "xo_wcwidth.h"
#else
#define xo_wcwidth(_x) wcwidth(_x)
#endif

#if defined(HAVE_STDIO_EXT_H)
#include <stdio_ext.h>
#endif





#if defined(HAVE_HUMANIZE_NUMBER)
#include <libutil.h>
#define xo_humanize_number humanize_number
#else
#include "xo_humanize.h"
#endif

#if defined(HAVE_GETTEXT)
#include <libintl.h>
#endif


#if !defined(FALSE)
#define FALSE 0
#endif
#if !defined(TRUE)
#define TRUE 1
#endif









#define THREAD_LOCAL_before 1
#define THREAD_LOCAL_after 2
#define THREAD_LOCAL_declspec 3

#if !defined(HAVE_THREAD_LOCAL)
#define THREAD_LOCAL(_x) _x
#elif HAVE_THREAD_LOCAL == THREAD_LOCAL_before
#define THREAD_LOCAL(_x) __thread _x
#elif HAVE_THREAD_LOCAL == THREAD_LOCAL_after
#define THREAD_LOCAL(_x) _x __thread
#elif HAVE_THREAD_LOCAL == THREAD_LOCAL_declspec
#define THREAD_LOCAL(_x) __declspec(_x)
#else
#error unknown thread-local setting
#endif

const char xo_version[] = LIBXO_VERSION;
const char xo_version_extra[] = LIBXO_VERSION_EXTRA;
static const char xo_default_format[] = "%s";

#if !defined(UNUSED)
#define UNUSED __attribute__ ((__unused__))
#endif

#define XO_INDENT_BY 2
#define XO_DEPTH 128
#define XO_MAX_ANCHOR_WIDTH (8*1024)

#define XO_FAILURE_NAME "failure"


typedef unsigned xo_xsf_flags_t;
#define XSF_NOT_FIRST (1<<0)
#define XSF_LIST (1<<1)
#define XSF_INSTANCE (1<<2)
#define XSF_DTRT (1<<3)

#define XSF_CONTENT (1<<4)
#define XSF_EMIT (1<<5)
#define XSF_EMIT_KEY (1<<6)
#define XSF_EMIT_LEAF_LIST (1<<7)


#define XSF_MARKER_FLAGS (XSF_NOT_FIRST | XSF_CONTENT | XSF_EMIT | XSF_EMIT_KEY | XSF_EMIT_LEAF_LIST )






#define XSS_TRANSITION(_old, _new) ((_old) << 8 | (_new))






typedef struct xo_stack_s {
xo_xsf_flags_t xs_flags;
xo_state_t xs_state;
char *xs_name;
char *xs_keys;
} xo_stack_t;






typedef uint8_t xo_color_t;
#define XO_COL_DEFAULT 0
#define XO_COL_BLACK 1
#define XO_COL_RED 2
#define XO_COL_GREEN 3
#define XO_COL_YELLOW 4
#define XO_COL_BLUE 5
#define XO_COL_MAGENTA 6
#define XO_COL_CYAN 7
#define XO_COL_WHITE 8

#define XO_NUM_COLORS 9















#define XO_EFF_RESET (1<<0)
#define XO_EFF_NORMAL (1<<1)
#define XO_EFF_BOLD (1<<2)
#define XO_EFF_UNDERLINE (1<<3)
#define XO_EFF_INVERSE (1<<4)

#define XO_EFF_CLEAR_BITS XO_EFF_RESET

typedef uint8_t xo_effect_t;
typedef struct xo_colors_s {
xo_effect_t xoc_effects;
xo_color_t xoc_col_fg;
xo_color_t xoc_col_bg;
} xo_colors_t;






struct xo_handle_s {
xo_xof_flags_t xo_flags;
xo_xof_flags_t xo_iflags;
xo_style_t xo_style;
unsigned short xo_indent;
unsigned short xo_indent_by;
xo_write_func_t xo_write;
xo_close_func_t xo_close;
xo_flush_func_t xo_flush;
xo_formatter_t xo_formatter;
xo_checkpointer_t xo_checkpointer;
void *xo_opaque;
xo_buffer_t xo_data;
xo_buffer_t xo_fmt;
xo_buffer_t xo_attrs;
xo_buffer_t xo_predicate;
xo_stack_t *xo_stack;
int xo_depth;
int xo_stack_size;
xo_info_t *xo_info;
int xo_info_count;
va_list xo_vap;
char *xo_leading_xpath;
mbstate_t xo_mbstate;
ssize_t xo_anchor_offset;
ssize_t xo_anchor_columns;
ssize_t xo_anchor_min_width;
ssize_t xo_units_offset;
ssize_t xo_columns;
#if !defined(LIBXO_TEXT_ONLY)
xo_color_t xo_color_map_fg[XO_NUM_COLORS];
xo_color_t xo_color_map_bg[XO_NUM_COLORS];
#endif
xo_colors_t xo_colors;
xo_buffer_t xo_color_buf;
char *xo_version;
int xo_errno;
char *xo_gt_domain;
xo_encoder_func_t xo_encoder;
void *xo_private;
};


#define XOF_BIT_ISSET(_flag, _bit) (((_flag) & (_bit)) ? 1 : 0)
#define XOF_BIT_SET(_flag, _bit) do { (_flag) |= (_bit); } while (0)
#define XOF_BIT_CLEAR(_flag, _bit) do { (_flag) &= ~(_bit); } while (0)

#define XOF_ISSET(_xop, _bit) XOF_BIT_ISSET(_xop->xo_flags, _bit)
#define XOF_SET(_xop, _bit) XOF_BIT_SET(_xop->xo_flags, _bit)
#define XOF_CLEAR(_xop, _bit) XOF_BIT_CLEAR(_xop->xo_flags, _bit)

#define XOIF_ISSET(_xop, _bit) XOF_BIT_ISSET(_xop->xo_iflags, _bit)
#define XOIF_SET(_xop, _bit) XOF_BIT_SET(_xop->xo_iflags, _bit)
#define XOIF_CLEAR(_xop, _bit) XOF_BIT_CLEAR(_xop->xo_iflags, _bit)


#define XOIF_REORDER XOF_BIT(0)
#define XOIF_DIV_OPEN XOF_BIT(1)
#define XOIF_TOP_EMITTED XOF_BIT(2)
#define XOIF_ANCHOR XOF_BIT(3)

#define XOIF_UNITS_PENDING XOF_BIT(4)
#define XOIF_INIT_IN_PROGRESS XOF_BIT(5)
#define XOIF_MADE_OUTPUT XOF_BIT(6)


































#define XF_WIDTH_MIN 0
#define XF_WIDTH_SIZE 1
#define XF_WIDTH_MAX 2
#define XF_WIDTH_NUM 3


#define XF_ENC_WIDE 1
#define XF_ENC_UTF8 2
#define XF_ENC_LOCALE 3




typedef struct xo_format_s {
unsigned char xf_fc;
unsigned char xf_enc;
unsigned char xf_skip;
unsigned char xf_lflag;
unsigned char xf_hflag;;
unsigned char xf_jflag;
unsigned char xf_tflag;
unsigned char xf_zflag;
unsigned char xf_qflag;
unsigned char xf_seen_minus;
int xf_leading_zero;
unsigned xf_dots;
int xf_width[XF_WIDTH_NUM];
unsigned xf_stars;
unsigned char xf_star[XF_WIDTH_NUM];
} xo_format_t;










typedef struct xo_field_info_s {
xo_xff_flags_t xfi_flags;
unsigned xfi_ftype;
const char *xfi_start;
const char *xfi_content;
const char *xfi_format;
const char *xfi_encoding;
const char *xfi_next;
ssize_t xfi_len;
ssize_t xfi_clen;
ssize_t xfi_flen;
ssize_t xfi_elen;
unsigned xfi_fnum;
unsigned xfi_renum;
} xo_field_info_t;








static THREAD_LOCAL(xo_handle_t) xo_default_handle;
static THREAD_LOCAL(int) xo_default_inited;
static int xo_locale_inited;
static const char *xo_program;





xo_realloc_func_t xo_realloc = realloc;
xo_free_func_t xo_free = free;


static ssize_t
xo_transition (xo_handle_t *xop, xo_xof_flags_t flags, const char *name,
xo_state_t new_state);

static int
xo_set_options_simple (xo_handle_t *xop, const char *input);

static int
xo_color_find (const char *str);

static void
xo_buf_append_div (xo_handle_t *xop, const char *class, xo_xff_flags_t flags,
const char *name, ssize_t nlen,
const char *value, ssize_t vlen,
const char *fmt, ssize_t flen,
const char *encoding, ssize_t elen);

static void
xo_anchor_clear (xo_handle_t *xop);









static inline xo_style_t
xo_style (xo_handle_t *xop UNUSED)
{
#if defined(LIBXO_TEXT_ONLY)
return XO_STYLE_TEXT;
#else
return xop->xo_style;
#endif
}





static inline int
xo_text_only (void)
{
#if defined(LIBXO_TEXT_ONLY)
return TRUE;
#else
return FALSE;
#endif
}




static xo_ssize_t
xo_write_to_file (void *opaque, const char *data)
{
FILE *fp = (FILE *) opaque;

return fprintf(fp, "%s", data);
}




static void
xo_close_file (void *opaque)
{
FILE *fp = (FILE *) opaque;

fclose(fp);
}




static int
xo_flush_file (void *opaque)
{
FILE *fp = (FILE *) opaque;

return fflush(fp);
}




#define XO_NUMBUFS 8
#define XO_SMBUFSZ 128

static const char *
xo_printable (const char *str)
{
static THREAD_LOCAL(char) bufset[XO_NUMBUFS][XO_SMBUFSZ];
static THREAD_LOCAL(int) bufnum = 0;

if (str == NULL)
return "";

if (++bufnum == XO_NUMBUFS)
bufnum = 0;

char *res = bufset[bufnum], *cp, *ep;

for (cp = res, ep = res + XO_SMBUFSZ - 1; *str && cp < ep; cp++, str++) {
if (*str == '\n') {
*cp++ = '\\';
*cp = 'n';
} else if (*str == '\r') {
*cp++ = '\\';
*cp = 'r';
} else if (*str == '\"') {
*cp++ = '\\';
*cp = '"';
} else
*cp = *str;
}

*cp = '\0';
return res;
}

static int
xo_depth_check (xo_handle_t *xop, int depth)
{
xo_stack_t *xsp;

if (depth >= xop->xo_stack_size) {
depth += XO_DEPTH;

xsp = xo_realloc(xop->xo_stack, sizeof(xop->xo_stack[0]) * depth);
if (xsp == NULL) {
xo_failure(xop, "xo_depth_check: out of memory (%d)", depth);
return -1;
}

int count = depth - xop->xo_stack_size;

bzero(xsp + xop->xo_stack_size, count * sizeof(*xsp));
xop->xo_stack_size = depth;
xop->xo_stack = xsp;
}

return 0;
}

void
xo_no_setlocale (void)
{
xo_locale_inited = 1;
}







static const char *
xo_xml_leader_len (xo_handle_t *xop, const char *name, xo_ssize_t nlen)
{
if (name == NULL || isalpha(name[0]) || name[0] == '_')
return "";

xo_failure(xop, "invalid XML tag name: '%.*s'", nlen, name);
return "_";
}

static const char *
xo_xml_leader (xo_handle_t *xop, const char *name)
{
return xo_xml_leader_len(xop, name, strlen(name));
}











static int
xo_is_line_buffered (FILE *stream)
{
#if HAVE___FLBF
if (__flbf(stream))
return 1;
#else
if (isatty(fileno(stream)))
return 1;
#endif
return 0;
}






static void
xo_init_handle (xo_handle_t *xop)
{
xop->xo_opaque = stdout;
xop->xo_write = xo_write_to_file;
xop->xo_flush = xo_flush_file;

if (xo_is_line_buffered(stdout))
XOF_SET(xop, XOF_FLUSH_LINE);







if (!xo_locale_inited) {
xo_locale_inited = 1;

#if defined(__FreeBSD__)
const char *cp = "";
#else
const char *cp = getenv("LC_ALL");
if (cp == NULL)
cp = getenv("LC_CTYPE");
if (cp == NULL)
cp = getenv("LANG");
if (cp == NULL)
cp = "C";
#endif

(void) setlocale(LC_CTYPE, cp);
}





xo_buf_init(&xop->xo_data);
xo_buf_init(&xop->xo_fmt);

if (XOIF_ISSET(xop, XOIF_INIT_IN_PROGRESS))
return;
XOIF_SET(xop, XOIF_INIT_IN_PROGRESS);

xop->xo_indent_by = XO_INDENT_BY;
xo_depth_check(xop, XO_DEPTH);

XOIF_CLEAR(xop, XOIF_INIT_IN_PROGRESS);
}




static void
xo_default_init (void)
{
xo_handle_t *xop = &xo_default_handle;

xo_init_handle(xop);

#if !defined(NO_LIBXO_OPTIONS)
if (!XOF_ISSET(xop, XOF_NO_ENV)) {
char *env = getenv("LIBXO_OPTIONS");

if (env)
xo_set_options_simple(xop, env);

}
#endif

xo_default_inited = 1;
}







static xo_handle_t *
xo_default (xo_handle_t *xop)
{
if (xop == NULL) {
if (xo_default_inited == 0)
xo_default_init();
xop = &xo_default_handle;
}

return xop;
}





static int
xo_indent (xo_handle_t *xop)
{
int rc = 0;

xop = xo_default(xop);

if (XOF_ISSET(xop, XOF_PRETTY)) {
rc = xop->xo_indent * xop->xo_indent_by;
if (XOIF_ISSET(xop, XOIF_TOP_EMITTED))
rc += xop->xo_indent_by;
}

return (rc > 0) ? rc : 0;
}

static void
xo_buf_indent (xo_handle_t *xop, int indent)
{
xo_buffer_t *xbp = &xop->xo_data;

if (indent <= 0)
indent = xo_indent(xop);

if (!xo_buf_has_room(xbp, indent))
return;

memset(xbp->xb_curp, ' ', indent);
xbp->xb_curp += indent;
}

static char xo_xml_amp[] = "&amp;";
static char xo_xml_lt[] = "&lt;";
static char xo_xml_gt[] = "&gt;";
static char xo_xml_quot[] = "&quot;";

static ssize_t
xo_escape_xml (xo_buffer_t *xbp, ssize_t len, xo_xff_flags_t flags)
{
ssize_t slen;
ssize_t delta = 0;
char *cp, *ep, *ip;
const char *sp;
int attr = XOF_BIT_ISSET(flags, XFF_ATTR);

for (cp = xbp->xb_curp, ep = cp + len; cp < ep; cp++) {

if (*cp == '<')
delta += sizeof(xo_xml_lt) - 2;
else if (*cp == '>')
delta += sizeof(xo_xml_gt) - 2;
else if (*cp == '&')
delta += sizeof(xo_xml_amp) - 2;
else if (attr && *cp == '"')
delta += sizeof(xo_xml_quot) - 2;
}

if (delta == 0)
return len;

if (!xo_buf_has_room(xbp, delta))
return 0;

ep = xbp->xb_curp;
cp = ep + len;
ip = cp + delta;
do {
cp -= 1;
ip -= 1;

if (*cp == '<')
sp = xo_xml_lt;
else if (*cp == '>')
sp = xo_xml_gt;
else if (*cp == '&')
sp = xo_xml_amp;
else if (attr && *cp == '"')
sp = xo_xml_quot;
else {
*ip = *cp;
continue;
}

slen = strlen(sp);
ip -= slen - 1;
memcpy(ip, sp, slen);

} while (cp > ep && cp != ip);

return len + delta;
}

static ssize_t
xo_escape_json (xo_buffer_t *xbp, ssize_t len, xo_xff_flags_t flags UNUSED)
{
ssize_t delta = 0;
char *cp, *ep, *ip;

for (cp = xbp->xb_curp, ep = cp + len; cp < ep; cp++) {
if (*cp == '\\' || *cp == '"')
delta += 1;
else if (*cp == '\n' || *cp == '\r')
delta += 1;
}

if (delta == 0)
return len;

if (!xo_buf_has_room(xbp, delta))
return 0;

ep = xbp->xb_curp;
cp = ep + len;
ip = cp + delta;
do {
cp -= 1;
ip -= 1;

if (*cp == '\\' || *cp == '"') {
*ip-- = *cp;
*ip = '\\';
} else if (*cp == '\n') {
*ip-- = 'n';
*ip = '\\';
} else if (*cp == '\r') {
*ip-- = 'r';
*ip = '\\';
} else {
*ip = *cp;
}

} while (cp > ep && cp != ip);

return len + delta;
}





static ssize_t
xo_escape_sdparams (xo_buffer_t *xbp, ssize_t len, xo_xff_flags_t flags UNUSED)
{
ssize_t delta = 0;
char *cp, *ep, *ip;

for (cp = xbp->xb_curp, ep = cp + len; cp < ep; cp++) {
if (*cp == '\\' || *cp == '"' || *cp == ']')
delta += 1;
}

if (delta == 0)
return len;

if (!xo_buf_has_room(xbp, delta))
return 0;

ep = xbp->xb_curp;
cp = ep + len;
ip = cp + delta;
do {
cp -= 1;
ip -= 1;

if (*cp == '\\' || *cp == '"' || *cp == ']') {
*ip-- = *cp;
*ip = '\\';
} else {
*ip = *cp;
}

} while (cp > ep && cp != ip);

return len + delta;
}

static void
xo_buf_escape (xo_handle_t *xop, xo_buffer_t *xbp,
const char *str, ssize_t len, xo_xff_flags_t flags)
{
if (!xo_buf_has_room(xbp, len))
return;

memcpy(xbp->xb_curp, str, len);

switch (xo_style(xop)) {
case XO_STYLE_XML:
case XO_STYLE_HTML:
len = xo_escape_xml(xbp, len, flags);
break;

case XO_STYLE_JSON:
len = xo_escape_json(xbp, len, flags);
break;

case XO_STYLE_SDPARAMS:
len = xo_escape_sdparams(xbp, len, flags);
break;
}

xbp->xb_curp += len;
}





static ssize_t
xo_write (xo_handle_t *xop)
{
ssize_t rc = 0;
xo_buffer_t *xbp = &xop->xo_data;

if (xbp->xb_curp != xbp->xb_bufp) {
xo_buf_append(xbp, "", 1);
xo_anchor_clear(xop);
if (xop->xo_write)
rc = xop->xo_write(xop->xo_opaque, xbp->xb_bufp);
xbp->xb_curp = xbp->xb_bufp;
}


XOIF_CLEAR(xop, XOIF_UNITS_PENDING);

return rc;
}





static ssize_t
xo_vsnprintf (xo_handle_t *xop, xo_buffer_t *xbp, const char *fmt, va_list vap)
{
va_list va_local;
ssize_t rc;
ssize_t left = xbp->xb_size - (xbp->xb_curp - xbp->xb_bufp);

va_copy(va_local, vap);

if (xop->xo_formatter)
rc = xop->xo_formatter(xop, xbp->xb_curp, left, fmt, va_local);
else
rc = vsnprintf(xbp->xb_curp, left, fmt, va_local);

if (rc >= left) {
if (!xo_buf_has_room(xbp, rc)) {
va_end(va_local);
return -1;
}









va_end(va_local);
va_copy(va_local, vap);

left = xbp->xb_size - (xbp->xb_curp - xbp->xb_bufp);
if (xop->xo_formatter)
rc = xop->xo_formatter(xop, xbp->xb_curp, left, fmt, va_local);
else
rc = vsnprintf(xbp->xb_curp, left, fmt, va_local);
}
va_end(va_local);

return rc;
}




static ssize_t
xo_printf_v (xo_handle_t *xop, const char *fmt, va_list vap)
{
xo_buffer_t *xbp = &xop->xo_data;
ssize_t left = xbp->xb_size - (xbp->xb_curp - xbp->xb_bufp);
ssize_t rc;
va_list va_local;

va_copy(va_local, vap);

rc = vsnprintf(xbp->xb_curp, left, fmt, va_local);

if (rc >= left) {
if (!xo_buf_has_room(xbp, rc)) {
va_end(va_local);
return -1;
}

va_end(va_local);
va_copy(va_local, vap);

left = xbp->xb_size - (xbp->xb_curp - xbp->xb_bufp);
rc = vsnprintf(xbp->xb_curp, left, fmt, va_local);
}

va_end(va_local);

if (rc > 0)
xbp->xb_curp += rc;

return rc;
}

static ssize_t
xo_printf (xo_handle_t *xop, const char *fmt, ...)
{
ssize_t rc;
va_list vap;

va_start(vap, fmt);

rc = xo_printf_v(xop, fmt, vap);

va_end(vap);
return rc;
}





static uint8_t xo_utf8_data_bits[5] = { 0, 0x7f, 0x1f, 0x0f, 0x07 };
static uint8_t xo_utf8_len_bits[5] = { 0, 0x00, 0xc0, 0xe0, 0xf0 };




static int
xo_is_utf8 (char ch)
{
return (ch & 0x80);
}





static inline ssize_t
xo_utf8_to_wc_len (const char *buf)
{
uint8_t bval = (uint8_t) *buf;
ssize_t len;

if ((bval & 0x80) == 0x0)
len = 1;
else if ((bval & 0xe0) == 0xc0)
len = 2;
else if ((bval & 0xf0) == 0xe0)
len = 3;
else if ((bval & 0xf8) == 0xf0)
len = 4;
else
len = -1;

return len;
}

static ssize_t
xo_buf_utf8_len (xo_handle_t *xop, const char *buf, ssize_t bufsiz)
{
unsigned b = (unsigned char) *buf;
ssize_t len, i;

len = xo_utf8_to_wc_len(buf);
if (len < 0) {
xo_failure(xop, "invalid UTF-8 data: %02hhx", b);
return -1;
}

if (len > bufsiz) {
xo_failure(xop, "invalid UTF-8 data (short): %02hhx (%d/%d)",
b, len, bufsiz);
return -1;
}

for (i = 2; i < len; i++) {
b = (unsigned char ) buf[i];
if ((b & 0xc0) != 0x80) {
xo_failure(xop, "invalid UTF-8 data (byte %d): %x", i, b);
return -1;
}
}

return len;
}






static inline wchar_t
xo_utf8_char (const char *buf, ssize_t len)
{

if (len == 1)
return (unsigned char) buf[0];

ssize_t i;
wchar_t wc;
const unsigned char *cp = (const unsigned char *) buf;

wc = *cp & xo_utf8_data_bits[len];
for (i = 1; i < len; i++) {
wc <<= 6;
wc |= cp[i] & 0x3f;
if ((cp[i] & 0xc0) != 0x80)
return (wchar_t) -1;
}

return wc;
}




static ssize_t
xo_utf8_emit_len (wchar_t wc)
{
ssize_t len;

if ((wc & ((1 << 7) - 1)) == wc)
len = 1;
else if ((wc & ((1 << 11) - 1)) == wc)
len = 2;
else if ((wc & ((1 << 16) - 1)) == wc)
len = 3;
else if ((wc & ((1 << 21) - 1)) == wc)
len = 4;
else
len = -1;

return len;
}




static void
xo_utf8_emit_char (char *buf, ssize_t len, wchar_t wc)
{
ssize_t i;

if (len == 1) {
buf[0] = wc & 0x7f;
return;
}


for (i = len - 1; i >= 0; i--) {
buf[i] = 0x80 | (wc & 0x3f);
wc >>= 6;
}


buf[0] &= xo_utf8_data_bits[len];
buf[0] |= xo_utf8_len_bits[len];
}






static ssize_t
xo_buf_append_locale_from_utf8 (xo_handle_t *xop, xo_buffer_t *xbp,
const char *ibuf, ssize_t ilen)
{
wchar_t wc;
ssize_t len;






wc = xo_utf8_char(ibuf, ilen);
if (wc == (wchar_t) -1) {
xo_failure(xop, "invalid UTF-8 byte sequence");
return 0;
}

if (XOF_ISSET(xop, XOF_NO_LOCALE)) {
if (!xo_buf_has_room(xbp, ilen))
return 0;

memcpy(xbp->xb_curp, ibuf, ilen);
xbp->xb_curp += ilen;

} else {
if (!xo_buf_has_room(xbp, MB_LEN_MAX + 1))
return 0;

bzero(&xop->xo_mbstate, sizeof(xop->xo_mbstate));
len = wcrtomb(xbp->xb_curp, wc, &xop->xo_mbstate);

if (len <= 0) {
xo_failure(xop, "could not convert wide char: %lx",
(unsigned long) wc);
return 0;
}
xbp->xb_curp += len;
}

return xo_wcwidth(wc);
}




static void
xo_buf_append_locale (xo_handle_t *xop, xo_buffer_t *xbp,
const char *cp, ssize_t len)
{
const char *sp = cp, *ep = cp + len;
ssize_t save_off = xbp->xb_bufp - xbp->xb_curp;
ssize_t slen;
int cols = 0;

for ( ; cp < ep; cp++) {
if (!xo_is_utf8(*cp)) {
cols += 1;
continue;
}









if (sp != cp)
xo_buf_append(xbp, sp, cp - sp);

slen = xo_buf_utf8_len(xop, cp, ep - cp);
if (slen <= 0) {

xbp->xb_curp = xbp->xb_bufp + save_off;
return;
}

cols += xo_buf_append_locale_from_utf8(xop, xbp, cp, slen);


cp += slen - 1;
sp = cp + 1;
}


if (XOF_ISSET(xop, XOF_COLUMNS))
xop->xo_columns += cols;
if (XOIF_ISSET(xop, XOIF_ANCHOR))
xop->xo_anchor_columns += cols;


len = ep - sp;
if (len != 0)
xo_buf_append(xbp, sp, len);
}






static void
xo_data_append (xo_handle_t *xop, const char *str, ssize_t len)
{
xo_buf_append(&xop->xo_data, str, len);
}




static void
xo_data_escape (xo_handle_t *xop, const char *str, ssize_t len)
{
xo_buf_escape(xop, &xop->xo_data, str, len, 0);
}

#if defined(LIBXO_NO_RETAIN)




void
xo_retain_clear_all (void)
{
return;
}

void
xo_retain_clear (const char *fmt UNUSED)
{
return;
}
static void
xo_retain_add (const char *fmt UNUSED, xo_field_info_t *fields UNUSED,
unsigned num_fields UNUSED)
{
return;
}

static int
xo_retain_find (const char *fmt UNUSED, xo_field_info_t **valp UNUSED,
unsigned *nump UNUSED)
{
return -1;
}

#else












typedef struct xo_retain_entry_s {
struct xo_retain_entry_s *xre_next;
unsigned long xre_hits;
const char *xre_format;
unsigned xre_num_fields;
xo_field_info_t *xre_fields;
} xo_retain_entry_t;




#if !defined(XO_RETAIN_SIZE)
#define XO_RETAIN_SIZE 6
#endif
#define RETAIN_HASH_SIZE (1<<XO_RETAIN_SIZE)

typedef struct xo_retain_s {
xo_retain_entry_t *xr_bucket[RETAIN_HASH_SIZE];
} xo_retain_t;

static THREAD_LOCAL(xo_retain_t) xo_retain;
static THREAD_LOCAL(unsigned) xo_retain_count;














static unsigned
xo_retain_hash (const char *fmt)
{
volatile uintptr_t iptr = (uintptr_t) (const void *) fmt;


uint32_t val = (uint32_t) ((iptr >> 4) & (((1 << 24) - 1)));

val = (val ^ 61) ^ (val >> 16);
val = val + (val << 3);
val = val ^ (val >> 4);
val = val * 0x3a8f05c5;
val = val ^ (val >> 15);
val &= RETAIN_HASH_SIZE - 1;

return val;
}




void
xo_retain_clear_all (void)
{
int i;
xo_retain_entry_t *xrep, *next;

for (i = 0; i < RETAIN_HASH_SIZE; i++) {
for (xrep = xo_retain.xr_bucket[i]; xrep; xrep = next) {
next = xrep->xre_next;
xo_free(xrep);
}
xo_retain.xr_bucket[i] = NULL;
}
xo_retain_count = 0;
}




void
xo_retain_clear (const char *fmt)
{
xo_retain_entry_t **xrepp;
unsigned hash = xo_retain_hash(fmt);

for (xrepp = &xo_retain.xr_bucket[hash]; *xrepp;
xrepp = &(*xrepp)->xre_next) {
if ((*xrepp)->xre_format == fmt) {
*xrepp = (*xrepp)->xre_next;
xo_retain_count -= 1;
return;
}
}
}




static int
xo_retain_find (const char *fmt, xo_field_info_t **valp, unsigned *nump)
{
if (xo_retain_count == 0)
return -1;

unsigned hash = xo_retain_hash(fmt);
xo_retain_entry_t *xrep;

for (xrep = xo_retain.xr_bucket[hash]; xrep != NULL;
xrep = xrep->xre_next) {
if (xrep->xre_format == fmt) {
*valp = xrep->xre_fields;
*nump = xrep->xre_num_fields;
xrep->xre_hits += 1;
return 0;
}
}

return -1;
}

static void
xo_retain_add (const char *fmt, xo_field_info_t *fields, unsigned num_fields)
{
unsigned hash = xo_retain_hash(fmt);
xo_retain_entry_t *xrep;
ssize_t sz = sizeof(*xrep) + (num_fields + 1) * sizeof(*fields);
xo_field_info_t *xfip;

xrep = xo_realloc(NULL, sz);
if (xrep == NULL)
return;

xfip = (xo_field_info_t *) &xrep[1];
memcpy(xfip, fields, num_fields * sizeof(*fields));

bzero(xrep, sizeof(*xrep));

xrep->xre_format = fmt;
xrep->xre_fields = xfip;
xrep->xre_num_fields = num_fields;


xrep->xre_next = xo_retain.xr_bucket[hash];
xo_retain.xr_bucket[hash] = xrep;
xo_retain_count += 1;
}

#endif






static void
xo_warn_hcv (xo_handle_t *xop, int code, int check_warn,
const char *fmt, va_list vap)
{
xop = xo_default(xop);
if (check_warn && !XOF_ISSET(xop, XOF_WARN))
return;

if (fmt == NULL)
return;

ssize_t len = strlen(fmt);
ssize_t plen = xo_program ? strlen(xo_program) : 0;
char *newfmt = alloca(len + 1 + plen + 2);

if (plen) {
memcpy(newfmt, xo_program, plen);
newfmt[plen++] = ':';
newfmt[plen++] = ' ';
}

memcpy(newfmt + plen, fmt, len);
newfmt[len + plen] = '\0';

if (XOF_ISSET(xop, XOF_WARN_XML)) {
static char err_open[] = "<error>";
static char err_close[] = "</error>";
static char msg_open[] = "<message>";
static char msg_close[] = "</message>";

xo_buffer_t *xbp = &xop->xo_data;

xo_buf_append(xbp, err_open, sizeof(err_open) - 1);
xo_buf_append(xbp, msg_open, sizeof(msg_open) - 1);

va_list va_local;
va_copy(va_local, vap);

ssize_t left = xbp->xb_size - (xbp->xb_curp - xbp->xb_bufp);
ssize_t rc = vsnprintf(xbp->xb_curp, left, newfmt, vap);

if (rc >= left) {
if (!xo_buf_has_room(xbp, rc)) {
va_end(va_local);
return;
}

va_end(vap);
va_copy(vap, va_local);

left = xbp->xb_size - (xbp->xb_curp - xbp->xb_bufp);
rc = vsnprintf(xbp->xb_curp, left, fmt, vap);
}

va_end(va_local);

rc = xo_escape_xml(xbp, rc, 1);
xbp->xb_curp += rc;

xo_buf_append(xbp, msg_close, sizeof(msg_close) - 1);
xo_buf_append(xbp, err_close, sizeof(err_close) - 1);

if (code >= 0) {
const char *msg = strerror(code);

if (msg) {
xo_buf_append(xbp, ": ", 2);
xo_buf_append(xbp, msg, strlen(msg));
}
}

xo_buf_append(xbp, "\n", 1);
(void) xo_write(xop);

} else {
vfprintf(stderr, newfmt, vap);
if (code >= 0) {
const char *msg = strerror(code);

if (msg)
fprintf(stderr, ": %s", msg);
}
fprintf(stderr, "\n");
}
}

void
xo_warn_hc (xo_handle_t *xop, int code, const char *fmt, ...)
{
va_list vap;

va_start(vap, fmt);
xo_warn_hcv(xop, code, 0, fmt, vap);
va_end(vap);
}

void
xo_warn_c (int code, const char *fmt, ...)
{
va_list vap;

va_start(vap, fmt);
xo_warn_hcv(NULL, code, 0, fmt, vap);
va_end(vap);
}

void
xo_warn (const char *fmt, ...)
{
int code = errno;
va_list vap;

va_start(vap, fmt);
xo_warn_hcv(NULL, code, 0, fmt, vap);
va_end(vap);
}

void
xo_warnx (const char *fmt, ...)
{
va_list vap;

va_start(vap, fmt);
xo_warn_hcv(NULL, -1, 0, fmt, vap);
va_end(vap);
}

void
xo_err (int eval, const char *fmt, ...)
{
int code = errno;
va_list vap;

va_start(vap, fmt);
xo_warn_hcv(NULL, code, 0, fmt, vap);
va_end(vap);
xo_finish();
exit(eval);
}

void
xo_errx (int eval, const char *fmt, ...)
{
va_list vap;

va_start(vap, fmt);
xo_warn_hcv(NULL, -1, 0, fmt, vap);
va_end(vap);
xo_finish();
exit(eval);
}

void
xo_errc (int eval, int code, const char *fmt, ...)
{
va_list vap;

va_start(vap, fmt);
xo_warn_hcv(NULL, code, 0, fmt, vap);
va_end(vap);
xo_finish();
exit(eval);
}






void
xo_message_hcv (xo_handle_t *xop, int code, const char *fmt, va_list vap)
{
static char msg_open[] = "<message>";
static char msg_close[] = "</message>";
xo_buffer_t *xbp;
ssize_t rc;
va_list va_local;

xop = xo_default(xop);

if (fmt == NULL || *fmt == '\0')
return;

int need_nl = (fmt[strlen(fmt) - 1] != '\n');

switch (xo_style(xop)) {
case XO_STYLE_XML:
xbp = &xop->xo_data;
if (XOF_ISSET(xop, XOF_PRETTY))
xo_buf_indent(xop, xop->xo_indent_by);
xo_buf_append(xbp, msg_open, sizeof(msg_open) - 1);

va_copy(va_local, vap);

ssize_t left = xbp->xb_size - (xbp->xb_curp - xbp->xb_bufp);

rc = vsnprintf(xbp->xb_curp, left, fmt, vap);
if (rc >= left) {
if (!xo_buf_has_room(xbp, rc)) {
va_end(va_local);
return;
}

va_end(vap);
va_copy(vap, va_local);

left = xbp->xb_size - (xbp->xb_curp - xbp->xb_bufp);
rc = vsnprintf(xbp->xb_curp, left, fmt, vap);
}

va_end(va_local);

rc = xo_escape_xml(xbp, rc, 0);
xbp->xb_curp += rc;

if (need_nl && code > 0) {
const char *msg = strerror(code);

if (msg) {
xo_buf_append(xbp, ": ", 2);
xo_buf_append(xbp, msg, strlen(msg));
}
}

if (need_nl)
xo_buf_append(xbp, "\n", 1);

xo_buf_append(xbp, msg_close, sizeof(msg_close) - 1);

if (XOF_ISSET(xop, XOF_PRETTY))
xo_buf_append(xbp, "\n", 1);

(void) xo_write(xop);
break;

case XO_STYLE_HTML:
{
char buf[BUFSIZ], *bp = buf, *cp;
ssize_t bufsiz = sizeof(buf);
ssize_t rc2;

va_copy(va_local, vap);

rc = vsnprintf(bp, bufsiz, fmt, va_local);
if (rc > bufsiz) {
bufsiz = rc + BUFSIZ;
bp = alloca(bufsiz);
va_end(va_local);
va_copy(va_local, vap);
rc = vsnprintf(bp, bufsiz, fmt, va_local);
}

va_end(va_local);
cp = bp + rc;

if (need_nl) {
rc2 = snprintf(cp, bufsiz - rc, "%s%s\n",
(code > 0) ? ": " : "",
(code > 0) ? strerror(code) : "");
if (rc2 > 0)
rc += rc2;
}

xo_buf_append_div(xop, "message", 0, NULL, 0, bp, rc,
NULL, 0, NULL, 0);
}
break;

case XO_STYLE_JSON:
case XO_STYLE_SDPARAMS:
case XO_STYLE_ENCODER:

return;

case XO_STYLE_TEXT:
rc = xo_printf_v(xop, fmt, vap);



if (rc > 0) {
if (XOF_ISSET(xop, XOF_COLUMNS))
xop->xo_columns += rc;
if (XOIF_ISSET(xop, XOIF_ANCHOR))
xop->xo_anchor_columns += rc;
}

if (need_nl && code > 0) {
const char *msg = strerror(code);

if (msg) {
xo_printf(xop, ": %s", msg);
}
}
if (need_nl)
xo_printf(xop, "\n");

break;
}

switch (xo_style(xop)) {
case XO_STYLE_HTML:
if (XOIF_ISSET(xop, XOIF_DIV_OPEN)) {
static char div_close[] = "</div>";

XOIF_CLEAR(xop, XOIF_DIV_OPEN);
xo_data_append(xop, div_close, sizeof(div_close) - 1);

if (XOF_ISSET(xop, XOF_PRETTY))
xo_data_append(xop, "\n", 1);
}
break;
}

(void) xo_flush_h(xop);
}

void
xo_message_hc (xo_handle_t *xop, int code, const char *fmt, ...)
{
va_list vap;

va_start(vap, fmt);
xo_message_hcv(xop, code, fmt, vap);
va_end(vap);
}

void
xo_message_c (int code, const char *fmt, ...)
{
va_list vap;

va_start(vap, fmt);
xo_message_hcv(NULL, code, fmt, vap);
va_end(vap);
}

void
xo_message_e (const char *fmt, ...)
{
int code = errno;
va_list vap;

va_start(vap, fmt);
xo_message_hcv(NULL, code, fmt, vap);
va_end(vap);
}

void
xo_message (const char *fmt, ...)
{
va_list vap;

va_start(vap, fmt);
xo_message_hcv(NULL, 0, fmt, vap);
va_end(vap);
}

void
xo_failure (xo_handle_t *xop, const char *fmt, ...)
{
if (!XOF_ISSET(xop, XOF_WARN))
return;

va_list vap;

va_start(vap, fmt);
xo_warn_hcv(xop, -1, 1, fmt, vap);
va_end(vap);
}












xo_handle_t *
xo_create (xo_style_t style, xo_xof_flags_t flags)
{
xo_handle_t *xop = xo_realloc(NULL, sizeof(*xop));

if (xop) {
bzero(xop, sizeof(*xop));

xop->xo_style = style;
XOF_SET(xop, flags);
xo_init_handle(xop);
xop->xo_style = style;
}

return xop;
}











xo_handle_t *
xo_create_to_file (FILE *fp, xo_style_t style, xo_xof_flags_t flags)
{
xo_handle_t *xop = xo_create(style, flags);

if (xop) {
xop->xo_opaque = fp;
xop->xo_write = xo_write_to_file;
xop->xo_close = xo_close_file;
xop->xo_flush = xo_flush_file;
}

return xop;
}








int
xo_set_file_h (xo_handle_t *xop, FILE *fp)
{
xop = xo_default(xop);

if (fp == NULL) {
xo_failure(xop, "xo_set_file: NULL fp");
return -1;
}

xop->xo_opaque = fp;
xop->xo_write = xo_write_to_file;
xop->xo_close = xo_close_file;
xop->xo_flush = xo_flush_file;

return 0;
}







int
xo_set_file (FILE *fp)
{
return xo_set_file_h(NULL, fp);
}






void
xo_destroy (xo_handle_t *xop_arg)
{
xo_handle_t *xop = xo_default(xop_arg);

xo_flush_h(xop);

if (xop->xo_close && XOF_ISSET(xop, XOF_CLOSE_FP))
xop->xo_close(xop->xo_opaque);

xo_free(xop->xo_stack);
xo_buf_cleanup(&xop->xo_data);
xo_buf_cleanup(&xop->xo_fmt);
xo_buf_cleanup(&xop->xo_predicate);
xo_buf_cleanup(&xop->xo_attrs);
xo_buf_cleanup(&xop->xo_color_buf);

if (xop->xo_version)
xo_free(xop->xo_version);

if (xop_arg == NULL) {
bzero(&xo_default_handle, sizeof(xo_default_handle));
xo_default_inited = 0;
} else
xo_free(xop);
}








void
xo_set_style (xo_handle_t *xop, xo_style_t style)
{
xop = xo_default(xop);
xop->xo_style = style;
}







xo_style_t
xo_get_style (xo_handle_t *xop)
{
xop = xo_default(xop);
return xo_style(xop);
}







static int
xo_name_to_style (const char *name)
{
if (xo_streq(name, "xml"))
return XO_STYLE_XML;
else if (xo_streq(name, "json"))
return XO_STYLE_JSON;
else if (xo_streq(name, "encoder"))
return XO_STYLE_ENCODER;
else if (xo_streq(name, "text"))
return XO_STYLE_TEXT;
else if (xo_streq(name, "html"))
return XO_STYLE_HTML;
else if (xo_streq(name, "sdparams"))
return XO_STYLE_SDPARAMS;

return -1;
}




static int
xo_style_is_encoding (xo_handle_t *xop)
{
if (xo_style(xop) == XO_STYLE_JSON
|| xo_style(xop) == XO_STYLE_XML
|| xo_style(xop) == XO_STYLE_SDPARAMS
|| xo_style(xop) == XO_STYLE_ENCODER)
return 1;
return 0;
}


typedef struct xo_mapping_s {
xo_xff_flags_t xm_value;
const char *xm_name;
} xo_mapping_t;

static xo_xff_flags_t
xo_name_lookup (xo_mapping_t *map, const char *value, ssize_t len)
{
if (len == 0)
return 0;

if (len < 0)
len = strlen(value);

while (isspace((int) *value)) {
value += 1;
len -= 1;
}

while (isspace((int) value[len]))
len -= 1;

if (*value == '\0')
return 0;

for ( ; map->xm_name; map++)
if (strncmp(map->xm_name, value, len) == 0)
return map->xm_value;

return 0;
}

#if defined(NOT_NEEDED_YET)
static const char *
xo_value_lookup (xo_mapping_t *map, xo_xff_flags_t value)
{
if (value == 0)
return NULL;

for ( ; map->xm_name; map++)
if (map->xm_value == value)
return map->xm_name;

return NULL;
}
#endif

static xo_mapping_t xo_xof_names[] = {
{ XOF_COLOR_ALLOWED, "color" },
{ XOF_COLOR, "color-force" },
{ XOF_COLUMNS, "columns" },
{ XOF_DTRT, "dtrt" },
{ XOF_FLUSH, "flush" },
{ XOF_FLUSH_LINE, "flush-line" },
{ XOF_IGNORE_CLOSE, "ignore-close" },
{ XOF_INFO, "info" },
{ XOF_KEYS, "keys" },
{ XOF_LOG_GETTEXT, "log-gettext" },
{ XOF_LOG_SYSLOG, "log-syslog" },
{ XOF_NO_HUMANIZE, "no-humanize" },
{ XOF_NO_LOCALE, "no-locale" },
{ XOF_RETAIN_NONE, "no-retain" },
{ XOF_NO_TOP, "no-top" },
{ XOF_NOT_FIRST, "not-first" },
{ XOF_PRETTY, "pretty" },
{ XOF_RETAIN_ALL, "retain" },
{ XOF_UNDERSCORES, "underscores" },
{ XOF_UNITS, "units" },
{ XOF_WARN, "warn" },
{ XOF_WARN_XML, "warn-xml" },
{ XOF_XPATH, "xpath" },
{ 0, NULL }
};


static xo_mapping_t xo_xof_simple_names[] = {
{ XOF_COLOR_ALLOWED, "color" },
{ XOF_FLUSH, "flush" },
{ XOF_FLUSH_LINE, "flush-line" },
{ XOF_NO_HUMANIZE, "no-humanize" },
{ XOF_NO_LOCALE, "no-locale" },
{ XOF_RETAIN_NONE, "no-retain" },
{ XOF_PRETTY, "pretty" },
{ XOF_RETAIN_ALL, "retain" },
{ XOF_UNDERSCORES, "underscores" },
{ XOF_WARN, "warn" },
{ 0, NULL }
};





static unsigned
xo_name_to_flag (const char *name)
{
return (unsigned) xo_name_lookup(xo_xof_names, name, -1);
}








int
xo_set_style_name (xo_handle_t *xop, const char *name)
{
if (name == NULL)
return -1;

int style = xo_name_to_style(name);

if (style < 0)
return -1;

xo_set_style(xop, style);
return 0;
}





static void
xo_set_color_map (xo_handle_t *xop, char *value)
{
if (xo_text_only())
return;

char *cp, *ep, *vp, *np;
ssize_t len = value ? strlen(value) + 1 : 0;
int num = 1, fg, bg;

for (cp = value, ep = cp + len - 1; cp && *cp && cp < ep; cp = np) {
np = strchr(cp, '+');
if (np)
*np++ = '\0';

vp = strchr(cp, '/');
if (vp)
*vp++ = '\0';

fg = *cp ? xo_color_find(cp) : -1;
bg = (vp && *vp) ? xo_color_find(vp) : -1;

#if !defined(LIBXO_TEXT_ONLY)
xop->xo_color_map_fg[num] = (fg < 0) ? num : fg;
xop->xo_color_map_bg[num] = (bg < 0) ? num : bg;
#endif

if (++num > XO_NUM_COLORS)
break;
}


if (num > 1)
XOF_SET(xop, XOF_COLOR_MAP);
else
XOF_CLEAR(xop, XOF_COLOR_MAP);

#if !defined(LIBXO_TEXT_ONLY)

for ( ; num < XO_NUM_COLORS; num++)
xop->xo_color_map_fg[num] = xop->xo_color_map_bg[num] = num;
#endif
}

static int
xo_set_options_simple (xo_handle_t *xop, const char *input)
{
xo_xof_flags_t new_flag;
char *cp, *ep, *vp, *np, *bp;
ssize_t len = strlen(input) + 1;

bp = alloca(len);
memcpy(bp, input, len);

for (cp = bp, ep = cp + len - 1; cp && cp < ep; cp = np) {
np = strchr(cp, ',');
if (np)
*np++ = '\0';

vp = strchr(cp, '=');
if (vp)
*vp++ = '\0';

if (xo_streq("colors", cp)) {
xo_set_color_map(xop, vp);
continue;
}

new_flag = xo_name_lookup(xo_xof_simple_names, cp, -1);
if (new_flag != 0) {
XOF_SET(xop, new_flag);
} else if (xo_streq(cp, "no-color")) {
XOF_CLEAR(xop, XOF_COLOR_ALLOWED);
} else {
xo_failure(xop, "unknown simple option: %s", cp);
return -1;
}
}

return 0;
}










int
xo_set_options (xo_handle_t *xop, const char *input)
{
char *cp, *ep, *vp, *np, *bp;
int style = -1, new_style, rc = 0;
ssize_t len;
xo_xof_flags_t new_flag;

if (input == NULL)
return 0;

xop = xo_default(xop);

#if defined(LIBXO_COLOR_ON_BY_DEFAULT)

XOF_SET(xop, XOF_COLOR_ALLOWED);
#endif






if (*input == ':') {
ssize_t sz;

for (input++ ; *input; input++) {
switch (*input) {
case 'c':
XOF_SET(xop, XOF_COLOR_ALLOWED);
break;

case 'f':
XOF_SET(xop, XOF_FLUSH);
break;

case 'F':
XOF_SET(xop, XOF_FLUSH_LINE);
break;

case 'g':
XOF_SET(xop, XOF_LOG_GETTEXT);
break;

case 'H':
xop->xo_style = XO_STYLE_HTML;
break;

case 'I':
XOF_SET(xop, XOF_INFO);
break;

case 'i':
sz = strspn(input + 1, "0123456789");
if (sz > 0) {
xop->xo_indent_by = atoi(input + 1);
input += sz - 1;
}
break;

case 'J':
xop->xo_style = XO_STYLE_JSON;
break;

case 'k':
XOF_SET(xop, XOF_KEYS);
break;

case 'n':
XOF_SET(xop, XOF_NO_HUMANIZE);
break;

case 'P':
XOF_SET(xop, XOF_PRETTY);
break;

case 'T':
xop->xo_style = XO_STYLE_TEXT;
break;

case 'U':
XOF_SET(xop, XOF_UNITS);
break;

case 'u':
XOF_SET(xop, XOF_UNDERSCORES);
break;

case 'W':
XOF_SET(xop, XOF_WARN);
break;

case 'X':
xop->xo_style = XO_STYLE_XML;
break;

case 'x':
XOF_SET(xop, XOF_XPATH);
break;
}
}
return 0;
}

len = strlen(input) + 1;
bp = alloca(len);
memcpy(bp, input, len);

for (cp = bp, ep = cp + len - 1; cp && cp < ep; cp = np) {
np = strchr(cp, ',');
if (np)
*np++ = '\0';






if (*cp == '@') {
vp = cp + 1;

if (*vp == '\0')
xo_failure(xop, "missing value for encoder option");
else {
rc = xo_encoder_init(xop, vp);
if (rc)
xo_warnx("error initializing encoder: %s", vp);
}

continue;
}

vp = strchr(cp, '=');
if (vp)
*vp++ = '\0';

if (xo_streq("colors", cp)) {
xo_set_color_map(xop, vp);
continue;
}





new_style = xo_name_to_style(cp);
if (new_style >= 0 && new_style != XO_STYLE_ENCODER) {
if (style >= 0)
xo_warnx("ignoring multiple styles: '%s'", cp);
else
style = new_style;
} else {
new_flag = xo_name_to_flag(cp);
if (new_flag != 0)
XOF_SET(xop, new_flag);
else if (xo_streq(cp, "no-color"))
XOF_CLEAR(xop, XOF_COLOR_ALLOWED);
else if (xo_streq(cp, "indent")) {
if (vp)
xop->xo_indent_by = atoi(vp);
else
xo_failure(xop, "missing value for indent option");
} else if (xo_streq(cp, "encoder")) {
if (vp == NULL)
xo_failure(xop, "missing value for encoder option");
else {
rc = xo_encoder_init(xop, vp);
if (rc)
xo_warnx("error initializing encoder: %s", vp);
}

} else {
xo_warnx("unknown libxo option value: '%s'", cp);
rc = -1;
}
}
}

if (style > 0)
xop->xo_style= style;

return rc;
}








void
xo_set_flags (xo_handle_t *xop, xo_xof_flags_t flags)
{
xop = xo_default(xop);

XOF_SET(xop, flags);
}






xo_xof_flags_t
xo_get_flags (xo_handle_t *xop)
{
xop = xo_default(xop);

return xop->xo_flags;
}




static char *
xo_strndup (const char *str, ssize_t len)
{
if (len < 0)
len = strlen(str);

char *cp = xo_realloc(NULL, len + 1);
if (cp) {
memcpy(cp, str, len);
cp[len] = '\0';
}

return cp;
}









void
xo_set_leading_xpath (xo_handle_t *xop, const char *path)
{
xop = xo_default(xop);

if (xop->xo_leading_xpath) {
xo_free(xop->xo_leading_xpath);
xop->xo_leading_xpath = NULL;
}

if (path == NULL)
return;

xop->xo_leading_xpath = xo_strndup(path, -1);
}








void
xo_set_info (xo_handle_t *xop, xo_info_t *infop, int count)
{
xop = xo_default(xop);

if (count < 0 && infop) {
xo_info_t *xip;

for (xip = infop, count = 0; xip->xi_name; xip++, count++)
continue;
}

xop->xo_info = infop;
xop->xo_info_count = count;
}






void
xo_set_formatter (xo_handle_t *xop, xo_formatter_t func,
xo_checkpointer_t cfunc)
{
xop = xo_default(xop);

xop->xo_formatter = func;
xop->xo_checkpointer = cfunc;
}








void
xo_clear_flags (xo_handle_t *xop, xo_xof_flags_t flags)
{
xop = xo_default(xop);

XOF_CLEAR(xop, flags);
}

static const char *
xo_state_name (xo_state_t state)
{
static const char *names[] = {
"init",
"open_container",
"close_container",
"open_list",
"close_list",
"open_instance",
"close_instance",
"open_leaf_list",
"close_leaf_list",
"discarding",
"marker",
"emit",
"emit_leaf_list",
"finish",
NULL
};

if (state < (sizeof(names) / sizeof(names[0])))
return names[state];

return "unknown";
}

static void
xo_line_ensure_open (xo_handle_t *xop, xo_xff_flags_t flags UNUSED)
{
static char div_open[] = "<div class=\"line\">";
static char div_open_blank[] = "<div class=\"blank-line\">";

if (XOF_ISSET(xop, XOF_CONTINUATION)) {
XOF_CLEAR(xop, XOF_CONTINUATION);
XOIF_SET(xop, XOIF_DIV_OPEN);
return;
}

if (XOIF_ISSET(xop, XOIF_DIV_OPEN))
return;

if (xo_style(xop) != XO_STYLE_HTML)
return;

XOIF_SET(xop, XOIF_DIV_OPEN);
if (flags & XFF_BLANK_LINE)
xo_data_append(xop, div_open_blank, sizeof(div_open_blank) - 1);
else
xo_data_append(xop, div_open, sizeof(div_open) - 1);

if (XOF_ISSET(xop, XOF_PRETTY))
xo_data_append(xop, "\n", 1);
}

static void
xo_line_close (xo_handle_t *xop)
{
static char div_close[] = "</div>";

switch (xo_style(xop)) {
case XO_STYLE_HTML:
if (!XOIF_ISSET(xop, XOIF_DIV_OPEN))
xo_line_ensure_open(xop, 0);

XOIF_CLEAR(xop, XOIF_DIV_OPEN);
xo_data_append(xop, div_close, sizeof(div_close) - 1);

if (XOF_ISSET(xop, XOF_PRETTY))
xo_data_append(xop, "\n", 1);
break;

case XO_STYLE_TEXT:
xo_data_append(xop, "\n", 1);
break;
}
}

static int
xo_info_compare (const void *key, const void *data)
{
const char *name = key;
const xo_info_t *xip = data;

return strcmp(name, xip->xi_name);
}


static xo_info_t *
xo_info_find (xo_handle_t *xop, const char *name, ssize_t nlen)
{
xo_info_t *xip;
char *cp = alloca(nlen + 1);

memcpy(cp, name, nlen);
cp[nlen] = '\0';

xip = bsearch(cp, xop->xo_info, xop->xo_info_count,
sizeof(xop->xo_info[0]), xo_info_compare);
return xip;
}

#define CONVERT(_have, _need) (((_have) << 8) | (_need))




static int
xo_check_conversion (xo_handle_t *xop, int have_enc, int need_enc)
{
switch (CONVERT(have_enc, need_enc)) {
case CONVERT(XF_ENC_UTF8, XF_ENC_UTF8):
case CONVERT(XF_ENC_UTF8, XF_ENC_LOCALE):
case CONVERT(XF_ENC_WIDE, XF_ENC_UTF8):
case CONVERT(XF_ENC_WIDE, XF_ENC_LOCALE):
case CONVERT(XF_ENC_LOCALE, XF_ENC_LOCALE):
case CONVERT(XF_ENC_LOCALE, XF_ENC_UTF8):
return 0;

default:
xo_failure(xop, "invalid conversion (%c:%c)", have_enc, need_enc);
return 1;
}
}

static int
xo_format_string_direct (xo_handle_t *xop, xo_buffer_t *xbp,
xo_xff_flags_t flags,
const wchar_t *wcp, const char *cp,
ssize_t len, int max,
int need_enc, int have_enc)
{
int cols = 0;
wchar_t wc = 0;
ssize_t ilen, olen;
ssize_t width;
int attr = XOF_BIT_ISSET(flags, XFF_ATTR);
const char *sp;

if (len > 0 && !xo_buf_has_room(xbp, len))
return 0;

for (;;) {
if (len == 0)
break;

if (cp) {
if (*cp == '\0')
break;
if ((flags & XFF_UNESCAPE) && (*cp == '\\' || *cp == '%')) {
cp += 1;
len -= 1;
if (len == 0 || *cp == '\0')
break;
}
}

if (wcp && *wcp == L'\0')
break;

ilen = 0;

switch (have_enc) {
case XF_ENC_WIDE:
wc = *wcp++;
ilen = 1;
break;

case XF_ENC_UTF8:
ilen = xo_utf8_to_wc_len(cp);
if (ilen < 0) {
xo_failure(xop, "invalid UTF-8 character: %02hhx", *cp);
return -1;
}

if (len > 0 && len < ilen) {
len = 0;
continue;
}

wc = xo_utf8_char(cp, ilen);
if (wc == (wchar_t) -1) {
xo_failure(xop, "invalid UTF-8 character: %02hhx/%d",
*cp, ilen);
return -1;
}
cp += ilen;
break;

case XF_ENC_LOCALE:
ilen = (len > 0) ? len : MB_LEN_MAX;
ilen = mbrtowc(&wc, cp, ilen, &xop->xo_mbstate);
if (ilen < 0) {
xo_failure(xop, "invalid mbs char: %02hhx", *cp);
wc = L'?';
ilen = 1;
}

if (ilen == 0) {
len = 0;
continue;
}

cp += ilen;
break;
}


if (len > 0) {
len -= ilen;
if (len < 0)
len = 0;
}






width = xo_wcwidth(wc);
if (width < 0)
width = iswcntrl(wc) ? 0 : 1;

if (xo_style(xop) == XO_STYLE_TEXT || xo_style(xop) == XO_STYLE_HTML) {
if (max > 0 && cols + width > max)
break;
}

switch (need_enc) {
case XF_ENC_UTF8:


switch (xo_style(xop)) {
case XO_STYLE_XML:
case XO_STYLE_HTML:
if (wc == '<')
sp = xo_xml_lt;
else if (wc == '>')
sp = xo_xml_gt;
else if (wc == '&')
sp = xo_xml_amp;
else if (attr && wc == '"')
sp = xo_xml_quot;
else
break;

ssize_t slen = strlen(sp);
if (!xo_buf_has_room(xbp, slen - 1))
return -1;

memcpy(xbp->xb_curp, sp, slen);
xbp->xb_curp += slen;
goto done_with_encoding;

case XO_STYLE_JSON:
if (wc != '\\' && wc != '"' && wc != '\n' && wc != '\r')
break;

if (!xo_buf_has_room(xbp, 2))
return -1;

*xbp->xb_curp++ = '\\';
if (wc == '\n')
wc = 'n';
else if (wc == '\r')
wc = 'r';
else wc = wc & 0x7f;

*xbp->xb_curp++ = wc;
goto done_with_encoding;

case XO_STYLE_SDPARAMS:
if (wc != '\\' && wc != '"' && wc != ']')
break;

if (!xo_buf_has_room(xbp, 2))
return -1;

*xbp->xb_curp++ = '\\';
wc = wc & 0x7f;
*xbp->xb_curp++ = wc;
goto done_with_encoding;
}

olen = xo_utf8_emit_len(wc);
if (olen < 0) {
xo_failure(xop, "ignoring bad length");
continue;
}

if (!xo_buf_has_room(xbp, olen))
return -1;

xo_utf8_emit_char(xbp->xb_curp, olen, wc);
xbp->xb_curp += olen;
break;

case XF_ENC_LOCALE:
if (!xo_buf_has_room(xbp, MB_LEN_MAX + 1))
return -1;

olen = wcrtomb(xbp->xb_curp, wc, &xop->xo_mbstate);
if (olen <= 0) {
xo_failure(xop, "could not convert wide char: %lx",
(unsigned long) wc);
width = 1;
*xbp->xb_curp++ = '?';
} else
xbp->xb_curp += olen;
break;
}

done_with_encoding:
cols += width;
}

return cols;
}

static int
xo_needed_encoding (xo_handle_t *xop)
{
if (XOF_ISSET(xop, XOF_UTF8))
return XF_ENC_UTF8;

if (xo_style(xop) == XO_STYLE_TEXT)
return XF_ENC_LOCALE;

return XF_ENC_UTF8;
}

static ssize_t
xo_format_string (xo_handle_t *xop, xo_buffer_t *xbp, xo_xff_flags_t flags,
xo_format_t *xfp)
{
static char null[] = "(null)";
static char null_no_quotes[] = "null";

char *cp = NULL;
wchar_t *wcp = NULL;
ssize_t len;
ssize_t cols = 0, rc = 0;
ssize_t off = xbp->xb_curp - xbp->xb_bufp, off2;
int need_enc = xo_needed_encoding(xop);

if (xo_check_conversion(xop, xfp->xf_enc, need_enc))
return 0;

len = xfp->xf_width[XF_WIDTH_SIZE];

if (xfp->xf_fc == 'm') {
cp = strerror(xop->xo_errno);
if (len < 0)
len = cp ? strlen(cp) : 0;
goto normal_string;

} else if (xfp->xf_enc == XF_ENC_WIDE) {
wcp = va_arg(xop->xo_vap, wchar_t *);
if (xfp->xf_skip)
return 0;





if (wcp == NULL) {
cp = null;
len = sizeof(null) - 1;
}

} else {
cp = va_arg(xop->xo_vap, char *);

normal_string:
if (xfp->xf_skip)
return 0;


if (cp == NULL) {
if ((flags & XFF_NOQUOTE) && xo_style_is_encoding(xop)) {
cp = null_no_quotes;
len = sizeof(null_no_quotes) - 1;
} else {
cp = null;
len = sizeof(null) - 1;
}
}





if (xfp->xf_enc == need_enc
&& xfp->xf_width[XF_WIDTH_MIN] < 0
&& xfp->xf_width[XF_WIDTH_SIZE] < 0
&& xfp->xf_width[XF_WIDTH_MAX] < 0
&& !(XOIF_ISSET(xop, XOIF_ANCHOR)
|| XOF_ISSET(xop, XOF_COLUMNS))) {
len = strlen(cp);
xo_buf_escape(xop, xbp, cp, len, flags);






off2 = xbp->xb_curp - xbp->xb_bufp;
rc = off2 - off;
xbp->xb_curp = xbp->xb_bufp + off;

return rc;
}
}

cols = xo_format_string_direct(xop, xbp, flags, wcp, cp, len,
xfp->xf_width[XF_WIDTH_MAX],
need_enc, xfp->xf_enc);
if (cols < 0)
goto bail;




off2 = xbp->xb_curp - xbp->xb_bufp;
rc = off2 - off;
xbp->xb_curp = xbp->xb_bufp + off;

if (cols < xfp->xf_width[XF_WIDTH_MIN]) {





int delta = xfp->xf_width[XF_WIDTH_MIN] - cols;
if (!xo_buf_has_room(xbp, xfp->xf_width[XF_WIDTH_MIN]))
goto bail;





if (xfp->xf_seen_minus) {
cp = xbp->xb_curp + rc;
} else {
cp = xbp->xb_curp;
memmove(xbp->xb_curp + delta, xbp->xb_curp, rc);
}


memset(cp, (xfp->xf_leading_zero > 0) ? '0' : ' ', delta);
rc += delta;
cols += delta;
}

if (XOF_ISSET(xop, XOF_COLUMNS))
xop->xo_columns += cols;
if (XOIF_ISSET(xop, XOIF_ANCHOR))
xop->xo_anchor_columns += cols;

return rc;

bail:
xbp->xb_curp = xbp->xb_bufp + off;
return 0;
}




static int
xo_buf_find_last_number (xo_buffer_t *xbp, ssize_t start_offset)
{
int rc = 0;
int digit = 1;
char *sp = xbp->xb_bufp;
char *cp = sp + start_offset;

while (--cp >= sp)
if (isdigit((int) *cp))
break;

for ( ; cp >= sp; cp--) {
if (!isdigit((int) *cp))
break;
rc += (*cp - '0') * digit;
digit *= 10;
}

return rc;
}

static ssize_t
xo_count_utf8_cols (const char *str, ssize_t len)
{
ssize_t tlen;
wchar_t wc;
ssize_t cols = 0;
const char *ep = str + len;

while (str < ep) {
tlen = xo_utf8_to_wc_len(str);
if (tlen < 0)
return cols;

wc = xo_utf8_char(str, tlen);
if (wc == (wchar_t) -1)
return cols;


if (iswprint((wint_t) wc)) {




ssize_t width = xo_wcwidth(wc);
if (width < 0)
width = iswcntrl(wc) ? 0 : 1;

cols += width;
}

str += tlen;
}

return cols;
}

#if defined(HAVE_GETTEXT)
static inline const char *
xo_dgettext (xo_handle_t *xop, const char *str)
{
const char *domainname = xop->xo_gt_domain;
const char *res;

res = dgettext(domainname, str);

if (XOF_ISSET(xop, XOF_LOG_GETTEXT))
fprintf(stderr, "xo: gettext: %s%s%smsgid \"%s\" returns \"%s\"\n",
domainname ? "domain \"" : "", xo_printable(domainname),
domainname ? "\", " : "", xo_printable(str), xo_printable(res));

return res;
}

static inline const char *
xo_dngettext (xo_handle_t *xop, const char *sing, const char *plural,
unsigned long int n)
{
const char *domainname = xop->xo_gt_domain;
const char *res;

res = dngettext(domainname, sing, plural, n);
if (XOF_ISSET(xop, XOF_LOG_GETTEXT))
fprintf(stderr, "xo: gettext: %s%s%s"
"msgid \"%s\", msgid_plural \"%s\" (%lu) returns \"%s\"\n",
domainname ? "domain \"" : "",
xo_printable(domainname), domainname ? "\", " : "",
xo_printable(sing),
xo_printable(plural), n, xo_printable(res));

return res;
}
#else
static inline const char *
xo_dgettext (xo_handle_t *xop UNUSED, const char *str)
{
return str;
}

static inline const char *
xo_dngettext (xo_handle_t *xop UNUSED, const char *singular,
const char *plural, unsigned long int n)
{
return (n == 1) ? singular : plural;
}
#endif









static ssize_t
xo_format_gettext (xo_handle_t *xop, xo_xff_flags_t flags,
ssize_t start_offset, ssize_t cols, int need_enc)
{
xo_buffer_t *xbp = &xop->xo_data;

if (!xo_buf_has_room(xbp, 1))
return cols;

xbp->xb_curp[0] = '\0';

char *cp = xbp->xb_bufp + start_offset;
ssize_t len = xbp->xb_curp - cp;
const char *newstr = NULL;





if (flags & XFF_GT_PLURAL) {
int n = xo_buf_find_last_number(xbp, start_offset);
char *two = memchr(cp, (int) ',', len);
if (two == NULL) {
xo_failure(xop, "no comma in plural gettext field: '%s'", cp);
return cols;
}

if (two == cp) {
xo_failure(xop, "nothing before comma in plural gettext "
"field: '%s'", cp);
return cols;
}

if (two == xbp->xb_curp) {
xo_failure(xop, "nothing after comma in plural gettext "
"field: '%s'", cp);
return cols;
}

*two++ = '\0';
if (flags & XFF_GT_FIELD) {
newstr = xo_dngettext(xop, cp, two, n);
} else {

newstr = (n == 1) ? cp : two;
}





if (newstr == cp) {
xbp->xb_curp = two - 1;




if (need_enc == XF_ENC_UTF8)
return xo_count_utf8_cols(cp, xbp->xb_curp - cp);
}

} else {

newstr = xo_dgettext(xop, cp);

if (newstr == cp) {

if (need_enc == XF_ENC_UTF8)
return cols;
}
}





ssize_t nlen = strlen(newstr);
char *newcopy = alloca(nlen + 1);
memcpy(newcopy, newstr, nlen + 1);

xbp->xb_curp = xbp->xb_bufp + start_offset;
return xo_format_string_direct(xop, xbp, flags, NULL, newcopy, nlen, 0,
need_enc, XF_ENC_UTF8);
}

static void
xo_data_append_content (xo_handle_t *xop, const char *str, ssize_t len,
xo_xff_flags_t flags)
{
int cols;
int need_enc = xo_needed_encoding(xop);
ssize_t start_offset = xo_buf_offset(&xop->xo_data);

cols = xo_format_string_direct(xop, &xop->xo_data, XFF_UNESCAPE | flags,
NULL, str, len, -1,
need_enc, XF_ENC_UTF8);
if (flags & XFF_GT_FLAGS)
cols = xo_format_gettext(xop, flags, start_offset, cols, need_enc);

if (XOF_ISSET(xop, XOF_COLUMNS))
xop->xo_columns += cols;
if (XOIF_ISSET(xop, XOIF_ANCHOR))
xop->xo_anchor_columns += cols;
}






static void
xo_bump_width (xo_format_t *xfp, int digit)
{
int *ip = &xfp->xf_width[xfp->xf_dots];

*ip = ((*ip > 0) ? *ip : 0) * 10 + digit;
}

static ssize_t
xo_trim_ws (xo_buffer_t *xbp, ssize_t len)
{
char *cp, *sp, *ep;
ssize_t delta;


for (cp = sp = xbp->xb_curp, ep = cp + len; cp < ep; cp++) {
if (*cp != ' ')
break;
}

delta = cp - sp;
if (delta) {
len -= delta;
memmove(sp, cp, len);
}


for (cp = xbp->xb_curp, sp = ep = cp + len; cp < ep; ep--) {
if (ep[-1] != ' ')
break;
}

delta = sp - ep;
if (delta) {
len -= delta;
cp[len] = '\0';
}

return len;
}






static ssize_t
xo_do_format_field (xo_handle_t *xop, xo_buffer_t *xbp,
const char *fmt, ssize_t flen, xo_xff_flags_t flags)
{
xo_format_t xf;
const char *cp, *ep, *sp, *xp = NULL;
ssize_t rc, cols;
int style = (flags & XFF_XML) ? XO_STYLE_XML : xo_style(xop);
unsigned make_output = !(flags & XFF_NO_OUTPUT) ? 1 : 0;
int need_enc = xo_needed_encoding(xop);
int real_need_enc = need_enc;
ssize_t old_cols = xop->xo_columns;


if (flags & XFF_GT_FIELD)
need_enc = XF_ENC_UTF8;

if (xbp == NULL)
xbp = &xop->xo_data;

ssize_t start_offset = xo_buf_offset(xbp);

for (cp = fmt, ep = fmt + flen; cp < ep; cp++) {





if (*cp != '%') {
add_one:
if (xp == NULL)
xp = cp;

if (*cp == '\\' && cp[1] != '\0')
cp += 1;
continue;

} else if (cp + 1 < ep && cp[1] == '%') {
cp += 1;
goto add_one;
}

if (xp) {
if (make_output) {
cols = xo_format_string_direct(xop, xbp, flags | XFF_UNESCAPE,
NULL, xp, cp - xp, -1,
need_enc, XF_ENC_UTF8);
if (XOF_ISSET(xop, XOF_COLUMNS))
xop->xo_columns += cols;
if (XOIF_ISSET(xop, XOIF_ANCHOR))
xop->xo_anchor_columns += cols;
}

xp = NULL;
}

bzero(&xf, sizeof(xf));
xf.xf_leading_zero = -1;
xf.xf_width[0] = xf.xf_width[1] = xf.xf_width[2] = -1;





if (cp[1] == '@') {
for (cp += 2; cp < ep; cp++) {
if (*cp == '@') {
break;
}
if (*cp == '*') {




if (!XOF_ISSET(xop, XOF_NO_VA_ARG))
va_arg(xop->xo_vap, int);
}
}
}


if (XOF_ISSET(xop, XFF_ENCODE_ONLY)) {
if (style != XO_STYLE_XML
&& !xo_style_is_encoding(xop))
xf.xf_skip = 1;
} else if (XOF_ISSET(xop, XFF_DISPLAY_ONLY)) {
if (style != XO_STYLE_TEXT
&& xo_style(xop) != XO_STYLE_HTML)
xf.xf_skip = 1;
}

if (!make_output)
xf.xf_skip = 1;







sp = cp;
for (cp += 1; cp < ep; cp++) {
if (*cp == 'l')
xf.xf_lflag += 1;
else if (*cp == 'h')
xf.xf_hflag += 1;
else if (*cp == 'j')
xf.xf_jflag += 1;
else if (*cp == 't')
xf.xf_tflag += 1;
else if (*cp == 'z')
xf.xf_zflag += 1;
else if (*cp == 'q')
xf.xf_qflag += 1;
else if (*cp == '.') {
if (++xf.xf_dots >= XF_WIDTH_NUM) {
xo_failure(xop, "Too many dots in format: '%s'", fmt);
return -1;
}
} else if (*cp == '-')
xf.xf_seen_minus = 1;
else if (isdigit((int) *cp)) {
if (xf.xf_leading_zero < 0)
xf.xf_leading_zero = (*cp == '0');
xo_bump_width(&xf, *cp - '0');
} else if (*cp == '*') {
xf.xf_stars += 1;
xf.xf_star[xf.xf_dots] = 1;
} else if (strchr("diouxXDOUeEfFgGaAcCsSpm", *cp) != NULL)
break;
else if (*cp == 'n' || *cp == 'v') {
xo_failure(xop, "unsupported format: '%s'", fmt);
return -1;
}
}

if (cp == ep)
xo_failure(xop, "field format missing format character: %s",
fmt);

xf.xf_fc = *cp;

if (!XOF_ISSET(xop, XOF_NO_VA_ARG)) {
if (*cp == 's' || *cp == 'S') {

int s;
for (s = 0; s < XF_WIDTH_NUM; s++) {
if (xf.xf_star[s]) {
xf.xf_width[s] = va_arg(xop->xo_vap, int);


if (xf.xf_width[s] < 0) {
if (s == 0) {
xf.xf_width[0] = -xf.xf_width[0];
xf.xf_seen_minus = 1;
} else
xf.xf_width[s] = -1;
}
}
}
}
}


if (xf.xf_width[XF_WIDTH_MAX] < 0 && xf.xf_width[XF_WIDTH_SIZE] >= 0)
xf.xf_width[XF_WIDTH_MAX] = xf.xf_width[XF_WIDTH_SIZE];

if (xf.xf_fc == 'D' || xf.xf_fc == 'O' || xf.xf_fc == 'U')
xf.xf_lflag = 1;

if (!xf.xf_skip) {
xo_buffer_t *fbp = &xop->xo_fmt;
ssize_t len = cp - sp + 1;
if (!xo_buf_has_room(fbp, len + 1))
return -1;

char *newfmt = fbp->xb_curp;
memcpy(newfmt, sp, len);
newfmt[0] = '%';
newfmt[len] = '\0';






if (xop->xo_formatter == NULL
&& (xf.xf_fc == 's' || xf.xf_fc == 'S'
|| xf.xf_fc == 'm')) {

xf.xf_enc = (xf.xf_fc == 'm') ? XF_ENC_UTF8
: (xf.xf_lflag || (xf.xf_fc == 'S')) ? XF_ENC_WIDE
: xf.xf_hflag ? XF_ENC_LOCALE : XF_ENC_UTF8;

rc = xo_format_string(xop, xbp, flags, &xf);

if ((flags & XFF_TRIM_WS) && xo_style_is_encoding(xop))
rc = xo_trim_ws(xbp, rc);

} else {
ssize_t columns = rc = xo_vsnprintf(xop, xbp, newfmt,
xop->xo_vap);

if (rc > 0) {




switch (style) {
case XO_STYLE_XML:
if (flags & XFF_TRIM_WS)
columns = rc = xo_trim_ws(xbp, rc);

case XO_STYLE_HTML:
rc = xo_escape_xml(xbp, rc, (flags & XFF_ATTR));
break;

case XO_STYLE_JSON:
if (flags & XFF_TRIM_WS)
columns = rc = xo_trim_ws(xbp, rc);
rc = xo_escape_json(xbp, rc, 0);
break;

case XO_STYLE_SDPARAMS:
if (flags & XFF_TRIM_WS)
columns = rc = xo_trim_ws(xbp, rc);
rc = xo_escape_sdparams(xbp, rc, 0);
break;

case XO_STYLE_ENCODER:
if (flags & XFF_TRIM_WS)
columns = rc = xo_trim_ws(xbp, rc);
break;
}








if (XOF_ISSET(xop, XOF_COLUMNS))
xop->xo_columns += columns;
if (XOIF_ISSET(xop, XOIF_ANCHOR))
xop->xo_anchor_columns += columns;
}
}

if (rc > 0)
xbp->xb_curp += rc;
}





if (!XOF_ISSET(xop, XOF_NO_VA_ARG)) {

if (xf.xf_fc == 's' ||xf.xf_fc == 'S') {





if (xf.xf_skip)
va_arg(xop->xo_vap, char *);

} else if (xf.xf_fc == 'm') {


} else {
int s;
for (s = 0; s < XF_WIDTH_NUM; s++) {
if (xf.xf_star[s])
va_arg(xop->xo_vap, int);
}

if (strchr("diouxXDOU", xf.xf_fc) != NULL) {
if (xf.xf_hflag > 1) {
va_arg(xop->xo_vap, int);

} else if (xf.xf_hflag > 0) {
va_arg(xop->xo_vap, int);

} else if (xf.xf_lflag > 1) {
va_arg(xop->xo_vap, unsigned long long);

} else if (xf.xf_lflag > 0) {
va_arg(xop->xo_vap, unsigned long);

} else if (xf.xf_jflag > 0) {
va_arg(xop->xo_vap, intmax_t);

} else if (xf.xf_tflag > 0) {
va_arg(xop->xo_vap, ptrdiff_t);

} else if (xf.xf_zflag > 0) {
va_arg(xop->xo_vap, size_t);

} else if (xf.xf_qflag > 0) {
va_arg(xop->xo_vap, quad_t);

} else {
va_arg(xop->xo_vap, int);
}
} else if (strchr("eEfFgGaA", xf.xf_fc) != NULL)
if (xf.xf_lflag)
va_arg(xop->xo_vap, long double);
else
va_arg(xop->xo_vap, double);

else if (xf.xf_fc == 'C' || (xf.xf_fc == 'c' && xf.xf_lflag))
va_arg(xop->xo_vap, wint_t);

else if (xf.xf_fc == 'c')
va_arg(xop->xo_vap, int);

else if (xf.xf_fc == 'p')
va_arg(xop->xo_vap, void *);
}
}
}

if (xp) {
if (make_output) {
cols = xo_format_string_direct(xop, xbp, flags | XFF_UNESCAPE,
NULL, xp, cp - xp, -1,
need_enc, XF_ENC_UTF8);

if (XOF_ISSET(xop, XOF_COLUMNS))
xop->xo_columns += cols;
if (XOIF_ISSET(xop, XOIF_ANCHOR))
xop->xo_anchor_columns += cols;
}

xp = NULL;
}

if (flags & XFF_GT_FLAGS) {





ssize_t new_cols = xo_format_gettext(xop, flags, start_offset,
old_cols, real_need_enc);

if (XOF_ISSET(xop, XOF_COLUMNS))
xop->xo_columns += new_cols - old_cols;
if (XOIF_ISSET(xop, XOIF_ANCHOR))
xop->xo_anchor_columns += new_cols - old_cols;
}

return 0;
}





static char *
xo_fix_encoding (xo_handle_t *xop UNUSED, char *encoding)
{
char *cp = encoding;

if (cp[0] != '%' || !isdigit((int) cp[1]))
return encoding;

for (cp += 2; *cp; cp++) {
if (!isdigit((int) *cp))
break;
}

*--cp = '%';

return cp;
}

static void
xo_color_append_html (xo_handle_t *xop)
{




if (!xo_buf_is_empty(&xop->xo_color_buf)) {
xo_buffer_t *xbp = &xop->xo_color_buf;

xo_data_append(xop, xbp->xb_bufp, xbp->xb_curp - xbp->xb_bufp);
}
}









static ssize_t
xo_humanize (char *buf, ssize_t len, uint64_t value, int flags)
{
int scale = 0;

if (value) {
uint64_t left = value;

if (flags & HN_DIVISOR_1000) {
for ( ; left; scale++)
left /= 1000;
} else {
for ( ; left; scale++)
left /= 1024;
}
scale -= 1;
}

return xo_humanize_number(buf, len, value, "", scale, flags);
}






typedef struct xo_humanize_save_s {
ssize_t xhs_offset;
ssize_t xhs_columns;
ssize_t xhs_anchor_columns;
} xo_humanize_save_t;






static void
xo_format_humanize (xo_handle_t *xop, xo_buffer_t *xbp,
xo_humanize_save_t *savep, xo_xff_flags_t flags)
{
if (XOF_ISSET(xop, XOF_NO_HUMANIZE))
return;

ssize_t end_offset = xbp->xb_curp - xbp->xb_bufp;
if (end_offset == savep->xhs_offset)
return;






uint64_t value;
char *ep;

xo_buf_append(xbp, "", 1);

value = strtoull(xbp->xb_bufp + savep->xhs_offset, &ep, 0);
if (!(value == ULLONG_MAX && errno == ERANGE)
&& (ep != xbp->xb_bufp + savep->xhs_offset)) {





if (xo_buf_has_room(xbp, 10)) {
xbp->xb_curp = xbp->xb_bufp + savep->xhs_offset;

ssize_t rc;
ssize_t left = (xbp->xb_bufp + xbp->xb_size) - xbp->xb_curp;
int hn_flags = HN_NOSPACE;

if (flags & XFF_HN_SPACE)
hn_flags &= ~HN_NOSPACE;

if (flags & XFF_HN_DECIMAL)
hn_flags |= HN_DECIMAL;

if (flags & XFF_HN_1000)
hn_flags |= HN_DIVISOR_1000;

rc = xo_humanize(xbp->xb_curp, left, value, hn_flags);
if (rc > 0) {
xbp->xb_curp += rc;
xop->xo_columns = savep->xhs_columns + rc;
xop->xo_anchor_columns = savep->xhs_anchor_columns + rc;
}
}
}
}







static inline void
xo_simple_field (xo_handle_t *xop, unsigned encode_only,
const char *value, ssize_t vlen,
const char *fmt, ssize_t flen, xo_xff_flags_t flags)
{
if (encode_only)
flags |= XFF_NO_OUTPUT;

if (vlen == 0)
xo_do_format_field(xop, NULL, fmt, flen, flags);
else if (!encode_only)
xo_data_append_content(xop, value, vlen, flags);
}





static void
xo_buf_append_div (xo_handle_t *xop, const char *class, xo_xff_flags_t flags,
const char *name, ssize_t nlen,
const char *value, ssize_t vlen,
const char *fmt, ssize_t flen,
const char *encoding, ssize_t elen)
{
static char div_start[] = "<div class=\"";
static char div_tag[] = "\" data-tag=\"";
static char div_xpath[] = "\" data-xpath=\"";
static char div_key[] = "\" data-key=\"key";
static char div_end[] = "\">";
static char div_close[] = "</div>";


if (encoding == NULL && fmt != NULL) {
char *enc = alloca(flen + 1);
memcpy(enc, fmt, flen);
enc[flen] = '\0';
encoding = xo_fix_encoding(xop, enc);
elen = strlen(encoding);
}








int need_predidate =
(name && (flags & XFF_KEY) && !(flags & XFF_DISPLAY_ONLY)
&& XOF_ISSET(xop, XOF_XPATH)) ? 1 : 0;

if (need_predidate) {
va_list va_local;

va_copy(va_local, xop->xo_vap);
if (xop->xo_checkpointer)
xop->xo_checkpointer(xop, xop->xo_vap, 0);





xo_buffer_t *pbp = &xop->xo_predicate;
pbp->xb_curp = pbp->xb_bufp;

xo_buf_append(pbp, "[", 1);
xo_buf_escape(xop, pbp, name, nlen, 0);
if (XOF_ISSET(xop, XOF_PRETTY))
xo_buf_append(pbp, " = '", 4);
else
xo_buf_append(pbp, "='", 2);

xo_xff_flags_t pflags = flags | XFF_XML | XFF_ATTR;
pflags &= ~(XFF_NO_OUTPUT | XFF_ENCODE_ONLY);
xo_do_format_field(xop, pbp, encoding, elen, pflags);

xo_buf_append(pbp, "']", 2);


xo_stack_t *xsp = &xop->xo_stack[xop->xo_depth];
ssize_t olen = xsp->xs_keys ? strlen(xsp->xs_keys) : 0;
ssize_t dlen = pbp->xb_curp - pbp->xb_bufp;

char *cp = xo_realloc(xsp->xs_keys, olen + dlen + 1);
if (cp) {
memcpy(cp + olen, pbp->xb_bufp, dlen);
cp[olen + dlen] = '\0';
xsp->xs_keys = cp;
}


va_end(xop->xo_vap);
va_copy(xop->xo_vap, va_local);
va_end(va_local);
if (xop->xo_checkpointer)
xop->xo_checkpointer(xop, xop->xo_vap, 1);
}

if (flags & XFF_ENCODE_ONLY) {






xo_simple_field(xop, TRUE, NULL, 0, encoding, elen, flags);
return;
}

xo_line_ensure_open(xop, 0);

if (XOF_ISSET(xop, XOF_PRETTY))
xo_buf_indent(xop, xop->xo_indent_by);

xo_data_append(xop, div_start, sizeof(div_start) - 1);
xo_data_append(xop, class, strlen(class));





if (!xo_buf_is_empty(&xop->xo_color_buf)) {
xo_buffer_t *xbp = &xop->xo_color_buf;

xo_data_append(xop, xbp->xb_bufp, xbp->xb_curp - xbp->xb_bufp);
}

if (name) {
xo_data_append(xop, div_tag, sizeof(div_tag) - 1);
xo_data_escape(xop, name, nlen);




if (XOF_ISSET(xop, XOF_UNITS)) {
XOIF_SET(xop, XOIF_UNITS_PENDING);





xop->xo_units_offset =
xop->xo_data.xb_curp -xop->xo_data.xb_bufp + 1;
}

if (XOF_ISSET(xop, XOF_XPATH)) {
int i;
xo_stack_t *xsp;

xo_data_append(xop, div_xpath, sizeof(div_xpath) - 1);
if (xop->xo_leading_xpath)
xo_data_append(xop, xop->xo_leading_xpath,
strlen(xop->xo_leading_xpath));

for (i = 0; i <= xop->xo_depth; i++) {
xsp = &xop->xo_stack[i];
if (xsp->xs_name == NULL)
continue;






if (xsp->xs_state == XSS_OPEN_LIST
|| xsp->xs_state == XSS_OPEN_LEAF_LIST)
continue;

xo_data_append(xop, "/", 1);
xo_data_escape(xop, xsp->xs_name, strlen(xsp->xs_name));
if (xsp->xs_keys) {

if (i != xop->xo_depth || !(flags & XFF_KEY))
xo_data_append(xop, xsp->xs_keys, strlen(xsp->xs_keys));
}
}

xo_data_append(xop, "/", 1);
xo_data_escape(xop, name, nlen);
}

if (XOF_ISSET(xop, XOF_INFO) && xop->xo_info) {
static char in_type[] = "\" data-type=\"";
static char in_help[] = "\" data-help=\"";

xo_info_t *xip = xo_info_find(xop, name, nlen);
if (xip) {
if (xip->xi_type) {
xo_data_append(xop, in_type, sizeof(in_type) - 1);
xo_data_escape(xop, xip->xi_type, strlen(xip->xi_type));
}
if (xip->xi_help) {
xo_data_append(xop, in_help, sizeof(in_help) - 1);
xo_data_escape(xop, xip->xi_help, strlen(xip->xi_help));
}
}
}

if ((flags & XFF_KEY) && XOF_ISSET(xop, XOF_KEYS))
xo_data_append(xop, div_key, sizeof(div_key) - 1);
}

xo_buffer_t *xbp = &xop->xo_data;
ssize_t base_offset = xbp->xb_curp - xbp->xb_bufp;

xo_data_append(xop, div_end, sizeof(div_end) - 1);

xo_humanize_save_t save;

save.xhs_offset = xbp->xb_curp - xbp->xb_bufp;
save.xhs_columns = xop->xo_columns;
save.xhs_anchor_columns = xop->xo_anchor_columns;

xo_simple_field(xop, FALSE, value, vlen, fmt, flen, flags);

if (flags & XFF_HUMANIZE) {




static const char div_number[] = "\" data-number=\"";
ssize_t div_len = sizeof(div_number) - 1;

ssize_t end_offset = xbp->xb_curp - xbp->xb_bufp;
ssize_t olen = end_offset - save.xhs_offset;

char *cp = alloca(olen + 1);
memcpy(cp, xbp->xb_bufp + save.xhs_offset, olen);
cp[olen] = '\0';

xo_format_humanize(xop, xbp, &save, flags);

if (xo_buf_has_room(xbp, div_len + olen)) {
ssize_t new_offset = xbp->xb_curp - xbp->xb_bufp;



memmove(xbp->xb_bufp + base_offset + div_len + olen,
xbp->xb_bufp + base_offset, new_offset - base_offset);


memcpy(xbp->xb_bufp + base_offset, div_number, div_len);


memcpy(xbp->xb_bufp + base_offset + div_len, cp, olen);
xbp->xb_curp += div_len + olen;
}
}

xo_data_append(xop, div_close, sizeof(div_close) - 1);

if (XOF_ISSET(xop, XOF_PRETTY))
xo_data_append(xop, "\n", 1);
}

static void
xo_format_text (xo_handle_t *xop, const char *str, ssize_t len)
{
switch (xo_style(xop)) {
case XO_STYLE_TEXT:
xo_buf_append_locale(xop, &xop->xo_data, str, len);
break;

case XO_STYLE_HTML:
xo_buf_append_div(xop, "text", 0, NULL, 0, str, len, NULL, 0, NULL, 0);
break;
}
}

static void
xo_format_title (xo_handle_t *xop, xo_field_info_t *xfip,
const char *value, ssize_t vlen)
{
const char *fmt = xfip->xfi_format;
ssize_t flen = xfip->xfi_flen;
xo_xff_flags_t flags = xfip->xfi_flags;

static char div_open[] = "<div class=\"title";
static char div_middle[] = "\">";
static char div_close[] = "</div>";

if (flen == 0) {
fmt = "%s";
flen = 2;
}

switch (xo_style(xop)) {
case XO_STYLE_XML:
case XO_STYLE_JSON:
case XO_STYLE_SDPARAMS:
case XO_STYLE_ENCODER:




xo_simple_field(xop, TRUE, value, vlen, fmt, flen, flags);
return;
}

xo_buffer_t *xbp = &xop->xo_data;
ssize_t start = xbp->xb_curp - xbp->xb_bufp;
ssize_t left = xbp->xb_size - start;
ssize_t rc;

if (xo_style(xop) == XO_STYLE_HTML) {
xo_line_ensure_open(xop, 0);
if (XOF_ISSET(xop, XOF_PRETTY))
xo_buf_indent(xop, xop->xo_indent_by);
xo_buf_append(&xop->xo_data, div_open, sizeof(div_open) - 1);
xo_color_append_html(xop);
xo_buf_append(&xop->xo_data, div_middle, sizeof(div_middle) - 1);
}

start = xbp->xb_curp - xbp->xb_bufp;
if (vlen) {
char *newfmt = alloca(flen + 1);
memcpy(newfmt, fmt, flen);
newfmt[flen] = '\0';


char *newstr = alloca(vlen + 1);
memcpy(newstr, value, vlen);
newstr[vlen] = '\0';

if (newstr[vlen - 1] == 's') {
char *bp;

rc = snprintf(NULL, 0, newfmt, newstr);
if (rc > 0) {




bp = alloca(rc + 1);
rc = snprintf(bp, rc + 1, newfmt, newstr);

xo_data_append_content(xop, bp, rc, flags);
}
goto move_along;

} else {
rc = snprintf(xbp->xb_curp, left, newfmt, newstr);
if (rc >= left) {
if (!xo_buf_has_room(xbp, rc))
return;
left = xbp->xb_size - (xbp->xb_curp - xbp->xb_bufp);
rc = snprintf(xbp->xb_curp, left, newfmt, newstr);
}

if (rc > 0) {
if (XOF_ISSET(xop, XOF_COLUMNS))
xop->xo_columns += rc;
if (XOIF_ISSET(xop, XOIF_ANCHOR))
xop->xo_anchor_columns += rc;
}
}

} else {
xo_do_format_field(xop, NULL, fmt, flen, flags);


rc = xbp->xb_curp - (xbp->xb_bufp + start);
xbp->xb_curp = xbp->xb_bufp + start;
}


if (xo_style(xop) == XO_STYLE_HTML) {
rc = xo_escape_xml(xbp, rc, 0);
}

if (rc > 0)
xbp->xb_curp += rc;

move_along:
if (xo_style(xop) == XO_STYLE_HTML) {
xo_data_append(xop, div_close, sizeof(div_close) - 1);
if (XOF_ISSET(xop, XOF_PRETTY))
xo_data_append(xop, "\n", 1);
}
}




static ssize_t
xo_strnspn (const char *str, size_t len, const char *accept)
{
ssize_t i;
const char *cp, *ep;

for (i = 0, cp = str, ep = str + len; cp < ep && *cp != '\0'; i++, cp++) {
if (strchr(accept, *cp) == NULL)
break;
}

return i;
}








static int
xo_format_is_numeric (const char *fmt, ssize_t flen)
{
if (flen <= 0 || *fmt++ != '%')
return FALSE;
flen -= 1;


ssize_t spn = xo_strnspn(fmt, flen, "0123456789.*+ -");
if (spn >= flen)
return FALSE;

fmt += spn;
flen -= spn;


spn = xo_strnspn(fmt, flen, "hljtqz");
if (spn >= flen)
return FALSE;

fmt += spn;
flen -= spn;

if (flen != 1)
return FALSE;

return (strchr("diouDOUeEfFgG", *fmt) == NULL) ? FALSE : TRUE;
}





static void
xo_stack_set_flags (xo_handle_t *xop)
{
if (XOF_ISSET(xop, XOF_NOT_FIRST)) {
xo_stack_t *xsp = &xop->xo_stack[xop->xo_depth];

xsp->xs_flags |= XSF_NOT_FIRST;
XOF_CLEAR(xop, XOF_NOT_FIRST);
}
}

static void
xo_format_prep (xo_handle_t *xop, xo_xff_flags_t flags)
{
if (xop->xo_stack[xop->xo_depth].xs_flags & XSF_NOT_FIRST) {
xo_data_append(xop, ",", 1);
if (!(flags & XFF_LEAF_LIST) && XOF_ISSET(xop, XOF_PRETTY))
xo_data_append(xop, "\n", 1);
} else
xop->xo_stack[xop->xo_depth].xs_flags |= XSF_NOT_FIRST;
}

#if 0

void
xo_arg (xo_handle_t *xop);
void
xo_arg (xo_handle_t *xop)
{
xop = xo_default(xop);
fprintf(stderr, "0x%x", va_arg(xop->xo_vap, unsigned));
}
#endif

static void
xo_format_value (xo_handle_t *xop, const char *name, ssize_t nlen,
const char *value, ssize_t vlen,
const char *fmt, ssize_t flen,
const char *encoding, ssize_t elen, xo_xff_flags_t flags)
{
int pretty = XOF_ISSET(xop, XOF_PRETTY);
int quote;




xo_stack_t *xsp = &xop->xo_stack[xop->xo_depth];

if (flags & XFF_LEAF_LIST) {




if ((xsp->xs_flags & (XSF_EMIT | XSF_EMIT_KEY))
|| !(xsp->xs_flags & XSF_EMIT_LEAF_LIST)) {
char nbuf[nlen + 1];
memcpy(nbuf, name, nlen);
nbuf[nlen] = '\0';

ssize_t rc = xo_transition(xop, 0, nbuf, XSS_EMIT_LEAF_LIST);
if (rc < 0)
flags |= XFF_DISPLAY_ONLY | XFF_ENCODE_ONLY;
else
xop->xo_stack[xop->xo_depth].xs_flags |= XSF_EMIT_LEAF_LIST;
}

xsp = &xop->xo_stack[xop->xo_depth];
if (xsp->xs_name) {
name = xsp->xs_name;
nlen = strlen(name);
}

} else if (flags & XFF_KEY) {

if ((xsp->xs_flags & XSF_EMIT) && !(flags & XFF_DISPLAY_ONLY)) {
xo_failure(xop, "key field emitted after normal value field: '%.*s'",
nlen, name);

} else if (!(xsp->xs_flags & XSF_EMIT_KEY)) {
char nbuf[nlen + 1];
memcpy(nbuf, name, nlen);
nbuf[nlen] = '\0';

ssize_t rc = xo_transition(xop, 0, nbuf, XSS_EMIT);
if (rc < 0)
flags |= XFF_DISPLAY_ONLY | XFF_ENCODE_ONLY;
else
xop->xo_stack[xop->xo_depth].xs_flags |= XSF_EMIT_KEY;

xsp = &xop->xo_stack[xop->xo_depth];
xsp->xs_flags |= XSF_EMIT_KEY;
}

} else {

if ((xsp->xs_flags & XSF_EMIT_LEAF_LIST)
|| !(xsp->xs_flags & XSF_EMIT)) {
char nbuf[nlen + 1];
memcpy(nbuf, name, nlen);
nbuf[nlen] = '\0';

ssize_t rc = xo_transition(xop, 0, nbuf, XSS_EMIT);
if (rc < 0)
flags |= XFF_DISPLAY_ONLY | XFF_ENCODE_ONLY;
else
xop->xo_stack[xop->xo_depth].xs_flags |= XSF_EMIT;

xsp = &xop->xo_stack[xop->xo_depth];
xsp->xs_flags |= XSF_EMIT;
}
}

xo_buffer_t *xbp = &xop->xo_data;
xo_humanize_save_t save;

const char *leader = xo_xml_leader_len(xop, name, nlen);

switch (xo_style(xop)) {
case XO_STYLE_TEXT:
if (flags & XFF_ENCODE_ONLY)
flags |= XFF_NO_OUTPUT;

save.xhs_offset = xbp->xb_curp - xbp->xb_bufp;
save.xhs_columns = xop->xo_columns;
save.xhs_anchor_columns = xop->xo_anchor_columns;

xo_simple_field(xop, FALSE, value, vlen, fmt, flen, flags);

if (flags & XFF_HUMANIZE)
xo_format_humanize(xop, xbp, &save, flags);
break;

case XO_STYLE_HTML:
if (flags & XFF_ENCODE_ONLY)
flags |= XFF_NO_OUTPUT;

xo_buf_append_div(xop, "data", flags, name, nlen, value, vlen,
fmt, flen, encoding, elen);
break;

case XO_STYLE_XML:




if (flags & XFF_DISPLAY_ONLY) {
xo_simple_field(xop, TRUE, value, vlen, fmt, flen, flags);
break;
}

if (encoding) {
fmt = encoding;
flen = elen;
} else {
char *enc = alloca(flen + 1);
memcpy(enc, fmt, flen);
enc[flen] = '\0';
fmt = xo_fix_encoding(xop, enc);
flen = strlen(fmt);
}

if (nlen == 0) {
static char missing[] = "missing-field-name";
xo_failure(xop, "missing field name: %s", fmt);
name = missing;
nlen = sizeof(missing) - 1;
}

if (pretty)
xo_buf_indent(xop, -1);
xo_data_append(xop, "<", 1);
if (*leader)
xo_data_append(xop, leader, 1);
xo_data_escape(xop, name, nlen);

if (xop->xo_attrs.xb_curp != xop->xo_attrs.xb_bufp) {
xo_data_append(xop, xop->xo_attrs.xb_bufp,
xop->xo_attrs.xb_curp - xop->xo_attrs.xb_bufp);
xop->xo_attrs.xb_curp = xop->xo_attrs.xb_bufp;
}







if ((flags & XFF_KEY) && XOF_ISSET(xop, XOF_KEYS)) {
static char attr[] = " key=\"key\"";
xo_data_append(xop, attr, sizeof(attr) - 1);
}




if (XOF_ISSET(xop, XOF_UNITS)) {
XOIF_SET(xop, XOIF_UNITS_PENDING);
xop->xo_units_offset = xop->xo_data.xb_curp -xop->xo_data.xb_bufp;
}

xo_data_append(xop, ">", 1);

xo_simple_field(xop, FALSE, value, vlen, fmt, flen, flags);

xo_data_append(xop, "</", 2);
if (*leader)
xo_data_append(xop, leader, 1);
xo_data_escape(xop, name, nlen);
xo_data_append(xop, ">", 1);
if (pretty)
xo_data_append(xop, "\n", 1);
break;

case XO_STYLE_JSON:
if (flags & XFF_DISPLAY_ONLY) {
xo_simple_field(xop, TRUE, value, vlen, fmt, flen, flags);
break;
}

if (encoding) {
fmt = encoding;
flen = elen;
} else {
char *enc = alloca(flen + 1);
memcpy(enc, fmt, flen);
enc[flen] = '\0';
fmt = xo_fix_encoding(xop, enc);
flen = strlen(fmt);
}

xo_stack_set_flags(xop);

int first = (xop->xo_stack[xop->xo_depth].xs_flags & XSF_NOT_FIRST)
? 0 : 1;

xo_format_prep(xop, flags);

if (flags & XFF_QUOTE)
quote = 1;
else if (flags & XFF_NOQUOTE)
quote = 0;
else if (vlen != 0)
quote = 1;
else if (flen == 0) {
quote = 0;
fmt = "true";
flen = 4;
} else if (xo_format_is_numeric(fmt, flen))
quote = 0;
else
quote = 1;

if (nlen == 0) {
static char missing[] = "missing-field-name";
xo_failure(xop, "missing field name: %s", fmt);
name = missing;
nlen = sizeof(missing) - 1;
}

if (flags & XFF_LEAF_LIST) {
if (!first && pretty)
xo_data_append(xop, "\n", 1);
if (pretty)
xo_buf_indent(xop, -1);
} else {
if (pretty)
xo_buf_indent(xop, -1);
xo_data_append(xop, "\"", 1);

xbp = &xop->xo_data;
ssize_t off = xbp->xb_curp - xbp->xb_bufp;

xo_data_escape(xop, name, nlen);

if (XOF_ISSET(xop, XOF_UNDERSCORES)) {
ssize_t coff = xbp->xb_curp - xbp->xb_bufp;
for ( ; off < coff; off++)
if (xbp->xb_bufp[off] == '-')
xbp->xb_bufp[off] = '_';
}
xo_data_append(xop, "\":", 2);
if (pretty)
xo_data_append(xop, " ", 1);
}

if (quote)
xo_data_append(xop, "\"", 1);

xo_simple_field(xop, FALSE, value, vlen, fmt, flen, flags);

if (quote)
xo_data_append(xop, "\"", 1);
break;

case XO_STYLE_SDPARAMS:
if (flags & XFF_DISPLAY_ONLY) {
xo_simple_field(xop, TRUE, value, vlen, fmt, flen, flags);
break;
}

if (encoding) {
fmt = encoding;
flen = elen;
} else {
char *enc = alloca(flen + 1);
memcpy(enc, fmt, flen);
enc[flen] = '\0';
fmt = xo_fix_encoding(xop, enc);
flen = strlen(fmt);
}

if (nlen == 0) {
static char missing[] = "missing-field-name";
xo_failure(xop, "missing field name: %s", fmt);
name = missing;
nlen = sizeof(missing) - 1;
}

xo_data_escape(xop, name, nlen);
xo_data_append(xop, "=\"", 2);

xo_simple_field(xop, FALSE, value, vlen, fmt, flen, flags);

xo_data_append(xop, "\" ", 2);
break;

case XO_STYLE_ENCODER:
if (flags & XFF_DISPLAY_ONLY) {
xo_simple_field(xop, TRUE, value, vlen, fmt, flen, flags);
break;
}

if (flags & XFF_QUOTE)
quote = 1;
else if (flags & XFF_NOQUOTE)
quote = 0;
else if (flen == 0) {
quote = 0;
fmt = "true";
flen = 4;
} else if (strchr("diouxXDOUeEfFgGaAcCp", fmt[flen - 1]) == NULL)
quote = 1;
else
quote = 0;

if (encoding) {
fmt = encoding;
flen = elen;
} else {
char *enc = alloca(flen + 1);
memcpy(enc, fmt, flen);
enc[flen] = '\0';
fmt = xo_fix_encoding(xop, enc);
flen = strlen(fmt);
}

if (nlen == 0) {
static char missing[] = "missing-field-name";
xo_failure(xop, "missing field name: %s", fmt);
name = missing;
nlen = sizeof(missing) - 1;
}

ssize_t name_offset = xo_buf_offset(&xop->xo_data);
xo_data_append(xop, name, nlen);
xo_data_append(xop, "", 1);

ssize_t value_offset = xo_buf_offset(&xop->xo_data);

xo_simple_field(xop, FALSE, value, vlen, fmt, flen, flags);

xo_data_append(xop, "", 1);

xo_encoder_handle(xop, quote ? XO_OP_STRING : XO_OP_CONTENT,
xo_buf_data(&xop->xo_data, name_offset),
xo_buf_data(&xop->xo_data, value_offset), flags);
xo_buf_reset(&xop->xo_data);
break;
}
}

static void
xo_set_gettext_domain (xo_handle_t *xop, xo_field_info_t *xfip,
const char *str, ssize_t len)
{
const char *fmt = xfip->xfi_format;
ssize_t flen = xfip->xfi_flen;


if (xop->xo_gt_domain) {
xo_free(xop->xo_gt_domain);
xop->xo_gt_domain = NULL;
}


if (len == 0 && flen == 0)
return;

ssize_t start_offset = -1;
if (len == 0 && flen != 0) {

start_offset = xop->xo_data.xb_curp - xop->xo_data.xb_bufp;
xo_do_format_field(xop, NULL, fmt, flen, 0);

ssize_t end_offset = xop->xo_data.xb_curp - xop->xo_data.xb_bufp;
len = end_offset - start_offset;
str = xop->xo_data.xb_bufp + start_offset;
}

xop->xo_gt_domain = xo_strndup(str, len);


if (start_offset >= 0)
xop->xo_data.xb_curp = xop->xo_data.xb_bufp + start_offset;
}

static void
xo_format_content (xo_handle_t *xop, const char *class_name,
const char *tag_name,
const char *value, ssize_t vlen,
const char *fmt, ssize_t flen,
xo_xff_flags_t flags)
{
switch (xo_style(xop)) {
case XO_STYLE_TEXT:
xo_simple_field(xop, FALSE, value, vlen, fmt, flen, flags);
break;

case XO_STYLE_HTML:
xo_buf_append_div(xop, class_name, flags, NULL, 0,
value, vlen, fmt, flen, NULL, 0);
break;

case XO_STYLE_XML:
case XO_STYLE_JSON:
case XO_STYLE_SDPARAMS:
if (tag_name) {
xo_open_container_h(xop, tag_name);
xo_format_value(xop, "message", 7, value, vlen,
fmt, flen, NULL, 0, flags);
xo_close_container_h(xop, tag_name);

} else {




xo_simple_field(xop, TRUE, value, vlen, fmt, flen, flags);
}
break;

case XO_STYLE_ENCODER:
xo_simple_field(xop, TRUE, value, vlen, fmt, flen, flags);
break;
}
}

static const char *xo_color_names[] = {
"default",
"black",
"red",
"green",
"yellow",
"blue",
"magenta",
"cyan",
"white",
NULL
};

static int
xo_color_find (const char *str)
{
int i;

for (i = 0; xo_color_names[i]; i++) {
if (xo_streq(xo_color_names[i], str))
return i;
}

return -1;
}

static const char *xo_effect_names[] = {
"reset",
"normal",
"bold",
"underline",
"inverse",
NULL
};

static const char *xo_effect_on_codes[] = {
"0",
"0",
"1",
"4",
"7",
NULL
};

#if 0









static const char *xo_effect_off_codes[] = {
"0",
"0",
"21",
"24",
"27",
NULL
};
#endif

static int
xo_effect_find (const char *str)
{
int i;

for (i = 0; xo_effect_names[i]; i++) {
if (xo_streq(xo_effect_names[i], str))
return i;
}

return -1;
}

static void
xo_colors_parse (xo_handle_t *xop, xo_colors_t *xocp, char *str)
{
if (xo_text_only())
return;

char *cp, *ep, *np, *xp;
ssize_t len = strlen(str);
int rc;




for (cp = str, ep = cp + len - 1; cp && cp < ep; cp = np) {

while (isspace((int) *cp))
cp += 1;

np = strchr(cp, ',');
if (np)
*np++ = '\0';


xp = cp + strlen(cp) - 1;
while (isspace(*xp) && xp > cp)
*xp-- = '\0';

if (cp[0] == 'f' && cp[1] == 'g' && cp[2] == '-') {
rc = xo_color_find(cp + 3);
if (rc < 0)
goto unknown;

xocp->xoc_col_fg = rc;

} else if (cp[0] == 'b' && cp[1] == 'g' && cp[2] == '-') {
rc = xo_color_find(cp + 3);
if (rc < 0)
goto unknown;
xocp->xoc_col_bg = rc;

} else if (cp[0] == 'n' && cp[1] == 'o' && cp[2] == '-') {
rc = xo_effect_find(cp + 3);
if (rc < 0)
goto unknown;
xocp->xoc_effects &= ~(1 << rc);

} else {
rc = xo_effect_find(cp);
if (rc < 0)
goto unknown;
xocp->xoc_effects |= 1 << rc;

switch (1 << rc) {
case XO_EFF_RESET:
xocp->xoc_col_fg = xocp->xoc_col_bg = 0;

xocp->xoc_effects = XO_EFF_RESET;
break;

case XO_EFF_NORMAL:
xocp->xoc_effects &= ~(XO_EFF_BOLD | XO_EFF_UNDERLINE
| XO_EFF_INVERSE | XO_EFF_NORMAL);
break;
}
}
continue;

unknown:
if (XOF_ISSET(xop, XOF_WARN))
xo_failure(xop, "unknown color/effect string detected: '%s'", cp);
}
}

static inline int
xo_colors_enabled (xo_handle_t *xop UNUSED)
{
#if defined(LIBXO_TEXT_ONLY)
return 0;
#else
return XOF_ISSET(xop, XOF_COLOR);
#endif
}





static void
xo_colors_update (xo_handle_t *xop UNUSED, xo_colors_t *newp UNUSED)
{
#if !defined(LIBXO_TEXT_ONLY)
xo_color_t fg = newp->xoc_col_fg;
if (XOF_ISSET(xop, XOF_COLOR_MAP) && fg < XO_NUM_COLORS)
fg = xop->xo_color_map_fg[fg];
newp->xoc_col_fg = fg;

xo_color_t bg = newp->xoc_col_bg;
if (XOF_ISSET(xop, XOF_COLOR_MAP) && bg < XO_NUM_COLORS)
bg = xop->xo_color_map_bg[bg];
newp->xoc_col_bg = bg;
#endif
}

static void
xo_colors_handle_text (xo_handle_t *xop, xo_colors_t *newp)
{
char buf[BUFSIZ];
char *cp = buf, *ep = buf + sizeof(buf);
unsigned i, bit;
xo_colors_t *oldp = &xop->xo_colors;
const char *code = NULL;






*cp++ = 0x1b;









if (oldp->xoc_effects != (newp->xoc_effects & oldp->xoc_effects)) {
newp->xoc_effects |= XO_EFF_RESET;
oldp->xoc_effects = 0;
}

for (i = 0, bit = 1; xo_effect_names[i]; i++, bit <<= 1) {
if ((newp->xoc_effects & bit) == (oldp->xoc_effects & bit))
continue;

code = xo_effect_on_codes[i];

cp += snprintf(cp, ep - cp, ";%s", code);
if (cp >= ep)
return;

if (bit == XO_EFF_RESET) {

oldp->xoc_effects = 0;
oldp->xoc_col_fg = oldp->xoc_col_bg = XO_COL_DEFAULT;
}
}

xo_color_t fg = newp->xoc_col_fg;
if (fg != oldp->xoc_col_fg) {
cp += snprintf(cp, ep - cp, ";3%u",
(fg != XO_COL_DEFAULT) ? fg - 1 : 9);
}

xo_color_t bg = newp->xoc_col_bg;
if (bg != oldp->xoc_col_bg) {
cp += snprintf(cp, ep - cp, ";4%u",
(bg != XO_COL_DEFAULT) ? bg - 1 : 9);
}

if (cp - buf != 1 && cp < ep - 3) {
buf[1] = '[';
*cp++ = 'm';
*cp = '\0';
xo_buf_append(&xop->xo_data, buf, cp - buf);
}
}

static void
xo_colors_handle_html (xo_handle_t *xop, xo_colors_t *newp)
{
xo_colors_t *oldp = &xop->xo_colors;







if (oldp->xoc_effects == newp->xoc_effects
&& oldp->xoc_col_fg == newp->xoc_col_fg
&& oldp->xoc_col_bg == newp->xoc_col_bg)
return;

unsigned i, bit;
xo_buffer_t *xbp = &xop->xo_color_buf;

xo_buf_reset(xbp);

for (i = 0, bit = 1; xo_effect_names[i]; i++, bit <<= 1) {
if (!(newp->xoc_effects & bit))
continue;

xo_buf_append_str(xbp, " effect-");
xo_buf_append_str(xbp, xo_effect_names[i]);
}

const char *fg = NULL;
const char *bg = NULL;

if (newp->xoc_col_fg != XO_COL_DEFAULT)
fg = xo_color_names[newp->xoc_col_fg];
if (newp->xoc_col_bg != XO_COL_DEFAULT)
bg = xo_color_names[newp->xoc_col_bg];

if (newp->xoc_effects & XO_EFF_INVERSE) {
const char *tmp = fg;
fg = bg;
bg = tmp;
if (fg == NULL)
fg = "inverse";
if (bg == NULL)
bg = "inverse";

}

if (fg) {
xo_buf_append_str(xbp, " color-fg-");
xo_buf_append_str(xbp, fg);
}

if (bg) {
xo_buf_append_str(xbp, " color-bg-");
xo_buf_append_str(xbp, bg);
}
}

static void
xo_format_colors (xo_handle_t *xop, xo_field_info_t *xfip,
const char *value, ssize_t vlen)
{
const char *fmt = xfip->xfi_format;
ssize_t flen = xfip->xfi_flen;

xo_buffer_t xb;


if (vlen != 0 && xo_style_is_encoding(xop))
return;

xo_buf_init(&xb);

if (vlen)
xo_buf_append(&xb, value, vlen);
else if (flen)
xo_do_format_field(xop, &xb, fmt, flen, 0);
else
xo_buf_append(&xb, "reset", 6);

if (xo_colors_enabled(xop)) {
switch (xo_style(xop)) {
case XO_STYLE_TEXT:
case XO_STYLE_HTML:
xo_buf_append(&xb, "", 1);

xo_colors_t xoc = xop->xo_colors;
xo_colors_parse(xop, &xoc, xb.xb_bufp);
xo_colors_update(xop, &xoc);

if (xo_style(xop) == XO_STYLE_TEXT) {










xo_colors_handle_text(xop, &xoc);
xoc.xoc_effects &= ~XO_EFF_RESET;

} else {





xoc.xoc_effects &= ~XO_EFF_RESET;
xo_colors_handle_html(xop, &xoc);
}

xop->xo_colors = xoc;
break;

case XO_STYLE_XML:
case XO_STYLE_JSON:
case XO_STYLE_SDPARAMS:
case XO_STYLE_ENCODER:




break;
}
}

xo_buf_cleanup(&xb);
}

static void
xo_format_units (xo_handle_t *xop, xo_field_info_t *xfip,
const char *value, ssize_t vlen)
{
const char *fmt = xfip->xfi_format;
ssize_t flen = xfip->xfi_flen;
xo_xff_flags_t flags = xfip->xfi_flags;

static char units_start_xml[] = " units=\"";
static char units_start_html[] = " data-units=\"";

if (!XOIF_ISSET(xop, XOIF_UNITS_PENDING)) {
xo_format_content(xop, "units", NULL, value, vlen, fmt, flen, flags);
return;
}

xo_buffer_t *xbp = &xop->xo_data;
ssize_t start = xop->xo_units_offset;
ssize_t stop = xbp->xb_curp - xbp->xb_bufp;

if (xo_style(xop) == XO_STYLE_XML)
xo_buf_append(xbp, units_start_xml, sizeof(units_start_xml) - 1);
else if (xo_style(xop) == XO_STYLE_HTML)
xo_buf_append(xbp, units_start_html, sizeof(units_start_html) - 1);
else
return;

if (vlen)
xo_data_escape(xop, value, vlen);
else
xo_do_format_field(xop, NULL, fmt, flen, flags);

xo_buf_append(xbp, "\"", 1);

ssize_t now = xbp->xb_curp - xbp->xb_bufp;
ssize_t delta = now - stop;
if (delta <= 0) {
xbp->xb_curp = xbp->xb_bufp + stop;
return;
}







char *buf = alloca(delta);

memcpy(buf, xbp->xb_bufp + stop, delta);
memmove(xbp->xb_bufp + start + delta, xbp->xb_bufp + start, stop - start);
memmove(xbp->xb_bufp + start, buf, delta);
}

static ssize_t
xo_find_width (xo_handle_t *xop, xo_field_info_t *xfip,
const char *value, ssize_t vlen)
{
const char *fmt = xfip->xfi_format;
ssize_t flen = xfip->xfi_flen;

long width = 0;
char *bp;
char *cp;

if (vlen) {
bp = alloca(vlen + 1);
memcpy(bp, value, vlen);
bp[vlen] = '\0';

width = strtol(bp, &cp, 0);
if (width == LONG_MIN || width == LONG_MAX || bp == cp || *cp != '\0') {
width = 0;
xo_failure(xop, "invalid width for anchor: '%s'", bp);
}
} else if (flen) {






if (xop->xo_formatter == NULL && flen == 2
&& strncmp("%d", fmt, flen) == 0) {
if (!XOF_ISSET(xop, XOF_NO_VA_ARG))
width = va_arg(xop->xo_vap, int);
} else if (xop->xo_formatter == NULL && flen == 2
&& strncmp("%u", fmt, flen) == 0) {
if (!XOF_ISSET(xop, XOF_NO_VA_ARG))
width = va_arg(xop->xo_vap, unsigned);
} else {






int anchor_was_set = FALSE;
xo_buffer_t *xbp = &xop->xo_data;
ssize_t start_offset = xo_buf_offset(xbp);
bp = xo_buf_cur(xbp);
cp = NULL;

if (XOIF_ISSET(xop, XOIF_ANCHOR)) {
XOIF_CLEAR(xop, XOIF_ANCHOR);
anchor_was_set = TRUE;
}

ssize_t rc = xo_do_format_field(xop, xbp, fmt, flen, 0);
if (rc >= 0) {
xo_buf_append(xbp, "", 1);

width = strtol(bp, &cp, 0);
if (width == LONG_MIN || width == LONG_MAX
|| bp == cp || *cp != '\0') {
width = 0;
xo_failure(xop, "invalid width for anchor: '%s'", bp);
}
}


xbp->xb_curp = xbp->xb_bufp + start_offset;
if (anchor_was_set)
XOIF_SET(xop, XOIF_ANCHOR);
}
}

return width;
}

static void
xo_anchor_clear (xo_handle_t *xop)
{
XOIF_CLEAR(xop, XOIF_ANCHOR);
xop->xo_anchor_offset = 0;
xop->xo_anchor_columns = 0;
xop->xo_anchor_min_width = 0;
}









static void
xo_anchor_start (xo_handle_t *xop, xo_field_info_t *xfip,
const char *value, ssize_t vlen)
{
if (XOIF_ISSET(xop, XOIF_ANCHOR))
xo_failure(xop, "the anchor already recording is discarded");

XOIF_SET(xop, XOIF_ANCHOR);
xo_buffer_t *xbp = &xop->xo_data;
xop->xo_anchor_offset = xbp->xb_curp - xbp->xb_bufp;
xop->xo_anchor_columns = 0;





xop->xo_anchor_min_width = xo_find_width(xop, xfip, value, vlen);
}

static void
xo_anchor_stop (xo_handle_t *xop, xo_field_info_t *xfip,
const char *value, ssize_t vlen)
{
if (!XOIF_ISSET(xop, XOIF_ANCHOR)) {
xo_failure(xop, "no start anchor");
return;
}

XOIF_CLEAR(xop, XOIF_UNITS_PENDING);

ssize_t width = xo_find_width(xop, xfip, value, vlen);
if (width == 0)
width = xop->xo_anchor_min_width;

if (width == 0)
goto done;

xo_buffer_t *xbp = &xop->xo_data;
ssize_t start = xop->xo_anchor_offset;
ssize_t stop = xbp->xb_curp - xbp->xb_bufp;
ssize_t abswidth = (width > 0) ? width : -width;
ssize_t blen = abswidth - xop->xo_anchor_columns;

if (blen <= 0)
goto done;

if (abswidth > XO_MAX_ANCHOR_WIDTH) {
xo_failure(xop, "width over %u are not supported",
XO_MAX_ANCHOR_WIDTH);
goto done;
}


char *buf = alloca(blen);
memset(buf, ' ', blen);
xo_format_content(xop, "padding", NULL, buf, blen, NULL, 0, 0);

if (width < 0)
goto done;

ssize_t now = xbp->xb_curp - xbp->xb_bufp;
ssize_t delta = now - stop;
if (delta <= 0)
goto done;








if (delta > blen)
buf = alloca(delta);

memcpy(buf, xbp->xb_bufp + stop, delta);
memmove(xbp->xb_bufp + start + delta, xbp->xb_bufp + start, stop - start);
memmove(xbp->xb_bufp + start, buf, delta);

done:
xo_anchor_clear(xop);
}

static const char *
xo_class_name (int ftype)
{
switch (ftype) {
case 'D': return "decoration";
case 'E': return "error";
case 'L': return "label";
case 'N': return "note";
case 'P': return "padding";
case 'W': return "warning";
}

return NULL;
}

static const char *
xo_tag_name (int ftype)
{
switch (ftype) {
case 'E': return "__error";
case 'W': return "__warning";
}

return NULL;
}

static int
xo_role_wants_default_format (int ftype)
{
switch (ftype) {

case 'C':
case 'G':
case '[':
case ']':
return 0;
}

return 1;
}

static xo_mapping_t xo_role_names[] = {
{ 'C', "color" },
{ 'D', "decoration" },
{ 'E', "error" },
{ 'L', "label" },
{ 'N', "note" },
{ 'P', "padding" },
{ 'T', "title" },
{ 'U', "units" },
{ 'V', "value" },
{ 'W', "warning" },
{ '[', "start-anchor" },
{ ']', "stop-anchor" },
{ 0, NULL }
};

#define XO_ROLE_EBRACE '{'
#define XO_ROLE_TEXT '+'
#define XO_ROLE_NEWLINE '\n'

static xo_mapping_t xo_modifier_names[] = {
{ XFF_ARGUMENT, "argument" },
{ XFF_COLON, "colon" },
{ XFF_COMMA, "comma" },
{ XFF_DISPLAY_ONLY, "display" },
{ XFF_ENCODE_ONLY, "encoding" },
{ XFF_GT_FIELD, "gettext" },
{ XFF_HUMANIZE, "humanize" },
{ XFF_HUMANIZE, "hn" },
{ XFF_HN_SPACE, "hn-space" },
{ XFF_HN_DECIMAL, "hn-decimal" },
{ XFF_HN_1000, "hn-1000" },
{ XFF_KEY, "key" },
{ XFF_LEAF_LIST, "leaf-list" },
{ XFF_LEAF_LIST, "list" },
{ XFF_NOQUOTE, "no-quotes" },
{ XFF_NOQUOTE, "no-quote" },
{ XFF_GT_PLURAL, "plural" },
{ XFF_QUOTE, "quotes" },
{ XFF_QUOTE, "quote" },
{ XFF_TRIM_WS, "trim" },
{ XFF_WS, "white" },
{ 0, NULL }
};

#if defined(NOT_NEEDED_YET)
static xo_mapping_t xo_modifier_short_names[] = {
{ XFF_COLON, "c" },
{ XFF_DISPLAY_ONLY, "d" },
{ XFF_ENCODE_ONLY, "e" },
{ XFF_GT_FIELD, "g" },
{ XFF_HUMANIZE, "h" },
{ XFF_KEY, "k" },
{ XFF_LEAF_LIST, "l" },
{ XFF_NOQUOTE, "n" },
{ XFF_GT_PLURAL, "p" },
{ XFF_QUOTE, "q" },
{ XFF_TRIM_WS, "t" },
{ XFF_WS, "w" },
{ 0, NULL }
};
#endif

static int
xo_count_fields (xo_handle_t *xop UNUSED, const char *fmt)
{
int rc = 1;
const char *cp;

for (cp = fmt; *cp; cp++)
if (*cp == '{' || *cp == '\n')
rc += 1;

return rc * 2 + 1;
}




































static const char *
xo_parse_roles (xo_handle_t *xop, const char *fmt,
const char *basep, xo_field_info_t *xfip)
{
const char *sp;
unsigned ftype = 0;
xo_xff_flags_t flags = 0;
uint8_t fnum = 0;

for (sp = basep; sp && *sp; sp++) {
if (*sp == ':' || *sp == '/' || *sp == '}')
break;

if (*sp == '\\') {
if (sp[1] == '\0') {
xo_failure(xop, "backslash at the end of string");
return NULL;
}


sp += 1;
continue;
}

if (*sp == ',') {
const char *np;
for (np = ++sp; *np; np++)
if (*np == ':' || *np == '/' || *np == '}' || *np == ',')
break;

ssize_t slen = np - sp;
if (slen > 0) {
xo_xff_flags_t value;

value = xo_name_lookup(xo_role_names, sp, slen);
if (value)
ftype = value;
else {
value = xo_name_lookup(xo_modifier_names, sp, slen);
if (value)
flags |= value;
else
xo_failure(xop, "unknown keyword ignored: '%.*s'",
slen, sp);
}
}

sp = np - 1;
continue;
}

switch (*sp) {
case 'C':
case 'D':
case 'E':
case 'G':
case 'L':
case 'N':
case 'P':
case 'T':
case 'U':
case 'V':
case 'W':
case '[':
case ']':
if (ftype != 0) {
xo_failure(xop, "field descriptor uses multiple types: '%s'",
xo_printable(fmt));
return NULL;
}
ftype = *sp;
break;

case '0':
case '1':
case '2':
case '3':
case '4':
case '5':
case '6':
case '7':
case '8':
case '9':
fnum = (fnum * 10) + (*sp - '0');
break;

case 'a':
flags |= XFF_ARGUMENT;
break;

case 'c':
flags |= XFF_COLON;
break;

case 'd':
flags |= XFF_DISPLAY_ONLY;
break;

case 'e':
flags |= XFF_ENCODE_ONLY;
break;

case 'g':
flags |= XFF_GT_FIELD;
break;

case 'h':
flags |= XFF_HUMANIZE;
break;

case 'k':
flags |= XFF_KEY;
break;

case 'l':
flags |= XFF_LEAF_LIST;
break;

case 'n':
flags |= XFF_NOQUOTE;
break;

case 'p':
flags |= XFF_GT_PLURAL;
break;

case 'q':
flags |= XFF_QUOTE;
break;

case 't':
flags |= XFF_TRIM_WS;
break;

case 'w':
flags |= XFF_WS;
break;

default:
xo_failure(xop, "field descriptor uses unknown modifier: '%s'",
xo_printable(fmt));






return NULL;
}

if (ftype == 'N' || ftype == 'U') {
if (flags & XFF_COLON) {
xo_failure(xop, "colon modifier on 'N' or 'U' field ignored: "
"'%s'", xo_printable(fmt));
flags &= ~XFF_COLON;
}
}
}

xfip->xfi_flags = flags;
xfip->xfi_ftype = ftype ?: 'V';
xfip->xfi_fnum = fnum;

return sp;
}





static void
xo_gettext_finish_numbering_fields (xo_handle_t *xop UNUSED,
const char *fmt UNUSED,
xo_field_info_t *fields)
{
xo_field_info_t *xfip;
unsigned fnum, max_fields;
uint64_t bits = 0;
const uint64_t one = 1;


for (xfip = fields, fnum = 0; xfip->xfi_ftype; xfip++) {
switch (xfip->xfi_ftype) {
case XO_ROLE_NEWLINE:
case XO_ROLE_TEXT:
case XO_ROLE_EBRACE:
case 'G':
continue;
}

fnum += 1;
if (fnum >= 63)
break;

if (xfip->xfi_fnum)
bits |= one << xfip->xfi_fnum;
}

max_fields = fnum;

for (xfip = fields, fnum = 0; xfip->xfi_ftype; xfip++) {
switch (xfip->xfi_ftype) {
case XO_ROLE_NEWLINE:
case XO_ROLE_TEXT:
case XO_ROLE_EBRACE:
case 'G':
continue;
}

if (xfip->xfi_fnum != 0)
continue;


for (fnum++; bits & (one << fnum); fnum++)
continue;

if (fnum > max_fields)
break;

xfip->xfi_fnum = fnum;
bits |= one << fnum;
}
}





static int
xo_parse_field_numbers (xo_handle_t *xop, const char *fmt,
xo_field_info_t *fields, unsigned num_fields)
{
xo_field_info_t *xfip;
unsigned field, fnum;
uint64_t bits = 0;
const uint64_t one = 1;

for (xfip = fields, field = 0; field < num_fields; xfip++, field++) {

if (xfip->xfi_fnum == 0)
xfip->xfi_fnum = field + 1;
else if (xfip->xfi_fnum > num_fields) {
xo_failure(xop, "field number exceeds number of fields: '%s'", fmt);
return -1;
}

fnum = xfip->xfi_fnum - 1;
if (fnum < 64) {
if (bits & (one << fnum)) {
xo_failure(xop, "field number %u reused: '%s'",
xfip->xfi_fnum, fmt);
return -1;
}
bits |= one << fnum;
}
}

return 0;
}

static int
xo_parse_fields (xo_handle_t *xop, xo_field_info_t *fields,
unsigned num_fields, const char *fmt)
{
const char *cp, *sp, *ep, *basep;
unsigned field = 0;
xo_field_info_t *xfip = fields;
unsigned seen_fnum = 0;

for (cp = fmt; *cp && field < num_fields; field++, xfip++) {
xfip->xfi_start = cp;

if (*cp == '\n') {
xfip->xfi_ftype = XO_ROLE_NEWLINE;
xfip->xfi_len = 1;
cp += 1;
continue;
}

if (*cp != '{') {

for (sp = cp; *sp; sp++) {
if (*sp == '{' || *sp == '\n')
break;
}

xfip->xfi_ftype = XO_ROLE_TEXT;
xfip->xfi_content = cp;
xfip->xfi_clen = sp - cp;
xfip->xfi_next = sp;

cp = sp;
continue;
}

if (cp[1] == '{') {
xfip->xfi_start = cp + 1;
xfip->xfi_ftype = XO_ROLE_EBRACE;

cp += 2;
for (sp = cp; *sp; sp++) {
if (*sp == '}' && sp[1] == '}')
break;
}
if (*sp == '\0') {
xo_failure(xop, "missing closing '}}': '%s'",
xo_printable(fmt));
return -1;
}

xfip->xfi_len = sp - xfip->xfi_start + 1;


if (*sp == '}' && sp[1] == '}')
sp += 2;

cp = sp;
xfip->xfi_next = cp;
continue;
}


xfip->xfi_start = basep = cp + 1;

const char *format = NULL;
ssize_t flen = 0;


sp = xo_parse_roles(xop, fmt, basep, xfip);
if (sp == NULL) {

return -1;
}

if (xfip->xfi_fnum)
seen_fnum = 1;


if (*sp == ':') {
for (ep = ++sp; *sp; sp++) {
if (*sp == '}' || *sp == '/')
break;
if (*sp == '\\') {
if (sp[1] == '\0') {
xo_failure(xop, "backslash at the end of string");
return -1;
}
sp += 1;
continue;
}
}
if (ep != sp) {
xfip->xfi_clen = sp - ep;
xfip->xfi_content = ep;
}
} else {
xo_failure(xop, "missing content (':'): '%s'", xo_printable(fmt));
return -1;
}


if (*sp == '/') {
for (ep = ++sp; *sp; sp++) {
if (*sp == '}' || *sp == '/')
break;
if (*sp == '\\') {
if (sp[1] == '\0') {
xo_failure(xop, "backslash at the end of string");
return -1;
}
sp += 1;
continue;
}
}
flen = sp - ep;
format = ep;
}


if (*sp == '/') {
for (ep = ++sp; *sp; sp++) {
if (*sp == '}')
break;
}

xfip->xfi_encoding = ep;
xfip->xfi_elen = sp - ep;
}

if (*sp != '}') {
xo_failure(xop, "missing closing '}': %s", xo_printable(fmt));
return -1;
}

xfip->xfi_len = sp - xfip->xfi_start;
xfip->xfi_next = ++sp;


if (xfip->xfi_clen || format || (xfip->xfi_flags & XFF_ARGUMENT)) {
if (format) {
xfip->xfi_format = format;
xfip->xfi_flen = flen;
} else if (xo_role_wants_default_format(xfip->xfi_ftype)) {
xfip->xfi_format = xo_default_format;
xfip->xfi_flen = 2;
}
}

cp = sp;
}

int rc = 0;





if (seen_fnum)
rc = xo_parse_field_numbers(xop, fmt, fields, field);

return rc;
}





static int
xo_gettext_simplify_format (xo_handle_t *xop UNUSED,
xo_buffer_t *xbp,
xo_field_info_t *fields,
int this_field,
const char *fmt UNUSED,
xo_simplify_field_func_t field_cb)
{
unsigned ftype;
xo_xff_flags_t flags;
int field = this_field + 1;
xo_field_info_t *xfip;
char ch;

for (xfip = &fields[field]; xfip->xfi_ftype; xfip++, field++) {
ftype = xfip->xfi_ftype;
flags = xfip->xfi_flags;

if ((flags & XFF_GT_FIELD) && xfip->xfi_content && ftype != 'V') {
if (field_cb)
field_cb(xfip->xfi_content, xfip->xfi_clen,
(flags & XFF_GT_PLURAL) ? 1 : 0);
}

switch (ftype) {
case 'G':

break;

case XO_ROLE_NEWLINE:
xo_buf_append(xbp, "\n", 1);
break;

case XO_ROLE_EBRACE:
xo_buf_append(xbp, "{", 1);
xo_buf_append(xbp, xfip->xfi_content, xfip->xfi_clen);
xo_buf_append(xbp, "}", 1);
break;

case XO_ROLE_TEXT:
xo_buf_append(xbp, xfip->xfi_content, xfip->xfi_clen);
break;

default:
xo_buf_append(xbp, "{", 1);
if (ftype != 'V') {
ch = ftype;
xo_buf_append(xbp, &ch, 1);
}

unsigned fnum = xfip->xfi_fnum ?: 0;
if (fnum) {
char num[12];

snprintf(num, sizeof(num), "%u", fnum);
xo_buf_append(xbp, num, strlen(num));
}

xo_buf_append(xbp, ":", 1);
xo_buf_append(xbp, xfip->xfi_content, xfip->xfi_clen);
xo_buf_append(xbp, "}", 1);
}
}

xo_buf_append(xbp, "", 1);
return 0;
}

void
xo_dump_fields (xo_field_info_t *);
void
xo_dump_fields (xo_field_info_t *fields)
{
xo_field_info_t *xfip;

for (xfip = fields; xfip->xfi_ftype; xfip++) {
printf("%lu(%u): %lx [%c/%u] [%.*s] [%.*s] [%.*s]\n",
(unsigned long) (xfip - fields), xfip->xfi_fnum,
(unsigned long) xfip->xfi_flags,
isprint((int) xfip->xfi_ftype) ? xfip->xfi_ftype : ' ',
xfip->xfi_ftype,
(int) xfip->xfi_clen, xfip->xfi_content ?: "",
(int) xfip->xfi_flen, xfip->xfi_format ?: "",
(int) xfip->xfi_elen, xfip->xfi_encoding ?: "");
}
}

#if defined(HAVE_GETTEXT)



static xo_field_info_t *
xo_gettext_find_field (xo_field_info_t *fields, unsigned fnum)
{
xo_field_info_t *xfip;

for (xfip = fields; xfip->xfi_ftype; xfip++)
if (xfip->xfi_fnum == fnum)
return xfip;

return NULL;
}












static void
xo_gettext_rewrite_fields (xo_handle_t *xop UNUSED,
xo_field_info_t *fields, unsigned max_fields)
{
xo_field_info_t tmp[max_fields];
bzero(tmp, max_fields * sizeof(tmp[0]));

unsigned fnum = 0;
xo_field_info_t *newp, *outp, *zp;
for (newp = fields, outp = tmp; newp->xfi_ftype; newp++, outp++) {
switch (newp->xfi_ftype) {
case XO_ROLE_NEWLINE:
case XO_ROLE_TEXT:
case XO_ROLE_EBRACE:
case 'G':
*outp = *newp;
outp->xfi_renum = 0;
continue;
}

zp = xo_gettext_find_field(fields, ++fnum);
if (zp == NULL) {
*outp = *newp;
outp->xfi_renum = 0;
continue;
}

*outp = *zp;
outp->xfi_renum = newp->xfi_fnum;
}

memcpy(fields, tmp, max_fields * sizeof(tmp[0]));
}











static int
xo_gettext_combine_formats (xo_handle_t *xop, const char *fmt UNUSED,
const char *gtfmt, xo_field_info_t *old_fields,
xo_field_info_t *new_fields, unsigned new_max_fields,
int *reorderedp)
{
int reordered = 0;
xo_field_info_t *newp, *oldp, *startp = old_fields;

xo_gettext_finish_numbering_fields(xop, fmt, old_fields);

for (newp = new_fields; newp->xfi_ftype; newp++) {
switch (newp->xfi_ftype) {
case XO_ROLE_NEWLINE:
case XO_ROLE_TEXT:
case XO_ROLE_EBRACE:
continue;

case 'V':
for (oldp = startp; oldp->xfi_ftype; oldp++) {
if (oldp->xfi_ftype != 'V')
continue;
if (newp->xfi_clen != oldp->xfi_clen
|| strncmp(newp->xfi_content, oldp->xfi_content,
oldp->xfi_clen) != 0) {
reordered = 1;
continue;
}
startp = oldp + 1;
break;
}


if (oldp->xfi_ftype == 0) {
for (oldp = old_fields; oldp < startp; oldp++) {
if (oldp->xfi_ftype != 'V')
continue;
if (newp->xfi_clen != oldp->xfi_clen)
continue;
if (strncmp(newp->xfi_content, oldp->xfi_content,
oldp->xfi_clen) != 0)
continue;
reordered = 1;
break;
}
if (oldp == startp) {

xo_failure(xop, "post-gettext format can't find field "
"'%.*s' in format '%s'",
newp->xfi_clen, newp->xfi_content,
xo_printable(gtfmt));
return -1;
}
}
break;

default:





for (oldp = startp; oldp->xfi_ftype; oldp++) {
if (oldp->xfi_ftype == 'V')
break;
if (oldp->xfi_ftype == newp->xfi_ftype)
goto copy_it;
}
continue;
}




copy_it:
newp->xfi_flags = oldp->xfi_flags;
newp->xfi_fnum = oldp->xfi_fnum;
newp->xfi_format = oldp->xfi_format;
newp->xfi_flen = oldp->xfi_flen;
newp->xfi_encoding = oldp->xfi_encoding;
newp->xfi_elen = oldp->xfi_elen;
}

*reorderedp = reordered;
if (reordered) {
xo_gettext_finish_numbering_fields(xop, fmt, new_fields);
xo_gettext_rewrite_fields(xop, new_fields, new_max_fields);
}

return 0;
}






















static const char *
xo_gettext_build_format (xo_handle_t *xop,
xo_field_info_t *fields, int this_field,
const char *fmt, char **new_fmtp)
{
if (xo_style_is_encoding(xop))
goto bail;

xo_buffer_t xb;
xo_buf_init(&xb);

if (xo_gettext_simplify_format(xop, &xb, fields,
this_field, fmt, NULL))
goto bail2;

const char *gtfmt = xo_dgettext(xop, xb.xb_bufp);
if (gtfmt == NULL || gtfmt == fmt || xo_streq(gtfmt, fmt))
goto bail2;

char *new_fmt = xo_strndup(gtfmt, -1);
if (new_fmt == NULL)
goto bail2;

xo_buf_cleanup(&xb);

*new_fmtp = new_fmt;
return new_fmt;

bail2:
xo_buf_cleanup(&xb);
bail:
*new_fmtp = NULL;
return fmt;
}

static void
xo_gettext_rebuild_content (xo_handle_t *xop, xo_field_info_t *fields,
ssize_t *fstart, unsigned min_fstart,
ssize_t *fend, unsigned max_fend)
{
xo_field_info_t *xfip;
char *buf;
ssize_t base = fstart[min_fstart];
ssize_t blen = fend[max_fend] - base;
xo_buffer_t *xbp = &xop->xo_data;

if (blen == 0)
return;

buf = xo_realloc(NULL, blen);
if (buf == NULL)
return;

memcpy(buf, xbp->xb_bufp + fstart[min_fstart], blen);

unsigned field = min_fstart, len, fnum;
ssize_t soff, doff = base;
xo_field_info_t *zp;









for (xfip = fields + field; xfip->xfi_ftype; xfip++, field++) {
fnum = field;
if (xfip->xfi_renum) {
zp = xo_gettext_find_field(fields, xfip->xfi_renum);
fnum = zp ? zp - fields : field;
}

soff = fstart[fnum];
len = fend[fnum] - soff;

if (len > 0) {
soff -= base;
memcpy(xbp->xb_bufp + doff, buf + soff, len);
doff += len;
}
}

xo_free(buf);
}
#else
static const char *
xo_gettext_build_format (xo_handle_t *xop UNUSED,
xo_field_info_t *fields UNUSED,
int this_field UNUSED,
const char *fmt UNUSED, char **new_fmtp)
{
*new_fmtp = NULL;
return fmt;
}

static int
xo_gettext_combine_formats (xo_handle_t *xop UNUSED, const char *fmt UNUSED,
const char *gtfmt UNUSED,
xo_field_info_t *old_fields UNUSED,
xo_field_info_t *new_fields UNUSED,
unsigned new_max_fields UNUSED,
int *reorderedp UNUSED)
{
return -1;
}

static void
xo_gettext_rebuild_content (xo_handle_t *xop UNUSED,
xo_field_info_t *fields UNUSED,
ssize_t *fstart UNUSED, unsigned min_fstart UNUSED,
ssize_t *fend UNUSED, unsigned max_fend UNUSED)
{
return;
}
#endif




static ssize_t
xo_do_emit_fields (xo_handle_t *xop, xo_field_info_t *fields,
unsigned max_fields, const char *fmt)
{
int gettext_inuse = 0;
int gettext_changed = 0;
int gettext_reordered = 0;
unsigned ftype;
xo_xff_flags_t flags;
xo_field_info_t *new_fields = NULL;
xo_field_info_t *xfip;
unsigned field;
ssize_t rc = 0;

int flush = XOF_ISSET(xop, XOF_FLUSH);
int flush_line = XOF_ISSET(xop, XOF_FLUSH_LINE);
char *new_fmt = NULL;

if (XOIF_ISSET(xop, XOIF_REORDER) || xo_style(xop) == XO_STYLE_ENCODER)
flush_line = 0;








unsigned flimit = max_fields * 2;
unsigned min_fstart = flimit - 1;
unsigned max_fend = 0;
ssize_t fstart[flimit];
bzero(fstart, flimit * sizeof(fstart[0]));
ssize_t fend[flimit];
bzero(fend, flimit * sizeof(fend[0]));

for (xfip = fields, field = 0; field < max_fields && xfip->xfi_ftype;
xfip++, field++) {
ftype = xfip->xfi_ftype;
flags = xfip->xfi_flags;


if (gettext_reordered) {
fstart[field] = xo_buf_offset(&xop->xo_data);
if (min_fstart > field)
min_fstart = field;
}

const char *content = xfip->xfi_content;
ssize_t clen = xfip->xfi_clen;

if (flags & XFF_ARGUMENT) {




content = va_arg(xop->xo_vap, char *);
clen = content ? strlen(content) : 0;
}

if (ftype == XO_ROLE_NEWLINE) {
xo_line_close(xop);
if (flush_line && xo_flush_h(xop) < 0)
return -1;
goto bottom;

} else if (ftype == XO_ROLE_EBRACE) {
xo_format_text(xop, xfip->xfi_start, xfip->xfi_len);
goto bottom;

} else if (ftype == XO_ROLE_TEXT) {

xo_format_text(xop, xfip->xfi_content, xfip->xfi_clen);
goto bottom;
}




if (ftype == 'N' || ftype == 'U') {
if (flags & XFF_WS) {
xo_format_content(xop, "padding", NULL, " ", 1,
NULL, 0, flags);
flags &= ~XFF_WS;
}
}

if (ftype == 'V')
xo_format_value(xop, content, clen, NULL, 0,
xfip->xfi_format, xfip->xfi_flen,
xfip->xfi_encoding, xfip->xfi_elen, flags);
else if (ftype == '[')
xo_anchor_start(xop, xfip, content, clen);
else if (ftype == ']')
xo_anchor_stop(xop, xfip, content, clen);
else if (ftype == 'C')
xo_format_colors(xop, xfip, content, clen);

else if (ftype == 'G') {








xo_set_gettext_domain(xop, xfip, content, clen);

if (!gettext_inuse) {
gettext_inuse = 1;
if (new_fmt) {
xo_free(new_fmt);
new_fmt = NULL;
}

xo_gettext_build_format(xop, fields, field,
xfip->xfi_next, &new_fmt);
if (new_fmt) {
gettext_changed = 1;

unsigned new_max_fields = xo_count_fields(xop, new_fmt);

if (++new_max_fields < max_fields)
new_max_fields = max_fields;


ssize_t sz = (new_max_fields + 1) * sizeof(xo_field_info_t);
new_fields = alloca(sz);
bzero(new_fields, sz);

if (!xo_parse_fields(xop, new_fields + 1,
new_max_fields, new_fmt)) {
gettext_reordered = 0;

if (!xo_gettext_combine_formats(xop, fmt, new_fmt,
fields, new_fields + 1,
new_max_fields, &gettext_reordered)) {

if (gettext_reordered) {
if (XOF_ISSET(xop, XOF_LOG_GETTEXT))
xo_failure(xop, "gettext finds reordered "
"fields in '%s' and '%s'",
xo_printable(fmt),
xo_printable(new_fmt));
flush_line = 0;
XOIF_SET(xop, XOIF_REORDER);
}

field = -1;
xfip = new_fields;
max_fields = new_max_fields;
}
}
}
}
continue;

} else if (clen || xfip->xfi_format) {

const char *class_name = xo_class_name(ftype);
if (class_name)
xo_format_content(xop, class_name, xo_tag_name(ftype),
content, clen,
xfip->xfi_format, xfip->xfi_flen, flags);
else if (ftype == 'T')
xo_format_title(xop, xfip, content, clen);
else if (ftype == 'U')
xo_format_units(xop, xfip, content, clen);
else
xo_failure(xop, "unknown field type: '%c'", ftype);
}

if (flags & XFF_COLON)
xo_format_content(xop, "decoration", NULL, ":", 1, NULL, 0, 0);

if (flags & XFF_WS)
xo_format_content(xop, "padding", NULL, " ", 1, NULL, 0, 0);

bottom:

if (gettext_reordered) {
fend[field] = xo_buf_offset(&xop->xo_data);
max_fend = field;
}
}

if (gettext_changed && gettext_reordered) {

xo_gettext_rebuild_content(xop, new_fields + 1, fstart, min_fstart,
fend, max_fend);
}

XOIF_CLEAR(xop, XOIF_REORDER);




if (xo_buf_offset(&xop->xo_data) > XO_BUF_HIGH_WATER)
flush = 1;


if (flush && !XOIF_ISSET(xop, XOIF_ANCHOR)) {
if (xo_flush_h(xop) < 0)
rc = -1;
}

if (new_fmt)
xo_free(new_fmt);






if (xop->xo_gt_domain) {
xo_free(xop->xo_gt_domain);
xop->xo_gt_domain = NULL;
}

return (rc < 0) ? rc : xop->xo_columns;
}




static int
xo_do_emit (xo_handle_t *xop, xo_emit_flags_t flags, const char *fmt)
{
xop->xo_columns = 0;
xop->xo_errno = errno;

if (fmt == NULL)
return 0;

unsigned max_fields;
xo_field_info_t *fields = NULL;


if (XOF_ISSET(xop, XOF_RETAIN_ALL))
flags |= XOEF_RETAIN;
if (XOF_ISSET(xop, XOF_RETAIN_NONE))
flags &= ~XOEF_RETAIN;






if (!(flags & XOEF_RETAIN)
|| xo_retain_find(fmt, &fields, &max_fields) != 0
|| fields == NULL) {


max_fields = xo_count_fields(xop, fmt);
fields = alloca(max_fields * sizeof(fields[0]));
bzero(fields, max_fields * sizeof(fields[0]));

if (xo_parse_fields(xop, fields, max_fields, fmt))
return -1;

if (flags & XOEF_RETAIN) {

xo_retain_add(fmt, fields, max_fields);
}
}

return xo_do_emit_fields(xop, fields, max_fields, fmt);
}





char *
xo_simplify_format (xo_handle_t *xop, const char *fmt, int with_numbers,
xo_simplify_field_func_t field_cb)
{
xop = xo_default(xop);

xop->xo_columns = 0;
xop->xo_errno = errno;

unsigned max_fields = xo_count_fields(xop, fmt);
xo_field_info_t fields[max_fields];

bzero(fields, max_fields * sizeof(fields[0]));

if (xo_parse_fields(xop, fields, max_fields, fmt))
return NULL;

xo_buffer_t xb;
xo_buf_init(&xb);

if (with_numbers)
xo_gettext_finish_numbering_fields(xop, fmt, fields);

if (xo_gettext_simplify_format(xop, &xb, fields, -1, fmt, field_cb))
return NULL;

return xb.xb_bufp;
}

xo_ssize_t
xo_emit_hv (xo_handle_t *xop, const char *fmt, va_list vap)
{
ssize_t rc;

xop = xo_default(xop);
va_copy(xop->xo_vap, vap);
rc = xo_do_emit(xop, 0, fmt);
va_end(xop->xo_vap);
bzero(&xop->xo_vap, sizeof(xop->xo_vap));

return rc;
}

xo_ssize_t
xo_emit_h (xo_handle_t *xop, const char *fmt, ...)
{
ssize_t rc;

xop = xo_default(xop);
va_start(xop->xo_vap, fmt);
rc = xo_do_emit(xop, 0, fmt);
va_end(xop->xo_vap);
bzero(&xop->xo_vap, sizeof(xop->xo_vap));

return rc;
}

xo_ssize_t
xo_emit (const char *fmt, ...)
{
xo_handle_t *xop = xo_default(NULL);
ssize_t rc;

va_start(xop->xo_vap, fmt);
rc = xo_do_emit(xop, 0, fmt);
va_end(xop->xo_vap);
bzero(&xop->xo_vap, sizeof(xop->xo_vap));

return rc;
}

xo_ssize_t
xo_emit_hvf (xo_handle_t *xop, xo_emit_flags_t flags,
const char *fmt, va_list vap)
{
ssize_t rc;

xop = xo_default(xop);
va_copy(xop->xo_vap, vap);
rc = xo_do_emit(xop, flags, fmt);
va_end(xop->xo_vap);
bzero(&xop->xo_vap, sizeof(xop->xo_vap));

return rc;
}

xo_ssize_t
xo_emit_hf (xo_handle_t *xop, xo_emit_flags_t flags, const char *fmt, ...)
{
ssize_t rc;

xop = xo_default(xop);
va_start(xop->xo_vap, fmt);
rc = xo_do_emit(xop, flags, fmt);
va_end(xop->xo_vap);
bzero(&xop->xo_vap, sizeof(xop->xo_vap));

return rc;
}

xo_ssize_t
xo_emit_f (xo_emit_flags_t flags, const char *fmt, ...)
{
xo_handle_t *xop = xo_default(NULL);
ssize_t rc;

va_start(xop->xo_vap, fmt);
rc = xo_do_emit(xop, flags, fmt);
va_end(xop->xo_vap);
bzero(&xop->xo_vap, sizeof(xop->xo_vap));

return rc;
}







xo_ssize_t
xo_emit_field_hv (xo_handle_t *xop, const char *rolmod, const char *contents,
const char *fmt, const char *efmt,
va_list vap)
{
ssize_t rc;

xop = xo_default(xop);

if (rolmod == NULL)
rolmod = "V";

xo_field_info_t xfi;

bzero(&xfi, sizeof(xfi));

const char *cp;
cp = xo_parse_roles(xop, rolmod, rolmod, &xfi);
if (cp == NULL)
return -1;

xfi.xfi_start = fmt;
xfi.xfi_content = contents;
xfi.xfi_format = fmt;
xfi.xfi_encoding = efmt;
xfi.xfi_clen = contents ? strlen(contents) : 0;
xfi.xfi_flen = fmt ? strlen(fmt) : 0;
xfi.xfi_elen = efmt ? strlen(efmt) : 0;


if (contents && fmt == NULL
&& xo_role_wants_default_format(xfi.xfi_ftype)) {
xfi.xfi_format = xo_default_format;
xfi.xfi_flen = 2;
}

va_copy(xop->xo_vap, vap);

rc = xo_do_emit_fields(xop, &xfi, 1, fmt ?: contents ?: "field");

va_end(xop->xo_vap);

return rc;
}

xo_ssize_t
xo_emit_field_h (xo_handle_t *xop, const char *rolmod, const char *contents,
const char *fmt, const char *efmt, ...)
{
ssize_t rc;
va_list vap;

va_start(vap, efmt);
rc = xo_emit_field_hv(xop, rolmod, contents, fmt, efmt, vap);
va_end(vap);

return rc;
}

xo_ssize_t
xo_emit_field (const char *rolmod, const char *contents,
const char *fmt, const char *efmt, ...)
{
ssize_t rc;
va_list vap;

va_start(vap, efmt);
rc = xo_emit_field_hv(NULL, rolmod, contents, fmt, efmt, vap);
va_end(vap);

return rc;
}

xo_ssize_t
xo_attr_hv (xo_handle_t *xop, const char *name, const char *fmt, va_list vap)
{
const ssize_t extra = 5;
xop = xo_default(xop);

ssize_t rc = 0;
ssize_t nlen = strlen(name);
xo_buffer_t *xbp = &xop->xo_attrs;
ssize_t name_offset, value_offset;

switch (xo_style(xop)) {
case XO_STYLE_XML:
if (!xo_buf_has_room(xbp, nlen + extra))
return -1;

*xbp->xb_curp++ = ' ';
memcpy(xbp->xb_curp, name, nlen);
xbp->xb_curp += nlen;
*xbp->xb_curp++ = '=';
*xbp->xb_curp++ = '"';

rc = xo_vsnprintf(xop, xbp, fmt, vap);

if (rc >= 0) {
rc = xo_escape_xml(xbp, rc, 1);
xbp->xb_curp += rc;
}

if (!xo_buf_has_room(xbp, 2))
return -1;

*xbp->xb_curp++ = '"';
*xbp->xb_curp = '\0';

rc += nlen + extra;
break;

case XO_STYLE_ENCODER:
name_offset = xo_buf_offset(xbp);
xo_buf_append(xbp, name, nlen);
xo_buf_append(xbp, "", 1);

value_offset = xo_buf_offset(xbp);
rc = xo_vsnprintf(xop, xbp, fmt, vap);
if (rc >= 0) {
xbp->xb_curp += rc;
*xbp->xb_curp = '\0';
rc = xo_encoder_handle(xop, XO_OP_ATTRIBUTE,
xo_buf_data(xbp, name_offset),
xo_buf_data(xbp, value_offset), 0);
}
}

return rc;
}

xo_ssize_t
xo_attr_h (xo_handle_t *xop, const char *name, const char *fmt, ...)
{
ssize_t rc;
va_list vap;

va_start(vap, fmt);
rc = xo_attr_hv(xop, name, fmt, vap);
va_end(vap);

return rc;
}

xo_ssize_t
xo_attr (const char *name, const char *fmt, ...)
{
ssize_t rc;
va_list vap;

va_start(vap, fmt);
rc = xo_attr_hv(NULL, name, fmt, vap);
va_end(vap);

return rc;
}

static void
xo_depth_change (xo_handle_t *xop, const char *name,
int delta, int indent, xo_state_t state, xo_xsf_flags_t flags)
{
if (xo_style(xop) == XO_STYLE_HTML || xo_style(xop) == XO_STYLE_TEXT)
indent = 0;

if (XOF_ISSET(xop, XOF_DTRT))
flags |= XSF_DTRT;

if (delta >= 0) {
if (xo_depth_check(xop, xop->xo_depth + delta))
return;

xo_stack_t *xsp = &xop->xo_stack[xop->xo_depth + delta];
xsp->xs_flags = flags;
xsp->xs_state = state;
xo_stack_set_flags(xop);

if (name == NULL)
name = XO_FAILURE_NAME;

xsp->xs_name = xo_strndup(name, -1);

} else {
if (xop->xo_depth == 0) {
if (!XOF_ISSET(xop, XOF_IGNORE_CLOSE))
xo_failure(xop, "close with empty stack: '%s'", name);
return;
}

xo_stack_t *xsp = &xop->xo_stack[xop->xo_depth];
if (XOF_ISSET(xop, XOF_WARN)) {
const char *top = xsp->xs_name;
if (top != NULL && name != NULL && !xo_streq(name, top)) {
xo_failure(xop, "incorrect close: '%s' .vs. '%s'",
name, top);
return;
}
if ((xsp->xs_flags & XSF_LIST) != (flags & XSF_LIST)) {
xo_failure(xop, "list close on list confict: '%s'",
name);
return;
}
if ((xsp->xs_flags & XSF_INSTANCE) != (flags & XSF_INSTANCE)) {
xo_failure(xop, "list close on instance confict: '%s'",
name);
return;
}
}

if (xsp->xs_name) {
xo_free(xsp->xs_name);
xsp->xs_name = NULL;
}
if (xsp->xs_keys) {
xo_free(xsp->xs_keys);
xsp->xs_keys = NULL;
}
}

xop->xo_depth += delta;
xop->xo_indent += indent;
}

void
xo_set_depth (xo_handle_t *xop, int depth)
{
xop = xo_default(xop);

if (xo_depth_check(xop, depth))
return;

xop->xo_depth += depth;
xop->xo_indent += depth;






if (xop->xo_style == XO_STYLE_JSON
&& !XOF_ISSET(xop, XOF_NO_TOP) && xop->xo_depth > 0)
XOIF_SET(xop, XOIF_TOP_EMITTED);
}

static xo_xsf_flags_t
xo_stack_flags (xo_xof_flags_t xflags)
{
if (xflags & XOF_DTRT)
return XSF_DTRT;
return 0;
}

static void
xo_emit_top (xo_handle_t *xop, const char *ppn)
{
xo_printf(xop, "%*s{%s", xo_indent(xop), "", ppn);
XOIF_SET(xop, XOIF_TOP_EMITTED);

if (xop->xo_version) {
xo_printf(xop, "%*s\"__version\": \"%s\", %s",
xo_indent(xop), "", xop->xo_version, ppn);
xo_free(xop->xo_version);
xop->xo_version = NULL;
}
}

static ssize_t
xo_do_open_container (xo_handle_t *xop, xo_xof_flags_t flags, const char *name)
{
ssize_t rc = 0;
const char *ppn = XOF_ISSET(xop, XOF_PRETTY) ? "\n" : "";
const char *pre_nl = "";

if (name == NULL) {
xo_failure(xop, "NULL passed for container name");
name = XO_FAILURE_NAME;
}

const char *leader = xo_xml_leader(xop, name);
flags |= xop->xo_flags;

switch (xo_style(xop)) {
case XO_STYLE_XML:
rc = xo_printf(xop, "%*s<%s%s", xo_indent(xop), "", leader, name);

if (xop->xo_attrs.xb_curp != xop->xo_attrs.xb_bufp) {
rc += xop->xo_attrs.xb_curp - xop->xo_attrs.xb_bufp;
xo_data_append(xop, xop->xo_attrs.xb_bufp,
xop->xo_attrs.xb_curp - xop->xo_attrs.xb_bufp);
xop->xo_attrs.xb_curp = xop->xo_attrs.xb_bufp;
}

rc += xo_printf(xop, ">%s", ppn);
break;

case XO_STYLE_JSON:
xo_stack_set_flags(xop);

if (!XOF_ISSET(xop, XOF_NO_TOP)
&& !XOIF_ISSET(xop, XOIF_TOP_EMITTED))
xo_emit_top(xop, ppn);

if (xop->xo_stack[xop->xo_depth].xs_flags & XSF_NOT_FIRST)
pre_nl = XOF_ISSET(xop, XOF_PRETTY) ? ",\n" : ", ";
xop->xo_stack[xop->xo_depth].xs_flags |= XSF_NOT_FIRST;

rc = xo_printf(xop, "%s%*s\"%s\": {%s",
pre_nl, xo_indent(xop), "", name, ppn);
break;

case XO_STYLE_SDPARAMS:
break;

case XO_STYLE_ENCODER:
rc = xo_encoder_handle(xop, XO_OP_OPEN_CONTAINER, name, NULL, flags);
break;
}

xo_depth_change(xop, name, 1, 1, XSS_OPEN_CONTAINER,
xo_stack_flags(flags));

return rc;
}

xo_ssize_t
xo_open_container_hf (xo_handle_t *xop, xo_xof_flags_t flags, const char *name)
{
return xo_transition(xop, flags, name, XSS_OPEN_CONTAINER);
}

xo_ssize_t
xo_open_container_h (xo_handle_t *xop, const char *name)
{
return xo_open_container_hf(xop, 0, name);
}

xo_ssize_t
xo_open_container (const char *name)
{
return xo_open_container_hf(NULL, 0, name);
}

xo_ssize_t
xo_open_container_hd (xo_handle_t *xop, const char *name)
{
return xo_open_container_hf(xop, XOF_DTRT, name);
}

xo_ssize_t
xo_open_container_d (const char *name)
{
return xo_open_container_hf(NULL, XOF_DTRT, name);
}

static int
xo_do_close_container (xo_handle_t *xop, const char *name)
{
xop = xo_default(xop);

ssize_t rc = 0;
const char *ppn = XOF_ISSET(xop, XOF_PRETTY) ? "\n" : "";
const char *pre_nl = "";

if (name == NULL) {
xo_stack_t *xsp = &xop->xo_stack[xop->xo_depth];

name = xsp->xs_name;
if (name) {
ssize_t len = strlen(name) + 1;

char *cp = alloca(len);
memcpy(cp, name, len);
name = cp;
} else if (!(xsp->xs_flags & XSF_DTRT)) {
xo_failure(xop, "missing name without 'dtrt' mode");
name = XO_FAILURE_NAME;
}
}

const char *leader = xo_xml_leader(xop, name);

switch (xo_style(xop)) {
case XO_STYLE_XML:
xo_depth_change(xop, name, -1, -1, XSS_CLOSE_CONTAINER, 0);
rc = xo_printf(xop, "%*s</%s%s>%s", xo_indent(xop), "", leader, name, ppn);
break;

case XO_STYLE_JSON:
xo_stack_set_flags(xop);

pre_nl = XOF_ISSET(xop, XOF_PRETTY) ? "\n" : "";
ppn = "";

xo_depth_change(xop, name, -1, -1, XSS_CLOSE_CONTAINER, 0);
rc = xo_printf(xop, "%s%*s}%s", pre_nl, xo_indent(xop), "", ppn);
xop->xo_stack[xop->xo_depth].xs_flags |= XSF_NOT_FIRST;
break;

case XO_STYLE_HTML:
case XO_STYLE_TEXT:
xo_depth_change(xop, name, -1, 0, XSS_CLOSE_CONTAINER, 0);
break;

case XO_STYLE_SDPARAMS:
break;

case XO_STYLE_ENCODER:
xo_depth_change(xop, name, -1, 0, XSS_CLOSE_CONTAINER, 0);
rc = xo_encoder_handle(xop, XO_OP_CLOSE_CONTAINER, name, NULL, 0);
break;
}

return rc;
}

xo_ssize_t
xo_close_container_h (xo_handle_t *xop, const char *name)
{
return xo_transition(xop, 0, name, XSS_CLOSE_CONTAINER);
}

xo_ssize_t
xo_close_container (const char *name)
{
return xo_close_container_h(NULL, name);
}

xo_ssize_t
xo_close_container_hd (xo_handle_t *xop)
{
return xo_close_container_h(xop, NULL);
}

xo_ssize_t
xo_close_container_d (void)
{
return xo_close_container_h(NULL, NULL);
}

static int
xo_do_open_list (xo_handle_t *xop, xo_xof_flags_t flags, const char *name)
{
ssize_t rc = 0;
int indent = 0;

xop = xo_default(xop);

const char *ppn = XOF_ISSET(xop, XOF_PRETTY) ? "\n" : "";
const char *pre_nl = "";

switch (xo_style(xop)) {
case XO_STYLE_JSON:

indent = 1;
if (!XOF_ISSET(xop, XOF_NO_TOP)
&& !XOIF_ISSET(xop, XOIF_TOP_EMITTED))
xo_emit_top(xop, ppn);

if (name == NULL) {
xo_failure(xop, "NULL passed for list name");
name = XO_FAILURE_NAME;
}

xo_stack_set_flags(xop);

if (xop->xo_stack[xop->xo_depth].xs_flags & XSF_NOT_FIRST)
pre_nl = XOF_ISSET(xop, XOF_PRETTY) ? ",\n" : ", ";
xop->xo_stack[xop->xo_depth].xs_flags |= XSF_NOT_FIRST;

rc = xo_printf(xop, "%s%*s\"%s\": [%s",
pre_nl, xo_indent(xop), "", name, ppn);
break;

case XO_STYLE_ENCODER:
rc = xo_encoder_handle(xop, XO_OP_OPEN_LIST, name, NULL, flags);
break;
}

xo_depth_change(xop, name, 1, indent, XSS_OPEN_LIST,
XSF_LIST | xo_stack_flags(flags));

return rc;
}

xo_ssize_t
xo_open_list_hf (xo_handle_t *xop, xo_xof_flags_t flags, const char *name)
{
return xo_transition(xop, flags, name, XSS_OPEN_LIST);
}

xo_ssize_t
xo_open_list_h (xo_handle_t *xop, const char *name)
{
return xo_open_list_hf(xop, 0, name);
}

xo_ssize_t
xo_open_list (const char *name)
{
return xo_open_list_hf(NULL, 0, name);
}

xo_ssize_t
xo_open_list_hd (xo_handle_t *xop, const char *name)
{
return xo_open_list_hf(xop, XOF_DTRT, name);
}

xo_ssize_t
xo_open_list_d (const char *name)
{
return xo_open_list_hf(NULL, XOF_DTRT, name);
}

static int
xo_do_close_list (xo_handle_t *xop, const char *name)
{
ssize_t rc = 0;
const char *pre_nl = "";

if (name == NULL) {
xo_stack_t *xsp = &xop->xo_stack[xop->xo_depth];

name = xsp->xs_name;
if (name) {
ssize_t len = strlen(name) + 1;

char *cp = alloca(len);
memcpy(cp, name, len);
name = cp;
} else if (!(xsp->xs_flags & XSF_DTRT)) {
xo_failure(xop, "missing name without 'dtrt' mode");
name = XO_FAILURE_NAME;
}
}

switch (xo_style(xop)) {
case XO_STYLE_JSON:
if (xop->xo_stack[xop->xo_depth].xs_flags & XSF_NOT_FIRST)
pre_nl = XOF_ISSET(xop, XOF_PRETTY) ? "\n" : "";
xop->xo_stack[xop->xo_depth].xs_flags |= XSF_NOT_FIRST;

xo_depth_change(xop, name, -1, -1, XSS_CLOSE_LIST, XSF_LIST);
rc = xo_printf(xop, "%s%*s]", pre_nl, xo_indent(xop), "");
xop->xo_stack[xop->xo_depth].xs_flags |= XSF_NOT_FIRST;
break;

case XO_STYLE_ENCODER:
xo_depth_change(xop, name, -1, 0, XSS_CLOSE_LIST, XSF_LIST);
rc = xo_encoder_handle(xop, XO_OP_CLOSE_LIST, name, NULL, 0);
break;

default:
xo_depth_change(xop, name, -1, 0, XSS_CLOSE_LIST, XSF_LIST);
xop->xo_stack[xop->xo_depth].xs_flags |= XSF_NOT_FIRST;
break;
}

return rc;
}

xo_ssize_t
xo_close_list_h (xo_handle_t *xop, const char *name)
{
return xo_transition(xop, 0, name, XSS_CLOSE_LIST);
}

xo_ssize_t
xo_close_list (const char *name)
{
return xo_close_list_h(NULL, name);
}

xo_ssize_t
xo_close_list_hd (xo_handle_t *xop)
{
return xo_close_list_h(xop, NULL);
}

xo_ssize_t
xo_close_list_d (void)
{
return xo_close_list_h(NULL, NULL);
}

static int
xo_do_open_leaf_list (xo_handle_t *xop, xo_xof_flags_t flags, const char *name)
{
ssize_t rc = 0;
int indent = 0;

xop = xo_default(xop);

const char *ppn = XOF_ISSET(xop, XOF_PRETTY) ? "\n" : "";
const char *pre_nl = "";

switch (xo_style(xop)) {
case XO_STYLE_JSON:
indent = 1;

if (!XOF_ISSET(xop, XOF_NO_TOP)) {
if (!XOIF_ISSET(xop, XOIF_TOP_EMITTED)) {
xo_printf(xop, "%*s{%s", xo_indent(xop), "", ppn);
XOIF_SET(xop, XOIF_TOP_EMITTED);
}
}

if (name == NULL) {
xo_failure(xop, "NULL passed for list name");
name = XO_FAILURE_NAME;
}

xo_stack_set_flags(xop);

if (xop->xo_stack[xop->xo_depth].xs_flags & XSF_NOT_FIRST)
pre_nl = XOF_ISSET(xop, XOF_PRETTY) ? ",\n" : ", ";
xop->xo_stack[xop->xo_depth].xs_flags |= XSF_NOT_FIRST;

rc = xo_printf(xop, "%s%*s\"%s\": [%s",
pre_nl, xo_indent(xop), "", name, ppn);
break;

case XO_STYLE_ENCODER:
rc = xo_encoder_handle(xop, XO_OP_OPEN_LEAF_LIST, name, NULL, flags);
break;
}

xo_depth_change(xop, name, 1, indent, XSS_OPEN_LEAF_LIST,
XSF_LIST | xo_stack_flags(flags));

return rc;
}

static int
xo_do_close_leaf_list (xo_handle_t *xop, const char *name)
{
ssize_t rc = 0;
const char *pre_nl = "";

if (name == NULL) {
xo_stack_t *xsp = &xop->xo_stack[xop->xo_depth];

name = xsp->xs_name;
if (name) {
ssize_t len = strlen(name) + 1;

char *cp = alloca(len);
memcpy(cp, name, len);
name = cp;
} else if (!(xsp->xs_flags & XSF_DTRT)) {
xo_failure(xop, "missing name without 'dtrt' mode");
name = XO_FAILURE_NAME;
}
}

switch (xo_style(xop)) {
case XO_STYLE_JSON:
if (xop->xo_stack[xop->xo_depth].xs_flags & XSF_NOT_FIRST)
pre_nl = XOF_ISSET(xop, XOF_PRETTY) ? "\n" : "";
xop->xo_stack[xop->xo_depth].xs_flags |= XSF_NOT_FIRST;

xo_depth_change(xop, name, -1, -1, XSS_CLOSE_LEAF_LIST, XSF_LIST);
rc = xo_printf(xop, "%s%*s]", pre_nl, xo_indent(xop), "");
xop->xo_stack[xop->xo_depth].xs_flags |= XSF_NOT_FIRST;
break;

case XO_STYLE_ENCODER:
rc = xo_encoder_handle(xop, XO_OP_CLOSE_LEAF_LIST, name, NULL, 0);


default:
xo_depth_change(xop, name, -1, 0, XSS_CLOSE_LEAF_LIST, XSF_LIST);
xop->xo_stack[xop->xo_depth].xs_flags |= XSF_NOT_FIRST;
break;
}

return rc;
}

static int
xo_do_open_instance (xo_handle_t *xop, xo_xof_flags_t flags, const char *name)
{
xop = xo_default(xop);

ssize_t rc = 0;
const char *ppn = XOF_ISSET(xop, XOF_PRETTY) ? "\n" : "";
const char *pre_nl = "";

if (name == NULL) {
xo_failure(xop, "NULL passed for instance name");
name = XO_FAILURE_NAME;
}

const char *leader = xo_xml_leader(xop, name);
flags |= xop->xo_flags;

switch (xo_style(xop)) {
case XO_STYLE_XML:
rc = xo_printf(xop, "%*s<%s%s", xo_indent(xop), "", leader, name);

if (xop->xo_attrs.xb_curp != xop->xo_attrs.xb_bufp) {
rc += xop->xo_attrs.xb_curp - xop->xo_attrs.xb_bufp;
xo_data_append(xop, xop->xo_attrs.xb_bufp,
xop->xo_attrs.xb_curp - xop->xo_attrs.xb_bufp);
xop->xo_attrs.xb_curp = xop->xo_attrs.xb_bufp;
}

rc += xo_printf(xop, ">%s", ppn);
break;

case XO_STYLE_JSON:
xo_stack_set_flags(xop);

if (xop->xo_stack[xop->xo_depth].xs_flags & XSF_NOT_FIRST)
pre_nl = XOF_ISSET(xop, XOF_PRETTY) ? ",\n" : ", ";
xop->xo_stack[xop->xo_depth].xs_flags |= XSF_NOT_FIRST;

rc = xo_printf(xop, "%s%*s{%s",
pre_nl, xo_indent(xop), "", ppn);
break;

case XO_STYLE_SDPARAMS:
break;

case XO_STYLE_ENCODER:
rc = xo_encoder_handle(xop, XO_OP_OPEN_INSTANCE, name, NULL, flags);
break;
}

xo_depth_change(xop, name, 1, 1, XSS_OPEN_INSTANCE, xo_stack_flags(flags));

return rc;
}

xo_ssize_t
xo_open_instance_hf (xo_handle_t *xop, xo_xof_flags_t flags, const char *name)
{
return xo_transition(xop, flags, name, XSS_OPEN_INSTANCE);
}

xo_ssize_t
xo_open_instance_h (xo_handle_t *xop, const char *name)
{
return xo_open_instance_hf(xop, 0, name);
}

xo_ssize_t
xo_open_instance (const char *name)
{
return xo_open_instance_hf(NULL, 0, name);
}

xo_ssize_t
xo_open_instance_hd (xo_handle_t *xop, const char *name)
{
return xo_open_instance_hf(xop, XOF_DTRT, name);
}

xo_ssize_t
xo_open_instance_d (const char *name)
{
return xo_open_instance_hf(NULL, XOF_DTRT, name);
}

static int
xo_do_close_instance (xo_handle_t *xop, const char *name)
{
xop = xo_default(xop);

ssize_t rc = 0;
const char *ppn = XOF_ISSET(xop, XOF_PRETTY) ? "\n" : "";
const char *pre_nl = "";

if (name == NULL) {
xo_stack_t *xsp = &xop->xo_stack[xop->xo_depth];

name = xsp->xs_name;
if (name) {
ssize_t len = strlen(name) + 1;

char *cp = alloca(len);
memcpy(cp, name, len);
name = cp;
} else if (!(xsp->xs_flags & XSF_DTRT)) {
xo_failure(xop, "missing name without 'dtrt' mode");
name = XO_FAILURE_NAME;
}
}

const char *leader = xo_xml_leader(xop, name);

switch (xo_style(xop)) {
case XO_STYLE_XML:
xo_depth_change(xop, name, -1, -1, XSS_CLOSE_INSTANCE, 0);
rc = xo_printf(xop, "%*s</%s%s>%s", xo_indent(xop), "", leader, name, ppn);
break;

case XO_STYLE_JSON:
pre_nl = XOF_ISSET(xop, XOF_PRETTY) ? "\n" : "";

xo_depth_change(xop, name, -1, -1, XSS_CLOSE_INSTANCE, 0);
rc = xo_printf(xop, "%s%*s}", pre_nl, xo_indent(xop), "");
xop->xo_stack[xop->xo_depth].xs_flags |= XSF_NOT_FIRST;
break;

case XO_STYLE_HTML:
case XO_STYLE_TEXT:
xo_depth_change(xop, name, -1, 0, XSS_CLOSE_INSTANCE, 0);
break;

case XO_STYLE_SDPARAMS:
break;

case XO_STYLE_ENCODER:
xo_depth_change(xop, name, -1, 0, XSS_CLOSE_INSTANCE, 0);
rc = xo_encoder_handle(xop, XO_OP_CLOSE_INSTANCE, name, NULL, 0);
break;
}

return rc;
}

xo_ssize_t
xo_close_instance_h (xo_handle_t *xop, const char *name)
{
return xo_transition(xop, 0, name, XSS_CLOSE_INSTANCE);
}

xo_ssize_t
xo_close_instance (const char *name)
{
return xo_close_instance_h(NULL, name);
}

xo_ssize_t
xo_close_instance_hd (xo_handle_t *xop)
{
return xo_close_instance_h(xop, NULL);
}

xo_ssize_t
xo_close_instance_d (void)
{
return xo_close_instance_h(NULL, NULL);
}

static int
xo_do_close_all (xo_handle_t *xop, xo_stack_t *limit)
{
xo_stack_t *xsp;
ssize_t rc = 0;
xo_xsf_flags_t flags;

for (xsp = &xop->xo_stack[xop->xo_depth]; xsp >= limit; xsp--) {
switch (xsp->xs_state) {
case XSS_INIT:

rc = 0;
break;

case XSS_OPEN_CONTAINER:
rc = xo_do_close_container(xop, NULL);
break;

case XSS_OPEN_LIST:
rc = xo_do_close_list(xop, NULL);
break;

case XSS_OPEN_INSTANCE:
rc = xo_do_close_instance(xop, NULL);
break;

case XSS_OPEN_LEAF_LIST:
rc = xo_do_close_leaf_list(xop, NULL);
break;

case XSS_MARKER:
flags = xsp->xs_flags & XSF_MARKER_FLAGS;
xo_depth_change(xop, xsp->xs_name, -1, 0, XSS_MARKER, 0);
xop->xo_stack[xop->xo_depth].xs_flags |= flags;
rc = 0;
break;
}

if (rc < 0)
xo_failure(xop, "close %d failed: %d", xsp->xs_state, rc);
}

return 0;
}





static int
xo_do_close (xo_handle_t *xop, const char *name, xo_state_t new_state)
{
xo_stack_t *xsp, *limit = NULL;
ssize_t rc;
xo_state_t need_state = new_state;

if (new_state == XSS_CLOSE_CONTAINER)
need_state = XSS_OPEN_CONTAINER;
else if (new_state == XSS_CLOSE_LIST)
need_state = XSS_OPEN_LIST;
else if (new_state == XSS_CLOSE_INSTANCE)
need_state = XSS_OPEN_INSTANCE;
else if (new_state == XSS_CLOSE_LEAF_LIST)
need_state = XSS_OPEN_LEAF_LIST;
else if (new_state == XSS_MARKER)
need_state = XSS_MARKER;
else
return 0;

for (xsp = &xop->xo_stack[xop->xo_depth]; xsp > xop->xo_stack; xsp--) {




if (xsp->xs_state == XSS_MARKER && need_state != XSS_MARKER) {
if (name) {
xo_failure(xop, "close (xo_%s) fails at marker '%s'; "
"not found '%s'",
xo_state_name(new_state),
xsp->xs_name, name);
return 0;

} else {
limit = xsp;
xo_failure(xop, "close stops at marker '%s'", xsp->xs_name);
}
break;
}

if (xsp->xs_state != need_state)
continue;

if (name && xsp->xs_name && !xo_streq(name, xsp->xs_name))
continue;

limit = xsp;
break;
}

if (limit == NULL) {
xo_failure(xop, "xo_%s can't find match for '%s'",
xo_state_name(new_state), name);
return 0;
}

rc = xo_do_close_all(xop, limit);

return rc;
}




static ssize_t
xo_transition (xo_handle_t *xop, xo_xof_flags_t flags, const char *name,
xo_state_t new_state)
{
xo_stack_t *xsp;
ssize_t rc = 0;
int old_state, on_marker;

xop = xo_default(xop);

xsp = &xop->xo_stack[xop->xo_depth];
old_state = xsp->xs_state;
on_marker = (old_state == XSS_MARKER);


while (old_state == XSS_MARKER) {
if (xsp == xop->xo_stack)
break;
xsp -= 1;
old_state = xsp->xs_state;
}






switch (XSS_TRANSITION(old_state, new_state)) {

open_container:
case XSS_TRANSITION(XSS_INIT, XSS_OPEN_CONTAINER):
case XSS_TRANSITION(XSS_OPEN_INSTANCE, XSS_OPEN_CONTAINER):
case XSS_TRANSITION(XSS_OPEN_CONTAINER, XSS_OPEN_CONTAINER):
rc = xo_do_open_container(xop, flags, name);
break;

case XSS_TRANSITION(XSS_OPEN_LIST, XSS_OPEN_CONTAINER):
case XSS_TRANSITION(XSS_OPEN_LEAF_LIST, XSS_OPEN_CONTAINER):
if (on_marker)
goto marker_prevents_close;
rc = xo_do_close_leaf_list(xop, NULL);
if (rc >= 0)
goto open_container;
break;

case XSS_TRANSITION(XSS_INIT, XSS_CLOSE_CONTAINER):

rc = xo_do_close_container(xop, name);
break;


case XSS_TRANSITION(XSS_OPEN_CONTAINER, XSS_CLOSE_CONTAINER):
case XSS_TRANSITION(XSS_OPEN_LIST, XSS_CLOSE_CONTAINER):
case XSS_TRANSITION(XSS_OPEN_INSTANCE, XSS_CLOSE_CONTAINER):
if (on_marker)
goto marker_prevents_close;
rc = xo_do_close(xop, name, new_state);
break;

case XSS_TRANSITION(XSS_OPEN_LEAF_LIST, XSS_CLOSE_CONTAINER):
if (on_marker)
goto marker_prevents_close;
rc = xo_do_close_leaf_list(xop, NULL);
if (rc >= 0)
rc = xo_do_close(xop, name, new_state);
break;

open_list:
case XSS_TRANSITION(XSS_INIT, XSS_OPEN_LIST):
case XSS_TRANSITION(XSS_OPEN_CONTAINER, XSS_OPEN_LIST):
case XSS_TRANSITION(XSS_OPEN_INSTANCE, XSS_OPEN_LIST):
rc = xo_do_open_list(xop, flags, name);
break;

case XSS_TRANSITION(XSS_OPEN_LIST, XSS_OPEN_LIST):
if (on_marker)
goto marker_prevents_close;
rc = xo_do_close_list(xop, NULL);
if (rc >= 0)
goto open_list;
break;

case XSS_TRANSITION(XSS_OPEN_LEAF_LIST, XSS_OPEN_LIST):
if (on_marker)
goto marker_prevents_close;
rc = xo_do_close_leaf_list(xop, NULL);
if (rc >= 0)
goto open_list;
break;


case XSS_TRANSITION(XSS_OPEN_LIST, XSS_CLOSE_LIST):
if (on_marker)
goto marker_prevents_close;
rc = xo_do_close(xop, name, new_state);
break;

case XSS_TRANSITION(XSS_INIT, XSS_CLOSE_LIST):
case XSS_TRANSITION(XSS_OPEN_CONTAINER, XSS_CLOSE_LIST):
case XSS_TRANSITION(XSS_OPEN_INSTANCE, XSS_CLOSE_LIST):
case XSS_TRANSITION(XSS_OPEN_LEAF_LIST, XSS_CLOSE_LIST):
rc = xo_do_close(xop, name, new_state);
break;

open_instance:
case XSS_TRANSITION(XSS_OPEN_LIST, XSS_OPEN_INSTANCE):
rc = xo_do_open_instance(xop, flags, name);
break;

case XSS_TRANSITION(XSS_INIT, XSS_OPEN_INSTANCE):
case XSS_TRANSITION(XSS_OPEN_CONTAINER, XSS_OPEN_INSTANCE):
rc = xo_do_open_list(xop, flags, name);
if (rc >= 0)
goto open_instance;
break;

case XSS_TRANSITION(XSS_OPEN_INSTANCE, XSS_OPEN_INSTANCE):
if (on_marker) {
rc = xo_do_open_list(xop, flags, name);
} else {
rc = xo_do_close_instance(xop, NULL);
}
if (rc >= 0)
goto open_instance;
break;

case XSS_TRANSITION(XSS_OPEN_LEAF_LIST, XSS_OPEN_INSTANCE):
if (on_marker)
goto marker_prevents_close;
rc = xo_do_close_leaf_list(xop, NULL);
if (rc >= 0)
goto open_instance;
break;


case XSS_TRANSITION(XSS_OPEN_INSTANCE, XSS_CLOSE_INSTANCE):
if (on_marker)
goto marker_prevents_close;
rc = xo_do_close_instance(xop, name);
break;

case XSS_TRANSITION(XSS_INIT, XSS_CLOSE_INSTANCE):

xo_failure(xop, "xo_close_instance ignored when called from "
"initial state ('%s')", name ?: "(unknown)");
break;

case XSS_TRANSITION(XSS_OPEN_CONTAINER, XSS_CLOSE_INSTANCE):
case XSS_TRANSITION(XSS_OPEN_LIST, XSS_CLOSE_INSTANCE):
if (on_marker)
goto marker_prevents_close;
rc = xo_do_close(xop, name, new_state);
break;

case XSS_TRANSITION(XSS_OPEN_LEAF_LIST, XSS_CLOSE_INSTANCE):
if (on_marker)
goto marker_prevents_close;
rc = xo_do_close_leaf_list(xop, NULL);
if (rc >= 0)
rc = xo_do_close(xop, name, new_state);
break;

open_leaf_list:
case XSS_TRANSITION(XSS_OPEN_CONTAINER, XSS_OPEN_LEAF_LIST):
case XSS_TRANSITION(XSS_OPEN_INSTANCE, XSS_OPEN_LEAF_LIST):
case XSS_TRANSITION(XSS_INIT, XSS_OPEN_LEAF_LIST):
rc = xo_do_open_leaf_list(xop, flags, name);
break;

case XSS_TRANSITION(XSS_OPEN_LIST, XSS_OPEN_LEAF_LIST):
case XSS_TRANSITION(XSS_OPEN_LEAF_LIST, XSS_OPEN_LEAF_LIST):
if (on_marker)
goto marker_prevents_close;
rc = xo_do_close_list(xop, NULL);
if (rc >= 0)
goto open_leaf_list;
break;


case XSS_TRANSITION(XSS_OPEN_LEAF_LIST, XSS_CLOSE_LEAF_LIST):
if (on_marker)
goto marker_prevents_close;
rc = xo_do_close_leaf_list(xop, name);
break;

case XSS_TRANSITION(XSS_INIT, XSS_CLOSE_LEAF_LIST):

xo_failure(xop, "xo_close_leaf_list ignored when called from "
"initial state ('%s')", name ?: "(unknown)");
break;

case XSS_TRANSITION(XSS_OPEN_CONTAINER, XSS_CLOSE_LEAF_LIST):
case XSS_TRANSITION(XSS_OPEN_LIST, XSS_CLOSE_LEAF_LIST):
case XSS_TRANSITION(XSS_OPEN_INSTANCE, XSS_CLOSE_LEAF_LIST):
if (on_marker)
goto marker_prevents_close;
rc = xo_do_close(xop, name, new_state);
break;


case XSS_TRANSITION(XSS_OPEN_CONTAINER, XSS_EMIT):
case XSS_TRANSITION(XSS_OPEN_INSTANCE, XSS_EMIT):
break;

case XSS_TRANSITION(XSS_OPEN_LIST, XSS_EMIT):
if (on_marker)
goto marker_prevents_close;
rc = xo_do_close(xop, NULL, XSS_CLOSE_LIST);
break;

case XSS_TRANSITION(XSS_INIT, XSS_EMIT):
break;

case XSS_TRANSITION(XSS_OPEN_LEAF_LIST, XSS_EMIT):
if (on_marker)
goto marker_prevents_close;
rc = xo_do_close_leaf_list(xop, NULL);
break;


case XSS_TRANSITION(XSS_INIT, XSS_EMIT_LEAF_LIST):
case XSS_TRANSITION(XSS_OPEN_CONTAINER, XSS_EMIT_LEAF_LIST):
case XSS_TRANSITION(XSS_OPEN_INSTANCE, XSS_EMIT_LEAF_LIST):
rc = xo_do_open_leaf_list(xop, flags, name);
break;

case XSS_TRANSITION(XSS_OPEN_LEAF_LIST, XSS_EMIT_LEAF_LIST):
break;

case XSS_TRANSITION(XSS_OPEN_LIST, XSS_EMIT_LEAF_LIST):






break;

default:
xo_failure(xop, "unknown transition: (%u -> %u)",
xsp->xs_state, new_state);
}


if (rc >= 0 && XOF_ISSET(xop, XOF_FLUSH))
if (xo_flush_h(xop) < 0)
rc = -1;


XOIF_SET(xop, XOIF_MADE_OUTPUT);

return rc;

marker_prevents_close:
xo_failure(xop, "marker '%s' prevents transition from %s to %s",
xop->xo_stack[xop->xo_depth].xs_name,
xo_state_name(old_state), xo_state_name(new_state));
return -1;
}

xo_ssize_t
xo_open_marker_h (xo_handle_t *xop, const char *name)
{
xop = xo_default(xop);

xo_depth_change(xop, name, 1, 0, XSS_MARKER,
xop->xo_stack[xop->xo_depth].xs_flags & XSF_MARKER_FLAGS);

return 0;
}

xo_ssize_t
xo_open_marker (const char *name)
{
return xo_open_marker_h(NULL, name);
}

xo_ssize_t
xo_close_marker_h (xo_handle_t *xop, const char *name)
{
xop = xo_default(xop);

return xo_do_close(xop, name, XSS_MARKER);
}

xo_ssize_t
xo_close_marker (const char *name)
{
return xo_close_marker_h(NULL, name);
}





void
xo_set_writer (xo_handle_t *xop, void *opaque, xo_write_func_t write_func,
xo_close_func_t close_func, xo_flush_func_t flush_func)
{
xop = xo_default(xop);

xop->xo_opaque = opaque;
xop->xo_write = write_func;
xop->xo_close = close_func;
xop->xo_flush = flush_func;
}

void
xo_set_allocator (xo_realloc_func_t realloc_func, xo_free_func_t free_func)
{
xo_realloc = realloc_func;
xo_free = free_func;
}

xo_ssize_t
xo_flush_h (xo_handle_t *xop)
{
ssize_t rc;

xop = xo_default(xop);

switch (xo_style(xop)) {
case XO_STYLE_ENCODER:
xo_encoder_handle(xop, XO_OP_FLUSH, NULL, NULL, 0);
}

rc = xo_write(xop);
if (rc >= 0 && xop->xo_flush)
if (xop->xo_flush(xop->xo_opaque) < 0)
return -1;

return rc;
}

xo_ssize_t
xo_flush (void)
{
return xo_flush_h(NULL);
}

xo_ssize_t
xo_finish_h (xo_handle_t *xop)
{
const char *open_if_empty = "";
xop = xo_default(xop);

if (!XOF_ISSET(xop, XOF_NO_CLOSE))
xo_do_close_all(xop, xop->xo_stack);

switch (xo_style(xop)) {
case XO_STYLE_JSON:
if (!XOF_ISSET(xop, XOF_NO_TOP)) {
const char *pre_nl = XOF_ISSET(xop, XOF_PRETTY) ? "\n" : "";

if (XOIF_ISSET(xop, XOIF_TOP_EMITTED))
XOIF_CLEAR(xop, XOIF_TOP_EMITTED);
else if (!XOIF_ISSET(xop, XOIF_MADE_OUTPUT)) {
open_if_empty = "{ ";
pre_nl = "";
}

xo_printf(xop, "%s%*s%s}\n",
pre_nl, xo_indent(xop), "", open_if_empty);
}
break;

case XO_STYLE_ENCODER:
xo_encoder_handle(xop, XO_OP_FINISH, NULL, NULL, 0);
break;
}

return xo_flush_h(xop);
}

xo_ssize_t
xo_finish (void)
{
return xo_finish_h(NULL);
}





void
xo_finish_atexit (void)
{
(void) xo_finish_h(NULL);
}




void
xo_errorn_hv (xo_handle_t *xop, int need_newline, const char *fmt, va_list vap)
{
xop = xo_default(xop);





if (need_newline) {
ssize_t len = strlen(fmt);
if (len > 0 && fmt[len - 1] != '\n') {
char *newfmt = alloca(len + 2);
memcpy(newfmt, fmt, len);
newfmt[len] = '\n';
newfmt[len + 1] = '\0';
fmt = newfmt;
}
}

switch (xo_style(xop)) {
case XO_STYLE_TEXT:
vfprintf(stderr, fmt, vap);
break;

case XO_STYLE_HTML:
va_copy(xop->xo_vap, vap);

xo_buf_append_div(xop, "error", 0, NULL, 0, NULL, 0,
fmt, strlen(fmt), NULL, 0);

if (XOIF_ISSET(xop, XOIF_DIV_OPEN))
xo_line_close(xop);

xo_write(xop);

va_end(xop->xo_vap);
bzero(&xop->xo_vap, sizeof(xop->xo_vap));
break;

case XO_STYLE_XML:
case XO_STYLE_JSON:
va_copy(xop->xo_vap, vap);

xo_open_container_h(xop, "error");
xo_format_value(xop, "message", 7, NULL, 0,
fmt, strlen(fmt), NULL, 0, 0);
xo_close_container_h(xop, "error");

va_end(xop->xo_vap);
bzero(&xop->xo_vap, sizeof(xop->xo_vap));
break;

case XO_STYLE_SDPARAMS:
case XO_STYLE_ENCODER:
break;
}
}

void
xo_error_h (xo_handle_t *xop, const char *fmt, ...)
{
va_list vap;

va_start(vap, fmt);
xo_errorn_hv(xop, 0, fmt, vap);
va_end(vap);
}




void
xo_error (const char *fmt, ...)
{
va_list vap;

va_start(vap, fmt);
xo_errorn_hv(NULL, 0, fmt, vap);
va_end(vap);
}

void
xo_errorn_h (xo_handle_t *xop, const char *fmt, ...)
{
va_list vap;

va_start(vap, fmt);
xo_errorn_hv(xop, 1, fmt, vap);
va_end(vap);
}




void
xo_errorn (const char *fmt, ...)
{
va_list vap;

va_start(vap, fmt);
xo_errorn_hv(NULL, 1, fmt, vap);
va_end(vap);
}







int
xo_parse_args (int argc, char **argv)
{
static char libxo_opt[] = "--libxo";
char *cp;
int i, save;




if (xo_program == NULL) {

xo_program = argv[0];
cp = strrchr(xo_program, '/');
if (cp)
xo_program = ++cp;
else
cp = argv[0];






size_t len = strlen(xo_program);
static const char gnu_ext[] = ".test";
if (len >= sizeof(gnu_ext)) {
cp += len + 1 - sizeof(gnu_ext);
if (xo_streq(cp, gnu_ext))
*cp = '\0';
}
}

xo_handle_t *xop = xo_default(NULL);

for (save = i = 1; i < argc; i++) {
if (argv[i] == NULL
|| strncmp(argv[i], libxo_opt, sizeof(libxo_opt) - 1) != 0) {
if (save != i)
argv[save] = argv[i];
save += 1;
continue;
}

cp = argv[i] + sizeof(libxo_opt) - 1;
if (*cp == '\0') {
cp = argv[++i];
if (cp == NULL) {
xo_warnx("missing libxo option");
return -1;
}

if (xo_set_options(xop, cp) < 0)
return -1;
} else if (*cp == ':') {
if (xo_set_options(xop, cp) < 0)
return -1;

} else if (*cp == '=') {
if (xo_set_options(xop, ++cp) < 0)
return -1;

} else if (*cp == '-') {
cp += 1;
if (xo_streq(cp, "check")) {
exit(XO_HAS_LIBXO);

} else {
xo_warnx("unknown libxo option: '%s'", argv[i]);
return -1;
}
} else {
xo_warnx("unknown libxo option: '%s'", argv[i]);
return -1;
}
}





if (XOF_ISSET(xop, XOF_COLOR_ALLOWED) && isatty(1))
XOF_SET(xop, XOF_COLOR);

argv[save] = NULL;
return save;
}





void
xo_dump_stack (xo_handle_t *xop)
{
int i;
xo_stack_t *xsp;

xop = xo_default(xop);

fprintf(stderr, "Stack dump:\n");

xsp = xop->xo_stack;
for (i = 1, xsp++; i <= xop->xo_depth; i++, xsp++) {
fprintf(stderr, " [%d] %s '%s' [%x]\n",
i, xo_state_name(xsp->xs_state),
xsp->xs_name ?: "--", xsp->xs_flags);
}
}




void
xo_set_program (const char *name)
{
xo_program = name;
}

void
xo_set_version_h (xo_handle_t *xop, const char *version)
{
xop = xo_default(xop);

if (version == NULL || strchr(version, '"') != NULL)
return;

if (!xo_style_is_encoding(xop))
return;

switch (xo_style(xop)) {
case XO_STYLE_XML:

xo_attr_h(xop, "version", "%s", version);
break;

case XO_STYLE_JSON:




xop->xo_version = xo_strndup(version, -1);
break;

case XO_STYLE_ENCODER:
xo_encoder_handle(xop, XO_OP_VERSION, NULL, version, 0);
break;
}
}





void
xo_set_version (const char *version)
{
xo_set_version_h(NULL, version);
}






void
xo_emit_warn_hcv (xo_handle_t *xop, int as_warning, int code,
const char *fmt, va_list vap)
{
xop = xo_default(xop);

if (fmt == NULL)
return;

xo_open_marker_h(xop, "xo_emit_warn_hcv");
xo_open_container_h(xop, as_warning ? "__warning" : "__error");

if (xo_program)
xo_emit("{wc:program}", xo_program);

if (xo_style(xop) == XO_STYLE_XML || xo_style(xop) == XO_STYLE_JSON) {
va_list ap;
xo_handle_t temp;

bzero(&temp, sizeof(temp));
temp.xo_style = XO_STYLE_TEXT;
xo_buf_init(&temp.xo_data);
xo_depth_check(&temp, XO_DEPTH);

va_copy(ap, vap);
(void) xo_emit_hv(&temp, fmt, ap);
va_end(ap);

xo_buffer_t *src = &temp.xo_data;
xo_format_value(xop, "message", 7, src->xb_bufp,
src->xb_curp - src->xb_bufp, NULL, 0, NULL, 0, 0);

xo_free(temp.xo_stack);
xo_buf_cleanup(src);
}

(void) xo_emit_hv(xop, fmt, vap);

ssize_t len = strlen(fmt);
if (len > 0 && fmt[len - 1] != '\n') {
if (code > 0) {
const char *msg = strerror(code);
if (msg)
xo_emit_h(xop, ": {G:strerror}{g:error/%s}", msg);
}
xo_emit("\n");
}

xo_close_marker_h(xop, "xo_emit_warn_hcv");
xo_flush_h(xop);
}

void
xo_emit_warn_hc (xo_handle_t *xop, int code, const char *fmt, ...)
{
va_list vap;

va_start(vap, fmt);
xo_emit_warn_hcv(xop, 1, code, fmt, vap);
va_end(vap);
}

void
xo_emit_warn_c (int code, const char *fmt, ...)
{
va_list vap;

va_start(vap, fmt);
xo_emit_warn_hcv(NULL, 1, code, fmt, vap);
va_end(vap);
}

void
xo_emit_warn (const char *fmt, ...)
{
int code = errno;
va_list vap;

va_start(vap, fmt);
xo_emit_warn_hcv(NULL, 1, code, fmt, vap);
va_end(vap);
}

void
xo_emit_warnx (const char *fmt, ...)
{
va_list vap;

va_start(vap, fmt);
xo_emit_warn_hcv(NULL, 1, -1, fmt, vap);
va_end(vap);
}

void
xo_emit_err_v (int eval, int code, const char *fmt, va_list vap)
{
xo_emit_warn_hcv(NULL, 0, code, fmt, vap);
xo_finish();
exit(eval);
}

void
xo_emit_err (int eval, const char *fmt, ...)
{
int code = errno;
va_list vap;
va_start(vap, fmt);
xo_emit_err_v(eval, code, fmt, vap);

}

void
xo_emit_errx (int eval, const char *fmt, ...)
{
va_list vap;

va_start(vap, fmt);
xo_emit_err_v(eval, -1, fmt, vap);

}

void
xo_emit_errc (int eval, int code, const char *fmt, ...)
{
va_list vap;

va_start(vap, fmt);
xo_emit_err_v(eval, code, fmt, vap);

}




void *
xo_get_private (xo_handle_t *xop)
{
xop = xo_default(xop);
return xop->xo_private;
}




void
xo_set_private (xo_handle_t *xop, void *opaque)
{
xop = xo_default(xop);
xop->xo_private = opaque;
}




xo_encoder_func_t
xo_get_encoder (xo_handle_t *xop)
{
xop = xo_default(xop);
return xop->xo_encoder;
}




void
xo_set_encoder (xo_handle_t *xop, xo_encoder_func_t encoder)
{
xop = xo_default(xop);

xop->xo_style = XO_STYLE_ENCODER;
xop->xo_encoder = encoder;
}









void
xo_explicit_transition (xo_handle_t *xop, xo_state_t new_state,
const char *name, xo_xof_flags_t flags)
{
xo_xsf_flags_t xsf_flags;

xop = xo_default(xop);

switch (new_state) {

case XSS_OPEN_LIST:
xo_do_open_list(xop, flags, name);
break;

case XSS_OPEN_INSTANCE:
xo_do_open_instance(xop, flags, name);
break;

case XSS_CLOSE_INSTANCE:
xo_depth_change(xop, name, 1, 1, XSS_OPEN_INSTANCE,
xo_stack_flags(flags));
xo_stack_set_flags(xop);
xo_do_close_instance(xop, name);
break;

case XSS_CLOSE_LIST:
xsf_flags = XOF_ISSET(xop, XOF_NOT_FIRST) ? XSF_NOT_FIRST : 0;

xo_depth_change(xop, name, 1, 1, XSS_OPEN_LIST,
XSF_LIST | xsf_flags | xo_stack_flags(flags));
xo_do_close_list(xop, name);
break;
}
}
