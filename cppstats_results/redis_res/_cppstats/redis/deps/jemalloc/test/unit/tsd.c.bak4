#include "test/jemalloc_test.h"





static bool originally_fast;
static int data_cleanup_count;

void
data_cleanup(int *data) {
if (data_cleanup_count == 0) {
assert_x_eq(*data, MALLOC_TSD_TEST_DATA_INIT,
"Argument passed into cleanup function should match tsd "
"value");
}
++data_cleanup_count;





bool reincarnate = false;
switch (*data) {
case MALLOC_TSD_TEST_DATA_INIT:
*data = 1;
reincarnate = true;
break;
case 1:
*data = 2;
reincarnate = true;
break;
case 2:
return;
default:
not_reached();
}

if (reincarnate) {
void *p = mallocx(1, 0);
assert_ptr_not_null(p, "Unexpeced mallocx() failure");
dallocx(p, 0);
}
}

static void *
thd_start(void *arg) {
int d = (int)(uintptr_t)arg;
void *p;

tsd_t *tsd = tsd_fetch();
assert_x_eq(tsd_test_data_get(tsd), MALLOC_TSD_TEST_DATA_INIT,
"Initial tsd get should return initialization value");

p = malloc(1);
assert_ptr_not_null(p, "Unexpected malloc() failure");

tsd_test_data_set(tsd, d);
assert_x_eq(tsd_test_data_get(tsd), d,
"After tsd set, tsd get should return value that was set");

d = 0;
assert_x_eq(tsd_test_data_get(tsd), (int)(uintptr_t)arg,
"Resetting local data should have no effect on tsd");

tsd_test_callback_set(tsd, &data_cleanup);

free(p);
return NULL;
}

TEST_BEGIN(test_tsd_main_thread) {
thd_start((void *)(uintptr_t)0xa5f3e329);
}
TEST_END

TEST_BEGIN(test_tsd_sub_thread) {
thd_t thd;

data_cleanup_count = 0;
thd_create(&thd, thd_start, (void *)MALLOC_TSD_TEST_DATA_INIT);
thd_join(thd, NULL);




assert_x_ge(data_cleanup_count, 3,
"Cleanup function should have executed multiple times.");
}
TEST_END

static void *
thd_start_reincarnated(void *arg) {
tsd_t *tsd = tsd_fetch();
assert(tsd);

void *p = malloc(1);
assert_ptr_not_null(p, "Unexpected malloc() failure");


assert_ptr_not_null(tsd_arena_get(tsd),
"Should have tsd arena set.");
tsd_cleanup((void *)tsd);
assert_ptr_null(*tsd_arenap_get_unsafe(tsd),
"TSD arena should have been cleared.");
assert_u_eq(tsd_state_get(tsd), tsd_state_purgatory,
"TSD state should be purgatory\n");

free(p);
assert_u_eq(tsd_state_get(tsd), tsd_state_reincarnated,
"TSD state should be reincarnated\n");
p = mallocx(1, MALLOCX_TCACHE_NONE);
assert_ptr_not_null(p, "Unexpected malloc() failure");
assert_ptr_null(*tsd_arenap_get_unsafe(tsd),
"Should not have tsd arena set after reincarnation.");

free(p);
tsd_cleanup((void *)tsd);
assert_ptr_null(*tsd_arenap_get_unsafe(tsd),
"TSD arena should have been cleared after 2nd cleanup.");

return NULL;
}

TEST_BEGIN(test_tsd_reincarnation) {
thd_t thd;
thd_create(&thd, thd_start_reincarnated, NULL);
thd_join(thd, NULL);
}
TEST_END

typedef struct {
atomic_u32_t phase;
atomic_b_t error;
} global_slow_data_t;

static void *
thd_start_global_slow(void *arg) {

global_slow_data_t *data = (global_slow_data_t *)arg;
free(mallocx(1, 0));

tsd_t *tsd = tsd_fetch();




atomic_store_b(&data->error, originally_fast && !tsd_fast(tsd),
ATOMIC_SEQ_CST);
atomic_store_u32(&data->phase, 1, ATOMIC_SEQ_CST);


while (atomic_load_u32(&data->phase, ATOMIC_SEQ_CST) != 2) {
}
free(mallocx(1, 0));
atomic_store_b(&data->error, tsd_fast(tsd), ATOMIC_SEQ_CST);
atomic_store_u32(&data->phase, 3, ATOMIC_SEQ_CST);


while (atomic_load_u32(&data->phase, ATOMIC_SEQ_CST) != 4) {
}
free(mallocx(1, 0));
atomic_store_b(&data->error, tsd_fast(tsd), ATOMIC_SEQ_CST);
atomic_store_u32(&data->phase, 5, ATOMIC_SEQ_CST);


while (atomic_load_u32(&data->phase, ATOMIC_SEQ_CST) != 6) {
}
free(mallocx(1, 0));

atomic_store_b(&data->error, tsd_fast(tsd), ATOMIC_SEQ_CST);
atomic_store_u32(&data->phase, 7, ATOMIC_SEQ_CST);


while (atomic_load_u32(&data->phase, ATOMIC_SEQ_CST) != 8) {
}
free(mallocx(1, 0));




atomic_store_b(&data->error, originally_fast && !tsd_fast(tsd),
ATOMIC_SEQ_CST);
atomic_store_u32(&data->phase, 9, ATOMIC_SEQ_CST);

return NULL;
}

TEST_BEGIN(test_tsd_global_slow) {
global_slow_data_t data = {ATOMIC_INIT(0), ATOMIC_INIT(false)};





free(mallocx(1, 0));
tsd_t *tsd = tsd_fetch();
originally_fast = tsd_fast(tsd);

thd_t thd;
thd_create(&thd, thd_start_global_slow, (void *)&data.phase);

while (atomic_load_u32(&data.phase, ATOMIC_SEQ_CST) != 1) {




}
assert_false(atomic_load_b(&data.error, ATOMIC_SEQ_CST), "");
tsd_global_slow_inc(tsd_tsdn(tsd));
free(mallocx(1, 0));
assert_false(tsd_fast(tsd), "");
atomic_store_u32(&data.phase, 2, ATOMIC_SEQ_CST);


while (atomic_load_u32(&data.phase, ATOMIC_SEQ_CST) != 3) {
}
assert_false(atomic_load_b(&data.error, ATOMIC_SEQ_CST), "");

tsd_global_slow_inc(tsd_tsdn(tsd));
atomic_store_u32(&data.phase, 4, ATOMIC_SEQ_CST);
free(mallocx(1, 0));
assert_false(tsd_fast(tsd), "");


while (atomic_load_u32(&data.phase, ATOMIC_SEQ_CST) != 5) {
}
assert_false(atomic_load_b(&data.error, ATOMIC_SEQ_CST), "");
tsd_global_slow_dec(tsd_tsdn(tsd));
atomic_store_u32(&data.phase, 6, ATOMIC_SEQ_CST);

free(mallocx(1, 0));
assert_false(tsd_fast(tsd), "");


while (atomic_load_u32(&data.phase, ATOMIC_SEQ_CST) != 7) {
}
assert_false(atomic_load_b(&data.error, ATOMIC_SEQ_CST), "");
tsd_global_slow_dec(tsd_tsdn(tsd));
atomic_store_u32(&data.phase, 8, ATOMIC_SEQ_CST);

free(mallocx(1, 0));
assert_true(!originally_fast || tsd_fast(tsd), "");


while (atomic_load_u32(&data.phase, ATOMIC_SEQ_CST) != 9) {
}
assert_false(atomic_load_b(&data.error, ATOMIC_SEQ_CST), "");

thd_join(thd, NULL);
}
TEST_END

int
main(void) {

if (nallocx(1, 0) == 0) {
malloc_printf("Initialization error");
return test_status_fail;
}

return test_no_reentrancy(
test_tsd_main_thread,
test_tsd_sub_thread,
test_tsd_reincarnation,
test_tsd_global_slow);
}
