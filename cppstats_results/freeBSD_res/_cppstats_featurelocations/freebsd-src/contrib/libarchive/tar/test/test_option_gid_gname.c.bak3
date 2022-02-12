























#include "test.h"
__FBSDID("$FreeBSD$");

DEFINE_TEST(test_option_gid_gname)
{
char *reference, *data;
size_t s;

assertUmask(0);
assertMakeFile("file", 0644, "1234567890");


failure("Error invoking %s c", testprog);
assertEqualInt(0,
systemf("%s cf archive1 --format=ustar file >stdout1.txt 2>stderr1.txt",
testprog));
assertEmptyFile("stdout1.txt");
assertEmptyFile("stderr1.txt");
reference = slurpfile(&s, "archive1");


failure("Error invoking %s c", testprog);
assertEqualInt(0,
systemf("%s cf archive2 --gid=17 --gname=foofoofoo --format=ustar file >stdout2.txt 2>stderr2.txt",
testprog));
assertEmptyFile("stdout2.txt");
assertEmptyFile("stderr2.txt");
data = slurpfile(&s, "archive2");

assertEqualMem(data + 116, "000021 \0", 8);
assertEqualMem(data + 297, "foofoofoo\0", 10);
free(data);


failure("Error invoking %s c", testprog);
assertEqualInt(0,
systemf("%s cf archive4 --gname=foofoofoo --format=ustar file >stdout4.txt 2>stderr4.txt",
testprog));
assertEmptyFile("stdout4.txt");
assertEmptyFile("stderr4.txt");
data = slurpfile(&s, "archive4");

assertEqualMem(data + 116, reference + 116, 8);
assertEqualMem(data + 297, "foofoofoo\0", 10);
free(data);
free(reference);


failure("Error invoking %s c", testprog);
assertEqualInt(0,
systemf("%s cf archive3 --gid=17 --gname= --format=ustar file >stdout3.txt 2>stderr3.txt",
testprog));
assertEmptyFile("stdout3.txt");
assertEmptyFile("stderr3.txt");
data = slurpfile(&s, "archive3");
assertEqualMem(data + 116, "000021 \0", 8);

assertEqualMem(data + 297, "\0", 1);
free(data);








}
