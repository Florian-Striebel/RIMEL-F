































#if !defined(__file_h__)
#define __file_h__

#if defined(HAVE_CONFIG_H)
#include <config.h>
#endif

#if defined(HAVE_STDINT_H)
#include <stdint.h>
#endif

#if defined(HAVE_INTTYPES_H)
#include <inttypes.h>
#endif

#if !defined(__STDC_LIMIT_MACROS)
#define __STDC_LIMIT_MACROS
#endif
#if !defined(__STDC_FORMAT_MACROS)
#define __STDC_FORMAT_MACROS
#endif

#if defined(_WIN32)
#if defined(PRIu32)
#if defined(_WIN64)
#define SIZE_T_FORMAT PRIu64
#else
#define SIZE_T_FORMAT PRIu32
#endif
#define INT64_T_FORMAT PRIi64
#define INTMAX_T_FORMAT PRIiMAX
#else
#if defined(_WIN64)
#define SIZE_T_FORMAT "I64"
#else
#define SIZE_T_FORMAT ""
#endif
#define INT64_T_FORMAT "I64"
#define INTMAX_T_FORMAT "I64"
#endif
#else
#define SIZE_T_FORMAT "z"
#define INT64_T_FORMAT "ll"
#define INTMAX_T_FORMAT "j"
#endif

#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <regex.h>
#include <time.h>
#include <sys/types.h>
#if !defined(WIN32)
#include <sys/param.h>
#endif

#include <sys/stat.h>
#include <stdarg.h>

#define ENABLE_CONDITIONALS

#if !defined(MAGIC)
#define MAGIC "/etc/magic"
#endif

#if defined(__EMX__) || defined (WIN32)
#define PATHSEP ';'
#else
#define PATHSEP ':'
#endif

#define private static

#if HAVE_VISIBILITY && !defined(WIN32)
#define public __attribute__ ((__visibility__("default")))
#if !defined(protected)
#define protected __attribute__ ((__visibility__("hidden")))
#endif
#else
#define public
#if !defined(protected)
#define protected
#endif
#endif

#if !defined(__arraycount)
#define __arraycount(a) (sizeof(a) / sizeof(a[0]))
#endif

#if !defined(__GNUC_PREREQ__)
#if defined(__GNUC__)
#define __GNUC_PREREQ__(x, y) ((__GNUC__ == (x) && __GNUC_MINOR__ >= (y)) || (__GNUC__ > (x)))


#else
#define __GNUC_PREREQ__(x, y) 0
#endif
#endif

#if !defined(__GNUC__)
#if !defined(__attribute__)
#define __attribute__(a)
#endif
#endif

#if !defined(MIN)
#define MIN(a,b) (((a) < (b)) ? (a) : (b))
#endif

#if !defined(MAX)
#define MAX(a,b) (((a) > (b)) ? (a) : (b))
#endif

#if !defined(O_CLOEXEC)
#define O_CLOEXEC 0
#endif

#if !defined(FD_CLOEXEC)
#define FD_CLOEXEC 1
#endif

#define FILE_BADSIZE CAST(size_t, ~0ul)
#define MAXDESC 64
#define MAXMIME 80
#define MAXstring 128

#define MAGICNO 0xF11E041C
#define VERSIONNO 16
#define FILE_MAGICSIZE 376

#define FILE_GUID_SIZE sizeof("XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX")

#define FILE_LOAD 0
#define FILE_CHECK 1
#define FILE_COMPILE 2
#define FILE_LIST 3

struct buffer {
int fd;
struct stat st;
const void *fbuf;
size_t flen;
off_t eoff;
void *ebuf;
size_t elen;
};

union VALUETYPE {
uint8_t b;
uint16_t h;
uint32_t l;
uint64_t q;
uint8_t hs[2];
uint8_t hl[4];
uint8_t hq[8];
char s[MAXstring];
unsigned char us[MAXstring];
uint64_t guid[2];
float f;
double d;
};

struct magic {

uint16_t cont_level;
uint8_t flag;
#define INDIR 0x01
#define OFFADD 0x02
#define INDIROFFADD 0x04
#define UNSIGNED 0x08
#define NOSPACE 0x10
#define BINTEST 0x20

#define TEXTTEST 0x40
#define OFFNEGATIVE 0x80

uint8_t factor;


uint8_t reln;
uint8_t vallen;
uint8_t type;
uint8_t in_type;
#define FILE_INVALID 0
#define FILE_BYTE 1
#define FILE_SHORT 2
#define FILE_DEFAULT 3
#define FILE_LONG 4
#define FILE_STRING 5
#define FILE_DATE 6
#define FILE_BESHORT 7
#define FILE_BELONG 8
#define FILE_BEDATE 9
#define FILE_LESHORT 10
#define FILE_LELONG 11
#define FILE_LEDATE 12
#define FILE_PSTRING 13
#define FILE_LDATE 14
#define FILE_BELDATE 15
#define FILE_LELDATE 16
#define FILE_REGEX 17
#define FILE_BESTRING16 18
#define FILE_LESTRING16 19
#define FILE_SEARCH 20
#define FILE_MEDATE 21
#define FILE_MELDATE 22
#define FILE_MELONG 23
#define FILE_QUAD 24
#define FILE_LEQUAD 25
#define FILE_BEQUAD 26
#define FILE_QDATE 27
#define FILE_LEQDATE 28
#define FILE_BEQDATE 29
#define FILE_QLDATE 30
#define FILE_LEQLDATE 31
#define FILE_BEQLDATE 32
#define FILE_FLOAT 33
#define FILE_BEFLOAT 34
#define FILE_LEFLOAT 35
#define FILE_DOUBLE 36
#define FILE_BEDOUBLE 37
#define FILE_LEDOUBLE 38
#define FILE_BEID3 39
#define FILE_LEID3 40
#define FILE_INDIRECT 41
#define FILE_QWDATE 42
#define FILE_LEQWDATE 43
#define FILE_BEQWDATE 44
#define FILE_NAME 45
#define FILE_USE 46
#define FILE_CLEAR 47
#define FILE_DER 48
#define FILE_GUID 49
#define FILE_OFFSET 50
#define FILE_BEVARINT 51
#define FILE_LEVARINT 52
#define FILE_NAMES_SIZE 53

#define IS_STRING(t) ((t) == FILE_STRING || (t) == FILE_PSTRING || (t) == FILE_BESTRING16 || (t) == FILE_LESTRING16 || (t) == FILE_REGEX || (t) == FILE_SEARCH || (t) == FILE_INDIRECT || (t) == FILE_NAME || (t) == FILE_USE)










#define FILE_FMT_NONE 0
#define FILE_FMT_NUM 1
#define FILE_FMT_STR 2
#define FILE_FMT_QUAD 3
#define FILE_FMT_FLOAT 4
#define FILE_FMT_DOUBLE 5


uint8_t in_op;
uint8_t mask_op;
#if defined(ENABLE_CONDITIONALS)
uint8_t cond;
#else
uint8_t dummy;
#endif
uint8_t factor_op;
#define FILE_FACTOR_OP_PLUS '+'
#define FILE_FACTOR_OP_MINUS '-'
#define FILE_FACTOR_OP_TIMES '*'
#define FILE_FACTOR_OP_DIV '/'
#define FILE_FACTOR_OP_NONE '\0'

#define FILE_OPS "&|^+-*/%"
#define FILE_OPAND 0
#define FILE_OPOR 1
#define FILE_OPXOR 2
#define FILE_OPADD 3
#define FILE_OPMINUS 4
#define FILE_OPMULTIPLY 5
#define FILE_OPDIVIDE 6
#define FILE_OPMODULO 7
#define FILE_OPS_MASK 0x07
#define FILE_UNUSED_1 0x08
#define FILE_UNUSED_2 0x10
#define FILE_OPSIGNED 0x20
#define FILE_OPINVERSE 0x40
#define FILE_OPINDIRECT 0x80

#if defined(ENABLE_CONDITIONALS)
#define COND_NONE 0
#define COND_IF 1
#define COND_ELIF 2
#define COND_ELSE 3
#endif


int32_t offset;

int32_t in_offset;

uint32_t lineno;

union {
uint64_t _mask;
struct {
uint32_t _count;
uint32_t _flags;
} _s;
} _u;
#define num_mask _u._mask
#define str_range _u._s._count
#define str_flags _u._s._flags

union VALUETYPE value;

char desc[MAXDESC];

char mimetype[MAXMIME];

char apple[8];

char ext[64];
};

#define BIT(A) (1 << (A))
#define STRING_COMPACT_WHITESPACE BIT(0)
#define STRING_COMPACT_OPTIONAL_WHITESPACE BIT(1)
#define STRING_IGNORE_LOWERCASE BIT(2)
#define STRING_IGNORE_UPPERCASE BIT(3)
#define REGEX_OFFSET_START BIT(4)
#define STRING_TEXTTEST BIT(5)
#define STRING_BINTEST BIT(6)
#define PSTRING_1_BE BIT(7)
#define PSTRING_1_LE BIT(7)
#define PSTRING_2_BE BIT(8)
#define PSTRING_2_LE BIT(9)
#define PSTRING_4_BE BIT(10)
#define PSTRING_4_LE BIT(11)
#define REGEX_LINE_COUNT BIT(11)
#define PSTRING_LEN (PSTRING_1_BE|PSTRING_2_LE|PSTRING_2_BE|PSTRING_4_LE|PSTRING_4_BE)

#define PSTRING_LENGTH_INCLUDES_ITSELF BIT(12)
#define STRING_TRIM BIT(13)
#define STRING_FULL_WORD BIT(14)
#define CHAR_COMPACT_WHITESPACE 'W'
#define CHAR_COMPACT_OPTIONAL_WHITESPACE 'w'
#define CHAR_IGNORE_LOWERCASE 'c'
#define CHAR_IGNORE_UPPERCASE 'C'
#define CHAR_REGEX_OFFSET_START 's'
#define CHAR_TEXTTEST 't'
#define CHAR_TRIM 'T'
#define CHAR_FULL_WORD 'f'
#define CHAR_BINTEST 'b'
#define CHAR_PSTRING_1_BE 'B'
#define CHAR_PSTRING_1_LE 'B'
#define CHAR_PSTRING_2_BE 'H'
#define CHAR_PSTRING_2_LE 'h'
#define CHAR_PSTRING_4_BE 'L'
#define CHAR_PSTRING_4_LE 'l'
#define CHAR_PSTRING_LENGTH_INCLUDES_ITSELF 'J'
#define STRING_IGNORE_CASE (STRING_IGNORE_LOWERCASE|STRING_IGNORE_UPPERCASE)
#define STRING_DEFAULT_RANGE 100

#define INDIRECT_RELATIVE BIT(0)
#define CHAR_INDIRECT_RELATIVE 'r'


struct mlist {
struct magic *magic;
uint32_t nmagic;
void *map;
struct mlist *next, *prev;
};

#if defined(__cplusplus)
#define CAST(T, b) static_cast<T>(b)
#define RCAST(T, b) reinterpret_cast<T>(b)
#define CCAST(T, b) const_cast<T>(b)
#else
#define CAST(T, b) ((T)(b))
#define RCAST(T, b) ((T)(uintptr_t)(b))
#define CCAST(T, b) ((T)(uintptr_t)(b))
#endif

struct level_info {
int32_t off;
int got_match;
#if defined(ENABLE_CONDITIONALS)
int last_match;
int last_cond;
#endif
};

struct cont {
size_t len;
struct level_info *li;
};

#define MAGIC_SETS 2

struct magic_set {
struct mlist *mlist[MAGIC_SETS];
struct cont c;
struct out {
char *buf;
size_t blen;
char *pbuf;
} o;
uint32_t offset;

uint32_t eoffset;
int error;
int flags;
int event_flags;
#define EVENT_HAD_ERR 0x01
const char *file;
size_t line;
mode_t mode;


struct {
const char *s;
size_t s_len;
size_t offset;
size_t rm_len;
} search;



union VALUETYPE ms_value;
uint16_t indir_max;
uint16_t name_max;
uint16_t elf_shnum_max;
uint16_t elf_phnum_max;
uint16_t elf_notes_max;
uint16_t regex_max;
size_t bytes_max;
size_t encoding_max;
#if !defined(FILE_BYTES_MAX)
#define FILE_BYTES_MAX (1024 * 1024)
#endif
#define FILE_ELF_NOTES_MAX 256
#define FILE_ELF_PHNUM_MAX 2048
#define FILE_ELF_SHNUM_MAX 32768
#define FILE_INDIR_MAX 50
#define FILE_NAME_MAX 50
#define FILE_REGEX_MAX 8192
#define FILE_ENCODING_MAX (64 * 1024)
};


typedef unsigned long file_unichar_t;

struct stat;
#define FILE_T_LOCAL 1
#define FILE_T_WINDOWS 2
protected const char *file_fmttime(char *, size_t, uint64_t, int);
protected const char *file_fmtvarint(const unsigned char *, int, char *,
size_t);
protected struct magic_set *file_ms_alloc(int);
protected void file_ms_free(struct magic_set *);
protected int file_default(struct magic_set *, size_t);
protected int file_buffer(struct magic_set *, int, struct stat *, const char *,
const void *, size_t);
protected int file_fsmagic(struct magic_set *, const char *, struct stat *);
protected int file_pipe2file(struct magic_set *, int, const void *, size_t);
protected int file_vprintf(struct magic_set *, const char *, va_list)
__attribute__((__format__(__printf__, 2, 0)));
protected int file_separator(struct magic_set *);
protected char *file_copystr(char *, size_t, size_t, const char *);
protected int file_checkfmt(char *, size_t, const char *);
protected size_t file_printedlen(const struct magic_set *);
protected int file_print_guid(char *, size_t, const uint64_t *);
protected int file_parse_guid(const char *, uint64_t *);
protected int file_replace(struct magic_set *, const char *, const char *);
protected int file_printf(struct magic_set *, const char *, ...)
__attribute__((__format__(__printf__, 2, 3)));
protected int file_reset(struct magic_set *, int);
protected int file_tryelf(struct magic_set *, const struct buffer *);
protected int file_trycdf(struct magic_set *, const struct buffer *);
#if HAVE_FORK
protected int file_zmagic(struct magic_set *, const struct buffer *,
const char *);
#endif
protected int file_ascmagic(struct magic_set *, const struct buffer *,
int);
protected int file_ascmagic_with_encoding(struct magic_set *,
const struct buffer *, file_unichar_t *, size_t, const char *, const char *, int);
protected int file_encoding(struct magic_set *, const struct buffer *,
file_unichar_t **, size_t *, const char **, const char **, const char **);
protected int file_is_json(struct magic_set *, const struct buffer *);
protected int file_is_csv(struct magic_set *, const struct buffer *, int);
protected int file_is_tar(struct magic_set *, const struct buffer *);
protected int file_softmagic(struct magic_set *, const struct buffer *,
uint16_t *, uint16_t *, int, int);
protected int file_apprentice(struct magic_set *, const char *, int);
protected int buffer_apprentice(struct magic_set *, struct magic **,
size_t *, size_t);
protected int file_magicfind(struct magic_set *, const char *, struct mlist *);
protected uint64_t file_signextend(struct magic_set *, struct magic *,
uint64_t);
protected uintmax_t file_varint2uintmax_t(const unsigned char *, int, size_t *);

protected void file_badread(struct magic_set *);
protected void file_badseek(struct magic_set *);
protected void file_oomem(struct magic_set *, size_t);
protected void file_error(struct magic_set *, int, const char *, ...)
__attribute__((__format__(__printf__, 3, 4)));
protected void file_magerror(struct magic_set *, const char *, ...)
__attribute__((__format__(__printf__, 2, 3)));
protected void file_magwarn(struct magic_set *, const char *, ...)
__attribute__((__format__(__printf__, 2, 3)));
protected void file_mdump(struct magic *);
protected void file_showstr(FILE *, const char *, size_t);
protected size_t file_mbswidth(const char *);
protected const char *file_getbuffer(struct magic_set *);
protected ssize_t sread(int, void *, size_t, int);
protected int file_check_mem(struct magic_set *, unsigned int);
protected int file_looks_utf8(const unsigned char *, size_t, file_unichar_t *,
size_t *);
protected size_t file_pstring_length_size(struct magic_set *,
const struct magic *);
protected size_t file_pstring_get_length(struct magic_set *,
const struct magic *, const char *);
protected char * file_printable(struct magic_set *, char *, size_t,
const char *, size_t);
#if defined(__EMX__)
protected int file_os2_apptype(struct magic_set *, const char *, const void *,
size_t);
#endif
protected int file_pipe_closexec(int *);
protected int file_clear_closexec(int);
protected char *file_strtrim(char *);

protected void buffer_init(struct buffer *, int, const struct stat *,
const void *, size_t);
protected void buffer_fini(struct buffer *);
protected int buffer_fill(const struct buffer *);

#include <locale.h>
#if defined(HAVE_XLOCALE_H)
#include <xlocale.h>
#endif

typedef struct {
const char *pat;
#if defined(HAVE_NEWLOCALE) && defined(HAVE_USELOCALE) && defined(HAVE_FREELOCALE)
#define USE_C_LOCALE
locale_t old_lc_ctype;
locale_t c_lc_ctype;
#else
char *old_lc_ctype;
#endif
int rc;
regex_t rx;
} file_regex_t;

protected int file_regcomp(file_regex_t *, const char *, int);
protected int file_regexec(file_regex_t *, const char *, size_t, regmatch_t *,
int);
protected void file_regfree(file_regex_t *);
protected void file_regerror(file_regex_t *, int, struct magic_set *);

typedef struct {
char *buf;
size_t blen;
uint32_t offset;
} file_pushbuf_t;

protected file_pushbuf_t *file_push_buffer(struct magic_set *);
protected char *file_pop_buffer(struct magic_set *, file_pushbuf_t *);

#if !defined(COMPILE_ONLY)
extern const char *file_names[];
extern const size_t file_nnames;
#endif

#if !defined(HAVE_PREAD)
ssize_t pread(int, void *, size_t, off_t);
#endif
#if !defined(HAVE_VASPRINTF)
int vasprintf(char **, const char *, va_list);
#endif
#if !defined(HAVE_ASPRINTF)
int asprintf(char **, const char *, ...);
#endif
#if !defined(HAVE_DPRINTF)
int dprintf(int, const char *, ...);
#endif

#if !defined(HAVE_STRLCPY)
size_t strlcpy(char *, const char *, size_t);
#endif
#if !defined(HAVE_STRLCAT)
size_t strlcat(char *, const char *, size_t);
#endif
#if !defined(HAVE_STRCASESTR)
char *strcasestr(const char *, const char *);
#endif
#if !defined(HAVE_GETLINE)
ssize_t getline(char **, size_t *, FILE *);
ssize_t getdelim(char **, size_t *, int, FILE *);
#endif
#if !defined(HAVE_CTIME_R)
char *ctime_r(const time_t *, char *);
#endif
#if !defined(HAVE_ASCTIME_R)
char *asctime_r(const struct tm *, char *);
#endif
#if !defined(HAVE_GMTIME_R)
struct tm *gmtime_r(const time_t *, struct tm *);
#endif
#if !defined(HAVE_LOCALTIME_R)
struct tm *localtime_r(const time_t *, struct tm *);
#endif
#if !defined(HAVE_FMTCHECK)
const char *fmtcheck(const char *, const char *)
__attribute__((__format_arg__(2)));
#endif

#if defined(HAVE_LIBSECCOMP)



int enable_sandbox_basic(void);




int enable_sandbox_full(void);
#endif

protected const char *file_getprogname(void);
protected void file_setprogname(const char *);
protected void file_err(int, const char *, ...)
__attribute__((__format__(__printf__, 2, 3), __noreturn__));
protected void file_errx(int, const char *, ...)
__attribute__((__format__(__printf__, 2, 3), __noreturn__));
protected void file_warn(const char *, ...)
__attribute__((__format__(__printf__, 1, 2)));
protected void file_warnx(const char *, ...)
__attribute__((__format__(__printf__, 1, 2)));

#if defined(HAVE_MMAP) && defined(HAVE_SYS_MMAN_H) && !defined(QUICK)
#define QUICK
#endif

#if !defined(O_BINARY)
#define O_BINARY 0
#endif
#if !defined(O_NONBLOCK)
#define O_NONBLOCK 0
#endif

#if !defined(__cplusplus)
#if defined(__GNUC__) && (__GNUC__ >= 3)
#define FILE_RCSID(id) static const char rcsid[] __attribute__((__used__)) = id;

#else
#define FILE_RCSID(id) static const char *rcsid(const char *p) { return rcsid(p = id); }



#endif
#else
#define FILE_RCSID(id)
#endif
#if !defined(__RCSID)
#define __RCSID(a)
#endif

#endif
