#include "test/jemalloc_test.h"

static const uint64_t smoothstep_tab[] = {
#define STEP(step, h, x, y) h,

SMOOTHSTEP
#undef STEP
};

TEST_BEGIN(test_smoothstep_integral) {
uint64_t sum, min, max;
unsigned i;








sum = 0;
for (i = 0; i < SMOOTHSTEP_NSTEPS; i++) {
sum += smoothstep_tab[i];
}

max = (KQU(1) << (SMOOTHSTEP_BFP-1)) * (SMOOTHSTEP_NSTEPS+1);
min = max - SMOOTHSTEP_NSTEPS;

assert_u64_ge(sum, min,
"Integral too small, even accounting for truncation");
assert_u64_le(sum, max, "Integral exceeds 1/2");
if (false) {
malloc_printf("%"FMTu64" ulps under 1/2 (limit %d)\n",
max - sum, SMOOTHSTEP_NSTEPS);
}
}
TEST_END

TEST_BEGIN(test_smoothstep_monotonic) {
uint64_t prev_h;
unsigned i;







prev_h = 0;
for (i = 0; i < SMOOTHSTEP_NSTEPS; i++) {
uint64_t h = smoothstep_tab[i];
assert_u64_ge(h, prev_h, "Piecewise non-monotonic, i=%u", i);
prev_h = h;
}
assert_u64_eq(smoothstep_tab[SMOOTHSTEP_NSTEPS-1],
(KQU(1) << SMOOTHSTEP_BFP), "Last step must equal 1");
}
TEST_END

TEST_BEGIN(test_smoothstep_slope) {
uint64_t prev_h, prev_delta;
unsigned i;







prev_h = 0;
prev_delta = 0;
for (i = 0; i < SMOOTHSTEP_NSTEPS / 2 + SMOOTHSTEP_NSTEPS % 2; i++) {
uint64_t h = smoothstep_tab[i];
uint64_t delta = h - prev_h;
assert_u64_ge(delta, prev_delta,
"Slope must monotonically increase in 0.0 <= x <= 0.5, "
"i=%u", i);
prev_h = h;
prev_delta = delta;
}

prev_h = KQU(1) << SMOOTHSTEP_BFP;
prev_delta = 0;
for (i = SMOOTHSTEP_NSTEPS-1; i >= SMOOTHSTEP_NSTEPS / 2; i--) {
uint64_t h = smoothstep_tab[i];
uint64_t delta = prev_h - h;
assert_u64_ge(delta, prev_delta,
"Slope must monotonically decrease in 0.5 <= x <= 1.0, "
"i=%u", i);
prev_h = h;
prev_delta = delta;
}
}
TEST_END

int
main(void) {
return test(
test_smoothstep_integral,
test_smoothstep_monotonic,
test_smoothstep_slope);
}
