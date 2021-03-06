





























#if defined(HAVE_CONFIG_H)
#include <config.h>
#endif

#include <pcap-types.h>
#if defined(_WIN32)
#include <io.h>
#include <fcntl.h>
#endif

#include <errno.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "pcap-int.h"

#include "pcap-common.h"

#if defined(HAVE_OS_PROTO_H)
#include "os-proto.h"
#endif

#include "sf-pcap.h"




#if defined(_WIN32)
#define SET_BINMODE(f) _setmode(_fileno(f), _O_BINARY)
#elif defined(MSDOS)
#if defined(__HIGHC__)
#define SET_BINMODE(f) setmode(f, O_BINARY)
#else
#define SET_BINMODE(f) setmode(fileno(f), O_BINARY)
#endif
#endif




#define TCPDUMP_MAGIC 0xa1b2c3d4




#define KUZNETZOV_TCPDUMP_MAGIC 0xa1b2cd34





#define FMESQUITA_TCPDUMP_MAGIC 0xa1b234cd





#define NAVTEL_TCPDUMP_MAGIC 0xa12b3c4d





#define NSEC_TCPDUMP_MAGIC 0xa1b23c4d










#define LT_LINKTYPE(x) ((x) & 0x03FFFFFF)
#define LT_LINKTYPE_EXT(x) ((x) & 0xFC000000)

static int pcap_next_packet(pcap_t *p, struct pcap_pkthdr *hdr, u_char **datap);

#if defined(_WIN32)










static pcap_dumper_t *pcap_dump_fopen(pcap_t *p, FILE *f);
#endif




typedef enum {
NOT_SWAPPED,
SWAPPED,
MAYBE_SWAPPED
} swapped_type_t;

typedef enum {
PASS_THROUGH,
SCALE_UP,
SCALE_DOWN
} tstamp_scale_type_t;

struct pcap_sf {
size_t hdrsize;
swapped_type_t lengths_swapped;
tstamp_scale_type_t scale_type;
};





pcap_t *
pcap_check_header(const uint8_t *magic, FILE *fp, u_int precision, char *errbuf,
int *err)
{
bpf_u_int32 magic_int;
struct pcap_file_header hdr;
size_t amt_read;
pcap_t *p;
int swapped = 0;
struct pcap_sf *ps;




*err = 0;






memcpy(&magic_int, magic, sizeof(magic_int));
if (magic_int != TCPDUMP_MAGIC &&
magic_int != KUZNETZOV_TCPDUMP_MAGIC &&
magic_int != NSEC_TCPDUMP_MAGIC) {
magic_int = SWAPLONG(magic_int);
if (magic_int != TCPDUMP_MAGIC &&
magic_int != KUZNETZOV_TCPDUMP_MAGIC &&
magic_int != NSEC_TCPDUMP_MAGIC)
return (NULL);
swapped = 1;
}





hdr.magic = magic_int;
amt_read = fread(((char *)&hdr) + sizeof hdr.magic, 1,
sizeof(hdr) - sizeof(hdr.magic), fp);
if (amt_read != sizeof(hdr) - sizeof(hdr.magic)) {
if (ferror(fp)) {
pcap_fmt_errmsg_for_errno(errbuf, PCAP_ERRBUF_SIZE,
errno, "error reading dump file");
} else {
pcap_snprintf(errbuf, PCAP_ERRBUF_SIZE,
"truncated dump file; tried to read %" PRIsize " file header bytes, only got %" PRIsize,
sizeof(hdr), amt_read);
}
*err = 1;
return (NULL);
}




if (swapped) {
hdr.version_major = SWAPSHORT(hdr.version_major);
hdr.version_minor = SWAPSHORT(hdr.version_minor);
hdr.thiszone = SWAPLONG(hdr.thiszone);
hdr.sigfigs = SWAPLONG(hdr.sigfigs);
hdr.snaplen = SWAPLONG(hdr.snaplen);
hdr.linktype = SWAPLONG(hdr.linktype);
}

if (hdr.version_major < PCAP_VERSION_MAJOR) {
pcap_snprintf(errbuf, PCAP_ERRBUF_SIZE,
"archaic pcap savefile format");
*err = 1;
return (NULL);
}





if (! ((hdr.version_major == PCAP_VERSION_MAJOR &&
hdr.version_minor <= PCAP_VERSION_MINOR) ||
(hdr.version_major == 543 &&
hdr.version_minor == 0))) {
pcap_snprintf(errbuf, PCAP_ERRBUF_SIZE,
"unsupported pcap savefile version %u.%u",
hdr.version_major, hdr.version_minor);
*err = 1;
return NULL;
}





p = pcap_open_offline_common(errbuf, sizeof (struct pcap_sf));
if (p == NULL) {

*err = 1;
return (NULL);
}
p->swapped = swapped;
p->version_major = hdr.version_major;
p->version_minor = hdr.version_minor;
p->tzoff = hdr.thiszone;
p->linktype = linktype_to_dlt(LT_LINKTYPE(hdr.linktype));
p->linktype_ext = LT_LINKTYPE_EXT(hdr.linktype);
p->snapshot = pcap_adjust_snapshot(p->linktype, hdr.snaplen);

p->next_packet_op = pcap_next_packet;

ps = p->priv;

p->opt.tstamp_precision = precision;





switch (precision) {

case PCAP_TSTAMP_PRECISION_MICRO:
if (magic_int == NSEC_TCPDUMP_MAGIC) {





ps->scale_type = SCALE_DOWN;
} else {




ps->scale_type = PASS_THROUGH;
}
break;

case PCAP_TSTAMP_PRECISION_NANO:
if (magic_int == NSEC_TCPDUMP_MAGIC) {




ps->scale_type = PASS_THROUGH;
} else {





ps->scale_type = SCALE_UP;
}
break;

default:
pcap_snprintf(errbuf, PCAP_ERRBUF_SIZE,
"unknown time stamp resolution %u", precision);
free(p);
*err = 1;
return (NULL);
}











switch (hdr.version_major) {

case 2:
if (hdr.version_minor < 3)
ps->lengths_swapped = SWAPPED;
else if (hdr.version_minor == 3)
ps->lengths_swapped = MAYBE_SWAPPED;
else
ps->lengths_swapped = NOT_SWAPPED;
break;

case 543:
ps->lengths_swapped = SWAPPED;
break;

default:
ps->lengths_swapped = NOT_SWAPPED;
break;
}

if (magic_int == KUZNETZOV_TCPDUMP_MAGIC) {

















ps->hdrsize = sizeof(struct pcap_sf_patched_pkthdr);

if (p->linktype == DLT_EN10MB) {























if (p->snapshot <= INT_MAX - 14)
p->snapshot += 14;
else
p->snapshot = INT_MAX;
}
} else
ps->hdrsize = sizeof(struct pcap_sf_pkthdr);










p->bufsize = p->snapshot;
if (p->bufsize > 2048)
p->bufsize = 2048;
p->buffer = malloc(p->bufsize);
if (p->buffer == NULL) {
pcap_snprintf(errbuf, PCAP_ERRBUF_SIZE, "out of memory");
free(p);
*err = 1;
return (NULL);
}

p->cleanup_op = sf_cleanup;

return (p);
}




static int
grow_buffer(pcap_t *p, u_int bufsize)
{
void *bigger_buffer;

bigger_buffer = realloc(p->buffer, bufsize);
if (bigger_buffer == NULL) {
pcap_snprintf(p->errbuf, PCAP_ERRBUF_SIZE, "out of memory");
return (0);
}
p->buffer = bigger_buffer;
p->bufsize = bufsize;
return (1);
}






static int
pcap_next_packet(pcap_t *p, struct pcap_pkthdr *hdr, u_char **data)
{
struct pcap_sf *ps = p->priv;
struct pcap_sf_patched_pkthdr sf_hdr;
FILE *fp = p->rfile;
size_t amt_read;
bpf_u_int32 t;








amt_read = fread(&sf_hdr, 1, ps->hdrsize, fp);
if (amt_read != ps->hdrsize) {
if (ferror(fp)) {
pcap_fmt_errmsg_for_errno(p->errbuf, PCAP_ERRBUF_SIZE,
errno, "error reading dump file");
return (-1);
} else {
if (amt_read != 0) {
pcap_snprintf(p->errbuf, PCAP_ERRBUF_SIZE,
"truncated dump file; tried to read %" PRIsize " header bytes, only got %" PRIsize,
ps->hdrsize, amt_read);
return (-1);
}

return (1);
}
}

if (p->swapped) {

hdr->caplen = SWAPLONG(sf_hdr.caplen);
hdr->len = SWAPLONG(sf_hdr.len);
hdr->ts.tv_sec = SWAPLONG(sf_hdr.ts.tv_sec);
hdr->ts.tv_usec = SWAPLONG(sf_hdr.ts.tv_usec);
} else {
hdr->caplen = sf_hdr.caplen;
hdr->len = sf_hdr.len;
hdr->ts.tv_sec = sf_hdr.ts.tv_sec;
hdr->ts.tv_usec = sf_hdr.ts.tv_usec;
}

switch (ps->scale_type) {

case PASS_THROUGH:



break;

case SCALE_UP:




hdr->ts.tv_usec = hdr->ts.tv_usec * 1000;
break;

case SCALE_DOWN:




hdr->ts.tv_usec = hdr->ts.tv_usec / 1000;
break;
}


switch (ps->lengths_swapped) {

case NOT_SWAPPED:
break;

case MAYBE_SWAPPED:
if (hdr->caplen <= hdr->len) {




break;
}


case SWAPPED:
t = hdr->caplen;
hdr->caplen = hdr->len;
hdr->len = t;
break;
}




if (hdr->caplen > max_snaplen_for_dlt(p->linktype)) {








if (hdr->caplen > (bpf_u_int32)p->snapshot) {
pcap_snprintf(p->errbuf, PCAP_ERRBUF_SIZE,
"invalid packet capture length %u, bigger than "
"snaplen of %d", hdr->caplen, p->snapshot);
} else {
pcap_snprintf(p->errbuf, PCAP_ERRBUF_SIZE,
"invalid packet capture length %u, bigger than "
"maximum of %u", hdr->caplen,
max_snaplen_for_dlt(p->linktype));
}
return (-1);
}

if (hdr->caplen > (bpf_u_int32)p->snapshot) {


































size_t bytes_to_discard;
size_t bytes_to_read, bytes_read;
char discard_buf[4096];

if (hdr->caplen > p->bufsize) {



if (!grow_buffer(p, p->snapshot))
return (-1);
}




amt_read = fread(p->buffer, 1, p->snapshot, fp);
if (amt_read != (bpf_u_int32)p->snapshot) {
if (ferror(fp)) {
pcap_fmt_errmsg_for_errno(p->errbuf,
PCAP_ERRBUF_SIZE, errno,
"error reading dump file");
} else {







pcap_snprintf(p->errbuf, PCAP_ERRBUF_SIZE,
"truncated dump file; tried to read %u captured bytes, only got %" PRIsize,
p->snapshot, amt_read);
}
return (-1);
}




bytes_to_discard = hdr->caplen - p->snapshot;
bytes_read = amt_read;
while (bytes_to_discard != 0) {
bytes_to_read = bytes_to_discard;
if (bytes_to_read > sizeof (discard_buf))
bytes_to_read = sizeof (discard_buf);
amt_read = fread(discard_buf, 1, bytes_to_read, fp);
bytes_read += amt_read;
if (amt_read != bytes_to_read) {
if (ferror(fp)) {
pcap_fmt_errmsg_for_errno(p->errbuf,
PCAP_ERRBUF_SIZE, errno,
"error reading dump file");
} else {
pcap_snprintf(p->errbuf, PCAP_ERRBUF_SIZE,
"truncated dump file; tried to read %u captured bytes, only got %" PRIsize,
hdr->caplen, bytes_read);
}
return (-1);
}
bytes_to_discard -= amt_read;
}





hdr->caplen = p->snapshot;
} else {



if (hdr->caplen > p->bufsize) {




u_int new_bufsize;

new_bufsize = hdr->caplen;



new_bufsize--;
new_bufsize |= new_bufsize >> 1;
new_bufsize |= new_bufsize >> 2;
new_bufsize |= new_bufsize >> 4;
new_bufsize |= new_bufsize >> 8;
new_bufsize |= new_bufsize >> 16;
new_bufsize++;

if (new_bufsize > (u_int)p->snapshot)
new_bufsize = p->snapshot;

if (!grow_buffer(p, new_bufsize))
return (-1);
}


amt_read = fread(p->buffer, 1, hdr->caplen, fp);
if (amt_read != hdr->caplen) {
if (ferror(fp)) {
pcap_fmt_errmsg_for_errno(p->errbuf,
PCAP_ERRBUF_SIZE, errno,
"error reading dump file");
} else {
pcap_snprintf(p->errbuf, PCAP_ERRBUF_SIZE,
"truncated dump file; tried to read %u captured bytes, only got %" PRIsize,
hdr->caplen, amt_read);
}
return (-1);
}
}
*data = p->buffer;

if (p->swapped)
swap_pseudo_headers(p->linktype, hdr, *data);

return (0);
}

static int
sf_write_header(pcap_t *p, FILE *fp, int linktype, int thiszone, int snaplen)
{
struct pcap_file_header hdr;

hdr.magic = p->opt.tstamp_precision == PCAP_TSTAMP_PRECISION_NANO ? NSEC_TCPDUMP_MAGIC : TCPDUMP_MAGIC;
hdr.version_major = PCAP_VERSION_MAJOR;
hdr.version_minor = PCAP_VERSION_MINOR;

hdr.thiszone = thiszone;
hdr.snaplen = snaplen;
hdr.sigfigs = 0;
hdr.linktype = linktype;

if (fwrite((char *)&hdr, sizeof(hdr), 1, fp) != 1)
return (-1);

return (0);
}




void
pcap_dump(u_char *user, const struct pcap_pkthdr *h, const u_char *sp)
{
register FILE *f;
struct pcap_sf_pkthdr sf_hdr;

f = (FILE *)user;
sf_hdr.ts.tv_sec = h->ts.tv_sec;
sf_hdr.ts.tv_usec = h->ts.tv_usec;
sf_hdr.caplen = h->caplen;
sf_hdr.len = h->len;

(void)fwrite(&sf_hdr, sizeof(sf_hdr), 1, f);
(void)fwrite(sp, h->caplen, 1, f);
}

static pcap_dumper_t *
pcap_setup_dump(pcap_t *p, int linktype, FILE *f, const char *fname)
{

#if defined(_WIN32) || defined(MSDOS)







if (f == stdout)
SET_BINMODE(f);
else
setvbuf(f, NULL, _IONBF, 0);
#endif
if (sf_write_header(p, f, linktype, p->tzoff, p->snapshot) == -1) {
pcap_fmt_errmsg_for_errno(p->errbuf, PCAP_ERRBUF_SIZE,
errno, "Can't write to %s", fname);
if (f != stdout)
(void)fclose(f);
return (NULL);
}
return ((pcap_dumper_t *)f);
}




pcap_dumper_t *
pcap_dump_open(pcap_t *p, const char *fname)
{
FILE *f;
int linktype;





if (!p->activated) {
pcap_snprintf(p->errbuf, PCAP_ERRBUF_SIZE,
"%s: not-yet-activated pcap_t passed to pcap_dump_open",
fname);
return (NULL);
}
linktype = dlt_to_linktype(p->linktype);
if (linktype == -1) {
pcap_snprintf(p->errbuf, PCAP_ERRBUF_SIZE,
"%s: link-layer type %d isn't supported in savefiles",
fname, p->linktype);
return (NULL);
}
linktype |= p->linktype_ext;

if (fname == NULL) {
pcap_snprintf(p->errbuf, PCAP_ERRBUF_SIZE,
"A null pointer was supplied as the file name");
return NULL;
}
if (fname[0] == '-' && fname[1] == '\0') {
f = stdout;
fname = "standard output";
} else {






f = fopen(fname, "wb");
if (f == NULL) {
pcap_fmt_errmsg_for_errno(p->errbuf, PCAP_ERRBUF_SIZE,
errno, "%s", fname);
return (NULL);
}
}
return (pcap_setup_dump(p, linktype, f, fname));
}

#if defined(_WIN32)




pcap_dumper_t *
pcap_dump_hopen(pcap_t *p, intptr_t osfd)
{
int fd;
FILE *file;

fd = _open_osfhandle(osfd, _O_APPEND);
if (fd < 0) {
pcap_fmt_errmsg_for_errno(p->errbuf, PCAP_ERRBUF_SIZE,
errno, "_open_osfhandle");
return NULL;
}

file = _fdopen(fd, "wb");
if (file == NULL) {
pcap_fmt_errmsg_for_errno(p->errbuf, PCAP_ERRBUF_SIZE,
errno, "_fdopen");
_close(fd);
return NULL;
}

return pcap_dump_fopen(p, file);
}
#endif




#if defined(_WIN32)
static
#endif
pcap_dumper_t *
pcap_dump_fopen(pcap_t *p, FILE *f)
{
int linktype;

linktype = dlt_to_linktype(p->linktype);
if (linktype == -1) {
pcap_snprintf(p->errbuf, PCAP_ERRBUF_SIZE,
"stream: link-layer type %d isn't supported in savefiles",
p->linktype);
return (NULL);
}
linktype |= p->linktype_ext;

return (pcap_setup_dump(p, linktype, f, "stream"));
}

pcap_dumper_t *
pcap_dump_open_append(pcap_t *p, const char *fname)
{
FILE *f;
int linktype;
size_t amt_read;
struct pcap_file_header ph;

linktype = dlt_to_linktype(p->linktype);
if (linktype == -1) {
pcap_snprintf(p->errbuf, PCAP_ERRBUF_SIZE,
"%s: link-layer type %d isn't supported in savefiles",
fname, linktype);
return (NULL);
}

if (fname == NULL) {
pcap_snprintf(p->errbuf, PCAP_ERRBUF_SIZE,
"A null pointer was supplied as the file name");
return NULL;
}
if (fname[0] == '-' && fname[1] == '\0')
return (pcap_setup_dump(p, linktype, stdout, "standard output"));














f = fopen(fname, "ab+");
if (f == NULL) {
pcap_fmt_errmsg_for_errno(p->errbuf, PCAP_ERRBUF_SIZE,
errno, "%s", fname);
return (NULL);
}













if (fseek(f, 0, SEEK_SET) == -1) {
pcap_fmt_errmsg_for_errno(p->errbuf, PCAP_ERRBUF_SIZE,
errno, "Can't seek to the beginning of %s", fname);
(void)fclose(f);
return (NULL);
}
amt_read = fread(&ph, 1, sizeof (ph), f);
if (amt_read != sizeof (ph)) {
if (ferror(f)) {
pcap_fmt_errmsg_for_errno(p->errbuf, PCAP_ERRBUF_SIZE,
errno, "%s", fname);
(void)fclose(f);
return (NULL);
} else if (feof(f) && amt_read > 0) {
pcap_snprintf(p->errbuf, PCAP_ERRBUF_SIZE,
"%s: truncated pcap file header", fname);
(void)fclose(f);
return (NULL);
}
}

#if defined(_WIN32) || defined(MSDOS)




setvbuf(f, NULL, _IONBF, 0);
#endif













if (amt_read > 0) {




switch (ph.magic) {

case TCPDUMP_MAGIC:
if (p->opt.tstamp_precision != PCAP_TSTAMP_PRECISION_MICRO) {
pcap_snprintf(p->errbuf, PCAP_ERRBUF_SIZE,
"%s: different time stamp precision, cannot append to file", fname);
(void)fclose(f);
return (NULL);
}
break;

case NSEC_TCPDUMP_MAGIC:
if (p->opt.tstamp_precision != PCAP_TSTAMP_PRECISION_NANO) {
pcap_snprintf(p->errbuf, PCAP_ERRBUF_SIZE,
"%s: different time stamp precision, cannot append to file", fname);
(void)fclose(f);
return (NULL);
}
break;

case SWAPLONG(TCPDUMP_MAGIC):
case SWAPLONG(NSEC_TCPDUMP_MAGIC):
pcap_snprintf(p->errbuf, PCAP_ERRBUF_SIZE,
"%s: different byte order, cannot append to file", fname);
(void)fclose(f);
return (NULL);

case KUZNETZOV_TCPDUMP_MAGIC:
case SWAPLONG(KUZNETZOV_TCPDUMP_MAGIC):
case NAVTEL_TCPDUMP_MAGIC:
case SWAPLONG(NAVTEL_TCPDUMP_MAGIC):
pcap_snprintf(p->errbuf, PCAP_ERRBUF_SIZE,
"%s: not a pcap file to which we can append", fname);
(void)fclose(f);
return (NULL);

default:
pcap_snprintf(p->errbuf, PCAP_ERRBUF_SIZE,
"%s: not a pcap file", fname);
(void)fclose(f);
return (NULL);
}




if (ph.version_major != PCAP_VERSION_MAJOR ||
ph.version_minor != PCAP_VERSION_MINOR) {
pcap_snprintf(p->errbuf, PCAP_ERRBUF_SIZE,
"%s: version is %u.%u, cannot append to file", fname,
ph.version_major, ph.version_minor);
(void)fclose(f);
return (NULL);
}
if ((bpf_u_int32)linktype != ph.linktype) {
pcap_snprintf(p->errbuf, PCAP_ERRBUF_SIZE,
"%s: different linktype, cannot append to file", fname);
(void)fclose(f);
return (NULL);
}
if ((bpf_u_int32)p->snapshot != ph.snaplen) {
pcap_snprintf(p->errbuf, PCAP_ERRBUF_SIZE,
"%s: different snaplen, cannot append to file", fname);
(void)fclose(f);
return (NULL);
}
} else {



if (sf_write_header(p, f, linktype, p->tzoff, p->snapshot) == -1) {
pcap_fmt_errmsg_for_errno(p->errbuf, PCAP_ERRBUF_SIZE,
errno, "Can't write to %s", fname);
(void)fclose(f);
return (NULL);
}
}








if (fseek(f, 0, SEEK_END) == -1) {
pcap_fmt_errmsg_for_errno(p->errbuf, PCAP_ERRBUF_SIZE,
errno, "Can't seek to the end of %s", fname);
(void)fclose(f);
return (NULL);
}
return ((pcap_dumper_t *)f);
}

FILE *
pcap_dump_file(pcap_dumper_t *p)
{
return ((FILE *)p);
}

long
pcap_dump_ftell(pcap_dumper_t *p)
{
return (ftell((FILE *)p));
}

#if defined(HAVE_FSEEKO)






int64_t
pcap_dump_ftell64(pcap_dumper_t *p)
{
return (ftello((FILE *)p));
}
#elif defined(_MSC_VER)




int64_t
pcap_dump_ftell64(pcap_dumper_t *p)
{
return (_ftelli64((FILE *)p));
}
#else









int64_t
pcap_dump_ftell64(pcap_dumper_t *p)
{
return (ftell((FILE *)p));
}
#endif

int
pcap_dump_flush(pcap_dumper_t *p)
{

if (fflush((FILE *)p) == EOF)
return (-1);
else
return (0);
}

void
pcap_dump_close(pcap_dumper_t *p)
{

#if defined(notyet)
if (ferror((FILE *)p))
return-an-error;

#endif
(void)fclose((FILE *)p);
}
