














#if !defined(FP_MODE)
#define FP_MODE

typedef enum {
CRT_FE_TONEAREST,
CRT_FE_DOWNWARD,
CRT_FE_UPWARD,
CRT_FE_TOWARDZERO
} CRT_FE_ROUND_MODE;

CRT_FE_ROUND_MODE __fe_getround(void);
int __fe_raise_inexact(void);

#endif
