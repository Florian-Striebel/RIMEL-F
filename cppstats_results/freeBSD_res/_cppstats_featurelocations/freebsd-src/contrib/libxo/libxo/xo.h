


















#if !defined(INCLUDE_XO_H)
#define INCLUDE_XO_H

#include <stdio.h>
#include <sys/types.h>
#include <stdarg.h>
#include <limits.h>
#include <stdlib.h>
#include <errno.h>

#if defined(__dead2)
#define NORETURN __dead2
#else
#define NORETURN
#endif








#if !defined(NO_PRINTFLIKE)
#if defined(__linux) && !defined(__printflike)
#define __printflike(_x, _y) __attribute__((__format__ (__printf__, _x, _y)))
#endif
#define PRINTFLIKE(_x, _y) __printflike(_x, _y)
#else
#define PRINTFLIKE(_x, _y)
#endif


typedef unsigned short xo_style_t;
#define XO_STYLE_TEXT 0
#define XO_STYLE_XML 1
#define XO_STYLE_JSON 2
#define XO_STYLE_HTML 3
#define XO_STYLE_SDPARAMS 4
#define XO_STYLE_ENCODER 5


typedef unsigned long long xo_xof_flags_t;
#define XOF_BIT(_n) ((xo_xof_flags_t) 1 << (_n))
#define XOF_CLOSE_FP XOF_BIT(0)
#define XOF_PRETTY XOF_BIT(1)
#define XOF_LOG_SYSLOG XOF_BIT(2)
#define XOF_RESV3 XOF_BIT(3)

#define XOF_WARN XOF_BIT(4)
#define XOF_XPATH XOF_BIT(5)
#define XOF_INFO XOF_BIT(6)
#define XOF_WARN_XML XOF_BIT(7)

#define XOF_NO_ENV XOF_BIT(8)
#define XOF_NO_VA_ARG XOF_BIT(9)
#define XOF_DTRT XOF_BIT(10)
#define XOF_KEYS XOF_BIT(11)

#define XOF_IGNORE_CLOSE XOF_BIT(12)
#define XOF_NOT_FIRST XOF_BIT(13)
#define XOF_NO_LOCALE XOF_BIT(14)
#define XOF_RESV15 XOF_BIT(15)

#define XOF_NO_TOP XOF_BIT(16)
#define XOF_RESV17 XOF_BIT(17)
#define XOF_UNITS XOF_BIT(18)
#define XOF_RESV19 XOF_BIT(19)

#define XOF_UNDERSCORES XOF_BIT(20)
#define XOF_COLUMNS XOF_BIT(21)
#define XOF_FLUSH XOF_BIT(22)
#define XOF_FLUSH_LINE XOF_BIT(23)

#define XOF_NO_CLOSE XOF_BIT(24)
#define XOF_COLOR_ALLOWED XOF_BIT(25)
#define XOF_COLOR XOF_BIT(26)
#define XOF_NO_HUMANIZE XOF_BIT(27)

#define XOF_LOG_GETTEXT XOF_BIT(28)
#define XOF_UTF8 XOF_BIT(29)
#define XOF_RETAIN_ALL XOF_BIT(30)
#define XOF_RETAIN_NONE XOF_BIT(31)

#define XOF_COLOR_MAP XOF_BIT(32)
#define XOF_CONTINUATION XOF_BIT(33)

typedef unsigned xo_emit_flags_t;
#define XOEF_RETAIN (1<<0)





typedef struct xo_info_s {
const char *xi_name;
const char *xi_type;
const char *xi_help;
} xo_info_t;

#define XO_INFO_NULL NULL, NULL, NULL

struct xo_handle_s;
typedef struct xo_handle_s xo_handle_t;






#if defined(XO_USE_INT_RETURN_CODES)
typedef int xo_ssize_t;
#else
typedef ssize_t xo_ssize_t;
#endif

typedef xo_ssize_t (*xo_write_func_t)(void *, const char *);
typedef void (*xo_close_func_t)(void *);
typedef int (*xo_flush_func_t)(void *);
typedef void *(*xo_realloc_func_t)(void *, size_t);
typedef void (*xo_free_func_t)(void *);






typedef xo_ssize_t (*xo_formatter_t)(xo_handle_t *, char *, xo_ssize_t,
const char *, va_list);
typedef void (*xo_checkpointer_t)(xo_handle_t *, va_list, int);

xo_handle_t *
xo_create (xo_style_t style, xo_xof_flags_t flags);

xo_handle_t *
xo_create_to_file (FILE *fp, xo_style_t style, xo_xof_flags_t flags);

void
xo_destroy (xo_handle_t *xop);

void
xo_set_writer (xo_handle_t *xop, void *opaque, xo_write_func_t write_func,
xo_close_func_t close_func, xo_flush_func_t flush_func);

void
xo_set_allocator (xo_realloc_func_t realloc_func, xo_free_func_t free_func);

void
xo_set_style (xo_handle_t *xop, xo_style_t style);

xo_style_t
xo_get_style (xo_handle_t *xop);

int
xo_set_style_name (xo_handle_t *xop, const char *style);

int
xo_set_options (xo_handle_t *xop, const char *input);

xo_xof_flags_t
xo_get_flags (xo_handle_t *xop);

void
xo_set_flags (xo_handle_t *xop, xo_xof_flags_t flags);

void
xo_clear_flags (xo_handle_t *xop, xo_xof_flags_t flags);

int
xo_set_file_h (xo_handle_t *xop, FILE *fp);

int
xo_set_file (FILE *fp);

void
xo_set_info (xo_handle_t *xop, xo_info_t *infop, int count);

void
xo_set_formatter (xo_handle_t *xop, xo_formatter_t func, xo_checkpointer_t);

void
xo_set_depth (xo_handle_t *xop, int depth);

xo_ssize_t
xo_emit_hv (xo_handle_t *xop, const char *fmt, va_list vap);

xo_ssize_t
xo_emit_h (xo_handle_t *xop, const char *fmt, ...);

xo_ssize_t
xo_emit (const char *fmt, ...);

xo_ssize_t
xo_emit_hvf (xo_handle_t *xop, xo_emit_flags_t flags,
const char *fmt, va_list vap);

xo_ssize_t
xo_emit_hf (xo_handle_t *xop, xo_emit_flags_t flags, const char *fmt, ...);

xo_ssize_t
xo_emit_f (xo_emit_flags_t flags, const char *fmt, ...);

PRINTFLIKE(2, 0)
static inline xo_ssize_t
xo_emit_hvp (xo_handle_t *xop, const char *fmt, va_list vap)
{
return xo_emit_hv(xop, fmt, vap);
}

PRINTFLIKE(2, 3)
static inline xo_ssize_t
xo_emit_hp (xo_handle_t *xop, const char *fmt, ...)
{
va_list vap;
va_start(vap, fmt);
xo_ssize_t rc = xo_emit_hv(xop, fmt, vap);
va_end(vap);
return rc;
}

PRINTFLIKE(1, 2)
static inline xo_ssize_t
xo_emit_p (const char *fmt, ...)
{
va_list vap;
va_start(vap, fmt);
xo_ssize_t rc = xo_emit_hv(NULL, fmt, vap);
va_end(vap);
return rc;
}

PRINTFLIKE(3, 0)
static inline xo_ssize_t
xo_emit_hvfp (xo_handle_t *xop, xo_emit_flags_t flags,
const char *fmt, va_list vap)
{
return xo_emit_hvf(xop, flags, fmt, vap);
}

PRINTFLIKE(3, 4)
static inline xo_ssize_t
xo_emit_hfp (xo_handle_t *xop, xo_emit_flags_t flags, const char *fmt, ...)
{
va_list vap;
va_start(vap, fmt);
xo_ssize_t rc = xo_emit_hvf(xop, flags, fmt, vap);
va_end(vap);
return rc;
}

PRINTFLIKE(2, 3)
static inline xo_ssize_t
xo_emit_fp (xo_emit_flags_t flags, const char *fmt, ...)
{
va_list vap;
va_start(vap, fmt);
xo_ssize_t rc = xo_emit_hvf(NULL, flags, fmt, vap);
va_end(vap);
return rc;
}

xo_ssize_t
xo_open_container_hf (xo_handle_t *xop, xo_xof_flags_t flags, const char *name);

xo_ssize_t
xo_open_container_h (xo_handle_t *xop, const char *name);

xo_ssize_t
xo_open_container (const char *name);

xo_ssize_t
xo_open_container_hd (xo_handle_t *xop, const char *name);

xo_ssize_t
xo_open_container_d (const char *name);

xo_ssize_t
xo_close_container_h (xo_handle_t *xop, const char *name);

xo_ssize_t
xo_close_container (const char *name);

xo_ssize_t
xo_close_container_hd (xo_handle_t *xop);

xo_ssize_t
xo_close_container_d (void);

xo_ssize_t
xo_open_list_hf (xo_handle_t *xop, xo_xof_flags_t flags, const char *name);

xo_ssize_t
xo_open_list_h (xo_handle_t *xop, const char *name);

xo_ssize_t
xo_open_list (const char *name);

xo_ssize_t
xo_open_list_hd (xo_handle_t *xop, const char *name);

xo_ssize_t
xo_open_list_d (const char *name);

xo_ssize_t
xo_close_list_h (xo_handle_t *xop, const char *name);

xo_ssize_t
xo_close_list (const char *name);

xo_ssize_t
xo_close_list_hd (xo_handle_t *xop);

xo_ssize_t
xo_close_list_d (void);

xo_ssize_t
xo_open_instance_hf (xo_handle_t *xop, xo_xof_flags_t flags, const char *name);

xo_ssize_t
xo_open_instance_h (xo_handle_t *xop, const char *name);

xo_ssize_t
xo_open_instance (const char *name);

xo_ssize_t
xo_open_instance_hd (xo_handle_t *xop, const char *name);

xo_ssize_t
xo_open_instance_d (const char *name);

xo_ssize_t
xo_close_instance_h (xo_handle_t *xop, const char *name);

xo_ssize_t
xo_close_instance (const char *name);

xo_ssize_t
xo_close_instance_hd (xo_handle_t *xop);

xo_ssize_t
xo_close_instance_d (void);

xo_ssize_t
xo_open_marker_h (xo_handle_t *xop, const char *name);

xo_ssize_t
xo_open_marker (const char *name);

xo_ssize_t
xo_close_marker_h (xo_handle_t *xop, const char *name);

xo_ssize_t
xo_close_marker (const char *name);

xo_ssize_t
xo_attr_h (xo_handle_t *xop, const char *name, const char *fmt, ...);

xo_ssize_t
xo_attr_hv (xo_handle_t *xop, const char *name, const char *fmt, va_list vap);

xo_ssize_t
xo_attr (const char *name, const char *fmt, ...);

void
xo_error_hv (xo_handle_t *xop, const char *fmt, va_list vap);

void
xo_error_h (xo_handle_t *xop, const char *fmt, ...);

void
xo_error (const char *fmt, ...);

void
xo_errorn_hv (xo_handle_t *xop, int need_newline, const char *fmt, va_list vap);

void
xo_errorn_h (xo_handle_t *xop, const char *fmt, ...);

void
xo_errorn (const char *fmt, ...);

xo_ssize_t
xo_flush_h (xo_handle_t *xop);

xo_ssize_t
xo_flush (void);

xo_ssize_t
xo_finish_h (xo_handle_t *xop);

xo_ssize_t
xo_finish (void);

void
xo_finish_atexit (void);

void
xo_set_leading_xpath (xo_handle_t *xop, const char *path);

void
xo_warn_hc (xo_handle_t *xop, int code, const char *fmt, ...) PRINTFLIKE(3, 4);

void
xo_warn_c (int code, const char *fmt, ...) PRINTFLIKE(2, 3);

void
xo_warn (const char *fmt, ...) PRINTFLIKE(1, 2);

void
xo_warnx (const char *fmt, ...) PRINTFLIKE(1, 2);

void
xo_err (int eval, const char *fmt, ...) NORETURN PRINTFLIKE(2, 3);

void
xo_errx (int eval, const char *fmt, ...) NORETURN PRINTFLIKE(2, 3);

void
xo_errc (int eval, int code, const char *fmt, ...) NORETURN PRINTFLIKE(3, 4);

void
xo_message_hcv (xo_handle_t *xop, int code, const char *fmt, va_list vap) PRINTFLIKE(3, 0);

void
xo_message_hc (xo_handle_t *xop, int code, const char *fmt, ...) PRINTFLIKE(3, 4);

void
xo_message_c (int code, const char *fmt, ...) PRINTFLIKE(2, 3);

void
xo_message_e (const char *fmt, ...) PRINTFLIKE(1, 2);

void
xo_message (const char *fmt, ...) PRINTFLIKE(1, 2);

void
xo_emit_warn_hcv (xo_handle_t *xop, int as_warning, int code,
const char *fmt, va_list vap);

void
xo_emit_warn_hc (xo_handle_t *xop, int code, const char *fmt, ...);

void
xo_emit_warn_c (int code, const char *fmt, ...);

void
xo_emit_warn (const char *fmt, ...);

void
xo_emit_warnx (const char *fmt, ...);

void
xo_emit_err (int eval, const char *fmt, ...) NORETURN;

void
xo_emit_errx (int eval, const char *fmt, ...) NORETURN;

void
xo_emit_errc (int eval, int code, const char *fmt, ...) NORETURN;

PRINTFLIKE(4, 0)
static inline void
xo_emit_warn_hcvp (xo_handle_t *xop, int as_warning, int code,
const char *fmt, va_list vap)
{
xo_emit_warn_hcv(xop, as_warning, code, fmt, vap);
}

PRINTFLIKE(3, 4)
static inline void
xo_emit_warn_hcp (xo_handle_t *xop, int code, const char *fmt, ...)
{
va_list vap;
va_start(vap, fmt);
xo_emit_warn_hcv(xop, 1, code, fmt, vap);
va_end(vap);
}

PRINTFLIKE(2, 3)
static inline void
xo_emit_warn_cp (int code, const char *fmt, ...)
{
va_list vap;
va_start(vap, fmt);
xo_emit_warn_hcv(NULL, 1, code, fmt, vap);
va_end(vap);
}

PRINTFLIKE(1, 2)
static inline void
xo_emit_warn_p (const char *fmt, ...)
{
int code = errno;
va_list vap;
va_start(vap, fmt);
xo_emit_warn_hcv(NULL, 1, code, fmt, vap);
va_end(vap);
}

PRINTFLIKE(1, 2)
static inline void
xo_emit_warnx_p (const char *fmt, ...)
{
va_list vap;
va_start(vap, fmt);
xo_emit_warn_hcv(NULL, 1, -1, fmt, vap);
va_end(vap);
}

NORETURN PRINTFLIKE(2, 3)
static inline void
xo_emit_err_p (int eval, const char *fmt, ...)
{
int code = errno;
va_list vap;
va_start(vap, fmt);
xo_emit_warn_hcv(NULL, 0, code, fmt, vap);
va_end(vap);

exit(eval);
}

PRINTFLIKE(2, 3)
static inline void
xo_emit_errx_p (int eval, const char *fmt, ...)
{
va_list vap;
va_start(vap, fmt);
xo_emit_warn_hcv(NULL, 0, -1, fmt, vap);
va_end(vap);
exit(eval);
}

PRINTFLIKE(3, 4)
static inline void
xo_emit_errc_p (int eval, int code, const char *fmt, ...)
{
va_list vap;
va_start(vap, fmt);
xo_emit_warn_hcv(NULL, 0, code, fmt, vap);
va_end(vap);
exit(eval);
}

void
xo_emit_err_v (int eval, int code, const char *fmt, va_list vap) NORETURN PRINTFLIKE(3, 0);

void
xo_no_setlocale (void);














int
xo_parse_args (int argc, char **argv);









#define XO_HAS_LIBXO 121




extern const char xo_version[];
extern const char xo_version_extra[];










void
xo_dump_stack (xo_handle_t *xop);











void
xo_set_program (const char *name);










void
xo_set_version (const char *version);






void
xo_set_version_h (xo_handle_t *xop, const char *version);

void
xo_open_log (const char *ident, int logopt, int facility);

void
xo_close_log (void);

int
xo_set_logmask (int maskpri);

void
xo_set_unit_test_mode (int value);

void
xo_syslog (int priority, const char *name, const char *message, ...);

void
xo_vsyslog (int priority, const char *name, const char *message, va_list args);

typedef void (*xo_syslog_open_t)(void);
typedef void (*xo_syslog_send_t)(const char *full_msg,
const char *v0_hdr, const char *text_only);
typedef void (*xo_syslog_close_t)(void);

void
xo_set_syslog_handler (xo_syslog_open_t open_func, xo_syslog_send_t send_func,
xo_syslog_close_t close_func);

void
xo_set_syslog_enterprise_id (unsigned short eid);

typedef void (*xo_simplify_field_func_t)(const char *, unsigned, int);

char *
xo_simplify_format (xo_handle_t *xop, const char *fmt, int with_numbers,
xo_simplify_field_func_t field_cb);

xo_ssize_t
xo_emit_field_hv (xo_handle_t *xop, const char *rolmod, const char *contents,
const char *fmt, const char *efmt,
va_list vap);

xo_ssize_t
xo_emit_field_h (xo_handle_t *xop, const char *rolmod, const char *contents,
const char *fmt, const char *efmt, ...);

xo_ssize_t
xo_emit_field (const char *rolmod, const char *contents,
const char *fmt, const char *efmt, ...);

void
xo_retain_clear_all (void);

void
xo_retain_clear (const char *fmt);

#endif
