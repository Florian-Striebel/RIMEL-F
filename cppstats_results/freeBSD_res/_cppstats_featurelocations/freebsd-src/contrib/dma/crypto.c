


































#include <openssl/x509.h>
#include <openssl/md5.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/rand.h>

#include <strings.h>
#include <string.h>
#include <syslog.h>

#include "dma.h"

static int
init_cert_file(SSL_CTX *ctx, const char *path)
{
int error;


error = SSL_CTX_use_certificate_chain_file(ctx, path);
if (error < 1) {
syslog(LOG_ERR, "SSL: Cannot load certificate `%s': %s", path, ssl_errstr());
return (-1);
}


error = SSL_CTX_use_PrivateKey_file(ctx, path, SSL_FILETYPE_PEM);
if (error < 1) {
syslog(LOG_ERR, "SSL: Cannot load private key `%s': %s", path, ssl_errstr());
return (-1);
}





error = SSL_CTX_check_private_key(ctx);
if (error < 1) {
syslog(LOG_ERR, "SSL: Cannot check private key: %s", ssl_errstr());
return (-1);
}

return (0);
}

static int
verify_server_fingerprint(const X509 *cert)
{
unsigned char fingerprint[EVP_MAX_MD_SIZE] = {0};
unsigned int fingerprint_len = 0;
if(!X509_digest(cert, EVP_sha256(), fingerprint, &fingerprint_len)) {
syslog(LOG_WARNING, "failed to load fingerprint of server certicate: %s",
ssl_errstr());
return (1);
}
if(fingerprint_len != SHA256_DIGEST_LENGTH) {
syslog(LOG_WARNING, "sha256 fingerprint has unexpected length of %d bytes",
fingerprint_len);
return (1);
}
if(memcmp(fingerprint, config.fingerprint, SHA256_DIGEST_LENGTH) != 0) {
syslog(LOG_WARNING, "fingerprints do not match");
return (1);
}
syslog(LOG_DEBUG, "successfully verified server certificate fingerprint");
return (0);
}

int
smtp_init_crypto(int fd, int feature, struct smtp_features* features)
{
SSL_CTX *ctx = NULL;
#if (OPENSSL_VERSION_NUMBER >= 0x00909000L)
const SSL_METHOD *meth = NULL;
#else
SSL_METHOD *meth = NULL;
#endif
X509 *cert;
int error;



SSL_library_init();
SSL_load_error_strings();


#if (OPENSSL_VERSION_NUMBER >= 0x10100000L)
meth = TLS_client_method();
#else
meth = SSLv23_client_method();
#endif

ctx = SSL_CTX_new(meth);
if (ctx == NULL) {
syslog(LOG_WARNING, "remote delivery deferred: SSL init failed: %s", ssl_errstr());
return (1);
}


if (config.certfile != NULL) {
error = init_cert_file(ctx, config.certfile);
if (error) {
syslog(LOG_WARNING, "remote delivery deferred");
return (1);
}
}




if (((feature & SECURETRANSFER) != 0) &&
(feature & STARTTLS) != 0) {

config.features |= NOSSL;

if (perform_server_greeting(fd, features) == 0) {
send_remote_command(fd, "STARTTLS");
if (read_remote(fd, 0, NULL) != 2) {
if ((feature & TLS_OPP) == 0) {
syslog(LOG_ERR, "remote delivery deferred: STARTTLS not available: %s", neterr);
return (1);
} else {
syslog(LOG_INFO, "in opportunistic TLS mode, STARTTLS not available: %s", neterr);
return (0);
}
}
} else {
syslog(LOG_ERR, "remote delivery deferred: could not perform server greeting: %s",
neterr);
return (1);
}


config.features &= ~NOSSL;
}

config.ssl = SSL_new(ctx);
if (config.ssl == NULL) {
syslog(LOG_NOTICE, "remote delivery deferred: SSL struct creation failed: %s",
ssl_errstr());
return (1);
}


SSL_set_connect_state(config.ssl);


error = SSL_set_fd(config.ssl, fd);
if (error == 0) {
syslog(LOG_NOTICE, "remote delivery deferred: SSL set fd failed: %s",
ssl_errstr());
return (1);
}


error = SSL_connect(config.ssl);
if (error != 1) {
syslog(LOG_ERR, "remote delivery deferred: SSL handshake failed fatally: %s",
ssl_errstr());
return (1);
}


cert = SSL_get_peer_certificate(config.ssl);
if (cert == NULL) {
syslog(LOG_WARNING, "remote delivery deferred: Peer did not provide certificate: %s",
ssl_errstr());
return (1);
}
if(config.fingerprint != NULL && verify_server_fingerprint(cert)) {
X509_free(cert);
return (1);
}
X509_free(cert);

return (0);
}











void
hmac_md5(unsigned char *text, int text_len, unsigned char *key, int key_len,
unsigned char* digest)
{
MD5_CTX context;
unsigned char k_ipad[65];


unsigned char k_opad[65];


unsigned char tk[16];
int i;

if (key_len > 64) {

MD5_CTX tctx;

MD5_Init(&tctx);
MD5_Update(&tctx, key, key_len);
MD5_Final(tk, &tctx);

key = tk;
key_len = 16;
}














bzero( k_ipad, sizeof k_ipad);
bzero( k_opad, sizeof k_opad);
bcopy( key, k_ipad, key_len);
bcopy( key, k_opad, key_len);


for (i=0; i<64; i++) {
k_ipad[i] ^= 0x36;
k_opad[i] ^= 0x5c;
}



MD5_Init(&context);

MD5_Update(&context, k_ipad, 64);
MD5_Update(&context, text, text_len);
MD5_Final(digest, &context);



MD5_Init(&context);

MD5_Update(&context, k_opad, 64);
MD5_Update(&context, digest, 16);

MD5_Final(digest, &context);
}




int
smtp_auth_md5(int fd, char *login, char *password)
{
unsigned char digest[BUF_SIZE];
char buffer[BUF_SIZE], ascii_digest[33];
char *temp;
int len, i;
static char hextab[] = "0123456789abcdef";

temp = calloc(BUF_SIZE, 1);
memset(buffer, 0, sizeof(buffer));
memset(digest, 0, sizeof(digest));
memset(ascii_digest, 0, sizeof(ascii_digest));


send_remote_command(fd, "AUTH CRAM-MD5");
if (read_remote(fd, sizeof(buffer), buffer) != 3) {
syslog(LOG_DEBUG, "smarthost authentication:"
" AUTH cram-md5 not available: %s", neterr);

free(temp);
return (-1);
}


base64_decode(buffer + 4, temp);
hmac_md5((unsigned char *)temp, strlen(temp),
(unsigned char *)password, strlen(password), digest);
free(temp);

ascii_digest[32] = 0;
for (i = 0; i < 16; i++) {
ascii_digest[2*i] = hextab[digest[i] >> 4];
ascii_digest[2*i+1] = hextab[digest[i] & 15];
}


snprintf(buffer, BUF_SIZE, "%s %s", login, ascii_digest);


len = base64_encode(buffer, strlen(buffer), &temp);
if (len < 0) {
syslog(LOG_ERR, "can not encode auth reply: %m");
return (-1);
}


send_remote_command(fd, "%s", temp);
free(temp);
if (read_remote(fd, 0, NULL) != 2) {
syslog(LOG_WARNING, "remote delivery deferred:"
" AUTH cram-md5 failed: %s", neterr);
return (-2);
}

return (0);
}
