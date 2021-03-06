






















#if defined(HAVE_CONFIG_H)
#include <config.h>
#endif

#include <pcap/pcap-inttypes.h>

#include <errno.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pcap-int.h"

#include "pcap-common.h"

#if defined(HAVE_OS_PROTO_H)
#include "os-proto.h"
#endif

#include "sf-pcapng.h"








struct block_header {
bpf_u_int32 block_type;
bpf_u_int32 total_length;
};




struct block_trailer {
bpf_u_int32 total_length;
};




#define OPT_ENDOFOPT 0
#define OPT_COMMENT 1




struct option_header {
u_short option_code;
u_short option_length;
};









#define BT_SHB 0x0A0D0D0A
#define BT_SHB_INSANE_MAX 1024U*1024U*1U
struct section_header_block {
bpf_u_int32 byte_order_magic;
u_short major_version;
u_short minor_version;
uint64_t section_length;

};




#define BYTE_ORDER_MAGIC 0x1A2B3C4D





#define PCAP_NG_VERSION_MAJOR 1
#define PCAP_NG_VERSION_MINOR 0




#define BT_IDB 0x00000001

struct interface_description_block {
u_short linktype;
u_short reserved;
bpf_u_int32 snaplen;

};




#define IF_NAME 2
#define IF_DESCRIPTION 3
#define IF_IPV4ADDR 4
#define IF_IPV6ADDR 5
#define IF_MACADDR 6
#define IF_EUIADDR 7
#define IF_SPEED 8
#define IF_TSRESOL 9
#define IF_TZONE 10
#define IF_FILTER 11
#define IF_OS 12
#define IF_FCSLEN 13
#define IF_TSOFFSET 14




#define BT_EPB 0x00000006

struct enhanced_packet_block {
bpf_u_int32 interface_id;
bpf_u_int32 timestamp_high;
bpf_u_int32 timestamp_low;
bpf_u_int32 caplen;
bpf_u_int32 len;

};




#define BT_SPB 0x00000003

struct simple_packet_block {
bpf_u_int32 len;

};




#define BT_PB 0x00000002

struct packet_block {
u_short interface_id;
u_short drops_count;
bpf_u_int32 timestamp_high;
bpf_u_int32 timestamp_low;
bpf_u_int32 caplen;
bpf_u_int32 len;

};






struct block_cursor {
u_char *data;
size_t data_remaining;
bpf_u_int32 block_type;
};

typedef enum {
PASS_THROUGH,
SCALE_UP_DEC,
SCALE_DOWN_DEC,
SCALE_UP_BIN,
SCALE_DOWN_BIN
} tstamp_scale_type_t;




struct pcap_ng_if {
uint64_t tsresol;
tstamp_scale_type_t scale_type;
uint64_t scale_factor;
uint64_t tsoffset;
};




















struct pcap_ng_sf {
uint64_t user_tsresol;
u_int max_blocksize;
bpf_u_int32 ifcount;
bpf_u_int32 ifaces_size;
struct pcap_ng_if *ifaces;
};





#define INITIAL_MAX_BLOCKSIZE (16*1024*1024)






#define MAX_BLOCKSIZE_FOR_SNAPLEN(max_snaplen) (sizeof (struct block_header) + sizeof (struct enhanced_packet_block) + (max_snaplen) + 131072 + sizeof (struct block_trailer))





static void pcap_ng_cleanup(pcap_t *p);
static int pcap_ng_next_packet(pcap_t *p, struct pcap_pkthdr *hdr,
u_char **data);

static int
read_bytes(FILE *fp, void *buf, size_t bytes_to_read, int fail_on_eof,
char *errbuf)
{
size_t amt_read;

amt_read = fread(buf, 1, bytes_to_read, fp);
if (amt_read != bytes_to_read) {
if (ferror(fp)) {
pcap_fmt_errmsg_for_errno(errbuf, PCAP_ERRBUF_SIZE,
errno, "error reading dump file");
} else {
if (amt_read == 0 && !fail_on_eof)
return (0);
pcap_snprintf(errbuf, PCAP_ERRBUF_SIZE,
"truncated pcapng dump file; tried to read %" PRIsize " bytes, only got %" PRIsize,
bytes_to_read, amt_read);
}
return (-1);
}
return (1);
}

static int
read_block(FILE *fp, pcap_t *p, struct block_cursor *cursor, char *errbuf)
{
struct pcap_ng_sf *ps;
int status;
struct block_header bhdr;
struct block_trailer *btrlr;
u_char *bdata;
size_t data_remaining;

ps = p->priv;

status = read_bytes(fp, &bhdr, sizeof(bhdr), 0, errbuf);
if (status <= 0)
return (status);

if (p->swapped) {
bhdr.block_type = SWAPLONG(bhdr.block_type);
bhdr.total_length = SWAPLONG(bhdr.total_length);
}





if (bhdr.total_length < sizeof(struct block_header) +
sizeof(struct block_trailer)) {
pcap_snprintf(errbuf, PCAP_ERRBUF_SIZE,
"block in pcapng dump file has a length of %u < %" PRIsize,
bhdr.total_length,
sizeof(struct block_header) + sizeof(struct block_trailer));
return (-1);
}




if ((bhdr.total_length % 4) != 0) {



pcap_snprintf(errbuf, PCAP_ERRBUF_SIZE,
"block in pcapng dump file has a length of %u that is not a multiple of 4" PRIsize,
bhdr.total_length);
return (-1);
}




if (p->bufsize < bhdr.total_length) {




void *bigger_buffer;

if (bhdr.total_length > ps->max_blocksize) {
pcap_snprintf(errbuf, PCAP_ERRBUF_SIZE, "pcapng block size %u > maximum %u", bhdr.total_length,
ps->max_blocksize);
return (-1);
}
bigger_buffer = realloc(p->buffer, bhdr.total_length);
if (bigger_buffer == NULL) {
pcap_snprintf(errbuf, PCAP_ERRBUF_SIZE, "out of memory");
return (-1);
}
p->buffer = bigger_buffer;
}





memcpy(p->buffer, &bhdr, sizeof(bhdr));
bdata = (u_char *)p->buffer + sizeof(bhdr);
data_remaining = bhdr.total_length - sizeof(bhdr);
if (read_bytes(fp, bdata, data_remaining, 1, errbuf) == -1)
return (-1);




btrlr = (struct block_trailer *)(bdata + data_remaining - sizeof (struct block_trailer));
if (p->swapped)
btrlr->total_length = SWAPLONG(btrlr->total_length);





if (bhdr.total_length != btrlr->total_length) {



pcap_snprintf(errbuf, PCAP_ERRBUF_SIZE,
"block total length in header and trailer don't match");
return (-1);
}




cursor->data = bdata;
cursor->data_remaining = data_remaining - sizeof(struct block_trailer);
cursor->block_type = bhdr.block_type;
return (1);
}

static void *
get_from_block_data(struct block_cursor *cursor, size_t chunk_size,
char *errbuf)
{
void *data;





if (cursor->data_remaining < chunk_size) {
pcap_snprintf(errbuf, PCAP_ERRBUF_SIZE,
"block of type %u in pcapng dump file is too short",
cursor->block_type);
return (NULL);
}




data = cursor->data;
cursor->data += chunk_size;
cursor->data_remaining -= chunk_size;
return (data);
}

static struct option_header *
get_opthdr_from_block_data(pcap_t *p, struct block_cursor *cursor, char *errbuf)
{
struct option_header *opthdr;

opthdr = get_from_block_data(cursor, sizeof(*opthdr), errbuf);
if (opthdr == NULL) {



return (NULL);
}




if (p->swapped) {
opthdr->option_code = SWAPSHORT(opthdr->option_code);
opthdr->option_length = SWAPSHORT(opthdr->option_length);
}

return (opthdr);
}

static void *
get_optvalue_from_block_data(struct block_cursor *cursor,
struct option_header *opthdr, char *errbuf)
{
size_t padded_option_len;
void *optvalue;


padded_option_len = opthdr->option_length;
padded_option_len = ((padded_option_len + 3)/4)*4;

optvalue = get_from_block_data(cursor, padded_option_len, errbuf);
if (optvalue == NULL) {



return (NULL);
}

return (optvalue);
}

static int
process_idb_options(pcap_t *p, struct block_cursor *cursor, uint64_t *tsresol,
uint64_t *tsoffset, int *is_binary, char *errbuf)
{
struct option_header *opthdr;
void *optvalue;
int saw_tsresol, saw_tsoffset;
uint8_t tsresol_opt;
u_int i;

saw_tsresol = 0;
saw_tsoffset = 0;
while (cursor->data_remaining != 0) {



opthdr = get_opthdr_from_block_data(p, cursor, errbuf);
if (opthdr == NULL) {



return (-1);
}




optvalue = get_optvalue_from_block_data(cursor, opthdr,
errbuf);
if (optvalue == NULL) {



return (-1);
}

switch (opthdr->option_code) {

case OPT_ENDOFOPT:
if (opthdr->option_length != 0) {
pcap_snprintf(errbuf, PCAP_ERRBUF_SIZE,
"Interface Description Block has opt_endofopt option with length %u != 0",
opthdr->option_length);
return (-1);
}
goto done;

case IF_TSRESOL:
if (opthdr->option_length != 1) {
pcap_snprintf(errbuf, PCAP_ERRBUF_SIZE,
"Interface Description Block has if_tsresol option with length %u != 1",
opthdr->option_length);
return (-1);
}
if (saw_tsresol) {
pcap_snprintf(errbuf, PCAP_ERRBUF_SIZE,
"Interface Description Block has more than one if_tsresol option");
return (-1);
}
saw_tsresol = 1;
memcpy(&tsresol_opt, optvalue, sizeof(tsresol_opt));
if (tsresol_opt & 0x80) {



uint8_t tsresol_shift = (tsresol_opt & 0x7F);

if (tsresol_shift > 63) {




pcap_snprintf(errbuf, PCAP_ERRBUF_SIZE,
"Interface Description Block if_tsresol option resolution 2^-%u is too high",
tsresol_shift);
return (-1);
}
*is_binary = 1;
*tsresol = ((uint64_t)1) << tsresol_shift;
} else {



if (tsresol_opt > 19) {








pcap_snprintf(errbuf, PCAP_ERRBUF_SIZE,
"Interface Description Block if_tsresol option resolution 10^-%u is too high",
tsresol_opt);
return (-1);
}
*is_binary = 0;
*tsresol = 1;
for (i = 0; i < tsresol_opt; i++)
*tsresol *= 10;
}
break;

case IF_TSOFFSET:
if (opthdr->option_length != 8) {
pcap_snprintf(errbuf, PCAP_ERRBUF_SIZE,
"Interface Description Block has if_tsoffset option with length %u != 8",
opthdr->option_length);
return (-1);
}
if (saw_tsoffset) {
pcap_snprintf(errbuf, PCAP_ERRBUF_SIZE,
"Interface Description Block has more than one if_tsoffset option");
return (-1);
}
saw_tsoffset = 1;
memcpy(tsoffset, optvalue, sizeof(*tsoffset));
if (p->swapped)
*tsoffset = SWAPLL(*tsoffset);
break;

default:
break;
}
}

done:
return (0);
}

static int
add_interface(pcap_t *p, struct block_cursor *cursor, char *errbuf)
{
struct pcap_ng_sf *ps;
uint64_t tsresol;
uint64_t tsoffset;
int is_binary;

ps = p->priv;




ps->ifcount++;




if (ps->ifcount > ps->ifaces_size) {



bpf_u_int32 new_ifaces_size;
struct pcap_ng_if *new_ifaces;

if (ps->ifaces_size == 0) {
















new_ifaces_size = 1;
new_ifaces = malloc(sizeof (struct pcap_ng_if));
} else {






if (ps->ifaces_size * 2 < ps->ifaces_size) {






pcap_snprintf(errbuf, PCAP_ERRBUF_SIZE,
"more than %u interfaces in the file",
0x80000000U);
return (0);
}





new_ifaces_size = ps->ifaces_size * 2;










if (new_ifaces_size * sizeof (struct pcap_ng_if) < new_ifaces_size) {









pcap_snprintf(errbuf, PCAP_ERRBUF_SIZE,
"more than %u interfaces in the file",
0xFFFFFFFFU / ((u_int)sizeof (struct pcap_ng_if)));
return (0);
}
new_ifaces = realloc(ps->ifaces, new_ifaces_size * sizeof (struct pcap_ng_if));
}
if (new_ifaces == NULL) {




pcap_snprintf(errbuf, PCAP_ERRBUF_SIZE,
"out of memory for per-interface information (%u interfaces)",
ps->ifcount);
return (0);
}
ps->ifaces_size = new_ifaces_size;
ps->ifaces = new_ifaces;
}




tsresol = 1000000;
is_binary = 0;
tsoffset = 0;





if (process_idb_options(p, cursor, &tsresol, &tsoffset, &is_binary,
errbuf) == -1)
return (0);

ps->ifaces[ps->ifcount - 1].tsresol = tsresol;
ps->ifaces[ps->ifcount - 1].tsoffset = tsoffset;





if (tsresol == ps->user_tsresol) {




ps->ifaces[ps->ifcount - 1].scale_type = PASS_THROUGH;
} else if (tsresol > ps->user_tsresol) {




if (is_binary)
ps->ifaces[ps->ifcount - 1].scale_type = SCALE_DOWN_BIN;
else {



ps->ifaces[ps->ifcount - 1].scale_factor = tsresol/ps->user_tsresol;
ps->ifaces[ps->ifcount - 1].scale_type = SCALE_DOWN_DEC;
}
} else {




if (is_binary)
ps->ifaces[ps->ifcount - 1].scale_type = SCALE_UP_BIN;
else {



ps->ifaces[ps->ifcount - 1].scale_factor = ps->user_tsresol/tsresol;
ps->ifaces[ps->ifcount - 1].scale_type = SCALE_UP_DEC;
}
}
return (1);
}





pcap_t *
pcap_ng_check_header(const uint8_t *magic, FILE *fp, u_int precision,
char *errbuf, int *err)
{
bpf_u_int32 magic_int;
size_t amt_read;
bpf_u_int32 total_length;
bpf_u_int32 byte_order_magic;
struct block_header *bhdrp;
struct section_header_block *shbp;
pcap_t *p;
int swapped = 0;
struct pcap_ng_sf *ps;
int status;
struct block_cursor cursor;
struct interface_description_block *idbp;




*err = 0;





memcpy(&magic_int, magic, sizeof(magic_int));
if (magic_int != BT_SHB) {









return (NULL);
}












amt_read = fread(&total_length, 1, sizeof(total_length), fp);
if (amt_read < sizeof(total_length)) {
if (ferror(fp)) {
pcap_fmt_errmsg_for_errno(errbuf, PCAP_ERRBUF_SIZE,
errno, "error reading dump file");
*err = 1;
return (NULL);
}





return (NULL);
}
amt_read = fread(&byte_order_magic, 1, sizeof(byte_order_magic), fp);
if (amt_read < sizeof(byte_order_magic)) {
if (ferror(fp)) {
pcap_fmt_errmsg_for_errno(errbuf, PCAP_ERRBUF_SIZE,
errno, "error reading dump file");
*err = 1;
return (NULL);
}





return (NULL);
}
if (byte_order_magic != BYTE_ORDER_MAGIC) {
byte_order_magic = SWAPLONG(byte_order_magic);
if (byte_order_magic != BYTE_ORDER_MAGIC) {



return (NULL);
}
swapped = 1;
total_length = SWAPLONG(total_length);
}




if (total_length < sizeof(*bhdrp) + sizeof(*shbp) + sizeof(struct block_trailer) ||
(total_length > BT_SHB_INSANE_MAX)) {
pcap_snprintf(errbuf, PCAP_ERRBUF_SIZE,
"Section Header Block in pcapng dump file has invalid length %" PRIsize " < _%u_ < %u (BT_SHB_INSANE_MAX)",
sizeof(*bhdrp) + sizeof(*shbp) + sizeof(struct block_trailer),
total_length,
BT_SHB_INSANE_MAX);

*err = 1;
return (NULL);
}





p = pcap_open_offline_common(errbuf, sizeof (struct pcap_ng_sf));
if (p == NULL) {

*err = 1;
return (NULL);
}
p->swapped = swapped;
ps = p->priv;




switch (precision) {

case PCAP_TSTAMP_PRECISION_MICRO:
ps->user_tsresol = 1000000;
break;

case PCAP_TSTAMP_PRECISION_NANO:
ps->user_tsresol = 1000000000;
break;

default:
pcap_snprintf(errbuf, PCAP_ERRBUF_SIZE,
"unknown time stamp resolution %u", precision);
free(p);
*err = 1;
return (NULL);
}

p->opt.tstamp_precision = precision;

















p->bufsize = 2048;
if (p->bufsize < total_length)
p->bufsize = total_length;
p->buffer = malloc(p->bufsize);
if (p->buffer == NULL) {
pcap_snprintf(errbuf, PCAP_ERRBUF_SIZE, "out of memory");
free(p);
*err = 1;
return (NULL);
}
ps->max_blocksize = INITIAL_MAX_BLOCKSIZE;





bhdrp = (struct block_header *)p->buffer;
shbp = (struct section_header_block *)((u_char *)p->buffer + sizeof(struct block_header));
bhdrp->block_type = magic_int;
bhdrp->total_length = total_length;
shbp->byte_order_magic = byte_order_magic;
if (read_bytes(fp,
(u_char *)p->buffer + (sizeof(magic_int) + sizeof(total_length) + sizeof(byte_order_magic)),
total_length - (sizeof(magic_int) + sizeof(total_length) + sizeof(byte_order_magic)),
1, errbuf) == -1)
goto fail;

if (p->swapped) {



shbp->major_version = SWAPSHORT(shbp->major_version);
shbp->minor_version = SWAPSHORT(shbp->minor_version);




}

if (! (shbp->major_version == PCAP_NG_VERSION_MAJOR &&
shbp->minor_version == PCAP_NG_VERSION_MINOR)) {
pcap_snprintf(errbuf, PCAP_ERRBUF_SIZE,
"unsupported pcapng savefile version %u.%u",
shbp->major_version, shbp->minor_version);
goto fail;
}
p->version_major = shbp->major_version;
p->version_minor = shbp->minor_version;




p->opt.tstamp_precision = precision;




for (;;) {



status = read_block(fp, p, &cursor, errbuf);
if (status == 0) {

pcap_snprintf(errbuf, PCAP_ERRBUF_SIZE,
"the capture file has no Interface Description Blocks");
goto fail;
}
if (status == -1)
goto fail;
switch (cursor.block_type) {

case BT_IDB:




idbp = get_from_block_data(&cursor, sizeof(*idbp),
errbuf);
if (idbp == NULL)
goto fail;




if (p->swapped) {
idbp->linktype = SWAPSHORT(idbp->linktype);
idbp->snaplen = SWAPLONG(idbp->snaplen);
}




if (!add_interface(p, &cursor, errbuf))
goto fail;

goto done;

case BT_EPB:
case BT_SPB:
case BT_PB:





pcap_snprintf(errbuf, PCAP_ERRBUF_SIZE,
"the capture file has a packet block before any Interface Description Blocks");
goto fail;

default:



break;
}
}

done:
p->tzoff = 0;
p->linktype = linktype_to_dlt(idbp->linktype);
p->snapshot = pcap_adjust_snapshot(p->linktype, idbp->snaplen);
p->linktype_ext = 0;






if (MAX_BLOCKSIZE_FOR_SNAPLEN(max_snaplen_for_dlt(p->linktype)) > ps->max_blocksize)
ps->max_blocksize = MAX_BLOCKSIZE_FOR_SNAPLEN(max_snaplen_for_dlt(p->linktype));

p->next_packet_op = pcap_ng_next_packet;
p->cleanup_op = pcap_ng_cleanup;

return (p);

fail:
free(ps->ifaces);
free(p->buffer);
free(p);
*err = 1;
return (NULL);
}

static void
pcap_ng_cleanup(pcap_t *p)
{
struct pcap_ng_sf *ps = p->priv;

free(ps->ifaces);
sf_cleanup(p);
}






static int
pcap_ng_next_packet(pcap_t *p, struct pcap_pkthdr *hdr, u_char **data)
{
struct pcap_ng_sf *ps = p->priv;
struct block_cursor cursor;
int status;
struct enhanced_packet_block *epbp;
struct simple_packet_block *spbp;
struct packet_block *pbp;
bpf_u_int32 interface_id = 0xFFFFFFFF;
struct interface_description_block *idbp;
struct section_header_block *shbp;
FILE *fp = p->rfile;
uint64_t t, sec, frac;





for (;;) {




status = read_block(fp, p, &cursor, p->errbuf);
if (status == 0)
return (1);
if (status == -1)
return (-1);
switch (cursor.block_type) {

case BT_EPB:




epbp = get_from_block_data(&cursor, sizeof(*epbp),
p->errbuf);
if (epbp == NULL)
return (-1);




if (p->swapped) {

interface_id = SWAPLONG(epbp->interface_id);
hdr->caplen = SWAPLONG(epbp->caplen);
hdr->len = SWAPLONG(epbp->len);
t = ((uint64_t)SWAPLONG(epbp->timestamp_high)) << 32 |
SWAPLONG(epbp->timestamp_low);
} else {
interface_id = epbp->interface_id;
hdr->caplen = epbp->caplen;
hdr->len = epbp->len;
t = ((uint64_t)epbp->timestamp_high) << 32 |
epbp->timestamp_low;
}
goto found;

case BT_SPB:




spbp = get_from_block_data(&cursor, sizeof(*spbp),
p->errbuf);
if (spbp == NULL)
return (-1);





interface_id = 0;




if (p->swapped) {

hdr->len = SWAPLONG(spbp->len);
} else
hdr->len = spbp->len;






hdr->caplen = hdr->len;
if (hdr->caplen > (bpf_u_int32)p->snapshot)
hdr->caplen = p->snapshot;
t = 0;
goto found;

case BT_PB:




pbp = get_from_block_data(&cursor, sizeof(*pbp),
p->errbuf);
if (pbp == NULL)
return (-1);




if (p->swapped) {

interface_id = SWAPSHORT(pbp->interface_id);
hdr->caplen = SWAPLONG(pbp->caplen);
hdr->len = SWAPLONG(pbp->len);
t = ((uint64_t)SWAPLONG(pbp->timestamp_high)) << 32 |
SWAPLONG(pbp->timestamp_low);
} else {
interface_id = pbp->interface_id;
hdr->caplen = pbp->caplen;
hdr->len = pbp->len;
t = ((uint64_t)pbp->timestamp_high) << 32 |
pbp->timestamp_low;
}
goto found;

case BT_IDB:




idbp = get_from_block_data(&cursor, sizeof(*idbp),
p->errbuf);
if (idbp == NULL)
return (-1);




if (p->swapped) {
idbp->linktype = SWAPSHORT(idbp->linktype);
idbp->snaplen = SWAPLONG(idbp->snaplen);
}









if (p->linktype != idbp->linktype) {
pcap_snprintf(p->errbuf, PCAP_ERRBUF_SIZE,
"an interface has a type %u different from the type of the first interface",
idbp->linktype);
return (-1);
}





if ((bpf_u_int32)p->snapshot !=
pcap_adjust_snapshot(p->linktype, idbp->snaplen)) {
pcap_snprintf(p->errbuf, PCAP_ERRBUF_SIZE,
"an interface has a snapshot length %u different from the type of the first interface",
idbp->snaplen);
return (-1);
}




if (!add_interface(p, &cursor, p->errbuf))
return (-1);
break;

case BT_SHB:




shbp = get_from_block_data(&cursor, sizeof(*shbp),
p->errbuf);
if (shbp == NULL)
return (-1);






if (p->swapped) {
shbp->byte_order_magic =
SWAPLONG(shbp->byte_order_magic);
shbp->major_version =
SWAPSHORT(shbp->major_version);
}






switch (shbp->byte_order_magic) {

case BYTE_ORDER_MAGIC:



break;

case SWAPLONG(BYTE_ORDER_MAGIC):



pcap_snprintf(p->errbuf, PCAP_ERRBUF_SIZE,
"the file has sections with different byte orders");
return (-1);

default:



pcap_snprintf(p->errbuf, PCAP_ERRBUF_SIZE,
"the file has a section with a bad byte order magic field");
return (-1);
}





if (shbp->major_version != PCAP_NG_VERSION_MAJOR) {
pcap_snprintf(p->errbuf, PCAP_ERRBUF_SIZE,
"unknown pcapng savefile major version number %u",
shbp->major_version);
return (-1);
}










ps->ifcount = 0;
break;

default:



break;
}
}

found:



if (interface_id >= ps->ifcount) {



pcap_snprintf(p->errbuf, PCAP_ERRBUF_SIZE,
"a packet arrived on interface %u, but there's no Interface Description Block for that interface",
interface_id);
return (-1);
}

if (hdr->caplen > (bpf_u_int32)p->snapshot) {
pcap_snprintf(p->errbuf, PCAP_ERRBUF_SIZE,
"invalid packet capture length %u, bigger than "
"snaplen of %d", hdr->caplen, p->snapshot);
return (-1);
}





sec = t / ps->ifaces[interface_id].tsresol + ps->ifaces[interface_id].tsoffset;
frac = t % ps->ifaces[interface_id].tsresol;





switch (ps->ifaces[interface_id].scale_type) {

case PASS_THROUGH:




break;

case SCALE_UP_DEC:













frac *= ps->ifaces[interface_id].scale_factor;
break;

case SCALE_UP_BIN:



















frac *= ps->user_tsresol;
frac /= ps->ifaces[interface_id].tsresol;
break;

case SCALE_DOWN_DEC:















frac /= ps->ifaces[interface_id].scale_factor;
break;


case SCALE_DOWN_BIN:






















frac *= ps->user_tsresol;
frac /= ps->ifaces[interface_id].tsresol;
break;
}
#if defined(_WIN32)




hdr->ts.tv_sec = (long)sec;
hdr->ts.tv_usec = (long)frac;
#else






hdr->ts.tv_sec = (time_t)sec;
hdr->ts.tv_usec = (int)frac;
#endif




*data = get_from_block_data(&cursor, hdr->caplen, p->errbuf);
if (*data == NULL)
return (-1);

if (p->swapped)
swap_pseudo_headers(p->linktype, hdr, *data);

return (0);
}
