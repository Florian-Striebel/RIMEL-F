






















#if !defined(UCL_INTERNAL_H_)
#define UCL_INTERNAL_H_

#if defined(HAVE_CONFIG_H)
#include "config.h"
#else

#define HAVE_SYS_TYPES_H
#define HAVE_SYS_MMAN_H
#define HAVE_SYS_STAT_H
#define HAVE_SYS_PARAM_H
#define HAVE_LIMITS_H
#define HAVE_FCNTL_H
#define HAVE_ERRNO_H
#define HAVE_UNISTD_H
#define HAVE_CTYPE_H
#define HAVE_STDIO_H
#define HAVE_STRING_H
#define HAVE_FLOAT_H
#define HAVE_LIBGEN_H
#define HAVE_MATH_H
#define HAVE_STDBOOL_H
#define HAVE_STDINT_H
#define HAVE_STDARG_H
#if !defined(_WIN32)
#define HAVE_REGEX_H
#endif
#endif

#if defined(HAVE_SYS_TYPES_H)
#include <sys/types.h>
#endif

#if defined(HAVE_SYS_MMAN_H)
#if !defined(_WIN32)
#include <sys/mman.h>
#endif
#endif
#if defined(HAVE_SYS_STAT_H)
#include <sys/stat.h>
#endif
#if defined(HAVE_SYS_PARAM_H)
#if !defined(_WIN32)
#include <sys/param.h>
#endif
#endif

#if defined(HAVE_LIMITS_H)
#include <limits.h>
#endif
#if defined(HAVE_FCNTL_H)
#include <fcntl.h>
#endif
#if defined(HAVE_ERRNO_H)
#include <errno.h>
#endif
#if defined(HAVE_UNISTD_H)
#if !defined(_WIN32)
#include <unistd.h>
#endif
#endif
#if defined(HAVE_CTYPE_H)
#include <ctype.h>
#endif
#if defined(HAVE_STDIO_H)
#include <stdio.h>
#endif
#if defined(HAVE_STRING_H)
#include <string.h>
#endif
#if defined(HAVE_STRINGS_H)
#include <strings.h>
#endif

#if defined(_MSC_VER)

#include <BaseTsd.h>
#include <inttypes.h>
typedef SSIZE_T ssize_t;
#define strdup _strdup
#define snprintf _snprintf
#define vsnprintf _vsnprintf
#define strcasecmp _stricmp
#define strncasecmp _strnicmp
#define S_ISREG(m) (((m) & S_IFMT) == S_IFREG)
#define S_ISDIR(m) (((m) & S_IFMT) == S_IFDIR)
#if _MSC_VER >= 1900
#include <../ucrt/stdlib.h>
#else
#include <../include/stdlib.h>
#endif
#if !defined(PATH_MAX)
#define PATH_MAX _MAX_PATH
#endif




#endif

#include "utlist.h"
#include "utstring.h"
#include "uthash.h"
#include "ucl.h"
#include "ucl_hash.h"

#if defined(HAVE_OPENSSL)
#include <openssl/evp.h>
#endif

#if !defined(__DECONST)
#define __DECONST(type, var) ((type)(uintptr_t)(const void *)(var))
#endif






#define UCL_MAX_RECURSION 16
#define UCL_TRASH_KEY 0
#define UCL_TRASH_VALUE 1

enum ucl_parser_state {
UCL_STATE_INIT = 0,
UCL_STATE_OBJECT,
UCL_STATE_ARRAY,
UCL_STATE_KEY,
UCL_STATE_KEY_OBRACE,
UCL_STATE_VALUE,
UCL_STATE_AFTER_VALUE,
UCL_STATE_ARRAY_VALUE,
UCL_STATE_SCOMMENT,
UCL_STATE_MCOMMENT,
UCL_STATE_MACRO_NAME,
UCL_STATE_MACRO,
UCL_STATE_ERROR
};

enum ucl_character_type {
UCL_CHARACTER_DENIED = (1 << 0),
UCL_CHARACTER_KEY = (1 << 1),
UCL_CHARACTER_KEY_START = (1 << 2),
UCL_CHARACTER_WHITESPACE = (1 << 3),
UCL_CHARACTER_WHITESPACE_UNSAFE = (1 << 4),
UCL_CHARACTER_VALUE_END = (1 << 5),
UCL_CHARACTER_VALUE_STR = (1 << 6),
UCL_CHARACTER_VALUE_DIGIT = (1 << 7),
UCL_CHARACTER_VALUE_DIGIT_START = (1 << 8),
UCL_CHARACTER_ESCAPE = (1 << 9),
UCL_CHARACTER_KEY_SEP = (1 << 10),
UCL_CHARACTER_JSON_UNSAFE = (1 << 11),
UCL_CHARACTER_UCL_UNSAFE = (1 << 12)
};

struct ucl_macro {
char *name;
union _ucl_macro {
ucl_macro_handler handler;
ucl_context_macro_handler context_handler;
} h;
void* ud;
bool is_context;
UT_hash_handle hh;
};

enum ucl_stack_flags {
UCL_STACK_HAS_OBRACE = (1u << 0),
UCL_STACK_MAX = (1u << 1),
};

struct ucl_stack {
ucl_object_t *obj;
struct ucl_stack *next;
union {
struct {
uint16_t level;
uint16_t flags;
uint32_t line;
} params;
uint64_t len;
} e;
struct ucl_chunk *chunk;
};

struct ucl_parser_special_handler_chain {
unsigned char *begin;
size_t len;
struct ucl_parser_special_handler *special_handler;
struct ucl_parser_special_handler_chain *next;
};

struct ucl_chunk {
const unsigned char *begin;
const unsigned char *end;
const unsigned char *pos;
char *fname;
size_t remain;
unsigned int line;
unsigned int column;
unsigned priority;
enum ucl_duplicate_strategy strategy;
enum ucl_parse_type parse_type;
struct ucl_parser_special_handler_chain *special_handlers;
struct ucl_chunk *next;
};

#if defined(HAVE_OPENSSL)
struct ucl_pubkey {
EVP_PKEY *key;
struct ucl_pubkey *next;
};
#else
struct ucl_pubkey {
struct ucl_pubkey *next;
};
#endif

struct ucl_variable {
char *var;
char *value;
size_t var_len;
size_t value_len;
struct ucl_variable *prev, *next;
};

struct ucl_parser {
enum ucl_parser_state state;
enum ucl_parser_state prev_state;
unsigned int recursion;
int flags;
unsigned default_priority;
int err_code;
ucl_object_t *top_obj;
ucl_object_t *cur_obj;
ucl_object_t *trash_objs;
ucl_object_t *includepaths;
char *cur_file;
struct ucl_macro *macroes;
struct ucl_stack *stack;
struct ucl_chunk *chunks;
struct ucl_pubkey *keys;
struct ucl_parser_special_handler *special_handlers;
ucl_include_trace_func_t *include_trace_func;
void *include_trace_ud;
struct ucl_variable *variables;
ucl_variable_handler var_handler;
void *var_data;
ucl_object_t *comments;
ucl_object_t *last_comment;
UT_string *err;
};

struct ucl_object_userdata {
ucl_object_t obj;
ucl_userdata_dtor dtor;
ucl_userdata_emitter emitter;
};





size_t ucl_unescape_json_string (char *str, size_t len);






size_t ucl_unescape_squoted_string (char *str, size_t len);









bool ucl_include_handler (const unsigned char *data, size_t len,
const ucl_object_t *args, void* ud);









bool ucl_try_include_handler (const unsigned char *data, size_t len,
const ucl_object_t *args, void* ud);









bool ucl_includes_handler (const unsigned char *data, size_t len,
const ucl_object_t *args, void* ud);









bool ucl_priority_handler (const unsigned char *data, size_t len,
const ucl_object_t *args, void* ud);









bool ucl_load_handler (const unsigned char *data, size_t len,
const ucl_object_t *args, void* ud);









bool ucl_inherit_handler (const unsigned char *data, size_t len,
const ucl_object_t *args, const ucl_object_t *ctx, void* ud);

size_t ucl_strlcpy (char *dst, const char *src, size_t siz);
size_t ucl_strlcpy_unsafe (char *dst, const char *src, size_t siz);
size_t ucl_strlcpy_tolower (char *dst, const char *src, size_t siz);

char *ucl_strnstr (const char *s, const char *find, int len);
char *ucl_strncasestr (const char *s, const char *find, int len);

#if defined(__GNUC__)
static inline void
ucl_create_err (UT_string **err, const char *fmt, ...)
__attribute__ (( format( printf, 2, 3) ));
#endif

#undef UCL_FATAL_ERRORS

static inline void
ucl_create_err (UT_string **err, const char *fmt, ...)
{
if (*err == NULL) {
utstring_new (*err);
va_list ap;
va_start (ap, fmt);
utstring_printf_va (*err, fmt, ap);
va_end (ap);
}

#if defined(UCL_FATAL_ERRORS)
assert (0);
#endif
}








static inline bool
ucl_maybe_parse_boolean (ucl_object_t *obj, const unsigned char *start, size_t len)
{
const char *p = (const char *)start;
bool ret = false, val = false;

if (len == 5) {
if ((p[0] == 'f' || p[0] == 'F') && strncasecmp (p, "false", 5) == 0) {
ret = true;
val = false;
}
}
else if (len == 4) {
if ((p[0] == 't' || p[0] == 'T') && strncasecmp (p, "true", 4) == 0) {
ret = true;
val = true;
}
}
else if (len == 3) {
if ((p[0] == 'y' || p[0] == 'Y') && strncasecmp (p, "yes", 3) == 0) {
ret = true;
val = true;
}
else if ((p[0] == 'o' || p[0] == 'O') && strncasecmp (p, "off", 3) == 0) {
ret = true;
val = false;
}
}
else if (len == 2) {
if ((p[0] == 'n' || p[0] == 'N') && strncasecmp (p, "no", 2) == 0) {
ret = true;
val = false;
}
else if ((p[0] == 'o' || p[0] == 'O') && strncasecmp (p, "on", 2) == 0) {
ret = true;
val = true;
}
}

if (ret && obj != NULL) {
obj->type = UCL_BOOLEAN;
obj->value.iv = val;
}

return ret;
}










int ucl_maybe_parse_number (ucl_object_t *obj,
const char *start, const char *end, const char **pos,
bool allow_double, bool number_bytes, bool allow_time);


static inline const ucl_object_t *
ucl_hash_search_obj (ucl_hash_t* hashlin, ucl_object_t *obj)
{
return (const ucl_object_t *)ucl_hash_search (hashlin, obj->key, obj->keylen);
}

static inline ucl_hash_t * ucl_hash_insert_object (ucl_hash_t *hashlin,
const ucl_object_t *obj,
bool ignore_case) UCL_WARN_UNUSED_RESULT;

static inline ucl_hash_t *
ucl_hash_insert_object (ucl_hash_t *hashlin,
const ucl_object_t *obj,
bool ignore_case)
{
ucl_hash_t *nhp;

if (hashlin == NULL) {
nhp = ucl_hash_create (ignore_case);
if (nhp == NULL) {
return NULL;
}
} else {
nhp = hashlin;
}
if (!ucl_hash_insert (nhp, obj, obj->key, obj->keylen)) {
if (nhp != hashlin) {
ucl_hash_destroy(nhp, NULL);
}
return NULL;
}

return nhp;
}






const struct ucl_emitter_context *
ucl_emit_get_standard_context (enum ucl_emitter emit_type);






void ucl_elt_string_write_json (const char *str, size_t size,
struct ucl_emitter_context *ctx);







void
ucl_elt_string_write_squoted (const char *str, size_t size,
struct ucl_emitter_context *ctx);







void ucl_elt_string_write_multiline (const char *str, size_t size,
struct ucl_emitter_context *ctx);






unsigned char * ucl_object_emit_single_json (const ucl_object_t *obj);







bool ucl_maybe_long_string (const ucl_object_t *obj);






void ucl_emitter_print_int_msgpack (struct ucl_emitter_context *ctx,
int64_t val);





void ucl_emitter_print_double_msgpack (struct ucl_emitter_context *ctx,
double val);





void ucl_emitter_print_bool_msgpack (struct ucl_emitter_context *ctx,
bool val);






void ucl_emitter_print_string_msgpack (struct ucl_emitter_context *ctx,
const char *s, size_t len);







void ucl_emitter_print_binary_string_msgpack (struct ucl_emitter_context *ctx,
const char *s, size_t len);






void ucl_emitter_print_array_msgpack (struct ucl_emitter_context *ctx,
size_t len);






void ucl_emitter_print_object_msgpack (struct ucl_emitter_context *ctx,
size_t len);




void ucl_emitter_print_null_msgpack (struct ucl_emitter_context *ctx);






void ucl_emitter_print_key_msgpack (bool print_key,
struct ucl_emitter_context *ctx,
const ucl_object_t *obj);









bool ucl_fetch_url (const unsigned char *url,
unsigned char **buf,
size_t *buflen,
UT_string **err,
bool must_exist);









bool ucl_fetch_file (const unsigned char *filename,
unsigned char **buf,
size_t *buflen,
UT_string **err,
bool must_exist);







bool ucl_parser_process_object_element (struct ucl_parser *parser,
ucl_object_t *nobj);






bool ucl_parse_msgpack (struct ucl_parser *parser);

bool ucl_parse_csexp (struct ucl_parser *parser);





void ucl_chunk_free (struct ucl_chunk *chunk);

#endif
