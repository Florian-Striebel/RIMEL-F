





















int bcmp(const char *, const char *, u_int);
void bcopy(const void *, void *, u_int);
void bzero(void *, u_int);
int getopt(int, char * const *, const char *);
#if defined(__STDC__)
struct timeval;
struct timezone;
#endif
int gettimeofday(struct timeval *, struct timezone *);
int ioctl(int, int, caddr_t);
int pfopen(char *, int);
int setlinebuf(FILE *);
int socket(int, int, int);
int strcasecmp(const char *, const char *);
