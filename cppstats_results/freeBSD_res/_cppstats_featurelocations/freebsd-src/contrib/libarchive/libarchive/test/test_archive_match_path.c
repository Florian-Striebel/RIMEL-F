
























#include "test.h"
__FBSDID("$FreeBSD$");

static void
test_exclusion_mbs(void)
{
struct archive_entry *ae;
struct archive *m;

if (!assert((m = archive_match_new()) != NULL))
return;
if (!assert((ae = archive_entry_new()) != NULL)) {
archive_match_free(m);
return;
}


assertEqualIntA(m, 0, archive_match_exclude_pattern(m, "^aa*"));


archive_entry_copy_pathname(ae, "aa1234");
failure("'aa1234' should be excluded");
assertEqualInt(1, archive_match_path_excluded(m, ae));
assertEqualInt(1, archive_match_excluded(m, ae));
archive_entry_clear(ae);
archive_entry_copy_pathname_w(ae, L"aa1234");
failure("'aa1234' should be excluded");
assertEqualInt(1, archive_match_path_excluded(m, ae));
assertEqualInt(1, archive_match_excluded(m, ae));


archive_entry_copy_pathname(ae, "a1234");
failure("'a1234' should not be excluded");
assertEqualInt(0, archive_match_path_excluded(m, ae));
assertEqualInt(0, archive_match_excluded(m, ae));
archive_entry_clear(ae);
archive_entry_copy_pathname_w(ae, L"a1234");
failure("'a1234' should not be excluded");
assertEqualInt(0, archive_match_path_excluded(m, ae));
assertEqualInt(0, archive_match_excluded(m, ae));


archive_entry_free(ae);
archive_match_free(m);
}

static void
test_exclusion_wcs(void)
{
struct archive_entry *ae;
struct archive *m;

if (!assert((m = archive_match_new()) != NULL))
return;
if (!assert((ae = archive_entry_new()) != NULL)) {
archive_match_free(m);
return;
}


assertEqualIntA(m, 0, archive_match_exclude_pattern_w(m, L"^aa*"));


archive_entry_copy_pathname(ae, "aa1234");
failure("'aa1234' should be excluded");
assertEqualInt(1, archive_match_path_excluded(m, ae));
assertEqualInt(1, archive_match_excluded(m, ae));
archive_entry_clear(ae);
archive_entry_copy_pathname_w(ae, L"aa1234");
failure("'aa1234' should be excluded");
assertEqualInt(1, archive_match_path_excluded(m, ae));
assertEqualInt(1, archive_match_excluded(m, ae));


archive_entry_copy_pathname(ae, "a1234");
failure("'a1234' should not be excluded");
assertEqualInt(0, archive_match_path_excluded(m, ae));
assertEqualInt(0, archive_match_excluded(m, ae));
archive_entry_clear(ae);
archive_entry_copy_pathname_w(ae, L"a1234");
failure("'a1234' should not be excluded");
assertEqualInt(0, archive_match_path_excluded(m, ae));
assertEqualInt(0, archive_match_excluded(m, ae));


archive_entry_free(ae);
archive_match_free(m);
}

static void
exclusion_from_file(struct archive *m)
{
struct archive_entry *ae;

if (!assert((ae = archive_entry_new()) != NULL)) {
archive_match_free(m);
return;
}


archive_entry_copy_pathname(ae, "first");
failure("'first' should not be excluded");
assertEqualInt(0, archive_match_path_excluded(m, ae));
assertEqualInt(0, archive_match_excluded(m, ae));
archive_entry_clear(ae);
archive_entry_copy_pathname_w(ae, L"first");
failure("'first' should not be excluded");
assertEqualInt(0, archive_match_path_excluded(m, ae));
assertEqualInt(0, archive_match_excluded(m, ae));


archive_entry_copy_pathname(ae, "second");
failure("'second' should be excluded");
assertEqualInt(1, archive_match_path_excluded(m, ae));
assertEqualInt(1, archive_match_excluded(m, ae));
archive_entry_clear(ae);
archive_entry_copy_pathname_w(ae, L"second");
failure("'second' should be excluded");
assertEqualInt(1, archive_match_path_excluded(m, ae));
assertEqualInt(1, archive_match_excluded(m, ae));


archive_entry_copy_pathname(ae, "third");
failure("'third' should not be excluded");
assertEqualInt(0, archive_match_path_excluded(m, ae));
assertEqualInt(0, archive_match_excluded(m, ae));
archive_entry_clear(ae);
archive_entry_copy_pathname_w(ae, L"third");
failure("'third' should not be excluded");
assertEqualInt(0, archive_match_path_excluded(m, ae));
assertEqualInt(0, archive_match_excluded(m, ae));


archive_entry_copy_pathname(ae, "four");
failure("'four' should be excluded");
assertEqualInt(1, archive_match_path_excluded(m, ae));
assertEqualInt(1, archive_match_excluded(m, ae));
archive_entry_clear(ae);
archive_entry_copy_pathname_w(ae, L"four");
failure("'four' should be excluded");
assertEqualInt(1, archive_match_path_excluded(m, ae));
assertEqualInt(1, archive_match_excluded(m, ae));


archive_entry_free(ae);
}

static void
test_exclusion_from_file_mbs(void)
{
struct archive *m;


if (!assert((m = archive_match_new()) != NULL))
return;
assertEqualIntA(m, 0,
archive_match_exclude_pattern_from_file(m, "exclusion", 0));
exclusion_from_file(m);

archive_match_free(m);


if (!assert((m = archive_match_new()) != NULL))
return;

assertEqualIntA(m, 0,
archive_match_exclude_pattern_from_file(m, "exclusion_null", 1));
exclusion_from_file(m);

archive_match_free(m);
}

static void
test_exclusion_from_file_wcs(void)
{
struct archive *m;


if (!assert((m = archive_match_new()) != NULL))
return;
assertEqualIntA(m, 0,
archive_match_exclude_pattern_from_file_w(m, L"exclusion", 0));
exclusion_from_file(m);

archive_match_free(m);


if (!assert((m = archive_match_new()) != NULL))
return;

assertEqualIntA(m, 0,
archive_match_exclude_pattern_from_file_w(m, L"exclusion_null", 1));
exclusion_from_file(m);

archive_match_free(m);
}

static void
test_inclusion_mbs(void)
{
struct archive_entry *ae;
struct archive *m;
const char *mp;

if (!assert((m = archive_match_new()) != NULL))
return;
if (!assert((ae = archive_entry_new()) != NULL)) {
archive_match_free(m);
return;
}


assertEqualIntA(m, 0, archive_match_include_pattern(m, "^aa*"));


archive_entry_copy_pathname(ae, "aa1234");
failure("'aa1234' should not be excluded");
assertEqualInt(0, archive_match_path_excluded(m, ae));
assertEqualInt(0, archive_match_excluded(m, ae));
archive_entry_clear(ae);
archive_entry_copy_pathname_w(ae, L"aa1234");
failure("'aa1234' should not be excluded");
assertEqualInt(0, archive_match_path_excluded(m, ae));
assertEqualInt(0, archive_match_excluded(m, ae));


archive_entry_copy_pathname(ae, "a1234");
failure("'a1234' should be excluded");
assertEqualInt(1, archive_match_path_excluded(m, ae));
assertEqualInt(1, archive_match_excluded(m, ae));
archive_entry_clear(ae);
archive_entry_copy_pathname_w(ae, L"a1234");
failure("'a1234' should be excluded");
assertEqualInt(1, archive_match_path_excluded(m, ae));
assertEqualInt(1, archive_match_excluded(m, ae));


assertEqualInt(0, archive_match_path_unmatched_inclusions(m));
assertEqualIntA(m, ARCHIVE_EOF,
archive_match_path_unmatched_inclusions_next(m, &mp));


archive_entry_free(ae);
archive_match_free(m);
}

static void
test_inclusion_wcs(void)
{
struct archive_entry *ae;
struct archive *m;
const char *mp;

if (!assert((m = archive_match_new()) != NULL))
return;
if (!assert((ae = archive_entry_new()) != NULL)) {
archive_match_free(m);
return;
}


assertEqualIntA(m, 0, archive_match_include_pattern_w(m, L"^aa*"));


archive_entry_copy_pathname(ae, "aa1234");
failure("'aa1234' should not be excluded");
assertEqualInt(0, archive_match_path_excluded(m, ae));
assertEqualInt(0, archive_match_excluded(m, ae));
archive_entry_clear(ae);
archive_entry_copy_pathname_w(ae, L"aa1234");
failure("'aa1234' should not be excluded");
assertEqualInt(0, archive_match_path_excluded(m, ae));
assertEqualInt(0, archive_match_excluded(m, ae));


archive_entry_copy_pathname(ae, "a1234");
failure("'a1234' should be excluded");
assertEqualInt(1, archive_match_path_excluded(m, ae));
assertEqualInt(1, archive_match_excluded(m, ae));
archive_entry_clear(ae);
archive_entry_copy_pathname_w(ae, L"a1234");
failure("'a1234' should be excluded");
assertEqualInt(1, archive_match_path_excluded(m, ae));
assertEqualInt(1, archive_match_excluded(m, ae));


assertEqualInt(0, archive_match_path_unmatched_inclusions(m));
assertEqualIntA(m, ARCHIVE_EOF,
archive_match_path_unmatched_inclusions_next(m, &mp));


archive_entry_free(ae);
archive_match_free(m);
}

static void
test_inclusion_from_file_mbs(void)
{
struct archive *m;


if (!assert((m = archive_match_new()) != NULL))
return;
assertEqualIntA(m, 0,
archive_match_include_pattern_from_file(m, "inclusion", 0));
exclusion_from_file(m);

archive_match_free(m);


if (!assert((m = archive_match_new()) != NULL))
return;
assertEqualIntA(m, 0,
archive_match_include_pattern_from_file(m, "inclusion_null", 1));
exclusion_from_file(m);

archive_match_free(m);
}

static void
test_inclusion_from_file_wcs(void)
{
struct archive *m;


if (!assert((m = archive_match_new()) != NULL))
return;

assertEqualIntA(m, 0,
archive_match_include_pattern_from_file_w(m, L"inclusion", 0));
exclusion_from_file(m);

archive_match_free(m);


if (!assert((m = archive_match_new()) != NULL))
return;

assertEqualIntA(m, 0,
archive_match_include_pattern_from_file_w(m, L"inclusion_null", 1));
exclusion_from_file(m);

archive_match_free(m);
}

static void
test_exclusion_and_inclusion(void)
{
struct archive_entry *ae;
struct archive *m;
const char *mp;
const wchar_t *wp;

if (!assert((m = archive_match_new()) != NULL))
return;
if (!assert((ae = archive_entry_new()) != NULL)) {
archive_match_free(m);
return;
}

assertEqualIntA(m, 0, archive_match_exclude_pattern(m, "^aaa*"));
assertEqualIntA(m, 0, archive_match_include_pattern_w(m, L"^aa*"));
assertEqualIntA(m, 0, archive_match_include_pattern(m, "^a1*"));


archive_entry_copy_pathname(ae, "aa1234");
failure("'aa1234' should not be excluded");
assertEqualInt(0, archive_match_path_excluded(m, ae));
assertEqualInt(0, archive_match_excluded(m, ae));
archive_entry_clear(ae);
archive_entry_copy_pathname_w(ae, L"aa1234");
failure("'aa1234' should not be excluded");
assertEqualInt(0, archive_match_path_excluded(m, ae));
assertEqualInt(0, archive_match_excluded(m, ae));


archive_entry_copy_pathname(ae, "aaa1234");
failure("'aaa1234' should be excluded");
assertEqualInt(1, archive_match_path_excluded(m, ae));
assertEqualInt(1, archive_match_excluded(m, ae));
archive_entry_clear(ae);
archive_entry_copy_pathname_w(ae, L"aaa1234");
failure("'aaa1234' should be excluded");
assertEqualInt(1, archive_match_path_excluded(m, ae));
assertEqualInt(1, archive_match_excluded(m, ae));


assertEqualInt(1, archive_match_path_unmatched_inclusions(m));

assertEqualIntA(m, ARCHIVE_OK,
archive_match_path_unmatched_inclusions_next(m, &mp));
assertEqualString("^a1*", mp);
assertEqualIntA(m, ARCHIVE_EOF,
archive_match_path_unmatched_inclusions_next(m, &mp));

assertEqualIntA(m, ARCHIVE_OK,
archive_match_path_unmatched_inclusions_next_w(m, &wp));
assertEqualWString(L"^a1*", wp);
assertEqualIntA(m, ARCHIVE_EOF,
archive_match_path_unmatched_inclusions_next_w(m, &wp));


archive_entry_free(ae);
archive_match_free(m);
}

DEFINE_TEST(test_archive_match_path)
{

assertMakeFile("exclusion", 0666, "second\nfour\n");
assertMakeBinFile("exclusion_null", 0666, 12, "second\0four\0");

assertMakeFile("inclusion", 0666, "first\nthird\n");
assertMakeBinFile("inclusion_null", 0666, 12, "first\0third\0");

test_exclusion_mbs();
test_exclusion_wcs();
test_exclusion_from_file_mbs();
test_exclusion_from_file_wcs();
test_inclusion_mbs();
test_inclusion_wcs();
test_inclusion_from_file_mbs();
test_inclusion_from_file_wcs();
test_exclusion_and_inclusion();
}
