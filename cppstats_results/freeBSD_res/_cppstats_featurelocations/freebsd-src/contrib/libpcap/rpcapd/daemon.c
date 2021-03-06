






























#if defined(HAVE_CONFIG_H)
#include <config.h>
#endif

#include "ftmacros.h"
#include "varattrs.h"

#include <errno.h>
#include <stdlib.h>
#include <string.h>

#if defined(_WIN32)
#include <process.h>
#else
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/types.h>
#include <pwd.h>
#endif

#if defined(HAVE_GETSPNAM)
#include <shadow.h>
#endif

#include <pcap.h>

#include "fmtutils.h"
#include "sockutils.h"
#include "portability.h"
#include "rpcap-protocol.h"
#include "daemon.h"
#include "log.h"






#define RPCAP_TIMEOUT_INIT 90







#define RPCAP_TIMEOUT_RUNTIME 180






#define RPCAP_SUSPEND_WRONGAUTH 1


struct daemon_slpars
{
SOCKET sockctrl;
int isactive;
int nullAuthAllowed;
};









struct session {
SOCKET sockctrl;
SOCKET sockdata;
uint8 protocol_version;
pcap_t *fp;
unsigned int TotCapt;
int have_thread;
#if defined(_WIN32)
HANDLE thread;
#else
pthread_t thread;
#endif
};


static int daemon_msg_err(SOCKET sockctrl, uint32 plen);
static int daemon_msg_auth_req(struct daemon_slpars *pars, uint32 plen);
static int daemon_AuthUserPwd(char *username, char *password, char *errbuf);

static int daemon_msg_findallif_req(uint8 ver, struct daemon_slpars *pars,
uint32 plen);

static int daemon_msg_open_req(uint8 ver, struct daemon_slpars *pars,
uint32 plen, char *source, size_t sourcelen);
static int daemon_msg_startcap_req(uint8 ver, struct daemon_slpars *pars,
uint32 plen, char *source, struct session **sessionp,
struct rpcap_sampling *samp_param);
static int daemon_msg_endcap_req(uint8 ver, struct daemon_slpars *pars,
struct session *session);

static int daemon_msg_updatefilter_req(uint8 ver, struct daemon_slpars *pars,
struct session *session, uint32 plen);
static int daemon_unpackapplyfilter(SOCKET sockctrl, struct session *session, uint32 *plenp, char *errbuf);

static int daemon_msg_stats_req(uint8 ver, struct daemon_slpars *pars,
struct session *session, uint32 plen, struct pcap_stat *stats,
unsigned int svrcapt);

static int daemon_msg_setsampling_req(uint8 ver, struct daemon_slpars *pars,
uint32 plen, struct rpcap_sampling *samp_param);

static void daemon_seraddr(struct sockaddr_storage *sockaddrin, struct rpcap_sockaddr *sockaddrout);
#if defined(_WIN32)
static unsigned __stdcall daemon_thrdatamain(void *ptr);
#else
static void *daemon_thrdatamain(void *ptr);
static void noop_handler(int sign);
#endif

static int rpcapd_recv_msg_header(SOCKET sock, struct rpcap_header *headerp);
static int rpcapd_recv(SOCKET sock, char *buffer, size_t toread, uint32 *plen, char *errmsgbuf);
static int rpcapd_discard(SOCKET sock, uint32 len);
static void session_close(struct session *);

static int is_url(const char *source);

int
daemon_serviceloop(SOCKET sockctrl, int isactive, char *passiveClients,
int nullAuthAllowed)
{
struct daemon_slpars pars;
char errbuf[PCAP_ERRBUF_SIZE + 1];
char errmsgbuf[PCAP_ERRBUF_SIZE + 1];
int host_port_check_status;
int nrecv;
struct rpcap_header header;
uint32 plen;
int authenticated = 0;
char source[PCAP_BUF_SIZE+1];
int got_source = 0;
#if !defined(_WIN32)
struct sigaction action;
#endif
struct session *session = NULL;
const char *msg_type_string;
int client_told_us_to_close = 0;


struct pcap_stat stats;
unsigned int svrcapt;

struct rpcap_sampling samp_param;


fd_set rfds;
struct timeval tv;
int retval;

*errbuf = 0;


pars.sockctrl = sockctrl;
pars.isactive = isactive;
pars.nullAuthAllowed = nullAuthAllowed;










if (pars.isactive)
{

free(passiveClients);
passiveClients = NULL;
}
else
{
struct sockaddr_storage from;
socklen_t fromlen;




fromlen = sizeof(struct sockaddr_storage);
if (getpeername(pars.sockctrl, (struct sockaddr *)&from,
&fromlen) == -1)
{
sock_geterror("getpeername()", errmsgbuf, PCAP_ERRBUF_SIZE);
if (rpcap_senderror(pars.sockctrl, 0, PCAP_ERR_NETW, errmsgbuf, errbuf) == -1)
rpcapd_log(LOGPRIO_ERROR, "Send to client failed: %s", errbuf);
goto end;
}




host_port_check_status = sock_check_hostlist(passiveClients, RPCAP_HOSTLIST_SEP, &from, errmsgbuf, PCAP_ERRBUF_SIZE);
free(passiveClients);
passiveClients = NULL;
if (host_port_check_status < 0)
{
if (host_port_check_status == -2) {



rpcapd_log(LOGPRIO_ERROR, "%s", errmsgbuf);
}




if (rpcap_senderror(pars.sockctrl, 0, PCAP_ERR_HOSTNOAUTH, errmsgbuf, errbuf) == -1)
rpcapd_log(LOGPRIO_ERROR, "Send to client failed: %s", errbuf);
goto end;
}
}

#if !defined(_WIN32)









memset(&action, 0, sizeof (action));
action.sa_handler = noop_handler;
action.sa_flags = 0;
sigemptyset(&action.sa_mask);
sigaction(SIGUSR1, &action, NULL);
#endif







while (!authenticated)
{







if (!pars.isactive)
{
FD_ZERO(&rfds);

tv.tv_sec = RPCAP_TIMEOUT_INIT;
tv.tv_usec = 0;

FD_SET(pars.sockctrl, &rfds);

retval = select(pars.sockctrl + 1, &rfds, NULL, NULL, &tv);
if (retval == -1)
{
sock_geterror("select() failed", errmsgbuf, PCAP_ERRBUF_SIZE);
if (rpcap_senderror(pars.sockctrl, 0, PCAP_ERR_NETW, errmsgbuf, errbuf) == -1)
rpcapd_log(LOGPRIO_ERROR, "Send to client failed: %s", errbuf);
goto end;
}



if (retval == 0)
{
if (rpcap_senderror(pars.sockctrl, 0, PCAP_ERR_INITTIMEOUT, "The RPCAP initial timeout has expired", errbuf) == -1)
rpcapd_log(LOGPRIO_ERROR, "Send to client failed: %s", errbuf);
goto end;
}
}




nrecv = rpcapd_recv_msg_header(pars.sockctrl, &header);
if (nrecv == -1)
{

goto end;
}
if (nrecv == -2)
{

goto end;
}

plen = header.plen;





if (header.ver != 0)
{



if (rpcap_senderror(pars.sockctrl, header.ver,
PCAP_ERR_WRONGVER,
"RPCAP version in requests in the authentication phase must be 0",
errbuf) == -1)
{

rpcapd_log(LOGPRIO_ERROR, "Send to client failed: %s", errbuf);
goto end;
}



(void)rpcapd_discard(pars.sockctrl, plen);
goto end;
}

switch (header.type)
{
case RPCAP_MSG_AUTH_REQ:
retval = daemon_msg_auth_req(&pars, plen);
if (retval == -1)
{


goto end;
}
if (retval == -2)
{



continue;
}



authenticated = 1;
break;

case RPCAP_MSG_CLOSE:





(void)rpcapd_discard(pars.sockctrl, plen);

goto end;

case RPCAP_MSG_ERROR:







(void)daemon_msg_err(pars.sockctrl, plen);
goto end;

case RPCAP_MSG_FINDALLIF_REQ:
case RPCAP_MSG_OPEN_REQ:
case RPCAP_MSG_STARTCAP_REQ:
case RPCAP_MSG_UPDATEFILTER_REQ:
case RPCAP_MSG_STATS_REQ:
case RPCAP_MSG_ENDCAP_REQ:
case RPCAP_MSG_SETSAMPLING_REQ:




msg_type_string = rpcap_msg_type_string(header.type);
if (msg_type_string != NULL)
{
pcap_snprintf(errmsgbuf, PCAP_ERRBUF_SIZE, "%s request sent before authentication was completed", msg_type_string);
}
else
{
pcap_snprintf(errmsgbuf, PCAP_ERRBUF_SIZE, "Message of type %u sent before authentication was completed", header.type);
}
if (rpcap_senderror(pars.sockctrl, header.ver,
PCAP_ERR_WRONGMSG, errmsgbuf, errbuf) == -1)
{
rpcapd_log(LOGPRIO_ERROR, "Send to client failed: %s", errbuf);
goto end;
}

if (rpcapd_discard(pars.sockctrl, plen) == -1)
{

goto end;
}
break;

case RPCAP_MSG_PACKET:
case RPCAP_MSG_FINDALLIF_REPLY:
case RPCAP_MSG_OPEN_REPLY:
case RPCAP_MSG_STARTCAP_REPLY:
case RPCAP_MSG_UPDATEFILTER_REPLY:
case RPCAP_MSG_AUTH_REPLY:
case RPCAP_MSG_STATS_REPLY:
case RPCAP_MSG_ENDCAP_REPLY:
case RPCAP_MSG_SETSAMPLING_REPLY:



msg_type_string = rpcap_msg_type_string(header.type);
if (msg_type_string != NULL)
{
pcap_snprintf(errmsgbuf, PCAP_ERRBUF_SIZE, "Server-to-client message %s received from client", msg_type_string);
}
else
{
pcap_snprintf(errmsgbuf, PCAP_ERRBUF_SIZE, "Server-to-client message of type %u received from client", header.type);
}
if (rpcap_senderror(pars.sockctrl, header.ver,
PCAP_ERR_WRONGMSG, errmsgbuf, errbuf) == -1)
{
rpcapd_log(LOGPRIO_ERROR, "Send to client failed: %s", errbuf);
goto end;
}

if (rpcapd_discard(pars.sockctrl, plen) == -1)
{

goto end;
}
break;

default:



pcap_snprintf(errmsgbuf, PCAP_ERRBUF_SIZE, "Unknown message type %u", header.type);
if (rpcap_senderror(pars.sockctrl, header.ver,
PCAP_ERR_WRONGMSG, errmsgbuf, errbuf) == -1)
{
rpcapd_log(LOGPRIO_ERROR, "Send to client failed: %s", errbuf);
goto end;
}

if (rpcapd_discard(pars.sockctrl, plen) == -1)
{

goto end;
}
break;
}
}









stats.ps_ifdrop = 0;
stats.ps_recv = 0;
stats.ps_drop = 0;
svrcapt = 0;




for (;;)
{
errbuf[0] = 0;










if ((!pars.isactive) && ((session == NULL) || ((session != NULL) && (session->sockdata == 0))))
{

FD_ZERO(&rfds);

tv.tv_sec = RPCAP_TIMEOUT_RUNTIME;
tv.tv_usec = 0;

FD_SET(pars.sockctrl, &rfds);

retval = select(pars.sockctrl + 1, &rfds, NULL, NULL, &tv);
if (retval == -1)
{
sock_geterror("select() failed", errmsgbuf, PCAP_ERRBUF_SIZE);
if (rpcap_senderror(pars.sockctrl, 0,
PCAP_ERR_NETW, errmsgbuf, errbuf) == -1)
rpcapd_log(LOGPRIO_ERROR, "Send to client failed: %s", errbuf);
goto end;
}



if (retval == 0)
{
if (rpcap_senderror(pars.sockctrl, 0,
PCAP_ERR_INITTIMEOUT,
"The RPCAP initial timeout has expired",
errbuf) == -1)
rpcapd_log(LOGPRIO_ERROR, "Send to client failed: %s", errbuf);
goto end;
}
}




nrecv = rpcapd_recv_msg_header(pars.sockctrl, &header);
if (nrecv == -1)
{

goto end;
}
if (nrecv == -2)
{

goto end;
}

plen = header.plen;





if (!RPCAP_VERSION_IS_SUPPORTED(header.ver))
{






if (rpcap_senderror(pars.sockctrl,
header.ver, PCAP_ERR_WRONGVER,
"RPCAP version in message isn't supported by the server",
errbuf) == -1)
{

rpcapd_log(LOGPRIO_ERROR, "Send to client failed: %s", errbuf);
goto end;
}


(void)rpcapd_discard(pars.sockctrl, plen);

goto end;
}

switch (header.type)
{
case RPCAP_MSG_ERROR:
{
(void)daemon_msg_err(pars.sockctrl, plen);


break;
}

case RPCAP_MSG_FINDALLIF_REQ:
{
if (daemon_msg_findallif_req(header.ver, &pars, plen) == -1)
{


goto end;
}
break;
}

case RPCAP_MSG_OPEN_REQ:
{









retval = daemon_msg_open_req(header.ver, &pars,
plen, source, sizeof(source));
if (retval == -1)
{


goto end;
}
got_source = 1;
break;
}

case RPCAP_MSG_STARTCAP_REQ:
{
if (!got_source)
{


if (rpcap_senderror(pars.sockctrl,
header.ver,
PCAP_ERR_STARTCAPTURE,
"No capture device was specified",
errbuf) == -1)
{


rpcapd_log(LOGPRIO_ERROR, "Send to client failed: %s", errbuf);
goto end;
}
if (rpcapd_discard(pars.sockctrl, plen) == -1)
{
goto end;
}
break;
}

if (daemon_msg_startcap_req(header.ver, &pars,
plen, source, &session, &samp_param) == -1)
{


goto end;
}
break;
}

case RPCAP_MSG_UPDATEFILTER_REQ:
{
if (session)
{
if (daemon_msg_updatefilter_req(header.ver,
&pars, session, plen) == -1)
{


goto end;
}
}
else
{
if (rpcap_senderror(pars.sockctrl,
header.ver, PCAP_ERR_UPDATEFILTER,
"Device not opened. Cannot update filter",
errbuf) == -1)
{

rpcapd_log(LOGPRIO_ERROR, "Send to client failed: %s", errbuf);
goto end;
}
}
break;
}

case RPCAP_MSG_CLOSE:
{





client_told_us_to_close = 1;
rpcapd_log(LOGPRIO_DEBUG, "The other end system asked to close the connection.");
goto end;
}

case RPCAP_MSG_STATS_REQ:
{
if (daemon_msg_stats_req(header.ver, &pars,
session, plen, &stats, svrcapt) == -1)
{


goto end;
}
break;
}

case RPCAP_MSG_ENDCAP_REQ:
{
if (session)
{

if (pcap_stats(session->fp, &stats))
{
svrcapt = session->TotCapt;
}
else
{
stats.ps_ifdrop = 0;
stats.ps_recv = 0;
stats.ps_drop = 0;
svrcapt = 0;
}

if (daemon_msg_endcap_req(header.ver,
&pars, session) == -1)
{
free(session);
session = NULL;


goto end;
}
free(session);
session = NULL;
}
else
{
rpcap_senderror(pars.sockctrl,
header.ver, PCAP_ERR_ENDCAPTURE,
"Device not opened. Cannot close the capture",
errbuf);
}
break;
}

case RPCAP_MSG_SETSAMPLING_REQ:
{
if (daemon_msg_setsampling_req(header.ver,
&pars, plen, &samp_param) == -1)
{


goto end;
}
break;
}

case RPCAP_MSG_AUTH_REQ:
{




rpcapd_log(LOGPRIO_INFO, "The client sent an RPCAP_MSG_AUTH_REQ message after authentication was completed");
if (rpcap_senderror(pars.sockctrl, header.ver,
PCAP_ERR_WRONGMSG,
"RPCAP_MSG_AUTH_REQ request sent after authentication was completed",
errbuf) == -1)
{
rpcapd_log(LOGPRIO_ERROR, "Send to client failed: %s", errbuf);
goto end;
}

if (rpcapd_discard(pars.sockctrl, plen) == -1)
{

goto end;
}
goto end;

case RPCAP_MSG_PACKET:
case RPCAP_MSG_FINDALLIF_REPLY:
case RPCAP_MSG_OPEN_REPLY:
case RPCAP_MSG_STARTCAP_REPLY:
case RPCAP_MSG_UPDATEFILTER_REPLY:
case RPCAP_MSG_AUTH_REPLY:
case RPCAP_MSG_STATS_REPLY:
case RPCAP_MSG_ENDCAP_REPLY:
case RPCAP_MSG_SETSAMPLING_REPLY:



msg_type_string = rpcap_msg_type_string(header.type);
if (msg_type_string != NULL)
{
rpcapd_log(LOGPRIO_INFO, "The client sent a %s server-to-client message", msg_type_string);
pcap_snprintf(errmsgbuf, PCAP_ERRBUF_SIZE, "Server-to-client message %s received from client", msg_type_string);
}
else
{
rpcapd_log(LOGPRIO_INFO, "The client sent a server-to-client message of type %u", header.type);
pcap_snprintf(errmsgbuf, PCAP_ERRBUF_SIZE, "Server-to-client message of type %u received from client", header.type);
}
if (rpcap_senderror(pars.sockctrl, header.ver,
PCAP_ERR_WRONGMSG, errmsgbuf, errbuf) == -1)
{
rpcapd_log(LOGPRIO_ERROR, "Send to client failed: %s", errbuf);
goto end;
}

if (rpcapd_discard(pars.sockctrl, plen) == -1)
{

goto end;
}
goto end;

default:



rpcapd_log(LOGPRIO_INFO, "The client sent a message of type %u", header.type);
pcap_snprintf(errmsgbuf, PCAP_ERRBUF_SIZE, "Unknown message type %u", header.type);
if (rpcap_senderror(pars.sockctrl, header.ver,
PCAP_ERR_WRONGMSG, errbuf, errmsgbuf) == -1)
{
rpcapd_log(LOGPRIO_ERROR, "Send to client failed: %s", errbuf);
goto end;
}

if (rpcapd_discard(pars.sockctrl, plen) == -1)
{

goto end;
}
goto end;
}
}
}

end:


if (session)
{
session_close(session);
free(session);
session = NULL;
}

sock_close(sockctrl, NULL, 0);


rpcapd_log(LOGPRIO_DEBUG, "I'm exiting from the child loop");

return client_told_us_to_close;
}




static int
daemon_msg_err(SOCKET sockctrl, uint32 plen)
{
char errbuf[PCAP_ERRBUF_SIZE];
char remote_errbuf[PCAP_ERRBUF_SIZE];

if (plen >= PCAP_ERRBUF_SIZE)
{




if (sock_recv(sockctrl, remote_errbuf, PCAP_ERRBUF_SIZE - 1,
SOCK_RECEIVEALL_YES|SOCK_EOF_IS_ERROR, errbuf,
PCAP_ERRBUF_SIZE) == -1)
{

rpcapd_log(LOGPRIO_ERROR, "Read from client failed: %s", errbuf);
return -1;
}
if (rpcapd_discard(sockctrl, plen - (PCAP_ERRBUF_SIZE - 1)) == -1)
{

return -1;
}




remote_errbuf[PCAP_ERRBUF_SIZE - 1] = '\0';
}
else if (plen == 0)
{

remote_errbuf[0] = '\0';
}
else
{
if (sock_recv(sockctrl, remote_errbuf, plen,
SOCK_RECEIVEALL_YES|SOCK_EOF_IS_ERROR, errbuf,
PCAP_ERRBUF_SIZE) == -1)
{

rpcapd_log(LOGPRIO_ERROR, "Read from client failed: %s", errbuf);
return -1;
}




remote_errbuf[plen] = '\0';
}

rpcapd_log(LOGPRIO_ERROR, "Error from client: %s", remote_errbuf);
return 0;
}























static int
daemon_msg_auth_req(struct daemon_slpars *pars, uint32 plen)
{
char errbuf[PCAP_ERRBUF_SIZE];
char errmsgbuf[PCAP_ERRBUF_SIZE];
int status;
struct rpcap_auth auth;
char sendbuf[RPCAP_NETBUF_SIZE];
int sendbufidx = 0;
struct rpcap_authreply *authreply;

status = rpcapd_recv(pars->sockctrl, (char *) &auth, sizeof(struct rpcap_auth), &plen, errmsgbuf);
if (status == -1)
{
return -1;
}
if (status == -2)
{
goto error;
}

switch (ntohs(auth.type))
{
case RPCAP_RMTAUTH_NULL:
{
if (!pars->nullAuthAllowed)
{

pcap_snprintf(errmsgbuf, PCAP_ERRBUF_SIZE,
"Authentication failed; NULL authentication not permitted.");
if (rpcap_senderror(pars->sockctrl, 0,
PCAP_ERR_AUTH_FAILED, errmsgbuf, errbuf) == -1)
{

rpcapd_log(LOGPRIO_ERROR, "Send to client failed: %s", errbuf);
return -1;
}
goto error_noreply;
}
break;
}

case RPCAP_RMTAUTH_PWD:
{
char *username, *passwd;
uint32 usernamelen, passwdlen;

usernamelen = ntohs(auth.slen1);
username = (char *) malloc (usernamelen + 1);
if (username == NULL)
{
pcap_fmt_errmsg_for_errno(errmsgbuf,
PCAP_ERRBUF_SIZE, errno, "malloc() failed");
goto error;
}
status = rpcapd_recv(pars->sockctrl, username, usernamelen, &plen, errmsgbuf);
if (status == -1)
{
free(username);
return -1;
}
if (status == -2)
{
free(username);
goto error;
}
username[usernamelen] = '\0';

passwdlen = ntohs(auth.slen2);
passwd = (char *) malloc (passwdlen + 1);
if (passwd == NULL)
{
pcap_fmt_errmsg_for_errno(errmsgbuf,
PCAP_ERRBUF_SIZE, errno, "malloc() failed");
free(username);
goto error;
}
status = rpcapd_recv(pars->sockctrl, passwd, passwdlen, &plen, errmsgbuf);
if (status == -1)
{
free(username);
free(passwd);
return -1;
}
if (status == -2)
{
free(username);
free(passwd);
goto error;
}
passwd[passwdlen] = '\0';

if (daemon_AuthUserPwd(username, passwd, errmsgbuf))
{




free(username);
free(passwd);
if (rpcap_senderror(pars->sockctrl, 0,
PCAP_ERR_AUTH_FAILED, errmsgbuf, errbuf) == -1)
{

rpcapd_log(LOGPRIO_ERROR, "Send to client failed: %s", errbuf);
return -1;
}











sleep_secs(RPCAP_SUSPEND_WRONGAUTH);
goto error_noreply;
}

free(username);
free(passwd);
break;
}

default:
pcap_snprintf(errmsgbuf, PCAP_ERRBUF_SIZE,
"Authentication type not recognized.");
if (rpcap_senderror(pars->sockctrl, 0,
PCAP_ERR_AUTH_TYPE_NOTSUP, errmsgbuf, errbuf) == -1)
{

rpcapd_log(LOGPRIO_ERROR, "Send to client failed: %s", errbuf);
return -1;
}
goto error_noreply;
}


if (sock_bufferize(NULL, sizeof(struct rpcap_header), NULL, &sendbufidx,
RPCAP_NETBUF_SIZE, SOCKBUF_CHECKONLY, errmsgbuf, PCAP_ERRBUF_SIZE) == -1)
goto error;

rpcap_createhdr((struct rpcap_header *) sendbuf, 0,
RPCAP_MSG_AUTH_REPLY, 0, sizeof(struct rpcap_authreply));

authreply = (struct rpcap_authreply *) &sendbuf[sendbufidx];

if (sock_bufferize(NULL, sizeof(struct rpcap_authreply), NULL, &sendbufidx,
RPCAP_NETBUF_SIZE, SOCKBUF_CHECKONLY, errmsgbuf, PCAP_ERRBUF_SIZE) == -1)
goto error;




memset(authreply, 0, sizeof(struct rpcap_authreply));
authreply->minvers = RPCAP_MIN_VERSION;
authreply->maxvers = RPCAP_MAX_VERSION;


if (sock_send(pars->sockctrl, sendbuf, sendbufidx, errbuf, PCAP_ERRBUF_SIZE) == -1)
{

rpcapd_log(LOGPRIO_ERROR, "Send to client failed: %s", errbuf);
return -1;
}


if (rpcapd_discard(pars->sockctrl, plen) == -1)
{
return -1;
}

return 0;

error:
if (rpcap_senderror(pars->sockctrl, 0, PCAP_ERR_AUTH, errmsgbuf,
errbuf) == -1)
{

rpcapd_log(LOGPRIO_ERROR, "Send to client failed: %s", errbuf);
return -1;
}

error_noreply:

if (rpcapd_discard(pars->sockctrl, plen) == -1)
{
return -1;
}

return -2;
}

static int
daemon_AuthUserPwd(char *username, char *password, char *errbuf)
{
#if defined(_WIN32)


















DWORD error;
HANDLE Token;
char errmsgbuf[PCAP_ERRBUF_SIZE];

if (LogonUser(username, ".", password, LOGON32_LOGON_NETWORK, LOGON32_PROVIDER_DEFAULT, &Token) == 0)
{
pcap_snprintf(errbuf, PCAP_ERRBUF_SIZE, "Authentication failed");
error = GetLastError();
if (error != ERROR_LOGON_FAILURE)
{


pcap_fmt_errmsg_for_win32_err(errmsgbuf,
PCAP_ERRBUF_SIZE, error, "LogonUser() failed");
rpcapd_log(LOGPRIO_ERROR, "%s", errmsgbuf);
}
return -1;
}



if (ImpersonateLoggedOnUser(Token) == 0)
{
pcap_snprintf(errbuf, PCAP_ERRBUF_SIZE, "Authentication failed");
pcap_fmt_errmsg_for_win32_err(errmsgbuf, PCAP_ERRBUF_SIZE,
GetLastError(), "ImpersonateLoggedOnUser() failed");
rpcapd_log(LOGPRIO_ERROR, "%s", errmsgbuf);
CloseHandle(Token);
return -1;
}

CloseHandle(Token);
return 0;

#else



















int error;
struct passwd *user;
char *user_password;
#if defined(HAVE_GETSPNAM)
struct spwd *usersp;
#endif
char *crypt_password;


if ((user = getpwnam(username)) == NULL)
{
pcap_snprintf(errbuf, PCAP_ERRBUF_SIZE, "Authentication failed");
return -1;
}

#if defined(HAVE_GETSPNAM)

if ((usersp = getspnam(username)) == NULL)
{
pcap_snprintf(errbuf, PCAP_ERRBUF_SIZE, "Authentication failed");
return -1;
}
user_password = usersp->sp_pwdp;
#else










user_password = user->pw_passwd;
#endif






errno = 0;
crypt_password = crypt(password, user_password);
if (crypt_password == NULL)
{
error = errno;
pcap_snprintf(errbuf, PCAP_ERRBUF_SIZE, "Authentication failed");
if (error == 0)
{

rpcapd_log(LOGPRIO_ERROR, "crypt() failed");
}
else
{
rpcapd_log(LOGPRIO_ERROR, "crypt() failed: %s",
strerror(error));
}
return -1;
}
if (strcmp(user_password, crypt_password) != 0)
{
pcap_snprintf(errbuf, PCAP_ERRBUF_SIZE, "Authentication failed");
return -1;
}

if (setuid(user->pw_uid))
{
error = errno;
pcap_fmt_errmsg_for_errno(errbuf, PCAP_ERRBUF_SIZE,
error, "setuid");
rpcapd_log(LOGPRIO_ERROR, "setuid() failed: %s",
strerror(error));
return -1;
}











return 0;

#endif

}

static int
daemon_msg_findallif_req(uint8 ver, struct daemon_slpars *pars, uint32 plen)
{
char errbuf[PCAP_ERRBUF_SIZE];
char errmsgbuf[PCAP_ERRBUF_SIZE];
char sendbuf[RPCAP_NETBUF_SIZE];
int sendbufidx = 0;
pcap_if_t *alldevs = NULL;
pcap_if_t *d;
struct pcap_addr *address;
struct rpcap_findalldevs_if *findalldevs_if;
uint32 replylen;
uint16 nif = 0;


if (rpcapd_discard(pars->sockctrl, plen) == -1)
{

return -1;
}


if (pcap_findalldevs(&alldevs, errmsgbuf) == -1)
goto error;

if (alldevs == NULL)
{
if (rpcap_senderror(pars->sockctrl, ver, PCAP_ERR_NOREMOTEIF,
"No interfaces found! Make sure libpcap/WinPcap is properly installed"
" and you have the right to access to the remote device.",
errbuf) == -1)
{
rpcapd_log(LOGPRIO_ERROR, "Send to client failed: %s", errbuf);
return -1;
}
return 0;
}



replylen = 0;
for (d = alldevs; d != NULL; d = d->next)
{
nif++;

if (d->description)
replylen += strlen(d->description);
if (d->name)
replylen += strlen(d->name);

replylen += sizeof(struct rpcap_findalldevs_if);

for (address = d->addresses; address != NULL; address = address->next)
{



switch (address->addr->sa_family)
{
case AF_INET:
#if defined(AF_INET6)
case AF_INET6:
#endif
replylen += (sizeof(struct rpcap_sockaddr) * 4);
break;

default:
break;
}
}
}


if (sock_bufferize(NULL, sizeof(struct rpcap_header), NULL,
&sendbufidx, RPCAP_NETBUF_SIZE, SOCKBUF_CHECKONLY, errmsgbuf,
PCAP_ERRBUF_SIZE) == -1)
goto error;

rpcap_createhdr((struct rpcap_header *) sendbuf, ver,
RPCAP_MSG_FINDALLIF_REPLY, nif, replylen);


for (d = alldevs; d != NULL; d = d->next)
{
uint16 lname, ldescr;

findalldevs_if = (struct rpcap_findalldevs_if *) &sendbuf[sendbufidx];

if (sock_bufferize(NULL, sizeof(struct rpcap_findalldevs_if), NULL,
&sendbufidx, RPCAP_NETBUF_SIZE, SOCKBUF_CHECKONLY, errmsgbuf, PCAP_ERRBUF_SIZE) == -1)
goto error;

memset(findalldevs_if, 0, sizeof(struct rpcap_findalldevs_if));

if (d->description) ldescr = (short) strlen(d->description);
else ldescr = 0;
if (d->name) lname = (short) strlen(d->name);
else lname = 0;

findalldevs_if->desclen = htons(ldescr);
findalldevs_if->namelen = htons(lname);
findalldevs_if->flags = htonl(d->flags);

for (address = d->addresses; address != NULL; address = address->next)
{



switch (address->addr->sa_family)
{
case AF_INET:
#if defined(AF_INET6)
case AF_INET6:
#endif
findalldevs_if->naddr++;
break;

default:
break;
}
}
findalldevs_if->naddr = htons(findalldevs_if->naddr);

if (sock_bufferize(d->name, lname, sendbuf, &sendbufidx,
RPCAP_NETBUF_SIZE, SOCKBUF_BUFFERIZE, errmsgbuf,
PCAP_ERRBUF_SIZE) == -1)
goto error;

if (sock_bufferize(d->description, ldescr, sendbuf, &sendbufidx,
RPCAP_NETBUF_SIZE, SOCKBUF_BUFFERIZE, errmsgbuf,
PCAP_ERRBUF_SIZE) == -1)
goto error;


for (address = d->addresses; address != NULL; address = address->next)
{
struct rpcap_sockaddr *sockaddr;




switch (address->addr->sa_family)
{
case AF_INET:
#if defined(AF_INET6)
case AF_INET6:
#endif
sockaddr = (struct rpcap_sockaddr *) &sendbuf[sendbufidx];
if (sock_bufferize(NULL, sizeof(struct rpcap_sockaddr), NULL,
&sendbufidx, RPCAP_NETBUF_SIZE, SOCKBUF_CHECKONLY, errmsgbuf, PCAP_ERRBUF_SIZE) == -1)
goto error;
daemon_seraddr((struct sockaddr_storage *) address->addr, sockaddr);

sockaddr = (struct rpcap_sockaddr *) &sendbuf[sendbufidx];
if (sock_bufferize(NULL, sizeof(struct rpcap_sockaddr), NULL,
&sendbufidx, RPCAP_NETBUF_SIZE, SOCKBUF_CHECKONLY, errmsgbuf, PCAP_ERRBUF_SIZE) == -1)
goto error;
daemon_seraddr((struct sockaddr_storage *) address->netmask, sockaddr);

sockaddr = (struct rpcap_sockaddr *) &sendbuf[sendbufidx];
if (sock_bufferize(NULL, sizeof(struct rpcap_sockaddr), NULL,
&sendbufidx, RPCAP_NETBUF_SIZE, SOCKBUF_CHECKONLY, errmsgbuf, PCAP_ERRBUF_SIZE) == -1)
goto error;
daemon_seraddr((struct sockaddr_storage *) address->broadaddr, sockaddr);

sockaddr = (struct rpcap_sockaddr *) &sendbuf[sendbufidx];
if (sock_bufferize(NULL, sizeof(struct rpcap_sockaddr), NULL,
&sendbufidx, RPCAP_NETBUF_SIZE, SOCKBUF_CHECKONLY, errmsgbuf, PCAP_ERRBUF_SIZE) == -1)
goto error;
daemon_seraddr((struct sockaddr_storage *) address->dstaddr, sockaddr);
break;

default:
break;
}
}
}


pcap_freealldevs(alldevs);


if (sock_send(pars->sockctrl, sendbuf, sendbufidx, errbuf, PCAP_ERRBUF_SIZE) == -1)
{
rpcapd_log(LOGPRIO_ERROR, "Send to client failed: %s", errbuf);
return -1;
}

return 0;

error:
if (alldevs)
pcap_freealldevs(alldevs);

if (rpcap_senderror(pars->sockctrl, ver, PCAP_ERR_FINDALLIF,
errmsgbuf, errbuf) == -1)
{
rpcapd_log(LOGPRIO_ERROR, "Send to client failed: %s", errbuf);
return -1;
}
return 0;
}





static int
daemon_msg_open_req(uint8 ver, struct daemon_slpars *pars, uint32 plen,
char *source, size_t sourcelen)
{
char errbuf[PCAP_ERRBUF_SIZE];
char errmsgbuf[PCAP_ERRBUF_SIZE];
pcap_t *fp;
int nread;
char sendbuf[RPCAP_NETBUF_SIZE];
int sendbufidx = 0;
struct rpcap_openreply *openreply;

if (plen > sourcelen - 1)
{
pcap_snprintf(errmsgbuf, PCAP_ERRBUF_SIZE, "Source string too long");
goto error;
}

nread = sock_recv(pars->sockctrl, source, plen,
SOCK_RECEIVEALL_YES|SOCK_EOF_IS_ERROR, errbuf, PCAP_ERRBUF_SIZE);
if (nread == -1)
{
rpcapd_log(LOGPRIO_ERROR, "Read from client failed: %s", errbuf);
return -1;
}
source[nread] = '\0';
plen -= nread;



if (is_url(source))
{
pcap_snprintf(errmsgbuf, PCAP_ERRBUF_SIZE, "Source string refers to a remote device");
goto error;
}



if ((fp = pcap_open_live(source,
1500 ,
0 ,
1000 ,
errmsgbuf)) == NULL)
goto error;


if (sock_bufferize(NULL, sizeof(struct rpcap_header), NULL, &sendbufidx,
RPCAP_NETBUF_SIZE, SOCKBUF_CHECKONLY, errmsgbuf, PCAP_ERRBUF_SIZE) == -1)
goto error;

rpcap_createhdr((struct rpcap_header *) sendbuf, ver,
RPCAP_MSG_OPEN_REPLY, 0, sizeof(struct rpcap_openreply));

openreply = (struct rpcap_openreply *) &sendbuf[sendbufidx];

if (sock_bufferize(NULL, sizeof(struct rpcap_openreply), NULL, &sendbufidx,
RPCAP_NETBUF_SIZE, SOCKBUF_CHECKONLY, errmsgbuf, PCAP_ERRBUF_SIZE) == -1)
goto error;

memset(openreply, 0, sizeof(struct rpcap_openreply));
openreply->linktype = htonl(pcap_datalink(fp));
openreply->tzoff = 0;


pcap_close(fp);


if (sock_send(pars->sockctrl, sendbuf, sendbufidx, errbuf, PCAP_ERRBUF_SIZE) == -1)
{
rpcapd_log(LOGPRIO_ERROR, "Send to client failed: %s", errbuf);
return -1;
}
return 0;

error:
if (rpcap_senderror(pars->sockctrl, ver, PCAP_ERR_OPEN,
errmsgbuf, errbuf) == -1)
{

rpcapd_log(LOGPRIO_ERROR, "Send to client failed: %s", errbuf);
return -1;
}


if (rpcapd_discard(pars->sockctrl, plen) == -1)
{
return -1;
}
return 0;
}





static int
daemon_msg_startcap_req(uint8 ver, struct daemon_slpars *pars, uint32 plen,
char *source, struct session **sessionp,
struct rpcap_sampling *samp_param _U_)
{
char errbuf[PCAP_ERRBUF_SIZE];
char errmsgbuf[PCAP_ERRBUF_SIZE];
char portdata[PCAP_BUF_SIZE];
char peerhost[PCAP_BUF_SIZE];
struct session *session = NULL;
int status;
char sendbuf[RPCAP_NETBUF_SIZE];
int sendbufidx = 0;


struct addrinfo hints;
struct addrinfo *addrinfo;
struct sockaddr_storage saddr;
socklen_t saddrlen;
int ret;


struct rpcap_startcapreq startcapreq;
struct rpcap_startcapreply *startcapreply;
int serveropen_dp;

addrinfo = NULL;

status = rpcapd_recv(pars->sockctrl, (char *) &startcapreq,
sizeof(struct rpcap_startcapreq), &plen, errmsgbuf);
if (status == -1)
{
goto fatal_error;
}
if (status == -2)
{
goto error;
}

startcapreq.flags = ntohs(startcapreq.flags);


session = malloc(sizeof(struct session));
if (session == NULL)
{
pcap_snprintf(errmsgbuf, PCAP_ERRBUF_SIZE, "Can't allocate session structure");
goto error;
}

session->sockdata = INVALID_SOCKET;

session->have_thread = 0;









#if defined(_WIN32)
session->thread = INVALID_HANDLE_VALUE;
#else
memset(&session->thread, 0, sizeof(session->thread));
#endif


if ((session->fp = pcap_open_live(source,
ntohl(startcapreq.snaplen),
(startcapreq.flags & RPCAP_STARTCAPREQ_FLAG_PROMISC) ? 1 : 0 ,
ntohl(startcapreq.read_timeout),
errmsgbuf)) == NULL)
goto error;

#if 0

fp->rmt_samp.method = samp_param->method;
fp->rmt_samp.value = samp_param->value;
#endif






serveropen_dp = (startcapreq.flags & RPCAP_STARTCAPREQ_FLAG_SERVEROPEN) || (startcapreq.flags & RPCAP_STARTCAPREQ_FLAG_DGRAM) || pars->isactive;










saddrlen = sizeof(struct sockaddr_storage);
if (getpeername(pars->sockctrl, (struct sockaddr *) &saddr, &saddrlen) == -1)
{
sock_geterror("getpeername()", errmsgbuf, PCAP_ERRBUF_SIZE);
goto error;
}

memset(&hints, 0, sizeof(struct addrinfo));
hints.ai_socktype = (startcapreq.flags & RPCAP_STARTCAPREQ_FLAG_DGRAM) ? SOCK_DGRAM : SOCK_STREAM;
hints.ai_family = saddr.ss_family;


if (serveropen_dp)
{
pcap_snprintf(portdata, sizeof portdata, "%d", ntohs(startcapreq.portdata));


if (getnameinfo((struct sockaddr *) &saddr, saddrlen, peerhost,
sizeof(peerhost), NULL, 0, NI_NUMERICHOST))
{
sock_geterror("getnameinfo()", errmsgbuf, PCAP_ERRBUF_SIZE);
goto error;
}

if (sock_initaddress(peerhost, portdata, &hints, &addrinfo, errmsgbuf, PCAP_ERRBUF_SIZE) == -1)
goto error;

if ((session->sockdata = sock_open(addrinfo, SOCKOPEN_CLIENT, 0, errmsgbuf, PCAP_ERRBUF_SIZE)) == INVALID_SOCKET)
goto error;
}
else
{
hints.ai_flags = AI_PASSIVE;


if (sock_initaddress(NULL, "0", &hints, &addrinfo, errmsgbuf, PCAP_ERRBUF_SIZE) == -1)
goto error;

if ((session->sockdata = sock_open(addrinfo, SOCKOPEN_SERVER, 1 , errmsgbuf, PCAP_ERRBUF_SIZE)) == INVALID_SOCKET)
goto error;


saddrlen = sizeof(struct sockaddr_storage);
if (getsockname(session->sockdata, (struct sockaddr *) &saddr, &saddrlen) == -1)
{
sock_geterror("getsockname()", errmsgbuf, PCAP_ERRBUF_SIZE);
goto error;
}


if (getnameinfo((struct sockaddr *) &saddr, saddrlen, NULL,
0, portdata, sizeof(portdata), NI_NUMERICSERV))
{
sock_geterror("getnameinfo()", errmsgbuf, PCAP_ERRBUF_SIZE);
goto error;
}
}


freeaddrinfo(addrinfo);
addrinfo = NULL;


session->sockctrl = pars->sockctrl;
session->protocol_version = ver;


ret = daemon_unpackapplyfilter(pars->sockctrl, session, &plen, errmsgbuf);
if (ret == -1)
{

goto fatal_error;
}
if (ret == -2)
{

goto error;
}


if (sock_bufferize(NULL, sizeof(struct rpcap_header), NULL, &sendbufidx,
RPCAP_NETBUF_SIZE, SOCKBUF_CHECKONLY, errmsgbuf, PCAP_ERRBUF_SIZE) == -1)
goto error;

rpcap_createhdr((struct rpcap_header *) sendbuf, ver,
RPCAP_MSG_STARTCAP_REPLY, 0, sizeof(struct rpcap_startcapreply));

startcapreply = (struct rpcap_startcapreply *) &sendbuf[sendbufidx];

if (sock_bufferize(NULL, sizeof(struct rpcap_startcapreply), NULL,
&sendbufidx, RPCAP_NETBUF_SIZE, SOCKBUF_CHECKONLY, errmsgbuf, PCAP_ERRBUF_SIZE) == -1)
goto error;

memset(startcapreply, 0, sizeof(struct rpcap_startcapreply));
startcapreply->bufsize = htonl(pcap_bufsize(session->fp));

if (!serveropen_dp)
{
unsigned short port = (unsigned short)strtoul(portdata,NULL,10);
startcapreply->portdata = htons(port);
}

if (sock_send(pars->sockctrl, sendbuf, sendbufidx, errbuf, PCAP_ERRBUF_SIZE) == -1)
{

rpcapd_log(LOGPRIO_ERROR, "Send to client failed: %s", errbuf);
goto fatal_error;
}

if (!serveropen_dp)
{
SOCKET socktemp;


saddrlen = sizeof(struct sockaddr_storage);

socktemp = accept(session->sockdata, (struct sockaddr *) &saddr, &saddrlen);

if (socktemp == INVALID_SOCKET)
{
sock_geterror("accept()", errbuf, PCAP_ERRBUF_SIZE);
rpcapd_log(LOGPRIO_ERROR, "Accept of data connection failed: %s",
errbuf);
goto error;
}


sock_close(session->sockdata, NULL, 0);
session->sockdata = socktemp;
}


#if defined(_WIN32)
session->thread = (HANDLE)_beginthreadex(NULL, 0, daemon_thrdatamain,
(void *) session, 0, NULL);
if (session->thread == 0)
{
pcap_snprintf(errbuf, PCAP_ERRBUF_SIZE, "Error creating the data thread");
goto error;
}
#else
ret = pthread_create(&session->thread, NULL, daemon_thrdatamain,
(void *) session);
if (ret != 0)
{
pcap_fmt_errmsg_for_errno(errbuf, PCAP_ERRBUF_SIZE,
ret, "Error creating the data thread");
goto error;
}
#endif
session->have_thread = 1;


if (rpcapd_discard(pars->sockctrl, plen) == -1)
goto fatal_error;

*sessionp = session;
return 0;

error:




*sessionp = NULL;

if (addrinfo)
freeaddrinfo(addrinfo);

if (session)
{
session_close(session);
free(session);
}

if (rpcap_senderror(pars->sockctrl, ver, PCAP_ERR_STARTCAPTURE,
errmsgbuf, errbuf) == -1)
{

rpcapd_log(LOGPRIO_ERROR, "Send to client failed: %s", errbuf);
return -1;
}


if (rpcapd_discard(pars->sockctrl, plen) == -1)
{

return -1;
}

return 0;

fatal_error:




*sessionp = NULL;

if (session)
{
session_close(session);
free(session);
}

return -1;
}

static int
daemon_msg_endcap_req(uint8 ver, struct daemon_slpars *pars,
struct session *session)
{
char errbuf[PCAP_ERRBUF_SIZE];
struct rpcap_header header;

session_close(session);

rpcap_createhdr(&header, ver, RPCAP_MSG_ENDCAP_REPLY, 0, 0);

if (sock_send(pars->sockctrl, (char *) &header, sizeof(struct rpcap_header), errbuf, PCAP_ERRBUF_SIZE) == -1)
{

rpcapd_log(LOGPRIO_ERROR, "Send to client failed: %s", errbuf);
return -1;
}

return 0;
}
















#define RPCAP_BPF_MAXINSNS 8192

static int
daemon_unpackapplyfilter(SOCKET sockctrl, struct session *session, uint32 *plenp, char *errmsgbuf)
{
int status;
struct rpcap_filter filter;
struct rpcap_filterbpf_insn insn;
struct bpf_insn *bf_insn;
struct bpf_program bf_prog;
unsigned int i;

status = rpcapd_recv(sockctrl, (char *) &filter,
sizeof(struct rpcap_filter), plenp, errmsgbuf);
if (status == -1)
{
return -1;
}
if (status == -2)
{
return -2;
}

bf_prog.bf_len = ntohl(filter.nitems);

if (ntohs(filter.filtertype) != RPCAP_UPDATEFILTER_BPF)
{
pcap_snprintf(errmsgbuf, PCAP_ERRBUF_SIZE, "Only BPF/NPF filters are currently supported");
return -2;
}

if (bf_prog.bf_len > RPCAP_BPF_MAXINSNS)
{
pcap_snprintf(errmsgbuf, PCAP_ERRBUF_SIZE,
"Filter program is larger than the maximum size of %u instructions",
RPCAP_BPF_MAXINSNS);
return -2;
}
bf_insn = (struct bpf_insn *) malloc (sizeof(struct bpf_insn) * bf_prog.bf_len);
if (bf_insn == NULL)
{
pcap_fmt_errmsg_for_errno(errmsgbuf, PCAP_ERRBUF_SIZE,
errno, "malloc() failed");
return -2;
}

bf_prog.bf_insns = bf_insn;

for (i = 0; i < bf_prog.bf_len; i++)
{
status = rpcapd_recv(sockctrl, (char *) &insn,
sizeof(struct rpcap_filterbpf_insn), plenp, errmsgbuf);
if (status == -1)
{
return -1;
}
if (status == -2)
{
return -2;
}

bf_insn->code = ntohs(insn.code);
bf_insn->jf = insn.jf;
bf_insn->jt = insn.jt;
bf_insn->k = ntohl(insn.k);

bf_insn++;
}

if (bpf_validate(bf_prog.bf_insns, bf_prog.bf_len) == 0)
{
pcap_snprintf(errmsgbuf, PCAP_ERRBUF_SIZE, "The filter contains bogus instructions");
return -2;
}

if (pcap_setfilter(session->fp, &bf_prog))
{
pcap_snprintf(errmsgbuf, PCAP_ERRBUF_SIZE, "RPCAP error: %s", pcap_geterr(session->fp));
return -2;
}

return 0;
}

static int
daemon_msg_updatefilter_req(uint8 ver, struct daemon_slpars *pars,
struct session *session, uint32 plen)
{
char errbuf[PCAP_ERRBUF_SIZE];
char errmsgbuf[PCAP_ERRBUF_SIZE];
int ret;
struct rpcap_header header;

ret = daemon_unpackapplyfilter(pars->sockctrl, session, &plen, errmsgbuf);
if (ret == -1)
{

return -1;
}
if (ret == -2)
{

goto error;
}


if (rpcapd_discard(pars->sockctrl, plen) == -1)
{

return -1;
}


rpcap_createhdr(&header, ver, RPCAP_MSG_UPDATEFILTER_REPLY, 0, 0);

if (sock_send(pars->sockctrl, (char *) &header, sizeof (struct rpcap_header), pcap_geterr(session->fp), PCAP_ERRBUF_SIZE))
{

rpcapd_log(LOGPRIO_ERROR, "Send to client failed: %s", errbuf);
return -1;
}

return 0;

error:
if (rpcapd_discard(pars->sockctrl, plen) == -1)
{
return -1;
}
rpcap_senderror(pars->sockctrl, ver, PCAP_ERR_UPDATEFILTER,
errmsgbuf, NULL);

return 0;
}




static int
daemon_msg_setsampling_req(uint8 ver, struct daemon_slpars *pars, uint32 plen,
struct rpcap_sampling *samp_param)
{
char errbuf[PCAP_ERRBUF_SIZE];
char errmsgbuf[PCAP_ERRBUF_SIZE];
struct rpcap_header header;
struct rpcap_sampling rpcap_samp;
int status;

status = rpcapd_recv(pars->sockctrl, (char *) &rpcap_samp, sizeof(struct rpcap_sampling), &plen, errmsgbuf);
if (status == -1)
{
return -1;
}
if (status == -2)
{
goto error;
}


samp_param->method = rpcap_samp.method;
samp_param->value = ntohl(rpcap_samp.value);


rpcap_createhdr(&header, ver, RPCAP_MSG_SETSAMPLING_REPLY, 0, 0);

if (sock_send(pars->sockctrl, (char *) &header, sizeof (struct rpcap_header), errbuf, PCAP_ERRBUF_SIZE) == -1)
{

rpcapd_log(LOGPRIO_ERROR, "Send to client failed: %s", errbuf);
return -1;
}

if (rpcapd_discard(pars->sockctrl, plen) == -1)
{
return -1;
}

return 0;

error:
if (rpcap_senderror(pars->sockctrl, ver, PCAP_ERR_SETSAMPLING,
errmsgbuf, errbuf) == -1)
{

rpcapd_log(LOGPRIO_ERROR, "Send to client failed: %s", errbuf);
return -1;
}


if (rpcapd_discard(pars->sockctrl, plen) == -1)
{
return -1;
}

return 0;
}

static int
daemon_msg_stats_req(uint8 ver, struct daemon_slpars *pars,
struct session *session, uint32 plen, struct pcap_stat *stats,
unsigned int svrcapt)
{
char errbuf[PCAP_ERRBUF_SIZE];
char errmsgbuf[PCAP_ERRBUF_SIZE];
char sendbuf[RPCAP_NETBUF_SIZE];
int sendbufidx = 0;
struct rpcap_stats *netstats;


if (rpcapd_discard(pars->sockctrl, plen) == -1)
{

return -1;
}

if (sock_bufferize(NULL, sizeof(struct rpcap_header), NULL,
&sendbufidx, RPCAP_NETBUF_SIZE, SOCKBUF_CHECKONLY, errmsgbuf, PCAP_ERRBUF_SIZE) == -1)
goto error;

rpcap_createhdr((struct rpcap_header *) sendbuf, ver,
RPCAP_MSG_STATS_REPLY, 0, (uint16) sizeof(struct rpcap_stats));

netstats = (struct rpcap_stats *) &sendbuf[sendbufidx];

if (sock_bufferize(NULL, sizeof(struct rpcap_stats), NULL,
&sendbufidx, RPCAP_NETBUF_SIZE, SOCKBUF_CHECKONLY, errmsgbuf, PCAP_ERRBUF_SIZE) == -1)
goto error;

if (session && session->fp)
{
if (pcap_stats(session->fp, stats) == -1)
{
pcap_snprintf(errmsgbuf, PCAP_ERRBUF_SIZE, "%s", pcap_geterr(session->fp));
goto error;
}

netstats->ifdrop = htonl(stats->ps_ifdrop);
netstats->ifrecv = htonl(stats->ps_recv);
netstats->krnldrop = htonl(stats->ps_drop);
netstats->svrcapt = htonl(session->TotCapt);
}
else
{



netstats->ifdrop = htonl(stats->ps_ifdrop);
netstats->ifrecv = htonl(stats->ps_recv);
netstats->krnldrop = htonl(stats->ps_drop);
netstats->svrcapt = htonl(svrcapt);
}


if (sock_send(pars->sockctrl, sendbuf, sendbufidx, errbuf, PCAP_ERRBUF_SIZE) == -1)
{
rpcapd_log(LOGPRIO_ERROR, "Send to client failed: %s", errbuf);
return -1;
}

return 0;

error:
rpcap_senderror(pars->sockctrl, ver, PCAP_ERR_GETSTATS,
errmsgbuf, NULL);
return 0;
}

#if defined(_WIN32)
static unsigned __stdcall
#else
static void *
#endif
daemon_thrdatamain(void *ptr)
{
char errbuf[PCAP_ERRBUF_SIZE + 1];
struct session *session;
int retval;
struct rpcap_pkthdr *net_pkt_header;
struct pcap_pkthdr *pkt_header;
u_char *pkt_data;
size_t sendbufsize;
char *sendbuf;
int sendbufidx;
int status;
#if !defined(_WIN32)
sigset_t sigusr1;
#endif

session = (struct session *) ptr;

session->TotCapt = 0;


memset(errbuf, 0, sizeof(errbuf));





if (pcap_snapshot(session->fp) < 0)
{




rpcapd_log(LOGPRIO_ERROR,
"Unable to allocate the buffer for this child thread: snapshot length of %d is negative",
pcap_snapshot(session->fp));
sendbuf = NULL;
goto error;
}










sendbufsize = sizeof(struct rpcap_header) + sizeof(struct rpcap_pkthdr) + pcap_snapshot(session->fp);
sendbuf = (char *) malloc (sendbufsize);
if (sendbuf == NULL)
{
rpcapd_log(LOGPRIO_ERROR,
"Unable to allocate the buffer for this child thread");
goto error;
}

#if !defined(_WIN32)







sigemptyset(&sigusr1);
sigaddset(&sigusr1, SIGUSR1);
pthread_sigmask(SIG_BLOCK, &sigusr1, NULL);
#endif


for (;;)
{
#if !defined(_WIN32)



pthread_sigmask(SIG_UNBLOCK, &sigusr1, NULL);
#endif
retval = pcap_next_ex(session->fp, &pkt_header, (const u_char **) &pkt_data);
#if !defined(_WIN32)



pthread_sigmask(SIG_BLOCK, &sigusr1, NULL);
#endif
if (retval < 0)
break;
if (retval == 0)
continue;

sendbufidx = 0;


if (sock_bufferize(NULL, sizeof(struct rpcap_header), NULL,
&sendbufidx, sendbufsize, SOCKBUF_CHECKONLY, errbuf,
PCAP_ERRBUF_SIZE) == -1)
{
rpcapd_log(LOGPRIO_ERROR,
"sock_bufferize() error sending packet message: %s",
errbuf);
goto error;
}

rpcap_createhdr((struct rpcap_header *) sendbuf,
session->protocol_version, RPCAP_MSG_PACKET, 0,
(uint16) (sizeof(struct rpcap_pkthdr) + pkt_header->caplen));

net_pkt_header = (struct rpcap_pkthdr *) &sendbuf[sendbufidx];


if (sock_bufferize(NULL, sizeof(struct rpcap_pkthdr), NULL,
&sendbufidx, sendbufsize, SOCKBUF_CHECKONLY, errbuf,
PCAP_ERRBUF_SIZE) == -1)
{
rpcapd_log(LOGPRIO_ERROR,
"sock_bufferize() error sending packet message: %s",
errbuf);
goto error;
}

net_pkt_header->caplen = htonl(pkt_header->caplen);
net_pkt_header->len = htonl(pkt_header->len);
net_pkt_header->npkt = htonl(++(session->TotCapt));
net_pkt_header->timestamp_sec = htonl(pkt_header->ts.tv_sec);
net_pkt_header->timestamp_usec = htonl(pkt_header->ts.tv_usec);


if (sock_bufferize((char *) pkt_data, pkt_header->caplen,
sendbuf, &sendbufidx, sendbufsize, SOCKBUF_BUFFERIZE,
errbuf, PCAP_ERRBUF_SIZE) == -1)
{
rpcapd_log(LOGPRIO_ERROR,
"sock_bufferize() error sending packet message: %s",
errbuf);
goto error;
}




status = sock_send(session->sockdata, sendbuf, sendbufidx, errbuf, PCAP_ERRBUF_SIZE);
if (status < 0)
{
if (status == -1)
{





rpcapd_log(LOGPRIO_ERROR,
"Send of packet to client failed: %s",
errbuf);
}




goto error;
}
}

if (retval < 0 && retval != PCAP_ERROR_BREAK)
{







pcap_snprintf(errbuf, PCAP_ERRBUF_SIZE, "Error reading the packets: %s", pcap_geterr(session->fp));
rpcap_senderror(session->sockctrl, session->protocol_version,
PCAP_ERR_READEX, errbuf, NULL);
}

error:



free(sendbuf);

return 0;
}

#if !defined(_WIN32)





static void noop_handler(int sign _U_)
{
}
#endif
















static void
daemon_seraddr(struct sockaddr_storage *sockaddrin, struct rpcap_sockaddr *sockaddrout)
{
memset(sockaddrout, 0, sizeof(struct sockaddr_storage));


if (sockaddrin == NULL) return;


switch (sockaddrin->ss_family)
{
case AF_INET:
{
struct sockaddr_in *sockaddrin_ipv4;
struct rpcap_sockaddr_in *sockaddrout_ipv4;

sockaddrin_ipv4 = (struct sockaddr_in *) sockaddrin;
sockaddrout_ipv4 = (struct rpcap_sockaddr_in *) sockaddrout;
sockaddrout_ipv4->family = htons(RPCAP_AF_INET);
sockaddrout_ipv4->port = htons(sockaddrin_ipv4->sin_port);
memcpy(&sockaddrout_ipv4->addr, &sockaddrin_ipv4->sin_addr, sizeof(sockaddrout_ipv4->addr));
memset(sockaddrout_ipv4->zero, 0, sizeof(sockaddrout_ipv4->zero));
break;
}

#if defined(AF_INET6)
case AF_INET6:
{
struct sockaddr_in6 *sockaddrin_ipv6;
struct rpcap_sockaddr_in6 *sockaddrout_ipv6;

sockaddrin_ipv6 = (struct sockaddr_in6 *) sockaddrin;
sockaddrout_ipv6 = (struct rpcap_sockaddr_in6 *) sockaddrout;
sockaddrout_ipv6->family = htons(RPCAP_AF_INET6);
sockaddrout_ipv6->port = htons(sockaddrin_ipv6->sin6_port);
sockaddrout_ipv6->flowinfo = htonl(sockaddrin_ipv6->sin6_flowinfo);
memcpy(&sockaddrout_ipv6->addr, &sockaddrin_ipv6->sin6_addr, sizeof(sockaddrout_ipv6->addr));
sockaddrout_ipv6->scope_id = htonl(sockaddrin_ipv6->sin6_scope_id);
break;
}
#endif
}
}





void sleep_secs(int secs)
{
#if defined(_WIN32)
Sleep(secs*1000);
#else
unsigned secs_remaining;

if (secs <= 0)
return;
secs_remaining = secs;
while (secs_remaining != 0)
secs_remaining = sleep(secs_remaining);
#endif
}




static int
rpcapd_recv_msg_header(SOCKET sock, struct rpcap_header *headerp)
{
int nread;
char errbuf[PCAP_ERRBUF_SIZE];

nread = sock_recv(sock, (char *) headerp, sizeof(struct rpcap_header),
SOCK_RECEIVEALL_YES|SOCK_EOF_ISNT_ERROR, errbuf, PCAP_ERRBUF_SIZE);
if (nread == -1)
{

rpcapd_log(LOGPRIO_ERROR, "Read from client failed: %s", errbuf);
return -1;
}
if (nread == 0)
{

return -2;
}
headerp->plen = ntohl(headerp->plen);
return 0;
}










static int
rpcapd_recv(SOCKET sock, char *buffer, size_t toread, uint32 *plen, char *errmsgbuf)
{
int nread;
char errbuf[PCAP_ERRBUF_SIZE];

if (toread > *plen)
{

pcap_snprintf(errmsgbuf, PCAP_ERRBUF_SIZE, "Message payload is too short");
return -2;
}
nread = sock_recv(sock, buffer, toread,
SOCK_RECEIVEALL_YES|SOCK_EOF_IS_ERROR, errbuf, PCAP_ERRBUF_SIZE);
if (nread == -1)
{
rpcapd_log(LOGPRIO_ERROR, "Read from client failed: %s", errbuf);
return -1;
}
*plen -= nread;
return 0;
}







static int
rpcapd_discard(SOCKET sock, uint32 len)
{
char errbuf[PCAP_ERRBUF_SIZE + 1];

if (len != 0)
{
if (sock_discard(sock, len, errbuf, PCAP_ERRBUF_SIZE) == -1)
{

rpcapd_log(LOGPRIO_ERROR, "Read from client failed: %s", errbuf);
return -1;
}
}
return 0;
}










static void session_close(struct session *session)
{
if (session->have_thread)
{







pcap_breakloop(session->fp);

#if defined(_WIN32)











SetEvent(pcap_getevent(session->fp));







WaitForSingleObject(session->thread, INFINITE);





CloseHandle(session->thread);
session->have_thread = 0;
session->thread = INVALID_HANDLE_VALUE;
#else







pthread_kill(session->thread, SIGUSR1);







pthread_join(session->thread, NULL);
session->have_thread = 0;
memset(&session->thread, 0, sizeof(session->thread));
#endif
}

if (session->sockdata != INVALID_SOCKET)
{
sock_close(session->sockdata, NULL, 0);
session->sockdata = INVALID_SOCKET;
}

if (session->fp)
{
pcap_close(session->fp);
session->fp = NULL;
}
}







static int
is_url(const char *source)
{
char *colonp;





















colonp = strchr(source, ':');
if (colonp == NULL)
{



return (0);
}











if (strncmp(colonp + 1, "//", 2) != 0)
{



return (0);
}




return (1);
}
