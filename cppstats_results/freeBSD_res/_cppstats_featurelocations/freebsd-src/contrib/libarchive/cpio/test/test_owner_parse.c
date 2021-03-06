























#include "test.h"
__FBSDID("$FreeBSD$");

#include "../cpio.h"
#include "err.h"

#if !defined(_WIN32)
#define ROOT "root"
static const int root_uids[] = { 0 };
static const int root_gids[] = { 0, 1 };
#elif defined(__CYGWIN__)








#define ROOT "Administrator"
static const int root_uids[] = { 500 };
static const int root_gids[] = { 513, 545, 544 };
#endif

#if defined(ROOT)
static int
int_in_list(int i, const int *l, size_t n)
{
while (n-- > 0)
if (*l++ == i)
return (1);
failure("%d", i);
return (0);
}
#endif

DEFINE_TEST(test_owner_parse)
{
#if !defined(ROOT)
skipping("No uid/gid configuration for this OS");
#else
int uid, gid;

assert(NULL == owner_parse(ROOT, &uid, &gid));
assert(int_in_list(uid, root_uids,
sizeof(root_uids)/sizeof(root_uids[0])));
assertEqualInt(-1, gid);


assert(NULL == owner_parse(ROOT ":", &uid, &gid));
assert(int_in_list(uid, root_uids,
sizeof(root_uids)/sizeof(root_uids[0])));
assert(int_in_list(gid, root_gids,
sizeof(root_gids)/sizeof(root_gids[0])));

assert(NULL == owner_parse(ROOT ".", &uid, &gid));
assert(int_in_list(uid, root_uids,
sizeof(root_uids)/sizeof(root_uids[0])));
assert(int_in_list(gid, root_gids,
sizeof(root_gids)/sizeof(root_gids[0])));

assert(NULL == owner_parse("111", &uid, &gid));
assertEqualInt(111, uid);
assertEqualInt(-1, gid);

assert(NULL == owner_parse("112:", &uid, &gid));
assertEqualInt(112, uid);


assert(NULL == owner_parse("113.", &uid, &gid));
assertEqualInt(113, uid);


assert(NULL == owner_parse(":114", &uid, &gid));
assertEqualInt(-1, uid);
assertEqualInt(114, gid);

assert(NULL == owner_parse(".115", &uid, &gid));
assertEqualInt(-1, uid);
assertEqualInt(115, gid);

assert(NULL == owner_parse("116:117", &uid, &gid));
assertEqualInt(116, uid);
assertEqualInt(117, gid);







assert(NULL != owner_parse(":nonexistentgroup", &uid, &gid));
assert(NULL != owner_parse(ROOT ":nonexistentgroup", &uid, &gid));
assert(NULL !=
owner_parse("nonexistentuser:nonexistentgroup", &uid, &gid));
#endif
}
