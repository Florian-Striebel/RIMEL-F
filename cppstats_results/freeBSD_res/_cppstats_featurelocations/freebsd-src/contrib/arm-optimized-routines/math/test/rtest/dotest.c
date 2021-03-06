






#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <limits.h>

#include "semi.h"
#include "intern.h"
#include "random.h"

#define MPFR_PREC 96

extern int lib_fo, lib_no_arith, ntests;




static void cases_biased(uint32 *, uint32, uint32);
static void cases_biased_positive(uint32 *, uint32, uint32);
static void cases_biased_float(uint32 *, uint32, uint32);
static void cases_uniform(uint32 *, uint32, uint32);
static void cases_uniform_positive(uint32 *, uint32, uint32);
static void cases_uniform_float(uint32 *, uint32, uint32);
static void cases_uniform_float_positive(uint32 *, uint32, uint32);
static void log_cases(uint32 *, uint32, uint32);
static void log_cases_float(uint32 *, uint32, uint32);
static void log1p_cases(uint32 *, uint32, uint32);
static void log1p_cases_float(uint32 *, uint32, uint32);
static void minmax_cases(uint32 *, uint32, uint32);
static void minmax_cases_float(uint32 *, uint32, uint32);
static void atan2_cases(uint32 *, uint32, uint32);
static void atan2_cases_float(uint32 *, uint32, uint32);
static void pow_cases(uint32 *, uint32, uint32);
static void pow_cases_float(uint32 *, uint32, uint32);
static void rred_cases(uint32 *, uint32, uint32);
static void rred_cases_float(uint32 *, uint32, uint32);
static void cases_semi1(uint32 *, uint32, uint32);
static void cases_semi1_float(uint32 *, uint32, uint32);
static void cases_semi2(uint32 *, uint32, uint32);
static void cases_semi2_float(uint32 *, uint32, uint32);
static void cases_ldexp(uint32 *, uint32, uint32);
static void cases_ldexp_float(uint32 *, uint32, uint32);

static void complex_cases_uniform(uint32 *, uint32, uint32);
static void complex_cases_uniform_float(uint32 *, uint32, uint32);
static void complex_cases_biased(uint32 *, uint32, uint32);
static void complex_cases_biased_float(uint32 *, uint32, uint32);
static void complex_log_cases(uint32 *, uint32, uint32);
static void complex_log_cases_float(uint32 *, uint32, uint32);
static void complex_pow_cases(uint32 *, uint32, uint32);
static void complex_pow_cases_float(uint32 *, uint32, uint32);
static void complex_arithmetic_cases(uint32 *, uint32, uint32);
static void complex_arithmetic_cases_float(uint32 *, uint32, uint32);

static uint32 doubletop(int x, int scale);
static uint32 floatval(int x, int scale);





static void set_mpfr_d(mpfr_t x, uint32 h, uint32 l)
{
uint64_t hl = ((uint64_t)h << 32) | l;
uint32 exp = (hl >> 52) & 0x7ff;
int64_t mantissa = hl & (((uint64_t)1 << 52) - 1);
int sign = (hl >> 63) ? -1 : +1;
if (exp == 0x7ff) {
if (mantissa == 0)
mpfr_set_inf(x, sign);
else
mpfr_set_nan(x);
} else if (exp == 0 && mantissa == 0) {
mpfr_set_ui(x, 0, GMP_RNDN);
mpfr_setsign(x, x, sign < 0, GMP_RNDN);
} else {
if (exp != 0)
mantissa |= ((uint64_t)1 << 52);
else
exp++;
mpfr_set_sj_2exp(x, mantissa * sign, (int)exp - 0x3ff - 52, GMP_RNDN);
}
}
static void set_mpfr_f(mpfr_t x, uint32 f)
{
uint32 exp = (f >> 23) & 0xff;
int32 mantissa = f & ((1 << 23) - 1);
int sign = (f >> 31) ? -1 : +1;
if (exp == 0xff) {
if (mantissa == 0)
mpfr_set_inf(x, sign);
else
mpfr_set_nan(x);
} else if (exp == 0 && mantissa == 0) {
mpfr_set_ui(x, 0, GMP_RNDN);
mpfr_setsign(x, x, sign < 0, GMP_RNDN);
} else {
if (exp != 0)
mantissa |= (1 << 23);
else
exp++;
mpfr_set_sj_2exp(x, mantissa * sign, (int)exp - 0x7f - 23, GMP_RNDN);
}
}
static void set_mpc_d(mpc_t z, uint32 rh, uint32 rl, uint32 ih, uint32 il)
{
mpfr_t x, y;
mpfr_init2(x, MPFR_PREC);
mpfr_init2(y, MPFR_PREC);
set_mpfr_d(x, rh, rl);
set_mpfr_d(y, ih, il);
mpc_set_fr_fr(z, x, y, MPC_RNDNN);
mpfr_clear(x);
mpfr_clear(y);
}
static void set_mpc_f(mpc_t z, uint32 r, uint32 i)
{
mpfr_t x, y;
mpfr_init2(x, MPFR_PREC);
mpfr_init2(y, MPFR_PREC);
set_mpfr_f(x, r);
set_mpfr_f(y, i);
mpc_set_fr_fr(z, x, y, MPC_RNDNN);
mpfr_clear(x);
mpfr_clear(y);
}
static void get_mpfr_d(const mpfr_t x, uint32 *h, uint32 *l, uint32 *extra)
{
uint32_t sign, expfield, mantfield;
mpfr_t significand;
int exp;

if (mpfr_nan_p(x)) {
*h = 0x7ff80000;
*l = 0;
*extra = 0;
return;
}

sign = mpfr_signbit(x) ? 0x80000000U : 0;

if (mpfr_inf_p(x)) {
*h = 0x7ff00000 | sign;
*l = 0;
*extra = 0;
return;
}

if (mpfr_zero_p(x)) {
*h = 0x00000000 | sign;
*l = 0;
*extra = 0;
return;
}

mpfr_init2(significand, MPFR_PREC);
mpfr_set(significand, x, GMP_RNDN);
exp = mpfr_get_exp(significand);
mpfr_set_exp(significand, 0);



if (exp > 0x400) {

*h = 0x7ff00000 | sign;
*l = 0;
*extra = 0;
mpfr_clear(significand);
return;
}

if (exp <= -0x3fe || mpfr_zero_p(x))
exp = -0x3fd;
expfield = exp + 0x3fd;

mpfr_div_2si(significand, x, exp - 21, GMP_RNDN);
mpfr_abs(significand, significand, GMP_RNDN);
mantfield = mpfr_get_ui(significand, GMP_RNDZ);
*h = sign + ((uint64_t)expfield << 20) + mantfield;
mpfr_sub_ui(significand, significand, mantfield, GMP_RNDN);
mpfr_mul_2ui(significand, significand, 32, GMP_RNDN);
mantfield = mpfr_get_ui(significand, GMP_RNDZ);
*l = mantfield;
mpfr_sub_ui(significand, significand, mantfield, GMP_RNDN);
mpfr_mul_2ui(significand, significand, 32, GMP_RNDN);
mantfield = mpfr_get_ui(significand, GMP_RNDZ);
*extra = mantfield;

mpfr_clear(significand);
}
static void get_mpfr_f(const mpfr_t x, uint32 *f, uint32 *extra)
{
uint32_t sign, expfield, mantfield;
mpfr_t significand;
int exp;

if (mpfr_nan_p(x)) {
*f = 0x7fc00000;
*extra = 0;
return;
}

sign = mpfr_signbit(x) ? 0x80000000U : 0;

if (mpfr_inf_p(x)) {
*f = 0x7f800000 | sign;
*extra = 0;
return;
}

if (mpfr_zero_p(x)) {
*f = 0x00000000 | sign;
*extra = 0;
return;
}

mpfr_init2(significand, MPFR_PREC);
mpfr_set(significand, x, GMP_RNDN);
exp = mpfr_get_exp(significand);
mpfr_set_exp(significand, 0);



if (exp > 0x80) {

*f = 0x7f800000 | sign;
*extra = 0;
mpfr_clear(significand);
return;
}

if (exp <= -0x7e || mpfr_zero_p(x))
exp = -0x7d;
expfield = exp + 0x7d;

mpfr_div_2si(significand, x, exp - 24, GMP_RNDN);
mpfr_abs(significand, significand, GMP_RNDN);
mantfield = mpfr_get_ui(significand, GMP_RNDZ);
*f = sign + ((uint64_t)expfield << 23) + mantfield;
mpfr_sub_ui(significand, significand, mantfield, GMP_RNDN);
mpfr_mul_2ui(significand, significand, 32, GMP_RNDN);
mantfield = mpfr_get_ui(significand, GMP_RNDZ);
*extra = mantfield;

mpfr_clear(significand);
}
static void get_mpc_d(const mpc_t z,
uint32 *rh, uint32 *rl, uint32 *rextra,
uint32 *ih, uint32 *il, uint32 *iextra)
{
mpfr_t x, y;
mpfr_init2(x, MPFR_PREC);
mpfr_init2(y, MPFR_PREC);
mpc_real(x, z, GMP_RNDN);
mpc_imag(y, z, GMP_RNDN);
get_mpfr_d(x, rh, rl, rextra);
get_mpfr_d(y, ih, il, iextra);
mpfr_clear(x);
mpfr_clear(y);
}
static void get_mpc_f(const mpc_t z,
uint32 *r, uint32 *rextra,
uint32 *i, uint32 *iextra)
{
mpfr_t x, y;
mpfr_init2(x, MPFR_PREC);
mpfr_init2(y, MPFR_PREC);
mpc_real(x, z, GMP_RNDN);
mpc_imag(y, z, GMP_RNDN);
get_mpfr_f(x, r, rextra);
get_mpfr_f(y, i, iextra);
mpfr_clear(x);
mpfr_clear(y);
}





int test_rred(mpfr_t ret, const mpfr_t x, int *quadrant)
{
mpfr_t halfpi;
long quo;
int status;




















mpfr_init2(halfpi, MPFR_PREC + 1024 + 64);
mpfr_const_pi(halfpi, GMP_RNDN);
mpfr_div_ui(halfpi, halfpi, 2, GMP_RNDN);

status = mpfr_remquo(ret, &quo, x, halfpi, GMP_RNDN);
*quadrant = quo & 3;

mpfr_clear(halfpi);

return status;
}
int test_lgamma(mpfr_t ret, const mpfr_t x, mpfr_rnd_t rnd)
{










int sign;
return mpfr_lgamma(ret, &sign, x, rnd);
}
int test_cpow(mpc_t ret, const mpc_t x, const mpc_t y, mpc_rnd_t rnd)
{

















mpc_t xbig, ybig, retbig;
int status;

mpc_init2(xbig, 1034 + 53 + 60 + MPFR_PREC);
mpc_init2(ybig, 1034 + 53 + 60 + MPFR_PREC);
mpc_init2(retbig, 1034 + 53 + 60 + MPFR_PREC);

mpc_set(xbig, x, MPC_RNDNN);
mpc_set(ybig, y, MPC_RNDNN);
status = mpc_pow(retbig, xbig, ybig, rnd);
mpc_set(ret, retbig, rnd);

mpc_clear(xbig);
mpc_clear(ybig);
mpc_clear(retbig);

return status;
}





#define is_shard(in) ( (((in)[0] & 0x7F800000) == 0x7F800000 || (((in)[0] & 0x7F800000) == 0 && ((in)[0]&0x7FFFFFFF) != 0)))



#define is_dhard(in) ( (((in)[0] & 0x7FF00000) == 0x7FF00000 || (((in)[0] & 0x7FF00000) == 0 && (((in)[0] & 0xFFFFF) | (in)[1]) != 0)))






int is_dinteger(uint32 *in)
{
uint32 out[3];
if ((0x7FF00000 & ~in[0]) == 0)
return 0;
test_ceil(in, out);
return in[0] == out[0] && in[1] == out[1];
}
int is_sinteger(uint32 *in)
{
uint32 out[3];
if ((0x7F800000 & ~in[0]) == 0)
return 0;
test_ceilf(in, out);
return in[0] == out[0];
}




int is_dsnan(const uint32 *in)
{
if ((in[0] & 0x7FF00000) != 0x7FF00000)
return 0;
if ((in[0] << 12) == 0 && in[1] == 0)
return 0;
if (in[0] & 0x00080000)
return 0;
return 1;
}
int is_ssnan(const uint32 *in)
{
if ((in[0] & 0x7F800000) != 0x7F800000)
return 0;
if ((in[0] << 9) == 0)
return 0;
if (in[0] & 0x00400000)
return 0;
return 1;
}
int is_snan(const uint32 *in, int size)
{
return size == 2 ? is_dsnan(in) : is_ssnan(in);
}





void universal_wrapper(wrapperctx *ctx)
{



int op;
for (op = 0; op < wrapper_get_nops(ctx); op++) {
int size = wrapper_get_size(ctx, op);

if (!wrapper_is_complex(ctx, op) &&
is_snan(wrapper_get_ieee(ctx, op), size)) {
wrapper_set_nan(ctx);
}
}
}

Testable functions[] = {








{"sin", (funcptr)mpfr_sin, args1, {NULL},
cases_uniform, 0x3e400000, 0x40300000},
{"sinf", (funcptr)mpfr_sin, args1f, {NULL},
cases_uniform_float, 0x39800000, 0x41800000},
{"cos", (funcptr)mpfr_cos, args1, {NULL},
cases_uniform, 0x3e400000, 0x40300000},
{"cosf", (funcptr)mpfr_cos, args1f, {NULL},
cases_uniform_float, 0x39800000, 0x41800000},
{"tan", (funcptr)mpfr_tan, args1, {NULL},
cases_uniform, 0x3e400000, 0x40300000},
{"tanf", (funcptr)mpfr_tan, args1f, {NULL},
cases_uniform_float, 0x39800000, 0x41800000},
{"sincosf_sinf", (funcptr)mpfr_sin, args1f, {NULL},
cases_uniform_float, 0x39800000, 0x41800000},
{"sincosf_cosf", (funcptr)mpfr_cos, args1f, {NULL},
cases_uniform_float, 0x39800000, 0x41800000},




{"asin", (funcptr)mpfr_asin, args1, {NULL},
cases_uniform, 0x3e400000, 0x3fefffff},
{"asinf", (funcptr)mpfr_asin, args1f, {NULL},
cases_uniform_float, 0x39800000, 0x3f7fffff},
{"acos", (funcptr)mpfr_acos, args1, {NULL},
cases_uniform, 0x3c900000, 0x3fefffff},
{"acosf", (funcptr)mpfr_acos, args1f, {NULL},
cases_uniform_float, 0x33800000, 0x3f7fffff},





{"atan", (funcptr)mpfr_atan, args1, {NULL},
cases_uniform, 0x3e400000, 0x43400000},
{"atanf", (funcptr)mpfr_atan, args1f, {NULL},
cases_uniform_float, 0x39800000, 0x4b800000},




{"atan2", (funcptr)mpfr_atan2, args2, {NULL},
atan2_cases, 0},
{"atan2f", (funcptr)mpfr_atan2, args2f, {NULL},
atan2_cases_float, 0},




{"exp", (funcptr)mpfr_exp, args1, {NULL},
cases_uniform, 0x3c900000, 0x40878000},
{"expf", (funcptr)mpfr_exp, args1f, {NULL},
cases_uniform_float, 0x33800000, 0x42dc0000},
{"sinh", (funcptr)mpfr_sinh, args1, {NULL},
cases_uniform, 0x3c900000, 0x40878000},
{"sinhf", (funcptr)mpfr_sinh, args1f, {NULL},
cases_uniform_float, 0x33800000, 0x42dc0000},
{"cosh", (funcptr)mpfr_cosh, args1, {NULL},
cases_uniform, 0x3e400000, 0x40878000},
{"coshf", (funcptr)mpfr_cosh, args1f, {NULL},
cases_uniform_float, 0x39800000, 0x42dc0000},



{"tanh", (funcptr)mpfr_tanh, args1, {NULL},
cases_uniform, 0x3e400000, 0x40340000},
{"tanhf", (funcptr)mpfr_tanh, args1f, {NULL},
cases_uniform, 0x39800000, 0x41100000},





{"log", (funcptr)mpfr_log, args1, {NULL}, log_cases, 0},
{"logf", (funcptr)mpfr_log, args1f, {NULL}, log_cases_float, 0},
{"log10", (funcptr)mpfr_log10, args1, {NULL}, log_cases, 0},
{"log10f", (funcptr)mpfr_log10, args1f, {NULL}, log_cases_float, 0},



{"pow", (funcptr)mpfr_pow, args2, {NULL}, pow_cases, 0},
{"powf", (funcptr)mpfr_pow, args2f, {NULL}, pow_cases_float, 0},





{"rred", (funcptr)test_rred, rred, {NULL}, rred_cases, 0},
{"rredf", (funcptr)test_rred, rredf, {NULL}, rred_cases_float, 0},



{"sqrt", (funcptr)mpfr_sqrt, args1, {NULL}, log_cases, 0},
{"sqrtf", (funcptr)mpfr_sqrt, args1f, {NULL}, log_cases_float, 0},
{"cbrt", (funcptr)mpfr_cbrt, args1, {NULL}, log_cases, 0},
{"cbrtf", (funcptr)mpfr_cbrt, args1f, {NULL}, log_cases_float, 0},
{"hypot", (funcptr)mpfr_hypot, args2, {NULL}, atan2_cases, 0},
{"hypotf", (funcptr)mpfr_hypot, args2f, {NULL}, atan2_cases_float, 0},



{"ceil", (funcptr)test_ceil, semi1, {NULL}, cases_semi1},
{"ceilf", (funcptr)test_ceilf, semi1f, {NULL}, cases_semi1_float},
{"floor", (funcptr)test_floor, semi1, {NULL}, cases_semi1},
{"floorf", (funcptr)test_floorf, semi1f, {NULL}, cases_semi1_float},
{"fmod", (funcptr)test_fmod, semi2, {NULL}, cases_semi2},
{"fmodf", (funcptr)test_fmodf, semi2f, {NULL}, cases_semi2_float},
{"ldexp", (funcptr)test_ldexp, t_ldexp, {NULL}, cases_ldexp},
{"ldexpf", (funcptr)test_ldexpf, t_ldexpf, {NULL}, cases_ldexp_float},
{"frexp", (funcptr)test_frexp, t_frexp, {NULL}, cases_semi1},
{"frexpf", (funcptr)test_frexpf, t_frexpf, {NULL}, cases_semi1_float},
{"modf", (funcptr)test_modf, t_modf, {NULL}, cases_semi1},
{"modff", (funcptr)test_modff, t_modff, {NULL}, cases_semi1_float},




{"copysign", (funcptr)test_copysign, semi2, {NULL}, cases_semi2},
{"copysignf", (funcptr)test_copysignf, semi2f, {NULL}, cases_semi2_float},
{"isfinite", (funcptr)test_isfinite, classify, {NULL}, cases_uniform, 0, 0x7fffffff},
{"isfinitef", (funcptr)test_isfinitef, classifyf, {NULL}, cases_uniform_float, 0, 0x7fffffff},
{"isinf", (funcptr)test_isinf, classify, {NULL}, cases_uniform, 0, 0x7fffffff},
{"isinff", (funcptr)test_isinff, classifyf, {NULL}, cases_uniform_float, 0, 0x7fffffff},
{"isnan", (funcptr)test_isnan, classify, {NULL}, cases_uniform, 0, 0x7fffffff},
{"isnanf", (funcptr)test_isnanf, classifyf, {NULL}, cases_uniform_float, 0, 0x7fffffff},
{"isnormal", (funcptr)test_isnormal, classify, {NULL}, cases_uniform, 0, 0x7fffffff},
{"isnormalf", (funcptr)test_isnormalf, classifyf, {NULL}, cases_uniform_float, 0, 0x7fffffff},
{"signbit", (funcptr)test_signbit, classify, {NULL}, cases_uniform, 0, 0x7fffffff},
{"signbitf", (funcptr)test_signbitf, classifyf, {NULL}, cases_uniform_float, 0, 0x7fffffff},
{"fpclassify", (funcptr)test_fpclassify, classify, {NULL}, cases_uniform, 0, 0x7fffffff},
{"fpclassifyf", (funcptr)test_fpclassifyf, classifyf, {NULL}, cases_uniform_float, 0, 0x7fffffff},



{"isgreater", (funcptr)test_isgreater, compare, {NULL}, cases_uniform, 0, 0x7fffffff},
{"isgreaterequal", (funcptr)test_isgreaterequal, compare, {NULL}, cases_uniform, 0, 0x7fffffff},
{"isless", (funcptr)test_isless, compare, {NULL}, cases_uniform, 0, 0x7fffffff},
{"islessequal", (funcptr)test_islessequal, compare, {NULL}, cases_uniform, 0, 0x7fffffff},
{"islessgreater", (funcptr)test_islessgreater, compare, {NULL}, cases_uniform, 0, 0x7fffffff},
{"isunordered", (funcptr)test_isunordered, compare, {NULL}, cases_uniform, 0, 0x7fffffff},

{"isgreaterf", (funcptr)test_isgreaterf, comparef, {NULL}, cases_uniform_float, 0, 0x7fffffff},
{"isgreaterequalf", (funcptr)test_isgreaterequalf, comparef, {NULL}, cases_uniform_float, 0, 0x7fffffff},
{"islessf", (funcptr)test_islessf, comparef, {NULL}, cases_uniform_float, 0, 0x7fffffff},
{"islessequalf", (funcptr)test_islessequalf, comparef, {NULL}, cases_uniform_float, 0, 0x7fffffff},
{"islessgreaterf", (funcptr)test_islessgreaterf, comparef, {NULL}, cases_uniform_float, 0, 0x7fffffff},
{"isunorderedf", (funcptr)test_isunorderedf, comparef, {NULL}, cases_uniform_float, 0, 0x7fffffff},




{"atanh", (funcptr)mpfr_atanh, args1, {NULL}, cases_uniform, 0x3e400000, 0x3fefffff},
{"asinh", (funcptr)mpfr_asinh, args1, {NULL}, cases_uniform, 0x3e400000, 0x3fefffff},
{"acosh", (funcptr)mpfr_acosh, args1, {NULL}, cases_uniform_positive, 0x3ff00000, 0x7fefffff},

{"atanhf", (funcptr)mpfr_atanh, args1f, {NULL}, cases_uniform_float, 0x32000000, 0x3f7fffff},
{"asinhf", (funcptr)mpfr_asinh, args1f, {NULL}, cases_uniform_float, 0x32000000, 0x3f7fffff},
{"acoshf", (funcptr)mpfr_acosh, args1f, {NULL}, cases_uniform_float_positive, 0x3f800000, 0x7f800000},






{"csin", (funcptr)mpc_sin, args1c, {NULL}, complex_cases_uniform, 0x3f000000, 0x40300000},
{"csinf", (funcptr)mpc_sin, args1fc, {NULL}, complex_cases_uniform_float, 0x38000000, 0x41800000},
{"ccos", (funcptr)mpc_cos, args1c, {NULL}, complex_cases_uniform, 0x3f000000, 0x40300000},
{"ccosf", (funcptr)mpc_cos, args1fc, {NULL}, complex_cases_uniform_float, 0x38000000, 0x41800000},
{"ctan", (funcptr)mpc_tan, args1c, {NULL}, complex_cases_uniform, 0x3f000000, 0x40300000},
{"ctanf", (funcptr)mpc_tan, args1fc, {NULL}, complex_cases_uniform_float, 0x38000000, 0x41800000},

{"casin", (funcptr)mpc_asin, args1c, {NULL}, complex_cases_uniform, 0x3f000000, 0x40300000},
{"casinf", (funcptr)mpc_asin, args1fc, {NULL}, complex_cases_uniform_float, 0x38000000, 0x41800000},
{"cacos", (funcptr)mpc_acos, args1c, {NULL}, complex_cases_uniform, 0x3f000000, 0x40300000},
{"cacosf", (funcptr)mpc_acos, args1fc, {NULL}, complex_cases_uniform_float, 0x38000000, 0x41800000},
{"catan", (funcptr)mpc_atan, args1c, {NULL}, complex_cases_uniform, 0x3f000000, 0x40300000},
{"catanf", (funcptr)mpc_atan, args1fc, {NULL}, complex_cases_uniform_float, 0x38000000, 0x41800000},

{"csinh", (funcptr)mpc_sinh, args1c, {NULL}, complex_cases_uniform, 0x3f000000, 0x40300000},
{"csinhf", (funcptr)mpc_sinh, args1fc, {NULL}, complex_cases_uniform_float, 0x38000000, 0x41800000},
{"ccosh", (funcptr)mpc_cosh, args1c, {NULL}, complex_cases_uniform, 0x3f000000, 0x40300000},
{"ccoshf", (funcptr)mpc_cosh, args1fc, {NULL}, complex_cases_uniform_float, 0x38000000, 0x41800000},
{"ctanh", (funcptr)mpc_tanh, args1c, {NULL}, complex_cases_uniform, 0x3f000000, 0x40300000},
{"ctanhf", (funcptr)mpc_tanh, args1fc, {NULL}, complex_cases_uniform_float, 0x38000000, 0x41800000},

{"casinh", (funcptr)mpc_asinh, args1c, {NULL}, complex_cases_uniform, 0x3f000000, 0x40300000},
{"casinhf", (funcptr)mpc_asinh, args1fc, {NULL}, complex_cases_uniform_float, 0x38000000, 0x41800000},
{"cacosh", (funcptr)mpc_acosh, args1c, {NULL}, complex_cases_uniform, 0x3f000000, 0x40300000},
{"cacoshf", (funcptr)mpc_acosh, args1fc, {NULL}, complex_cases_uniform_float, 0x38000000, 0x41800000},
{"catanh", (funcptr)mpc_atanh, args1c, {NULL}, complex_cases_uniform, 0x3f000000, 0x40300000},
{"catanhf", (funcptr)mpc_atanh, args1fc, {NULL}, complex_cases_uniform_float, 0x38000000, 0x41800000},

{"cexp", (funcptr)mpc_exp, args1c, {NULL}, complex_cases_uniform, 0x3c900000, 0x40862000},
{"cpow", (funcptr)test_cpow, args2c, {NULL}, complex_pow_cases, 0x3fc00000, 0x40000000},
{"clog", (funcptr)mpc_log, args1c, {NULL}, complex_log_cases, 0, 0},
{"csqrt", (funcptr)mpc_sqrt, args1c, {NULL}, complex_log_cases, 0, 0},

{"cexpf", (funcptr)mpc_exp, args1fc, {NULL}, complex_cases_uniform_float, 0x24800000, 0x42b00000},
{"cpowf", (funcptr)test_cpow, args2fc, {NULL}, complex_pow_cases_float, 0x3e000000, 0x41000000},
{"clogf", (funcptr)mpc_log, args1fc, {NULL}, complex_log_cases_float, 0, 0},
{"csqrtf", (funcptr)mpc_sqrt, args1fc, {NULL}, complex_log_cases_float, 0, 0},

{"cdiv", (funcptr)mpc_div, args2c, {NULL}, complex_arithmetic_cases, 0, 0},
{"cmul", (funcptr)mpc_mul, args2c, {NULL}, complex_arithmetic_cases, 0, 0},
{"cadd", (funcptr)mpc_add, args2c, {NULL}, complex_arithmetic_cases, 0, 0},
{"csub", (funcptr)mpc_sub, args2c, {NULL}, complex_arithmetic_cases, 0, 0},

{"cdivf", (funcptr)mpc_div, args2fc, {NULL}, complex_arithmetic_cases_float, 0, 0},
{"cmulf", (funcptr)mpc_mul, args2fc, {NULL}, complex_arithmetic_cases_float, 0, 0},
{"caddf", (funcptr)mpc_add, args2fc, {NULL}, complex_arithmetic_cases_float, 0, 0},
{"csubf", (funcptr)mpc_sub, args2fc, {NULL}, complex_arithmetic_cases_float, 0, 0},

{"cabsf", (funcptr)mpc_abs, args1fcr, {NULL}, complex_arithmetic_cases_float, 0, 0},
{"cabs", (funcptr)mpc_abs, args1cr, {NULL}, complex_arithmetic_cases, 0, 0},
{"cargf", (funcptr)mpc_arg, args1fcr, {NULL}, complex_arithmetic_cases_float, 0, 0},
{"carg", (funcptr)mpc_arg, args1cr, {NULL}, complex_arithmetic_cases, 0, 0},
{"cimagf", (funcptr)mpc_imag, args1fcr, {NULL}, complex_arithmetic_cases_float, 0, 0},
{"cimag", (funcptr)mpc_imag, args1cr, {NULL}, complex_arithmetic_cases, 0, 0},
{"conjf", (funcptr)mpc_conj, args1fc, {NULL}, complex_arithmetic_cases_float, 0, 0},
{"conj", (funcptr)mpc_conj, args1c, {NULL}, complex_arithmetic_cases, 0, 0},
{"cprojf", (funcptr)mpc_proj, args1fc, {NULL}, complex_arithmetic_cases_float, 0, 0},
{"cproj", (funcptr)mpc_proj, args1c, {NULL}, complex_arithmetic_cases, 0, 0},
{"crealf", (funcptr)mpc_real, args1fcr, {NULL}, complex_arithmetic_cases_float, 0, 0},
{"creal", (funcptr)mpc_real, args1cr, {NULL}, complex_arithmetic_cases, 0, 0},
{"erfcf", (funcptr)mpfr_erfc, args1f, {NULL}, cases_biased_float, 0x1e800000, 0x41000000},
{"erfc", (funcptr)mpfr_erfc, args1, {NULL}, cases_biased, 0x3bd00000, 0x403c0000},
{"erff", (funcptr)mpfr_erf, args1f, {NULL}, cases_biased_float, 0x03800000, 0x40700000},
{"erf", (funcptr)mpfr_erf, args1, {NULL}, cases_biased, 0x00800000, 0x40200000},
{"exp2f", (funcptr)mpfr_exp2, args1f, {NULL}, cases_uniform_float, 0x33800000, 0x43c00000},
{"exp2", (funcptr)mpfr_exp2, args1, {NULL}, cases_uniform, 0x3ca00000, 0x40a00000},
{"expm1f", (funcptr)mpfr_expm1, args1f, {NULL}, cases_uniform_float, 0x33000000, 0x43800000},
{"expm1", (funcptr)mpfr_expm1, args1, {NULL}, cases_uniform, 0x3c900000, 0x409c0000},
{"fmaxf", (funcptr)mpfr_max, args2f, {NULL}, minmax_cases_float, 0, 0x7f7fffff},
{"fmax", (funcptr)mpfr_max, args2, {NULL}, minmax_cases, 0, 0x7fefffff},
{"fminf", (funcptr)mpfr_min, args2f, {NULL}, minmax_cases_float, 0, 0x7f7fffff},
{"fmin", (funcptr)mpfr_min, args2, {NULL}, minmax_cases, 0, 0x7fefffff},
{"lgammaf", (funcptr)test_lgamma, args1f, {NULL}, cases_uniform_float, 0x01800000, 0x7f800000},
{"lgamma", (funcptr)test_lgamma, args1, {NULL}, cases_uniform, 0x00100000, 0x7ff00000},
{"log1pf", (funcptr)mpfr_log1p, args1f, {NULL}, log1p_cases_float, 0, 0},
{"log1p", (funcptr)mpfr_log1p, args1, {NULL}, log1p_cases, 0, 0},
{"log2f", (funcptr)mpfr_log2, args1f, {NULL}, log_cases_float, 0, 0},
{"log2", (funcptr)mpfr_log2, args1, {NULL}, log_cases, 0, 0},
{"tgammaf", (funcptr)mpfr_gamma, args1f, {NULL}, cases_uniform_float, 0x2f800000, 0x43000000},
{"tgamma", (funcptr)mpfr_gamma, args1, {NULL}, cases_uniform, 0x3c000000, 0x40800000},
};

const int nfunctions = ( sizeof(functions)/sizeof(*functions) );

#define random_sign ( random_upto(1) ? 0x80000000 : 0 )

static int iszero(uint32 *x) {
return !((x[0] & 0x7FFFFFFF) || x[1]);
}


static void complex_log_cases(uint32 *out, uint32 param1,
uint32 param2) {
cases_uniform(out,0x00100000,0x7fefffff);
cases_uniform(out+2,0x00100000,0x7fefffff);
}


static void complex_log_cases_float(uint32 *out, uint32 param1,
uint32 param2) {
cases_uniform_float(out,0x00800000,0x7f7fffff);
cases_uniform_float(out+2,0x00800000,0x7f7fffff);
}

static void complex_cases_biased(uint32 *out, uint32 lowbound,
uint32 highbound) {
cases_biased(out,lowbound,highbound);
cases_biased(out+2,lowbound,highbound);
}

static void complex_cases_biased_float(uint32 *out, uint32 lowbound,
uint32 highbound) {
cases_biased_float(out,lowbound,highbound);
cases_biased_float(out+2,lowbound,highbound);
}

static void complex_cases_uniform(uint32 *out, uint32 lowbound,
uint32 highbound) {
cases_uniform(out,lowbound,highbound);
cases_uniform(out+2,lowbound,highbound);
}

static void complex_cases_uniform_float(uint32 *out, uint32 lowbound,
uint32 highbound) {
cases_uniform_float(out,lowbound,highbound);
cases_uniform(out+2,lowbound,highbound);
}

static void complex_pow_cases(uint32 *out, uint32 lowbound,
uint32 highbound) {

















cases_uniform(out,0x3fe00000, 0x40000000);
cases_uniform(out+2,0x3fe00000, 0x40000000);
cases_uniform(out+4,0x3f800000, 0x40600000);
cases_uniform(out+6,0x3f800000, 0x40600000);
}

static void complex_pow_cases_float(uint32 *out, uint32 lowbound,
uint32 highbound) {




cases_uniform_float(out,0x3f000000, 0x40000000);
cases_uniform_float(out+2,0x3f000000, 0x40000000);
cases_uniform_float(out+4,0x3d600000, 0x41900000);
cases_uniform_float(out+6,0x3d600000, 0x41900000);
}

static void complex_arithmetic_cases(uint32 *out, uint32 lowbound,
uint32 highbound) {
cases_uniform(out,0,0x7fefffff);
cases_uniform(out+2,0,0x7fefffff);
cases_uniform(out+4,0,0x7fefffff);
cases_uniform(out+6,0,0x7fefffff);
}

static void complex_arithmetic_cases_float(uint32 *out, uint32 lowbound,
uint32 highbound) {
cases_uniform_float(out,0,0x7f7fffff);
cases_uniform_float(out+2,0,0x7f7fffff);
cases_uniform_float(out+4,0,0x7f7fffff);
cases_uniform_float(out+6,0,0x7f7fffff);
}






void float32_case(uint32 *ret) {
int n, bits;
uint32 f;
static int premax, preptr;
static uint32 *specifics = NULL;

if (!ret) {
if (specifics)
free(specifics);
specifics = NULL;
premax = preptr = 0;
return;
}

if (!specifics) {
int exps[] = {
-127, -126, -125, -24, -4, -3, -2, -1, 0, 1, 2, 3, 4,
24, 29, 30, 31, 32, 61, 62, 63, 64, 126, 127, 128
};
int sign, eptr;
uint32 se, j;














specifics = malloc(4400 * sizeof(*specifics));
preptr = 0;
for (sign = 0; sign <= 1; sign++) {
for (eptr = 0; eptr < sizeof(exps)/sizeof(*exps); eptr++) {
se = (sign ? 0x80000000 : 0) | ((exps[eptr]+127) << 23);



specifics[preptr++] = se | 0;



specifics[preptr++] = se | 0x7FFFFF;



for (j = 1; j && j <= 0x400000; j <<= 1)
specifics[preptr++] = se | j;



for (j = 1; j && j <= 0x400000; j <<= 1)
specifics[preptr++] = se | (0x7FFFFF ^ j);



for (j = 2; j && j <= 0x100000; j <<= 1)
specifics[preptr++] = se | (2*j-1);



for (j = 4; j && j <= 0x200000; j <<= 1)
specifics[preptr++] = se | (0x7FFFFF ^ (j-1));



}
}
assert(preptr == 4400);
premax = preptr;
}




n = random32() % (premax+1);
if (n < preptr) {



uint32 t;
t = specifics[n];
specifics[n] = specifics[preptr-1];
specifics[preptr-1] = t;
preptr--;
*ret = t;
} else {











n = random32() % 5;
f = random32();
bits = random32() % 22 + 1;
switch (n) {
case 0:
break;
case 1:
f |= (1<<bits)-1;
break;
case 2:
f &= ~((1<<bits)-1);
break;
case 3:
f |= ~((1<<bits)-1);
break;
case 4:
f &= (1<<bits)-1;
break;
}
f &= 0x7FFFFF;
f |= (random32() & 0xFF800000);
*ret = f;
}
}
static void float64_case(uint32 *ret) {
int n, bits;
uint32 f, g;
static int premax, preptr;
static uint32 (*specifics)[2] = NULL;

if (!ret) {
if (specifics)
free(specifics);
specifics = NULL;
premax = preptr = 0;
return;
}

if (!specifics) {
int exps[] = {
-1023, -1022, -1021, -129, -128, -127, -126, -53, -4, -3, -2,
-1, 0, 1, 2, 3, 4, 29, 30, 31, 32, 53, 61, 62, 63, 64, 127,
128, 129, 1022, 1023, 1024
};
int sign, eptr;
uint32 se, j;














specifics = malloc(13056 * sizeof(*specifics));
preptr = 0;
for (sign = 0; sign <= 1; sign++) {
for (eptr = 0; eptr < sizeof(exps)/sizeof(*exps); eptr++) {
se = (sign ? 0x80000000 : 0) | ((exps[eptr]+1023) << 20);



specifics[preptr][0] = 0;
specifics[preptr][1] = 0;
specifics[preptr++][0] |= se;



specifics[preptr][0] = 0xFFFFF;
specifics[preptr][1] = ~0;
specifics[preptr++][0] |= se;



for (j = 1; j && j <= 0x80000000; j <<= 1) {
specifics[preptr][0] = 0;
specifics[preptr][1] = j;
specifics[preptr++][0] |= se;
if (j & 0xFFFFF) {
specifics[preptr][0] = j;
specifics[preptr][1] = 0;
specifics[preptr++][0] |= se;
}
}



for (j = 1; j && j <= 0x80000000; j <<= 1) {
specifics[preptr][0] = 0xFFFFF;
specifics[preptr][1] = ~j;
specifics[preptr++][0] |= se;
if (j & 0xFFFFF) {
specifics[preptr][0] = 0xFFFFF ^ j;
specifics[preptr][1] = ~0;
specifics[preptr++][0] |= se;
}
}



for (j = 2; j && j <= 0x80000000; j <<= 1) {
specifics[preptr][0] = 0;
specifics[preptr][1] = 2*j-1;
specifics[preptr++][0] |= se;
}
for (j = 1; j && j <= 0x20000; j <<= 1) {
specifics[preptr][0] = 2*j-1;
specifics[preptr][1] = ~0;
specifics[preptr++][0] |= se;
}



for (j = 4; j && j <= 0x80000000; j <<= 1) {
specifics[preptr][0] = 0xFFFFF;
specifics[preptr][1] = ~(j-1);
specifics[preptr++][0] |= se;
}
for (j = 1; j && j <= 0x40000; j <<= 1) {
specifics[preptr][0] = 0xFFFFF ^ (j-1);
specifics[preptr][1] = 0;
specifics[preptr++][0] |= se;
}



}
}
assert(preptr == 13056);
premax = preptr;
}




n = (uint32) random32() % (uint32) (premax+1);
if (n < preptr) {



uint32 t;
t = specifics[n][0];
specifics[n][0] = specifics[preptr-1][0];
specifics[preptr-1][0] = t;
ret[0] = t;
t = specifics[n][1];
specifics[n][1] = specifics[preptr-1][1];
specifics[preptr-1][1] = t;
ret[1] = t;
preptr--;
} else {











n = random32() % 5;
f = random32();
g = random32();
bits = random32() % 51 + 1;
switch (n) {
case 0:
break;
case 1:
if (bits <= 32)
f |= (1<<bits)-1;
else {
bits -= 32;
g |= (1<<bits)-1;
f = ~0;
}
break;
case 2:
if (bits <= 32)
f &= ~((1<<bits)-1);
else {
bits -= 32;
g &= ~((1<<bits)-1);
f = 0;
}
break;
case 3:
if (bits <= 32)
g &= (1<<bits)-1;
else {
bits -= 32;
f &= (1<<bits)-1;
g = 0;
}
break;
case 4:
if (bits <= 32)
g |= ~((1<<bits)-1);
else {
bits -= 32;
f |= ~((1<<bits)-1);
g = ~0;
}
break;
}
g &= 0xFFFFF;
g |= (random32() & 0xFFF00000);
ret[0] = g;
ret[1] = f;
}
}

static void cases_biased(uint32 *out, uint32 lowbound,
uint32 highbound) {
do {
out[0] = highbound - random_upto_biased(highbound-lowbound, 8);
out[1] = random_upto(0xFFFFFFFF);
out[0] |= random_sign;
} while (iszero(out));
}

static void cases_biased_positive(uint32 *out, uint32 lowbound,
uint32 highbound) {
do {
out[0] = highbound - random_upto_biased(highbound-lowbound, 8);
out[1] = random_upto(0xFFFFFFFF);
} while (iszero(out));
}

static void cases_biased_float(uint32 *out, uint32 lowbound,
uint32 highbound) {
do {
out[0] = highbound - random_upto_biased(highbound-lowbound, 8);
out[1] = 0;
out[0] |= random_sign;
} while (iszero(out));
}

static void cases_semi1(uint32 *out, uint32 param1,
uint32 param2) {
float64_case(out);
}

static void cases_semi1_float(uint32 *out, uint32 param1,
uint32 param2) {
float32_case(out);
}

static void cases_semi2(uint32 *out, uint32 param1,
uint32 param2) {
float64_case(out);
float64_case(out+2);
}

static void cases_semi2_float(uint32 *out, uint32 param1,
uint32 param2) {
float32_case(out);
float32_case(out+2);
}

static void cases_ldexp(uint32 *out, uint32 param1,
uint32 param2) {
float64_case(out);
out[2] = random_upto(2048)-1024;
}

static void cases_ldexp_float(uint32 *out, uint32 param1,
uint32 param2) {
float32_case(out);
out[2] = random_upto(256)-128;
}

static void cases_uniform(uint32 *out, uint32 lowbound,
uint32 highbound) {
do {
out[0] = highbound - random_upto(highbound-lowbound);
out[1] = random_upto(0xFFFFFFFF);
out[0] |= random_sign;
} while (iszero(out));
}
static void cases_uniform_float(uint32 *out, uint32 lowbound,
uint32 highbound) {
do {
out[0] = highbound - random_upto(highbound-lowbound);
out[1] = 0;
out[0] |= random_sign;
} while (iszero(out));
}

static void cases_uniform_positive(uint32 *out, uint32 lowbound,
uint32 highbound) {
do {
out[0] = highbound - random_upto(highbound-lowbound);
out[1] = random_upto(0xFFFFFFFF);
} while (iszero(out));
}
static void cases_uniform_float_positive(uint32 *out, uint32 lowbound,
uint32 highbound) {
do {
out[0] = highbound - random_upto(highbound-lowbound);
out[1] = 0;
} while (iszero(out));
}


static void log_cases(uint32 *out, uint32 param1,
uint32 param2) {
do {
out[0] = random_upto(0x7FEFFFFF);
out[1] = random_upto(0xFFFFFFFF);
} while (iszero(out));
}

static void log_cases_float(uint32 *out, uint32 param1,
uint32 param2) {
do {
out[0] = random_upto(0x7F7FFFFF);
out[1] = 0;
} while (iszero(out));
}

static void log1p_cases(uint32 *out, uint32 param1, uint32 param2)
{
uint32 sign = random_sign;
if (sign == 0) {
cases_uniform_positive(out, 0x3c700000, 0x43400000);
} else {
cases_uniform_positive(out, 0x3c000000, 0x3ff00000);
}
out[0] |= sign;
}

static void log1p_cases_float(uint32 *out, uint32 param1, uint32 param2)
{
uint32 sign = random_sign;
if (sign == 0) {
cases_uniform_float_positive(out, 0x32000000, 0x4c000000);
} else {
cases_uniform_float_positive(out, 0x30000000, 0x3f800000);
}
out[0] |= sign;
}

static void minmax_cases(uint32 *out, uint32 param1, uint32 param2)
{
do {
out[0] = random_upto(0x7FEFFFFF);
out[1] = random_upto(0xFFFFFFFF);
out[0] |= random_sign;
out[2] = random_upto(0x7FEFFFFF);
out[3] = random_upto(0xFFFFFFFF);
out[2] |= random_sign;
} while (iszero(out));
}

static void minmax_cases_float(uint32 *out, uint32 param1, uint32 param2)
{
do {
out[0] = random_upto(0x7F7FFFFF);
out[1] = 0;
out[0] |= random_sign;
out[2] = random_upto(0x7F7FFFFF);
out[3] = 0;
out[2] |= random_sign;
} while (iszero(out));
}

static void rred_cases(uint32 *out, uint32 param1,
uint32 param2) {
do {
out[0] = ((0x3fc00000 + random_upto(0x036fffff)) |
(random_upto(1) << 31));
out[1] = random_upto(0xFFFFFFFF);
} while (iszero(out));
}

static void rred_cases_float(uint32 *out, uint32 param1,
uint32 param2) {
do {
out[0] = ((0x3e000000 + random_upto(0x0cffffff)) |
(random_upto(1) << 31));
out[1] = 0;
} while (iszero(out));
}

static void atan2_cases(uint32 *out, uint32 param1,
uint32 param2) {
do {
int expdiff = random_upto(101)-51;
int swap;
if (expdiff < 0) {
expdiff = -expdiff;
swap = 2;
} else
swap = 0;
out[swap ^ 0] = random_upto(0x7FEFFFFF-((expdiff+1)<<20));
out[swap ^ 2] = random_upto(((expdiff+1)<<20)-1) + out[swap ^ 0];
out[1] = random_upto(0xFFFFFFFF);
out[3] = random_upto(0xFFFFFFFF);
out[0] |= random_sign;
out[2] |= random_sign;
} while (iszero(out) || iszero(out+2));
}

static void atan2_cases_float(uint32 *out, uint32 param1,
uint32 param2) {
do {
int expdiff = random_upto(44)-22;
int swap;
if (expdiff < 0) {
expdiff = -expdiff;
swap = 2;
} else
swap = 0;
out[swap ^ 0] = random_upto(0x7F7FFFFF-((expdiff+1)<<23));
out[swap ^ 2] = random_upto(((expdiff+1)<<23)-1) + out[swap ^ 0];
out[0] |= random_sign;
out[2] |= random_sign;
out[1] = out[3] = 0;
} while (iszero(out) || iszero(out+2));
}

static void pow_cases(uint32 *out, uint32 param1,
uint32 param2) {
























int e, n;
uint32 dmin, dmax;
const uint32 pmin = 0x3e100000;




e = (random_upto(1) ?
0x3FE - random_upto_biased(0x431,2) :
0x3FF + random_upto_biased(0x3FF,2));




if (e < 0x3FE || e > 0x3FF) {
uint32 imin, imax;
if (e < 0x3FE)
imin = 0x40000 / (0x3FE - e), imax = 0x43200 / (0x3FE - e);
else
imin = 0x43200 / (e - 0x3FF), imax = 0x40000 / (e - 0x3FF);

dmin = doubletop(imin, -8);
dmax = doubletop(imax, -8);

n = (e > 0 ? 53 : 52+e);
} else {

n = 52 - random_upto_biased(51, 4);
if (e == 0x3FE)
dmax = 63 - n;
else
dmax = 62 - n;
dmax = (dmax << 20) + 0x3FF00000;
dmin = dmax;
}

if (n <= 32) {
out[0] = 0;
out[1] = random_upto((1 << (n-1)) - 1) + (1 << (n-1));
} else if (n == 33) {
out[0] = 1;
out[1] = random_upto(0xFFFFFFFF);
} else if (n > 33) {
out[0] = random_upto((1 << (n-33)) - 1) + (1 << (n-33));
out[1] = random_upto(0xFFFFFFFF);
}

if (e == 0x3FE) {
out[1] = -out[1];
out[0] = -out[0];
if (out[1]) out[0]--;
}

out[0] &= 0xFFFFF;
out[0] |= ((e > 0 ? e : 0) << 20);

if (random_upto(1)) {

out[2] = dmax - random_upto_biased(dmax-pmin, 10);
} else {

out[2] = (dmin - random_upto_biased(dmin-pmin, 10)) | 0x80000000;
}
out[3] = random_upto(0xFFFFFFFF);
}
static void pow_cases_float(uint32 *out, uint32 param1,
uint32 param2) {























int e, n;
uint32 dmin, dmax;
const uint32 pmin = 0x38000000;




e = (random_upto(1) ?
0x7E - random_upto_biased(0x94,2) :
0x7F + random_upto_biased(0x7f,2));




if (e < 0x7E || e > 0x7F) {
uint32 imin, imax;
if (e < 0x7E)
imin = 0x8000 / (0x7e - e), imax = 0x9500 / (0x7e - e);
else
imin = 0x9500 / (e - 0x7f), imax = 0x8000 / (e - 0x7f);

dmin = floatval(imin, -8);
dmax = floatval(imax, -8);

n = (e > 0 ? 24 : 23+e);
} else {

n = 23 - random_upto_biased(22, 4);
if (e == 0x7E)
dmax = 31 - n;
else
dmax = 30 - n;
dmax = (dmax << 23) + 0x3F800000;
dmin = dmax;
}

out[0] = random_upto((1 << (n-1)) - 1) + (1 << (n-1));
out[1] = 0;

if (e == 0x7E) {
out[0] = -out[0];
}

out[0] &= 0x7FFFFF;
out[0] |= ((e > 0 ? e : 0) << 23);

if (random_upto(1)) {

out[2] = dmax - random_upto_biased(dmax-pmin, 10);
} else {

out[2] = (dmin - random_upto_biased(dmin-pmin, 10)) | 0x80000000;
}
out[3] = 0;
}

void vet_for_decline(Testable *fn, uint32 *args, uint32 *result, int got_errno_in) {
int declined = 0;

switch (fn->type) {
case args1:
case rred:
case semi1:
case t_frexp:
case t_modf:
case classify:
case t_ldexp:
declined |= lib_fo && is_dhard(args+0);
break;
case args1f:
case rredf:
case semi1f:
case t_frexpf:
case t_modff:
case classifyf:
declined |= lib_fo && is_shard(args+0);
break;
case args2:
case semi2:
case args1c:
case args1cr:
case compare:
declined |= lib_fo && is_dhard(args+0);
declined |= lib_fo && is_dhard(args+2);
break;
case args2f:
case semi2f:
case t_ldexpf:
case comparef:
case args1fc:
case args1fcr:
declined |= lib_fo && is_shard(args+0);
declined |= lib_fo && is_shard(args+2);
break;
case args2c:
declined |= lib_fo && is_dhard(args+0);
declined |= lib_fo && is_dhard(args+2);
declined |= lib_fo && is_dhard(args+4);
declined |= lib_fo && is_dhard(args+6);
break;
case args2fc:
declined |= lib_fo && is_shard(args+0);
declined |= lib_fo && is_shard(args+2);
declined |= lib_fo && is_shard(args+4);
declined |= lib_fo && is_shard(args+6);
break;
}

switch (fn->type) {
case args1:
case args2:
case rred:
case semi1:
case semi2:
case t_ldexp:
case t_frexp:
case args1cr:
declined |= lib_fo && is_dhard(result);
break;
case args1f:
case args2f:
case rredf:
case semi1f:
case semi2f:
case t_ldexpf:
case args1fcr:
declined |= lib_fo && is_shard(result);
break;
case t_modf:
declined |= lib_fo && is_dhard(result+0);
declined |= lib_fo && is_dhard(result+2);
break;
case t_modff:
declined |= lib_fo && is_shard(result+2);

case t_frexpf:
declined |= lib_fo && is_shard(result+0);
break;
case args1c:
case args2c:
declined |= lib_fo && is_dhard(result+0);
declined |= lib_fo && is_dhard(result+4);
break;
case args1fc:
case args2fc:
declined |= lib_fo && is_shard(result+0);
declined |= lib_fo && is_shard(result+4);
break;
}



declined |= (lib_no_arith && (fn->func == (funcptr)mpc_add ||
fn->func == (funcptr)mpc_sub ||
fn->func == (funcptr)mpc_mul ||
fn->func == (funcptr)mpc_div));

if (!declined) {
if (got_errno_in)
ntests++;
else
ntests += 3;
}
}

void docase(Testable *fn, uint32 *args) {
uint32 result[8];
char *errstr = NULL;
mpfr_t a, b, r;
mpc_t ac, bc, rc;
int rejected, printextra;
wrapperctx ctx;

mpfr_init2(a, MPFR_PREC);
mpfr_init2(b, MPFR_PREC);
mpfr_init2(r, MPFR_PREC);
mpc_init2(ac, MPFR_PREC);
mpc_init2(bc, MPFR_PREC);
mpc_init2(rc, MPFR_PREC);

printf("func=%s", fn->name);

rejected = 0;

switch (fn->type) {
case args1:
case rred:
case semi1:
case t_frexp:
case t_modf:
case classify:
printf(" op1=%08x.%08x", args[0], args[1]);
break;
case args1f:
case rredf:
case semi1f:
case t_frexpf:
case t_modff:
case classifyf:
printf(" op1=%08x", args[0]);
break;
case args2:
case semi2:
case compare:
printf(" op1=%08x.%08x", args[0], args[1]);
printf(" op2=%08x.%08x", args[2], args[3]);
break;
case args2f:
case semi2f:
case t_ldexpf:
case comparef:
printf(" op1=%08x", args[0]);
printf(" op2=%08x", args[2]);
break;
case t_ldexp:
printf(" op1=%08x.%08x", args[0], args[1]);
printf(" op2=%08x", args[2]);
break;
case args1c:
case args1cr:
printf(" op1r=%08x.%08x", args[0], args[1]);
printf(" op1i=%08x.%08x", args[2], args[3]);
break;
case args2c:
printf(" op1r=%08x.%08x", args[0], args[1]);
printf(" op1i=%08x.%08x", args[2], args[3]);
printf(" op2r=%08x.%08x", args[4], args[5]);
printf(" op2i=%08x.%08x", args[6], args[7]);
break;
case args1fc:
case args1fcr:
printf(" op1r=%08x", args[0]);
printf(" op1i=%08x", args[2]);
break;
case args2fc:
printf(" op1r=%08x", args[0]);
printf(" op1i=%08x", args[2]);
printf(" op2r=%08x", args[4]);
printf(" op2i=%08x", args[6]);
break;
default:
fprintf(stderr, "internal inconsistency?!\n");
abort();
}

if (rejected == 2) {
printf(" - test case rejected\n");
goto cleanup;
}

wrapper_init(&ctx);

if (rejected == 0) {
switch (fn->type) {
case args1:
set_mpfr_d(a, args[0], args[1]);
wrapper_op_real(&ctx, a, 2, args);
((testfunc1)(fn->func))(r, a, GMP_RNDN);
get_mpfr_d(r, &result[0], &result[1], &result[2]);
wrapper_result_real(&ctx, r, 2, result);
if (wrapper_run(&ctx, fn->wrappers))
get_mpfr_d(r, &result[0], &result[1], &result[2]);
break;
case args1cr:
set_mpc_d(ac, args[0], args[1], args[2], args[3]);
wrapper_op_complex(&ctx, ac, 2, args);
((testfunc1cr)(fn->func))(r, ac, GMP_RNDN);
get_mpfr_d(r, &result[0], &result[1], &result[2]);
wrapper_result_real(&ctx, r, 2, result);
if (wrapper_run(&ctx, fn->wrappers))
get_mpfr_d(r, &result[0], &result[1], &result[2]);
break;
case args1f:
set_mpfr_f(a, args[0]);
wrapper_op_real(&ctx, a, 1, args);
((testfunc1)(fn->func))(r, a, GMP_RNDN);
get_mpfr_f(r, &result[0], &result[1]);
wrapper_result_real(&ctx, r, 1, result);
if (wrapper_run(&ctx, fn->wrappers))
get_mpfr_f(r, &result[0], &result[1]);
break;
case args1fcr:
set_mpc_f(ac, args[0], args[2]);
wrapper_op_complex(&ctx, ac, 1, args);
((testfunc1cr)(fn->func))(r, ac, GMP_RNDN);
get_mpfr_f(r, &result[0], &result[1]);
wrapper_result_real(&ctx, r, 1, result);
if (wrapper_run(&ctx, fn->wrappers))
get_mpfr_f(r, &result[0], &result[1]);
break;
case args2:
set_mpfr_d(a, args[0], args[1]);
wrapper_op_real(&ctx, a, 2, args);
set_mpfr_d(b, args[2], args[3]);
wrapper_op_real(&ctx, b, 2, args+2);
((testfunc2)(fn->func))(r, a, b, GMP_RNDN);
get_mpfr_d(r, &result[0], &result[1], &result[2]);
wrapper_result_real(&ctx, r, 2, result);
if (wrapper_run(&ctx, fn->wrappers))
get_mpfr_d(r, &result[0], &result[1], &result[2]);
break;
case args2f:
set_mpfr_f(a, args[0]);
wrapper_op_real(&ctx, a, 1, args);
set_mpfr_f(b, args[2]);
wrapper_op_real(&ctx, b, 1, args+2);
((testfunc2)(fn->func))(r, a, b, GMP_RNDN);
get_mpfr_f(r, &result[0], &result[1]);
wrapper_result_real(&ctx, r, 1, result);
if (wrapper_run(&ctx, fn->wrappers))
get_mpfr_f(r, &result[0], &result[1]);
break;
case rred:
set_mpfr_d(a, args[0], args[1]);
wrapper_op_real(&ctx, a, 2, args);
((testrred)(fn->func))(r, a, (int *)&result[3]);
get_mpfr_d(r, &result[0], &result[1], &result[2]);
wrapper_result_real(&ctx, r, 2, result);


if (wrapper_run(&ctx, fn->wrappers))
get_mpfr_d(r, &result[0], &result[1], &result[2]);
break;
case rredf:
set_mpfr_f(a, args[0]);
wrapper_op_real(&ctx, a, 1, args);
((testrred)(fn->func))(r, a, (int *)&result[3]);
get_mpfr_f(r, &result[0], &result[1]);
wrapper_result_real(&ctx, r, 1, result);


if (wrapper_run(&ctx, fn->wrappers))
get_mpfr_f(r, &result[0], &result[1]);
break;
case semi1:
case semi1f:
errstr = ((testsemi1)(fn->func))(args, result);
break;
case semi2:
case compare:
errstr = ((testsemi2)(fn->func))(args, args+2, result);
break;
case semi2f:
case comparef:
case t_ldexpf:
errstr = ((testsemi2f)(fn->func))(args, args+2, result);
break;
case t_ldexp:
errstr = ((testldexp)(fn->func))(args, args+2, result);
break;
case t_frexp:
errstr = ((testfrexp)(fn->func))(args, result, result+2);
break;
case t_frexpf:
errstr = ((testfrexp)(fn->func))(args, result, result+2);
break;
case t_modf:
errstr = ((testmodf)(fn->func))(args, result, result+2);
break;
case t_modff:
errstr = ((testmodf)(fn->func))(args, result, result+2);
break;
case classify:
errstr = ((testclassify)(fn->func))(args, &result[0]);
break;
case classifyf:
errstr = ((testclassifyf)(fn->func))(args, &result[0]);
break;
case args1c:
set_mpc_d(ac, args[0], args[1], args[2], args[3]);
wrapper_op_complex(&ctx, ac, 2, args);
((testfunc1c)(fn->func))(rc, ac, MPC_RNDNN);
get_mpc_d(rc, &result[0], &result[1], &result[2], &result[4], &result[5], &result[6]);
wrapper_result_complex(&ctx, rc, 2, result);
if (wrapper_run(&ctx, fn->wrappers))
get_mpc_d(rc, &result[0], &result[1], &result[2], &result[4], &result[5], &result[6]);
break;
case args2c:
set_mpc_d(ac, args[0], args[1], args[2], args[3]);
wrapper_op_complex(&ctx, ac, 2, args);
set_mpc_d(bc, args[4], args[5], args[6], args[7]);
wrapper_op_complex(&ctx, bc, 2, args+4);
((testfunc2c)(fn->func))(rc, ac, bc, MPC_RNDNN);
get_mpc_d(rc, &result[0], &result[1], &result[2], &result[4], &result[5], &result[6]);
wrapper_result_complex(&ctx, rc, 2, result);
if (wrapper_run(&ctx, fn->wrappers))
get_mpc_d(rc, &result[0], &result[1], &result[2], &result[4], &result[5], &result[6]);
break;
case args1fc:
set_mpc_f(ac, args[0], args[2]);
wrapper_op_complex(&ctx, ac, 1, args);
((testfunc1c)(fn->func))(rc, ac, MPC_RNDNN);
get_mpc_f(rc, &result[0], &result[1], &result[4], &result[5]);
wrapper_result_complex(&ctx, rc, 1, result);
if (wrapper_run(&ctx, fn->wrappers))
get_mpc_f(rc, &result[0], &result[1], &result[4], &result[5]);
break;
case args2fc:
set_mpc_f(ac, args[0], args[2]);
wrapper_op_complex(&ctx, ac, 1, args);
set_mpc_f(bc, args[4], args[6]);
wrapper_op_complex(&ctx, bc, 1, args+4);
((testfunc2c)(fn->func))(rc, ac, bc, MPC_RNDNN);
get_mpc_f(rc, &result[0], &result[1], &result[4], &result[5]);
wrapper_result_complex(&ctx, rc, 1, result);
if (wrapper_run(&ctx, fn->wrappers))
get_mpc_f(rc, &result[0], &result[1], &result[4], &result[5]);
break;
default:
fprintf(stderr, "internal inconsistency?!\n");
abort();
}
}

switch (fn->type) {
case args1:
case args2:
case args1cr:
case rred:
printextra = 1;
if (rejected == 0) {
errstr = NULL;
if (!mpfr_zero_p(a)) {
if ((result[0] & 0x7FFFFFFF) == 0 && result[1] == 0) {
































if (result[2] < 0x40000000) {




errstr = "underflow";
printextra = 0;
} else {



errstr = "?underflow";
}
} else if (!(result[0] & 0x7ff00000)) {









if (!strcmp(fn->name, "fmax") ||
!strcmp(fn->name, "fmin") ||
!strcmp(fn->name, "creal") ||
!strcmp(fn->name, "cimag")) {

} else {
errstr = "u";
}
} else if ((result[0] & 0x7FFFFFFF) > 0x7FEFFFFF) {





if (!strcmp(fn->name, "lgamma") &&
(args[0] & 0x80000000) != 0 &&
is_dinteger(args)) {
errstr = "ERANGE status=z";
} else {
errstr = "overflow";
}
printextra = 0;
}
} else {

if (!strcmp(fn->name, "lgamma")) {
errstr = "ERANGE status=z";
printextra = 0;
}
}
}

if (!printextra || (rejected && !(rejected==1 && result[2]!=0))) {
printf(" result=%08x.%08x",
result[0], result[1]);
} else {
printf(" result=%08x.%08x.%03x",
result[0], result[1], (result[2] >> 20) & 0xFFF);
}
if (fn->type == rred) {
printf(" res2=%08x", result[3]);
}
break;
case args1f:
case args2f:
case args1fcr:
case rredf:
printextra = 1;
if (rejected == 0) {
errstr = NULL;
if (!mpfr_zero_p(a)) {
if ((result[0] & 0x7FFFFFFF) == 0) {






if (result[1] < 0x40000000) {
errstr = "underflow";
printextra = 0;
} else {
errstr = "?underflow";
}
} else if (!(result[0] & 0x7f800000)) {






if (!strcmp(fn->name, "fmaxf") ||
!strcmp(fn->name, "fminf") ||
!strcmp(fn->name, "crealf") ||
!strcmp(fn->name, "cimagf")) {

} else {
errstr = "u";
}
} else if ((result[0] & 0x7FFFFFFF) > 0x7F7FFFFF) {





if (!strcmp(fn->name, "lgammaf") &&
(args[0] & 0x80000000) != 0 &&
is_sinteger(args)) {
errstr = "ERANGE status=z";
} else {
errstr = "overflow";
}
printextra = 0;
}
} else {

if (!strcmp(fn->name, "lgammaf")) {
errstr = "ERANGE status=z";
printextra = 0;
}
}
}

if (!printextra || (rejected && !(rejected==1 && result[1]!=0))) {
printf(" result=%08x",
result[0]);
} else {
printf(" result=%08x.%03x",
result[0], (result[1] >> 20) & 0xFFF);
}
if (fn->type == rredf) {
printf(" res2=%08x", result[3]);
}
break;
case semi1:
case semi2:
case t_ldexp:
printf(" result=%08x.%08x", result[0], result[1]);
break;
case semi1f:
case semi2f:
case t_ldexpf:
printf(" result=%08x", result[0]);
break;
case t_frexp:
printf(" result=%08x.%08x res2=%08x", result[0], result[1],
result[2]);
break;
case t_modf:
printf(" result=%08x.%08x res2=%08x.%08x",
result[0], result[1], result[2], result[3]);
break;
case t_modff:

case t_frexpf:
printf(" result=%08x res2=%08x", result[0], result[2]);
break;
case classify:
case classifyf:
case compare:
case comparef:
printf(" result=%x", result[0]);
break;
case args1c:
case args2c:
if (0) {
printf(" resultr=%08x.%08x", result[0], result[1]);
printf(" resulti=%08x.%08x", result[4], result[5]);
} else {
printf(" resultr=%08x.%08x.%03x",
result[0], result[1], (result[2] >> 20) & 0xFFF);
printf(" resulti=%08x.%08x.%03x",
result[4], result[5], (result[6] >> 20) & 0xFFF);
}

errstr = "?underflow";
break;
case args1fc:
case args2fc:
if (0) {
printf(" resultr=%08x", result[0]);
printf(" resulti=%08x", result[4]);
} else {
printf(" resultr=%08x.%03x",
result[0], (result[1] >> 20) & 0xFFF);
printf(" resulti=%08x.%03x",
result[4], (result[5] >> 20) & 0xFFF);
}

errstr = "?underflow";
break;
}

if (errstr && *(errstr+1) == '\0') {
printf(" errno=0 status=%c",*errstr);
} else if (errstr && *errstr == '?') {
printf(" maybeerror=%s", errstr+1);
} else if (errstr && errstr[0] == 'E') {
printf(" errno=%s", errstr);
} else {
printf(" error=%s", errstr && *errstr ? errstr : "0");
}

printf("\n");

vet_for_decline(fn, args, result, 0);

cleanup:
mpfr_clear(a);
mpfr_clear(b);
mpfr_clear(r);
mpc_clear(ac);
mpc_clear(bc);
mpc_clear(rc);
}

void gencases(Testable *fn, int number) {
int i;
uint32 args[8];

float32_case(NULL);
float64_case(NULL);

printf("random=on\n");
for (i = 0; i < number; i++) {

fn->cases(args, fn->caseparam1, fn->caseparam2);
docase(fn, args);
}
printf("random=off\n");
}

static uint32 doubletop(int x, int scale) {
int e = 0x412 + scale;
while (!(x & 0x100000))
x <<= 1, e--;
return (e << 20) + x;
}

static uint32 floatval(int x, int scale) {
int e = 0x95 + scale;
while (!(x & 0x800000))
x <<= 1, e--;
return (e << 23) + x;
}
