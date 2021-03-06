
















#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/queue.h>
#include <sys/param.h>
#include <dlfcn.h>

#include "xo_config.h"
#include "xo.h"
#include "xo_encoder.h"

#if defined(HAVE_DLFCN_H)
#include <dlfcn.h>
#if !defined(HAVE_DLFUNC)
#define dlfunc(_p, _n) dlsym(_p, _n)
#endif
#else
#define dlopen(_n, _f) NULL
#define dlsym(_p, _n) NULL
#define dlfunc(_p, _n) NULL
#endif

static void xo_encoder_setup (void);




typedef struct xo_string_node_s {
TAILQ_ENTRY(xo_string_node_s) xs_link;
char xs_data[0];
} xo_string_node_t;

typedef TAILQ_HEAD(xo_string_list_s, xo_string_node_s) xo_string_list_t;

static inline void
xo_string_list_init (xo_string_list_t *listp)
{
if (listp->tqh_last == NULL)
TAILQ_INIT(listp);
}

static inline xo_string_node_t *
xo_string_add (xo_string_list_t *listp, const char *str)
{
if (listp == NULL || str == NULL)
return NULL;

xo_string_list_init(listp);
size_t len = strlen(str);
xo_string_node_t *xsp;

xsp = xo_realloc(NULL, sizeof(*xsp) + len + 1);
if (xsp) {
memcpy(xsp->xs_data, str, len);
xsp->xs_data[len] = '\0';
TAILQ_INSERT_TAIL(listp, xsp, xs_link);
}

return xsp;
}

#define XO_STRING_LIST_FOREACH(_xsp, _listp) xo_string_list_init(_listp); TAILQ_FOREACH(_xsp, _listp, xs_link)



static inline void
xo_string_list_clean (xo_string_list_t *listp)
{
xo_string_node_t *xsp;

xo_string_list_init(listp);

for (;;) {
xsp = TAILQ_FIRST(listp);
if (xsp == NULL)
break;
TAILQ_REMOVE(listp, xsp, xs_link);
xo_free(xsp);
}
}

static xo_string_list_t xo_encoder_path;

void
xo_encoder_path_add (const char *path)
{
xo_encoder_setup();

if (path)
xo_string_add(&xo_encoder_path, path);
}



typedef struct xo_encoder_node_s {
TAILQ_ENTRY(xo_encoder_node_s) xe_link;
char *xe_name;
xo_encoder_func_t xe_handler;
void *xe_dlhandle;
} xo_encoder_node_t;

typedef TAILQ_HEAD(xo_encoder_list_s, xo_encoder_node_s) xo_encoder_list_t;

#define XO_ENCODER_LIST_FOREACH(_xep, _listp) xo_encoder_list_init(_listp); TAILQ_FOREACH(_xep, _listp, xe_link)



static xo_encoder_list_t xo_encoders;

static void
xo_encoder_list_init (xo_encoder_list_t *listp)
{
if (listp->tqh_last == NULL)
TAILQ_INIT(listp);
}

static xo_encoder_node_t *
xo_encoder_list_add (const char *name)
{
if (name == NULL)
return NULL;

xo_encoder_node_t *xep = xo_realloc(NULL, sizeof(*xep));
if (xep) {
ssize_t len = strlen(name) + 1;
xep->xe_name = xo_realloc(NULL, len);
if (xep->xe_name == NULL) {
xo_free(xep);
return NULL;
}

memcpy(xep->xe_name, name, len);

TAILQ_INSERT_TAIL(&xo_encoders, xep, xe_link);
}

return xep;
}

void
xo_encoders_clean (void)
{
xo_encoder_node_t *xep;

xo_encoder_setup();

for (;;) {
xep = TAILQ_FIRST(&xo_encoders);
if (xep == NULL)
break;

TAILQ_REMOVE(&xo_encoders, xep, xe_link);

if (xep->xe_dlhandle)
dlclose(xep->xe_dlhandle);

xo_free(xep);
}

xo_string_list_clean(&xo_encoder_path);
}

static void
xo_encoder_setup (void)
{
static int initted;
if (!initted) {
initted = 1;

xo_string_list_init(&xo_encoder_path);
xo_encoder_list_init(&xo_encoders);

xo_encoder_path_add(XO_ENCODERDIR);
}
}

static xo_encoder_node_t *
xo_encoder_find (const char *name)
{
xo_encoder_node_t *xep;

xo_encoder_list_init(&xo_encoders);

XO_ENCODER_LIST_FOREACH(xep, &xo_encoders) {
if (xo_streq(xep->xe_name, name))
return xep;
}

return NULL;
}

static xo_encoder_node_t *
xo_encoder_discover (const char *name)
{
void *dlp = NULL;
char buf[MAXPATHLEN];
xo_string_node_t *xsp;
xo_encoder_node_t *xep = NULL;

XO_STRING_LIST_FOREACH(xsp, &xo_encoder_path) {
static const char fmt[] = "%s/%s.enc";
char *dir = xsp->xs_data;
size_t len = snprintf(buf, sizeof(buf), fmt, dir, name);

if (len > sizeof(buf))
continue;

dlp = dlopen((const char *) buf, RTLD_NOW);
if (dlp)
break;
}

if (dlp) {




xo_encoder_init_func_t func;

func = (xo_encoder_init_func_t) dlfunc(dlp, XO_ENCODER_INIT_NAME);
if (func) {
xo_encoder_init_args_t xei;

bzero(&xei, sizeof(xei));

xei.xei_version = XO_ENCODER_VERSION;
ssize_t rc = func(&xei);
if (rc == 0 && xei.xei_handler) {
xep = xo_encoder_list_add(name);
if (xep) {
xep->xe_handler = xei.xei_handler;
xep->xe_dlhandle = dlp;
}
}
}

if (xep == NULL)
dlclose(dlp);
}

return xep;
}

void
xo_encoder_register (const char *name, xo_encoder_func_t func)
{
xo_encoder_setup();

xo_encoder_node_t *xep = xo_encoder_find(name);

if (xep)
return;

xep = xo_encoder_list_add(name);
if (xep)
xep->xe_handler = func;
}

void
xo_encoder_unregister (const char *name)
{
xo_encoder_setup();

xo_encoder_node_t *xep = xo_encoder_find(name);
if (xep) {
TAILQ_REMOVE(&xo_encoders, xep, xe_link);
xo_free(xep);
}
}

int
xo_encoder_init (xo_handle_t *xop, const char *name)
{
xo_encoder_setup();

char opts_char = '\0';
const char *col_opts = strchr(name, ':');
const char *plus_opts = strchr(name, '+');





const char *opts = (col_opts == NULL) ? plus_opts
: (plus_opts == NULL) ? col_opts
: (plus_opts < col_opts) ? plus_opts : col_opts;

if (opts) {
opts_char = *opts;


size_t len = strlen(name);
char *copy = alloca(len + 1);
memcpy(copy, name, len);
copy[len] = '\0';

char *opts_copy = copy + (opts - name);
*opts_copy++ = '\0';

opts = opts_copy;
name = copy;
}


if (strchr(name, '/') != NULL || strchr(name, ':') != NULL) {
xo_failure(xop, "invalid encoder name: %s", name);
return -1;
}






xo_encoder_node_t *xep = xo_encoder_find(name);
if (xep == NULL) {
xep = xo_encoder_discover(name);
if (xep == NULL) {
xo_failure(xop, "encoder not founde: %s", name);
return -1;
}
}

xo_set_encoder(xop, xep->xe_handler);

int rc = xo_encoder_handle(xop, XO_OP_CREATE, name, NULL, 0);
if (rc == 0 && opts != NULL) {
xo_encoder_op_t op;


op = (opts_char == '+') ? XO_OP_OPTIONS_PLUS : XO_OP_OPTIONS;
rc = xo_encoder_handle(xop, op, name, opts, 0);
}

return rc;
}






xo_handle_t *
xo_encoder_create (const char *name, xo_xof_flags_t flags)
{
xo_handle_t *xop;

xop = xo_create(XO_STYLE_ENCODER, flags);
if (xop) {
if (xo_encoder_init(xop, name)) {
xo_destroy(xop);
xop = NULL;
}
}

return xop;
}

int
xo_encoder_handle (xo_handle_t *xop, xo_encoder_op_t op,
const char *name, const char *value, xo_xff_flags_t flags)
{
void *private = xo_get_private(xop);
xo_encoder_func_t func = xo_get_encoder(xop);

if (func == NULL)
return -1;

return func(xop, op, name, value, private, flags);
}

const char *
xo_encoder_op_name (xo_encoder_op_t op)
{
static const char *names[] = {
"unknown",
"create",
"open_container",
"close_container",
"open_list",
"close_list",
"open_leaf_list",
"close_leaf_list",
"open_instance",
"close_instance",
"string",
"content",
"finish",
"flush",
"destroy",
"attr",
"version",
"options",
};

if (op > sizeof(names) / sizeof(names[0]))
return "unknown";

return names[op];
}
