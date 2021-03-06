
















#if !defined(XO_ENCODER_H)
#define XO_ENCODER_H

#include <string.h>




extern xo_realloc_func_t xo_realloc;
extern xo_free_func_t xo_free;





static inline int
xo_streq (const char *one, const char *two)
{
return strcmp(one, two) == 0;
}


typedef unsigned long xo_xff_flags_t;
#define XFF_COLON (1<<0)
#define XFF_COMMA (1<<1)
#define XFF_WS (1<<2)
#define XFF_ENCODE_ONLY (1<<3)

#define XFF_QUOTE (1<<4)
#define XFF_NOQUOTE (1<<5)
#define XFF_DISPLAY_ONLY (1<<6)
#define XFF_KEY (1<<7)

#define XFF_XML (1<<8)
#define XFF_ATTR (1<<9)
#define XFF_BLANK_LINE (1<<10)
#define XFF_NO_OUTPUT (1<<11)

#define XFF_TRIM_WS (1<<12)
#define XFF_LEAF_LIST (1<<13)
#define XFF_UNESCAPE (1<<14)
#define XFF_HUMANIZE (1<<15)

#define XFF_HN_SPACE (1<<16)
#define XFF_HN_DECIMAL (1<<17)
#define XFF_HN_1000 (1<<18)
#define XFF_GT_FIELD (1<<19)

#define XFF_GT_PLURAL (1<<20)
#define XFF_ARGUMENT (1<<21)


#define XFF_GT_FLAGS (XFF_GT_FIELD | XFF_GT_PLURAL)

typedef unsigned xo_encoder_op_t;


#define XO_OP_UNKNOWN 0
#define XO_OP_CREATE 1
#define XO_OP_OPEN_CONTAINER 2
#define XO_OP_CLOSE_CONTAINER 3
#define XO_OP_OPEN_LIST 4
#define XO_OP_CLOSE_LIST 5
#define XO_OP_OPEN_LEAF_LIST 6
#define XO_OP_CLOSE_LEAF_LIST 7
#define XO_OP_OPEN_INSTANCE 8
#define XO_OP_CLOSE_INSTANCE 9
#define XO_OP_STRING 10
#define XO_OP_CONTENT 11
#define XO_OP_FINISH 12
#define XO_OP_FLUSH 13
#define XO_OP_DESTROY 14
#define XO_OP_ATTRIBUTE 15
#define XO_OP_VERSION 16
#define XO_OP_OPTIONS 17
#define XO_OP_OPTIONS_PLUS 18

#define XO_ENCODER_HANDLER_ARGS xo_handle_t *xop __attribute__ ((__unused__)), xo_encoder_op_t op __attribute__ ((__unused__)), const char *name __attribute__ ((__unused__)), const char *value __attribute__ ((__unused__)), void *private __attribute__ ((__unused__)), xo_xff_flags_t flags __attribute__ ((__unused__))







typedef int (*xo_encoder_func_t)(XO_ENCODER_HANDLER_ARGS);

typedef struct xo_encoder_init_args_s {
unsigned xei_version;
xo_encoder_func_t xei_handler;
} xo_encoder_init_args_t;

#define XO_ENCODER_VERSION 1

#define XO_ENCODER_INIT_ARGS xo_encoder_init_args_t *arg __attribute__ ((__unused__))


typedef int (*xo_encoder_init_func_t)(XO_ENCODER_INIT_ARGS);





#define XO_ENCODER_INIT_NAME_TOKEN xo_encoder_library_init
#define XO_STRINGIFY(_x) #_x
#define XO_STRINGIFY2(_x) XO_STRINGIFY(_x)
#define XO_ENCODER_INIT_NAME XO_STRINGIFY2(XO_ENCODER_INIT_NAME_TOKEN)
extern int XO_ENCODER_INIT_NAME_TOKEN (XO_ENCODER_INIT_ARGS);

void
xo_encoder_register (const char *name, xo_encoder_func_t func);

void
xo_encoder_unregister (const char *name);

void *
xo_get_private (xo_handle_t *xop);

void
xo_encoder_path_add (const char *path);

void
xo_set_private (xo_handle_t *xop, void *opaque);

xo_encoder_func_t
xo_get_encoder (xo_handle_t *xop);

void
xo_set_encoder (xo_handle_t *xop, xo_encoder_func_t encoder);

int
xo_encoder_init (xo_handle_t *xop, const char *name);

xo_handle_t *
xo_encoder_create (const char *name, xo_xof_flags_t flags);

int
xo_encoder_handle (xo_handle_t *xop, xo_encoder_op_t op,
const char *name, const char *value, xo_xff_flags_t flags);

void
xo_encoders_clean (void);

const char *
xo_encoder_op_name (xo_encoder_op_t op);




void
xo_failure (xo_handle_t *xop, const char *fmt, ...);

#endif
