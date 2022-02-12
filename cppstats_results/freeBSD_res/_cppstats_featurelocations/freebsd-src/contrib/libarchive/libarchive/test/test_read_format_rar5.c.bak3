























#include "test.h"



#define __LIBARCHIVE_BUILD
#include <archive_crc32.h>
#include <archive_endian.h>

#define PROLOGUE(reffile) struct archive_entry *ae; struct archive *a; (void) a; (void) ae; extract_reference_file(reffile); assert((a = archive_read_new()) != NULL); assertA(0 == archive_read_support_filter_all(a)); assertA(0 == archive_read_support_format_all(a)); assertA(0 == archive_read_open_filename(a, reffile, 10240))












#define PROLOGUE_MULTI(reffile) struct archive_entry *ae; struct archive *a; (void) a; (void) ae; extract_reference_files(reffile); assert((a = archive_read_new()) != NULL); assertA(0 == archive_read_support_filter_all(a)); assertA(0 == archive_read_support_format_all(a)); assertA(0 == archive_read_open_filenames(a, reffile, 10240))













#define EPILOGUE() assertEqualIntA(a, ARCHIVE_OK, archive_read_close(a)); assertEqualInt(ARCHIVE_OK, archive_read_free(a))



static
int verify_data(const uint8_t* data_ptr, int magic, int size) {
int i = 0;







for(i = 0; i < size / 4; ++i) {
const int k = i + 1;
const signed int* lptr = (const signed int*) &data_ptr[i * 4];
signed int val = k * k - 3 * k + (1 + magic);

if(val < 0)
val = 0;




if(archive_le32dec(lptr) != (uint32_t) val)
return 0;
}

return 1;
}

static
int extract_one(struct archive* a, struct archive_entry* ae, uint32_t crc) {
la_ssize_t fsize, bytes_read;
uint8_t* buf;
int ret = 1;
uint32_t computed_crc;

fsize = (la_ssize_t) archive_entry_size(ae);
buf = malloc(fsize);
if(buf == NULL)
return 1;

bytes_read = archive_read_data(a, buf, fsize);
if(bytes_read != fsize) {
assertEqualInt(bytes_read, fsize);
goto fn_exit;
}

computed_crc = crc32(0, buf, fsize);
assertEqualInt(computed_crc, crc);
ret = 0;

fn_exit:
free(buf);
return ret;
}

DEFINE_TEST(test_read_format_rar5_set_format)
{
struct archive *a;
struct archive_entry *ae;
const char reffile[] = "test_read_format_rar5_stored.rar";

extract_reference_file(reffile);
assert((a = archive_read_new()) != NULL);
assertA(0 == archive_read_support_filter_all(a));
assertA(0 == archive_read_set_format(a, ARCHIVE_FORMAT_RAR_V5));
assertA(0 == archive_read_open_filename(a, reffile, 10240));
assertA(0 == archive_read_next_header(a, &ae));
EPILOGUE();
}

DEFINE_TEST(test_read_format_rar5_stored)
{
const char helloworld_txt[] = "hello libarchive test suite!\n";
la_ssize_t file_size = sizeof(helloworld_txt) - 1;
char buff[64];

PROLOGUE("test_read_format_rar5_stored.rar");

assertA(0 == archive_read_next_header(a, &ae));
assertEqualString("helloworld.txt", archive_entry_pathname(ae));
assertA((int) archive_entry_mtime(ae) > 0);
assertA((int) archive_entry_ctime(ae) == 0);
assertA((int) archive_entry_atime(ae) == 0);
assertEqualInt(file_size, archive_entry_size(ae));
assertEqualInt(33188, archive_entry_mode(ae));
assertA(file_size == archive_read_data(a, buff, file_size));
assertEqualMem(buff, helloworld_txt, file_size);
assertEqualInt(archive_entry_is_encrypted(ae), 0);

assertA(ARCHIVE_EOF == archive_read_next_header(a, &ae));

EPILOGUE();
}

DEFINE_TEST(test_read_format_rar5_compressed)
{
const int DATA_SIZE = 1200;
uint8_t buff[1200];

PROLOGUE("test_read_format_rar5_compressed.rar");

assertA(0 == archive_read_next_header(a, &ae));
assertEqualString("test.bin", archive_entry_pathname(ae));
assertA((int) archive_entry_mtime(ae) > 0);
assertEqualInt(DATA_SIZE, archive_entry_size(ae));
assertA(DATA_SIZE == archive_read_data(a, buff, DATA_SIZE));
assertA(ARCHIVE_EOF == archive_read_next_header(a, &ae));
verify_data(buff, 0, DATA_SIZE);

EPILOGUE();
}

DEFINE_TEST(test_read_format_rar5_multiple_files)
{
const int DATA_SIZE = 4096;
uint8_t buff[4096];

PROLOGUE("test_read_format_rar5_multiple_files.rar");




assertA(0 == archive_read_next_header(a, &ae));
assertEqualString("test1.bin", archive_entry_pathname(ae));
assertEqualInt(DATA_SIZE, archive_entry_size(ae));
assertA(DATA_SIZE == archive_read_data(a, buff, DATA_SIZE));
assertA(verify_data(buff, 1, DATA_SIZE));

assertA(0 == archive_read_next_header(a, &ae));
assertEqualString("test2.bin", archive_entry_pathname(ae));
assertEqualInt(DATA_SIZE, archive_entry_size(ae));
assertA(DATA_SIZE == archive_read_data(a, buff, DATA_SIZE));
assertA(verify_data(buff, 2, DATA_SIZE));

assertA(0 == archive_read_next_header(a, &ae));
assertEqualString("test3.bin", archive_entry_pathname(ae));
assertEqualInt(DATA_SIZE, archive_entry_size(ae));
assertA(DATA_SIZE == archive_read_data(a, buff, DATA_SIZE));
assertA(verify_data(buff, 3, DATA_SIZE));

assertA(0 == archive_read_next_header(a, &ae));
assertEqualString("test4.bin", archive_entry_pathname(ae));
assertEqualInt(DATA_SIZE, archive_entry_size(ae));
assertA(DATA_SIZE == archive_read_data(a, buff, DATA_SIZE));
assertA(verify_data(buff, 4, DATA_SIZE));



assertA(ARCHIVE_EOF == archive_read_next_header(a, &ae));
EPILOGUE();
}







DEFINE_TEST(test_read_format_rar5_multiple_files_solid)
{
const int DATA_SIZE = 4096;
uint8_t buff[4096];

PROLOGUE("test_read_format_rar5_multiple_files_solid.rar");

assertA(0 == archive_read_next_header(a, &ae));
assertEqualString("test1.bin", archive_entry_pathname(ae));
assertEqualInt(DATA_SIZE, archive_entry_size(ae));
assertA(DATA_SIZE == archive_read_data(a, buff, DATA_SIZE));
assertA(verify_data(buff, 1, DATA_SIZE));

assertA(0 == archive_read_next_header(a, &ae));
assertEqualString("test2.bin", archive_entry_pathname(ae));
assertEqualInt(DATA_SIZE, archive_entry_size(ae));
assertA(DATA_SIZE == archive_read_data(a, buff, DATA_SIZE));
assertA(verify_data(buff, 2, DATA_SIZE));

assertA(0 == archive_read_next_header(a, &ae));
assertEqualString("test3.bin", archive_entry_pathname(ae));
assertEqualInt(DATA_SIZE, archive_entry_size(ae));
assertA(DATA_SIZE == archive_read_data(a, buff, DATA_SIZE));
assertA(verify_data(buff, 3, DATA_SIZE));

assertA(0 == archive_read_next_header(a, &ae));
assertEqualString("test4.bin", archive_entry_pathname(ae));
assertEqualInt(DATA_SIZE, archive_entry_size(ae));
assertA(DATA_SIZE == archive_read_data(a, buff, DATA_SIZE));
assertA(verify_data(buff, 4, DATA_SIZE));

assertA(ARCHIVE_EOF == archive_read_next_header(a, &ae));
EPILOGUE();
}

DEFINE_TEST(test_read_format_rar5_multiarchive_skip_all)
{
const char* reffiles[] = {
"test_read_format_rar5_multiarchive.part01.rar",
"test_read_format_rar5_multiarchive.part02.rar",
"test_read_format_rar5_multiarchive.part03.rar",
"test_read_format_rar5_multiarchive.part04.rar",
"test_read_format_rar5_multiarchive.part05.rar",
"test_read_format_rar5_multiarchive.part06.rar",
"test_read_format_rar5_multiarchive.part07.rar",
"test_read_format_rar5_multiarchive.part08.rar",
NULL
};

PROLOGUE_MULTI(reffiles);
assertA(0 == archive_read_next_header(a, &ae));
assertEqualString("home/antek/temp/build/unrar5/libarchive/bin/bsdcat_test", archive_entry_pathname(ae));
assertA(0 == archive_read_next_header(a, &ae));
assertEqualString("home/antek/temp/build/unrar5/libarchive/bin/bsdtar_test", archive_entry_pathname(ae));
assertA(ARCHIVE_EOF == archive_read_next_header(a, &ae));
EPILOGUE();
}

DEFINE_TEST(test_read_format_rar5_multiarchive_skip_all_but_first)
{
const char* reffiles[] = {
"test_read_format_rar5_multiarchive.part01.rar",
"test_read_format_rar5_multiarchive.part02.rar",
"test_read_format_rar5_multiarchive.part03.rar",
"test_read_format_rar5_multiarchive.part04.rar",
"test_read_format_rar5_multiarchive.part05.rar",
"test_read_format_rar5_multiarchive.part06.rar",
"test_read_format_rar5_multiarchive.part07.rar",
"test_read_format_rar5_multiarchive.part08.rar",
NULL
};

PROLOGUE_MULTI(reffiles);
assertA(0 == archive_read_next_header(a, &ae));
assertA(0 == extract_one(a, ae, 0x35277473));
assertA(0 == archive_read_next_header(a, &ae));
assertA(ARCHIVE_EOF == archive_read_next_header(a, &ae));
EPILOGUE();
}

DEFINE_TEST(test_read_format_rar5_multiarchive_skip_all_but_second)
{
const char* reffiles[] = {
"test_read_format_rar5_multiarchive.part01.rar",
"test_read_format_rar5_multiarchive.part02.rar",
"test_read_format_rar5_multiarchive.part03.rar",
"test_read_format_rar5_multiarchive.part04.rar",
"test_read_format_rar5_multiarchive.part05.rar",
"test_read_format_rar5_multiarchive.part06.rar",
"test_read_format_rar5_multiarchive.part07.rar",
"test_read_format_rar5_multiarchive.part08.rar",
NULL
};

PROLOGUE_MULTI(reffiles);
assertA(0 == archive_read_next_header(a, &ae));
assertA(0 == archive_read_next_header(a, &ae));
assertA(0 == extract_one(a, ae, 0xE59665F8));
assertA(ARCHIVE_EOF == archive_read_next_header(a, &ae));
EPILOGUE();
}

DEFINE_TEST(test_read_format_rar5_blake2)
{
const la_ssize_t proper_size = 814;
uint8_t buf[814];

PROLOGUE("test_read_format_rar5_blake2.rar");
assertA(0 == archive_read_next_header(a, &ae));
assertEqualInt(proper_size, archive_entry_size(ae));




assertA(proper_size == archive_read_data(a, buf, proper_size));


assertEqualInt(crc32(0, buf, proper_size), 0x7E5EC49E);

assertA(ARCHIVE_EOF == archive_read_next_header(a, &ae));
EPILOGUE();
}

DEFINE_TEST(test_read_format_rar5_arm_filter)
{




const la_ssize_t proper_size = 90808;
uint8_t buf[90808];

PROLOGUE("test_read_format_rar5_arm.rar");
assertA(0 == archive_read_next_header(a, &ae));
assertEqualInt(proper_size, archive_entry_size(ae));
assertA(proper_size == archive_read_data(a, buf, proper_size));




assertEqualInt(crc32(0, buf, proper_size), 0x886F91EB);

assertA(ARCHIVE_EOF == archive_read_next_header(a, &ae));
EPILOGUE();
}

DEFINE_TEST(test_read_format_rar5_stored_skip_all)
{
const char* fname = "test_read_format_rar5_stored_manyfiles.rar";

PROLOGUE(fname);
assertA(0 == archive_read_next_header(a, &ae));
assertEqualString("make_uue.tcl", archive_entry_pathname(ae));
assertA(0 == archive_read_next_header(a, &ae));
assertEqualString("cebula.txt", archive_entry_pathname(ae));
assertA(0 == archive_read_next_header(a, &ae));
assertEqualString("test.bin", archive_entry_pathname(ae));
assertA(ARCHIVE_EOF == archive_read_next_header(a, &ae));
EPILOGUE();
}

DEFINE_TEST(test_read_format_rar5_stored_skip_in_part)
{
const char* fname = "test_read_format_rar5_stored_manyfiles.rar";
char buf[6];



PROLOGUE(fname);
assertA(0 == archive_read_next_header(a, &ae));
assertEqualString("make_uue.tcl", archive_entry_pathname(ae));
assertA(0 == archive_read_next_header(a, &ae));
assertEqualString("cebula.txt", archive_entry_pathname(ae));
assertA(6 == archive_read_data(a, buf, 6));
assertEqualInt(0, memcmp(buf, "Cebula", 6));
assertA(0 == archive_read_next_header(a, &ae));
assertEqualString("test.bin", archive_entry_pathname(ae));
assertA(4 == archive_read_data(a, buf, 4));
assertA(ARCHIVE_EOF == archive_read_next_header(a, &ae));
EPILOGUE();
}

DEFINE_TEST(test_read_format_rar5_stored_skip_all_but_first)
{
const char* fname = "test_read_format_rar5_stored_manyfiles.rar";
char buf[405];



PROLOGUE(fname);
assertA(0 == archive_read_next_header(a, &ae));
assertEqualString("make_uue.tcl", archive_entry_pathname(ae));
assertA(405 == archive_read_data(a, buf, sizeof(buf)));
assertA(0 == archive_read_next_header(a, &ae));
assertEqualString("cebula.txt", archive_entry_pathname(ae));
assertA(0 == archive_read_next_header(a, &ae));
assertEqualString("test.bin", archive_entry_pathname(ae));
assertA(ARCHIVE_EOF == archive_read_next_header(a, &ae));
EPILOGUE();
}

DEFINE_TEST(test_read_format_rar5_stored_skip_all_in_part)
{
const char* fname = "test_read_format_rar5_stored_manyfiles.rar";
char buf[4];



PROLOGUE(fname);
assertA(0 == archive_read_next_header(a, &ae));
assertEqualString("make_uue.tcl", archive_entry_pathname(ae));
assertA(4 == archive_read_data(a, buf, 4));
assertA(0 == archive_read_next_header(a, &ae));
assertEqualString("cebula.txt", archive_entry_pathname(ae));
assertA(4 == archive_read_data(a, buf, 4));
assertA(0 == archive_read_next_header(a, &ae));
assertEqualString("test.bin", archive_entry_pathname(ae));
assertA(4 == archive_read_data(a, buf, 4));
assertA(ARCHIVE_EOF == archive_read_next_header(a, &ae));
EPILOGUE();
}

DEFINE_TEST(test_read_format_rar5_multiarchive_solid_extr_all)
{
const char* reffiles[] = {
"test_read_format_rar5_multiarchive_solid.part01.rar",
"test_read_format_rar5_multiarchive_solid.part02.rar",
"test_read_format_rar5_multiarchive_solid.part03.rar",
"test_read_format_rar5_multiarchive_solid.part04.rar",
NULL
};

PROLOGUE_MULTI(reffiles);
assertA(0 == archive_read_next_header(a, &ae));
assertEqualString("cebula.txt", archive_entry_pathname(ae));
assertA(0 == extract_one(a, ae, 0x7E5EC49E));

assertA(0 == archive_read_next_header(a, &ae));
assertEqualString("test.bin", archive_entry_pathname(ae));
assertA(0 == extract_one(a, ae, 0x7cca70cd));

assertA(0 == archive_read_next_header(a, &ae));
assertEqualString("test1.bin", archive_entry_pathname(ae));
assertA(0 == extract_one(a, ae, 0x7e13b2c6));

assertA(0 == archive_read_next_header(a, &ae));
assertEqualString("test2.bin", archive_entry_pathname(ae));
assertA(0 == extract_one(a, ae, 0xf166afcb));

assertA(0 == archive_read_next_header(a, &ae));
assertEqualString("test3.bin", archive_entry_pathname(ae));
assertA(0 == extract_one(a, ae, 0x9fb123d9));

assertA(0 == archive_read_next_header(a, &ae));
assertEqualString("test4.bin", archive_entry_pathname(ae));
assertA(0 == extract_one(a, ae, 0x10c43ed4));

assertA(0 == archive_read_next_header(a, &ae));
assertEqualString("test5.bin", archive_entry_pathname(ae));
assertA(0 == extract_one(a, ae, 0xb9d155f2));

assertA(0 == archive_read_next_header(a, &ae));
assertEqualString("test6.bin", archive_entry_pathname(ae));
assertA(0 == extract_one(a, ae, 0x36a448ff));

assertA(0 == archive_read_next_header(a, &ae));
assertEqualString("elf-Linux-ARMv7-ls", archive_entry_pathname(ae));
assertA(0 == extract_one(a, ae, 0x886F91EB));

assertA(ARCHIVE_EOF == archive_read_next_header(a, &ae));
EPILOGUE();
}

DEFINE_TEST(test_read_format_rar5_multiarchive_solid_skip_all)
{
const char* reffiles[] = {
"test_read_format_rar5_multiarchive_solid.part01.rar",
"test_read_format_rar5_multiarchive_solid.part02.rar",
"test_read_format_rar5_multiarchive_solid.part03.rar",
"test_read_format_rar5_multiarchive_solid.part04.rar",
NULL
};

PROLOGUE_MULTI(reffiles);
assertA(0 == archive_read_next_header(a, &ae));
assertEqualString("cebula.txt", archive_entry_pathname(ae));
assertA(0 == archive_read_next_header(a, &ae));
assertEqualString("test.bin", archive_entry_pathname(ae));
assertA(0 == archive_read_next_header(a, &ae));
assertEqualString("test1.bin", archive_entry_pathname(ae));
assertA(0 == archive_read_next_header(a, &ae));
assertEqualString("test2.bin", archive_entry_pathname(ae));
assertA(0 == archive_read_next_header(a, &ae));
assertEqualString("test3.bin", archive_entry_pathname(ae));
assertA(0 == archive_read_next_header(a, &ae));
assertEqualString("test4.bin", archive_entry_pathname(ae));
assertA(0 == archive_read_next_header(a, &ae));
assertEqualString("test5.bin", archive_entry_pathname(ae));
assertA(0 == archive_read_next_header(a, &ae));
assertEqualString("test6.bin", archive_entry_pathname(ae));
assertA(0 == archive_read_next_header(a, &ae));
assertEqualString("elf-Linux-ARMv7-ls", archive_entry_pathname(ae));
assertA(ARCHIVE_EOF == archive_read_next_header(a, &ae));
EPILOGUE();
}

DEFINE_TEST(test_read_format_rar5_multiarchive_solid_skip_all_but_first)
{
const char* reffiles[] = {
"test_read_format_rar5_multiarchive_solid.part01.rar",
"test_read_format_rar5_multiarchive_solid.part02.rar",
"test_read_format_rar5_multiarchive_solid.part03.rar",
"test_read_format_rar5_multiarchive_solid.part04.rar",
NULL
};

PROLOGUE_MULTI(reffiles);
assertA(0 == archive_read_next_header(a, &ae));
assertEqualString("cebula.txt", archive_entry_pathname(ae));
assertA(0 == extract_one(a, ae, 0x7E5EC49E));
assertA(0 == archive_read_next_header(a, &ae));
assertEqualString("test.bin", archive_entry_pathname(ae));
assertA(0 == archive_read_next_header(a, &ae));
assertEqualString("test1.bin", archive_entry_pathname(ae));
assertA(0 == archive_read_next_header(a, &ae));
assertEqualString("test2.bin", archive_entry_pathname(ae));
assertA(0 == archive_read_next_header(a, &ae));
assertEqualString("test3.bin", archive_entry_pathname(ae));
assertA(0 == archive_read_next_header(a, &ae));
assertEqualString("test4.bin", archive_entry_pathname(ae));
assertA(0 == archive_read_next_header(a, &ae));
assertEqualString("test5.bin", archive_entry_pathname(ae));
assertA(0 == archive_read_next_header(a, &ae));
assertEqualString("test6.bin", archive_entry_pathname(ae));
assertA(0 == archive_read_next_header(a, &ae));
assertEqualString("elf-Linux-ARMv7-ls", archive_entry_pathname(ae));
assertA(ARCHIVE_EOF == archive_read_next_header(a, &ae));
EPILOGUE();
}




DEFINE_TEST(test_read_format_rar5_multiarchive_solid_skip_all_but_scnd)
{
const char* reffiles[] = {
"test_read_format_rar5_multiarchive_solid.part01.rar",
"test_read_format_rar5_multiarchive_solid.part02.rar",
"test_read_format_rar5_multiarchive_solid.part03.rar",
"test_read_format_rar5_multiarchive_solid.part04.rar",
NULL
};

PROLOGUE_MULTI(reffiles);
assertA(0 == archive_read_next_header(a, &ae));
assertEqualString("cebula.txt", archive_entry_pathname(ae));
assertA(0 == archive_read_next_header(a, &ae));
assertEqualString("test.bin", archive_entry_pathname(ae));
assertA(0 == extract_one(a, ae, 0x7CCA70CD));
assertA(0 == archive_read_next_header(a, &ae));
assertEqualString("test1.bin", archive_entry_pathname(ae));
assertA(0 == archive_read_next_header(a, &ae));
assertEqualString("test2.bin", archive_entry_pathname(ae));
assertA(0 == archive_read_next_header(a, &ae));
assertEqualString("test3.bin", archive_entry_pathname(ae));
assertA(0 == archive_read_next_header(a, &ae));
assertEqualString("test4.bin", archive_entry_pathname(ae));
assertA(0 == archive_read_next_header(a, &ae));
assertEqualString("test5.bin", archive_entry_pathname(ae));
assertA(0 == archive_read_next_header(a, &ae));
assertEqualString("test6.bin", archive_entry_pathname(ae));
assertA(0 == archive_read_next_header(a, &ae));
assertEqualString("elf-Linux-ARMv7-ls", archive_entry_pathname(ae));
assertA(ARCHIVE_EOF == archive_read_next_header(a, &ae));
EPILOGUE();
}

DEFINE_TEST(test_read_format_rar5_multiarchive_solid_skip_all_but_third)
{
const char* reffiles[] = {
"test_read_format_rar5_multiarchive_solid.part01.rar",
"test_read_format_rar5_multiarchive_solid.part02.rar",
"test_read_format_rar5_multiarchive_solid.part03.rar",
"test_read_format_rar5_multiarchive_solid.part04.rar",
NULL
};

PROLOGUE_MULTI(reffiles);
assertA(0 == archive_read_next_header(a, &ae));
assertEqualString("cebula.txt", archive_entry_pathname(ae));
assertA(0 == archive_read_next_header(a, &ae));
assertEqualString("test.bin", archive_entry_pathname(ae));
assertA(0 == archive_read_next_header(a, &ae));
assertEqualString("test1.bin", archive_entry_pathname(ae));
assertA(0 == extract_one(a, ae, 0x7E13B2C6));
assertA(0 == archive_read_next_header(a, &ae));
assertEqualString("test2.bin", archive_entry_pathname(ae));
assertA(0 == archive_read_next_header(a, &ae));
assertEqualString("test3.bin", archive_entry_pathname(ae));
assertA(0 == archive_read_next_header(a, &ae));
assertEqualString("test4.bin", archive_entry_pathname(ae));
assertA(0 == archive_read_next_header(a, &ae));
assertEqualString("test5.bin", archive_entry_pathname(ae));
assertA(0 == archive_read_next_header(a, &ae));
assertEqualString("test6.bin", archive_entry_pathname(ae));
assertA(0 == archive_read_next_header(a, &ae));
assertEqualString("elf-Linux-ARMv7-ls", archive_entry_pathname(ae));
assertA(ARCHIVE_EOF == archive_read_next_header(a, &ae));
EPILOGUE();
}

DEFINE_TEST(test_read_format_rar5_multiarchive_solid_skip_all_but_last)
{
const char* reffiles[] = {
"test_read_format_rar5_multiarchive_solid.part01.rar",
"test_read_format_rar5_multiarchive_solid.part02.rar",
"test_read_format_rar5_multiarchive_solid.part03.rar",
"test_read_format_rar5_multiarchive_solid.part04.rar",
NULL
};

PROLOGUE_MULTI(reffiles);
assertA(0 == archive_read_next_header(a, &ae));
assertEqualString("cebula.txt", archive_entry_pathname(ae));
assertA(0 == archive_read_next_header(a, &ae));
assertEqualString("test.bin", archive_entry_pathname(ae));
assertA(0 == archive_read_next_header(a, &ae));
assertEqualString("test1.bin", archive_entry_pathname(ae));
assertA(0 == archive_read_next_header(a, &ae));
assertEqualString("test2.bin", archive_entry_pathname(ae));
assertA(0 == archive_read_next_header(a, &ae));
assertEqualString("test3.bin", archive_entry_pathname(ae));
assertA(0 == archive_read_next_header(a, &ae));
assertEqualString("test4.bin", archive_entry_pathname(ae));
assertA(0 == archive_read_next_header(a, &ae));
assertEqualString("test5.bin", archive_entry_pathname(ae));
assertA(0 == archive_read_next_header(a, &ae));
assertEqualString("test6.bin", archive_entry_pathname(ae));
assertA(0 == archive_read_next_header(a, &ae));
assertEqualString("elf-Linux-ARMv7-ls", archive_entry_pathname(ae));
assertA(0 == extract_one(a, ae, 0x886F91EB));
assertA(ARCHIVE_EOF == archive_read_next_header(a, &ae));
EPILOGUE();
}

DEFINE_TEST(test_read_format_rar5_solid_skip_all)
{
const char* reffile = "test_read_format_rar5_solid.rar";



PROLOGUE(reffile);
assertA(0 == archive_read_next_header(a, &ae));
assertEqualString("test.bin", archive_entry_pathname(ae));
assertA(0 == archive_read_next_header(a, &ae));
assertEqualString("test1.bin", archive_entry_pathname(ae));
assertA(0 == archive_read_next_header(a, &ae));
assertEqualString("test2.bin", archive_entry_pathname(ae));
assertA(0 == archive_read_next_header(a, &ae));
assertEqualString("test3.bin", archive_entry_pathname(ae));
assertA(0 == archive_read_next_header(a, &ae));
assertEqualString("test4.bin", archive_entry_pathname(ae));
assertA(0 == archive_read_next_header(a, &ae));
assertEqualString("test5.bin", archive_entry_pathname(ae));
assertA(0 == archive_read_next_header(a, &ae));
assertEqualString("test6.bin", archive_entry_pathname(ae));
assertA(ARCHIVE_EOF == archive_read_next_header(a, &ae));
EPILOGUE();
}

DEFINE_TEST(test_read_format_rar5_solid_skip_all_but_first)
{
const char* reffile = "test_read_format_rar5_solid.rar";



PROLOGUE(reffile);
assertA(0 == archive_read_next_header(a, &ae));
assertEqualString("test.bin", archive_entry_pathname(ae));
assertA(0 == extract_one(a, ae, 0x7CCA70CD));
assertA(0 == archive_read_next_header(a, &ae));
assertEqualString("test1.bin", archive_entry_pathname(ae));
assertA(0 == archive_read_next_header(a, &ae));
assertEqualString("test2.bin", archive_entry_pathname(ae));
assertA(0 == archive_read_next_header(a, &ae));
assertEqualString("test3.bin", archive_entry_pathname(ae));
assertA(0 == archive_read_next_header(a, &ae));
assertEqualString("test4.bin", archive_entry_pathname(ae));
assertA(0 == archive_read_next_header(a, &ae));
assertEqualString("test5.bin", archive_entry_pathname(ae));
assertA(0 == archive_read_next_header(a, &ae));
assertEqualString("test6.bin", archive_entry_pathname(ae));
assertA(ARCHIVE_EOF == archive_read_next_header(a, &ae));
EPILOGUE();
}

DEFINE_TEST(test_read_format_rar5_solid_skip_all_but_second)
{
const char* reffile = "test_read_format_rar5_solid.rar";



PROLOGUE(reffile);
assertA(0 == archive_read_next_header(a, &ae));
assertEqualString("test.bin", archive_entry_pathname(ae));
assertA(0 == archive_read_next_header(a, &ae));
assertEqualString("test1.bin", archive_entry_pathname(ae));
assertA(0 == extract_one(a, ae, 0x7E13B2C6));
assertA(0 == archive_read_next_header(a, &ae));
assertEqualString("test2.bin", archive_entry_pathname(ae));
assertA(0 == archive_read_next_header(a, &ae));
assertEqualString("test3.bin", archive_entry_pathname(ae));
assertA(0 == archive_read_next_header(a, &ae));
assertEqualString("test4.bin", archive_entry_pathname(ae));
assertA(0 == archive_read_next_header(a, &ae));
assertEqualString("test5.bin", archive_entry_pathname(ae));
assertA(0 == archive_read_next_header(a, &ae));
assertEqualString("test6.bin", archive_entry_pathname(ae));
assertA(ARCHIVE_EOF == archive_read_next_header(a, &ae));
EPILOGUE();
}

DEFINE_TEST(test_read_format_rar5_solid_skip_all_but_last)
{
const char* reffile = "test_read_format_rar5_solid.rar";



PROLOGUE(reffile);
assertA(0 == archive_read_next_header(a, &ae));
assertEqualString("test.bin", archive_entry_pathname(ae));
assertA(0 == archive_read_next_header(a, &ae));
assertEqualString("test1.bin", archive_entry_pathname(ae));
assertA(0 == archive_read_next_header(a, &ae));
assertEqualString("test2.bin", archive_entry_pathname(ae));
assertA(0 == archive_read_next_header(a, &ae));
assertEqualString("test3.bin", archive_entry_pathname(ae));
assertA(0 == archive_read_next_header(a, &ae));
assertEqualString("test4.bin", archive_entry_pathname(ae));
assertA(0 == archive_read_next_header(a, &ae));
assertEqualString("test5.bin", archive_entry_pathname(ae));
assertA(0 == archive_read_next_header(a, &ae));
assertEqualString("test6.bin", archive_entry_pathname(ae));
assertA(0 == extract_one(a, ae, 0x36A448FF));
assertA(ARCHIVE_EOF == archive_read_next_header(a, &ae));
EPILOGUE();
}

DEFINE_TEST(test_read_format_rar5_extract_win32)
{
PROLOGUE("test_read_format_rar5_win32.rar");
assertA(0 == archive_read_next_header(a, &ae));
assertEqualString("testdir", archive_entry_pathname(ae));
assertEqualInt(archive_entry_mode(ae), AE_IFDIR | 0755);
assertA(0 == extract_one(a, ae, 0));
assertA(0 == archive_read_next_header(a, &ae));
assertEqualString("test.bin", archive_entry_pathname(ae));
assertEqualInt(archive_entry_mode(ae), AE_IFREG | 0644);
assertA(0 == extract_one(a, ae, 0x7CCA70CD));
assertA(0 == archive_read_next_header(a, &ae));
assertEqualString("test1.bin", archive_entry_pathname(ae));
assertEqualInt(archive_entry_mode(ae), AE_IFREG | 0644);
assertA(0 == extract_one(a, ae, 0x7E13B2C6));
assertA(0 == archive_read_next_header(a, &ae));

assertEqualString("test2.bin", archive_entry_pathname(ae));
assertEqualInt(archive_entry_mode(ae), AE_IFREG | 0444);
assertA(0 == extract_one(a, ae, 0xF166AFCB));
assertA(0 == archive_read_next_header(a, &ae));
assertEqualString("test3.bin", archive_entry_pathname(ae));
assertEqualInt(archive_entry_mode(ae), AE_IFREG | 0644);
assertA(0 == extract_one(a, ae, 0x9FB123D9));
assertA(0 == archive_read_next_header(a, &ae));
assertEqualString("test4.bin", archive_entry_pathname(ae));
assertEqualInt(archive_entry_mode(ae), AE_IFREG | 0644);
assertA(0 == extract_one(a, ae, 0x10C43ED4));
assertA(0 == archive_read_next_header(a, &ae));
assertEqualString("test5.bin", archive_entry_pathname(ae));
assertEqualInt(archive_entry_mode(ae), AE_IFREG | 0644);
assertA(0 == extract_one(a, ae, 0xB9D155F2));
assertA(0 == archive_read_next_header(a, &ae));
assertEqualString("test6.bin", archive_entry_pathname(ae));
assertEqualInt(archive_entry_mode(ae), AE_IFREG | 0644);
assertA(0 == extract_one(a, ae, 0x36A448FF));
EPILOGUE();
}

DEFINE_TEST(test_read_format_rar5_block_by_block)
{


struct archive_entry *ae;
struct archive *a;
uint8_t buf[173];
int bytes_read;
uint32_t computed_crc = 0;

extract_reference_file("test_read_format_rar5_compressed.rar");
assert((a = archive_read_new()) != NULL);
assertA(0 == archive_read_support_filter_all(a));
assertA(0 == archive_read_support_format_all(a));
assertA(0 == archive_read_open_filename(a, "test_read_format_rar5_compressed.rar", 130));
assertA(0 == archive_read_next_header(a, &ae));
assertEqualString("test.bin", archive_entry_pathname(ae));
assertEqualInt(1200, archive_entry_size(ae));




while(1) {






bytes_read = archive_read_data(a, buf, sizeof(buf));
assertA(bytes_read >= 0);
if(bytes_read <= 0)
break;

computed_crc = crc32(computed_crc, buf, bytes_read);
}

assertEqualInt(computed_crc, 0x7CCA70CD);
EPILOGUE();
}

DEFINE_TEST(test_read_format_rar5_owner)
{
const int DATA_SIZE = 5;
uint8_t buff[5];

PROLOGUE("test_read_format_rar5_owner.rar");

assertA(0 == archive_read_next_header(a, &ae));
assertEqualString("root.txt", archive_entry_pathname(ae));
assertEqualString("root", archive_entry_uname(ae));
assertEqualString("wheel", archive_entry_gname(ae));
assertA((int) archive_entry_mtime(ae) > 0);
assertEqualInt(DATA_SIZE, archive_entry_size(ae));
assertA(DATA_SIZE == archive_read_data(a, buff, DATA_SIZE));

assertA(0 == archive_read_next_header(a, &ae));
assertEqualString("nobody.txt", archive_entry_pathname(ae));
assertEqualString("nobody", archive_entry_uname(ae));
assertEqualString("nogroup", archive_entry_gname(ae));
assertA((int) archive_entry_mtime(ae) > 0);
assertEqualInt(DATA_SIZE, archive_entry_size(ae));
assertA(DATA_SIZE == archive_read_data(a, buff, DATA_SIZE));

assertA(0 == archive_read_next_header(a, &ae));
assertEqualString("numeric.txt", archive_entry_pathname(ae));
assertEqualInt(9999, archive_entry_uid(ae));
assertEqualInt(8888, archive_entry_gid(ae));
assertA((int) archive_entry_mtime(ae) > 0);
assertEqualInt(DATA_SIZE, archive_entry_size(ae));
assertA(DATA_SIZE == archive_read_data(a, buff, DATA_SIZE));

assertA(ARCHIVE_EOF == archive_read_next_header(a, &ae));

EPILOGUE();
}

DEFINE_TEST(test_read_format_rar5_symlink)
{
const int DATA_SIZE = 5;
uint8_t buff[5];

PROLOGUE("test_read_format_rar5_symlink.rar");

assertA(0 == archive_read_next_header(a, &ae));
assertEqualString("file.txt", archive_entry_pathname(ae));
assertEqualInt(AE_IFREG, archive_entry_filetype(ae));
assertA((int) archive_entry_mtime(ae) > 0);
assertEqualInt(DATA_SIZE, archive_entry_size(ae));
assertA(DATA_SIZE == archive_read_data(a, buff, DATA_SIZE));

assertA(0 == archive_read_next_header(a, &ae));
assertEqualString("symlink.txt", archive_entry_pathname(ae));
assertEqualInt(AE_IFLNK, archive_entry_filetype(ae));
assertEqualString("file.txt", archive_entry_symlink(ae));
assertEqualInt(AE_SYMLINK_TYPE_FILE, archive_entry_symlink_type(ae));
assertA(0 == archive_read_data(a, NULL, archive_entry_size(ae)));

assertA(0 == archive_read_next_header(a, &ae));
assertEqualString("dirlink", archive_entry_pathname(ae));
assertEqualInt(AE_IFLNK, archive_entry_filetype(ae));
assertEqualString("dir", archive_entry_symlink(ae));
assertEqualInt(AE_SYMLINK_TYPE_DIRECTORY, archive_entry_symlink_type(ae));
assertA(0 == archive_read_data(a, NULL, archive_entry_size(ae)));

assertA(0 == archive_read_next_header(a, &ae));
assertEqualString("dir", archive_entry_pathname(ae));
assertEqualInt(AE_IFDIR, archive_entry_filetype(ae));
assertA(0 == archive_read_data(a, NULL, archive_entry_size(ae)));

assertA(ARCHIVE_EOF == archive_read_next_header(a, &ae));

EPILOGUE();
}

DEFINE_TEST(test_read_format_rar5_hardlink)
{
const int DATA_SIZE = 5;
uint8_t buff[5];

PROLOGUE("test_read_format_rar5_hardlink.rar");

assertA(0 == archive_read_next_header(a, &ae));
assertEqualString("file.txt", archive_entry_pathname(ae));
assertEqualInt(AE_IFREG, archive_entry_filetype(ae));
assertA((int) archive_entry_mtime(ae) > 0);
assertEqualInt(DATA_SIZE, archive_entry_size(ae));
assertA(DATA_SIZE == archive_read_data(a, buff, DATA_SIZE));

assertA(0 == archive_read_next_header(a, &ae));
assertEqualString("hardlink.txt", archive_entry_pathname(ae));
assertEqualInt(AE_IFREG, archive_entry_filetype(ae));
assertEqualString("file.txt", archive_entry_hardlink(ae));
assertA(0 == archive_read_data(a, NULL, archive_entry_size(ae)));

assertA(ARCHIVE_EOF == archive_read_next_header(a, &ae));

EPILOGUE();
}

DEFINE_TEST(test_read_format_rar5_extra_field_version)
{
PROLOGUE("test_read_format_rar5_extra_field_version.rar");

assertA(0 == archive_read_next_header(a, &ae));
assertEqualString("bin/2to3;1", archive_entry_pathname(ae));
assertA(0 == extract_one(a, ae, 0xF24181B7));

assertA(0 == archive_read_next_header(a, &ae));
assertEqualString("bin/2to3", archive_entry_pathname(ae));
assertA(0 == extract_one(a, ae, 0xF24181B7));

assertA(ARCHIVE_EOF == archive_read_next_header(a, &ae));

EPILOGUE();
}

DEFINE_TEST(test_read_format_rar5_readtables_overflow)
{
uint8_t buf[16];

PROLOGUE("test_read_format_rar5_readtables_overflow.rar");




(void) archive_read_next_header(a, &ae);
(void) archive_read_data(a, buf, sizeof(buf));
(void) archive_read_next_header(a, &ae);

EPILOGUE();
}

DEFINE_TEST(test_read_format_rar5_leftshift1)
{
uint8_t buf[16];

PROLOGUE("test_read_format_rar5_leftshift1.rar");




(void) archive_read_next_header(a, &ae);
(void) archive_read_data(a, buf, sizeof(buf));
(void) archive_read_next_header(a, &ae);

EPILOGUE();
}

DEFINE_TEST(test_read_format_rar5_leftshift2)
{
uint8_t buf[16];

PROLOGUE("test_read_format_rar5_leftshift2.rar");




(void) archive_read_next_header(a, &ae);
(void) archive_read_data(a, buf, sizeof(buf));
(void) archive_read_next_header(a, &ae);

EPILOGUE();
}

DEFINE_TEST(test_read_format_rar5_truncated_huff)
{
uint8_t buf[16];

PROLOGUE("test_read_format_rar5_truncated_huff.rar");




(void) archive_read_next_header(a, &ae);
(void) archive_read_data(a, buf, sizeof(buf));
(void) archive_read_next_header(a, &ae);

EPILOGUE();
}

DEFINE_TEST(test_read_format_rar5_invalid_dict_reference)
{
uint8_t buf[16];

PROLOGUE("test_read_format_rar5_invalid_dict_reference.rar");


assertA(archive_read_next_header(a, &ae) != ARCHIVE_OK);



assertA(archive_read_data(a, buf, sizeof(buf)) <= 0);


assertA(ARCHIVE_OK != archive_read_next_header(a, &ae));

EPILOGUE();
}

DEFINE_TEST(test_read_format_rar5_distance_overflow)
{
uint8_t buf[16];

PROLOGUE("test_read_format_rar5_distance_overflow.rar");




(void) archive_read_next_header(a, &ae);
(void) archive_read_data(a, buf, sizeof(buf));
(void) archive_read_next_header(a, &ae);

EPILOGUE();
}

DEFINE_TEST(test_read_format_rar5_nonempty_dir_stream)
{
uint8_t buf[16];

PROLOGUE("test_read_format_rar5_nonempty_dir_stream.rar");




(void) archive_read_next_header(a, &ae);
(void) archive_read_data(a, buf, sizeof(buf));
(void) archive_read_next_header(a, &ae);

EPILOGUE();
}

DEFINE_TEST(test_read_format_rar5_fileattr)
{
unsigned long set, clear, flag;

flag = 0;

PROLOGUE("test_read_format_rar5_fileattr.rar");

assertA(0 == archive_read_next_header(a, &ae));
assertEqualInt(archive_entry_mode(ae), 0444 | AE_IFREG);
assertEqualString("readonly.txt", archive_entry_pathname(ae));
assertEqualString("rdonly", archive_entry_fflags_text(ae));
archive_entry_fflags(ae, &set, &clear);
#if defined(__FreeBSD__)
flag = UF_READONLY;
#elif defined(_WIN32) && !defined(CYGWIN)
flag = FILE_ATTRIBUTE_READONLY;
#endif
assertEqualInt(flag, set & flag);

assertA(0 == archive_read_next_header(a, &ae));
assertEqualInt(archive_entry_mode(ae), 0644 | AE_IFREG);
assertEqualString("hidden.txt", archive_entry_pathname(ae));
assertEqualString("hidden", archive_entry_fflags_text(ae));
archive_entry_fflags(ae, &set, &clear);
#if defined(__FreeBSD__)
flag = UF_HIDDEN;
#elif defined(_WIN32) && !defined(CYGWIN)
flag = FILE_ATTRIBUTE_HIDDEN;
#endif
assertEqualInt(flag, set & flag);

assertA(0 == archive_read_next_header(a, &ae));
assertEqualInt(archive_entry_mode(ae), 0644 | AE_IFREG);
assertEqualString("system.txt", archive_entry_pathname(ae));
assertEqualString("system", archive_entry_fflags_text(ae));
archive_entry_fflags(ae, &set, &clear);
#if defined(__FreeBSD__)
flag = UF_SYSTEM;;
#elif defined(_WIN32) && !defined(CYGWIN)
flag = FILE_ATTRIBUTE_SYSTEM;
#endif
assertEqualInt(flag, set & flag);

assertA(0 == archive_read_next_header(a, &ae));
assertEqualInt(archive_entry_mode(ae), 0444 | AE_IFREG);
assertEqualString("ro_hidden.txt", archive_entry_pathname(ae));
assertEqualString("rdonly,hidden", archive_entry_fflags_text(ae));
archive_entry_fflags(ae, &set, &clear);
#if defined(__FreeBSD__)
flag = UF_READONLY | UF_HIDDEN;
#elif defined(_WIN32) && !defined(CYGWIN)
flag = FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_HIDDEN;
#endif
assertEqualInt(flag, set & flag);

assertA(0 == archive_read_next_header(a, &ae));
assertEqualInt(archive_entry_mode(ae), 0555 | AE_IFDIR);
assertEqualString("dir_readonly", archive_entry_pathname(ae));
assertEqualString("rdonly", archive_entry_fflags_text(ae));
archive_entry_fflags(ae, &set, &clear);
#if defined(__FreeBSD__)
flag = UF_READONLY;
#elif defined(_WIN32) && !defined(CYGWIN)
flag = FILE_ATTRIBUTE_READONLY;
#endif
assertEqualInt(flag, set & flag);

assertA(0 == archive_read_next_header(a, &ae));
assertEqualInt(archive_entry_mode(ae), 0755 | AE_IFDIR);
assertEqualString("dir_hidden", archive_entry_pathname(ae));
assertEqualString("hidden", archive_entry_fflags_text(ae));
archive_entry_fflags(ae, &set, &clear);
#if defined(__FreeBSD__)
flag = UF_HIDDEN;
#elif defined(_WIN32) && !defined(CYGWIN)
flag = FILE_ATTRIBUTE_HIDDEN;
#endif
assertEqualInt(flag, set & flag);

assertA(0 == archive_read_next_header(a, &ae));
assertEqualInt(archive_entry_mode(ae), 0755 | AE_IFDIR);
assertEqualString("dir_system", archive_entry_pathname(ae));
assertEqualString("system", archive_entry_fflags_text(ae));
archive_entry_fflags(ae, &set, &clear);
#if defined(__FreeBSD__)
flag = UF_SYSTEM;
#elif defined(_WIN32) && !defined(CYGWIN)
flag = FILE_ATTRIBUTE_SYSTEM;
#endif
assertEqualInt(flag, set & flag);

assertA(0 == archive_read_next_header(a, &ae));
assertEqualInt(archive_entry_mode(ae), 0555 | AE_IFDIR);
assertEqualString("dir_rohidden", archive_entry_pathname(ae));
assertEqualString("rdonly,hidden", archive_entry_fflags_text(ae));
archive_entry_fflags(ae, &set, &clear);
#if defined(__FreeBSD__)
flag = UF_READONLY | UF_HIDDEN;
#elif defined(_WIN32) && !defined(CYGWIN)
flag = FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_HIDDEN;
#endif
assertEqualInt(flag, set & flag);

EPILOGUE();
}

DEFINE_TEST(test_read_format_rar5_different_window_size)
{
char buf[4096];
PROLOGUE("test_read_format_rar5_different_window_size.rar");





(void) archive_read_next_header(a, &ae);
while(0 < archive_read_data(a, buf, sizeof(buf))) {}

(void) archive_read_next_header(a, &ae);
while(0 < archive_read_data(a, buf, sizeof(buf))) {}

(void) archive_read_next_header(a, &ae);
while(0 < archive_read_data(a, buf, sizeof(buf))) {}

EPILOGUE();
}

DEFINE_TEST(test_read_format_rar5_arm_filter_on_window_boundary)
{
char buf[4096];
PROLOGUE("test_read_format_rar5_arm_filter_on_window_boundary.rar");





(void) archive_read_next_header(a, &ae);
while(0 < archive_read_data(a, buf, sizeof(buf))) {}

EPILOGUE();
}

DEFINE_TEST(test_read_format_rar5_different_solid_window_size)
{
char buf[4096];
PROLOGUE("test_read_format_rar5_different_solid_window_size.rar");





(void) archive_read_next_header(a, &ae);
while(0 < archive_read_data(a, buf, sizeof(buf))) {}

(void) archive_read_next_header(a, &ae);
while(0 < archive_read_data(a, buf, sizeof(buf))) {}

(void) archive_read_next_header(a, &ae);
while(0 < archive_read_data(a, buf, sizeof(buf))) {}

EPILOGUE();
}

DEFINE_TEST(test_read_format_rar5_different_winsize_on_merge)
{
char buf[4096];
PROLOGUE("test_read_format_rar5_different_winsize_on_merge.rar");





(void) archive_read_next_header(a, &ae);
while(0 < archive_read_data(a, buf, sizeof(buf))) {}

EPILOGUE();
}

DEFINE_TEST(test_read_format_rar5_block_size_is_too_small)
{
char buf[4096];
PROLOGUE("test_read_format_rar5_block_size_is_too_small.rar");





assertA(archive_read_next_header(a, &ae) != ARCHIVE_OK);
assertA(archive_read_data(a, buf, sizeof(buf)) <= 0);

EPILOGUE();
}
