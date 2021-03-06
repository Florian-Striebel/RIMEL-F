























#include "inner.h"





















































#define SESSION_ID_LEN 32
#define MASTER_SECRET_LEN 48

#define SESSION_ID_OFF 0
#define MASTER_SECRET_OFF 32
#define VERSION_OFF 80
#define CIPHER_SUITE_OFF 82
#define LIST_PREV_OFF 84
#define LIST_NEXT_OFF 88
#define TREE_LEFT_OFF 92
#define TREE_RIGHT_OFF 96

#define LRU_ENTRY_LEN 100

#define ADDR_NULL ((uint32_t)-1)

#define GETSET(name, off) static inline uint32_t get_ ##name(br_ssl_session_cache_lru *cc, uint32_t x) { return br_dec32be(cc->store + x + (off)); } static inline void set_ ##name(br_ssl_session_cache_lru *cc, uint32_t x, uint32_t val) { br_enc32be(cc->store + x + (off), val); }










GETSET(prev, LIST_PREV_OFF)
GETSET(next, LIST_NEXT_OFF)
GETSET(left, TREE_LEFT_OFF)
GETSET(right, TREE_RIGHT_OFF)















static void
mask_id(br_ssl_session_cache_lru *cc,
const unsigned char *src, unsigned char *dst)
{
br_hmac_key_context hkc;
br_hmac_context hc;

memcpy(dst, src, SESSION_ID_LEN);
br_hmac_key_init(&hkc, cc->hash, cc->index_key, sizeof cc->index_key);
br_hmac_init(&hc, &hkc, SESSION_ID_LEN);
br_hmac_update(&hc, src, SESSION_ID_LEN);
br_hmac_out(&hc, dst);
}









static uint32_t
find_node(br_ssl_session_cache_lru *cc, const unsigned char *id,
uint32_t *addr_link)
{
uint32_t x, y;

x = cc->root;
y = ADDR_NULL;
while (x != ADDR_NULL) {
int r;

r = memcmp(id, cc->store + x + SESSION_ID_OFF, SESSION_ID_LEN);
if (r < 0) {
y = x + TREE_LEFT_OFF;
x = get_left(cc, x);
} else if (r == 0) {
if (addr_link != NULL) {
*addr_link = y;
}
return x;
} else {
y = x + TREE_RIGHT_OFF;
x = get_right(cc, x);
}
}
if (addr_link != NULL) {
*addr_link = y;
}
return ADDR_NULL;
}

















static uint32_t
find_replacement_node(br_ssl_session_cache_lru *cc, uint32_t x, uint32_t *al)
{
uint32_t y1, y2;

y1 = get_left(cc, x);
if (y1 != ADDR_NULL) {
y2 = x + TREE_LEFT_OFF;
for (;;) {
uint32_t z;

z = get_right(cc, y1);
if (z == ADDR_NULL) {
*al = y2;
return y1;
}
y2 = y1 + TREE_RIGHT_OFF;
y1 = z;
}
}
y1 = get_right(cc, x);
if (y1 != ADDR_NULL) {
y2 = x + TREE_RIGHT_OFF;
for (;;) {
uint32_t z;

z = get_left(cc, y1);
if (z == ADDR_NULL) {
*al = y2;
return y1;
}
y2 = y1 + TREE_LEFT_OFF;
y1 = z;
}
}
*al = ADDR_NULL;
return ADDR_NULL;
}





static inline void
set_link(br_ssl_session_cache_lru *cc, uint32_t alx, uint32_t x)
{
if (alx == ADDR_NULL) {
cc->root = x;
} else {
br_enc32be(cc->store + alx, x);
}
}





static void
remove_node(br_ssl_session_cache_lru *cc, uint32_t x)
{
uint32_t alx, y, aly;






















find_node(cc, cc->store + x + SESSION_ID_OFF, &alx);






y = find_replacement_node(cc, x, &aly);

if (y != ADDR_NULL) {
uint32_t z;





z = get_left(cc, y);
if (z == ADDR_NULL) {
z = get_right(cc, y);
}
set_link(cc, aly, z);





set_link(cc, alx, y);







set_left(cc, y, get_left(cc, x));
set_right(cc, y, get_right(cc, x));
} else {



set_link(cc, alx, ADDR_NULL);
}
}

static void
lru_save(const br_ssl_session_cache_class **ctx,
br_ssl_server_context *server_ctx,
const br_ssl_session_parameters *params)
{
br_ssl_session_cache_lru *cc;
unsigned char id[SESSION_ID_LEN];
uint32_t x, alx;

cc = (br_ssl_session_cache_lru *)ctx;





if (cc->store_len < LRU_ENTRY_LEN) {
return;
}





if (!cc->init_done) {
br_hmac_drbg_generate(&server_ctx->eng.rng,
cc->index_key, sizeof cc->index_key);
cc->hash = br_hmac_drbg_get_hash(&server_ctx->eng.rng);
cc->init_done = 1;
}
mask_id(cc, params->session_id, id);








if (find_node(cc, id, NULL) != ADDR_NULL) {
return;
}









if (cc->store_ptr > (cc->store_len - LRU_ENTRY_LEN)) {




x = cc->tail;
cc->tail = get_prev(cc, x);
if (cc->tail == ADDR_NULL) {
cc->head = ADDR_NULL;
} else {
set_next(cc, cc->tail, ADDR_NULL);
}




remove_node(cc, x);
} else {



x = cc->store_ptr;
cc->store_ptr += LRU_ENTRY_LEN;
}




find_node(cc, id, &alx);
set_link(cc, alx, x);
set_left(cc, x, ADDR_NULL);
set_right(cc, x, ADDR_NULL);





if (cc->head == ADDR_NULL) {
cc->tail = x;
} else {
set_prev(cc, cc->head, x);
}
set_prev(cc, x, ADDR_NULL);
set_next(cc, x, cc->head);
cc->head = x;




memcpy(cc->store + x + SESSION_ID_OFF, id, SESSION_ID_LEN);
memcpy(cc->store + x + MASTER_SECRET_OFF,
params->master_secret, MASTER_SECRET_LEN);
br_enc16be(cc->store + x + VERSION_OFF, params->version);
br_enc16be(cc->store + x + CIPHER_SUITE_OFF, params->cipher_suite);
}

static int
lru_load(const br_ssl_session_cache_class **ctx,
br_ssl_server_context *server_ctx,
br_ssl_session_parameters *params)
{
br_ssl_session_cache_lru *cc;
unsigned char id[SESSION_ID_LEN];
uint32_t x;

(void)server_ctx;
cc = (br_ssl_session_cache_lru *)ctx;
if (!cc->init_done) {
return 0;
}
mask_id(cc, params->session_id, id);
x = find_node(cc, id, NULL);
if (x != ADDR_NULL) {
unsigned version;

version = br_dec16be(cc->store + x + VERSION_OFF);
if (version == 0) {





return 0;
}
params->version = version;
params->cipher_suite = br_dec16be(
cc->store + x + CIPHER_SUITE_OFF);
memcpy(params->master_secret,
cc->store + x + MASTER_SECRET_OFF,
MASTER_SECRET_LEN);
if (x != cc->head) {




uint32_t p, n;

p = get_prev(cc, x);
n = get_next(cc, x);
set_next(cc, p, n);
if (n == ADDR_NULL) {
cc->tail = p;
} else {
set_prev(cc, n, p);
}
set_prev(cc, cc->head, x);
set_next(cc, x, cc->head);
set_prev(cc, x, ADDR_NULL);
cc->head = x;
}
return 1;
}
return 0;
}

static const br_ssl_session_cache_class lru_class = {
sizeof(br_ssl_session_cache_lru),
&lru_save,
&lru_load
};


void
br_ssl_session_cache_lru_init(br_ssl_session_cache_lru *cc,
unsigned char *store, size_t store_len)
{
cc->vtable = &lru_class;
cc->store = store;
cc->store_len = store_len;
cc->store_ptr = 0;
cc->init_done = 0;
cc->head = ADDR_NULL;
cc->tail = ADDR_NULL;
cc->root = ADDR_NULL;
}


void br_ssl_session_cache_lru_forget(
br_ssl_session_cache_lru *cc, const unsigned char *id)
{
unsigned char mid[SESSION_ID_LEN];
uint32_t addr;





if (!cc->init_done) {
return;
}











mask_id(cc, id, mid);
addr = find_node(cc, mid, NULL);
if (addr != ADDR_NULL) {
br_enc16be(cc->store + addr + VERSION_OFF, 0);
}
}
