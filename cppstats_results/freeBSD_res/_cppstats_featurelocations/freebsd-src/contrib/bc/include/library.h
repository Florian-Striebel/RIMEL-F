


































#if !defined(LIBBC_PRIVATE_H)
#define LIBBC_PRIVATE_H

#include <bcl.h>

#include <num.h>






#define BC_FUNC_HEADER_LOCK(l) do { BC_SIG_LOCK; BC_SETJMP_LOCKED(l); vm.err = BCL_ERROR_NONE; vm.running = 1; } while (0)












#define BC_FUNC_FOOTER_UNLOCK(e) do { BC_SIG_ASSERT_LOCKED; e = vm.err; vm.running = 0; BC_UNSETJMP; BC_LONGJMP_STOP; vm.sig_lock = 0; } while (0)













#define BC_FUNC_HEADER(l) do { BC_SETJMP(l); vm.err = BCL_ERROR_NONE; vm.running = 1; } while (0)











#define BC_FUNC_HEADER_INIT(l) do { BC_SETJMP_LOCKED(l); vm.err = BCL_ERROR_NONE; vm.running = 1; } while (0)










#define BC_FUNC_FOOTER_NO_ERR do { vm.running = 0; BC_UNSETJMP; BC_LONGJMP_STOP; vm.sig_lock = 0; } while (0)












#define BC_FUNC_FOOTER(e) do { e = vm.err; BC_FUNC_FOOTER_NO_ERR; } while (0)













#define BC_MAYBE_SETUP(c, e, n, idx) do { if (BC_ERR((e) != BCL_ERROR_NONE)) { if ((n).num != NULL) bc_num_free(&(n)); idx.i = 0 - (size_t) (e); } else idx = bcl_num_insert(c, &(n)); } while (0)













#define BC_CHECK_CTXT(c) do { c = bcl_context(); if (BC_ERR(c == NULL)) { BclNumber n_num; n_num.i = 0 - (size_t) BCL_ERROR_INVALID_CONTEXT; return n_num; } } while (0)














#define BC_CHECK_CTXT_ERR(c) do { c = bcl_context(); if (BC_ERR(c == NULL)) { return BCL_ERROR_INVALID_CONTEXT; } } while (0)











#define BC_CHECK_CTXT_ASSERT(c) do { c = bcl_context(); assert(c != NULL); } while (0)











#define BC_CHECK_NUM(c, n) do { if (BC_ERR((n).i >= (c)->nums.len)) { if ((n).i > 0 - (size_t) BCL_ERROR_NELEMS) return (n); else { BclNumber n_num; n_num.i = 0 - (size_t) BCL_ERROR_INVALID_NUM; return n_num; } } } while (0)

















#define BC_CHECK_NUM_ERR(c, n) do { if (BC_ERR((n).i >= (c)->nums.len)) { if ((n).i > 0 - (size_t) BCL_ERROR_NELEMS) return (BclError) (0 - (n).i); else return BCL_ERROR_INVALID_NUM; } } while (0)













#define BC_NUM(c, n) ((BcNum*) bc_vec_item(&(c)->nums, (n).i))





void bcl_num_destruct(void *num);


typedef struct BclCtxt {


size_t scale;


size_t ibase;


size_t obase;


BcVec nums;



BcVec free_nums;

} BclCtxt;

#endif
