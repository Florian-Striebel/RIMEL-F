































#if defined(HAVE_CONFIG_H)
#include <config.h>
#endif

#include "ftmacros.h"

#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <pcap.h>
#include <signal.h>

#include "fmtutils.h"
#include "sockutils.h"
#include "varattrs.h"
#include "portability.h"
#include "rpcapd.h"
#include "config_params.h"
#include "fileconf.h"
#include "rpcap-protocol.h"
#include "daemon.h"
#include "log.h"

#if defined(_WIN32)
#include <process.h>
#include "win32-svc.h"
#include "getopt.h"
#else
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#endif




struct listen_sock {
struct listen_sock *next;
SOCKET sock;
};


char hostlist[MAX_HOST_LIST + 1];
struct active_pars activelist[MAX_ACTIVE_LIST];
int nullAuthAllowed;
static struct listen_sock *listen_socks;
char loadfile[MAX_LINE + 1];
static int passivemode = 1;
static struct addrinfo mainhints;
static char address[MAX_LINE + 1];
static char port[MAX_LINE + 1];
#if defined(_WIN32)
static HANDLE state_change_event;
#endif
static volatile sig_atomic_t shutdown_server;
static volatile sig_atomic_t reread_config;

extern char *optarg;


#if defined(_WIN32)
static unsigned __stdcall main_active(void *ptr);
static BOOL WINAPI main_ctrl_event(DWORD);
#else
static void *main_active(void *ptr);
static void main_terminate(int sign);
static void main_reread_config(int sign);
#endif
static void accept_connections(void);
static void accept_connection(SOCKET listen_sock);
#if !defined(_WIN32)
static void main_reap_children(int sign);
#endif
#if defined(_WIN32)
static unsigned __stdcall main_passive_serviceloop_thread(void *ptr);
#endif

#define RPCAP_ACTIVE_WAIT 30




static void printusage(void)
{
const char *usagetext =
"USAGE:"
" " PROGRAM_NAME " [-b <address>] [-p <port>] [-4] [-l <host_list>] [-a <host,port>]\n"
" [-n] [-v] [-d] "
#if !defined(_WIN32)
"[-i] "
#endif
"[-D] [-s <config_file>] [-f <config_file>]\n\n"
" -b <address> the address to bind to (either numeric or literal).\n"
" Default: binds to all local IPv4 and IPv6 addresses\n\n"
" -p <port> the port to bind to.\n"
" Default: binds to port " RPCAP_DEFAULT_NETPORT "\n\n"
" -4 use only IPv4.\n"
" Default: use both IPv4 and IPv6 waiting sockets\n\n"
" -l <host_list> a file that contains a list of hosts that are allowed\n"
" to connect to this server (if more than one, list them one\n"
" per line).\n"
" We suggest to use literal names (instead of numeric ones)\n"
" in order to avoid problems with different address families.\n\n"
" -n permit NULL authentication (usually used with '-l')\n\n"
" -a <host,port> run in active mode when connecting to 'host' on port 'port'\n"
" In case 'port' is omitted, the default port (" RPCAP_DEFAULT_NETPORT_ACTIVE ") is used\n\n"
" -v run in active mode only (default: if '-a' is specified, it\n"
" accepts passive connections as well)\n\n"
" -d run in daemon mode (UNIX only) or as a service (Win32 only)\n"
" Warning (Win32): this switch is provided automatically when\n"
" the service is started from the control panel\n\n"
#if !defined(_WIN32)
" -i run in inetd mode (UNIX only)\n\n"
#endif
" -D log debugging messages\n\n"
" -s <config_file> save the current configuration to file\n\n"
" -f <config_file> load the current configuration from file; all switches\n"
" specified from the command line are ignored\n\n"
" -h print this help screen\n\n";

(void)fprintf(stderr, "RPCAPD, a remote packet capture daemon.\n"
"Compiled with %s\n\n", pcap_lib_version());
printf("%s", usagetext);
}




int main(int argc, char *argv[])
{
char savefile[MAX_LINE + 1];
int log_to_systemlog = 0;
int isdaemon = 0;
#if !defined(_WIN32)
int isrunbyinetd = 0;
#endif
int log_debug_messages = 0;
int retval;
char errbuf[PCAP_ERRBUF_SIZE + 1];
#if !defined(_WIN32)
struct sigaction action;
#endif

savefile[0] = 0;
loadfile[0] = 0;
hostlist[0] = 0;


memset(errbuf, 0, sizeof(errbuf));

strncpy(address, RPCAP_DEFAULT_NETADDR, MAX_LINE);
strncpy(port, RPCAP_DEFAULT_NETPORT, MAX_LINE);


memset(&mainhints, 0, sizeof(struct addrinfo));

mainhints.ai_family = PF_UNSPEC;
mainhints.ai_flags = AI_PASSIVE;
mainhints.ai_socktype = SOCK_STREAM;


while ((retval = getopt(argc, argv, "b:dDhip:4l:na:s:f:v")) != -1)
{
switch (retval)
{
case 'D':
log_debug_messages = 1;
rpcapd_log_set(log_to_systemlog, log_debug_messages);
break;
case 'b':
strncpy(address, optarg, MAX_LINE);
break;
case 'p':
strncpy(port, optarg, MAX_LINE);
break;
case '4':
mainhints.ai_family = PF_INET;
break;
case 'd':
isdaemon = 1;
log_to_systemlog = 1;
rpcapd_log_set(log_to_systemlog, log_debug_messages);
break;
case 'i':
#if defined(_WIN32)
printusage();
exit(1);
#else
isrunbyinetd = 1;
log_to_systemlog = 1;
rpcapd_log_set(log_to_systemlog, log_debug_messages);
#endif
break;
case 'n':
nullAuthAllowed = 1;
break;
case 'v':
passivemode = 0;
break;
case 'l':
{
strncpy(hostlist, optarg, sizeof(hostlist));
break;
}
case 'a':
{
char *tmpaddress, *tmpport;
char *lasts;
int i = 0;

tmpaddress = pcap_strtok_r(optarg, RPCAP_HOSTLIST_SEP, &lasts);

while ((tmpaddress != NULL) && (i < MAX_ACTIVE_LIST))
{
tmpport = pcap_strtok_r(NULL, RPCAP_HOSTLIST_SEP, &lasts);

pcap_strlcpy(activelist[i].address, tmpaddress, MAX_LINE);

if ((tmpport == NULL) || (strcmp(tmpport, "DEFAULT") == 0))
pcap_strlcpy(activelist[i].port, RPCAP_DEFAULT_NETPORT_ACTIVE, MAX_LINE);
else
pcap_strlcpy(activelist[i].port, tmpport, MAX_LINE);

tmpaddress = pcap_strtok_r(NULL, RPCAP_HOSTLIST_SEP, &lasts);

i++;
}

if (i > MAX_ACTIVE_LIST)
rpcapd_log(LOGPRIO_ERROR, "Only MAX_ACTIVE_LIST active connections are currently supported.");



break;
}
case 'f':
pcap_strlcpy(loadfile, optarg, MAX_LINE);
break;
case 's':
pcap_strlcpy(savefile, optarg, MAX_LINE);
break;
case 'h':
printusage();
exit(0);

default:
exit(1);

}
}

#if !defined(_WIN32)
if (isdaemon && isrunbyinetd)
{
rpcapd_log(LOGPRIO_ERROR, "rpcapd: -d and -i can't be used together");
exit(1);
}
#endif

if (sock_init(errbuf, PCAP_ERRBUF_SIZE) == -1)
{
rpcapd_log(LOGPRIO_ERROR, "%s", errbuf);
exit(-1);
}

if (savefile[0] && fileconf_save(savefile))
rpcapd_log(LOGPRIO_DEBUG, "Error when saving the configuration to file");


if (loadfile[0])
fileconf_read();

#if defined(WIN32)




state_change_event = CreateEvent(NULL, FALSE, FALSE, NULL);
if (state_change_event == NULL)
{
sock_geterror("Can't create state change event", errbuf,
PCAP_ERRBUF_SIZE);
rpcapd_log(LOGPRIO_ERROR, "%s", errbuf);
exit(2);
}




if (!SetConsoleCtrlHandler(main_ctrl_event, TRUE))
{
sock_geterror("Can't set control handler", errbuf,
PCAP_ERRBUF_SIZE);
rpcapd_log(LOGPRIO_ERROR, "%s", errbuf);
exit(2);
}
#else
memset(&action, 0, sizeof (action));
action.sa_handler = main_terminate;
action.sa_flags = 0;
sigemptyset(&action.sa_mask);
sigaction(SIGTERM, &action, NULL);
memset(&action, 0, sizeof (action));
action.sa_handler = main_reap_children;
action.sa_flags = 0;
sigemptyset(&action.sa_mask);
sigaction(SIGCHLD, &action, NULL);


signal(SIGPIPE, SIG_IGN);
#endif

#if !defined(_WIN32)
if (isrunbyinetd)
{











int sockctrl;
int devnull_fd;




sockctrl = dup(0);
if (sockctrl == -1)
{
sock_geterror("Can't dup standard input", errbuf,
PCAP_ERRBUF_SIZE);
rpcapd_log(LOGPRIO_ERROR, "%s", errbuf);
exit(2);
}





devnull_fd = open("/dev/null", O_RDWR);
if (devnull_fd != -1)
{



(void)dup2(devnull_fd, 0);
(void)dup2(devnull_fd, 1);
(void)dup2(devnull_fd, 2);
close(devnull_fd);
}






char *hostlist_copy = strdup(hostlist);
if (hostlist_copy == NULL)
{
rpcapd_log(LOGPRIO_ERROR, "Out of memory copying the host/port list");
exit(0);
}
(void)daemon_serviceloop(sockctrl, 0, hostlist_copy,
nullAuthAllowed);




exit(0);
}
#endif

if (isdaemon)
{





#if !defined(_WIN32)
int pid;






if ((pid = fork()) != 0)
exit(0);



setsid();


memset(&action, 0, sizeof (action));
action.sa_handler = main_reread_config;
action.sa_flags = 0;
sigemptyset(&action.sa_mask);
sigaction(SIGHUP, &action, NULL);

if ((pid = fork()) != 0)
exit(0);









#else





if (svc_start() != 1)
rpcapd_log(LOGPRIO_DEBUG, "Unable to start the service");


exit(0);
#endif
}
else
{
#if !defined(_WIN32)

memset(&action, 0, sizeof (action));
action.sa_handler = main_terminate;
action.sa_flags = 0;
sigemptyset(&action.sa_mask);
sigaction(SIGINT, &action, NULL);



memset(&action, 0, sizeof (action));
action.sa_handler = main_reread_config;
action.sa_flags = 0;
sigemptyset(&action.sa_mask);
sigaction(SIGHUP, &action, NULL);
#endif

printf("Press CTRL + C to stop the server...\n");
}


main_startup();



exit(0);
}

void main_startup(void)
{
char errbuf[PCAP_ERRBUF_SIZE + 1];
struct addrinfo *addrinfo;
int i;
#if defined(_WIN32)
HANDLE threadId;
#else
pid_t pid;
#endif

i = 0;
addrinfo = NULL;
memset(errbuf, 0, sizeof(errbuf));


while ((i < MAX_ACTIVE_LIST) && (activelist[i].address[0] != 0))
{
activelist[i].ai_family = mainhints.ai_family;

#if defined(_WIN32)
threadId = (HANDLE)_beginthreadex(NULL, 0, main_active,
(void *)&activelist[i], 0, NULL);
if (threadId == 0)
{
rpcapd_log(LOGPRIO_DEBUG, "Error creating the active child threads");
continue;
}
CloseHandle(threadId);
#else
if ((pid = fork()) == 0)
{
main_active((void *) &activelist[i]);
exit(0);
}
#endif
i++;
}













if (passivemode)
{
struct addrinfo *tempaddrinfo;




if (sock_initaddress((address[0]) ? address : NULL, port, &mainhints, &addrinfo, errbuf, PCAP_ERRBUF_SIZE) == -1)
{
rpcapd_log(LOGPRIO_DEBUG, "%s", errbuf);
return;
}

for (tempaddrinfo = addrinfo; tempaddrinfo;
tempaddrinfo = tempaddrinfo->ai_next)
{
SOCKET sock;
struct listen_sock *sock_info;

if ((sock = sock_open(tempaddrinfo, SOCKOPEN_SERVER, SOCKET_MAXCONN, errbuf, PCAP_ERRBUF_SIZE)) == INVALID_SOCKET)
{
switch (tempaddrinfo->ai_family)
{
case AF_INET:
{
struct sockaddr_in *in;
char addrbuf[INET_ADDRSTRLEN];

in = (struct sockaddr_in *)tempaddrinfo->ai_addr;
rpcapd_log(LOGPRIO_WARNING, "Can't listen on socket for %s:%u: %s",
inet_ntop(AF_INET, &in->sin_addr,
addrbuf, sizeof (addrbuf)),
ntohs(in->sin_port),
errbuf);
break;
}

case AF_INET6:
{
struct sockaddr_in6 *in6;
char addrbuf[INET6_ADDRSTRLEN];

in6 = (struct sockaddr_in6 *)tempaddrinfo->ai_addr;
rpcapd_log(LOGPRIO_WARNING, "Can't listen on socket for %s:%u: %s",
inet_ntop(AF_INET6, &in6->sin6_addr,
addrbuf, sizeof (addrbuf)),
ntohs(in6->sin6_port),
errbuf);
break;
}

default:
rpcapd_log(LOGPRIO_WARNING, "Can't listen on socket for address family %u: %s",
tempaddrinfo->ai_family,
errbuf);
break;
}
continue;
}

sock_info = (struct listen_sock *) malloc(sizeof (struct listen_sock));
if (sock_info == NULL)
{
rpcapd_log(LOGPRIO_ERROR, "Can't allocate structure for listen socket");
exit(2);
}
sock_info->sock = sock;
sock_info->next = listen_socks;
listen_socks = sock_info;
}

freeaddrinfo(addrinfo);

if (listen_socks == NULL)
{
rpcapd_log(LOGPRIO_ERROR, "Can't listen on any address");
exit(2);
}




accept_connections();
}




rpcapd_log(LOGPRIO_DEBUG, PROGRAM_NAME " is closing.\n");

#if !defined(_WIN32)








kill(0, SIGKILL);
#endif




















exit(0);
}

#if defined(_WIN32)
static void
send_state_change_event(void)
{
char errbuf[PCAP_ERRBUF_SIZE + 1];

if (!SetEvent(state_change_event))
{
sock_geterror("SetEvent on shutdown event failed", errbuf,
PCAP_ERRBUF_SIZE);
rpcapd_log(LOGPRIO_ERROR, "%s", errbuf);
}
}

void
send_shutdown_notification(void)
{



shutdown_server = 1;




send_state_change_event();
}

void
send_reread_configuration_notification(void)
{



reread_config = 1;




send_state_change_event();
}

static BOOL WINAPI main_ctrl_event(DWORD ctrltype)
{














switch (ctrltype)
{
case CTRL_C_EVENT:
case CTRL_BREAK_EVENT:
case CTRL_CLOSE_EVENT:
case CTRL_SHUTDOWN_EVENT:



send_shutdown_notification();
break;

default:
break;
}




return TRUE;
}
#else
static void main_terminate(int sign _U_)
{





shutdown_server = 1;
}

static void main_reread_config(int sign _U_)
{





reread_config = 1;
}

static void main_reap_children(int sign _U_)
{
pid_t pid;
int exitstat;




while ((pid = waitpid(-1, &exitstat, WNOHANG)) > 0)
rpcapd_log(LOGPRIO_DEBUG, "Child terminated");

return;
}
#endif




static void
accept_connections(void)
{
#if defined(_WIN32)
struct listen_sock *sock_info;
DWORD num_events;
WSAEVENT *events;
int i;
char errbuf[PCAP_ERRBUF_SIZE + 1];






num_events = 1;
for (sock_info = listen_socks; sock_info;
sock_info = sock_info->next)
{
if (num_events == WSA_MAXIMUM_WAIT_EVENTS)
{





rpcapd_log(LOGPRIO_ERROR, "Too many sockets on which to listen");
exit(2);
}
num_events++;
}




events = (WSAEVENT *) malloc(num_events * sizeof (WSAEVENT));
if (events == NULL)
{
rpcapd_log(LOGPRIO_ERROR, "Can't allocate array of events which to listen");
exit(2);
}




events[0] = state_change_event;
for (sock_info = listen_socks, i = 1; sock_info;
sock_info = sock_info->next, i++)
{
WSAEVENT event;





event = WSACreateEvent();
if (event == WSA_INVALID_EVENT)
{
sock_geterror("Can't create socket event", errbuf,
PCAP_ERRBUF_SIZE);
rpcapd_log(LOGPRIO_ERROR, "%s", errbuf);
exit(2);
}
if (WSAEventSelect(sock_info->sock, event, FD_ACCEPT) == SOCKET_ERROR)
{
sock_geterror("Can't setup socket event", errbuf,
PCAP_ERRBUF_SIZE);
rpcapd_log(LOGPRIO_ERROR, "%s", errbuf);
exit(2);
}
events[i] = event;
}

for (;;)
{



DWORD ret;

ret = WSAWaitForMultipleEvents(num_events, events, FALSE,
WSA_INFINITE, FALSE);
if (ret == WSA_WAIT_FAILED)
{
sock_geterror("WSAWaitForMultipleEvents failed", errbuf,
PCAP_ERRBUF_SIZE);
rpcapd_log(LOGPRIO_ERROR, "%s", errbuf);
exit(2);
}

if (ret == WSA_WAIT_EVENT_0)
{



if (shutdown_server)
{



break;
}
if (reread_config)
{




reread_config = 0;
fileconf_read();
}
}




for (sock_info = listen_socks, i = 1; sock_info;
sock_info = sock_info->next, i++)
{
WSANETWORKEVENTS network_events;

if (WSAEnumNetworkEvents(sock_info->sock,
events[i], &network_events) == SOCKET_ERROR)
{
sock_geterror("WSAEnumNetworkEvents failed",
errbuf, PCAP_ERRBUF_SIZE);
rpcapd_log(LOGPRIO_ERROR, "%s", errbuf);
exit(2);
}
if (network_events.lNetworkEvents & FD_ACCEPT)
{



if (network_events.iErrorCode[FD_ACCEPT_BIT] != 0)
{



sock_fmterror("Socket error",
network_events.iErrorCode[FD_ACCEPT_BIT],
errbuf,
PCAP_ERRBUF_SIZE);
rpcapd_log(LOGPRIO_ERROR, "%s", errbuf);
continue;
}




accept_connection(sock_info->sock);
}
}
}
#else
struct listen_sock *sock_info;
int num_sock_fds;





num_sock_fds = 0;
for (sock_info = listen_socks; sock_info; sock_info = sock_info->next)
{
if (sock_info->sock + 1 > num_sock_fds)
{
if ((unsigned int)(sock_info->sock + 1) >
(unsigned int)FD_SETSIZE)
{
rpcapd_log(LOGPRIO_ERROR, "Socket FD is too bit for an fd_set");
exit(2);
}
num_sock_fds = sock_info->sock + 1;
}
}

for (;;)
{
fd_set sock_fds;
int ret;








FD_ZERO(&sock_fds);
for (sock_info = listen_socks; sock_info;
sock_info = sock_info->next)
{
FD_SET(sock_info->sock, &sock_fds);
}




ret = select(num_sock_fds, &sock_fds, NULL, NULL, NULL);
if (ret == -1)
{
if (errno == EINTR)
{





if (shutdown_server)
{



break;
}
if (reread_config)
{




reread_config = 0;
fileconf_read();
}




continue;
}
else
{
rpcapd_log(LOGPRIO_ERROR, "select failed: %s",
strerror(errno));
exit(2);
}
}




for (sock_info = listen_socks; sock_info;
sock_info = sock_info->next)
{
if (FD_ISSET(sock_info->sock, &sock_fds))
{



accept_connection(sock_info->sock);
}
}
}
#endif




for (sock_info = listen_socks; sock_info; sock_info = sock_info->next)
{
closesocket(sock_info->sock);
}
sock_cleanup();
}

#if defined(_WIN32)







struct params_copy {
SOCKET sockctrl;
char *hostlist;
};
#endif





static void
accept_connection(SOCKET listen_sock)
{
char errbuf[PCAP_ERRBUF_SIZE + 1];
SOCKET sockctrl;
struct sockaddr_storage from;
socklen_t fromlen;

#if defined(_WIN32)
HANDLE threadId;
u_long off = 0;
struct params_copy *params_copy = NULL;
#else
pid_t pid;
#endif


memset(errbuf, 0, sizeof(errbuf));

for (;;)
{

fromlen = sizeof(struct sockaddr_storage);

sockctrl = accept(listen_sock, (struct sockaddr *) &from, &fromlen);

if (sockctrl != INVALID_SOCKET)
{

break;
}




#if defined(_WIN32)
if (WSAGetLastError() == WSAEINTR)
#else
if (errno == EINTR)
#endif
continue;



sock_geterror("accept()", errbuf, PCAP_ERRBUF_SIZE);
rpcapd_log(LOGPRIO_ERROR, "Accept of control connection from client failed: %s",
errbuf);
return;
}

#if defined(_WIN32)















if (WSAEventSelect(sockctrl, NULL, 0) == SOCKET_ERROR)
{
sock_geterror("WSAEventSelect()", errbuf, PCAP_ERRBUF_SIZE);
rpcapd_log(LOGPRIO_ERROR, "%s", errbuf);
sock_close(sockctrl, NULL, 0);
return;
}
if (ioctlsocket(sockctrl, FIONBIO, &off) == SOCKET_ERROR)
{
sock_geterror("ioctlsocket(FIONBIO)", errbuf, PCAP_ERRBUF_SIZE);
rpcapd_log(LOGPRIO_ERROR, "%s", errbuf);
sock_close(sockctrl, NULL, 0);
return;
}








char *hostlist_copy = strdup(hostlist);
if (hostlist_copy == NULL)
{
rpcapd_log(LOGPRIO_ERROR, "Out of memory copying the host/port list");
sock_close(sockctrl, NULL, 0);
return;
}






params_copy = malloc(sizeof(*params_copy));
if (params_copy == NULL)
{
rpcapd_log(LOGPRIO_ERROR, "Out of memory allocating the parameter copy structure");
free(hostlist_copy);
sock_close(sockctrl, NULL, 0);
return;
}
params_copy->sockctrl = sockctrl;
params_copy->hostlist = hostlist_copy;

threadId = (HANDLE)_beginthreadex(NULL, 0,
main_passive_serviceloop_thread, (void *) params_copy, 0, NULL);
if (threadId == 0)
{
rpcapd_log(LOGPRIO_ERROR, "Error creating the child thread");
free(params_copy);
free(hostlist_copy);
sock_close(sockctrl, NULL, 0);
return;
}
CloseHandle(threadId);
#else
pid = fork();
if (pid == -1)
{
rpcapd_log(LOGPRIO_ERROR, "Error creating the child process: %s",
strerror(errno));
sock_close(sockctrl, NULL, 0);
return;
}
if (pid == 0)
{






closesocket(listen_sock);

#if 0







if (pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL))
goto end;
if (pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL))
goto end;
#endif






char *hostlist_copy = strdup(hostlist);
if (hostlist_copy == NULL)
{
rpcapd_log(LOGPRIO_ERROR, "Out of memory copying the host/port list");
exit(0);
}
(void)daemon_serviceloop(sockctrl, 0, hostlist_copy,
nullAuthAllowed);

exit(0);
}



closesocket(sockctrl);
#endif
}










#if defined(_WIN32)
static unsigned __stdcall
#else
static void *
#endif
main_active(void *ptr)
{
char errbuf[PCAP_ERRBUF_SIZE + 1];
SOCKET sockctrl;
struct addrinfo hints;
struct addrinfo *addrinfo;
struct active_pars *activepars;

activepars = (struct active_pars *) ptr;


memset(&hints, 0, sizeof(struct addrinfo));

hints.ai_family = AF_INET;
hints.ai_socktype = SOCK_STREAM;
hints.ai_family = activepars->ai_family;

rpcapd_log(LOGPRIO_DEBUG, "Connecting to host %s, port %s, using protocol %s",
activepars->address, activepars->port, (hints.ai_family == AF_INET) ? "IPv4":
(hints.ai_family == AF_INET6) ? "IPv6" : "Unspecified");


memset(errbuf, 0, sizeof(errbuf));


if (sock_initaddress(activepars->address, activepars->port, &hints, &addrinfo, errbuf, PCAP_ERRBUF_SIZE) == -1)
{
rpcapd_log(LOGPRIO_DEBUG, "%s", errbuf);
return 0;
}

for (;;)
{
int activeclose;

if ((sockctrl = sock_open(addrinfo, SOCKOPEN_CLIENT, 0, errbuf, PCAP_ERRBUF_SIZE)) == INVALID_SOCKET)
{
rpcapd_log(LOGPRIO_DEBUG, "%s", errbuf);

pcap_snprintf(errbuf, PCAP_ERRBUF_SIZE, "Error connecting to host %s, port %s, using protocol %s",
activepars->address, activepars->port, (hints.ai_family == AF_INET) ? "IPv4":
(hints.ai_family == AF_INET6) ? "IPv6" : "Unspecified");

rpcapd_log(LOGPRIO_DEBUG, "%s", errbuf);

sleep_secs(RPCAP_ACTIVE_WAIT);

continue;
}

char *hostlist_copy = strdup(hostlist);
if (hostlist_copy == NULL)
{
rpcapd_log(LOGPRIO_ERROR, "Out of memory copying the host/port list");
activeclose = 0;
sock_close(sockctrl, NULL, 0);
}
else
{



activeclose = daemon_serviceloop(sockctrl, 1,
hostlist_copy, nullAuthAllowed);
}



if (activeclose == 1)
break;
}

freeaddrinfo(addrinfo);
return 0;
}

#if defined(_WIN32)



unsigned __stdcall main_passive_serviceloop_thread(void *ptr)
{
struct params_copy params = *(struct params_copy *)ptr;
free(ptr);






(void)daemon_serviceloop(params.sockctrl, 0, params.hostlist,
nullAuthAllowed);

return 0;
}
#endif
