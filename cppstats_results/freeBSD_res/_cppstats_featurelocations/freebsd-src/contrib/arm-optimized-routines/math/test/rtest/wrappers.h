






typedef struct {


int nops;
int nresults;
mpfr_srcptr mpfr_ops[2];
mpfr_ptr mpfr_result;
mpc_srcptr mpc_ops[2];
mpc_ptr mpc_result;
const uint32 *ieee_ops[2];
uint32 *ieee_result;
int size_ops[2];
int size_result;
int need_regen;
} wrapperctx;

typedef void (*wrapperfunc)(wrapperctx *ctx);
#define MAXWRAPPERS 3






















void wrapper_init(wrapperctx *ctx);


void wrapper_op_real(wrapperctx *ctx, const mpfr_t r,
int size, const uint32 *ieee);


void wrapper_op_complex(wrapperctx *ctx, const mpc_t c,
int size, const uint32 *ieee);


void wrapper_result_real(wrapperctx *ctx, mpfr_t r,
int size, uint32 *ieee);



void wrapper_result_complex(wrapperctx *ctx, mpc_t c,
int size, uint32 *ieee);

int wrapper_run(wrapperctx *ctx, wrapperfunc wrappers[MAXWRAPPERS]);







mpfr_srcptr wrapper_get_mpfr(wrapperctx *ctx, int op);
const uint32 *wrapper_get_ieee(wrapperctx *ctx, int op);

mpc_srcptr wrapper_get_mpc(wrapperctx *ctx, int op);
mpfr_srcptr wrapper_get_mpfr_r(wrapperctx *ctx, int op);
mpfr_srcptr wrapper_get_mpfr_i(wrapperctx *ctx, int op);
const uint32 *wrapper_get_ieee_r(wrapperctx *ctx, int op);
const uint32 *wrapper_get_ieee_i(wrapperctx *ctx, int op);


int wrapper_get_nops(wrapperctx *ctx);
int wrapper_get_size(wrapperctx *ctx, int op);
int wrapper_is_complex(wrapperctx *ctx, int op);


void wrapper_set_sign(wrapperctx *ctx, uint32 sign);
void wrapper_set_sign_r(wrapperctx *ctx, uint32 sign);
void wrapper_set_sign_i(wrapperctx *ctx, uint32 sign);


void wrapper_set_nan(wrapperctx *ctx);
void wrapper_set_nan_r(wrapperctx *ctx);
void wrapper_set_nan_i(wrapperctx *ctx);



void wrapper_set_int(wrapperctx *ctx, int val);
void wrapper_set_int_r(wrapperctx *ctx, int val);
void wrapper_set_int_i(wrapperctx *ctx, int val);


void wrapper_set_mpfr(wrapperctx *ctx, const mpfr_t val);
void wrapper_set_mpfr_r(wrapperctx *ctx, const mpfr_t val);
void wrapper_set_mpfr_i(wrapperctx *ctx, const mpfr_t val);





void universal_wrapper(wrapperctx *ctx);
