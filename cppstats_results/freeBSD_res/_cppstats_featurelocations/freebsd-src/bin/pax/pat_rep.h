








































typedef struct replace {
char *nstr;
#if defined(NET2_REGEX)
regexp *rcmp;
#else
regex_t rcmp;
#endif
int flgs;
#define PRNT 0x1
#define GLOB 0x2
struct replace *fow;
} REPLACE;
