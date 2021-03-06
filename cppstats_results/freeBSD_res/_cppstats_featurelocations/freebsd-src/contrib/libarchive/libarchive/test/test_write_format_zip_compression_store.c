




























#include "test.h"
__FBSDID("$FreeBSD: head/lib/libarchive/test/test_write_format_zip_no_compression.c 201247 2009-12-30 05:59:21Z kientzle $");


static const char file_name[] = "file";
static const char file_data1[] = {'1', '2', '3', '4', '5'};
static const char file_data2[] = {'6', '7', '8', '9', '0'};
static const int file_perm = 00644;
static const short file_uid = 10;
static const short file_gid = 20;


static const char folder_name[] = "folder/";
static const int folder_perm = 00755;
static const short folder_uid = 30;
static const short folder_gid = 40;

static time_t now;

static unsigned long
bitcrc32(unsigned long c, const void *_p, size_t s)
{








const unsigned char *p = _p;
int bitctr;

if (p == NULL)
return (0);

for (; s > 0; --s) {
c ^= *p++;
for (bitctr = 8; bitctr > 0; --bitctr) {
if (c & 1) c = (c >> 1);
else c = (c >> 1) ^ 0xedb88320;
c ^= 0x80000000;
}
}
return (c);
}

static void verify_write_uncompressed(struct archive *a)
{
struct archive_entry *entry;




assert((entry = archive_entry_new()) != NULL);
archive_entry_set_pathname(entry, file_name);
archive_entry_set_mode(entry, S_IFREG | 0644);
archive_entry_set_size(entry, sizeof(file_data1) + sizeof(file_data2));
archive_entry_set_uid(entry, file_uid);
archive_entry_set_gid(entry, file_gid);
archive_entry_set_mtime(entry, now, 0);
archive_entry_set_atime(entry, now + 3, 0);
assertEqualIntA(a, 0, archive_write_header(a, entry));
assertEqualIntA(a, sizeof(file_data1), archive_write_data(a, file_data1, sizeof(file_data1)));
assertEqualIntA(a, sizeof(file_data2), archive_write_data(a, file_data2, sizeof(file_data2)));
archive_entry_free(entry);


assert((entry = archive_entry_new()) != NULL);
archive_entry_set_pathname(entry, folder_name);
archive_entry_set_mode(entry, S_IFDIR | folder_perm);
archive_entry_set_size(entry, 0);
archive_entry_set_uid(entry, folder_uid);
archive_entry_set_gid(entry, folder_gid);
archive_entry_set_mtime(entry, now, 0);
archive_entry_set_ctime(entry, now + 5, 0);
assertEqualIntA(a, 0, archive_write_header(a, entry));
archive_entry_free(entry);
}


static unsigned int
i2(const void *p_)
{
const unsigned char *p = p_;
return (p[0] | (p[1] << 8));
}

static unsigned int
i4(const void *p_)
{
const unsigned char *p = p_;
return (i2(p) | (i2(p + 2) << 16));
}

static void verify_uncompressed_contents(const char *buff, size_t used)
{
const char *buffend;


unsigned long crc;
struct tm *tm = localtime(&now);



const char *p, *q, *local_header, *extra_start;


buffend = buff + used;



p = buffend - 22;
failure("End-of-central-directory begins with PK\\005\\006 signature");
assertEqualMem(p, "PK\005\006", 4);
failure("This must be disk 0");
assertEqualInt(i2(p + 4), 0);
failure("Central dir must start on disk 0");
assertEqualInt(i2(p + 6), 0);
failure("All central dir entries are on this disk");
assertEqualInt(i2(p + 8), i2(p + 10));
failure("CD start (%d) + CD length (%d) should == archive size - 22",
i4(p + 12), i4(p + 16));
assertEqualInt(i4(p + 12) + i4(p + 16), used - 22);
failure("no zip comment");
assertEqualInt(i2(p + 20), 0);


p = buff + i4(buffend - 6);
failure("Central file record at offset %d should begin with"
" PK\\001\\002 signature",
i4(buffend - 10));


assertEqualMem(p, "PK\001\002", 4);
assertEqualInt(i2(p + 4), 3 * 256 + 10);
assertEqualInt(i2(p + 6), 10);
assertEqualInt(i2(p + 8), 8);
assertEqualInt(i2(p + 10), 0);
assertEqualInt(i2(p + 12), (tm->tm_hour * 2048) + (tm->tm_min * 32) + (tm->tm_sec / 2));
assertEqualInt(i2(p + 14), ((tm->tm_year - 80) * 512) + ((tm->tm_mon + 1) * 32) + tm->tm_mday);
crc = bitcrc32(0, file_data1, sizeof(file_data1));
crc = bitcrc32(crc, file_data2, sizeof(file_data2));
assertEqualInt(i4(p + 16), crc);
assertEqualInt(i4(p + 20), sizeof(file_data1) + sizeof(file_data2));
assertEqualInt(i4(p + 24), sizeof(file_data1) + sizeof(file_data2));
assertEqualInt(i2(p + 28), strlen(file_name));
assertEqualInt(i2(p + 30), 28);
assertEqualInt(i2(p + 32), 0);
assertEqualInt(i2(p + 34), 0);
assertEqualInt(i2(p + 36), 0);
assertEqualInt(i4(p + 38) >> 16 & 01777, file_perm);
assertEqualInt(i4(p + 42), 0);
assertEqualMem(p + 46, file_name, strlen(file_name));
p = p + 46 + strlen(file_name);
assertEqualInt(i2(p), 0x5455);
assertEqualInt(i2(p + 2), 9);
assertEqualInt(p[4], 3);
assertEqualInt(i4(p + 5), now);
assertEqualInt(i4(p + 9), now + 3);
p = p + 4 + i2(p + 2);
assertEqualInt(i2(p), 0x7875);
assertEqualInt(i2(p + 2), 11);

p = p + 4 + i2(p + 2);


local_header = q = buff;
assertEqualMem(q, "PK\003\004", 4);
assertEqualInt(i2(q + 4), 10);
assertEqualInt(i2(q + 6), 8);
assertEqualInt(i2(q + 8), 0);
assertEqualInt(i2(q + 10), (tm->tm_hour * 2048) + (tm->tm_min * 32) + (tm->tm_sec / 2));
assertEqualInt(i2(q + 12), ((tm->tm_year - 80) * 512) + ((tm->tm_mon + 1) * 32) + tm->tm_mday);
assertEqualInt(i4(q + 14), 0);
assertEqualInt(i4(q + 18), sizeof(file_data1) + sizeof(file_data2));
assertEqualInt(i4(q + 22), sizeof(file_data1) + sizeof(file_data2));
assertEqualInt(i2(q + 26), strlen(file_name));
assertEqualInt(i2(q + 28), 41);
assertEqualMem(q + 30, file_name, strlen(file_name));
extra_start = q = q + 30 + strlen(file_name);
assertEqualInt(i2(q), 0x5455);
assertEqualInt(i2(q + 2), 9);
assertEqualInt(q[4], 3);
assertEqualInt(i4(q + 5), now);
assertEqualInt(i4(q + 9), now + 3);
q = q + 4 + i2(q + 2);

assertEqualInt(i2(q), 0x7875);
assertEqualInt(i2(q + 2), 11);
assertEqualInt(q[4], 1);
assertEqualInt(q[5], 4);
assertEqualInt(i4(q + 6), file_uid);
assertEqualInt(q[10], 4);
assertEqualInt(i4(q + 11), file_gid);
q = q + 4 + i2(q + 2);

assertEqualInt(i2(q), 0x6c78);
assertEqualInt(i2(q + 2), 9);
assertEqualInt(q[4], 7);
assertEqualInt(i2(q + 5) >> 8, 3);
assertEqualInt(i2(q + 7), 0);
assertEqualInt(i4(q + 9) >> 16 & 01777, file_perm);
q = q + 4 + i2(q + 2);

assert(q == extra_start + i2(local_header + 28));
q = extra_start + i2(local_header + 28);


assertEqualMem(q, file_data1, sizeof(file_data1));
assertEqualMem(q + sizeof(file_data1), file_data2, sizeof(file_data2));
q = q + sizeof(file_data1) + sizeof(file_data2);


assertEqualMem(q, "PK\007\010", 4);
assertEqualInt(i4(q + 4), crc);
assertEqualInt(i4(q + 8), sizeof(file_data1) + sizeof(file_data2));
assertEqualInt(i4(q + 12), sizeof(file_data1) + sizeof(file_data2));
q = q + 16;


assertEqualMem(p, "PK\001\002", 4);
assertEqualInt(i2(p + 4), 3 * 256 + 20);
assertEqualInt(i2(p + 6), 20);
assertEqualInt(i2(p + 8), 0);
assertEqualInt(i2(p + 10), 0);
assertEqualInt(i2(p + 12), (tm->tm_hour * 2048) + (tm->tm_min * 32) + (tm->tm_sec / 2));
assertEqualInt(i2(p + 14), ((tm->tm_year - 80) * 512) + ((tm->tm_mon + 1) * 32) + tm->tm_mday);
crc = 0;
assertEqualInt(i4(p + 16), crc);
assertEqualInt(i4(p + 20), 0);
assertEqualInt(i4(p + 24), 0);
assertEqualInt(i2(p + 28), strlen(folder_name));
assertEqualInt(i2(p + 30), 28);
assertEqualInt(i2(p + 32), 0);
assertEqualInt(i2(p + 34), 0);
assertEqualInt(i2(p + 36), 0);
assertEqualInt(i4(p + 38) >> 16 & 01777, folder_perm);
assertEqualInt(i4(p + 42), q - buff);
assertEqualMem(p + 46, folder_name, strlen(folder_name));
p = p + 46 + strlen(folder_name);
assertEqualInt(i2(p), 0x5455);
assertEqualInt(i2(p + 2), 9);
assertEqualInt(p[4], 5);
assertEqualInt(i4(p + 5), now);
assertEqualInt(i4(p + 9), now + 5);
p = p + 4 + i2(p + 2);
assertEqualInt(i2(p), 0x7875);
assertEqualInt(i2(p + 2), 11);
assertEqualInt(p[4], 1);
assertEqualInt(p[5], 4);
assertEqualInt(i4(p + 6), folder_uid);
assertEqualInt(p[10], 4);
assertEqualInt(i4(p + 11), folder_gid);



local_header = q;
assertEqualMem(q, "PK\003\004", 4);
assertEqualInt(i2(q + 4), 20);
assertEqualInt(i2(q + 6), 0);
assertEqualInt(i2(q + 8), 0);
assertEqualInt(i2(q + 10), (tm->tm_hour * 2048) + (tm->tm_min * 32) + (tm->tm_sec / 2));
assertEqualInt(i2(q + 12), ((tm->tm_year - 80) * 512) + ((tm->tm_mon + 1) * 32) + tm->tm_mday);
assertEqualInt(i4(q + 14), 0);
assertEqualInt(i4(q + 18), 0);
assertEqualInt(i4(q + 22), 0);
assertEqualInt(i2(q + 26), strlen(folder_name));
assertEqualInt(i2(q + 28), 41);
assertEqualMem(q + 30, folder_name, strlen(folder_name));
extra_start = q = q + 30 + strlen(folder_name);
assertEqualInt(i2(q), 0x5455);
assertEqualInt(i2(q + 2), 9);
assertEqualInt(q[4], 5);
assertEqualInt(i4(q + 5), now);
assertEqualInt(i4(q + 9), now + 5);
q = q + 4 + i2(q + 2);
assertEqualInt(i2(q), 0x7875);
assertEqualInt(i2(q + 2), 11);
assertEqualInt(q[4], 1);
assertEqualInt(q[5], 4);
assertEqualInt(i4(q + 6), folder_uid);
assertEqualInt(q[10], 4);
assertEqualInt(i4(q + 11), folder_gid);
q = q + 4 + i2(q + 2);

assertEqualInt(i2(q), 0x6c78);
assertEqualInt(i2(q + 2), 9);
assertEqualInt(q[4], 7);
assertEqualInt(i2(q + 5) >> 8, 3);
assertEqualInt(i2(q + 7), 0);
assertEqualInt(i4(q + 9) >> 16 & 01777, folder_perm);
q = q + 4 + i2(q + 2);

assert(q == extra_start + i2(local_header + 28));
q = extra_start + i2(local_header + 28);



assertEqualMem(q, "PK\001\002", 4);
}

DEFINE_TEST(test_write_format_zip_compression_store)
{

struct archive *a;
char buff[100000];
size_t used;


now = time(NULL);



assert((a = archive_write_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_write_set_format_zip(a));
assertEqualIntA(a, ARCHIVE_OK,
archive_write_set_options(a, "zip:compression=store"));
assertEqualIntA(a, ARCHIVE_OK,
archive_write_set_options(a, "zip:experimental"));
assertEqualIntA(a, ARCHIVE_OK, archive_write_add_filter_none(a));
assertEqualIntA(a, ARCHIVE_OK, archive_write_set_bytes_per_block(a, 1));
assertEqualIntA(a, ARCHIVE_OK, archive_write_set_bytes_in_last_block(a, 1));
assertEqualIntA(a, ARCHIVE_OK, archive_write_open_memory(a, buff, sizeof(buff), &used));

verify_write_uncompressed(a);


assertEqualIntA(a, ARCHIVE_OK, archive_write_close(a));
assertEqualInt(ARCHIVE_OK, archive_write_free(a));
dumpfile("constructed.zip", buff, used);

verify_uncompressed_contents(buff, used);



assert((a = archive_write_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_write_set_format_zip(a));
assertEqualIntA(a, ARCHIVE_OK,
archive_write_set_options(a, "zip:compression-level=0"));
assertEqualIntA(a, ARCHIVE_OK,
archive_write_set_options(a, "zip:experimental"));
assertEqualIntA(a, ARCHIVE_OK, archive_write_add_filter_none(a));
assertEqualIntA(a, ARCHIVE_OK, archive_write_set_bytes_per_block(a, 1));
assertEqualIntA(a, ARCHIVE_OK, archive_write_set_bytes_in_last_block(a, 1));
assertEqualIntA(a, ARCHIVE_OK, archive_write_open_memory(a, buff, sizeof(buff), &used));

verify_write_uncompressed(a);


assertEqualIntA(a, ARCHIVE_OK, archive_write_close(a));
assertEqualInt(ARCHIVE_OK, archive_write_free(a));
dumpfile("constructed.zip", buff, used);

verify_uncompressed_contents(buff, used);

}
