#pragma ident "%Z%%M% %I% %E% SMI"
#if defined(__cplusplus)
extern "C" {
#endif
typedef struct stk stk_t;
stk_t *stack_new(void (*)(void *));
void stack_free(stk_t *);
void *stack_pop(stk_t *);
void *stack_peek(stk_t *);
void stack_push(stk_t *, void *);
int stack_level(stk_t *);
#if defined(__cplusplus)
}
#endif
