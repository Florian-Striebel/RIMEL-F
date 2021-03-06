























#include "test.h"
__FBSDID("$FreeBSD$");





DEFINE_TEST(test_passthrough_dotdot)
{
int r;
FILE *filelist;

assertUmask(0);




filelist = fopen("filelist", "w");


assertMakeDir("dir", 0755);
assertChdir("dir");

fprintf(filelist, ".\n");


assertMakeFile("file", 0642, "1234567890");
fprintf(filelist, "file\n");


fclose(filelist);





r = systemf("%s -pdvm .. <../filelist >../stdout 2>../stderr",
testprog);
failure("Error invoking %s -pd ..", testprog);
assertEqualInt(r, 0);

assertChdir("..");


assertTextFileContents("../.\n../file\n1 block\n", "stderr");
assertEmptyFile("stdout");


assertIsReg("file", 0642);
assertFileSize("file", 10);
assertFileNLinks("file", 1);
}
