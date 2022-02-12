


































#if !defined(BC_FILE_H)
#define BC_FILE_H

#include <stdarg.h>

#include <vector.h>

#define BC_FILE_ULL_LENGTH (21)


typedef struct BcFile {


int fd;


char *buf;


size_t len;


size_t cap;

} BcFile;

#if BC_ENABLE_HISTORY




typedef enum BcFlushType {


BC_FLUSH_NO_EXTRAS_NO_CLEAR,


BC_FLUSH_SAVE_EXTRAS_NO_CLEAR,


BC_FLUSH_NO_EXTRAS_CLEAR,


BC_FLUSH_SAVE_EXTRAS_CLEAR,

} BcFlushType;

#else




#define bc_file_putchar(f, t, c) bc_file_putchar(f, c)
#define bc_file_flushErr(f, t) bc_file_flushErr(f)
#define bc_file_flush(f, t) bc_file_flush(f)
#define bc_file_write(f, t, b, n) bc_file_write(f, b, n)
#define bc_file_puts(f, t, s) bc_file_puts(f, s)

#endif








void bc_file_init(BcFile *f, int fd, char *buf, size_t cap);





void bc_file_free(BcFile *f);







void bc_file_putchar(BcFile *restrict f, BcFlushType type, uchar c);









BcStatus bc_file_flushErr(BcFile *restrict f, BcFlushType type);






void bc_file_flush(BcFile *restrict f, BcFlushType type);








void bc_file_write(BcFile *restrict f, BcFlushType type,
const char *buf, size_t n);






void bc_file_printf(BcFile *restrict f, const char *fmt, ...);






void bc_file_vprintf(BcFile *restrict f, const char *fmt, va_list args);







void bc_file_puts(BcFile *restrict f, BcFlushType type, const char *str);

#if BC_ENABLE_HISTORY


extern const BcFlushType bc_flush_none;
extern const BcFlushType bc_flush_err;
extern const BcFlushType bc_flush_save;

#endif

#endif
