























#include "test.h"
__FBSDID("$FreeBSD$");

DEFINE_TEST(test_archive_api_feature)
{
char buff[128];
const char *p;


assertEqualInt(ARCHIVE_VERSION_NUMBER, archive_version_number());
sprintf(buff, "libarchive %d.%d.%d",
archive_version_number() / 1000000,
(archive_version_number() / 1000) % 1000,
archive_version_number() % 1000);
failure("Version string is: %s, computed is: %s",
archive_version_string(), buff);
assertEqualMem(buff, archive_version_string(), strlen(buff));
if (strlen(buff) < strlen(archive_version_string())) {
p = archive_version_string() + strlen(buff);
failure("Version string is: %s", archive_version_string());
if (p[0] == 'd'&& p[1] == 'e' && p[2] == 'v')
p += 3;
else {
assert(*p == 'a' || *p == 'b' || *p == 'c' || *p == 'd');
++p;
}
failure("Version string is: %s", archive_version_string());
assert(*p == '\0');
}
}
