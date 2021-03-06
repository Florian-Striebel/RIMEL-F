






















#include <stdio.h>
#include <errno.h>
#include <assert.h>
#include "ucl.h"

static void
ud_dtor (void *ptr)
{
assert (ptr == NULL);
}

static const char *
ud_emit (void *ptr)
{
return "test userdata emit";
}

int
main (int argc, char **argv)
{
ucl_object_t *obj, *cur, *ar, *ar1, *ref, *test_obj, *comments;
ucl_object_iter_t it;
const ucl_object_t *found, *it_obj, *test;
struct ucl_emitter_functions *fn;
FILE *out;
unsigned char *emitted;
const char *fname_out = NULL;
struct ucl_parser *parser;
int ret = 0;

switch (argc) {
case 2:
fname_out = argv[1];
break;
}


if (fname_out != NULL) {
out = fopen (fname_out, "w");
if (out == NULL) {
exit (-errno);
}
}
else {
out = stdout;
}

obj = ucl_object_typed_new (UCL_OBJECT);


cur = ucl_object_fromstring_common ("value1", 0, UCL_STRING_TRIM);
ucl_object_insert_key (obj, cur, "key0", 0, false);
cur = ucl_object_fromdouble (0.1);
assert (ucl_object_replace_key (obj, cur, "key0", 0, false));


cur = ucl_object_fromstring_common (" test string ", 0, UCL_STRING_TRIM);
ucl_object_insert_key (obj, cur, "key1", 0, false);
cur = ucl_object_fromstring_common (" test \nstring\n\r\n\b\t\f\\\" ", 0,
UCL_STRING_TRIM | UCL_STRING_ESCAPE);
ucl_object_insert_key (obj, cur, "key2", 0, false);
cur = ucl_object_fromstring_common (" test string \n", 0, 0);
ucl_object_insert_key (obj, cur, "key3", 0, false);

ar = ucl_object_typed_new (UCL_ARRAY);
cur = ucl_object_fromint (10);
ucl_array_append (ar, cur);
assert (ucl_array_index_of (ar, cur) == 0);
cur = ucl_object_fromdouble (10.1);
ucl_array_append (ar, cur);
assert (ucl_array_index_of (ar, cur) == 1);
cur = ucl_object_fromdouble (9.999);
ucl_array_prepend (ar, cur);
assert (ucl_array_index_of (ar, cur) == 0);

ar1 = ucl_object_copy (ar);
cur = ucl_object_fromstring ("abc");
ucl_array_prepend (ar1, cur);
cur = ucl_object_fromstring ("cde");
ucl_array_prepend (ar1, cur);
cur = ucl_object_fromstring ("абв");
ucl_array_prepend (ar1, cur);
cur = ucl_object_fromstring ("Ебв");
ucl_array_prepend (ar1, cur);







cur = ucl_object_fromstring ("😎");
ucl_array_prepend (ar1, cur);

ucl_object_array_sort (ar1, ucl_object_compare_qsort);


cur = ucl_object_fromdouble (1.0);
ucl_array_append (ar, cur);
cur = ucl_array_delete (ar, cur);
assert (ucl_object_todouble (cur) == 1.0);
ucl_object_unref (cur);
cur = ucl_object_fromdouble (2.0);
ucl_array_append (ar, cur);
cur = ucl_array_pop_last (ar);
assert (ucl_object_todouble (cur) == 2.0);
ucl_object_unref (cur);
cur = ucl_object_fromdouble (3.0);
ucl_array_prepend (ar, cur);
cur = ucl_array_pop_first (ar);
assert (ucl_object_todouble (cur) == 3.0);
ucl_object_unref (cur);

ucl_object_insert_key (obj, ar, "key4", 0, false);
cur = ucl_object_frombool (true);

ref = ucl_object_ref (cur);
ucl_object_insert_key (obj, cur, "key4", 0, false);

cur = ucl_object_fromstring_common (" ", 0, UCL_STRING_TRIM);
ucl_object_insert_key (obj, cur, "key5", 0, false);
cur = ucl_object_fromstring_common ("", 0, UCL_STRING_ESCAPE);
ucl_object_insert_key (obj, cur, "key6", 0, false);
cur = ucl_object_fromstring_common (" \n", 0, UCL_STRING_ESCAPE);
ucl_object_insert_key (obj, cur, "key7", 0, false);

cur = ucl_object_fromstring_common ("1mb", 0, UCL_STRING_ESCAPE | UCL_STRING_PARSE);
ucl_object_insert_key (obj, cur, "key8", 0, false);
cur = ucl_object_fromstring_common ("3.14", 0, UCL_STRING_PARSE);
ucl_object_insert_key (obj, cur, "key9", 0, false);
cur = ucl_object_fromstring_common ("true", 0, UCL_STRING_PARSE);
ucl_object_insert_key (obj, cur, "key10", 0, false);
cur = ucl_object_fromstring_common (" off ", 0, UCL_STRING_PARSE | UCL_STRING_TRIM);
ucl_object_insert_key (obj, cur, "key11", 0, false);
cur = ucl_object_fromstring_common ("gslin@gslin.org", 0, UCL_STRING_PARSE_INT);
ucl_object_insert_key (obj, cur, "key12", 0, false);
cur = ucl_object_fromstring_common ("#test", 0, UCL_STRING_PARSE_INT);
ucl_object_insert_key (obj, cur, "key13", 0, false);
cur = ucl_object_frombool (true);
ucl_object_insert_key (obj, cur, "k=3", 0, false);
ucl_object_insert_key (obj, ar1, "key14", 0, false);
cur = ucl_object_new_userdata (ud_dtor, ud_emit, NULL);
ucl_object_insert_key (obj, cur, "key15", 0, false);


cur = ucl_object_fromlstring ("test", 3);
ucl_object_insert_key (obj, cur, "key16", 0, false);
test = ucl_object_lookup_any (obj, "key100", "key200", "key300", "key16", NULL);
assert (test == cur);
test = ucl_object_lookup_len (obj, "key160", 5);
assert (test == cur);
cur = ucl_object_pop_key (obj, "key16");
assert (test == cur);
test = ucl_object_pop_key (obj, "key16");
assert (test == NULL);
test = ucl_object_lookup_len (obj, "key160", 5);
assert (test == NULL);

test_obj = ucl_object_new_full (UCL_OBJECT, 2);
ucl_object_insert_key (test_obj, cur, "key16", 0, true);
ucl_object_merge (obj, test_obj, true);
ucl_object_unref (test_obj);

test_obj = ucl_object_new_full (UCL_ARRAY, 3);
ucl_array_append (test_obj, ucl_object_fromstring ("test"));
ucl_array_merge (test_obj, ar1, false);
ucl_object_insert_key (obj, test_obj, "key17", 0, true);

cur = ucl_object_fromstring ("test");
ucl_object_insert_key (obj, cur, "key18", 0, true);
assert (ucl_object_delete_key (obj, "key18"));
assert (!ucl_object_delete_key (obj, "key18"));
cur = ucl_object_fromlstring ("test", 4);
ucl_object_insert_key (obj, cur, "key18\0\0", 7, true);
assert (ucl_object_lookup_len (obj, "key18\0\0", 7) == cur);
assert (ucl_object_lookup (obj, "key18") == NULL);
assert (ucl_object_lookup_len (obj, "key18\0\1", 7) == NULL);
assert (ucl_object_delete_keyl (obj, "key18\0\0", 7));



comments = ucl_object_typed_new (UCL_OBJECT);
found = ucl_object_lookup (obj, "key17");
test = ucl_object_lookup (obj, "key16");
ucl_comments_add (comments, found, "#test comment");
assert (ucl_comments_find (comments, found) != NULL);
assert (ucl_comments_find (comments, test) == NULL);
ucl_comments_move (comments, found, test);
assert (ucl_comments_find (comments, found) == NULL);
assert (ucl_comments_find (comments, test) != NULL);


ar1 = ucl_object_typed_new (UCL_ARRAY);
cur = ucl_object_fromstring ("test");
cur = ucl_elt_append (cur, ucl_object_fromstring ("test1"));
ucl_array_append (ar1, cur);
test = ucl_array_replace_index (ar1, ucl_object_fromstring ("test2"), 0);
assert (test == cur);



found = ucl_object_lookup_path (obj, "key4.1");
assert (found != NULL && ucl_object_toint (found) == 10);

found = ucl_object_lookup_path (obj, ".key4.1");
assert (found != NULL && ucl_object_toint (found) == 10);

found = ucl_object_lookup_path (obj, ".key4........1...");
assert (found != NULL && ucl_object_toint (found) == 10);

found = ucl_object_lookup_path (obj, ".key4.3");
assert (found == NULL);

found = ucl_object_lookup_path (obj, "key9..key1");
assert (found == NULL);


it = ucl_object_iterate_new (obj);
it_obj = ucl_object_iterate_safe (it, true);
assert (!ucl_object_iter_chk_excpn (it));

assert (ucl_object_type (it_obj) == UCL_FLOAT);
it_obj = ucl_object_iterate_safe (it, true);
assert (!ucl_object_iter_chk_excpn (it));

assert (ucl_object_type (it_obj) == UCL_STRING);
it_obj = ucl_object_iterate_safe (it, true);
assert (!ucl_object_iter_chk_excpn (it));

assert (ucl_object_type (it_obj) == UCL_STRING);
it_obj = ucl_object_iterate_safe (it, true);
assert (!ucl_object_iter_chk_excpn (it));

assert (ucl_object_type (it_obj) == UCL_STRING);
it_obj = ucl_object_iterate_safe (it, true);
assert (!ucl_object_iter_chk_excpn (it));

ucl_object_iterate_reset (it, it_obj);
it_obj = ucl_object_iterate_safe (it, true);
assert (!ucl_object_iter_chk_excpn (it));
assert (ucl_object_type (it_obj) == UCL_FLOAT);
it_obj = ucl_object_iterate_safe (it, true);
assert (!ucl_object_iter_chk_excpn (it));
assert (ucl_object_type (it_obj) == UCL_INT);
it_obj = ucl_object_iterate_safe (it, true);
assert (!ucl_object_iter_chk_excpn (it));
assert (ucl_object_type (it_obj) == UCL_FLOAT);
it_obj = ucl_object_iterate_safe (it, true);
assert (!ucl_object_iter_chk_excpn (it));
assert (ucl_object_type (it_obj) == UCL_BOOLEAN);
ucl_object_iterate_free (it);

fn = ucl_object_emit_memory_funcs ((void **)&emitted);
assert (ucl_object_emit_full (obj, UCL_EMIT_CONFIG, fn, comments));
fprintf (out, "%s\n", emitted);
ucl_object_emit_funcs_free (fn);
ucl_object_unref (obj);
ucl_object_unref (comments);

parser = ucl_parser_new (UCL_PARSER_NO_IMPLICIT_ARRAYS);

if (ucl_parser_add_chunk_full (parser, emitted, strlen (emitted),
3, UCL_DUPLICATE_ERROR, UCL_PARSE_UCL)) {

assert (0);
}
else {
assert (ucl_parser_get_error (parser) != NULL);
ucl_parser_clear_error (parser);
ucl_parser_free (parser);
parser = ucl_parser_new (0);
ucl_parser_add_chunk_full (parser, emitted, strlen (emitted),
3, UCL_DUPLICATE_MERGE, UCL_PARSE_UCL);
}

assert (ucl_parser_get_column (parser) == 0);
assert (ucl_parser_get_linenum (parser) != 0);
ucl_parser_clear_error (parser);
assert (ucl_parser_get_error_code (parser) == 0);
obj = ucl_parser_get_object (parser);
ucl_parser_free (parser);
ucl_object_unref (obj);

if (emitted != NULL) {
free (emitted);
}
fclose (out);


ref->value.iv = 100500;
ucl_object_unref (ref);

return ret;
}
