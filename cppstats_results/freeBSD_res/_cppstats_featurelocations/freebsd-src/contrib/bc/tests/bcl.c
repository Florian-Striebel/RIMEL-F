


































#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <bcl.h>





static void err(BclError e) {
if (e != BCL_ERROR_NONE) abort();
}

int main(void) {

BclError e;
BclContext ctxt;
size_t scale;
BclNumber n, n2, n3, n4, n5, n6;
char* res;
BclBigDig b = 0;


e = bcl_init();
err(e);
e = bcl_init();
err(e);



if (bcl_abortOnFatalError()) err(BCL_ERROR_FATAL_UNKNOWN_ERR);

bcl_setAbortOnFatalError(true);


if (!bcl_abortOnFatalError()) err(BCL_ERROR_FATAL_UNKNOWN_ERR);


ctxt = bcl_ctxt_create();
bcl_pushContext(ctxt);
ctxt = bcl_ctxt_create();
bcl_pushContext(ctxt);


scale = 10;
bcl_ctxt_setScale(ctxt, scale);
scale = bcl_ctxt_scale(ctxt);
if (scale != 10) err(BCL_ERROR_FATAL_UNKNOWN_ERR);

scale = 16;
bcl_ctxt_setIbase(ctxt, scale);
scale = bcl_ctxt_ibase(ctxt);
if (scale != 16) err(BCL_ERROR_FATAL_UNKNOWN_ERR);


bcl_ctxt_setObase(ctxt, scale);
scale = bcl_ctxt_obase(ctxt);
if (scale != 16) err(BCL_ERROR_FATAL_UNKNOWN_ERR);


bcl_ctxt_setIbase(ctxt, 10);
scale = bcl_ctxt_ibase(ctxt);
if (scale != 10) err(BCL_ERROR_FATAL_UNKNOWN_ERR);
bcl_ctxt_setObase(ctxt, 10);
scale = bcl_ctxt_obase(ctxt);
if (scale != 10) err(BCL_ERROR_FATAL_UNKNOWN_ERR);


n = bcl_num_create();
n2 = bcl_dup(n);
bcl_copy(n, n2);


n3 = bcl_parse("2938");
err(bcl_err(n3));
n4 = bcl_parse("-28390.9108273");
err(bcl_err(n4));




if (!bcl_num_neg(n4)) err(BCL_ERROR_FATAL_UNKNOWN_ERR);


n3 = bcl_add(n3, n4);
err(bcl_err(n3));
res = bcl_string(bcl_dup(n3));
if (strcmp(res, "-25452.9108273")) err(BCL_ERROR_FATAL_UNKNOWN_ERR);



free(res);


n4 = bcl_parse("8937458902.2890347");
err(bcl_err(n4));
e = bcl_divmod(bcl_dup(n4), n3, &n5, &n6);
err(e);

res = bcl_string(n5);

if (strcmp(res, "-351137.0060159482"))
err(BCL_ERROR_FATAL_UNKNOWN_ERR);

free(res);

res = bcl_string(n6);

if (strcmp(res, ".00000152374405414"))
err(BCL_ERROR_FATAL_UNKNOWN_ERR);

free(res);





n4 = bcl_sqrt(n4);
err(bcl_err(n4));

res = bcl_string(bcl_dup(n4));

if (strcmp(res, "94538.1346457028"))
err(BCL_ERROR_FATAL_UNKNOWN_ERR);

free(res);


e = bcl_num_setScale(n4, 20);
err(e);

res = bcl_string(bcl_dup(n4));

if (strcmp(res, "94538.13464570280000000000"))
err(BCL_ERROR_FATAL_UNKNOWN_ERR);

free(res);


e = bcl_num_setScale(n4, 0);
err(e);

res = bcl_string(bcl_dup(n4));

if (strcmp(res, "94538"))
err(BCL_ERROR_FATAL_UNKNOWN_ERR);

free(res);


e = bcl_bigdig(n4, &b);
err(e);

if (b != 94538) err(BCL_ERROR_FATAL_UNKNOWN_ERR);


n4 = bcl_bigdig2num(b);
err(bcl_err(n4));

res = bcl_string(bcl_dup(n4));

if (strcmp(res, "94538"))
err(BCL_ERROR_FATAL_UNKNOWN_ERR);

free(res);


n4 = bcl_frand(10);
err(bcl_err(n4));


n4 = bcl_lshift(n4, bcl_bigdig2num(10));
err(bcl_err(n4));


n3 = bcl_irand(n4);
err(bcl_err(n3));


n2 = bcl_ifrand(bcl_dup(n3), 10);
err(bcl_err(n2));


e = bcl_rand_seedWithNum(n3);
err(e);


n4 = bcl_rand_seed2num();
err(bcl_err(n4));


n5 = bcl_parse("10");
err(bcl_err(n5));

n6 = bcl_modexp(bcl_dup(n5), bcl_dup(n5), bcl_dup(n5));
err(bcl_err(n6));


bcl_num_free(n);


if (bcl_leadingZeroes())
err(BCL_ERROR_FATAL_UNKNOWN_ERR);

n = bcl_parse("0.01");
err(bcl_err(n));

n2 = bcl_parse("-0.01");
err(bcl_err(n2));

n3 = bcl_parse("1.01");
err(bcl_err(n3));

n4 = bcl_parse("-1.01");
err(bcl_err(n4));

res = bcl_string(bcl_dup(n));
if (strcmp(res, ".01"))
err(BCL_ERROR_FATAL_UNKNOWN_ERR);

free(res);

res = bcl_string(bcl_dup(n2));
if (strcmp(res, "-.01"))
err(BCL_ERROR_FATAL_UNKNOWN_ERR);

free(res);

res = bcl_string(bcl_dup(n3));
if (strcmp(res, "1.01"))
err(BCL_ERROR_FATAL_UNKNOWN_ERR);

free(res);

res = bcl_string(bcl_dup(n4));
if (strcmp(res, "-1.01"))
err(BCL_ERROR_FATAL_UNKNOWN_ERR);

free(res);

bcl_setLeadingZeroes(true);

if (!bcl_leadingZeroes())
err(BCL_ERROR_FATAL_UNKNOWN_ERR);

res = bcl_string(bcl_dup(n));
if (strcmp(res, "0.01"))
err(BCL_ERROR_FATAL_UNKNOWN_ERR);

free(res);

res = bcl_string(bcl_dup(n2));
if (strcmp(res, "-0.01"))
err(BCL_ERROR_FATAL_UNKNOWN_ERR);

free(res);

res = bcl_string(bcl_dup(n3));
if (strcmp(res, "1.01"))
err(BCL_ERROR_FATAL_UNKNOWN_ERR);

free(res);

res = bcl_string(bcl_dup(n4));
if (strcmp(res, "-1.01"))
err(BCL_ERROR_FATAL_UNKNOWN_ERR);

free(res);

bcl_setLeadingZeroes(false);

if (bcl_leadingZeroes())
err(BCL_ERROR_FATAL_UNKNOWN_ERR);

res = bcl_string(n);
if (strcmp(res, ".01"))
err(BCL_ERROR_FATAL_UNKNOWN_ERR);

free(res);

res = bcl_string(n2);
if (strcmp(res, "-.01"))
err(BCL_ERROR_FATAL_UNKNOWN_ERR);

free(res);

res = bcl_string(n3);
if (strcmp(res, "1.01"))
err(BCL_ERROR_FATAL_UNKNOWN_ERR);

free(res);

res = bcl_string(n4);
if (strcmp(res, "-1.01"))
err(BCL_ERROR_FATAL_UNKNOWN_ERR);

free(res);

bcl_ctxt_freeNums(ctxt);

bcl_gc();


bcl_popContext();

bcl_ctxt_free(ctxt);

ctxt = bcl_context();

bcl_popContext();

bcl_ctxt_free(ctxt);


bcl_free();

bcl_free();

return 0;
}
