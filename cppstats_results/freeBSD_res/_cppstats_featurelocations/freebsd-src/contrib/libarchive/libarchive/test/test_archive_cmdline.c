
























#include "test.h"
__FBSDID("$FreeBSD$");

#define __LIBARCHIVE_TEST
#include "archive_cmdline_private.h"

DEFINE_TEST(test_archive_cmdline)
{
struct archive_cmdline *cl;


assert((cl = __archive_cmdline_allocate()) != NULL);
if (cl == NULL)
return;
assertEqualInt(ARCHIVE_OK, __archive_cmdline_parse(cl, "gzip"));
assertEqualInt(1, cl->argc);
assertEqualString("gzip", cl->path);
assertEqualString("gzip", cl->argv[0]);
assertEqualInt(ARCHIVE_OK, __archive_cmdline_free(cl));

assert((cl = __archive_cmdline_allocate()) != NULL);
if (cl == NULL)
return;
assertEqualInt(ARCHIVE_OK, __archive_cmdline_parse(cl, "gzip "));
assertEqualInt(1, cl->argc);
failure("path should not include a space character");
assertEqualString("gzip", cl->path);
failure("arg0 should not include a space character");
assertEqualString("gzip", cl->argv[0]);
assertEqualInt(ARCHIVE_OK, __archive_cmdline_free(cl));

assert((cl = __archive_cmdline_allocate()) != NULL);
if (cl == NULL)
return;
assertEqualInt(ARCHIVE_OK, __archive_cmdline_parse(cl,
"/usr/bin/gzip "));
assertEqualInt(1, cl->argc);
failure("path should be a full path");
assertEqualString("/usr/bin/gzip", cl->path);
failure("arg0 should not be a full path");
assertEqualString("gzip", cl->argv[0]);
assertEqualInt(ARCHIVE_OK, __archive_cmdline_free(cl));


assert((cl = __archive_cmdline_allocate()) != NULL);
if (cl == NULL)
return;
assertEqualInt(ARCHIVE_OK, __archive_cmdline_parse(cl, "\"gzip \""));
assertEqualInt(1, cl->argc);
failure("path should include a space character");
assertEqualString("gzip ", cl->path);
failure("arg0 should include a space character");
assertEqualString("gzip ", cl->argv[0]);
assertEqualInt(ARCHIVE_OK, __archive_cmdline_free(cl));


assert((cl = __archive_cmdline_allocate()) != NULL);
if (cl == NULL)
return;
assertEqualInt(ARCHIVE_OK, __archive_cmdline_parse(cl, "\"gzip \"x"));
assertEqualInt(1, cl->argc);
failure("path should include a space character");
assertEqualString("gzip x", cl->path);
failure("arg0 should include a space character");
assertEqualString("gzip x", cl->argv[0]);
assertEqualInt(ARCHIVE_OK, __archive_cmdline_free(cl));


assert((cl = __archive_cmdline_allocate()) != NULL);
if (cl == NULL)
return;
assertEqualInt(ARCHIVE_OK, __archive_cmdline_parse(cl,
"\"gzip \"x\" s \""));
assertEqualInt(1, cl->argc);
failure("path should include a space character");
assertEqualString("gzip x s ", cl->path);
failure("arg0 should include a space character");
assertEqualString("gzip x s ", cl->argv[0]);
assertEqualInt(ARCHIVE_OK, __archive_cmdline_free(cl));


assert((cl = __archive_cmdline_allocate()) != NULL);
if (cl == NULL)
return;
assertEqualInt(ARCHIVE_OK, __archive_cmdline_parse(cl,
"\"gzip\\\" \""));
assertEqualInt(1, cl->argc);
failure("path should include a space character");
assertEqualString("gzip\" ", cl->path);
failure("arg0 should include a space character");
assertEqualString("gzip\" ", cl->argv[0]);
assertEqualInt(ARCHIVE_OK, __archive_cmdline_free(cl));


assert((cl = __archive_cmdline_allocate()) != NULL);
if (cl == NULL)
return;
assertEqualInt(ARCHIVE_OK, __archive_cmdline_parse(cl, "gzip -d"));
assertEqualInt(2, cl->argc);
assertEqualString("gzip", cl->path);
assertEqualString("gzip", cl->argv[0]);
assertEqualString("-d", cl->argv[1]);
assertEqualInt(ARCHIVE_OK, __archive_cmdline_free(cl));


assert((cl = __archive_cmdline_allocate()) != NULL);
if (cl == NULL)
return;
assertEqualInt(ARCHIVE_OK, __archive_cmdline_parse(cl, "gzip -d -q"));
assertEqualInt(3, cl->argc);
assertEqualString("gzip", cl->path);
assertEqualString("gzip", cl->argv[0]);
assertEqualString("-d", cl->argv[1]);
assertEqualString("-q", cl->argv[2]);
assertEqualInt(ARCHIVE_OK, __archive_cmdline_free(cl));
}
