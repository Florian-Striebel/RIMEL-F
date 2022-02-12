























#include "test.h"
__FBSDID("$FreeBSD$");







static void
verify_write_data(struct archive *a, int sparse)
{
static const char data[]="abcdefghijklmnopqrstuvwxyz";
struct stat st;
struct archive_entry *ae;
size_t buff_size = 64 * 1024;
char *buff, *p;
const char *msg = sparse ? "sparse" : "non-sparse";
FILE *f;

buff = malloc(buff_size);
assert(buff != NULL);
if (buff == NULL)
return;

ae = archive_entry_new();
assert(ae != NULL);
archive_entry_set_size(ae, 8 * buff_size);
archive_entry_set_pathname(ae, "test_write_data");
archive_entry_set_mode(ae, AE_IFREG | 0755);
assertEqualIntA(a, 0, archive_write_header(a, ae));




memset(buff, 0, buff_size);
memcpy(buff, data, sizeof(data));
failure("%s", msg);
assertEqualInt(buff_size, archive_write_data(a, buff, buff_size));


memset(buff, 0, buff_size);
memcpy(buff + buff_size / 2 - 3, data, sizeof(data));
failure("%s", msg);
assertEqualInt(buff_size, archive_write_data(a, buff, buff_size));


memset(buff, 0, buff_size);
memcpy(buff + buff_size - sizeof(data), data, sizeof(data));
failure("%s", msg);
assertEqualInt(buff_size, archive_write_data(a, buff, buff_size));

failure("%s", msg);
assertEqualIntA(a, 0, archive_write_finish_entry(a));


assert(0 == stat(archive_entry_pathname(ae), &st));
assertEqualInt(st.st_size, 8 * buff_size);
f = fopen(archive_entry_pathname(ae), "rb");
assert(f != NULL);
if (f == NULL) {
free(buff);
return;
}


assertEqualInt(buff_size, fread(buff, 1, buff_size, f));
failure("%s", msg);
assertEqualMem(buff, data, sizeof(data));
for (p = buff + sizeof(data); p < buff + buff_size; ++p) {
failure("offset: %d, %s", (int)(p - buff), msg);
if (!assertEqualInt(0, *p))
break;
}


assertEqualInt(buff_size, fread(buff, 1, buff_size, f));
for (p = buff; p < buff + buff_size; ++p) {
failure("offset: %d, %s", (int)(p - buff), msg);
if (p == buff + buff_size / 2 - 3) {
assertEqualMem(p, data, sizeof(data));
p += sizeof(data);
} else if (!assertEqualInt(0, *p))
break;
}


assertEqualInt(buff_size, fread(buff, 1, buff_size, f));
for (p = buff; p < buff + buff_size - sizeof(data); ++p) {
failure("offset: %d, %s", (int)(p - buff), msg);
if (!assertEqualInt(0, *p))
break;
}
failure("%s", msg);
assertEqualMem(buff + buff_size - sizeof(data), data, sizeof(data));



assertEqualInt(0, fclose(f));
archive_entry_free(ae);
free(buff);
}




static void
verify_write_data_block(struct archive *a, int sparse)
{
static const char data[]="abcdefghijklmnopqrstuvwxyz";
struct stat st;
struct archive_entry *ae;
size_t buff_size = 64 * 1024;
char *buff, *p;
const char *msg = sparse ? "sparse" : "non-sparse";
FILE *f;

buff = malloc(buff_size);
assert(buff != NULL);
if (buff == NULL)
return;

ae = archive_entry_new();
assert(ae != NULL);
archive_entry_set_size(ae, 8 * buff_size);
archive_entry_set_pathname(ae, "test_write_data_block");
archive_entry_set_mode(ae, AE_IFREG | 0755);
assertEqualIntA(a, 0, archive_write_header(a, ae));





memset(buff, 0, buff_size);
memcpy(buff, data, sizeof(data));
failure("%s", msg);
assertEqualInt(ARCHIVE_OK,
archive_write_data_block(a, buff, buff_size, 100));


memset(buff, 0, buff_size);
memcpy(buff + buff_size / 2 - 3, data, sizeof(data));
failure("%s", msg);
assertEqualInt(ARCHIVE_OK,
archive_write_data_block(a, buff, buff_size, buff_size + 200));


memset(buff, 0, buff_size);
memcpy(buff + buff_size - sizeof(data), data, sizeof(data));
failure("%s", msg);
assertEqualInt(ARCHIVE_OK,
archive_write_data_block(a, buff, buff_size, buff_size * 2 + 300));

failure("%s", msg);
assertEqualIntA(a, 0, archive_write_finish_entry(a));


assert(0 == stat(archive_entry_pathname(ae), &st));
assertEqualInt(st.st_size, 8 * buff_size);
f = fopen(archive_entry_pathname(ae), "rb");
assert(f != NULL);
if (f == NULL) {
free(buff);
return;
}


assertEqualInt(100, fread(buff, 1, 100, f));
failure("%s", msg);
for (p = buff; p < buff + 100; ++p) {
failure("offset: %d, %s", (int)(p - buff), msg);
if (!assertEqualInt(0, *p))
break;
}


assertEqualInt(buff_size, fread(buff, 1, buff_size, f));
failure("%s", msg);
assertEqualMem(buff, data, sizeof(data));
for (p = buff + sizeof(data); p < buff + buff_size; ++p) {
failure("offset: %d, %s", (int)(p - buff), msg);
if (!assertEqualInt(0, *p))
break;
}


assertEqualInt(100, fread(buff, 1, 100, f));
failure("%s", msg);
for (p = buff; p < buff + 100; ++p) {
failure("offset: %d, %s", (int)(p - buff), msg);
if (!assertEqualInt(0, *p))
break;
}


assertEqualInt(buff_size, fread(buff, 1, buff_size, f));
for (p = buff; p < buff + buff_size; ++p) {
failure("offset: %d, %s", (int)(p - buff), msg);
if (p == buff + buff_size / 2 - 3) {
assertEqualMem(p, data, sizeof(data));
p += sizeof(data);
} else if (!assertEqualInt(0, *p))
break;
}


assertEqualInt(100, fread(buff, 1, 100, f));
failure("%s", msg);
for (p = buff; p < buff + 100; ++p) {
failure("offset: %d, %s", (int)(p - buff), msg);
if (!assertEqualInt(0, *p))
break;
}


assertEqualInt(buff_size, fread(buff, 1, buff_size, f));
for (p = buff; p < buff + buff_size - sizeof(data); ++p) {
failure("offset: %d, %s", (int)(p - buff), msg);
if (!assertEqualInt(0, *p))
break;
}
failure("%s", msg);
assertEqualMem(buff + buff_size - sizeof(data), data, sizeof(data));


assertEqualInt(buff_size, fread(buff, 1, buff_size, f));
failure("%s", msg);
for (p = buff; p < buff + buff_size; ++p) {
failure("offset: %d, %s", (int)(p - buff), msg);
if (!assertEqualInt(0, *p))
break;
}




assertEqualInt(0, fclose(f));
free(buff);
archive_entry_free(ae);
}

DEFINE_TEST(test_write_disk_sparse)
{
struct archive *ad;









assert((ad = archive_write_disk_new()) != NULL);
archive_write_disk_set_options(ad, 0);
verify_write_data(ad, 0);
verify_write_data_block(ad, 0);
assertEqualInt(0, archive_write_free(ad));

assert((ad = archive_write_disk_new()) != NULL);
archive_write_disk_set_options(ad, ARCHIVE_EXTRACT_SPARSE);
verify_write_data(ad, 1);
verify_write_data_block(ad, 1);
assertEqualInt(0, archive_write_free(ad));

}
