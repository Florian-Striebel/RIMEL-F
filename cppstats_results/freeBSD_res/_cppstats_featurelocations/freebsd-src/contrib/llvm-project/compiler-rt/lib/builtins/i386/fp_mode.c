







#include "../fp_mode.h"

#define X87_TONEAREST 0x0000
#define X87_DOWNWARD 0x0400
#define X87_UPWARD 0x0800
#define X87_TOWARDZERO 0x0c00
#define X87_RMODE_MASK (X87_TONEAREST | X87_UPWARD | X87_DOWNWARD | X87_TOWARDZERO)

CRT_FE_ROUND_MODE __fe_getround() {

unsigned short cw;
__asm__ __volatile__ ("fnstcw %0" : "=m" (cw));

switch (cw & X87_RMODE_MASK) {
case X87_TONEAREST:
return CRT_FE_TONEAREST;
case X87_DOWNWARD:
return CRT_FE_DOWNWARD;
case X87_UPWARD:
return CRT_FE_UPWARD;
case X87_TOWARDZERO:
return CRT_FE_TOWARDZERO;
}
return CRT_FE_TONEAREST;
}

int __fe_raise_inexact() {
float f = 1.0f, g = 3.0f;
__asm__ __volatile__ ("fdivs %1" : "+t" (f) : "m" (g));
return 0;
}
