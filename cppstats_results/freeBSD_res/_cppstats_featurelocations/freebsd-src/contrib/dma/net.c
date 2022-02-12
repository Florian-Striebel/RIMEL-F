



































#include "dfcompat.h"

#include <sys/param.h>
#include <sys/queue.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <openssl/ssl.h>
#include <openssl/err.h>

#include <ctype.h>
#include <err.h>
#include <errno.h>
#include <netdb.h>
#include <setjmp.h>
#include <signal.h>
#include <strings.h>
#include <string.h>
#include <syslog.h>
#include <unistd.h>

#include "dma.h"

char neterr[ERRMSG_SIZE];

char *
ssl_errstr(void)
{
long oerr, nerr;

oerr = 0;
while ((nerr = ERR_get_error()) != 0)
oerr = nerr;

return (ERR_error_string(oerr, NULL));
}

ssize_t
send_remote_command(int fd, const char* fmt, ...)
{
va_list va;
char cmd[4096];
size_t len, pos;
int s;
ssize_t n;

va_start(va, fmt);
s = vsnprintf(cmd, sizeof(cmd) - 2, fmt, va);
va_end(va);
if (s == sizeof(cmd) - 2 || s < 0) {
strcpy(neterr, "Internal error: oversized command string");
return (-1);
}


strcat(cmd, "\r\n");
len = strlen(cmd);

if (((config.features & SECURETRANSFER) != 0) &&
((config.features & NOSSL) == 0)) {
while ((s = SSL_write(config.ssl, (const char*)cmd, len)) <= 0) {
s = SSL_get_error(config.ssl, s);
if (s != SSL_ERROR_WANT_READ &&
s != SSL_ERROR_WANT_WRITE) {
strlcpy(neterr, ssl_errstr(), sizeof(neterr));
return (-1);
}
}
}
else {
pos = 0;
while (pos < len) {
n = write(fd, cmd + pos, len - pos);
if (n < 0)
return (-1);
pos += n;
}
}

return (len);
}

int
read_remote(int fd, int extbufsize, char *extbuf)
{
ssize_t rlen = 0;
size_t pos, len, copysize;
char buff[BUF_SIZE];
int done = 0, status = 0, status_running = 0, extbufpos = 0;
enum { parse_status, parse_spacedash, parse_rest } parsestate;

if (do_timeout(CON_TIMEOUT, 1) != 0) {
snprintf(neterr, sizeof(neterr), "Timeout reached");
return (-1);
}





len = 0;
pos = 0;
parsestate = parse_status;
neterr[0] = 0;
while (!(done && parsestate == parse_status)) {
rlen = 0;
if (pos == 0 ||
(pos > 0 && memchr(buff + pos, '\n', len - pos) == NULL)) {
memmove(buff, buff + pos, len - pos);
len -= pos;
pos = 0;
if (((config.features & SECURETRANSFER) != 0) &&
(config.features & NOSSL) == 0) {
if ((rlen = SSL_read(config.ssl, buff + len, sizeof(buff) - len)) == -1) {
strlcpy(neterr, ssl_errstr(), sizeof(neterr));
goto error;
}
} else {
if ((rlen = read(fd, buff + len, sizeof(buff) - len)) == -1) {
strlcpy(neterr, strerror(errno), sizeof(neterr));
goto error;
}
}
len += rlen;

copysize = sizeof(neterr) - strlen(neterr) - 1;
if (copysize > len)
copysize = len;
strncat(neterr, buff, copysize);
}






if (extbufpos <= (extbufsize - 1) && rlen > 0 && extbufsize > 0 && extbuf != NULL) {

if(extbufpos + rlen > (extbufsize - 1)) {
rlen = extbufsize - extbufpos;
}
memcpy(extbuf + extbufpos, buff + len - rlen, rlen);
extbufpos += rlen;
}

if (pos == len)
continue;

switch (parsestate) {
case parse_status:
for (; pos < len; pos++) {
if (isdigit(buff[pos])) {
status_running = status_running * 10 + (buff[pos] - '0');
} else {
status = status_running;
status_running = 0;
parsestate = parse_spacedash;
break;
}
}
continue;

case parse_spacedash:
switch (buff[pos]) {
case ' ':
done = 1;
break;

case '-':


break;

default:
strcpy(neterr, "invalid syntax in reply from server");
goto error;
}

pos++;
parsestate = parse_rest;
continue;

case parse_rest:

for (; pos < len; pos++) {
if (buff[pos] == '\n') {
pos++;
parsestate = parse_status;
break;
}
}
}

}

do_timeout(0, 0);


while (neterr[0] != 0 && strchr("\r\n", neterr[strlen(neterr) - 1]) != 0)
neterr[strlen(neterr) - 1] = 0;

return (status/100);

error:
do_timeout(0, 0);
return (-1);
}




static int
smtp_login(int fd, char *login, char* password, const struct smtp_features* features)
{
char *temp;
int len, res = 0;


if (features->auth.cram_md5) {
res = smtp_auth_md5(fd, login, password);
if (res == 0) {
return (0);
} else if (res == -2) {




return (1);
}
}


if (features->auth.login) {
if ((config.features & INSECURE) != 0 ||
(config.features & SECURETRANSFER) != 0) {

send_remote_command(fd, "AUTH LOGIN");
if (read_remote(fd, 0, NULL) != 3) {
syslog(LOG_NOTICE, "remote delivery deferred:"
" AUTH login not available: %s",
neterr);
return (1);
}

len = base64_encode(login, strlen(login), &temp);
if (len < 0) {
encerr:
syslog(LOG_ERR, "can not encode auth reply: %m");
return (1);
}

send_remote_command(fd, "%s", temp);
free(temp);
res = read_remote(fd, 0, NULL);
if (res != 3) {
syslog(LOG_NOTICE, "remote delivery %s: AUTH login failed: %s",
res == 5 ? "failed" : "deferred", neterr);
return (res == 5 ? -1 : 1);
}

len = base64_encode(password, strlen(password), &temp);
if (len < 0)
goto encerr;

send_remote_command(fd, "%s", temp);
free(temp);
res = read_remote(fd, 0, NULL);
if (res != 2) {
syslog(LOG_NOTICE, "remote delivery %s: Authentication failed: %s",
res == 5 ? "failed" : "deferred", neterr);
return (res == 5 ? -1 : 1);
}
} else {
syslog(LOG_WARNING, "non-encrypted SMTP login is disabled in config, so skipping it. ");
return (1);
}
}

return (0);
}

static int
open_connection(struct mx_hostentry *h)
{
int fd;

syslog(LOG_INFO, "trying remote delivery to %s [%s] pref %d",
h->host, h->addr, h->pref);

fd = socket(h->ai.ai_family, h->ai.ai_socktype, h->ai.ai_protocol);
if (fd < 0) {
syslog(LOG_INFO, "socket for %s [%s] failed: %m",
h->host, h->addr);
return (-1);
}

if (connect(fd, (struct sockaddr *)&h->sa, h->ai.ai_addrlen) < 0) {
syslog(LOG_INFO, "connect to %s [%s] failed: %m",
h->host, h->addr);
close(fd);
return (-1);
}

return (fd);
}

static void
close_connection(int fd)
{
if (config.ssl != NULL) {
if (((config.features & SECURETRANSFER) != 0) &&
((config.features & NOSSL) == 0))
SSL_shutdown(config.ssl);
SSL_free(config.ssl);
}

close(fd);
}

static void parse_auth_line(char* line, struct smtp_auth_mechanisms* auth) {

line += strlen("AUTH ");

char* method = strtok(line, " ");
while (method) {
if (strcmp(method, "CRAM-MD5") == 0)
auth->cram_md5 = 1;

else if (strcmp(method, "LOGIN") == 0)
auth->login = 1;

method = strtok(NULL, " ");
}
}

int perform_server_greeting(int fd, struct smtp_features* features) {




send_remote_command(fd, "EHLO %s", hostname());

char buffer[EHLO_RESPONSE_SIZE];
memset(buffer, 0, sizeof(buffer));

int res = read_remote(fd, sizeof(buffer) - 1, buffer);


if (res != 2)
return -1;


memset(features, 0, sizeof(*features));


char linebuffer[EHLO_RESPONSE_SIZE];
char* p = buffer;

while (*p) {
char* line = linebuffer;
while (*p && *p != '\n') {
*line++ = *p++;
}




if (!*p) {
return -1;
}



p++;


*--line = '\0';
line = linebuffer;


if (*line == '\0')
break;





if ((strncmp(line, "250-", 4) != 0) && (strncmp(line, "250 ", 4) != 0)) {
syslog(LOG_ERR, "Invalid line: %s\n", line);
return -1;
}


line += 4;


if (strcmp(line, "STARTTLS") == 0)
features->starttls = 1;


else if (strncmp(line, "AUTH ", 5) == 0)
parse_auth_line(line, &features->auth);
}

syslog(LOG_DEBUG, "Server greeting successfully completed");


if (features->starttls)
syslog(LOG_DEBUG, " Server supports STARTTLS");
else
syslog(LOG_DEBUG, " Server does not support STARTTLS");


if (features->auth.cram_md5) {
syslog(LOG_DEBUG, " Server supports CRAM-MD5 authentication");
}
if (features->auth.login) {
syslog(LOG_DEBUG, " Server supports LOGIN authentication");
}

return 0;
}

static int
deliver_to_host(struct qitem *it, struct mx_hostentry *host)
{
struct authuser *a;
struct smtp_features features;
char line[1000], *addrtmp = NULL, *to_addr;
size_t linelen;
int fd, error = 0, do_auth = 0, res = 0;

if (fseek(it->mailf, 0, SEEK_SET) != 0) {
snprintf(errmsg, sizeof(errmsg), "can not seek: %s", strerror(errno));
return (-1);
}

fd = open_connection(host);
if (fd < 0)
return (1);

#define READ_REMOTE_CHECK(c, exp) do { res = read_remote(fd, 0, NULL); if (res == 5) { syslog(LOG_ERR, "remote delivery to %s [%s] failed after %s: %s", host->host, host->addr, c, neterr); snprintf(errmsg, sizeof(errmsg), "%s [%s] did not like our %s:\n%s", host->host, host->addr, c, neterr); error = -1; goto out; } else if (res != exp) { syslog(LOG_NOTICE, "remote delivery deferred: %s [%s] failed after %s: %s", host->host, host->addr, c, neterr); error = 1; goto out; } } while (0)


















if ((config.features & SECURETRANSFER) == 0 ||
(config.features & STARTTLS) != 0) {
config.features |= NOSSL;
READ_REMOTE_CHECK("connect", 2);

config.features &= ~NOSSL;
}

if ((config.features & SECURETRANSFER) != 0) {
error = smtp_init_crypto(fd, config.features, &features);
if (error == 0)
syslog(LOG_DEBUG, "SSL initialization successful");
else
goto out;

if ((config.features & STARTTLS) == 0)
READ_REMOTE_CHECK("connect", 2);
}


if (perform_server_greeting(fd, &features) != 0) {
syslog(LOG_ERR, "Could not perform server greeting at %s [%s]: %s",
host->host, host->addr, neterr);
return -1;
}





SLIST_FOREACH(a, &authusers, next) {
if (strcmp(a->host, host->host) == 0) {
do_auth = 1;
break;
}
}

if (do_auth == 1) {




syslog(LOG_INFO, "using SMTP authentication for user %s", a->login);
error = smtp_login(fd, a->login, a->password, &features);
if (error < 0) {
syslog(LOG_ERR, "remote delivery failed:"
" SMTP login failed: %m");
snprintf(errmsg, sizeof(errmsg), "SMTP login to %s failed", host->host);
error = -1;
goto out;
}

else if (error > 0) {
syslog(LOG_WARNING, "SMTP login not available. Trying without.");
}
}


send_remote_command(fd, "MAIL FROM:<%s>", it->sender);
READ_REMOTE_CHECK("MAIL FROM", 2);


if ((addrtmp = strdup(it->addr)) == NULL) {
syslog(LOG_CRIT, "remote delivery deferred: unable to allocate memory");
error = 1;
goto out;
}
to_addr = strtok(addrtmp, ",");
while (to_addr != NULL) {
send_remote_command(fd, "RCPT TO:<%s>", to_addr);
READ_REMOTE_CHECK("RCPT TO", 2);
to_addr = strtok(NULL, ",");
}

send_remote_command(fd, "DATA");
READ_REMOTE_CHECK("DATA", 3);

error = 0;
while (!feof(it->mailf)) {
if (fgets(line, sizeof(line), it->mailf) == NULL)
break;
linelen = strlen(line);
if (linelen == 0 || line[linelen - 1] != '\n') {
syslog(LOG_CRIT, "remote delivery failed: corrupted queue file");
snprintf(errmsg, sizeof(errmsg), "corrupted queue file");
error = -1;
goto out;
}


trim_line(line);





if (line[0] == '.')
linelen++;

if (send_remote_command(fd, "%s", line) != (ssize_t)linelen+1) {
syslog(LOG_NOTICE, "remote delivery deferred: write error");
error = 1;
goto out;
}
}

send_remote_command(fd, ".");
READ_REMOTE_CHECK("final DATA", 2);

send_remote_command(fd, "QUIT");
if (read_remote(fd, 0, NULL) != 2)
syslog(LOG_INFO, "remote delivery succeeded but QUIT failed: %s", neterr);
out:

free(addrtmp);
close_connection(fd);
return (error);
}

int
deliver_remote(struct qitem *it)
{
struct mx_hostentry *hosts, *h;
const char *host;
int port;
int error = 1, smarthost = 0;

port = SMTP_PORT;


if (config.smarthost != NULL) {
host = config.smarthost;
port = config.port;
syslog(LOG_INFO, "using smarthost (%s:%i)", host, port);
smarthost = 1;
} else {
host = strrchr(it->addr, '@');

if (host == NULL) {
snprintf(errmsg, sizeof(errmsg), "Internal error: badly formed address %s",
it->addr);
return(-1);
} else {

host++;
}
}

error = dns_get_mx_list(host, port, &hosts, smarthost);
if (error) {
snprintf(errmsg, sizeof(errmsg), "DNS lookup failure: host %s not found", host);
syslog(LOG_NOTICE, "remote delivery %s: DNS lookup failure: host %s not found",
error < 0 ? "failed" : "deferred",
host);
return (error);
}

for (h = hosts; *h->host != 0; h++) {
switch (deliver_to_host(it, h)) {
case 0:

error = 0;
goto out;
case 1:

error = 1;
break;
default:

error = -1;
goto out;
}
}
out:
free(hosts);

return (error);
}
