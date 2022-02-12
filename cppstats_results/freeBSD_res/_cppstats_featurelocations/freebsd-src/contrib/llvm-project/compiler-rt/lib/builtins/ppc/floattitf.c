












#include <stdint.h>


long double __floatditf(int64_t);
long double __floatunditf(uint64_t);











long double __floattitf(__int128_t arg) {

int64_t ArgHiPart = (int64_t)(arg >> 64);
uint64_t ArgLoPart = (uint64_t)arg;




long double ConvertedHiPart = __floatditf(ArgHiPart);
long double ConvertedLoPart = __floatunditf(ArgLoPart);





return ((ConvertedHiPart * 0x1.0p64) + ConvertedLoPart);
}
