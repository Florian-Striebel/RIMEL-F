























#include "test.h"
__FBSDID("$FreeBSD$");

#if defined(_WIN32) && !defined(__CYGWIN__) && !defined(__BORLANDC__)
#define chmod _chmod
#endif

static void
clear_fflags(const char *pathname)
{
#if defined(HAVE_STRUCT_STAT_ST_FLAGS)
chflags(pathname, 0);
#elif (defined(FS_IOC_GETFLAGS) && defined(HAVE_WORKING_FS_IOC_GETFLAGS)) || (defined(EXT2_IOC_GETFLAGS) && defined(HAVE_WORKING_EXT2_IOC_GETFLAGS))

int fd;

fd = open(pathname, O_RDONLY | O_NONBLOCK);
if (fd < 0)
return;
ioctl(fd,
#if defined(FS_IOC_GETFLAGS)
FS_IOC_GETFLAGS,
#else
EXT2_IOC_GETFLAGS,
#endif
0);
#else
(void)pathname;
#endif
return;
}

DEFINE_TEST(test_option_fflags)
{
int r;

if (!canNodump()) {
skipping("Can't test nodump flag on this filesystem");
return;
}


assertMakeFile("f", 0644, "a");


assertSetNodump("f");


chmod("f", 0644);


r = systemf("%s -c --fflags -f fflags.tar f >fflags.out 2>fflags.err", testprog);
assertEqualInt(r, 0);


r = systemf("%s -c --no-fflags -f nofflags.tar f >nofflags.out 2>nofflags.err", testprog);
assertEqualInt(r, 0);


assertMakeDir("fflags_fflags", 0755);
clear_fflags("fflags_fflags");
r = systemf("%s -x -C fflags_fflags --no-same-permissions --fflags -f fflags.tar >fflags_fflags.out 2>fflags_fflags.err", testprog);
assertEqualInt(r, 0);
assertEqualFflags("f", "fflags_fflags/f");


assertMakeDir("fflags_nofflags", 0755);
clear_fflags("fflags_nofflags");
r = systemf("%s -x -C fflags_nofflags -p --no-fflags -f fflags.tar >fflags_nofflags.out 2>fflags_nofflags.err", testprog);
assertEqualInt(r, 0);
assertUnequalFflags("f", "fflags_nofflags/f");


assertMakeDir("nofflags_fflags", 0755);
clear_fflags("nofflags_fflags");
r = systemf("%s -x -C nofflags_fflags --no-same-permissions --fflags -f nofflags.tar >nofflags_fflags.out 2>nofflags_fflags.err", testprog);
assertEqualInt(r, 0);
assertUnequalFflags("f", "nofflags_fflags/f");


assertMakeDir("nofflags_nofflags", 0755);
clear_fflags("nofflags_nofflags");
r = systemf("%s -x -C nofflags_nofflags -p --no-fflags -f nofflags.tar >nofflags_nofflags.out 2>nofflags_nofflags.err", testprog);
assertEqualInt(r, 0);
assertUnequalFflags("f", "nofflags_nofflags/f");
}
