





























#include "fmacros.h"
#include "cli_common.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <hiredis.h>
#include <sdscompat.h>
#include <sds.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#if defined(USE_OPENSSL)
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <hiredis_ssl.h>
#endif

#define UNUSED(V) ((void) V)




int cliSecureConnection(redisContext *c, cliSSLconfig config, const char **err) {
#if defined(USE_OPENSSL)
static SSL_CTX *ssl_ctx = NULL;

if (!ssl_ctx) {
ssl_ctx = SSL_CTX_new(SSLv23_client_method());
if (!ssl_ctx) {
*err = "Failed to create SSL_CTX";
goto error;
}
SSL_CTX_set_options(ssl_ctx, SSL_OP_NO_SSLv2 | SSL_OP_NO_SSLv3);
SSL_CTX_set_verify(ssl_ctx, config.skip_cert_verify ? SSL_VERIFY_NONE : SSL_VERIFY_PEER, NULL);

if (config.cacert || config.cacertdir) {
if (!SSL_CTX_load_verify_locations(ssl_ctx, config.cacert, config.cacertdir)) {
*err = "Invalid CA Certificate File/Directory";
goto error;
}
} else {
if (!SSL_CTX_set_default_verify_paths(ssl_ctx)) {
*err = "Failed to use default CA paths";
goto error;
}
}

if (config.cert && !SSL_CTX_use_certificate_chain_file(ssl_ctx, config.cert)) {
*err = "Invalid client certificate";
goto error;
}

if (config.key && !SSL_CTX_use_PrivateKey_file(ssl_ctx, config.key, SSL_FILETYPE_PEM)) {
*err = "Invalid private key";
goto error;
}
if (config.ciphers && !SSL_CTX_set_cipher_list(ssl_ctx, config.ciphers)) {
*err = "Error while configuring ciphers";
goto error;
}
#if defined(TLS1_3_VERSION)
if (config.ciphersuites && !SSL_CTX_set_ciphersuites(ssl_ctx, config.ciphersuites)) {
*err = "Error while setting cypher suites";
goto error;
}
#endif
}

SSL *ssl = SSL_new(ssl_ctx);
if (!ssl) {
*err = "Failed to create SSL object";
return REDIS_ERR;
}

if (config.sni && !SSL_set_tlsext_host_name(ssl, config.sni)) {
*err = "Failed to configure SNI";
SSL_free(ssl);
return REDIS_ERR;
}

return redisInitiateSSL(c, ssl);

error:
SSL_CTX_free(ssl_ctx);
ssl_ctx = NULL;
return REDIS_ERR;
#else
(void) config;
(void) c;
(void) err;
return REDIS_OK;
#endif
}















ssize_t cliWriteConn(redisContext *c, const char *buf, size_t buf_len)
{
int done = 0;




c->obuf = sdscatlen(c->obuf, buf, buf_len);
if (redisBufferWrite(c, &done) == REDIS_ERR) {
if (!(c->flags & REDIS_BLOCK))
errno = EAGAIN;




if (sdslen(c->obuf) > buf_len)
sdsrange(c->obuf, 0, -(buf_len+1));
else
sdsclear(c->obuf);

return -1;
}





if (done) {
sdsclear(c->obuf);
return buf_len;
}








if (sdslen(c->obuf) > buf_len) {
sdsrange(c->obuf, 0, -(buf_len+1));
return 0;
}




size_t left = sdslen(c->obuf);
sdsclear(c->obuf);
return buf_len - left;
}



int cliSecureInit()
{
#if defined(USE_OPENSSL)
ERR_load_crypto_strings();
SSL_load_error_strings();
SSL_library_init();
#endif
return REDIS_OK;
}


sds readArgFromStdin(void) {
char buf[1024];
sds arg = sdsempty();

while(1) {
int nread = read(fileno(stdin),buf,1024);

if (nread == 0) break;
else if (nread == -1) {
perror("Reading from standard input");
exit(1);
}
arg = sdscatlen(arg,buf,nread);
}
return arg;
}








sds *getSdsArrayFromArgv(int argc,char **argv, int quoted) {
sds *res = sds_malloc(sizeof(sds) * argc);

for (int j = 0; j < argc; j++) {
if (quoted) {
sds unquoted = unquoteCString(argv[j]);
if (!unquoted) {
while (--j >= 0) sdsfree(res[j]);
sds_free(res);
return NULL;
}
res[j] = unquoted;
} else {
res[j] = sdsnew(argv[j]);
}
}

return res;
}


sds unquoteCString(char *str) {
int count;
sds *unquoted = sdssplitargs(str, &count);
sds res = NULL;

if (unquoted && count == 1) {
res = unquoted[0];
unquoted[0] = NULL;
}

if (unquoted)
sdsfreesplitres(unquoted, count);

return res;
}



#define isHexChar(c) (isdigit(c) || ((c) >= 'a' && (c) <= 'f'))
#define decodeHexChar(c) (isdigit(c) ? (c) - '0' : (c) - 'a' + 10)
#define decodeHex(h, l) ((decodeHexChar(h) << 4) + decodeHexChar(l))

static sds percentDecode(const char *pe, size_t len) {
const char *end = pe + len;
sds ret = sdsempty();
const char *curr = pe;

while (curr < end) {
if (*curr == '%') {
if ((end - curr) < 2) {
fprintf(stderr, "Incomplete URI encoding\n");
exit(1);
}

char h = tolower(*(++curr));
char l = tolower(*(++curr));
if (!isHexChar(h) || !isHexChar(l)) {
fprintf(stderr, "Illegal character in URI encoding\n");
exit(1);
}
char c = decodeHex(h, l);
ret = sdscatlen(ret, &c, 1);
curr++;
} else {
ret = sdscatlen(ret, curr++, 1);
}
}

return ret;
}









void parseRedisUri(const char *uri, const char* tool_name, cliConnInfo *connInfo, int *tls_flag) {
#if defined(USE_OPENSSL)
UNUSED(tool_name);
#else
UNUSED(tls_flag);
#endif

const char *scheme = "redis://";
const char *tlsscheme = "rediss://";
const char *curr = uri;
const char *end = uri + strlen(uri);
const char *userinfo, *username, *port, *host, *path;


if (!strncasecmp(tlsscheme, curr, strlen(tlsscheme))) {
#if defined(USE_OPENSSL)
*tls_flag = 1;
curr += strlen(tlsscheme);
#else
fprintf(stderr,"rediss:// is only supported when %s is compiled with OpenSSL\n", tool_name);
exit(1);
#endif
} else if (!strncasecmp(scheme, curr, strlen(scheme))) {
curr += strlen(scheme);
} else {
fprintf(stderr,"Invalid URI scheme\n");
exit(1);
}
if (curr == end) return;


if ((userinfo = strchr(curr,'@'))) {
if ((username = strchr(curr, ':')) && username < userinfo) {
connInfo->user = percentDecode(curr, username - curr);
curr = username + 1;
}

connInfo->auth = percentDecode(curr, userinfo - curr);
curr = userinfo + 1;
}
if (curr == end) return;


path = strchr(curr, '/');
if (*curr != '/') {
host = path ? path - 1 : end;
if ((port = strchr(curr, ':'))) {
connInfo->hostport = atoi(port + 1);
host = port - 1;
}
sdsfree(connInfo->hostip);
connInfo->hostip = sdsnewlen(curr, host - curr + 1);
}
curr = path ? path + 1 : end;
if (curr == end) return;


connInfo->input_dbnum = atoi(curr);
}

void freeCliConnInfo(cliConnInfo connInfo){
if (connInfo.hostip) sdsfree(connInfo.hostip);
if (connInfo.auth) sdsfree(connInfo.auth);
if (connInfo.user) sdsfree(connInfo.user);
}
