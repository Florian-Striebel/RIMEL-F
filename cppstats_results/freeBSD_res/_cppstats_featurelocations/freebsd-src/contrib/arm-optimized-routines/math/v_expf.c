






#include "mathlib.h"
#include "v_math.h"
#if V_SUPPORTED

static const float Poly[] = {

0x1.0e4020p-7f,
0x1.573e2ep-5f,
0x1.555e66p-3f,
0x1.fffdb6p-2f,
0x1.ffffecp-1f,
};
#define C0 v_f32 (Poly[0])
#define C1 v_f32 (Poly[1])
#define C2 v_f32 (Poly[2])
#define C3 v_f32 (Poly[3])
#define C4 v_f32 (Poly[4])

#define Shift v_f32 (0x1.8p23f)
#define InvLn2 v_f32 (0x1.715476p+0f)
#define Ln2hi v_f32 (0x1.62e4p-1f)
#define Ln2lo v_f32 (0x1.7f7d1cp-20f)

VPCS_ATTR
static v_f32_t
specialcase (v_f32_t poly, v_f32_t n, v_u32_t e, v_f32_t absn, v_u32_t cmp1, v_f32_t scale)
{

v_u32_t b = v_cond_u32 (n <= v_f32 (0.0f)) & v_u32 (0x82000000);
v_f32_t s1 = v_as_f32_u32 (v_u32 (0x7f000000) + b);
v_f32_t s2 = v_as_f32_u32 (e - b);
v_u32_t cmp2 = v_cond_u32 (absn > v_f32 (192.0f));
v_u32_t r2 = v_as_u32_f32 (s1 * s1);
v_u32_t r1 = v_as_u32_f32 (v_fma_f32 (poly, s2, s2) * s1);

v_u32_t r0 = v_as_u32_f32 (v_fma_f32 (poly, scale, scale));
return v_as_f32_u32 ((cmp2 & r2) | (~cmp2 & cmp1 & r1) | (~cmp1 & r0));
}

VPCS_ATTR
v_f32_t
V_NAME(expf) (v_f32_t x)
{
v_f32_t n, r, r2, scale, p, q, poly, absn, z;
v_u32_t cmp, e;



#if 1
z = v_fma_f32 (x, InvLn2, Shift);
n = z - Shift;
r = v_fma_f32 (n, -Ln2hi, x);
r = v_fma_f32 (n, -Ln2lo, r);
e = v_as_u32_f32 (z) << 23;
#else
z = x * InvLn2;
n = v_round_f32 (z);
r = v_fma_f32 (n, -Ln2hi, x);
r = v_fma_f32 (n, -Ln2lo, r);
e = v_as_u32_s32 (v_round_s32 (z)) << 23;
#endif
scale = v_as_f32_u32 (e + v_u32 (0x3f800000));
absn = v_abs_f32 (n);
cmp = v_cond_u32 (absn > v_f32 (126.0f));
r2 = r * r;
p = v_fma_f32 (C0, r, C1);
q = v_fma_f32 (C2, r, C3);
q = v_fma_f32 (p, r2, q);
p = C4 * r;
poly = v_fma_f32 (q, r2, p);
if (unlikely (v_any_u32 (cmp)))
return specialcase (poly, n, e, absn, cmp, scale);
return v_fma_f32 (poly, scale, scale);
}
VPCS_ALIAS
#endif
