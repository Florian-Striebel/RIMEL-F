FPI *fpi, fpi1;
int Rounding;
#if defined(Trust_FLT_ROUNDS)
Rounding = Flt_Rounds;
#else
Rounding = 1;
switch(fegetround()) {
case FE_TOWARDZERO: Rounding = 0; break;
case FE_UPWARD: Rounding = 2; break;
case FE_DOWNWARD: Rounding = 3;
}
#endif
fpi = &fpi0;
if (Rounding != 1) {
fpi1 = fpi0;
fpi = &fpi1;
fpi1.rounding = Rounding;
}
