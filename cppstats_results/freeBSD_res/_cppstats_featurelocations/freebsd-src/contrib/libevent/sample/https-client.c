












#if defined(__APPLE__) && defined(__clang__)
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#endif

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#if defined(_WIN32)
#include <winsock2.h>
#include <ws2tcpip.h>

#define snprintf _snprintf
#define strcasecmp _stricmp
#else
#include <sys/socket.h>
#include <netinet/in.h>
#endif

#include <event2/bufferevent_ssl.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/listener.h>
#include <event2/util.h>
#include <event2/http.h>

#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/rand.h>

#include "openssl_hostname_validation.h"

static struct event_base *base;
static int ignore_cert = 0;

static void
http_request_done(struct evhttp_request *req, void *ctx)
{
char buffer[256];
int nread;

if (req == NULL) {



struct bufferevent *bev = (struct bufferevent *) ctx;
unsigned long oslerr;
int printed_err = 0;
int errcode = EVUTIL_SOCKET_ERROR();
fprintf(stderr, "some request failed - no idea which one though!\n");


while ((oslerr = bufferevent_get_openssl_error(bev))) {
ERR_error_string_n(oslerr, buffer, sizeof(buffer));
fprintf(stderr, "%s\n", buffer);
printed_err = 1;
}


if (! printed_err)
fprintf(stderr, "socket error = %s (%d)\n",
evutil_socket_error_to_string(errcode),
errcode);
return;
}

fprintf(stderr, "Response line: %d %s\n",
evhttp_request_get_response_code(req),
evhttp_request_get_response_code_line(req));

while ((nread = evbuffer_remove(evhttp_request_get_input_buffer(req),
buffer, sizeof(buffer)))
> 0) {


fwrite(buffer, nread, 1, stdout);
}
}

static void
syntax(void)
{
fputs("Syntax:\n", stderr);
fputs(" https-client -url <https-url> [-data data-file.bin] [-ignore-cert] [-retries num] [-timeout sec] [-crt crt]\n", stderr);
fputs("Example:\n", stderr);
fputs(" https-client -url https://ip.appspot.com/\n", stderr);
}

static void
err(const char *msg)
{
fputs(msg, stderr);
}

static void
err_openssl(const char *func)
{
fprintf (stderr, "%s failed:\n", func);



ERR_print_errors_fp (stderr);

exit(1);
}

#if !defined(_WIN32)

static int cert_verify_callback(X509_STORE_CTX *x509_ctx, void *arg)
{
char cert_str[256];
const char *host = (const char *) arg;
const char *res_str = "X509_verify_cert failed";
HostnameValidationResult res = Error;




int ok_so_far = 0;

X509 *server_cert = NULL;

if (ignore_cert) {
return 1;
}

ok_so_far = X509_verify_cert(x509_ctx);

server_cert = X509_STORE_CTX_get_current_cert(x509_ctx);

if (ok_so_far) {
res = validate_hostname(host, server_cert);

switch (res) {
case MatchFound:
res_str = "MatchFound";
break;
case MatchNotFound:
res_str = "MatchNotFound";
break;
case NoSANPresent:
res_str = "NoSANPresent";
break;
case MalformedCertificate:
res_str = "MalformedCertificate";
break;
case Error:
res_str = "Error";
break;
default:
res_str = "WTF!";
break;
}
}

X509_NAME_oneline(X509_get_subject_name (server_cert),
cert_str, sizeof (cert_str));

if (res == MatchFound) {
printf("https server '%s' has this certificate, "
"which looks good to me:\n%s\n",
host, cert_str);
return 1;
} else {
printf("Got '%s' for hostname '%s' and certificate:\n%s\n",
res_str, host, cert_str);
return 0;
}
}
#endif

int
main(int argc, char **argv)
{
int r;

struct evhttp_uri *http_uri = NULL;
const char *url = NULL, *data_file = NULL;
const char *crt = "/etc/ssl/certs/ca-certificates.crt";
const char *scheme, *host, *path, *query;
char uri[256];
int port;
int retries = 0;
int timeout = -1;

SSL_CTX *ssl_ctx = NULL;
SSL *ssl = NULL;
struct bufferevent *bev;
struct evhttp_connection *evcon = NULL;
struct evhttp_request *req;
struct evkeyvalq *output_headers;
struct evbuffer *output_buffer;

int i;
int ret = 0;
enum { HTTP, HTTPS } type = HTTP;

for (i = 1; i < argc; i++) {
if (!strcmp("-url", argv[i])) {
if (i < argc - 1) {
url = argv[i + 1];
} else {
syntax();
goto error;
}
} else if (!strcmp("-crt", argv[i])) {
if (i < argc - 1) {
crt = argv[i + 1];
} else {
syntax();
goto error;
}
} else if (!strcmp("-ignore-cert", argv[i])) {
ignore_cert = 1;
} else if (!strcmp("-data", argv[i])) {
if (i < argc - 1) {
data_file = argv[i + 1];
} else {
syntax();
goto error;
}
} else if (!strcmp("-retries", argv[i])) {
if (i < argc - 1) {
retries = atoi(argv[i + 1]);
} else {
syntax();
goto error;
}
} else if (!strcmp("-timeout", argv[i])) {
if (i < argc - 1) {
timeout = atoi(argv[i + 1]);
} else {
syntax();
goto error;
}
} else if (!strcmp("-help", argv[i])) {
syntax();
goto error;
}
}

if (!url) {
syntax();
goto error;
}

#if defined(_WIN32)
{
WORD wVersionRequested;
WSADATA wsaData;
int err;

wVersionRequested = MAKEWORD(2, 2);

err = WSAStartup(wVersionRequested, &wsaData);
if (err != 0) {
printf("WSAStartup failed with error: %d\n", err);
goto error;
}
}
#endif

http_uri = evhttp_uri_parse(url);
if (http_uri == NULL) {
err("malformed url");
goto error;
}

scheme = evhttp_uri_get_scheme(http_uri);
if (scheme == NULL || (strcasecmp(scheme, "https") != 0 &&
strcasecmp(scheme, "http") != 0)) {
err("url must be http or https");
goto error;
}

host = evhttp_uri_get_host(http_uri);
if (host == NULL) {
err("url must have a host");
goto error;
}

port = evhttp_uri_get_port(http_uri);
if (port == -1) {
port = (strcasecmp(scheme, "http") == 0) ? 80 : 443;
}

path = evhttp_uri_get_path(http_uri);
if (strlen(path) == 0) {
path = "/";
}

query = evhttp_uri_get_query(http_uri);
if (query == NULL) {
snprintf(uri, sizeof(uri) - 1, "%s", path);
} else {
snprintf(uri, sizeof(uri) - 1, "%s?%s", path, query);
}
uri[sizeof(uri) - 1] = '\0';

#if OPENSSL_VERSION_NUMBER < 0x10100000L

SSL_library_init();
ERR_load_crypto_strings();
SSL_load_error_strings();
OpenSSL_add_all_algorithms();
#endif



r = RAND_poll();
if (r == 0) {
err_openssl("RAND_poll");
goto error;
}


ssl_ctx = SSL_CTX_new(SSLv23_method());
if (!ssl_ctx) {
err_openssl("SSL_CTX_new");
goto error;
}

#if !defined(_WIN32)




if (1 != SSL_CTX_load_verify_locations(ssl_ctx, crt, NULL)) {
err_openssl("SSL_CTX_load_verify_locations");
goto error;
}








SSL_CTX_set_verify(ssl_ctx, SSL_VERIFY_PEER, NULL);













SSL_CTX_set_cert_verify_callback(ssl_ctx, cert_verify_callback,
(void *) host);
#else
(void)crt;
#endif


base = event_base_new();
if (!base) {
perror("event_base_new()");
goto error;
}


ssl = SSL_new(ssl_ctx);
if (ssl == NULL) {
err_openssl("SSL_new()");
goto error;
}

#if defined(SSL_CTRL_SET_TLSEXT_HOSTNAME)

SSL_set_tlsext_host_name(ssl, host);
#endif

if (strcasecmp(scheme, "http") == 0) {
bev = bufferevent_socket_new(base, -1, BEV_OPT_CLOSE_ON_FREE);
} else {
type = HTTPS;
bev = bufferevent_openssl_socket_new(base, -1, ssl,
BUFFEREVENT_SSL_CONNECTING,
BEV_OPT_CLOSE_ON_FREE|BEV_OPT_DEFER_CALLBACKS);
}

if (bev == NULL) {
fprintf(stderr, "bufferevent_openssl_socket_new() failed\n");
goto error;
}

bufferevent_openssl_set_allow_dirty_shutdown(bev, 1);



evcon = evhttp_connection_base_bufferevent_new(base, NULL, bev,
host, port);
if (evcon == NULL) {
fprintf(stderr, "evhttp_connection_base_bufferevent_new() failed\n");
goto error;
}

if (retries > 0) {
evhttp_connection_set_retries(evcon, retries);
}
if (timeout >= 0) {
evhttp_connection_set_timeout(evcon, timeout);
}


req = evhttp_request_new(http_request_done, bev);
if (req == NULL) {
fprintf(stderr, "evhttp_request_new() failed\n");
goto error;
}

output_headers = evhttp_request_get_output_headers(req);
evhttp_add_header(output_headers, "Host", host);
evhttp_add_header(output_headers, "Connection", "close");

if (data_file) {



FILE * f = fopen(data_file, "rb");
char buf[1024];
size_t s;
size_t bytes = 0;

if (!f) {
syntax();
goto error;
}

output_buffer = evhttp_request_get_output_buffer(req);
while ((s = fread(buf, 1, sizeof(buf), f)) > 0) {
evbuffer_add(output_buffer, buf, s);
bytes += s;
}
evutil_snprintf(buf, sizeof(buf)-1, "%lu", (unsigned long)bytes);
evhttp_add_header(output_headers, "Content-Length", buf);
fclose(f);
}

r = evhttp_make_request(evcon, req, data_file ? EVHTTP_REQ_POST : EVHTTP_REQ_GET, uri);
if (r != 0) {
fprintf(stderr, "evhttp_make_request() failed\n");
goto error;
}

event_base_dispatch(base);
goto cleanup;

error:
ret = 1;
cleanup:
if (evcon)
evhttp_connection_free(evcon);
if (http_uri)
evhttp_uri_free(http_uri);
event_base_free(base);

if (ssl_ctx)
SSL_CTX_free(ssl_ctx);
if (type == HTTP && ssl)
SSL_free(ssl);
#if OPENSSL_VERSION_NUMBER < 0x10100000L
EVP_cleanup();
ERR_free_strings();

#if defined(EVENT__HAVE_ERR_REMOVE_THREAD_STATE)
ERR_remove_thread_state(NULL);
#else
ERR_remove_state(0);
#endif
CRYPTO_cleanup_all_ex_data();

sk_SSL_COMP_free(SSL_COMP_get_compression_methods());
#endif

#if defined(_WIN32)
WSACleanup();
#endif

return ret;
}
