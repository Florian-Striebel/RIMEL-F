































#include "shell.h"

#define DIGITS(var) (3 + (2 + CHAR_BIT * sizeof((var))) / 3)

arith_t arith(const char *);
