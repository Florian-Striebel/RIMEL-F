#include <sys/types.h>
#include <sys/bitmap.h>
#include <assert.h>
#include <strings.h>
#include <stdlib.h>
#include <dt_regset.h>
#include <dt_impl.h>
dt_regset_t *
dt_regset_create(ulong_t nregs)
{
ulong_t n = BT_BITOUL(nregs);
dt_regset_t *drp = malloc(sizeof (dt_regset_t));
if (drp == NULL)
return (NULL);
drp->dr_bitmap = calloc(n, sizeof (ulong_t));
if (drp->dr_bitmap == NULL) {
dt_regset_destroy(drp);
return (NULL);
}
drp->dr_size = nregs;
return (drp);
}
void
dt_regset_destroy(dt_regset_t *drp)
{
free(drp->dr_bitmap);
free(drp);
}
void
dt_regset_reset(dt_regset_t *drp)
{
bzero(drp->dr_bitmap, sizeof (ulong_t) * BT_BITOUL(drp->dr_size));
}
void
dt_regset_assert_free(dt_regset_t *drp)
{
int reg;
boolean_t fail = B_FALSE;
for (reg = 0; reg < drp->dr_size; reg++) {
if (BT_TEST(drp->dr_bitmap, reg) != 0) {
dt_dprintf("%%r%d was left allocated\n", reg);
fail = B_TRUE;
}
}
if (fail && getenv("DTRACE_DEBUG_REGSET") != NULL)
abort();
}
int
dt_regset_alloc(dt_regset_t *drp)
{
ulong_t nbits = drp->dr_size - 1;
ulong_t maxw = nbits >> BT_ULSHIFT;
ulong_t wx;
for (wx = 0; wx <= maxw; wx++) {
if (drp->dr_bitmap[wx] != ~0UL)
break;
}
if (wx <= maxw) {
ulong_t maxb = (wx == maxw) ? nbits & BT_ULMASK : BT_NBIPUL - 1;
ulong_t word = drp->dr_bitmap[wx];
ulong_t bit, bx;
int reg;
for (bit = 1, bx = 0; bx <= maxb; bx++, bit <<= 1) {
if ((word & bit) == 0) {
reg = (int)((wx << BT_ULSHIFT) | bx);
BT_SET(drp->dr_bitmap, reg);
return (reg);
}
}
}
xyerror(D_NOREG, "Insufficient registers to generate code");
return (-1);
}
void
dt_regset_free(dt_regset_t *drp, int reg)
{
assert(reg >= 0 && reg < drp->dr_size);
assert(BT_TEST(drp->dr_bitmap, reg) != 0);
BT_CLEAR(drp->dr_bitmap, reg);
}
