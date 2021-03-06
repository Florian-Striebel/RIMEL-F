


































#include <assert.h>
#include <errno.h>
#include <string.h>

#if !defined(_WIN32)
#include <unistd.h>
#endif

#include <file.h>
#include <vm.h>






static void bc_file_ultoa(unsigned long long val, char buf[BC_FILE_ULL_LENGTH])
{
char buf2[BC_FILE_ULL_LENGTH];
size_t i, len;


memset(buf2, 0, BC_FILE_ULL_LENGTH);


for (i = 1; val; ++i) {

unsigned long long mod = val % 10;

buf2[i] = ((char) mod) + '0';
val /= 10;
}

len = i;


for (i = 0; i < len; ++i) buf[i] = buf2[len - i - 1];
}









static BcStatus bc_file_output(int fd, const char *buf, size_t n) {

size_t bytes = 0;
sig_atomic_t lock;

BC_SIG_TRYLOCK(lock);


while (bytes < n) {


ssize_t written = write(fd, buf + bytes, n - bytes);


if (BC_ERR(written == -1)) {

BC_SIG_TRYUNLOCK(lock);

return errno == EPIPE ? BC_STATUS_EOF : BC_STATUS_ERROR_FATAL;
}

bytes += (size_t) written;
}

BC_SIG_TRYUNLOCK(lock);

return BC_STATUS_SUCCESS;
}

BcStatus bc_file_flushErr(BcFile *restrict f, BcFlushType type)
{
BcStatus s;

BC_SIG_ASSERT_LOCKED;


if (f->len) {

#if BC_ENABLE_HISTORY


if (BC_TTY) {



if (f->buf[f->len - 1] != '\n' &&
(type == BC_FLUSH_SAVE_EXTRAS_CLEAR ||
type == BC_FLUSH_SAVE_EXTRAS_NO_CLEAR))
{
size_t i;


for (i = f->len - 2; i < f->len && f->buf[i] != '\n'; --i);

i += 1;


bc_vec_string(&vm.history.extras, f->len - i, f->buf + i);
}

else if (type >= BC_FLUSH_NO_EXTRAS_CLEAR) {
bc_vec_popAll(&vm.history.extras);
}
}
#endif


s = bc_file_output(f->fd, f->buf, f->len);
f->len = 0;
}
else s = BC_STATUS_SUCCESS;

return s;
}

void bc_file_flush(BcFile *restrict f, BcFlushType type) {

BcStatus s;
sig_atomic_t lock;

BC_SIG_TRYLOCK(lock);

s = bc_file_flushErr(f, type);


if (BC_ERR(s)) {


if (s == BC_STATUS_EOF) {
vm.status = (sig_atomic_t) s;
BC_SIG_TRYUNLOCK(lock);
BC_JMP;
}

else bc_vm_fatalError(BC_ERR_FATAL_IO_ERR);
}

BC_SIG_TRYUNLOCK(lock);
}

void bc_file_write(BcFile *restrict f, BcFlushType type,
const char *buf, size_t n)
{
sig_atomic_t lock;

BC_SIG_TRYLOCK(lock);


if (n > f->cap - f->len) {
bc_file_flush(f, type);
assert(!f->len);
}



if (BC_UNLIKELY(n > f->cap - f->len)) bc_file_output(f->fd, buf, n);
else {
memcpy(f->buf + f->len, buf, n);
f->len += n;
}

BC_SIG_TRYUNLOCK(lock);
}

void bc_file_printf(BcFile *restrict f, const char *fmt, ...)
{
va_list args;
sig_atomic_t lock;

BC_SIG_TRYLOCK(lock);

va_start(args, fmt);
bc_file_vprintf(f, fmt, args);
va_end(args);

BC_SIG_TRYUNLOCK(lock);
}

void bc_file_vprintf(BcFile *restrict f, const char *fmt, va_list args) {

char *percent;
const char *ptr = fmt;
char buf[BC_FILE_ULL_LENGTH];

BC_SIG_ASSERT_LOCKED;







while ((percent = strchr(ptr, '%')) != NULL) {

char c;



if (percent != ptr) {
size_t len = (size_t) (percent - ptr);
bc_file_write(f, bc_flush_none, ptr, len);
}

c = percent[1];



if (c == 'c') {

uchar uc = (uchar) va_arg(args, int);

bc_file_putchar(f, bc_flush_none, uc);
}
else if (c == 's') {

char *s = va_arg(args, char*);

bc_file_puts(f, bc_flush_none, s);
}
#if BC_DEBUG_CODE

else if (c == 'd') {

int d = va_arg(args, int);


if (d < 0) {
bc_file_putchar(f, bc_flush_none, '-');
d = -d;
}


if (!d) bc_file_putchar(f, bc_flush_none, '0');
else {
bc_file_ultoa((unsigned long long) d, buf);
bc_file_puts(f, bc_flush_none, buf);
}
}
#endif
else {

unsigned long long ull;




assert((c == 'l' || c == 'z') && percent[2] == 'u');

if (c == 'z') ull = (unsigned long long) va_arg(args, size_t);
else ull = (unsigned long long) va_arg(args, unsigned long);


if (!ull) bc_file_putchar(f, bc_flush_none, '0');
else {
bc_file_ultoa(ull, buf);
bc_file_puts(f, bc_flush_none, buf);
}
}


ptr = percent + 2 + (c == 'l' || c == 'z');
}



if (ptr[0]) bc_file_puts(f, bc_flush_none, ptr);
}

void bc_file_puts(BcFile *restrict f, BcFlushType type, const char *str) {
bc_file_write(f, type, str, strlen(str));
}

void bc_file_putchar(BcFile *restrict f, BcFlushType type, uchar c) {

sig_atomic_t lock;

BC_SIG_TRYLOCK(lock);

if (f->len == f->cap) bc_file_flush(f, type);

assert(f->len < f->cap);

f->buf[f->len] = (char) c;
f->len += 1;

BC_SIG_TRYUNLOCK(lock);
}

void bc_file_init(BcFile *f, int fd, char *buf, size_t cap) {

BC_SIG_ASSERT_LOCKED;

f->fd = fd;
f->buf = buf;
f->len = 0;
f->cap = cap;
}

void bc_file_free(BcFile *f) {
BC_SIG_ASSERT_LOCKED;
bc_file_flush(f, bc_flush_none);
}
