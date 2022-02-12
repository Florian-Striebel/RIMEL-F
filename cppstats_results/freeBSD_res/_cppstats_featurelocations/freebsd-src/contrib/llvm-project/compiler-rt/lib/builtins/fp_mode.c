













#include "fp_mode.h"


CRT_FE_ROUND_MODE __fe_getround() { return CRT_FE_TONEAREST; }

int __fe_raise_inexact() {
return 0;
}
