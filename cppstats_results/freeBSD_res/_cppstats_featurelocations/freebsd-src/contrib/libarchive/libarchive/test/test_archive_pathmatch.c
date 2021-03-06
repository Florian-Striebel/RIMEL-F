























#include "test.h"
__FBSDID("$FreeBSD$");

#define __LIBARCHIVE_TEST
#include "archive_pathmatch.h"

















DEFINE_TEST(test_archive_pathmatch)
{
assertEqualInt(1, archive_pathmatch("a/b/c", "a/b/c", 0));
assertEqualInt(0, archive_pathmatch("a/b/", "a/b/c", 0));
assertEqualInt(0, archive_pathmatch("a/b", "a/b/c", 0));
assertEqualInt(0, archive_pathmatch("a/b/c", "a/b/", 0));
assertEqualInt(0, archive_pathmatch("a/b/c", "a/b", 0));


assertEqualInt(0, archive_pathmatch("a/b/c", NULL, 0));
assertEqualInt(0, archive_pathmatch_w(L"a/b/c", NULL, 0));


assertEqualInt(1, archive_pathmatch("","", 0));
assertEqualInt(0, archive_pathmatch("","a", 0));
assertEqualInt(1, archive_pathmatch("*","", 0));
assertEqualInt(1, archive_pathmatch("*","a", 0));
assertEqualInt(1, archive_pathmatch("*","abcd", 0));

assertEqualInt(1, archive_pathmatch("*","abcd/efgh/ijkl", 0));
assertEqualInt(1, archive_pathmatch("abcd*efgh/ijkl","abcd/efgh/ijkl", 0));
assertEqualInt(1, archive_pathmatch("abcd***efgh/ijkl","abcd/efgh/ijkl", 0));
assertEqualInt(1, archive_pathmatch("abcd***/efgh/ijkl","abcd/efgh/ijkl", 0));
assertEqualInt(0, archive_pathmatch("?", "", 0));
assertEqualInt(0, archive_pathmatch("?", "\0", 0));
assertEqualInt(1, archive_pathmatch("?", "a", 0));
assertEqualInt(0, archive_pathmatch("?", "ab", 0));
assertEqualInt(1, archive_pathmatch("?", ".", 0));
assertEqualInt(1, archive_pathmatch("?", "?", 0));
assertEqualInt(1, archive_pathmatch("a", "a", 0));
assertEqualInt(0, archive_pathmatch("a", "ab", 0));
assertEqualInt(0, archive_pathmatch("a", "ab", 0));
assertEqualInt(1, archive_pathmatch("a?c", "abc", 0));

assertEqualInt(1, archive_pathmatch("a?c", "a/c", 0));
assertEqualInt(1, archive_pathmatch("a?*c*", "a/c", 0));
assertEqualInt(1, archive_pathmatch("*a*", "a/c", 0));
assertEqualInt(1, archive_pathmatch("*a*", "/a/c", 0));
assertEqualInt(1, archive_pathmatch("*a*", "defaaaaaaa", 0));
assertEqualInt(0, archive_pathmatch("a*", "defghi", 0));
assertEqualInt(0, archive_pathmatch("*a*", "defghi", 0));


assertEqualInt(1, archive_pathmatch("abc[def", "abc[def", 0));
assertEqualInt(0, archive_pathmatch("abc[def]", "abc[def", 0));
assertEqualInt(0, archive_pathmatch("abc[def", "abcd", 0));
assertEqualInt(1, archive_pathmatch("abc[def]", "abcd", 0));
assertEqualInt(1, archive_pathmatch("abc[def]", "abce", 0));
assertEqualInt(1, archive_pathmatch("abc[def]", "abcf", 0));
assertEqualInt(0, archive_pathmatch("abc[def]", "abcg", 0));
assertEqualInt(1, archive_pathmatch("abc[d*f]", "abcd", 0));
assertEqualInt(1, archive_pathmatch("abc[d*f]", "abc*", 0));
assertEqualInt(0, archive_pathmatch("abc[d*f]", "abcdefghi", 0));
assertEqualInt(0, archive_pathmatch("abc[d*", "abcdefghi", 0));
assertEqualInt(1, archive_pathmatch("abc[d*", "abc[defghi", 0));
assertEqualInt(1, archive_pathmatch("abc[d-f]", "abcd", 0));
assertEqualInt(1, archive_pathmatch("abc[d-f]", "abce", 0));
assertEqualInt(1, archive_pathmatch("abc[d-f]", "abcf", 0));
assertEqualInt(0, archive_pathmatch("abc[d-f]", "abcg", 0));
assertEqualInt(0, archive_pathmatch("abc[d-fh-k]", "abca", 0));
assertEqualInt(1, archive_pathmatch("abc[d-fh-k]", "abcd", 0));
assertEqualInt(1, archive_pathmatch("abc[d-fh-k]", "abce", 0));
assertEqualInt(1, archive_pathmatch("abc[d-fh-k]", "abcf", 0));
assertEqualInt(0, archive_pathmatch("abc[d-fh-k]", "abcg", 0));
assertEqualInt(1, archive_pathmatch("abc[d-fh-k]", "abch", 0));
assertEqualInt(1, archive_pathmatch("abc[d-fh-k]", "abci", 0));
assertEqualInt(1, archive_pathmatch("abc[d-fh-k]", "abcj", 0));
assertEqualInt(1, archive_pathmatch("abc[d-fh-k]", "abck", 0));
assertEqualInt(0, archive_pathmatch("abc[d-fh-k]", "abcl", 0));
assertEqualInt(0, archive_pathmatch("abc[d-fh-k]", "abc-", 0));


assertEqualInt(0, archive_pathmatch("abc[]efg", "abcdefg", 0));
assertEqualInt(0, archive_pathmatch("abc[]efg", "abcqefg", 0));
assertEqualInt(0, archive_pathmatch("abc[]efg", "abcefg", 0));
assertEqualInt(1, archive_pathmatch("abc[!]efg", "abcdefg", 0));
assertEqualInt(1, archive_pathmatch("abc[!]efg", "abcqefg", 0));
assertEqualInt(0, archive_pathmatch("abc[!]efg", "abcefg", 0));


assertEqualInt(0, archive_pathmatch("abc[d-fh-]", "abcl", 0));
assertEqualInt(1, archive_pathmatch("abc[d-fh-]", "abch", 0));
assertEqualInt(1, archive_pathmatch("abc[d-fh-]", "abc-", 0));
assertEqualInt(1, archive_pathmatch("abc[d-fh-]", "abc-", 0));


assertEqualInt(1, archive_pathmatch("abc[\\]]", "abc]", 0));
assertEqualInt(1, archive_pathmatch("abc[\\]d]", "abc]", 0));
assertEqualInt(1, archive_pathmatch("abc[\\]d]", "abcd", 0));
assertEqualInt(1, archive_pathmatch("abc[d\\]]", "abc]", 0));
assertEqualInt(1, archive_pathmatch("abc[d\\]]", "abcd", 0));
assertEqualInt(1, archive_pathmatch("abc[d]e]", "abcde]", 0));
assertEqualInt(1, archive_pathmatch("abc[d\\]e]", "abc]", 0));
assertEqualInt(0, archive_pathmatch("abc[d\\]e]", "abcd]e", 0));
assertEqualInt(0, archive_pathmatch("abc[d]e]", "abc]", 0));


assertEqualInt(1, archive_pathmatch("abc[\\d-f]gh", "abcegh", 0));
assertEqualInt(0, archive_pathmatch("abc[\\d-f]gh", "abcggh", 0));
assertEqualInt(0, archive_pathmatch("abc[\\d-f]gh", "abc\\gh", 0));
assertEqualInt(1, archive_pathmatch("abc[d-\\f]gh", "abcegh", 0));
assertEqualInt(1, archive_pathmatch("abc[\\d-\\f]gh", "abcegh", 0));
assertEqualInt(1, archive_pathmatch("abc[\\d-\\f]gh", "abcegh", 0));

assertEqualInt(0, archive_pathmatch("abc[d\\-f]gh", "abcegh", 0));
assertEqualInt(1, archive_pathmatch("abc[d\\-f]gh", "abc-gh", 0));


assertEqualInt(0, archive_pathmatch("abc[!d]", "abcd", 0));
assertEqualInt(1, archive_pathmatch("abc[!d]", "abce", 0));
assertEqualInt(1, archive_pathmatch("abc[!d]", "abcc", 0));
assertEqualInt(0, archive_pathmatch("abc[!d-z]", "abcq", 0));
assertEqualInt(1, archive_pathmatch("abc[!d-gi-z]", "abch", 0));
assertEqualInt(1, archive_pathmatch("abc[!fgijkl]", "abch", 0));
assertEqualInt(0, archive_pathmatch("abc[!fghijkl]", "abch", 0));


assertEqualInt(0, archive_pathmatch("abc\\[def]", "abc\\d", 0));
assertEqualInt(1, archive_pathmatch("abc\\[def]", "abc[def]", 0));
assertEqualInt(0, archive_pathmatch("abc\\\\[def]", "abc[def]", 0));
assertEqualInt(0, archive_pathmatch("abc\\\\[def]", "abc\\[def]", 0));
assertEqualInt(1, archive_pathmatch("abc\\\\[def]", "abc\\d", 0));
assertEqualInt(1, archive_pathmatch("abcd\\", "abcd\\", 0));
assertEqualInt(0, archive_pathmatch("abcd\\", "abcd\\[", 0));
assertEqualInt(0, archive_pathmatch("abcd\\", "abcde", 0));
assertEqualInt(0, archive_pathmatch("abcd\\[", "abcd\\", 0));







assertEqualInt(0, archive_pathmatch("a/b/", "a/bc", 0));
assertEqualInt(1, archive_pathmatch("a/./b", "a/b", 0));
assertEqualInt(0, archive_pathmatch("a\\/./b", "a/b", 0));
assertEqualInt(0, archive_pathmatch("a/\\./b", "a/b", 0));
assertEqualInt(0, archive_pathmatch("a/.\\/b", "a/b", 0));
assertEqualInt(0, archive_pathmatch("a\\/\\.\\/b", "a/b", 0));
assertEqualInt(1, archive_pathmatch("./abc/./def/", "abc/def/", 0));
assertEqualInt(1, archive_pathmatch("abc/def", "./././abc/./def", 0));
assertEqualInt(1, archive_pathmatch("abc/def/././//", "./././abc/./def/", 0));
assertEqualInt(1, archive_pathmatch(".////abc/.//def", "./././abc/./def", 0));
assertEqualInt(1, archive_pathmatch("./abc?def/", "abc/def/", 0));
failure("\"?./\" is not the same as \"/./\"");
assertEqualInt(0, archive_pathmatch("./abc?./def/", "abc/def/", 0));
failure("Trailing '/' should match no trailing '/'");
assertEqualInt(1, archive_pathmatch("./abc/./def/", "abc/def", 0));
failure("Trailing '/./' is still the same directory.");
assertEqualInt(1, archive_pathmatch("./abc/./def/./", "abc/def", 0));
failure("Trailing '/.' is still the same directory.");
assertEqualInt(1, archive_pathmatch("./abc/./def/.", "abc/def", 0));
assertEqualInt(1, archive_pathmatch("./abc/./def", "abc/def/", 0));
failure("Trailing '/./' is still the same directory.");
assertEqualInt(1, archive_pathmatch("./abc/./def", "abc/def/./", 0));
failure("Trailing '/.' is still the same directory.");
assertEqualInt(1, archive_pathmatch("./abc*/./def", "abc/def/.", 0));


assertEqualInt(0,
archive_pathmatch("bcd", "abcd", PATHMATCH_NO_ANCHOR_START));
assertEqualInt(1,
archive_pathmatch("abcd", "abcd", PATHMATCH_NO_ANCHOR_START));
assertEqualInt(0,
archive_pathmatch("^bcd", "abcd", PATHMATCH_NO_ANCHOR_START));
assertEqualInt(1,
archive_pathmatch("b/c/d", "a/b/c/d", PATHMATCH_NO_ANCHOR_START));
assertEqualInt(0,
archive_pathmatch("^b/c/d", "a/b/c/d", PATHMATCH_NO_ANCHOR_START));
assertEqualInt(0,
archive_pathmatch("/b/c/d", "a/b/c/d", PATHMATCH_NO_ANCHOR_START));
assertEqualInt(0,
archive_pathmatch("a/b/c", "a/b/c/d", PATHMATCH_NO_ANCHOR_START));
assertEqualInt(1,
archive_pathmatch("a/b/c/d", "a/b/c/d", PATHMATCH_NO_ANCHOR_START));
assertEqualInt(0,
archive_pathmatch("b/c", "a/b/c/d", PATHMATCH_NO_ANCHOR_START));
assertEqualInt(0,
archive_pathmatch("^b/c", "a/b/c/d", PATHMATCH_NO_ANCHOR_START));


assertEqualInt(1,
archive_pathmatch("b/c/d", "a/b/c/d", PATHMATCH_NO_ANCHOR_START));
assertEqualInt(1,
archive_pathmatch("b/c/d", "/a/b/c/d", PATHMATCH_NO_ANCHOR_START));



assertEqualInt(0,
archive_pathmatch("bcd", "abcd", PATHMATCH_NO_ANCHOR_END));
assertEqualInt(1,
archive_pathmatch("abcd", "abcd", PATHMATCH_NO_ANCHOR_END));
assertEqualInt(1,
archive_pathmatch("abcd", "abcd/", PATHMATCH_NO_ANCHOR_END));
assertEqualInt(1,
archive_pathmatch("abcd", "abcd/.", PATHMATCH_NO_ANCHOR_END));
assertEqualInt(0,
archive_pathmatch("abc", "abcd", PATHMATCH_NO_ANCHOR_END));
assertEqualInt(1,
archive_pathmatch("a/b/c", "a/b/c/d", PATHMATCH_NO_ANCHOR_END));
assertEqualInt(0,
archive_pathmatch("a/b/c$", "a/b/c/d", PATHMATCH_NO_ANCHOR_END));
assertEqualInt(1,
archive_pathmatch("a/b/c$", "a/b/c", PATHMATCH_NO_ANCHOR_END));
assertEqualInt(1,
archive_pathmatch("a/b/c$", "a/b/c/", PATHMATCH_NO_ANCHOR_END));
assertEqualInt(1,
archive_pathmatch("a/b/c/", "a/b/c/d", PATHMATCH_NO_ANCHOR_END));
assertEqualInt(0,
archive_pathmatch("a/b/c/$", "a/b/c/d", PATHMATCH_NO_ANCHOR_END));
assertEqualInt(1,
archive_pathmatch("a/b/c/$", "a/b/c/", PATHMATCH_NO_ANCHOR_END));
assertEqualInt(1,
archive_pathmatch("a/b/c/$", "a/b/c", PATHMATCH_NO_ANCHOR_END));
assertEqualInt(0,
archive_pathmatch("b/c", "a/b/c/d", PATHMATCH_NO_ANCHOR_END));


assertEqualInt(1,
archive_pathmatch("b/c", "a/b/c/d", PATHMATCH_NO_ANCHOR_START | PATHMATCH_NO_ANCHOR_END));
assertEqualInt(0,
archive_pathmatch("/b/c", "a/b/c/d", PATHMATCH_NO_ANCHOR_START | PATHMATCH_NO_ANCHOR_END));
assertEqualInt(0,
archive_pathmatch("/a/b/c", "a/b/c/d", PATHMATCH_NO_ANCHOR_START | PATHMATCH_NO_ANCHOR_END));
assertEqualInt(1,
archive_pathmatch("/a/b/c", "/a/b/c/d", PATHMATCH_NO_ANCHOR_START | PATHMATCH_NO_ANCHOR_END));
assertEqualInt(0,
archive_pathmatch("/a/b/c$", "a/b/c/d", PATHMATCH_NO_ANCHOR_START | PATHMATCH_NO_ANCHOR_END));
assertEqualInt(0,
archive_pathmatch("/a/b/c/d$", "a/b/c/d", PATHMATCH_NO_ANCHOR_START | PATHMATCH_NO_ANCHOR_END));
assertEqualInt(0,
archive_pathmatch("/a/b/c/d$", "/a/b/c/d/e", PATHMATCH_NO_ANCHOR_START | PATHMATCH_NO_ANCHOR_END));
assertEqualInt(1,
archive_pathmatch("/a/b/c/d$", "/a/b/c/d", PATHMATCH_NO_ANCHOR_START | PATHMATCH_NO_ANCHOR_END));
assertEqualInt(1,
archive_pathmatch("^a/b/c", "a/b/c/d", PATHMATCH_NO_ANCHOR_START | PATHMATCH_NO_ANCHOR_END));
assertEqualInt(0,
archive_pathmatch("^a/b/c$", "a/b/c/d", PATHMATCH_NO_ANCHOR_START | PATHMATCH_NO_ANCHOR_END));
assertEqualInt(0,
archive_pathmatch("a/b/c$", "a/b/c/d", PATHMATCH_NO_ANCHOR_START | PATHMATCH_NO_ANCHOR_END));
assertEqualInt(1,
archive_pathmatch("b/c/d$", "a/b/c/d", PATHMATCH_NO_ANCHOR_START | PATHMATCH_NO_ANCHOR_END));
}
