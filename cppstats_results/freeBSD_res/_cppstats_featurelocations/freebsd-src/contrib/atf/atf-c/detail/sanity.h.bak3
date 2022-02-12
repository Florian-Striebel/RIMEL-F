
























#if !defined(ATF_C_DETAIL_SANITY_H)
#define ATF_C_DETAIL_SANITY_H

void atf_sanity_inv(const char *, int, const char *);
void atf_sanity_pre(const char *, int, const char *);
void atf_sanity_post(const char *, int, const char *);

#if !defined(NDEBUG)

#define INV(x) do { if (!(x)) atf_sanity_inv(__FILE__, __LINE__, #x); } while (0)




#define PRE(x) do { if (!(x)) atf_sanity_pre(__FILE__, __LINE__, #x); } while (0)




#define POST(x) do { if (!(x)) atf_sanity_post(__FILE__, __LINE__, #x); } while (0)





#else

#define INV(x) do { } while (0)



#define PRE(x) do { } while (0)



#define POST(x) do { } while (0)



#endif

#define UNREACHABLE INV(0)

#endif
