#include_next <sha256.h>
#define SHA256Init(c) SHA256_Init(c)
#define SHA256Update(c, d, s) SHA256_Update((c), (d), (s))
#define SHA256Final(b, c) SHA256_Final((unsigned char *)(b), (c))
