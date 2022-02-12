











#if !defined(IRIX) && !defined(__digital__) && !defined(__osf__)
#if !defined(_ranlib_h)
#define _ranlib_h

#if 0
#define RANLIBMAG "!<arch>\n__.SYMDEF"
#endif
#define RANLIBMAG "__.SYMDEF"
#define RANLIBSKEW 3

struct ranlib {
union {
off_t ran_strx;
char *ran_name;
} ran_un;
off_t ran_off;
};

#endif
#endif
