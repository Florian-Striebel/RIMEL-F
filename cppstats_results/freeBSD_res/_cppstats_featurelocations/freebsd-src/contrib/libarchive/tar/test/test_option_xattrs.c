























#include "test.h"
__FBSDID("$FreeBSD$");

DEFINE_TEST(test_option_xattrs)
{
#if !ARCHIVE_XATTR_SUPPORT
skipping("Extended attributes are not supported on this platform");
#else

const char *testattr = "user.libarchive.test";
const char *testval = "testval";
void *readval;
size_t size;
int r;


assertMakeFile("f", 0644, "a");

if (!setXattr("f", "user.libarchive.test", testval,
strlen(testval) + 1)) {
skipping("Can't set user extended attributes on this "
"filesystem");
return;
}


r = systemf("%s -c --no-mac-metadata --xattrs -f xattrs.tar f >xattrs.out 2>xattrs.err", testprog);
assertEqualInt(r, 0);


r = systemf("%s -c --no-mac-metadata --no-xattrs -f noxattrs.tar f >noxattrs.out 2>noxattrs.err", testprog);
assertEqualInt(r, 0);


assertMakeDir("xattrs_xattrs", 0755);
r = systemf("%s -x -C xattrs_xattrs --no-same-permissions --xattrs -f xattrs.tar >xattrs_xattrs.out 2>xattrs_xattrs.err", testprog);
assertEqualInt(r, 0);
readval = getXattr("xattrs_xattrs/f", testattr, &size);
if(assertEqualInt(size, strlen(testval) + 1) != 0)
assertEqualMem(readval, testval, size);
free(readval);


assertMakeDir("xattrs_noxattrs", 0755);
r = systemf("%s -x -C xattrs_noxattrs -p --no-xattrs -f xattrs.tar >xattrs_noxattrs.out 2>xattrs_noxattrs.err", testprog);
assertEqualInt(r, 0);
readval = getXattr("xattrs_noxattrs/f", testattr, &size);
assert(readval == NULL);


assertMakeDir("noxattrs_xattrs", 0755);
r = systemf("%s -x -C noxattrs_xattrs --no-same-permissions --xattrs -f noxattrs.tar >noxattrs_xattrs.out 2>noxattrs_xattrs.err", testprog);
assertEqualInt(r, 0);
readval = getXattr("noxattrs_xattrs/f", testattr, &size);
assert(readval == NULL);


assertMakeDir("noxattrs_noxattrs", 0755);
r = systemf("%s -x -C noxattrs_noxattrs -p --no-xattrs -f noxattrs.tar >noxattrs_noxattrs.out 2>noxattrs_noxattrs.err", testprog);
assertEqualInt(r, 0);
readval = getXattr("noxattrs_noxattrs/f", testattr, &size);
assert(readval == NULL);
#endif
}
