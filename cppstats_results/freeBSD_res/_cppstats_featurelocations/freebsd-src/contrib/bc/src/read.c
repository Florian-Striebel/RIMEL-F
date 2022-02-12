


































#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include <signal.h>

#include <fcntl.h>
#include <sys/stat.h>

#if !defined(_WIN32)
#include <unistd.h>
#endif

#include <read.h>
#include <history.h>
#include <program.h>
#include <vm.h>






static int bc_read_open(const char* path, int mode) {

int fd;

#if !defined(_WIN32)
fd = open(path, mode);
#else
fd = -1;
open(&fd, path, mode);
#endif

return fd;
}






static bool bc_read_binary(const char *buf, size_t size) {

size_t i;

for (i = 0; i < size; ++i) {
if (BC_ERR(BC_READ_BIN_CHAR(buf[i]))) return true;
}

return false;
}

bool bc_read_buf(BcVec *vec, char *buf, size_t *buf_len) {

char *nl;


if (!*buf_len) return false;


nl = strchr(buf, '\n');


if (nl != NULL) {


size_t nllen = (size_t) ((nl + 1) - buf);

nllen = *buf_len >= nllen ? nllen : *buf_len;



bc_vec_npush(vec, nllen, buf);
*buf_len -= nllen;
memmove(buf, nl + 1, *buf_len + 1);

return true;
}


bc_vec_npush(vec, *buf_len, buf);
*buf_len = 0;

return false;
}

BcStatus bc_read_chars(BcVec *vec, const char *prompt) {

bool done = false;

assert(vec != NULL && vec->size == sizeof(char));

BC_SIG_ASSERT_NOT_LOCKED;


bc_vec_popAll(vec);


if (BC_PROMPT) {
bc_file_puts(&vm.fout, bc_flush_none, prompt);
bc_file_flush(&vm.fout, bc_flush_none);
}


if (bc_read_buf(vec, vm.buf, &vm.buf_len)) {
bc_vec_pushByte(vec, '\0');
return BC_STATUS_SUCCESS;
}


while (!done) {

ssize_t r;

BC_SIG_LOCK;


r = read(STDIN_FILENO, vm.buf + vm.buf_len,
BC_VM_STDIN_BUF_SIZE - vm.buf_len);


if (BC_UNLIKELY(r < 0)) {


if (errno == EINTR) {



if (vm.status == (sig_atomic_t) BC_STATUS_QUIT) BC_JMP;

assert(vm.sig);


vm.sig = 0;
vm.status = (sig_atomic_t) BC_STATUS_SUCCESS;


bc_file_puts(&vm.fout, bc_flush_none, bc_program_ready_msg);
if (BC_PROMPT) bc_file_puts(&vm.fout, bc_flush_none, prompt);
bc_file_flush(&vm.fout, bc_flush_none);

BC_SIG_UNLOCK;

continue;
}

BC_SIG_UNLOCK;


bc_vm_fatalError(BC_ERR_FATAL_IO_ERR);
}

BC_SIG_UNLOCK;


if (r == 0) {
bc_vec_pushByte(vec, '\0');
return BC_STATUS_EOF;
}

BC_SIG_LOCK;


vm.buf_len += (size_t) r;
vm.buf[vm.buf_len] = '\0';


done = bc_read_buf(vec, vm.buf, &vm.buf_len);

BC_SIG_UNLOCK;
}


bc_vec_pushByte(vec, '\0');

return BC_STATUS_SUCCESS;
}

BcStatus bc_read_line(BcVec *vec, const char *prompt) {

BcStatus s;

#if BC_ENABLE_HISTORY

if (BC_TTY && !vm.history.badTerm)
s = bc_history_line(&vm.history, vec, prompt);
else s = bc_read_chars(vec, prompt);
#else
s = bc_read_chars(vec, prompt);
#endif

if (BC_ERR(bc_read_binary(vec->v, vec->len - 1)))
bc_verr(BC_ERR_FATAL_BIN_FILE, bc_program_stdin_name);

return s;
}

char* bc_read_file(const char *path) {

BcErr e = BC_ERR_FATAL_IO_ERR;
size_t size, to_read;
struct stat pstat;
int fd;
char* buf;
char* buf2;

BC_SIG_ASSERT_LOCKED;

assert(path != NULL);

#if !defined(NDEBUG)

memset(&pstat, 0, sizeof(struct stat));
#endif

fd = bc_read_open(path, O_RDONLY);


if (BC_ERR(fd < 0)) bc_verr(BC_ERR_FATAL_FILE_ERR, path);



if (BC_ERR(fstat(fd, &pstat) == -1)) goto malloc_err;


if (BC_ERR(S_ISDIR(pstat.st_mode))) {
e = BC_ERR_FATAL_PATH_DIR;
goto malloc_err;
}


size = (size_t) pstat.st_size;
buf = bc_vm_malloc(size + 1);
buf2 = buf;
to_read = size;

do {



ssize_t r = read(fd, buf2, to_read);
if (BC_ERR(r < 0)) goto read_err;
to_read -= (size_t) r;
buf2 += (size_t) r;
} while (to_read);


buf[size] = '\0';

if (BC_ERR(bc_read_binary(buf, size))) {
e = BC_ERR_FATAL_BIN_FILE;
goto read_err;
}

close(fd);

return buf;

read_err:
free(buf);
malloc_err:
close(fd);
bc_verr(e, path);
return NULL;
}
