











#include "int_lib.h"





COMPILER_RT_ABI di_int __subvdi3(di_int a, di_int b) {
di_int s = (du_int)a - (du_int)b;
if (b >= 0) {
if (s > a)
compilerrt_abort();
} else {
if (s <= a)
compilerrt_abort();
}
return s;
}
