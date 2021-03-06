























#include "inner.h"

#if 0






#if !defined(BR_USE_URANDOM)








#if defined _AIX || defined __ANDROID__ || defined __FreeBSD__ || defined __NetBSD__ || defined __OpenBSD__ || defined __DragonFly__ || defined __linux__ || (defined __sun && (defined __SVR4 || defined __svr4__)) || (defined __APPLE__ && defined __MACH__)








#define BR_USE_URANDOM 1
#endif

#endif




#if !defined(BR_USE_WIN32_RAND)

#if defined _WIN32 || defined _WIN64
#define BR_USE_WIN32_RAND 1
#endif

#endif

#if BR_USE_URANDOM
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#endif

#if BR_USE_WIN32_RAND
#include <windows.h>
#include <wincrypt.h>
#pragma comment(lib, "advapi32")
#endif

#endif








































































































































































































#define MAX_OUT_OVERHEAD 85
#define MAX_IN_OVERHEAD 325


void
br_ssl_engine_fail(br_ssl_engine_context *rc, int err)
{
if (rc->iomode != BR_IO_FAILED) {
rc->iomode = BR_IO_FAILED;
rc->err = err;
}
}




static void
make_ready_in(br_ssl_engine_context *rc)
{
rc->ixa = rc->ixb = 0;
rc->ixc = 5;
if (rc->iomode == BR_IO_IN) {
rc->iomode = BR_IO_INOUT;
}
}




static void
make_ready_out(br_ssl_engine_context *rc)
{
size_t a, b;

a = 5;
b = rc->obuf_len - a;
rc->out.vtable->max_plaintext(&rc->out.vtable, &a, &b);
if ((b - a) > rc->max_frag_len) {
b = a + rc->max_frag_len;
}
rc->oxa = a;
rc->oxb = b;
rc->oxc = a;
if (rc->iomode == BR_IO_OUT) {
rc->iomode = BR_IO_INOUT;
}
}


void
br_ssl_engine_new_max_frag_len(br_ssl_engine_context *rc, unsigned max_frag_len)
{
size_t nxb;

rc->max_frag_len = max_frag_len;
nxb = rc->oxc + max_frag_len;
if (rc->oxa < rc->oxb && rc->oxb > nxb && rc->oxa < nxb) {
rc->oxb = nxb;
}
}


void
br_ssl_engine_set_buffer(br_ssl_engine_context *rc,
void *buf, size_t buf_len, int bidi)
{
if (buf == NULL) {
br_ssl_engine_set_buffers_bidi(rc, NULL, 0, NULL, 0);
} else {
















if (bidi) {
size_t w;

if (buf_len < (512 + MAX_IN_OVERHEAD
+ 512 + MAX_OUT_OVERHEAD))
{
rc->iomode = BR_IO_FAILED;
rc->err = BR_ERR_BAD_PARAM;
return;
} else if (buf_len < (16384 + MAX_IN_OVERHEAD
+ 512 + MAX_OUT_OVERHEAD))
{
w = 512 + MAX_OUT_OVERHEAD;
} else {
w = buf_len - (16384 + MAX_IN_OVERHEAD);
}
br_ssl_engine_set_buffers_bidi(rc,
buf, buf_len - w,
(unsigned char *)buf + w, w);
} else {
br_ssl_engine_set_buffers_bidi(rc,
buf, buf_len, NULL, 0);
}
}
}


void
br_ssl_engine_set_buffers_bidi(br_ssl_engine_context *rc,
void *ibuf, size_t ibuf_len, void *obuf, size_t obuf_len)
{
rc->iomode = BR_IO_INOUT;
rc->incrypt = 0;
rc->err = BR_ERR_OK;
rc->version_in = 0;
rc->record_type_in = 0;
rc->version_out = 0;
rc->record_type_out = 0;
if (ibuf == NULL) {
if (rc->ibuf == NULL) {
br_ssl_engine_fail(rc, BR_ERR_BAD_PARAM);
}
} else {
unsigned u;

rc->ibuf = ibuf;
rc->ibuf_len = ibuf_len;
if (obuf == NULL) {
obuf = ibuf;
obuf_len = ibuf_len;
}
rc->obuf = obuf;
rc->obuf_len = obuf_len;









for (u = 14; u >= 9; u --) {
size_t flen;

flen = (size_t)1 << u;
if (obuf_len >= flen + MAX_OUT_OVERHEAD
&& ibuf_len >= flen + MAX_IN_OVERHEAD)
{
break;
}
}
if (u == 8) {
br_ssl_engine_fail(rc, BR_ERR_BAD_PARAM);
return;
} else if (u == 13) {
u = 12;
}
rc->max_frag_len = (size_t)1 << u;
rc->log_max_frag_len = u;
rc->peer_log_max_frag_len = 0;
}
rc->out.vtable = &br_sslrec_out_clear_vtable;
make_ready_in(rc);
make_ready_out(rc);
}




static void
engine_clearbuf(br_ssl_engine_context *rc)
{
make_ready_in(rc);
make_ready_out(rc);
}





static int
rng_init(br_ssl_engine_context *cc)
{
const br_hash_class *h;

if (cc->rng_init_done != 0) {
return 1;
}













h = br_multihash_getimpl(&cc->mhash, br_sha256_ID);
if (!h) {
h = br_multihash_getimpl(&cc->mhash, br_sha384_ID);
if (!h) {
h = br_multihash_getimpl(&cc->mhash,
br_sha1_ID);
if (!h) {
br_ssl_engine_fail(cc, BR_ERR_BAD_STATE);
return 0;
}
}
}
br_hmac_drbg_init(&cc->rng, h, NULL, 0);
cc->rng_init_done = 1;
return 1;
}


int
br_ssl_engine_init_rand(br_ssl_engine_context *cc)
{
if (!rng_init(cc)) {
return 0;
}






if (!cc->rng_os_rand_done) {
br_prng_seeder sd;

sd = br_prng_seeder_system(NULL);
if (sd != 0 && sd(&cc->rng.vtable)) {
cc->rng_init_done = 2;
}
cc->rng_os_rand_done = 1;
}
if (cc->rng_init_done < 2) {
br_ssl_engine_fail(cc, BR_ERR_NO_RANDOM);
return 0;
}
return 1;
}


void
br_ssl_engine_inject_entropy(br_ssl_engine_context *cc,
const void *data, size_t len)
{






if (!rng_init(cc)) {
return;
}
br_hmac_drbg_update(&cc->rng, data, len);
cc->rng_init_done = 2;
}













static unsigned char *
recvrec_buf(const br_ssl_engine_context *rc, size_t *len)
{
if (rc->shutdown_recv) {
*len = 0;
return NULL;
}











switch (rc->iomode) {
case BR_IO_IN:
case BR_IO_INOUT:
if (rc->ixa == rc->ixb) {
size_t z;

z = rc->ixc;
if (z > rc->ibuf_len - rc->ixa) {
z = rc->ibuf_len - rc->ixa;
}
*len = z;
return rc->ibuf + rc->ixa;
}
break;
}
*len = 0;
return NULL;
}

static void
recvrec_ack(br_ssl_engine_context *rc, size_t len)
{
unsigned char *pbuf;
size_t pbuf_len;






if (rc->iomode == BR_IO_INOUT && rc->ibuf == rc->obuf) {
rc->iomode = BR_IO_IN;
}




rc->ixb = (rc->ixa += len);
rc->ixc -= len;





if (rc->ixa < 5) {
return;
}




if (rc->ixa == 5) {
unsigned version;
unsigned rlen;













rc->record_type_in = rc->ibuf[0];
version = br_dec16be(rc->ibuf + 1);
if ((version >> 8) != 3) {
br_ssl_engine_fail(rc, BR_ERR_UNSUPPORTED_VERSION);
return;
}







if (rc->version_in != 0 && rc->version_in != version) {
br_ssl_engine_fail(rc, BR_ERR_BAD_VERSION);
return;
}
rc->version_in = version;











rlen = br_dec16be(rc->ibuf + 3);
if (rc->incrypt) {
if (!rc->in.vtable->check_length(
&rc->in.vtable, rlen))
{
br_ssl_engine_fail(rc, BR_ERR_BAD_LENGTH);
return;
}
if (rlen > (rc->ibuf_len - 5)) {
br_ssl_engine_fail(rc, BR_ERR_TOO_LARGE);
return;
}
} else {
if (rlen > 16384) {
br_ssl_engine_fail(rc, BR_ERR_BAD_LENGTH);
return;
}
}














if (rlen == 0) {
make_ready_in(rc);
} else {
rc->ixa = rc->ixb = 5;
rc->ixc = rlen;
}
return;
}







if (!rc->incrypt) {
rc->ixa = 5;
return;
}





if (rc->ixc != 0) {
return;
}




pbuf_len = rc->ixa - 5;
pbuf = rc->in.vtable->decrypt(&rc->in.vtable,
rc->record_type_in, rc->version_in, rc->ibuf + 5, &pbuf_len);
if (pbuf == 0) {
br_ssl_engine_fail(rc, BR_ERR_BAD_MAC);
return;
}
rc->ixa = (size_t)(pbuf - rc->ibuf);
rc->ixb = rc->ixa + pbuf_len;





if (rc->ixa == rc->ixb) {
make_ready_in(rc);
}
}


int
br_ssl_engine_recvrec_finished(const br_ssl_engine_context *rc)
{
switch (rc->iomode) {
case BR_IO_IN:
case BR_IO_INOUT:
return rc->ixc == 0 || rc->ixa < 5;
default:
return 1;
}
}

static unsigned char *
recvpld_buf(const br_ssl_engine_context *rc, size_t *len)
{




switch (rc->iomode) {
case BR_IO_IN:
case BR_IO_INOUT:
*len = rc->ixb - rc->ixa;
return (*len == 0) ? NULL : (rc->ibuf + rc->ixa);
default:
*len = 0;
return NULL;
}
}

static void
recvpld_ack(br_ssl_engine_context *rc, size_t len)
{
rc->ixa += len;







if (rc->ixa == rc->ixb) {
if (rc->ixc == 0) {
make_ready_in(rc);
} else {
rc->ixa = rc->ixb = 5;
}
}
}

static unsigned char *
sendpld_buf(const br_ssl_engine_context *rc, size_t *len)
{




switch (rc->iomode) {
case BR_IO_OUT:
case BR_IO_INOUT:
*len = rc->oxb - rc->oxa;
return (*len == 0) ? NULL : (rc->obuf + rc->oxa);
default:
*len = 0;
return NULL;
}
}









static void
sendpld_flush(br_ssl_engine_context *rc, int force)
{
size_t xlen;
unsigned char *buf;

if (rc->oxa == rc->oxb) {
return;
}
xlen = rc->oxa - rc->oxc;
if (xlen == 0 && !force) {
return;
}
buf = rc->out.vtable->encrypt(&rc->out.vtable,
rc->record_type_out, rc->version_out,
rc->obuf + rc->oxc, &xlen);
rc->oxb = rc->oxa = (size_t)(buf - rc->obuf);
rc->oxc = rc->oxa + xlen;
}

static void
sendpld_ack(br_ssl_engine_context *rc, size_t len)
{




if (rc->iomode == BR_IO_INOUT && rc->ibuf == rc->obuf) {
rc->iomode = BR_IO_OUT;
}
rc->oxa += len;
if (rc->oxa >= rc->oxb) {





rc->oxb = rc->oxa + 1;
sendpld_flush(rc, 0);
}
}

static unsigned char *
sendrec_buf(const br_ssl_engine_context *rc, size_t *len)
{






switch (rc->iomode) {
case BR_IO_OUT:
case BR_IO_INOUT:
if (rc->oxc > rc->oxa) {
*len = rc->oxc - rc->oxa;
return rc->obuf + rc->oxa;
}
break;
}
*len = 0;
return NULL;
}

static void
sendrec_ack(br_ssl_engine_context *rc, size_t len)
{
rc->oxb = (rc->oxa += len);
if (rc->oxa == rc->oxc) {
make_ready_out(rc);
}
}





static inline int
has_rec_tosend(const br_ssl_engine_context *rc)
{
return rc->oxa == rc->oxb && rc->oxa != rc->oxc;
}






static void
clear_max_plaintext(const br_sslrec_out_clear_context *cc,
size_t *start, size_t *end)
{
size_t len;

(void)cc;
len = *end - *start;
if (len > 16384) {
*end = *start + 16384;
}
}





static unsigned char *
clear_encrypt(br_sslrec_out_clear_context *cc,
int record_type, unsigned version, void *data, size_t *data_len)
{
unsigned char *buf;

(void)cc;
buf = (unsigned char *)data - 5;
buf[0] = record_type;
br_enc16be(buf + 1, version);
br_enc16be(buf + 3, *data_len);
*data_len += 5;
return buf;
}


const br_sslrec_out_class br_sslrec_out_clear_vtable = {
sizeof(br_sslrec_out_clear_context),
(void (*)(const br_sslrec_out_class *const *, size_t *, size_t *))
&clear_max_plaintext,
(unsigned char *(*)(const br_sslrec_out_class **,
int, unsigned, void *, size_t *))
&clear_encrypt
};




















































void
br_ssl_engine_set_suites(br_ssl_engine_context *cc,
const uint16_t *suites, size_t suites_num)
{
if ((suites_num * sizeof *suites) > sizeof cc->suites_buf) {
br_ssl_engine_fail(cc, BR_ERR_BAD_PARAM);
return;
}
memcpy(cc->suites_buf, suites, suites_num * sizeof *suites);
cc->suites_num = suites_num;
}





static void
jump_handshake(br_ssl_engine_context *cc, int action)
{






for (;;) {
size_t hlen_in, hlen_out;







cc->hbuf_in = recvpld_buf(cc, &hlen_in);
if (cc->hbuf_in != NULL
&& cc->record_type_in == BR_SSL_APPLICATION_DATA)
{
hlen_in = 0;
}







cc->saved_hbuf_out = cc->hbuf_out = sendpld_buf(cc, &hlen_out);
if (cc->hbuf_out != NULL && br_ssl_engine_has_pld_to_send(cc)) {
hlen_out = 0;
}







cc->hlen_in = hlen_in;
cc->hlen_out = hlen_out;
cc->action = action;
cc->hsrun(&cc->cpu);
if (br_ssl_engine_closed(cc)) {
return;
}
if (cc->hbuf_out != cc->saved_hbuf_out) {
sendpld_ack(cc, cc->hbuf_out - cc->saved_hbuf_out);
}
if (hlen_in != cc->hlen_in) {
recvpld_ack(cc, hlen_in - cc->hlen_in);
if (cc->hlen_in == 0) {







action = 0;
continue;
}
}
break;
}
}


void
br_ssl_engine_flush_record(br_ssl_engine_context *cc)
{
if (cc->hbuf_out != cc->saved_hbuf_out) {
sendpld_ack(cc, cc->hbuf_out - cc->saved_hbuf_out);
}
if (br_ssl_engine_has_pld_to_send(cc)) {
sendpld_flush(cc, 0);
}
cc->saved_hbuf_out = cc->hbuf_out = sendpld_buf(cc, &cc->hlen_out);
}


unsigned char *
br_ssl_engine_sendapp_buf(const br_ssl_engine_context *cc, size_t *len)
{
if (!(cc->application_data & 1)) {
*len = 0;
return NULL;
}
return sendpld_buf(cc, len);
}


void
br_ssl_engine_sendapp_ack(br_ssl_engine_context *cc, size_t len)
{
sendpld_ack(cc, len);
}


unsigned char *
br_ssl_engine_recvapp_buf(const br_ssl_engine_context *cc, size_t *len)
{
if (!(cc->application_data & 1)
|| cc->record_type_in != BR_SSL_APPLICATION_DATA)
{
*len = 0;
return NULL;
}
return recvpld_buf(cc, len);
}


void
br_ssl_engine_recvapp_ack(br_ssl_engine_context *cc, size_t len)
{
recvpld_ack(cc, len);
}


unsigned char *
br_ssl_engine_sendrec_buf(const br_ssl_engine_context *cc, size_t *len)
{
return sendrec_buf(cc, len);
}


void
br_ssl_engine_sendrec_ack(br_ssl_engine_context *cc, size_t len)
{
sendrec_ack(cc, len);
if (len != 0 && !has_rec_tosend(cc)
&& (cc->record_type_out != BR_SSL_APPLICATION_DATA
|| (cc->application_data & 1) == 0))
{
jump_handshake(cc, 0);
}
}


unsigned char *
br_ssl_engine_recvrec_buf(const br_ssl_engine_context *cc, size_t *len)
{
return recvrec_buf(cc, len);
}


void
br_ssl_engine_recvrec_ack(br_ssl_engine_context *cc, size_t len)
{
unsigned char *buf;

recvrec_ack(cc, len);
if (br_ssl_engine_closed(cc)) {
return;
}






buf = recvpld_buf(cc, &len);
if (buf != NULL) {
switch (cc->record_type_in) {
case BR_SSL_CHANGE_CIPHER_SPEC:
case BR_SSL_ALERT:
case BR_SSL_HANDSHAKE:
jump_handshake(cc, 0);
break;
case BR_SSL_APPLICATION_DATA:
if (cc->application_data == 1) {
break;
}






if (cc->application_data == 2) {
recvpld_ack(cc, len);
break;
}


default:
br_ssl_engine_fail(cc, BR_ERR_UNEXPECTED);
break;
}
}
}


void
br_ssl_engine_close(br_ssl_engine_context *cc)
{
if (!br_ssl_engine_closed(cc)) {
jump_handshake(cc, 1);
}
}


int
br_ssl_engine_renegotiate(br_ssl_engine_context *cc)
{
size_t len;

if (br_ssl_engine_closed(cc) || cc->reneg == 1
|| (cc->flags & BR_OPT_NO_RENEGOTIATION) != 0
|| br_ssl_engine_recvapp_buf(cc, &len) != NULL)
{
return 0;
}
jump_handshake(cc, 2);
return 1;
}


unsigned
br_ssl_engine_current_state(const br_ssl_engine_context *cc)
{
unsigned s;
size_t len;

if (br_ssl_engine_closed(cc)) {
return BR_SSL_CLOSED;
}

s = 0;
if (br_ssl_engine_sendrec_buf(cc, &len) != NULL) {
s |= BR_SSL_SENDREC;
}
if (br_ssl_engine_recvrec_buf(cc, &len) != NULL) {
s |= BR_SSL_RECVREC;
}
if (br_ssl_engine_sendapp_buf(cc, &len) != NULL) {
s |= BR_SSL_SENDAPP;
}
if (br_ssl_engine_recvapp_buf(cc, &len) != NULL) {
s |= BR_SSL_RECVAPP;
}
return s;
}


void
br_ssl_engine_flush(br_ssl_engine_context *cc, int force)
{
if (!br_ssl_engine_closed(cc) && (cc->application_data & 1) != 0) {
sendpld_flush(cc, force);
}
}


void
br_ssl_engine_hs_reset(br_ssl_engine_context *cc,
void (*hsinit)(void *), void (*hsrun)(void *))
{
engine_clearbuf(cc);
cc->cpu.dp = cc->dp_stack;
cc->cpu.rp = cc->rp_stack;
hsinit(&cc->cpu);
cc->hsrun = hsrun;
cc->shutdown_recv = 0;
cc->application_data = 0;
cc->alert = 0;
jump_handshake(cc, 0);
}


br_tls_prf_impl
br_ssl_engine_get_PRF(br_ssl_engine_context *cc, int prf_id)
{
if (cc->session.version >= BR_TLS12) {
if (prf_id == br_sha384_ID) {
return cc->prf_sha384;
} else {
return cc->prf_sha256;
}
} else {
return cc->prf10;
}
}


void
br_ssl_engine_compute_master(br_ssl_engine_context *cc,
int prf_id, const void *pms, size_t pms_len)
{
br_tls_prf_impl iprf;
br_tls_prf_seed_chunk seed[2] = {
{ cc->client_random, sizeof cc->client_random },
{ cc->server_random, sizeof cc->server_random }
};

iprf = br_ssl_engine_get_PRF(cc, prf_id);
iprf(cc->session.master_secret, sizeof cc->session.master_secret,
pms, pms_len, "master secret", 2, seed);
}




static void
compute_key_block(br_ssl_engine_context *cc, int prf_id,
size_t half_len, unsigned char *kb)
{
br_tls_prf_impl iprf;
br_tls_prf_seed_chunk seed[2] = {
{ cc->server_random, sizeof cc->server_random },
{ cc->client_random, sizeof cc->client_random }
};

iprf = br_ssl_engine_get_PRF(cc, prf_id);
iprf(kb, half_len << 1,
cc->session.master_secret, sizeof cc->session.master_secret,
"key expansion", 2, seed);
}


void
br_ssl_engine_switch_cbc_in(br_ssl_engine_context *cc,
int is_client, int prf_id, int mac_id,
const br_block_cbcdec_class *bc_impl, size_t cipher_key_len)
{
unsigned char kb[192];
unsigned char *cipher_key, *mac_key, *iv;
const br_hash_class *imh;
size_t mac_key_len, mac_out_len, iv_len;

imh = br_ssl_engine_get_hash(cc, mac_id);
mac_out_len = (imh->desc >> BR_HASHDESC_OUT_OFF) & BR_HASHDESC_OUT_MASK;
mac_key_len = mac_out_len;




if (cc->session.version >= BR_TLS11) {
iv_len = 0;
} else {
iv_len = bc_impl->block_size;
}
compute_key_block(cc, prf_id,
mac_key_len + cipher_key_len + iv_len, kb);
if (is_client) {
mac_key = &kb[mac_key_len];
cipher_key = &kb[(mac_key_len << 1) + cipher_key_len];
iv = &kb[((mac_key_len + cipher_key_len) << 1) + iv_len];
} else {
mac_key = &kb[0];
cipher_key = &kb[mac_key_len << 1];
iv = &kb[(mac_key_len + cipher_key_len) << 1];
}
if (iv_len == 0) {
iv = NULL;
}
cc->icbc_in->init(&cc->in.cbc.vtable,
bc_impl, cipher_key, cipher_key_len,
imh, mac_key, mac_key_len, mac_out_len, iv);
cc->incrypt = 1;
}


void
br_ssl_engine_switch_cbc_out(br_ssl_engine_context *cc,
int is_client, int prf_id, int mac_id,
const br_block_cbcenc_class *bc_impl, size_t cipher_key_len)
{
unsigned char kb[192];
unsigned char *cipher_key, *mac_key, *iv;
const br_hash_class *imh;
size_t mac_key_len, mac_out_len, iv_len;

imh = br_ssl_engine_get_hash(cc, mac_id);
mac_out_len = (imh->desc >> BR_HASHDESC_OUT_OFF) & BR_HASHDESC_OUT_MASK;
mac_key_len = mac_out_len;




if (cc->session.version >= BR_TLS11) {
iv_len = 0;
} else {
iv_len = bc_impl->block_size;
}
compute_key_block(cc, prf_id,
mac_key_len + cipher_key_len + iv_len, kb);
if (is_client) {
mac_key = &kb[0];
cipher_key = &kb[mac_key_len << 1];
iv = &kb[(mac_key_len + cipher_key_len) << 1];
} else {
mac_key = &kb[mac_key_len];
cipher_key = &kb[(mac_key_len << 1) + cipher_key_len];
iv = &kb[((mac_key_len + cipher_key_len) << 1) + iv_len];
}
if (iv_len == 0) {
iv = NULL;
}
cc->icbc_out->init(&cc->out.cbc.vtable,
bc_impl, cipher_key, cipher_key_len,
imh, mac_key, mac_key_len, mac_out_len, iv);
}


void
br_ssl_engine_switch_gcm_in(br_ssl_engine_context *cc,
int is_client, int prf_id,
const br_block_ctr_class *bc_impl, size_t cipher_key_len)
{
unsigned char kb[72];
unsigned char *cipher_key, *iv;

compute_key_block(cc, prf_id, cipher_key_len + 4, kb);
if (is_client) {
cipher_key = &kb[cipher_key_len];
iv = &kb[(cipher_key_len << 1) + 4];
} else {
cipher_key = &kb[0];
iv = &kb[cipher_key_len << 1];
}
cc->igcm_in->init(&cc->in.gcm.vtable.in,
bc_impl, cipher_key, cipher_key_len, cc->ighash, iv);
cc->incrypt = 1;
}


void
br_ssl_engine_switch_gcm_out(br_ssl_engine_context *cc,
int is_client, int prf_id,
const br_block_ctr_class *bc_impl, size_t cipher_key_len)
{
unsigned char kb[72];
unsigned char *cipher_key, *iv;

compute_key_block(cc, prf_id, cipher_key_len + 4, kb);
if (is_client) {
cipher_key = &kb[0];
iv = &kb[cipher_key_len << 1];
} else {
cipher_key = &kb[cipher_key_len];
iv = &kb[(cipher_key_len << 1) + 4];
}
cc->igcm_out->init(&cc->out.gcm.vtable.out,
bc_impl, cipher_key, cipher_key_len, cc->ighash, iv);
}


void
br_ssl_engine_switch_chapol_in(br_ssl_engine_context *cc,
int is_client, int prf_id)
{
unsigned char kb[88];
unsigned char *cipher_key, *iv;

compute_key_block(cc, prf_id, 44, kb);
if (is_client) {
cipher_key = &kb[32];
iv = &kb[76];
} else {
cipher_key = &kb[0];
iv = &kb[64];
}
cc->ichapol_in->init(&cc->in.chapol.vtable.in,
cc->ichacha, cc->ipoly, cipher_key, iv);
cc->incrypt = 1;
}


void
br_ssl_engine_switch_chapol_out(br_ssl_engine_context *cc,
int is_client, int prf_id)
{
unsigned char kb[88];
unsigned char *cipher_key, *iv;

compute_key_block(cc, prf_id, 44, kb);
if (is_client) {
cipher_key = &kb[0];
iv = &kb[64];
} else {
cipher_key = &kb[32];
iv = &kb[76];
}
cc->ichapol_out->init(&cc->out.chapol.vtable.out,
cc->ichacha, cc->ipoly, cipher_key, iv);
}


void
br_ssl_engine_switch_ccm_in(br_ssl_engine_context *cc,
int is_client, int prf_id,
const br_block_ctrcbc_class *bc_impl,
size_t cipher_key_len, size_t tag_len)
{
unsigned char kb[72];
unsigned char *cipher_key, *iv;

compute_key_block(cc, prf_id, cipher_key_len + 4, kb);
if (is_client) {
cipher_key = &kb[cipher_key_len];
iv = &kb[(cipher_key_len << 1) + 4];
} else {
cipher_key = &kb[0];
iv = &kb[cipher_key_len << 1];
}
cc->iccm_in->init(&cc->in.ccm.vtable.in,
bc_impl, cipher_key, cipher_key_len, iv, tag_len);
cc->incrypt = 1;
}


void
br_ssl_engine_switch_ccm_out(br_ssl_engine_context *cc,
int is_client, int prf_id,
const br_block_ctrcbc_class *bc_impl,
size_t cipher_key_len, size_t tag_len)
{
unsigned char kb[72];
unsigned char *cipher_key, *iv;

compute_key_block(cc, prf_id, cipher_key_len + 4, kb);
if (is_client) {
cipher_key = &kb[0];
iv = &kb[cipher_key_len << 1];
} else {
cipher_key = &kb[cipher_key_len];
iv = &kb[(cipher_key_len << 1) + 4];
}
cc->iccm_out->init(&cc->out.ccm.vtable.out,
bc_impl, cipher_key, cipher_key_len, iv, tag_len);
}
