






















#include <stdio.h>
#include <errno.h>
#include <assert.h>
#include "ucl.h"

int
main (int argc, char **argv)
{
ucl_object_t *obj, *cur, *ar;
FILE *out;
const char *fname_out = NULL;
struct ucl_emitter_context *ctx;
struct ucl_emitter_functions *f;
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


cur = ucl_object_fromstring_common (" test string ", 0, UCL_STRING_TRIM);
ucl_object_insert_key (obj, cur, "key1", 0, false);
cur = ucl_object_fromstring_common (" test \nstring\n ", 0, UCL_STRING_TRIM | UCL_STRING_ESCAPE);
ucl_object_insert_key (obj, cur, "key2", 0, false);
cur = ucl_object_fromstring_common (" test string \n", 0, 0);
ucl_object_insert_key (obj, cur, "key3", 0, false);

f = ucl_object_emit_file_funcs (out);
ctx = ucl_object_emit_streamline_new (obj, UCL_EMIT_CONFIG, f);

assert (ctx != NULL);


ar = ucl_object_typed_new (UCL_ARRAY);
ar->key = "key4";
ar->keylen = sizeof ("key4") - 1;

ucl_object_emit_streamline_start_container (ctx, ar);
cur = ucl_object_fromint (10);
ucl_object_emit_streamline_add_object (ctx, cur);
cur = ucl_object_fromdouble (10.1);
ucl_object_emit_streamline_add_object (ctx, cur);
cur = ucl_object_fromdouble (9.999);
ucl_object_emit_streamline_add_object (ctx, cur);


ucl_object_emit_streamline_end_container (ctx);
ucl_object_emit_streamline_finish (ctx);
ucl_object_emit_funcs_free (f);
ucl_object_unref (obj);

fclose (out);

return ret;
}
