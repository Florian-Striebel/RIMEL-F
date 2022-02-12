








#include "../util-internal.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>

#if defined(_WIN32)
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#if !defined(S_ISDIR)
#define S_ISDIR(x) (((x) & S_IFMT) == S_IFDIR)
#endif
#else
#include <sys/stat.h>
#include <sys/socket.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#endif

#include <event2/event.h>
#include <event2/http.h>
#include <event2/buffer.h>
#include <event2/util.h>
#include <event2/keyvalq_struct.h>

#if defined(EVENT__HAVE_NETINET_IN_H)
#include <netinet/in.h>
#if defined(_XOPEN_SOURCE_EXTENDED)
#include <arpa/inet.h>
#endif
#endif

#if defined(_WIN32)
#if !defined(stat)
#define stat _stat
#endif
#if !defined(fstat)
#define fstat _fstat
#endif
#if !defined(open)
#define open _open
#endif
#if !defined(close)
#define close _close
#endif
#if !defined(O_RDONLY)
#define O_RDONLY _O_RDONLY
#endif
#endif

char uri_root[512];

static const struct table_entry {
const char *extension;
const char *content_type;
} content_type_table[] = {
{ "txt", "text/plain" },
{ "c", "text/plain" },
{ "h", "text/plain" },
{ "html", "text/html" },
{ "htm", "text/htm" },
{ "css", "text/css" },
{ "gif", "image/gif" },
{ "jpg", "image/jpeg" },
{ "jpeg", "image/jpeg" },
{ "png", "image/png" },
{ "pdf", "application/pdf" },
{ "ps", "application/postscript" },
{ NULL, NULL },
};


static const char *
guess_content_type(const char *path)
{
const char *last_period, *extension;
const struct table_entry *ent;
last_period = strrchr(path, '.');
if (!last_period || strchr(last_period, '/'))
goto not_found;
extension = last_period + 1;
for (ent = &content_type_table[0]; ent->extension; ++ent) {
if (!evutil_ascii_strcasecmp(ent->extension, extension))
return ent->content_type;
}

not_found:
return "application/misc";
}



static void
dump_request_cb(struct evhttp_request *req, void *arg)
{
const char *cmdtype;
struct evkeyvalq *headers;
struct evkeyval *header;
struct evbuffer *buf;

switch (evhttp_request_get_command(req)) {
case EVHTTP_REQ_GET: cmdtype = "GET"; break;
case EVHTTP_REQ_POST: cmdtype = "POST"; break;
case EVHTTP_REQ_HEAD: cmdtype = "HEAD"; break;
case EVHTTP_REQ_PUT: cmdtype = "PUT"; break;
case EVHTTP_REQ_DELETE: cmdtype = "DELETE"; break;
case EVHTTP_REQ_OPTIONS: cmdtype = "OPTIONS"; break;
case EVHTTP_REQ_TRACE: cmdtype = "TRACE"; break;
case EVHTTP_REQ_CONNECT: cmdtype = "CONNECT"; break;
case EVHTTP_REQ_PATCH: cmdtype = "PATCH"; break;
default: cmdtype = "unknown"; break;
}

printf("Received a %s request for %s\nHeaders:\n",
cmdtype, evhttp_request_get_uri(req));

headers = evhttp_request_get_input_headers(req);
for (header = headers->tqh_first; header;
header = header->next.tqe_next) {
printf(" %s: %s\n", header->key, header->value);
}

buf = evhttp_request_get_input_buffer(req);
puts("Input data: <<<");
while (evbuffer_get_length(buf)) {
int n;
char cbuf[128];
n = evbuffer_remove(buf, cbuf, sizeof(cbuf));
if (n > 0)
(void) fwrite(cbuf, 1, n, stdout);
}
puts(">>>");

evhttp_send_reply(req, 200, "OK", NULL);
}





static void
send_document_cb(struct evhttp_request *req, void *arg)
{
struct evbuffer *evb = NULL;
const char *docroot = arg;
const char *uri = evhttp_request_get_uri(req);
struct evhttp_uri *decoded = NULL;
const char *path;
char *decoded_path;
char *whole_path = NULL;
size_t len;
int fd = -1;
struct stat st;

if (evhttp_request_get_command(req) != EVHTTP_REQ_GET) {
dump_request_cb(req, arg);
return;
}

printf("Got a GET request for <%s>\n", uri);


decoded = evhttp_uri_parse(uri);
if (!decoded) {
printf("It's not a good URI. Sending BADREQUEST\n");
evhttp_send_error(req, HTTP_BADREQUEST, 0);
return;
}


path = evhttp_uri_get_path(decoded);
if (!path) path = "/";


decoded_path = evhttp_uridecode(path, 0, NULL);
if (decoded_path == NULL)
goto err;




if (strstr(decoded_path, ".."))
goto err;

len = strlen(decoded_path)+strlen(docroot)+2;
if (!(whole_path = malloc(len))) {
perror("malloc");
goto err;
}
evutil_snprintf(whole_path, len, "%s/%s", docroot, decoded_path);

if (stat(whole_path, &st)<0) {
goto err;
}


evb = evbuffer_new();

if (S_ISDIR(st.st_mode)) {


#if defined(_WIN32)
HANDLE d;
WIN32_FIND_DATAA ent;
char *pattern;
size_t dirlen;
#else
DIR *d;
struct dirent *ent;
#endif
const char *trailing_slash = "";

if (!strlen(path) || path[strlen(path)-1] != '/')
trailing_slash = "/";

#if defined(_WIN32)
dirlen = strlen(whole_path);
pattern = malloc(dirlen+3);
memcpy(pattern, whole_path, dirlen);
pattern[dirlen] = '\\';
pattern[dirlen+1] = '*';
pattern[dirlen+2] = '\0';
d = FindFirstFileA(pattern, &ent);
free(pattern);
if (d == INVALID_HANDLE_VALUE)
goto err;
#else
if (!(d = opendir(whole_path)))
goto err;
#endif

evbuffer_add_printf(evb,
"<!DOCTYPE html>\n"
"<html>\n <head>\n"
" <meta charset='utf-8'>\n"
" <title>%s</title>\n"
" <base href='%s%s'>\n"
" </head>\n"
" <body>\n"
" <h1>%s</h1>\n"
" <ul>\n",
decoded_path,
path,
trailing_slash,
decoded_path );
#if defined(_WIN32)
do {
const char *name = ent.cFileName;
#else
while ((ent = readdir(d))) {
const char *name = ent->d_name;
#endif
evbuffer_add_printf(evb,
" <li><a href=\"%s\">%s</a>\n",
name, name);
#if defined(_WIN32)
} while (FindNextFileA(d, &ent));
#else
}
#endif
evbuffer_add_printf(evb, "</ul></body></html>\n");
#if defined(_WIN32)
FindClose(d);
#else
closedir(d);
#endif
evhttp_add_header(evhttp_request_get_output_headers(req),
"Content-Type", "text/html");
} else {


const char *type = guess_content_type(decoded_path);
if ((fd = open(whole_path, O_RDONLY)) < 0) {
perror("open");
goto err;
}

if (fstat(fd, &st)<0) {


perror("fstat");
goto err;
}
evhttp_add_header(evhttp_request_get_output_headers(req),
"Content-Type", type);
evbuffer_add_file(evb, fd, 0, st.st_size);
}

evhttp_send_reply(req, 200, "OK", evb);
goto done;
err:
evhttp_send_error(req, 404, "Document was not found");
if (fd>=0)
close(fd);
done:
if (decoded)
evhttp_uri_free(decoded);
if (decoded_path)
free(decoded_path);
if (whole_path)
free(whole_path);
if (evb)
evbuffer_free(evb);
}

static void
syntax(void)
{
fprintf(stdout, "Syntax: http-server <docroot>\n");
}

int
main(int argc, char **argv)
{
struct event_base *base;
struct evhttp *http;
struct evhttp_bound_socket *handle;

ev_uint16_t port = 0;
#if defined(_WIN32)
WSADATA WSAData;
WSAStartup(0x101, &WSAData);
#else
if (signal(SIGPIPE, SIG_IGN) == SIG_ERR)
return (1);
#endif
if (argc < 2) {
syntax();
return 1;
}

base = event_base_new();
if (!base) {
fprintf(stderr, "Couldn't create an event_base: exiting\n");
return 1;
}


http = evhttp_new(base);
if (!http) {
fprintf(stderr, "couldn't create evhttp. Exiting.\n");
return 1;
}


evhttp_set_cb(http, "/dump", dump_request_cb, NULL);



evhttp_set_gencb(http, send_document_cb, argv[1]);


handle = evhttp_bind_socket_with_handle(http, "0.0.0.0", port);
if (!handle) {
fprintf(stderr, "couldn't bind to port %d. Exiting.\n",
(int)port);
return 1;
}

{

struct sockaddr_storage ss;
evutil_socket_t fd;
ev_socklen_t socklen = sizeof(ss);
char addrbuf[128];
void *inaddr;
const char *addr;
int got_port = -1;
fd = evhttp_bound_socket_get_fd(handle);
memset(&ss, 0, sizeof(ss));
if (getsockname(fd, (struct sockaddr *)&ss, &socklen)) {
perror("getsockname() failed");
return 1;
}
if (ss.ss_family == AF_INET) {
got_port = ntohs(((struct sockaddr_in*)&ss)->sin_port);
inaddr = &((struct sockaddr_in*)&ss)->sin_addr;
} else if (ss.ss_family == AF_INET6) {
got_port = ntohs(((struct sockaddr_in6*)&ss)->sin6_port);
inaddr = &((struct sockaddr_in6*)&ss)->sin6_addr;
} else {
fprintf(stderr, "Weird address family %d\n",
ss.ss_family);
return 1;
}
addr = evutil_inet_ntop(ss.ss_family, inaddr, addrbuf,
sizeof(addrbuf));
if (addr) {
printf("Listening on %s:%d\n", addr, got_port);
evutil_snprintf(uri_root, sizeof(uri_root),
"http://%s:%d",addr,got_port);
} else {
fprintf(stderr, "evutil_inet_ntop failed\n");
return 1;
}
}

event_base_dispatch(base);

return 0;
}
