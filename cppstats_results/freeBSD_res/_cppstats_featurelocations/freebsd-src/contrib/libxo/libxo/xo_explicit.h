










#if !defined(XO_EXPLICIT_H)
#define XO_EXPLICIT_H

























typedef unsigned xo_state_t;
#define XSS_INIT 0
#define XSS_OPEN_CONTAINER 1
#define XSS_CLOSE_CONTAINER 2
#define XSS_OPEN_LIST 3
#define XSS_CLOSE_LIST 4
#define XSS_OPEN_INSTANCE 5
#define XSS_CLOSE_INSTANCE 6
#define XSS_OPEN_LEAF_LIST 7
#define XSS_CLOSE_LEAF_LIST 8
#define XSS_DISCARDING 9
#define XSS_MARKER 10
#define XSS_EMIT 11
#define XSS_EMIT_LEAF_LIST 12
#define XSS_FINISH 13

#define XSS_MAX 13

void
xo_explicit_transition (xo_handle_t *xop, xo_state_t new_state,
const char *tag, xo_xof_flags_t flags);

#endif
