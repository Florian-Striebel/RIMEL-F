#if !defined(JEMALLOC_INTERNAL_EMITTER_H)
#define JEMALLOC_INTERNAL_EMITTER_H

#include "jemalloc/internal/ql.h"

typedef enum emitter_output_e emitter_output_t;
enum emitter_output_e {
emitter_output_json,
emitter_output_table
};

typedef enum emitter_justify_e emitter_justify_t;
enum emitter_justify_e {
emitter_justify_left,
emitter_justify_right,

emitter_justify_none
};

typedef enum emitter_type_e emitter_type_t;
enum emitter_type_e {
emitter_type_bool,
emitter_type_int,
emitter_type_unsigned,
emitter_type_uint32,
emitter_type_uint64,
emitter_type_size,
emitter_type_ssize,
emitter_type_string,




emitter_type_title,
};

typedef struct emitter_col_s emitter_col_t;
struct emitter_col_s {

emitter_justify_t justify;
int width;
emitter_type_t type;
union {
bool bool_val;
int int_val;
unsigned unsigned_val;
uint32_t uint32_val;
uint32_t uint32_t_val;
uint64_t uint64_val;
uint64_t uint64_t_val;
size_t size_val;
ssize_t ssize_val;
const char *str_val;
};


ql_elm(emitter_col_t) link;
};

typedef struct emitter_row_s emitter_row_t;
struct emitter_row_s {
ql_head(emitter_col_t) cols;
};

typedef struct emitter_s emitter_t;
struct emitter_s {
emitter_output_t output;

void (*write_cb)(void *, const char *);
void *cbopaque;
int nesting_depth;

bool item_at_depth;

bool emitted_key;
};


JEMALLOC_FORMAT_PRINTF(2, 3)
static inline void
emitter_printf(emitter_t *emitter, const char *format, ...) {
va_list ap;

va_start(ap, format);
malloc_vcprintf(emitter->write_cb, emitter->cbopaque, format, ap);
va_end(ap);
}

static inline const char * JEMALLOC_FORMAT_ARG(3)
emitter_gen_fmt(char *out_fmt, size_t out_size, const char *fmt_specifier,
emitter_justify_t justify, int width) {
size_t written;
fmt_specifier++;
if (justify == emitter_justify_none) {
written = malloc_snprintf(out_fmt, out_size,
"%%%s", fmt_specifier);
} else if (justify == emitter_justify_left) {
written = malloc_snprintf(out_fmt, out_size,
"%%-%d%s", width, fmt_specifier);
} else {
written = malloc_snprintf(out_fmt, out_size,
"%%%d%s", width, fmt_specifier);
}

assert(written < out_size);
return out_fmt;
}








static inline void
emitter_print_value(emitter_t *emitter, emitter_justify_t justify, int width,
emitter_type_t value_type, const void *value) {
size_t str_written;
#define BUF_SIZE 256
#define FMT_SIZE 10






char fmt[FMT_SIZE];
char buf[BUF_SIZE];

#define EMIT_SIMPLE(type, format) emitter_printf(emitter, emitter_gen_fmt(fmt, FMT_SIZE, format, justify, width), *(const type *)value);




switch (value_type) {
case emitter_type_bool:
emitter_printf(emitter,
emitter_gen_fmt(fmt, FMT_SIZE, "%s", justify, width),
*(const bool *)value ? "true" : "false");
break;
case emitter_type_int:
EMIT_SIMPLE(int, "%d")
break;
case emitter_type_unsigned:
EMIT_SIMPLE(unsigned, "%u")
break;
case emitter_type_ssize:
EMIT_SIMPLE(ssize_t, "%zd")
break;
case emitter_type_size:
EMIT_SIMPLE(size_t, "%zu")
break;
case emitter_type_string:
str_written = malloc_snprintf(buf, BUF_SIZE, "\"%s\"",
*(const char *const *)value);




assert(str_written < BUF_SIZE);
emitter_printf(emitter,
emitter_gen_fmt(fmt, FMT_SIZE, "%s", justify, width), buf);
break;
case emitter_type_uint32:
EMIT_SIMPLE(uint32_t, "%" FMTu32)
break;
case emitter_type_uint64:
EMIT_SIMPLE(uint64_t, "%" FMTu64)
break;
case emitter_type_title:
EMIT_SIMPLE(char *const, "%s");
break;
default:
unreachable();
}
#undef BUF_SIZE
#undef FMT_SIZE
}



static inline void
emitter_nest_inc(emitter_t *emitter) {
emitter->nesting_depth++;
emitter->item_at_depth = false;
}

static inline void
emitter_nest_dec(emitter_t *emitter) {
emitter->nesting_depth--;
emitter->item_at_depth = true;
}

static inline void
emitter_indent(emitter_t *emitter) {
int amount = emitter->nesting_depth;
const char *indent_str;
if (emitter->output == emitter_output_json) {
indent_str = "\t";
} else {
amount *= 2;
indent_str = " ";
}
for (int i = 0; i < amount; i++) {
emitter_printf(emitter, "%s", indent_str);
}
}

static inline void
emitter_json_key_prefix(emitter_t *emitter) {
if (emitter->emitted_key) {
emitter->emitted_key = false;
return;
}
emitter_printf(emitter, "%s\n", emitter->item_at_depth ? "," : "");
emitter_indent(emitter);
}




static inline void
emitter_init(emitter_t *emitter, emitter_output_t emitter_output,
void (*write_cb)(void *, const char *), void *cbopaque) {
emitter->output = emitter_output;
emitter->write_cb = write_cb;
emitter->cbopaque = cbopaque;
emitter->item_at_depth = false;
emitter->emitted_key = false;
emitter->nesting_depth = 0;
}








static inline void
emitter_json_key(emitter_t *emitter, const char *json_key) {
if (emitter->output == emitter_output_json) {
emitter_json_key_prefix(emitter);
emitter_printf(emitter, "\"%s\": ", json_key);
emitter->emitted_key = true;
}
}

static inline void
emitter_json_value(emitter_t *emitter, emitter_type_t value_type,
const void *value) {
if (emitter->output == emitter_output_json) {
emitter_json_key_prefix(emitter);
emitter_print_value(emitter, emitter_justify_none, -1,
value_type, value);
emitter->item_at_depth = true;
}
}


static inline void
emitter_json_kv(emitter_t *emitter, const char *json_key,
emitter_type_t value_type, const void *value) {
emitter_json_key(emitter, json_key);
emitter_json_value(emitter, value_type, value);
}

static inline void
emitter_json_array_begin(emitter_t *emitter) {
if (emitter->output == emitter_output_json) {
emitter_json_key_prefix(emitter);
emitter_printf(emitter, "[");
emitter_nest_inc(emitter);
}
}


static inline void
emitter_json_array_kv_begin(emitter_t *emitter, const char *json_key) {
emitter_json_key(emitter, json_key);
emitter_json_array_begin(emitter);
}

static inline void
emitter_json_array_end(emitter_t *emitter) {
if (emitter->output == emitter_output_json) {
assert(emitter->nesting_depth > 0);
emitter_nest_dec(emitter);
emitter_printf(emitter, "\n");
emitter_indent(emitter);
emitter_printf(emitter, "]");
}
}

static inline void
emitter_json_object_begin(emitter_t *emitter) {
if (emitter->output == emitter_output_json) {
emitter_json_key_prefix(emitter);
emitter_printf(emitter, "{");
emitter_nest_inc(emitter);
}
}


static inline void
emitter_json_object_kv_begin(emitter_t *emitter, const char *json_key) {
emitter_json_key(emitter, json_key);
emitter_json_object_begin(emitter);
}

static inline void
emitter_json_object_end(emitter_t *emitter) {
if (emitter->output == emitter_output_json) {
assert(emitter->nesting_depth > 0);
emitter_nest_dec(emitter);
emitter_printf(emitter, "\n");
emitter_indent(emitter);
emitter_printf(emitter, "}");
}
}





static inline void
emitter_table_dict_begin(emitter_t *emitter, const char *table_key) {
if (emitter->output == emitter_output_table) {
emitter_indent(emitter);
emitter_printf(emitter, "%s\n", table_key);
emitter_nest_inc(emitter);
}
}

static inline void
emitter_table_dict_end(emitter_t *emitter) {
if (emitter->output == emitter_output_table) {
emitter_nest_dec(emitter);
}
}

static inline void
emitter_table_kv_note(emitter_t *emitter, const char *table_key,
emitter_type_t value_type, const void *value,
const char *table_note_key, emitter_type_t table_note_value_type,
const void *table_note_value) {
if (emitter->output == emitter_output_table) {
emitter_indent(emitter);
emitter_printf(emitter, "%s: ", table_key);
emitter_print_value(emitter, emitter_justify_none, -1,
value_type, value);
if (table_note_key != NULL) {
emitter_printf(emitter, " (%s: ", table_note_key);
emitter_print_value(emitter, emitter_justify_none, -1,
table_note_value_type, table_note_value);
emitter_printf(emitter, ")");
}
emitter_printf(emitter, "\n");
}
emitter->item_at_depth = true;
}

static inline void
emitter_table_kv(emitter_t *emitter, const char *table_key,
emitter_type_t value_type, const void *value) {
emitter_table_kv_note(emitter, table_key, value_type, value, NULL,
emitter_type_bool, NULL);
}



JEMALLOC_FORMAT_PRINTF(2, 3)
static inline void
emitter_table_printf(emitter_t *emitter, const char *format, ...) {
if (emitter->output == emitter_output_table) {
va_list ap;
va_start(ap, format);
malloc_vcprintf(emitter->write_cb, emitter->cbopaque, format, ap);
va_end(ap);
}
}

static inline void
emitter_table_row(emitter_t *emitter, emitter_row_t *row) {
if (emitter->output != emitter_output_table) {
return;
}
emitter_col_t *col;
ql_foreach(col, &row->cols, link) {
emitter_print_value(emitter, col->justify, col->width,
col->type, (const void *)&col->bool_val);
}
emitter_table_printf(emitter, "\n");
}

static inline void
emitter_row_init(emitter_row_t *row) {
ql_new(&row->cols);
}

static inline void
emitter_col_init(emitter_col_t *col, emitter_row_t *row) {
ql_elm_new(col, link);
ql_tail_insert(&row->cols, col, link);
}











static inline void
emitter_kv_note(emitter_t *emitter, const char *json_key, const char *table_key,
emitter_type_t value_type, const void *value,
const char *table_note_key, emitter_type_t table_note_value_type,
const void *table_note_value) {
if (emitter->output == emitter_output_json) {
emitter_json_key(emitter, json_key);
emitter_json_value(emitter, value_type, value);
} else {
emitter_table_kv_note(emitter, table_key, value_type, value,
table_note_key, table_note_value_type, table_note_value);
}
emitter->item_at_depth = true;
}

static inline void
emitter_kv(emitter_t *emitter, const char *json_key, const char *table_key,
emitter_type_t value_type, const void *value) {
emitter_kv_note(emitter, json_key, table_key, value_type, value, NULL,
emitter_type_bool, NULL);
}

static inline void
emitter_dict_begin(emitter_t *emitter, const char *json_key,
const char *table_header) {
if (emitter->output == emitter_output_json) {
emitter_json_key(emitter, json_key);
emitter_json_object_begin(emitter);
} else {
emitter_table_dict_begin(emitter, table_header);
}
}

static inline void
emitter_dict_end(emitter_t *emitter) {
if (emitter->output == emitter_output_json) {
emitter_json_object_end(emitter);
} else {
emitter_table_dict_end(emitter);
}
}

static inline void
emitter_begin(emitter_t *emitter) {
if (emitter->output == emitter_output_json) {
assert(emitter->nesting_depth == 0);
emitter_printf(emitter, "{");
emitter_nest_inc(emitter);
} else {






emitter_printf(emitter, "%s", "");
}
}

static inline void
emitter_end(emitter_t *emitter) {
if (emitter->output == emitter_output_json) {
assert(emitter->nesting_depth == 1);
emitter_nest_dec(emitter);
emitter_printf(emitter, "\n}\n");
}
}

#endif
