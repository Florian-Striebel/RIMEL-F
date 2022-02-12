






#define _GNU_SOURCE
#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <time.h>
#include <unistd.h>
#include "../include/networking.h"

#if WANT_ASSERT
#undef NDEBUG
#include <assert.h>
#define Assert(exp) assert(exp)
#else
#define Assert(exp) (void) (exp)
#endif

#if defined(__GNUC__)
#define may_alias __attribute__((__may_alias__))
#else
#define may_alias
#endif

#define CACHE_LINE 64
#define ALIGN(x, y) (((x) + (y) - 1) & ~((y) - 1))


static uint16_t
checksum_simple(const void *ptr, uint32_t nbytes)
{
const uint16_t *may_alias hptr = ptr;
uint64_t sum = 0;


for (uint32_t nhalfs = nbytes >> 1; nhalfs != 0; nhalfs--)
{
sum += *hptr++;
}


if ((nbytes & 0x01) != 0)
{
sum += *(uint8_t *) hptr;
}


sum = (sum & 0xffffffff) + (sum >> 32);
sum = (sum & 0xffffffff) + (sum >> 32);
Assert(sum == (uint32_t) sum);


sum = (sum & 0xffff) + (sum >> 16);
sum = (sum & 0xffff) + (sum >> 16);
Assert(sum == (uint16_t) sum);

return (uint16_t) sum;
}

static struct
{
uint16_t (*cksum_fp)(const void *, uint32_t);
const char *name;
} implementations[] =
{
{ checksum_simple, "simple"},
{ __chksum, "scalar"},
#if __arm__
{ __chksum_arm_simd, "simd" },
#elif __aarch64__
{ __chksum_aarch64_simd, "simd" },
#endif
{ NULL, NULL}
};

static int
find_impl(const char *name)
{
for (int i = 0; implementations[i].name != NULL; i++)
{
if (strcmp(implementations[i].name, name) == 0)
{
return i;
}
}
return -1;
}

static uint16_t (*CKSUM_FP)(const void *, uint32_t);
static volatile uint16_t SINK;

static bool
verify(const void *data, uint32_t offset, uint32_t size)
{

uint16_t csum_expected = checksum_simple(data, size);
uint16_t csum_actual = CKSUM_FP(data, size);
if (csum_actual != csum_expected)
{
fprintf(stderr, "\nInvalid checksum for offset %u size %u: "
"actual %04x expected %04x (valid)",
offset, size, csum_actual, csum_expected);
if (size < 65536)
{

exit(EXIT_FAILURE);
}

return false;
}
return true;
}

static uint64_t
clock_get_ns(void)
{
struct timespec ts;
clock_gettime(CLOCK_MONOTONIC, &ts);
return ts.tv_sec * (uint64_t) 1000000000 + ts.tv_nsec;
}

static void
benchmark(const uint8_t *base,
size_t poolsize,
uint32_t blksize,
uint32_t numops,
uint64_t cpufreq)
{
printf("%11u ", (unsigned int) blksize); fflush(stdout);

uint64_t start = clock_get_ns();
for (uint32_t i = 0; i < numops; i ++)
{

uint32_t random = ((uint32_t *) base)[i % (poolsize / 4)];

const void *data = &base[random % (poolsize - blksize)];
SINK = CKSUM_FP(data, blksize);
}
uint64_t end = clock_get_ns();

#define MEGABYTE 1000000
uint64_t elapsed_ns = end - start;
uint64_t elapsed_ms = elapsed_ns / 1000000;
uint32_t blks_per_s = (uint32_t) ((numops / elapsed_ms) * 1000);
uint64_t accbytes = (uint64_t) numops * blksize;
printf("%11ju ", (uintmax_t) ((accbytes / elapsed_ms) * 1000) / MEGABYTE);
unsigned int cyc_per_blk = cpufreq / blks_per_s;
printf("%11u ", cyc_per_blk);
if (blksize != 0)
{
unsigned int cyc_per_byte = 1000 * cyc_per_blk / blksize;
printf("%7u.%03u ",
cyc_per_byte / 1000, cyc_per_byte % 1000);
}
printf("\n");
}

int main(int argc, char *argv[])
{
int c;
bool DUMP = false;
uint32_t IMPL = 0;
uint64_t CPUFREQ = 0;
uint32_t BLKSIZE = 0;
uint32_t NUMOPS = 1000000;
uint32_t POOLSIZE = 512 * 1024;

setvbuf(stdout, NULL, _IOLBF, 160);
while ((c = getopt(argc, argv, "b:df:i:n:p:")) != -1)
{
switch (c)
{
case 'b' :
{
int blksize = atoi(optarg);
if (blksize < 1 || blksize > POOLSIZE / 2)
{
fprintf(stderr, "Invalid block size %d\n", blksize);
exit(EXIT_FAILURE);
}
BLKSIZE = (unsigned) blksize;
break;
}
case 'd' :
DUMP = true;
break;
case 'f' :
{
int64_t cpufreq = atoll(optarg);
if (cpufreq < 1)
{
fprintf(stderr, "Invalid CPU frequency %"PRId64"\n",
cpufreq);
exit(EXIT_FAILURE);
}
CPUFREQ = cpufreq;
break;
}
case 'i' :
{
int impl = find_impl(optarg);
if (impl < 0)
{
fprintf(stderr, "Invalid implementation %s\n", optarg);
goto usage;
}
IMPL = (unsigned) impl;
break;
}
case 'n' :
{
int numops = atoi(optarg);
if (numops < 1)
{
fprintf(stderr, "Invalid number of operations %d\n", numops);
exit(EXIT_FAILURE);
}
NUMOPS = (unsigned) numops;
break;
}
case 'p' :
{
int poolsize = atoi(optarg);
if (poolsize < 4096)
{
fprintf(stderr, "Invalid pool size %d\n", poolsize);
exit(EXIT_FAILURE);
}
char c = optarg[strlen(optarg) - 1];
if (c == 'M')
{
POOLSIZE = (unsigned) poolsize * 1024 * 1024;
}
else if (c == 'K')
{
POOLSIZE = (unsigned) poolsize * 1024;
}
else
{
POOLSIZE = (unsigned) poolsize;
}
break;
}
default :
usage :
fprintf(stderr, "Usage: checksum <options>\n"
"-b <blksize> Block size\n"
"-d Dump first 96 bytes of data\n"
"-f <cpufreq> CPU frequency (Hz)\n"
"-i <impl> Implementation\n"
"-n <numops> Number of operations\n"
"-p <poolsize> Pool size (K or M suffix)\n"
);
printf("Implementations:");
for (int i = 0; implementations[i].name != NULL; i++)
{
printf(" %s", implementations[i].name);
}
printf("\n");
exit(EXIT_FAILURE);
}
}
if (optind > argc)
{
goto usage;
}

CKSUM_FP = implementations[IMPL].cksum_fp;
POOLSIZE = ALIGN(POOLSIZE, CACHE_LINE);
uint8_t *base = mmap(0, POOLSIZE, PROT_READ|PROT_WRITE,
MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
if (base == MAP_FAILED)
{
perror("aligned_alloc"), exit(EXIT_FAILURE);
}
for (size_t i = 0; i < POOLSIZE / 4; i++)
{
((uint32_t *) base)[i] = rand();
}

printf("Implementation: %s\n", implementations[IMPL].name);
printf("numops %u, poolsize ", NUMOPS);
if (POOLSIZE % (1024 * 1024) == 0)
{
printf("%uMiB", POOLSIZE / (1024 * 1024));
}
else if (POOLSIZE % 1024 == 0)
{
printf("%uKiB", POOLSIZE / 1024);
}
else
{
printf("%uB", POOLSIZE);
}
printf(", blocksize %u, CPU frequency %juMHz\n",
BLKSIZE, (uintmax_t) (CPUFREQ / 1000000));
#if WANT_ASSERT
printf("Warning: assertions are enabled\n");
#endif

if (DUMP)
{

for (int i = 0; i < 96; i++)
{
if (i % 8 == 0)
printf("%2u:", i);
printf(" %02x", base[i]);
if (i % 8 == 7)
printf("\n");
}
}


printf("Verifying..."); fflush(stdout);
bool success = true;

for (int size = 0; size <= 256; size++)
{
for (int offset = 0; offset < 255; offset++)
{

success &= verify(&base[offset], offset, size);

uint8_t *p = base + POOLSIZE - (size + offset);
success &= verify(p, (uintptr_t) p % 64, size);
}
}

for (size_t size = 1; size < POOLSIZE; size *= 2)
{
success &= verify(base, 0, size);
}

success &= verify(base, 0, POOLSIZE);
printf("%s\n", success ? "OK" : "failure");


if (CPUFREQ != 0)
{
printf("%11s %11s %11s %11s\n",
"block size", "MB/s", "cycles/blk", "cycles/byte");
}
else
{
printf("%11s %11s %11s %11s\n",
"block size", "MB/s", "ns/blk", "ns/byte");
CPUFREQ = 1000000000;
}
if (BLKSIZE != 0)
{
benchmark(base, POOLSIZE, BLKSIZE, NUMOPS, CPUFREQ);
}
else
{
static const uint16_t sizes[] =
{ 20, 42, 102, 250, 612, 1500, 3674, 9000, 0 };
for (int i = 0; sizes[i] != 0; i++)
{
uint32_t numops = NUMOPS * 10000 / (40 + sizes[i]);
benchmark(base, POOLSIZE, sizes[i], numops, CPUFREQ);
}
}

if (munmap(base, POOLSIZE) != 0)
{
perror("munmap"), exit(EXIT_FAILURE);
}

return success ? EXIT_SUCCESS : EXIT_FAILURE;
}
