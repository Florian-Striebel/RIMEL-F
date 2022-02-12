























#include "inner.h"


void
br_sslio_init(br_sslio_context *ctx,
br_ssl_engine_context *engine,
int (*low_read)(void *read_context,
unsigned char *data, size_t len),
void *read_context,
int (*low_write)(void *write_context,
const unsigned char *data, size_t len),
void *write_context)
{
ctx->engine = engine;
ctx->low_read = low_read;
ctx->read_context = read_context;
ctx->low_write = low_write;
ctx->write_context = write_context;
}







static int
run_until(br_sslio_context *ctx, unsigned target)
{
for (;;) {
unsigned state;

state = br_ssl_engine_current_state(ctx->engine);
if (state & BR_SSL_CLOSED) {
return -1;
}





if (state & BR_SSL_SENDREC) {
unsigned char *buf;
size_t len;
int wlen;

buf = br_ssl_engine_sendrec_buf(ctx->engine, &len);
wlen = ctx->low_write(ctx->write_context, buf, len);
if (wlen < 0) {







if (!ctx->engine->shutdown_recv) {
br_ssl_engine_fail(
ctx->engine, BR_ERR_IO);
}
return -1;
}
if (wlen > 0) {
br_ssl_engine_sendrec_ack(ctx->engine, wlen);
}
continue;
}




if (state & target) {
return 0;
}









if (state & BR_SSL_RECVAPP) {
return -1;
}






if (state & BR_SSL_RECVREC) {
unsigned char *buf;
size_t len;
int rlen;

buf = br_ssl_engine_recvrec_buf(ctx->engine, &len);
rlen = ctx->low_read(ctx->read_context, buf, len);
if (rlen < 0) {
br_ssl_engine_fail(ctx->engine, BR_ERR_IO);
return -1;
}
if (rlen > 0) {
br_ssl_engine_recvrec_ack(ctx->engine, rlen);
}
continue;
}








br_ssl_engine_flush(ctx->engine, 0);
}
}


int
br_sslio_read(br_sslio_context *ctx, void *dst, size_t len)
{
unsigned char *buf;
size_t alen;

if (len == 0) {
return 0;
}
if (run_until(ctx, BR_SSL_RECVAPP) < 0) {
return -1;
}
buf = br_ssl_engine_recvapp_buf(ctx->engine, &alen);
if (alen > len) {
alen = len;
}
memcpy(dst, buf, alen);
br_ssl_engine_recvapp_ack(ctx->engine, alen);
return (int)alen;
}


int
br_sslio_read_all(br_sslio_context *ctx, void *dst, size_t len)
{
unsigned char *buf;

buf = dst;
while (len > 0) {
int rlen;

rlen = br_sslio_read(ctx, buf, len);
if (rlen < 0) {
return -1;
}
buf += rlen;
len -= (size_t)rlen;
}
return 0;
}


int
br_sslio_write(br_sslio_context *ctx, const void *src, size_t len)
{
unsigned char *buf;
size_t alen;

if (len == 0) {
return 0;
}
if (run_until(ctx, BR_SSL_SENDAPP) < 0) {
return -1;
}
buf = br_ssl_engine_sendapp_buf(ctx->engine, &alen);
if (alen > len) {
alen = len;
}
memcpy(buf, src, alen);
br_ssl_engine_sendapp_ack(ctx->engine, alen);
return (int)alen;
}


int
br_sslio_write_all(br_sslio_context *ctx, const void *src, size_t len)
{
const unsigned char *buf;

buf = src;
while (len > 0) {
int wlen;

wlen = br_sslio_write(ctx, buf, len);
if (wlen < 0) {
return -1;
}
buf += wlen;
len -= (size_t)wlen;
}
return 0;
}


int
br_sslio_flush(br_sslio_context *ctx)
{







br_ssl_engine_flush(ctx->engine, 0);
return run_until(ctx, BR_SSL_SENDAPP | BR_SSL_RECVAPP);
}


int
br_sslio_close(br_sslio_context *ctx)
{
br_ssl_engine_close(ctx->engine);
while (br_ssl_engine_current_state(ctx->engine) != BR_SSL_CLOSED) {



size_t len;

run_until(ctx, BR_SSL_RECVAPP);
if (br_ssl_engine_recvapp_buf(ctx->engine, &len) != NULL) {
br_ssl_engine_recvapp_ack(ctx->engine, len);
}
}
return br_ssl_engine_last_error(ctx->engine) == BR_ERR_OK;
}
