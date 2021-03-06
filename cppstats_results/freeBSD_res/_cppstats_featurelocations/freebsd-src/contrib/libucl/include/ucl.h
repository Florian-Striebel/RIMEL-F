






















#if !defined(UCL_H_)
#define UCL_H_

#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>

#if defined(_WIN32)
#define UCL_EXTERN __declspec(dllexport)
#else
#define UCL_EXTERN
#endif





















#if defined(__cplusplus)
extern "C" {
#endif






#if !defined(UCL_ALLOC)
#define UCL_ALLOC(size) malloc(size)
#endif
#if !defined(UCL_FREE)
#define UCL_FREE(size, ptr) free(ptr)
#endif

#if __GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 4)
#define UCL_WARN_UNUSED_RESULT __attribute__((warn_unused_result))

#else
#define UCL_WARN_UNUSED_RESULT
#endif

#if defined(__GNUC__)
#define UCL_DEPRECATED(func) func __attribute__ ((deprecated))
#elif defined(_MSC_VER)
#define UCL_DEPRECATED(func) __declspec(deprecated) func
#else
#define UCL_DEPRECATED(func) func
#endif











typedef enum ucl_error {
UCL_EOK = 0,
UCL_ESYNTAX,
UCL_EIO,
UCL_ESTATE,
UCL_ENESTED,
UCL_EUNPAIRED,
UCL_EMACRO,
UCL_EINTERNAL,
UCL_ESSL,
UCL_EMERGE
} ucl_error_t;







typedef enum ucl_type {
UCL_OBJECT = 0,
UCL_ARRAY,
UCL_INT,
UCL_FLOAT,
UCL_STRING,
UCL_BOOLEAN,
UCL_TIME,
UCL_USERDATA,
UCL_NULL
} ucl_type_t;




typedef enum ucl_emitter {
UCL_EMIT_JSON = 0,
UCL_EMIT_JSON_COMPACT,
UCL_EMIT_CONFIG,
UCL_EMIT_YAML,
UCL_EMIT_MSGPACK,
UCL_EMIT_MAX
} ucl_emitter_t;







typedef enum ucl_parser_flags {
UCL_PARSER_DEFAULT = 0,
UCL_PARSER_KEY_LOWERCASE = (1 << 0),
UCL_PARSER_ZEROCOPY = (1 << 1),
UCL_PARSER_NO_TIME = (1 << 2),
UCL_PARSER_NO_IMPLICIT_ARRAYS = (1 << 3),
UCL_PARSER_SAVE_COMMENTS = (1 << 4),
UCL_PARSER_DISABLE_MACRO = (1 << 5),
UCL_PARSER_NO_FILEVARS = (1 << 6)
} ucl_parser_flags_t;




typedef enum ucl_string_flags {
UCL_STRING_RAW = 0x0,
UCL_STRING_ESCAPE = (1 << 0),
UCL_STRING_TRIM = (1 << 1),
UCL_STRING_PARSE_BOOLEAN = (1 << 2),
UCL_STRING_PARSE_INT = (1 << 3),
UCL_STRING_PARSE_DOUBLE = (1 << 4),
UCL_STRING_PARSE_TIME = (1 << 5),
UCL_STRING_PARSE_NUMBER = UCL_STRING_PARSE_INT|UCL_STRING_PARSE_DOUBLE|UCL_STRING_PARSE_TIME,

UCL_STRING_PARSE = UCL_STRING_PARSE_BOOLEAN|UCL_STRING_PARSE_NUMBER,

UCL_STRING_PARSE_BYTES = (1 << 6)
} ucl_string_flags_t;





typedef enum ucl_object_flags {
UCL_OBJECT_ALLOCATED_KEY = (1 << 0),
UCL_OBJECT_ALLOCATED_VALUE = (1 << 1),
UCL_OBJECT_NEED_KEY_ESCAPE = (1 << 2),
UCL_OBJECT_EPHEMERAL = (1 << 3),
UCL_OBJECT_MULTILINE = (1 << 4),
UCL_OBJECT_MULTIVALUE = (1 << 5),
UCL_OBJECT_INHERITED = (1 << 6),
UCL_OBJECT_BINARY = (1 << 7),
UCL_OBJECT_SQUOTED = (1 << 8)
} ucl_object_flags_t;




enum ucl_duplicate_strategy {
UCL_DUPLICATE_APPEND = 0,
UCL_DUPLICATE_MERGE,
UCL_DUPLICATE_REWRITE,
UCL_DUPLICATE_ERROR
};




enum ucl_parse_type {
UCL_PARSE_UCL = 0,
UCL_PARSE_MSGPACK,
UCL_PARSE_CSEXP,
UCL_PARSE_AUTO
};





typedef struct ucl_object_s {



union {
int64_t iv;
const char *sv;
double dv;
void *av;
void *ov;
void* ud;
} value;
const char *key;
struct ucl_object_s *next;
struct ucl_object_s *prev;
uint32_t keylen;
uint32_t len;
uint32_t ref;
uint16_t flags;
uint16_t type;
unsigned char* trash_stack[2];
} ucl_object_t;





typedef void (*ucl_userdata_dtor)(void *ud);
typedef const char* (*ucl_userdata_emitter)(void *ud);














UCL_EXTERN char* ucl_copy_key_trash (const ucl_object_t *obj);






UCL_EXTERN char* ucl_copy_value_trash (const ucl_object_t *obj);





UCL_EXTERN ucl_object_t* ucl_object_new (void) UCL_WARN_UNUSED_RESULT;






UCL_EXTERN ucl_object_t* ucl_object_typed_new (ucl_type_t type) UCL_WARN_UNUSED_RESULT;







UCL_EXTERN ucl_object_t* ucl_object_new_full (ucl_type_t type, unsigned priority)
UCL_WARN_UNUSED_RESULT;








UCL_EXTERN ucl_object_t* ucl_object_new_userdata (ucl_userdata_dtor dtor,
ucl_userdata_emitter emitter, void *ptr) UCL_WARN_UNUSED_RESULT;






UCL_EXTERN ucl_object_t * ucl_object_copy (const ucl_object_t *other)
UCL_WARN_UNUSED_RESULT;





UCL_EXTERN ucl_type_t ucl_object_type (const ucl_object_t *obj);






UCL_EXTERN const char * ucl_object_type_to_string (ucl_type_t type);







UCL_EXTERN bool ucl_object_string_to_type (const char *input, ucl_type_t *res);








UCL_EXTERN ucl_object_t * ucl_object_fromstring_common (const char *str, size_t len,
enum ucl_string_flags flags) UCL_WARN_UNUSED_RESULT;






UCL_EXTERN ucl_object_t *ucl_object_fromstring (const char *str) UCL_WARN_UNUSED_RESULT;







UCL_EXTERN ucl_object_t *ucl_object_fromlstring (const char *str,
size_t len) UCL_WARN_UNUSED_RESULT;






UCL_EXTERN ucl_object_t* ucl_object_fromint (int64_t iv) UCL_WARN_UNUSED_RESULT;






UCL_EXTERN ucl_object_t* ucl_object_fromdouble (double dv) UCL_WARN_UNUSED_RESULT;






UCL_EXTERN ucl_object_t* ucl_object_frombool (bool bv) UCL_WARN_UNUSED_RESULT;










UCL_EXTERN bool ucl_object_insert_key (ucl_object_t *top, ucl_object_t *elt,
const char *key, size_t keylen, bool copy_key);











UCL_EXTERN bool ucl_object_replace_key (ucl_object_t *top, ucl_object_t *elt,
const char *key, size_t keylen, bool copy_key);








UCL_EXTERN bool ucl_object_merge (ucl_object_t *top, ucl_object_t *elt, bool copy);







UCL_EXTERN bool ucl_object_delete_keyl (ucl_object_t *top,
const char *key, size_t keylen);






UCL_EXTERN bool ucl_object_delete_key (ucl_object_t *top,
const char *key);











UCL_EXTERN ucl_object_t* ucl_object_pop_keyl (ucl_object_t *top, const char *key,
size_t keylen) UCL_WARN_UNUSED_RESULT;









UCL_EXTERN ucl_object_t* ucl_object_pop_key (ucl_object_t *top, const char *key)
UCL_WARN_UNUSED_RESULT;











UCL_EXTERN bool ucl_object_insert_key_merged (ucl_object_t *top, ucl_object_t *elt,
const char *key, size_t keylen, bool copy_key);







UCL_EXTERN bool ucl_object_reserve (ucl_object_t *obj, size_t reserved);







UCL_EXTERN bool ucl_array_append (ucl_object_t *top,
ucl_object_t *elt);







UCL_EXTERN bool ucl_array_prepend (ucl_object_t *top,
ucl_object_t *elt);








UCL_EXTERN bool ucl_array_merge (ucl_object_t *top, ucl_object_t *elt,
bool copy);









UCL_EXTERN ucl_object_t* ucl_array_delete (ucl_object_t *top,
ucl_object_t *elt);






UCL_EXTERN const ucl_object_t* ucl_array_head (const ucl_object_t *top);






UCL_EXTERN const ucl_object_t* ucl_array_tail (const ucl_object_t *top);








UCL_EXTERN ucl_object_t* ucl_array_pop_last (ucl_object_t *top);








UCL_EXTERN ucl_object_t* ucl_array_pop_first (ucl_object_t *top);






UCL_EXTERN unsigned int ucl_array_size (const ucl_object_t *top);







UCL_EXTERN const ucl_object_t* ucl_array_find_index (const ucl_object_t *top,
unsigned int index);







UCL_EXTERN unsigned int ucl_array_index_of (ucl_object_t *top,
ucl_object_t *elt);










ucl_object_t *
ucl_array_replace_index (ucl_object_t *top, ucl_object_t *elt,
unsigned int index);







UCL_EXTERN ucl_object_t * ucl_elt_append (ucl_object_t *head,
ucl_object_t *elt);







UCL_EXTERN bool ucl_object_todouble_safe (const ucl_object_t *obj, double *target);






UCL_EXTERN double ucl_object_todouble (const ucl_object_t *obj);







UCL_EXTERN bool ucl_object_toint_safe (const ucl_object_t *obj, int64_t *target);






UCL_EXTERN int64_t ucl_object_toint (const ucl_object_t *obj);







UCL_EXTERN bool ucl_object_toboolean_safe (const ucl_object_t *obj, bool *target);






UCL_EXTERN bool ucl_object_toboolean (const ucl_object_t *obj);







UCL_EXTERN bool ucl_object_tostring_safe (const ucl_object_t *obj, const char **target);






UCL_EXTERN const char* ucl_object_tostring (const ucl_object_t *obj);






UCL_EXTERN const char* ucl_object_tostring_forced (const ucl_object_t *obj);









UCL_EXTERN bool ucl_object_tolstring_safe (const ucl_object_t *obj,
const char **target, size_t *tlen);






UCL_EXTERN const char* ucl_object_tolstring (const ucl_object_t *obj, size_t *tlen);







UCL_EXTERN const ucl_object_t* ucl_object_lookup (const ucl_object_t *obj,
const char *key);
#define ucl_object_find_key ucl_object_lookup











UCL_EXTERN const ucl_object_t* ucl_object_lookup_any (const ucl_object_t *obj,
const char *key, ...);
#define ucl_object_find_any_key ucl_object_lookup_any








UCL_EXTERN const ucl_object_t* ucl_object_lookup_len (const ucl_object_t *obj,
const char *key, size_t klen);
#define ucl_object_find_keyl ucl_object_lookup_len







UCL_EXTERN const ucl_object_t *ucl_object_lookup_path (const ucl_object_t *obj,
const char *path);
#define ucl_lookup_path ucl_object_lookup_path








UCL_EXTERN const ucl_object_t *ucl_object_lookup_path_char (const ucl_object_t *obj,
const char *path, char sep);
#define ucl_lookup_path_char ucl_object_lookup_path_char






UCL_EXTERN const char* ucl_object_key (const ucl_object_t *obj);







UCL_EXTERN const char* ucl_object_keyl (const ucl_object_t *obj, size_t *len);






UCL_EXTERN ucl_object_t* ucl_object_ref (const ucl_object_t *obj);





UCL_DEPRECATED(UCL_EXTERN void ucl_object_free (ucl_object_t *obj));





UCL_EXTERN void ucl_object_unref (ucl_object_t *obj);











UCL_EXTERN int ucl_object_compare (const ucl_object_t *o1,
const ucl_object_t *o2);











UCL_EXTERN int ucl_object_compare_qsort (const ucl_object_t **o1,
const ucl_object_t **o2);






UCL_EXTERN void ucl_object_array_sort (ucl_object_t *ar,
int (*cmp)(const ucl_object_t **o1, const ucl_object_t **o2));

enum ucl_object_keys_sort_flags {
UCL_SORT_KEYS_DEFAULT = 0,
UCL_SORT_KEYS_ICASE = (1u << 0u),
UCL_SORT_KEYS_RECURSIVE = (1u << 1u),
};





UCL_EXTERN void ucl_object_sort_keys (ucl_object_t *obj,
enum ucl_object_keys_sort_flags how);






UCL_EXTERN unsigned int ucl_object_get_priority (const ucl_object_t *obj);






UCL_EXTERN void ucl_object_set_priority (ucl_object_t *obj,
unsigned int priority);




typedef void* ucl_object_iter_t;










UCL_EXTERN const ucl_object_t* ucl_object_iterate_with_error (const ucl_object_t *obj,
ucl_object_iter_t *iter, bool expand_values, int *ep);

#define ucl_iterate_object ucl_object_iterate
#define ucl_object_iterate(ob, it, ev) ucl_object_iterate_with_error((ob), (it), (ev), NULL)






UCL_EXTERN ucl_object_iter_t ucl_object_iterate_new (const ucl_object_t *obj)
UCL_WARN_UNUSED_RESULT;







UCL_EXTERN bool ucl_object_iter_chk_excpn(ucl_object_iter_t *it);






UCL_EXTERN ucl_object_iter_t ucl_object_iterate_reset (ucl_object_iter_t it,
const ucl_object_t *obj);








UCL_EXTERN const ucl_object_t* ucl_object_iterate_safe (ucl_object_iter_t iter,
bool expand_values);



enum ucl_iterate_type {
UCL_ITERATE_EXPLICIT = 1 << 0,
UCL_ITERATE_IMPLICIT = 1 << 1,
UCL_ITERATE_BOTH = (1 << 0) | (1 << 1),
};








UCL_EXTERN const ucl_object_t* ucl_object_iterate_full (ucl_object_iter_t iter,
enum ucl_iterate_type type);





UCL_EXTERN void ucl_object_iterate_free (ucl_object_iter_t it);




















typedef bool (*ucl_macro_handler) (const unsigned char *data, size_t len,
const ucl_object_t *arguments,
void* ud);











typedef bool (*ucl_context_macro_handler) (const unsigned char *data, size_t len,
const ucl_object_t *arguments,
const ucl_object_t *context,
void* ud);


struct ucl_parser;






UCL_EXTERN struct ucl_parser* ucl_parser_new (int flags);








UCL_EXTERN bool ucl_parser_set_default_priority (struct ucl_parser *parser,
unsigned prio);






UCL_EXTERN int ucl_parser_get_default_priority (struct ucl_parser *parser);









UCL_EXTERN bool ucl_parser_register_macro (struct ucl_parser *parser,
const char *macro,
ucl_macro_handler handler, void* ud);









UCL_EXTERN bool ucl_parser_register_context_macro (struct ucl_parser *parser,
const char *macro,
ucl_context_macro_handler handler,
void* ud);











typedef bool (*ucl_variable_handler) (const unsigned char *data, size_t len,
unsigned char **replace, size_t *replace_len, bool *need_free, void* ud);







UCL_EXTERN void ucl_parser_register_variable (struct ucl_parser *parser, const char *var,
const char *value);







UCL_EXTERN void ucl_parser_set_variables_handler (struct ucl_parser *parser,
ucl_variable_handler handler, void *ud);








UCL_EXTERN bool ucl_parser_add_chunk (struct ucl_parser *parser,
const unsigned char *data, size_t len);










UCL_EXTERN bool ucl_parser_add_chunk_priority (struct ucl_parser *parser,
const unsigned char *data, size_t len, unsigned priority);








UCL_EXTERN bool ucl_parser_insert_chunk (struct ucl_parser *parser,
const unsigned char *data, size_t len);












UCL_EXTERN bool ucl_parser_add_chunk_full (struct ucl_parser *parser,
const unsigned char *data, size_t len, unsigned priority,
enum ucl_duplicate_strategy strat, enum ucl_parse_type parse_type);








UCL_EXTERN bool ucl_parser_add_string (struct ucl_parser *parser,
const char *data, size_t len);










UCL_EXTERN bool ucl_parser_add_string_priority (struct ucl_parser *parser,
const char *data, size_t len, unsigned priority);








UCL_EXTERN bool ucl_parser_add_file (struct ucl_parser *parser,
const char *filename);










UCL_EXTERN bool ucl_parser_add_file_priority (struct ucl_parser *parser,
const char *filename, unsigned priority);











UCL_EXTERN bool ucl_parser_add_file_full (struct ucl_parser *parser, const char *filename,
unsigned priority, enum ucl_duplicate_strategy strat,
enum ucl_parse_type parse_type);








UCL_EXTERN bool ucl_parser_add_fd (struct ucl_parser *parser,
int fd);










UCL_EXTERN bool ucl_parser_add_fd_priority (struct ucl_parser *parser,
int fd, unsigned priority);












UCL_EXTERN bool ucl_parser_add_fd_full (struct ucl_parser *parser, int fd,
unsigned priority, enum ucl_duplicate_strategy strat,
enum ucl_parse_type parse_type);








UCL_EXTERN bool ucl_set_include_path (struct ucl_parser *parser,
ucl_object_t *paths);







UCL_EXTERN ucl_object_t* ucl_parser_get_object (struct ucl_parser *parser);








UCL_EXTERN ucl_object_t* ucl_parser_get_current_stack_object (struct ucl_parser *parser, unsigned int depth);






UCL_EXTERN unsigned char ucl_parser_chunk_peek (struct ucl_parser *parser);






UCL_EXTERN bool ucl_parser_chunk_skip (struct ucl_parser *parser);






UCL_EXTERN const char *ucl_parser_get_error (struct ucl_parser *parser);






UCL_EXTERN int ucl_parser_get_error_code (struct ucl_parser *parser);






UCL_EXTERN unsigned ucl_parser_get_column (struct ucl_parser *parser);






UCL_EXTERN unsigned ucl_parser_get_linenum (struct ucl_parser *parser);





UCL_EXTERN void ucl_parser_clear_error (struct ucl_parser *parser);





UCL_EXTERN void ucl_parser_free (struct ucl_parser *parser);







UCL_EXTERN const ucl_object_t * ucl_parser_get_comments (struct ucl_parser *parser);







UCL_EXTERN const ucl_object_t * ucl_comments_find (const ucl_object_t *comments,
const ucl_object_t *srch);








UCL_EXTERN bool ucl_comments_move (ucl_object_t *comments,
const ucl_object_t *from, const ucl_object_t *to);







UCL_EXTERN void ucl_comments_add (ucl_object_t *comments,
const ucl_object_t *obj, const char *comment);









UCL_EXTERN bool ucl_parser_pubkey_add (struct ucl_parser *parser,
const unsigned char *key, size_t len);








UCL_EXTERN bool ucl_parser_set_filevars (struct ucl_parser *parser, const char *filename,
bool need_expand);






UCL_EXTERN const char *ucl_parser_get_cur_file (struct ucl_parser *parser);




typedef bool (*ucl_parser_special_handler_t) (struct ucl_parser *parser,
const unsigned char *source, size_t source_len,
unsigned char **destination, size_t *dest_len,
void *user_data);




enum ucl_special_handler_flags {
UCL_SPECIAL_HANDLER_DEFAULT = 0,
UCL_SPECIAL_HANDLER_PREPROCESS_ALL = (1u << 0),
};




struct ucl_parser_special_handler {
const unsigned char *magic;
size_t magic_len;
enum ucl_special_handler_flags flags;
ucl_parser_special_handler_t handler;
void (*free_function) (unsigned char *data, size_t len, void *user_data);
void *user_data;
struct ucl_parser_special_handler *next;
};






UCL_EXTERN void ucl_parser_add_special_handler (struct ucl_parser *parser,
struct ucl_parser_special_handler *handler);










typedef void (ucl_include_trace_func_t) (struct ucl_parser *parser,
const ucl_object_t *parent,
const ucl_object_t *args,
const char *path,
size_t pathlen,
void *user_data);







UCL_EXTERN void ucl_parser_set_include_tracer (struct ucl_parser *parser,
ucl_include_trace_func_t func,
void *user_data);










struct ucl_emitter_context;



struct ucl_emitter_functions {

int (*ucl_emitter_append_character) (unsigned char c, size_t nchars, void *ud);

int (*ucl_emitter_append_len) (unsigned const char *str, size_t len, void *ud);

int (*ucl_emitter_append_int) (int64_t elt, void *ud);

int (*ucl_emitter_append_double) (double elt, void *ud);

void (*ucl_emitter_free_func)(void *ud);

void *ud;
};

struct ucl_emitter_operations {

void (*ucl_emitter_write_elt) (struct ucl_emitter_context *ctx,
const ucl_object_t *obj, bool first, bool print_key);

void (*ucl_emitter_start_object) (struct ucl_emitter_context *ctx,
const ucl_object_t *obj, bool print_key);

void (*ucl_emitter_end_object) (struct ucl_emitter_context *ctx,
const ucl_object_t *obj);

void (*ucl_emitter_start_array) (struct ucl_emitter_context *ctx,
const ucl_object_t *obj, bool print_key);
void (*ucl_emitter_end_array) (struct ucl_emitter_context *ctx,
const ucl_object_t *obj);
};




struct ucl_emitter_context {

const char *name;

int id;

const struct ucl_emitter_functions *func;

const struct ucl_emitter_operations *ops;

unsigned int indent;

const ucl_object_t *top;

const ucl_object_t *comments;
};








UCL_EXTERN unsigned char *ucl_object_emit (const ucl_object_t *obj,
enum ucl_emitter emit_type);









UCL_EXTERN unsigned char *ucl_object_emit_len (const ucl_object_t *obj,
enum ucl_emitter emit_type, size_t *len);










UCL_EXTERN bool ucl_object_emit_full (const ucl_object_t *obj,
enum ucl_emitter emit_type,
struct ucl_emitter_functions *emitter,
const ucl_object_t *comments);









UCL_EXTERN struct ucl_emitter_context* ucl_object_emit_streamline_new (
const ucl_object_t *obj, enum ucl_emitter emit_type,
struct ucl_emitter_functions *emitter);






UCL_EXTERN void ucl_object_emit_streamline_start_container (
struct ucl_emitter_context *ctx, const ucl_object_t *obj);





UCL_EXTERN void ucl_object_emit_streamline_add_object (
struct ucl_emitter_context *ctx, const ucl_object_t *obj);




UCL_EXTERN void ucl_object_emit_streamline_end_container (
struct ucl_emitter_context *ctx);




UCL_EXTERN void ucl_object_emit_streamline_finish (
struct ucl_emitter_context *ctx);






UCL_EXTERN struct ucl_emitter_functions* ucl_object_emit_memory_funcs (
void **pmem);






UCL_EXTERN struct ucl_emitter_functions* ucl_object_emit_file_funcs (
FILE *fp);





UCL_EXTERN struct ucl_emitter_functions* ucl_object_emit_fd_funcs (
int fd);





UCL_EXTERN void ucl_object_emit_funcs_free (struct ucl_emitter_functions *f);













enum ucl_schema_error_code {
UCL_SCHEMA_OK = 0,
UCL_SCHEMA_TYPE_MISMATCH,
UCL_SCHEMA_INVALID_SCHEMA,
UCL_SCHEMA_MISSING_PROPERTY,
UCL_SCHEMA_CONSTRAINT,
UCL_SCHEMA_MISSING_DEPENDENCY,
UCL_SCHEMA_EXTERNAL_REF_MISSING,
UCL_SCHEMA_EXTERNAL_REF_INVALID,
UCL_SCHEMA_INTERNAL_ERROR,
UCL_SCHEMA_UNKNOWN
};




struct ucl_schema_error {
enum ucl_schema_error_code code;
char msg[128];
const ucl_object_t *obj;
};









UCL_EXTERN bool ucl_object_validate (const ucl_object_t *schema,
const ucl_object_t *obj, struct ucl_schema_error *err);










UCL_EXTERN bool ucl_object_validate_root (const ucl_object_t *schema,
const ucl_object_t *obj,
const ucl_object_t *root,
struct ucl_schema_error *err);












UCL_EXTERN bool ucl_object_validate_root_ext (const ucl_object_t *schema,
const ucl_object_t *obj,
const ucl_object_t *root,
ucl_object_t *ext_refs,
struct ucl_schema_error *err);



#if defined(__cplusplus)
}
#endif





#define ucl_obj_todouble_safe ucl_object_todouble_safe
#define ucl_obj_todouble ucl_object_todouble
#define ucl_obj_tostring ucl_object_tostring
#define ucl_obj_tostring_safe ucl_object_tostring_safe
#define ucl_obj_tolstring ucl_object_tolstring
#define ucl_obj_tolstring_safe ucl_object_tolstring_safe
#define ucl_obj_toint ucl_object_toint
#define ucl_obj_toint_safe ucl_object_toint_safe
#define ucl_obj_toboolean ucl_object_toboolean
#define ucl_obj_toboolean_safe ucl_object_toboolean_safe
#define ucl_obj_get_key ucl_object_find_key
#define ucl_obj_get_keyl ucl_object_find_keyl
#define ucl_obj_unref ucl_object_unref
#define ucl_obj_ref ucl_object_ref
#define ucl_obj_free ucl_object_free

#define UCL_PRIORITY_MIN 0
#define UCL_PRIORITY_MAX 15

#endif
