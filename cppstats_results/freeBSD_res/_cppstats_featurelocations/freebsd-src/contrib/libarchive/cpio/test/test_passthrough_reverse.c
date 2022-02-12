























#include "test.h"
__FBSDID("$FreeBSD$");










DEFINE_TEST(test_passthrough_reverse)
{
int r;
FILE *filelist;

assertUmask(0);




filelist = fopen("filelist", "w");


assertMakeDir("dir", 0743);


assertMakeFile("dir/file", 0644, "1234567890");
fprintf(filelist, "dir/file\n");


fprintf(filelist, "dir\n");


fclose(filelist);





r = systemf("%s -pdvm out <filelist >stdout 2>stderr", testprog);
failure("Error invoking %s -pd out", testprog);
assertEqualInt(r, 0);

assertChdir("out");


assertTextFileContents("out/dir/file\nout/dir\n1 block\n",
"../stderr");
assertEmptyFile("../stdout");


assertIsDir("dir", 0743);



assertIsReg("dir/file", 0644);
assertFileSize("dir/file", 10);
assertFileNLinks("dir/file", 1);
}
