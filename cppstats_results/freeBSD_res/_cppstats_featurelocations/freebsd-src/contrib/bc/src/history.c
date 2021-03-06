















































































































































#if BC_ENABLE_HISTORY

#include <assert.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>

#include <signal.h>
#include <sys/stat.h>
#include <sys/types.h>

#if !defined(_WIN32)
#include <strings.h>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#endif

#include <status.h>
#include <vector.h>
#include <history.h>
#include <read.h>
#include <file.h>
#include <vm.h>

#if BC_DEBUG_CODE


BcFile bc_history_debug_fp;


char *bc_history_debug_buf;

#endif






static bool bc_history_wchar(uint32_t cp) {

size_t i;

for (i = 0; i < bc_history_wchars_len; ++i) {




if (bc_history_wchars[i][0] > cp) return false;


if (bc_history_wchars[i][0] <= cp && cp <= bc_history_wchars[i][1])
return true;
}

return false;
}






static bool bc_history_comboChar(uint32_t cp) {

size_t i;

for (i = 0; i < bc_history_combo_chars_len; ++i) {



if (bc_history_combo_chars[i] > cp) return false;
if (bc_history_combo_chars[i] == cp) return true;
}

return false;
}






static size_t bc_history_prevCharLen(const char *buf, size_t pos) {
size_t end = pos;
for (pos -= 1; pos < end && (buf[pos] & 0xC0) == 0x80; --pos);
return end - (pos >= end ? 0 : pos);
}








static size_t bc_history_codePoint(const char *s, size_t len, uint32_t *cp) {

if (len) {

uchar byte = (uchar) s[0];




if ((byte & 0x80) == 0) {
*cp = byte;
return 1;
}
else if ((byte & 0xE0) == 0xC0) {

if (len >= 2) {
*cp = (((uint32_t) (s[0] & 0x1F)) << 6) |
((uint32_t) (s[1] & 0x3F));
return 2;
}
}
else if ((byte & 0xF0) == 0xE0) {

if (len >= 3) {
*cp = (((uint32_t) (s[0] & 0x0F)) << 12) |
(((uint32_t) (s[1] & 0x3F)) << 6) |
((uint32_t) (s[2] & 0x3F));
return 3;
}
}
else if ((byte & 0xF8) == 0xF0) {

if (len >= 4) {
*cp = (((uint32_t) (s[0] & 0x07)) << 18) |
(((uint32_t) (s[1] & 0x3F)) << 12) |
(((uint32_t) (s[2] & 0x3F)) << 6) |
((uint32_t) (s[3] & 0x3F));
return 4;
}
}
else {
*cp = 0xFFFD;
return 1;
}
}

*cp = 0;

return 1;
}









static size_t bc_history_nextLen(const char *buf, size_t buf_len,
size_t pos, size_t *col_len)
{
uint32_t cp;
size_t beg = pos;
size_t len = bc_history_codePoint(buf + pos, buf_len - pos, &cp);

if (bc_history_comboChar(cp)) {

BC_UNREACHABLE

if (col_len != NULL) *col_len = 0;

return 0;
}


if (col_len != NULL) *col_len = bc_history_wchar(cp) ? 2 : 1;

pos += len;


while (pos < buf_len) {

len = bc_history_codePoint(buf + pos, buf_len - pos, &cp);

if (!bc_history_comboChar(cp)) return pos - beg;

pos += len;
}

return pos - beg;
}







static size_t bc_history_prevLen(const char *buf, size_t pos) {

size_t end = pos;


while (pos > 0) {

uint32_t cp;
size_t len = bc_history_prevCharLen(buf, pos);

pos -= len;
bc_history_codePoint(buf + pos, len, &cp);




if (!bc_history_comboChar(cp)) return end - pos;
}

BC_UNREACHABLE

return 0;
}








static ssize_t bc_history_read(char *buf, size_t n) {

ssize_t ret;

BC_SIG_ASSERT_LOCKED;

#if !defined(_WIN32)

do {

ret = read(STDIN_FILENO, buf, n);
} while (ret == EINTR);

#else

bool good;
DWORD read;
HANDLE hn = GetStdHandle(STD_INPUT_HANDLE);

good = ReadConsole(hn, buf, (DWORD) n, &read, NULL);

ret = (read != n) ? -1 : 1;

#endif

return ret;
}









static BcStatus bc_history_readCode(char *buf, size_t buf_len,
uint32_t *cp, size_t *nread)
{
ssize_t n;

assert(buf_len >= 1);

BC_SIG_LOCK;


n = bc_history_read(buf, 1);

BC_SIG_UNLOCK;

if (BC_ERR(n <= 0)) goto err;


uchar byte = ((uchar*) buf)[0];



if ((byte & 0x80) != 0) {

if ((byte & 0xE0) == 0xC0) {

assert(buf_len >= 2);

BC_SIG_LOCK;

n = bc_history_read(buf + 1, 1);

BC_SIG_UNLOCK;

if (BC_ERR(n <= 0)) goto err;
}
else if ((byte & 0xF0) == 0xE0) {

assert(buf_len >= 3);

BC_SIG_LOCK;

n = bc_history_read(buf + 1, 2);

BC_SIG_UNLOCK;

if (BC_ERR(n <= 0)) goto err;
}
else if ((byte & 0xF8) == 0xF0) {

assert(buf_len >= 3);

BC_SIG_LOCK;

n = bc_history_read(buf + 1, 3);

BC_SIG_UNLOCK;

if (BC_ERR(n <= 0)) goto err;
}
else {
n = -1;
goto err;
}
}


*nread = bc_history_codePoint(buf, buf_len, cp);

return BC_STATUS_SUCCESS;

err:

if (BC_ERR(n < 0)) bc_vm_fatalError(BC_ERR_FATAL_IO_ERR);
else *nread = (size_t) n;
return BC_STATUS_EOF;
}









static size_t bc_history_colPos(const char *buf, size_t buf_len, size_t pos) {

size_t ret = 0, off = 0;


while (off < pos && off < buf_len) {

size_t col_len, len;

len = bc_history_nextLen(buf, buf_len, off, &col_len);

off += len;
ret += col_len;
}

return ret;
}






static inline bool bc_history_isBadTerm(void) {

size_t i;
bool ret = false;
char *term = bc_vm_getenv("TERM");

if (term == NULL) return false;

for (i = 0; !ret && bc_history_bad_terms[i]; ++i)
ret = (!strcasecmp(term, bc_history_bad_terms[i]));

bc_vm_getenvFree(term);

return ret;
}





static void bc_history_enableRaw(BcHistory *h) {




#if !defined(_WIN32)
struct termios raw;
int err;

assert(BC_TTYIN);

if (h->rawMode) return;

BC_SIG_LOCK;

if (BC_ERR(tcgetattr(STDIN_FILENO, &h->orig_termios) == -1))
bc_vm_fatalError(BC_ERR_FATAL_IO_ERR);

BC_SIG_UNLOCK;


raw = h->orig_termios;



raw.c_iflag &= (unsigned int) (~(BRKINT | ICRNL | INPCK | ISTRIP | IXON));


raw.c_cflag |= (CS8);



raw.c_lflag &= (unsigned int) (~(ECHO | ICANON | IEXTEN | ISIG));



raw.c_cc[VMIN] = 1;
raw.c_cc[VTIME] = 0;

BC_SIG_LOCK;


do {
err = tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
} while (BC_ERR(err < 0) && errno == EINTR);

BC_SIG_UNLOCK;

if (BC_ERR(err < 0)) bc_vm_fatalError(BC_ERR_FATAL_IO_ERR);
#endif

h->rawMode = true;
}





static void bc_history_disableRaw(BcHistory *h) {

sig_atomic_t lock;

if (!h->rawMode) return;

BC_SIG_TRYLOCK(lock);

#if !defined(_WIN32)
if (BC_ERR(tcsetattr(STDIN_FILENO, TCSAFLUSH, &h->orig_termios) != -1))
h->rawMode = false;
#endif

BC_SIG_TRYUNLOCK(lock);
}







static size_t bc_history_cursorPos(void) {

char buf[BC_HIST_SEQ_SIZE];
char *ptr, *ptr2;
size_t cols, rows, i;

BC_SIG_ASSERT_LOCKED;


bc_file_write(&vm.fout, bc_flush_none, "\x1b[6n", 4);
bc_file_flush(&vm.fout, bc_flush_none);


for (i = 0; i < sizeof(buf) - 1; ++i) {
if (bc_history_read(buf + i, 1) != 1 || buf[i] == 'R') break;
}

buf[i] = '\0';


if (BC_ERR(buf[0] != BC_ACTION_ESC || buf[1] != '[')) return SIZE_MAX;


ptr = buf + 2;
rows = strtoul(ptr, &ptr2, 10);


if (BC_ERR(!rows || ptr2[0] != ';')) return SIZE_MAX;


ptr = ptr2 + 1;
cols = strtoul(ptr, NULL, 10);

if (BC_ERR(!cols)) return SIZE_MAX;

return cols <= UINT16_MAX ? cols : 0;
}






static size_t bc_history_columns(void) {

#if !defined(_WIN32)

struct winsize ws;
int ret;

ret = ioctl(vm.fout.fd, TIOCGWINSZ, &ws);

if (BC_ERR(ret == -1 || !ws.ws_col)) {


size_t start, cols;


start = bc_history_cursorPos();
if (BC_ERR(start == SIZE_MAX)) return BC_HIST_DEF_COLS;


bc_file_write(&vm.fout, bc_flush_none, "\x1b[999C", 6);
bc_file_flush(&vm.fout, bc_flush_none);
cols = bc_history_cursorPos();
if (BC_ERR(cols == SIZE_MAX)) return BC_HIST_DEF_COLS;


if (cols > start) {
bc_file_printf(&vm.fout, "\x1b[%zuD", cols - start);
bc_file_flush(&vm.fout, bc_flush_none);
}

return cols;
}

return ws.ws_col;

#else

CONSOLE_SCREEN_BUFFER_INFO csbi;

if (!GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi))
return 80;

return ((size_t) (csbi.srWindow.Right)) - csbi.srWindow.Left + 1;

#endif
}








static size_t bc_history_promptColLen(const char *prompt, size_t plen) {

char buf[BC_HIST_MAX_LINE + 1];
size_t buf_len = 0, off = 0;



while (off < plen) buf[buf_len++] = prompt[off++];

return bc_history_colPos(buf, buf_len, buf_len);
}






static void bc_history_refresh(BcHistory *h) {

char* buf = h->buf.v;
size_t colpos, len = BC_HIST_BUF_LEN(h), pos = h->pos, extras_len = 0;

BC_SIG_ASSERT_LOCKED;

bc_file_flush(&vm.fout, bc_flush_none);


while(h->pcol + bc_history_colPos(buf, len, pos) >= h->cols) {

size_t chlen = bc_history_nextLen(buf, len, 0, NULL);

buf += chlen;
len -= chlen;
pos -= chlen;
}


while (h->pcol + bc_history_colPos(buf, len, len) > h->cols)
len -= bc_history_prevLen(buf, len);


bc_file_write(&vm.fout, bc_flush_none, "\r", 1);



if (h->extras.len > 1) {

extras_len = h->extras.len - 1;

bc_vec_grow(&h->buf, extras_len);

len += extras_len;
pos += extras_len;

bc_file_write(&vm.fout, bc_flush_none, h->extras.v, extras_len);
}


if (BC_PROMPT) bc_file_write(&vm.fout, bc_flush_none, h->prompt, h->plen);

bc_file_write(&vm.fout, bc_flush_none, h->buf.v, len - extras_len);


bc_file_write(&vm.fout, bc_flush_none, "\x1b[0K", 4);


if (pos >= h->buf.len - extras_len)
bc_vec_grow(&h->buf, pos + extras_len);


colpos = bc_history_colPos(h->buf.v, len - extras_len, pos) + h->pcol;


if (colpos) bc_file_printf(&vm.fout, "\r\x1b[%zuC", colpos);

bc_file_flush(&vm.fout, bc_flush_none);
}







static void bc_history_edit_insert(BcHistory *h, const char *cbuf, size_t clen)
{
BC_SIG_ASSERT_LOCKED;

bc_vec_grow(&h->buf, clen);


if (h->pos == BC_HIST_BUF_LEN(h)) {

size_t colpos = 0, len;


memcpy(bc_vec_item(&h->buf, h->pos), cbuf, clen);


h->pos += clen;
h->buf.len += clen - 1;
bc_vec_pushByte(&h->buf, '\0');


len = BC_HIST_BUF_LEN(h) + h->extras.len - 1;
colpos = bc_history_promptColLen(h->prompt, h->plen);
colpos += bc_history_colPos(h->buf.v, len, len);


if (colpos < h->cols) {


bc_file_write(&vm.fout, bc_flush_none, cbuf, clen);
bc_file_flush(&vm.fout, bc_flush_none);
}
else bc_history_refresh(h);
}
else {


size_t amt = BC_HIST_BUF_LEN(h) - h->pos;


memmove(h->buf.v + h->pos + clen, h->buf.v + h->pos, amt);
memcpy(h->buf.v + h->pos, cbuf, clen);


h->pos += clen;
h->buf.len += clen;
h->buf.v[BC_HIST_BUF_LEN(h)] = '\0';

bc_history_refresh(h);
}
}





static void bc_history_edit_left(BcHistory *h) {

BC_SIG_ASSERT_LOCKED;


if (h->pos <= 0) return;

h->pos -= bc_history_prevLen(h->buf.v, h->pos);

bc_history_refresh(h);
}





static void bc_history_edit_right(BcHistory *h) {

BC_SIG_ASSERT_LOCKED;


if (h->pos == BC_HIST_BUF_LEN(h)) return;

h->pos += bc_history_nextLen(h->buf.v, BC_HIST_BUF_LEN(h), h->pos, NULL);

bc_history_refresh(h);
}





static void bc_history_edit_wordEnd(BcHistory *h) {

size_t len = BC_HIST_BUF_LEN(h);

BC_SIG_ASSERT_LOCKED;


if (!len || h->pos >= len) return;


while (h->pos < len && isspace(h->buf.v[h->pos])) h->pos += 1;
while (h->pos < len && !isspace(h->buf.v[h->pos])) h->pos += 1;

bc_history_refresh(h);
}





static void bc_history_edit_wordStart(BcHistory *h) {

size_t len = BC_HIST_BUF_LEN(h);

BC_SIG_ASSERT_LOCKED;


if (!len) return;


while (h->pos > 0 && isspace(h->buf.v[h->pos - 1])) h->pos -= 1;
while (h->pos > 0 && !isspace(h->buf.v[h->pos - 1])) h->pos -= 1;

bc_history_refresh(h);
}





static void bc_history_edit_home(BcHistory *h) {

BC_SIG_ASSERT_LOCKED;


if (!h->pos) return;

h->pos = 0;

bc_history_refresh(h);
}





static void bc_history_edit_end(BcHistory *h) {

BC_SIG_ASSERT_LOCKED;


if (h->pos == BC_HIST_BUF_LEN(h)) return;

h->pos = BC_HIST_BUF_LEN(h);

bc_history_refresh(h);
}







static void bc_history_edit_next(BcHistory *h, bool dir) {

const char *dup, *str;

BC_SIG_ASSERT_LOCKED;


if (h->history.len <= 1) return;


if (h->buf.v[0]) dup = bc_vm_strdup(h->buf.v);
else dup = "";


bc_vec_replaceAt(&h->history, h->history.len - 1 - h->idx, &dup);


h->idx += (dir == BC_HIST_PREV ? 1 : SIZE_MAX);


if (h->idx == SIZE_MAX) {
h->idx = 0;
return;
}
else if (h->idx >= h->history.len) {
h->idx = h->history.len - 1;
return;
}


str = *((char**) bc_vec_item(&h->history, h->history.len - 1 - h->idx));
bc_vec_string(&h->buf, strlen(str), str);

assert(h->buf.len > 0);


h->pos = BC_HIST_BUF_LEN(h);

bc_history_refresh(h);
}






static void bc_history_edit_delete(BcHistory *h) {

size_t chlen, len = BC_HIST_BUF_LEN(h);

BC_SIG_ASSERT_LOCKED;


if (!len || h->pos >= len) return;


chlen = bc_history_nextLen(h->buf.v, len, h->pos, NULL);


memmove(h->buf.v + h->pos, h->buf.v + h->pos + chlen, len - h->pos - chlen);


h->buf.len -= chlen;
h->buf.v[BC_HIST_BUF_LEN(h)] = '\0';

bc_history_refresh(h);
}






static void bc_history_edit_backspace(BcHistory *h) {

size_t chlen, len = BC_HIST_BUF_LEN(h);

BC_SIG_ASSERT_LOCKED;


if (!h->pos || !len) return;


chlen = bc_history_prevLen(h->buf.v, h->pos);


memmove(h->buf.v + h->pos - chlen, h->buf.v + h->pos, len - h->pos);


h->pos -= chlen;
h->buf.len -= chlen;
h->buf.v[BC_HIST_BUF_LEN(h)] = '\0';

bc_history_refresh(h);
}






static void bc_history_edit_deletePrevWord(BcHistory *h) {

size_t diff, old_pos = h->pos;

BC_SIG_ASSERT_LOCKED;


if (!old_pos) return;


while (h->pos > 0 && isspace(h->buf.v[h->pos - 1])) --h->pos;
while (h->pos > 0 && !isspace(h->buf.v[h->pos - 1])) --h->pos;


diff = old_pos - h->pos;


memmove(h->buf.v + h->pos, h->buf.v + old_pos,
BC_HIST_BUF_LEN(h) - old_pos + 1);


h->buf.len -= diff;

bc_history_refresh(h);
}





static void bc_history_edit_deleteNextWord(BcHistory *h) {

size_t next_end = h->pos, len = BC_HIST_BUF_LEN(h);

BC_SIG_ASSERT_LOCKED;


if (next_end == len) return;


while (next_end < len && isspace(h->buf.v[next_end])) ++next_end;
while (next_end < len && !isspace(h->buf.v[next_end])) ++next_end;


memmove(h->buf.v + h->pos, h->buf.v + next_end, len - next_end);


h->buf.len -= next_end - h->pos;

bc_history_refresh(h);
}





static void bc_history_swap(BcHistory *h) {

size_t pcl, ncl;
char auxb[5];

BC_SIG_ASSERT_LOCKED;


pcl = bc_history_prevLen(h->buf.v, h->pos);
ncl = bc_history_nextLen(h->buf.v, BC_HIST_BUF_LEN(h), h->pos, NULL);




if (pcl && h->pos != BC_HIST_BUF_LEN(h) && pcl < 5 && ncl < 5) {


memcpy(auxb, h->buf.v + h->pos - pcl, pcl);
memcpy(h->buf.v + h->pos - pcl, h->buf.v + h->pos, ncl);
memcpy(h->buf.v + h->pos - pcl + ncl, auxb, pcl);


h->pos += ((~pcl) + 1) + ncl;

bc_history_refresh(h);
}
}






static void bc_history_raise(BcHistory *h, int sig) {


bc_history_disableRaw(h);
raise(sig);
}






static void bc_history_escape(BcHistory *h) {

char c, seq[3];

BC_SIG_ASSERT_LOCKED;


if (BC_ERR(BC_HIST_READ(seq, 1))) return;

c = seq[0];


if (c != '[' && c != 'O') {
if (c == 'f') bc_history_edit_wordEnd(h);
else if (c == 'b') bc_history_edit_wordStart(h);
else if (c == 'd') bc_history_edit_deleteNextWord(h);
}
else {


if (BC_ERR(BC_HIST_READ(seq + 1, 1)))
bc_vm_fatalError(BC_ERR_FATAL_IO_ERR);


if (c == '[') {

c = seq[1];

if (c >= '0' && c <= '9') {


if (BC_ERR(BC_HIST_READ(seq + 2, 1)))
bc_vm_fatalError(BC_ERR_FATAL_IO_ERR);

if (seq[2] == '~' && c == '3') bc_history_edit_delete(h);
else if(seq[2] == ';') {


if (BC_ERR(BC_HIST_READ(seq, 2)))
bc_vm_fatalError(BC_ERR_FATAL_IO_ERR);

if (seq[0] != '5') return;
else if (seq[1] == 'C') bc_history_edit_wordEnd(h);
else if (seq[1] == 'D') bc_history_edit_wordStart(h);
}
}
else {

switch(c) {


case 'A':
{
bc_history_edit_next(h, BC_HIST_PREV);
break;
}


case 'B':
{
bc_history_edit_next(h, BC_HIST_NEXT);
break;
}


case 'C':
{
bc_history_edit_right(h);
break;
}


case 'D':
{
bc_history_edit_left(h);
break;
}


case 'H':
case '1':
{
bc_history_edit_home(h);
break;
}


case 'F':
case '4':
{
bc_history_edit_end(h);
break;
}

case 'd':
{
bc_history_edit_deleteNextWord(h);
break;
}
}
}
}

else {

switch (seq[1]) {

case 'A':
{
bc_history_edit_next(h, BC_HIST_PREV);
break;
}

case 'B':
{
bc_history_edit_next(h, BC_HIST_NEXT);
break;
}

case 'C':
{
bc_history_edit_right(h);
break;
}

case 'D':
{
bc_history_edit_left(h);
break;
}

case 'F':
{
bc_history_edit_end(h);
break;
}

case 'H':
{
bc_history_edit_home(h);
break;
}
}
}
}
}






static void bc_history_add(BcHistory *h, char *line) {

BC_SIG_ASSERT_LOCKED;


if (h->history.len) {


char *s = *((char**) bc_vec_item_rev(&h->history, 0));


if (!strcmp(s, line)) {
free(line);
return;
}
}

bc_vec_push(&h->history, &line);
}






static void bc_history_add_empty(BcHistory *h) {

BC_SIG_ASSERT_LOCKED;

const char *line = "";


if (h->history.len) {


char *s = *((char**) bc_vec_item_rev(&h->history, 0));


if (!s[0]) return;
}

bc_vec_push(&h->history, &line);
}





static void bc_history_reset(BcHistory *h) {

BC_SIG_ASSERT_LOCKED;

h->oldcolpos = h->pos = h->idx = 0;
h->cols = bc_history_columns();



bc_history_add_empty(h);


bc_vec_empty(&h->buf);
}






static void bc_history_printCtrl(BcHistory *h, unsigned int c) {

char str[3] = "^A";
const char newline[2] = "\n";

BC_SIG_ASSERT_LOCKED;


str[1] = (char) (c + 'A' - BC_ACTION_CTRL_A);


bc_vec_concat(&h->buf, str);

bc_history_refresh(h);


bc_vec_npop(&h->buf, sizeof(str));
bc_vec_pushByte(&h->buf, '\0');

#if !defined(_WIN32)
if (c != BC_ACTION_CTRL_C && c != BC_ACTION_CTRL_D)
#endif
{


bc_file_write(&vm.fout, bc_flush_none, newline, sizeof(newline) - 1);
bc_history_refresh(h);
}
}









static BcStatus bc_history_edit(BcHistory *h, const char *prompt) {

BC_SIG_LOCK;

bc_history_reset(h);







if (BC_PROMPT) {

h->prompt = prompt;
h->plen = strlen(prompt);
h->pcol = bc_history_promptColLen(prompt, h->plen);

bc_file_write(&vm.fout, bc_flush_none, prompt, h->plen);
bc_file_flush(&vm.fout, bc_flush_none);
}


for (;;) {

BcStatus s;
char cbuf[32];
unsigned int c = 0;
size_t nread = 0;

BC_SIG_UNLOCK;


s = bc_history_readCode(cbuf, sizeof(cbuf), &c, &nread);
if (BC_ERR(s)) return s;

BC_SIG_LOCK;

switch (c) {

case BC_ACTION_LINE_FEED:
case BC_ACTION_ENTER:
{

bc_vec_pop(&h->history);
BC_SIG_UNLOCK;
return s;
}

case BC_ACTION_TAB:
{

memcpy(cbuf, bc_history_tab, bc_history_tab_len + 1);
bc_history_edit_insert(h, cbuf, bc_history_tab_len);
break;
}

#if !defined(_WIN32)
case BC_ACTION_CTRL_C:
{
bc_history_printCtrl(h, c);


if (!BC_SIGINT) {
vm.status = BC_STATUS_QUIT;
BC_SIG_UNLOCK;
BC_JMP;
}


bc_file_write(&vm.fout, bc_flush_none, vm.sigmsg, vm.siglen);
bc_file_write(&vm.fout, bc_flush_none, bc_program_ready_msg,
bc_program_ready_msg_len);
bc_history_reset(h);
bc_history_refresh(h);

break;
}
#endif

case BC_ACTION_BACKSPACE:
case BC_ACTION_CTRL_H:
{
bc_history_edit_backspace(h);
break;
}

#if !defined(_WIN32)

case BC_ACTION_CTRL_D:
{
bc_history_printCtrl(h, c);
BC_SIG_UNLOCK;
return BC_STATUS_EOF;
}
#endif


case BC_ACTION_CTRL_T:
{
bc_history_swap(h);
break;
}

case BC_ACTION_CTRL_B:
{
bc_history_edit_left(h);
break;
}

case BC_ACTION_CTRL_F:
{
bc_history_edit_right(h);
break;
}

case BC_ACTION_CTRL_P:
{
bc_history_edit_next(h, BC_HIST_PREV);
break;
}

case BC_ACTION_CTRL_N:
{
bc_history_edit_next(h, BC_HIST_NEXT);
break;
}

case BC_ACTION_ESC:
{
bc_history_escape(h);
break;
}


case BC_ACTION_CTRL_U:
{
bc_vec_string(&h->buf, 0, "");
h->pos = 0;

bc_history_refresh(h);

break;
}


case BC_ACTION_CTRL_K:
{
bc_vec_npop(&h->buf, h->buf.len - h->pos);
bc_vec_pushByte(&h->buf, '\0');
bc_history_refresh(h);
break;
}


case BC_ACTION_CTRL_A:
{
bc_history_edit_home(h);
break;
}


case BC_ACTION_CTRL_E:
{
bc_history_edit_end(h);
break;
}


case BC_ACTION_CTRL_L:
{
bc_file_write(&vm.fout, bc_flush_none, "\x1b[H\x1b[2J", 7);
bc_history_refresh(h);
break;
}


case BC_ACTION_CTRL_W:
{
bc_history_edit_deletePrevWord(h);
break;
}

default:
{


if ((c >= BC_ACTION_CTRL_A && c <= BC_ACTION_CTRL_Z) ||
c == BC_ACTION_CTRL_BSLASH)
{
bc_history_printCtrl(h, c);
#if !defined(_WIN32)
if (c == BC_ACTION_CTRL_Z) bc_history_raise(h, SIGTSTP);
if (c == BC_ACTION_CTRL_S) bc_history_raise(h, SIGSTOP);
if (c == BC_ACTION_CTRL_BSLASH)
bc_history_raise(h, SIGQUIT);
#else
vm.status = BC_STATUS_QUIT;
BC_SIG_UNLOCK;
BC_JMP;
#endif
}

else bc_history_edit_insert(h, cbuf, nread);
break;
}
}
}

BC_SIG_UNLOCK;

return BC_STATUS_SUCCESS;
}






static inline bool bc_history_stdinHasData(BcHistory *h) {
#if !defined(_WIN32)
int n;
return pselect(1, &h->rdset, NULL, NULL, &h->ts, &h->sigmask) > 0 ||
(ioctl(STDIN_FILENO, FIONREAD, &n) >= 0 && n > 0);
#else
return false;
#endif
}

BcStatus bc_history_line(BcHistory *h, BcVec *vec, const char *prompt) {

BcStatus s;
char* line;

assert(vm.fout.len == 0);

bc_history_enableRaw(h);

do {


s = bc_history_edit(h, prompt);


bc_file_write(&vm.fout, bc_flush_none, "\n", 1);
bc_file_flush(&vm.fout, bc_flush_none);

BC_SIG_LOCK;


if (h->buf.v[0]) {


line = bc_vm_strdup(h->buf.v);


bc_history_add(h, line);
}

else bc_history_add_empty(h);

BC_SIG_UNLOCK;


bc_vec_concat(vec, h->buf.v);
bc_vec_concat(vec, "\n");

} while (!s && bc_history_stdinHasData(h));

assert(!s || s == BC_STATUS_EOF);

bc_history_disableRaw(h);

return s;
}

void bc_history_string_free(void *str) {
char *s = *((char**) str);
BC_SIG_ASSERT_LOCKED;
if (s[0]) free(s);
}

void bc_history_init(BcHistory *h) {

#if defined(_WIN32)
HANDLE out, in;
#endif

BC_SIG_ASSERT_LOCKED;

h->rawMode = false;
h->badTerm = bc_history_isBadTerm();

#if defined(_WIN32)

h->orig_in = 0;
h->orig_out = 0;

in = GetStdHandle(STD_INPUT_HANDLE);
out = GetStdHandle(STD_OUTPUT_HANDLE);

if (!h->badTerm) {
SetConsoleCP(CP_UTF8);
SetConsoleOutputCP(CP_UTF8);
if (!GetConsoleMode(in, &h->orig_in) ||
!GetConsoleMode(out, &h->orig_out))
{
h->badTerm = true;
return;
}
else {
DWORD reqOut = ENABLE_VIRTUAL_TERMINAL_PROCESSING |
DISABLE_NEWLINE_AUTO_RETURN;
DWORD reqIn = ENABLE_VIRTUAL_TERMINAL_INPUT;
if (!SetConsoleMode(in, h->orig_in | reqIn) ||
!SetConsoleMode(out, h->orig_out | reqOut))
{
h->badTerm = true;
}
}
}
#endif

bc_vec_init(&h->buf, sizeof(char), BC_DTOR_NONE);
bc_vec_init(&h->history, sizeof(char*), BC_DTOR_HISTORY_STRING);
bc_vec_init(&h->extras, sizeof(char), BC_DTOR_NONE);

#if !defined(_WIN32)
FD_ZERO(&h->rdset);
FD_SET(STDIN_FILENO, &h->rdset);
h->ts.tv_sec = 0;
h->ts.tv_nsec = 0;

sigemptyset(&h->sigmask);
sigaddset(&h->sigmask, SIGINT);
#endif
}

void bc_history_free(BcHistory *h) {
BC_SIG_ASSERT_LOCKED;
#if !defined(_WIN32)
bc_history_disableRaw(h);
#else
SetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), h->orig_in);
SetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), h->orig_out);
#endif
#if !defined(NDEBUG)
bc_vec_free(&h->buf);
bc_vec_free(&h->history);
bc_vec_free(&h->extras);
#endif
}

#if BC_DEBUG_CODE






void bc_history_printKeyCodes(BcHistory *h) {

char quit[4];

bc_vm_printf("Linenoise key codes debugging mode.\n"
"Press keys to see scan codes. "
"Type 'quit' at any time to exit.\n");

bc_history_enableRaw(h);
memset(quit, ' ', 4);

while(true) {

char c;
ssize_t nread;

nread = bc_history_read(&c, 1);
if (nread <= 0) continue;


memmove(quit, quit + 1, sizeof(quit) - 1);


quit[sizeof(quit) - 1] = c;
if (!memcmp(quit, "quit", sizeof(quit))) break;

bc_vm_printf("'%c' %lu (type quit to exit)\n",
isprint(c) ? c : '?', (unsigned long) c);


bc_vm_putchar('\r', bc_flush_none);
bc_file_flush(&vm.fout, bc_flush_none);
}

bc_history_disableRaw(h);
}
#endif

#endif
