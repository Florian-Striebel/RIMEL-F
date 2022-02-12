#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <limits.h>
#include <errno.h>
#include <termios.h>
#include <sys/ioctl.h>
#if defined(__sun)
#include <stropts.h>
#endif
#include "config.h"
#if (ULONG_MAX == 4294967295UL)
#define MEMTEST_32BIT
#elif (ULONG_MAX == 18446744073709551615ULL)
#define MEMTEST_64BIT
#else
#error "ULONG_MAX value not supported."
#endif
#if defined(MEMTEST_32BIT)
#define ULONG_ONEZERO 0xaaaaaaaaUL
#define ULONG_ZEROONE 0x55555555UL
#else
#define ULONG_ONEZERO 0xaaaaaaaaaaaaaaaaUL
#define ULONG_ZEROONE 0x5555555555555555UL
#endif
#if defined(__has_attribute)
#if __has_attribute(no_sanitize)
#define NO_SANITIZE(sanitizer) __attribute__((no_sanitize(sanitizer)))
#endif
#endif
#if !defined(NO_SANITIZE)
#define NO_SANITIZE(sanitizer)
#endif
static struct winsize ws;
size_t progress_printed;
size_t progress_full;
void memtest_progress_start(char *title, int pass) {
int j;
printf("\x1b[H\x1b[2J");
for (j = 0; j < ws.ws_col*(ws.ws_row-2); j++) printf(".");
printf("Please keep the test running several minutes per GB of memory.\n");
printf("Also check http://www.memtest86.com/ and http://pyropus.ca/software/memtester/");
printf("\x1b[H\x1b[2K");
printf("%s [%d]\n", title, pass);
progress_printed = 0;
progress_full = (size_t)ws.ws_col*(ws.ws_row-3);
fflush(stdout);
}
void memtest_progress_end(void) {
printf("\x1b[H\x1b[2J");
}
void memtest_progress_step(size_t curr, size_t size, char c) {
size_t chars = ((unsigned long long)curr*progress_full)/size, j;
for (j = 0; j < chars-progress_printed; j++) printf("%c",c);
progress_printed = chars;
fflush(stdout);
}
int memtest_addressing(unsigned long *l, size_t bytes, int interactive) {
unsigned long words = bytes/sizeof(unsigned long);
unsigned long j, *p;
p = l;
for (j = 0; j < words; j++) {
*p = (unsigned long)p;
p++;
if ((j & 0xffff) == 0 && interactive)
memtest_progress_step(j,words*2,'A');
}
p = l;
for (j = 0; j < words; j++) {
if (*p != (unsigned long)p) {
if (interactive) {
printf("\n*** MEMORY ADDRESSING ERROR: %p contains %lu\n",
(void*) p, *p);
exit(1);
}
return 1;
}
p++;
if ((j & 0xffff) == 0 && interactive)
memtest_progress_step(j+words,words*2,'A');
}
return 0;
}
#define xorshift64star_next() do { rseed ^= rseed >> 12; rseed ^= rseed << 25; rseed ^= rseed >> 27; rout = rseed * UINT64_C(2685821657736338717); } while(0)
void memtest_fill_random(unsigned long *l, size_t bytes, int interactive) {
unsigned long step = 4096/sizeof(unsigned long);
unsigned long words = bytes/sizeof(unsigned long)/2;
unsigned long iwords = words/step;
unsigned long off, w, *l1, *l2;
uint64_t rseed = UINT64_C(0xd13133de9afdb566);
uint64_t rout = 0;
assert((bytes & 4095) == 0);
for (off = 0; off < step; off++) {
l1 = l+off;
l2 = l1+words;
for (w = 0; w < iwords; w++) {
xorshift64star_next();
*l1 = *l2 = (unsigned long) rout;
l1 += step;
l2 += step;
if ((w & 0xffff) == 0 && interactive)
memtest_progress_step(w+iwords*off,words,'R');
}
}
}
void memtest_fill_value(unsigned long *l, size_t bytes, unsigned long v1,
unsigned long v2, char sym, int interactive)
{
unsigned long step = 4096/sizeof(unsigned long);
unsigned long words = bytes/sizeof(unsigned long)/2;
unsigned long iwords = words/step;
unsigned long off, w, *l1, *l2, v;
assert((bytes & 4095) == 0);
for (off = 0; off < step; off++) {
l1 = l+off;
l2 = l1+words;
v = (off & 1) ? v2 : v1;
for (w = 0; w < iwords; w++) {
#if defined(MEMTEST_32BIT)
*l1 = *l2 = ((unsigned long) v) |
(((unsigned long) v) << 16);
#else
*l1 = *l2 = ((unsigned long) v) |
(((unsigned long) v) << 16) |
(((unsigned long) v) << 32) |
(((unsigned long) v) << 48);
#endif
l1 += step;
l2 += step;
if ((w & 0xffff) == 0 && interactive)
memtest_progress_step(w+iwords*off,words,sym);
}
}
}
int memtest_compare(unsigned long *l, size_t bytes, int interactive) {
unsigned long words = bytes/sizeof(unsigned long)/2;
unsigned long w, *l1, *l2;
assert((bytes & 4095) == 0);
l1 = l;
l2 = l1+words;
for (w = 0; w < words; w++) {
if (*l1 != *l2) {
if (interactive) {
printf("\n*** MEMORY ERROR DETECTED: %p != %p (%lu vs %lu)\n",
(void*)l1, (void*)l2, *l1, *l2);
exit(1);
}
return 1;
}
l1 ++;
l2 ++;
if ((w & 0xffff) == 0 && interactive)
memtest_progress_step(w,words,'=');
}
return 0;
}
int memtest_compare_times(unsigned long *m, size_t bytes, int pass, int times,
int interactive)
{
int j;
int errors = 0;
for (j = 0; j < times; j++) {
if (interactive) memtest_progress_start("Compare",pass);
errors += memtest_compare(m,bytes,interactive);
if (interactive) memtest_progress_end();
}
return errors;
}
int memtest_test(unsigned long *m, size_t bytes, int passes, int interactive) {
int pass = 0;
int errors = 0;
while (pass != passes) {
pass++;
if (interactive) memtest_progress_start("Addressing test",pass);
errors += memtest_addressing(m,bytes,interactive);
if (interactive) memtest_progress_end();
if (interactive) memtest_progress_start("Random fill",pass);
memtest_fill_random(m,bytes,interactive);
if (interactive) memtest_progress_end();
errors += memtest_compare_times(m,bytes,pass,4,interactive);
if (interactive) memtest_progress_start("Solid fill",pass);
memtest_fill_value(m,bytes,0,(unsigned long)-1,'S',interactive);
if (interactive) memtest_progress_end();
errors += memtest_compare_times(m,bytes,pass,4,interactive);
if (interactive) memtest_progress_start("Checkerboard fill",pass);
memtest_fill_value(m,bytes,ULONG_ONEZERO,ULONG_ZEROONE,'C',interactive);
if (interactive) memtest_progress_end();
errors += memtest_compare_times(m,bytes,pass,4,interactive);
}
return errors;
}
#define MEMTEST_BACKUP_WORDS (1024*(1024/sizeof(long)))
#define MEMTEST_DECACHE_SIZE (1024*8)
NO_SANITIZE("undefined")
int memtest_preserving_test(unsigned long *m, size_t bytes, int passes) {
unsigned long backup[MEMTEST_BACKUP_WORDS];
unsigned long *p = m;
unsigned long *end = (unsigned long*) (((unsigned char*)m)+(bytes-MEMTEST_DECACHE_SIZE));
size_t left = bytes;
int errors = 0;
if (bytes & 4095) return 0;
if (bytes < 4096*2) return 0;
while(left) {
if (left == 4096) {
left += 4096;
p -= 4096/sizeof(unsigned long);
}
int pass = 0;
size_t len = (left > sizeof(backup)) ? sizeof(backup) : left;
if (len/4096 % 2) len -= 4096;
memcpy(backup,p,len);
while(pass != passes) {
pass++;
errors += memtest_addressing(p,len,0);
memtest_fill_random(p,len,0);
if (bytes >= MEMTEST_DECACHE_SIZE) {
memtest_compare_times(m,MEMTEST_DECACHE_SIZE,pass,1,0);
memtest_compare_times(end,MEMTEST_DECACHE_SIZE,pass,1,0);
}
errors += memtest_compare_times(p,len,pass,4,0);
memtest_fill_value(p,len,0,(unsigned long)-1,'S',0);
if (bytes >= MEMTEST_DECACHE_SIZE) {
memtest_compare_times(m,MEMTEST_DECACHE_SIZE,pass,1,0);
memtest_compare_times(end,MEMTEST_DECACHE_SIZE,pass,1,0);
}
errors += memtest_compare_times(p,len,pass,4,0);
memtest_fill_value(p,len,ULONG_ONEZERO,ULONG_ZEROONE,'C',0);
if (bytes >= MEMTEST_DECACHE_SIZE) {
memtest_compare_times(m,MEMTEST_DECACHE_SIZE,pass,1,0);
memtest_compare_times(end,MEMTEST_DECACHE_SIZE,pass,1,0);
}
errors += memtest_compare_times(p,len,pass,4,0);
}
memcpy(p,backup,len);
left -= len;
p += len/sizeof(unsigned long);
}
return errors;
}
void memtest_alloc_and_test(size_t megabytes, int passes) {
size_t bytes = megabytes*1024*1024;
unsigned long *m = malloc(bytes);
if (m == NULL) {
fprintf(stderr,"Unable to allocate %zu megabytes: %s",
megabytes, strerror(errno));
exit(1);
}
memtest_test(m,bytes,passes,1);
free(m);
}
void memtest(size_t megabytes, int passes) {
#if !defined(__HAIKU__)
if (ioctl(1, TIOCGWINSZ, &ws) == -1) {
ws.ws_col = 80;
ws.ws_row = 20;
}
#else
ws.ws_col = 80;
ws.ws_row = 20;
#endif
memtest_alloc_and_test(megabytes,passes);
printf("\nYour memory passed this test.\n");
printf("Please if you are still in doubt use the following two tools:\n");
printf("1) memtest86: http://www.memtest86.com/\n");
printf("2) memtester: http://pyropus.ca/software/memtester/\n");
exit(0);
}
