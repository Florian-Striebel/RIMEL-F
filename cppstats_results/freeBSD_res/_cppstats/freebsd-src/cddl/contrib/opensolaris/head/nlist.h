#if defined(__cplusplus)
extern "C" {
#endif
struct nlist {
char *n_name;
long n_value;
short n_scnum;
unsigned short n_type;
char n_sclass;
char n_numaux;
};
extern int nlist(const char *, struct nlist *);
#if defined(__cplusplus)
}
#endif
