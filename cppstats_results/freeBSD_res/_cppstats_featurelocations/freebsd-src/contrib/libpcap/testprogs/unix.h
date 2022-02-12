
































#if !defined(unix_h)
#define unix_h






#if defined(_MSC_VER)
#define stat _stat
#define fstat _fstat

#define open _open
#define O_RDONLY _O_RDONLY
#define O_WRONLY _O_WRONLY
#define O_RDWR _O_RDWR
#define O_BINARY _O_BINARY
#define O_CREAT _O_CREAT
#define O_TRUNC _O_TRUNC
#define read _read
#define write _write
#define close _close
#endif

#endif
