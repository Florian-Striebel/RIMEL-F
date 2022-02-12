























#define BR_POWER_ASM_MACROS 1
#include "inner.h"





#if BR_POWER8














#define HB0 0
#define HB1 1
#define HB2 2
#define HB6 3
#define HB7 4
#define TT0 5
#define TT1 6
#define TT2 7

#define BSW 8
#define XBSW 40




#define INIT vxor(HB0, HB0, HB0) vspltisb(HB1, 1) vspltisb(HB2, 2) vspltisb(HB6, 6) vspltisb(HB7, 7) INIT_BSW











#if BR_POWER8_LE
#define INIT_BSW lxvw4x(XBSW, 0, %[idx2be])
#define FIX_ENDIAN(xx) vperm(xx, xx, xx, BSW)
#else
#define INIT_BSW
#define FIX_ENDIAN(xx)
#endif







#define SL_256(x0, x1) vsldoi(TT0, HB0, x1, 1) vsl(x0, x0, HB1) vsr(TT0, TT0, HB7) vsl(x1, x1, HB1) vxor(x0, x0, TT0)










#define REDUCE_F128(xd, x0, x1) vxor(x0, x0, x1) vsr(TT0, x1, HB1) vsr(TT1, x1, HB2) vsr(TT2, x1, HB7) vxor(x0, x0, TT0) vxor(TT1, TT1, TT2) vxor(x0, x0, TT1) vsldoi(x1, x1, HB0, 15) vsl(TT1, x1, HB6) vsl(TT2, x1, HB1) vxor(x1, TT1, TT2) vsr(TT0, x1, HB1) vsr(TT1, x1, HB2) vsr(TT2, x1, HB7) vxor(x0, x0, x1) vxor(x0, x0, TT0) vxor(TT1, TT1, TT2) vxor(xd, x0, TT1)




















void
br_ghash_pwr8(void *y, const void *h, const void *data, size_t len)
{
const unsigned char *buf1, *buf2;
size_t num4, num1;
unsigned char tmp[64];
long cc0, cc1, cc2, cc3;

#if BR_POWER8_LE
static const uint32_t idx2be[] = {
0x03020100, 0x07060504, 0x0B0A0908, 0x0F0E0D0C
};
#endif

buf1 = data;













num4 = len >> 6;
buf2 = buf1 + (num4 << 6);
len &= 63;
num1 = (len + 15) >> 4;
if ((len & 15) != 0) {
memcpy(tmp, buf2, len);
memset(tmp + len, 0, (num1 << 4) - len);
buf2 = tmp;
}

cc0 = 0;
cc1 = 16;
cc2 = 32;
cc3 = 48;
asm volatile (
INIT




lxvw4x(41, 0, %[h])
FIX_ENDIAN(9)




lxvw4x(60, 0, %[y])
FIX_ENDIAN(28)







xxpermdi(49, 41, 41, 2)
vsldoi(18, HB0, 9, 8)
vsldoi(19, 9, HB0, 8)




cmpldi(%[num4], 0)
beq(chunk1)




vpmsumd(10, 18, 18)
vpmsumd(11, 19, 19)
SL_256(10, 11)
REDUCE_F128(10, 10, 11)









vsldoi(11, HB0, 10, 8)
vsldoi(12, 10, HB0, 8)
vpmsumd(13, 10, 17)
vpmsumd(11, 11, 18)
vpmsumd(12, 12, 19)
vsldoi(14, HB0, 13, 8)
vsldoi(15, 13, HB0, 8)
vxor(11, 11, 14)
vxor(12, 12, 15)
SL_256(11, 12)
REDUCE_F128(11, 11, 12)




vsldoi(12, HB0, 10, 8)
vsldoi(13, 10, HB0, 8)
vpmsumd(12, 12, 12)
vpmsumd(13, 13, 13)
SL_256(12, 13)
REDUCE_F128(12, 12, 13)








xxpermdi(45, 44, 43, 0)
xxpermdi(46, 44, 43, 3)
xxpermdi(47, 42, 41, 0)
xxpermdi(48, 42, 41, 3)




mtctr(%[num4])
label(loop4)







lxvw4x(52, %[cc0], %[buf1])
lxvw4x(53, %[cc1], %[buf1])
lxvw4x(54, %[cc2], %[buf1])
lxvw4x(55, %[cc3], %[buf1])
FIX_ENDIAN(20)
FIX_ENDIAN(21)
FIX_ENDIAN(22)
FIX_ENDIAN(23)
addi(%[buf1], %[buf1], 64)
vxor(20, 20, 28)








xxpermdi(41, 52, 53, 0)
xxpermdi(42, 52, 53, 3)
xxpermdi(43, 54, 55, 0)
xxpermdi(44, 54, 55, 3)












vpmsumd(20, 13, 9)
vpmsumd(21, 13, 10)
vpmsumd(22, 14, 9)
vpmsumd(23, 14, 10)
vpmsumd(24, 15, 11)
vpmsumd(25, 15, 12)
vpmsumd(26, 16, 11)
vpmsumd(27, 16, 12)




vxor(11, 20, 24)
vxor(12, 23, 27)
vxor( 9, 21, 22)
vxor(10, 25, 26)
vxor(20, 9, 10)
vsldoi( 9, HB0, 20, 8)
vsldoi(10, 20, HB0, 8)
vxor(11, 11, 9)
vxor(12, 12, 10)




SL_256(11, 12)
REDUCE_F128(28, 11, 12)




bdnz(loop4)




label(chunk1)
cmpldi(%[num1], 0)
beq(done)

mtctr(%[num1])
label(loop1)



lxvw4x(41, 0, %[buf2])
#if BR_POWER8_LE
FIX_ENDIAN(9)
#endif
addi(%[buf2], %[buf2], 16)
vxor(9, 28, 9)







vsldoi(10, HB0, 9, 8)
vsldoi(11, 9, HB0, 8)







vpmsumd(14, 9, 17)
vpmsumd(12, 10, 18)
vpmsumd(13, 11, 19)




vsldoi(10, HB0, 14, 8)
vsldoi(11, 14, HB0, 8)
vxor(12, 12, 10)
vxor(13, 13, 11)




SL_256(12, 13)
REDUCE_F128(28, 12, 13)
bdnz(loop1)

label(done)



FIX_ENDIAN(28)
stxvw4x(60, 0, %[y])

: [buf1] "+b" (buf1), [buf2] "+b" (buf2)
: [y] "b" (y), [h] "b" (h), [num4] "b" (num4), [num1] "b" (num1),
[cc0] "b" (cc0), [cc1] "b" (cc1), [cc2] "b" (cc2), [cc3] "b" (cc3)
#if BR_POWER8_LE
, [idx2be] "b" (idx2be)
#endif
: "v0", "v1", "v2", "v3", "v4", "v5", "v6", "v7", "v8", "v9",
"v10", "v11", "v12", "v13", "v14", "v15", "v16", "v17", "v18", "v19",
"v20", "v21", "v22", "v23", "v24", "v25", "v26", "v27", "v28", "v29",
"ctr", "memory"
);
}


br_ghash
br_ghash_pwr8_get(void)
{
return &br_ghash_pwr8;
}

#else


br_ghash
br_ghash_pwr8_get(void)
{
return 0;
}

#endif
