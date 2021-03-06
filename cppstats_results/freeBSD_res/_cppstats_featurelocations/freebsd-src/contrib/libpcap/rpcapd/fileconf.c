
































#if defined(HAVE_CONFIG_H)
#include <config.h>
#endif

#include "ftmacros.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <pcap.h>

#include "portability.h"
#include "rpcapd.h"
#include "config_params.h"
#include "fileconf.h"
#include "rpcap-protocol.h"
#include "log.h"




#define PARAM_ACTIVECLIENT "ActiveClient"
#define PARAM_PASSIVECLIENT "PassiveClient"
#define PARAM_NULLAUTHPERMIT "NullAuthPermit"

static char *skipws(char *ptr);

void fileconf_read(void)
{
FILE *fp;
unsigned int num_active_clients;

if ((fp = fopen(loadfile, "r")) != NULL)
{
char line[MAX_LINE + 1];
unsigned int lineno;

hostlist[0] = 0;
num_active_clients = 0;
lineno = 0;

while (fgets(line, MAX_LINE, fp) != NULL)
{
size_t linelen;
char *ptr;
char *param;
size_t result;
size_t toklen;

lineno++;

linelen = strlen(line);
if (line[linelen - 1] != '\n')
{
int c;







rpcapd_log(LOGPRIO_ERROR,
"%s, line %u is longer than %u characters",
loadfile, lineno, MAX_LINE);




while ((c = getc(fp)) != '\n')
{
if (c == EOF)
goto done;
}




continue;
}
ptr = line;




ptr = skipws(ptr);
if (ptr == NULL)
{

continue;
}





if (*ptr == '#')
continue;





if (!isascii((unsigned char)*ptr) ||
!isalpha((unsigned char)*ptr))
{
rpcapd_log(LOGPRIO_ERROR,
"%s, line %u doesn't have a valid parameter name",
loadfile, lineno);
continue;
}






param = ptr;
while (isascii((unsigned char)*ptr) &&
(isalnum((unsigned char)*ptr) || *ptr == '-' || *ptr == '_'))
ptr++;




ptr = skipws(ptr);
if (ptr == NULL || *ptr != '=')
{








rpcapd_log(LOGPRIO_ERROR,
"%s, line %u has a parameter but no =",
loadfile, lineno);
continue;
}





*ptr++ = '\0';




ptr = skipws(ptr);
if (ptr == NULL)
{



rpcapd_log(LOGPRIO_ERROR,
"%s, line %u has a parameter but no value",
loadfile, lineno);
continue;
}




if (strcmp(param, PARAM_ACTIVECLIENT) == 0) {



char *address, *port;





if (num_active_clients >= MAX_ACTIVE_LIST)
{





rpcapd_log(LOGPRIO_ERROR,
"%s, line %u has an %s parameter, but we already have %u active clients",
loadfile, lineno, PARAM_ACTIVECLIENT,
MAX_ACTIVE_LIST);
continue;
}








address = ptr;
toklen = strcspn(ptr, RPCAP_HOSTLIST_SEP "#");
ptr += toklen;
if (toklen == 0)
{
if (isascii((unsigned char)*ptr) &&
(isspace((unsigned char)*ptr) || *ptr == '#' || *ptr == '\0'))
{







rpcapd_log(LOGPRIO_ERROR,
"%s, line %u has a parameter but no value",
loadfile, lineno);
}
else
{







rpcapd_log(LOGPRIO_ERROR,
"%s, line %u has an %s parameter with a value containing no address",
loadfile, lineno, PARAM_ACTIVECLIENT);
}
continue;
}





*ptr++ = '\0';





ptr = skipws(ptr);
if (ptr == NULL)
{




rpcapd_log(LOGPRIO_ERROR,
"%s, line %u has an %s parameter with a value containing no port",
loadfile, lineno, PARAM_ACTIVECLIENT);
continue;
}








port = ptr;
toklen = strcspn(ptr, " \t#\r\n");
ptr += toklen;
if (toklen == 0)
{




rpcapd_log(LOGPRIO_ERROR,
"%s, line %u has an %s parameter with a value containing no port",
loadfile, lineno, PARAM_ACTIVECLIENT);
continue;
}





*ptr++ = '\0';
result = pcap_strlcpy(activelist[num_active_clients].address, address, sizeof(activelist[num_active_clients].address));
if (result >= sizeof(activelist[num_active_clients].address))
{



rpcapd_log(LOGPRIO_ERROR,
"%s, line %u has an %s parameter with an address with more than %u characters",
loadfile, lineno, PARAM_ACTIVECLIENT,
(unsigned int)(sizeof(activelist[num_active_clients].address) - 1));
continue;
}
if (strcmp(port, "DEFAULT") == 0)
result = pcap_strlcpy(activelist[num_active_clients].port, RPCAP_DEFAULT_NETPORT_ACTIVE, sizeof(activelist[num_active_clients].port));
else
result = pcap_strlcpy(activelist[num_active_clients].port, port, sizeof(activelist[num_active_clients].port));
if (result >= sizeof(activelist[num_active_clients].address))
{



rpcapd_log(LOGPRIO_ERROR,
"%s, line %u has an %s parameter with an port with more than %u characters",
loadfile, lineno, PARAM_ACTIVECLIENT,
(unsigned int)(sizeof(activelist[num_active_clients].port) - 1));
continue;
}

num_active_clients++;
}
else if (strcmp(param, PARAM_PASSIVECLIENT) == 0)
{
char *eos;
char *host;








host = ptr;
toklen = strcspn(ptr, " \t#\r\n");
if (toklen == 0)
{







rpcapd_log(LOGPRIO_ERROR,
"%s, line %u has a parameter but no value",
loadfile, lineno);
continue;
}
ptr += toklen;
*ptr++ = '\0';








eos = hostlist + strlen(hostlist);
if (eos != hostlist)
{




result = pcap_strlcat(hostlist, ",", sizeof(hostlist));
if (result >= sizeof(hostlist))
{






*eos = '\0';
rpcapd_log(LOGPRIO_ERROR,
"%s, line %u has a %s parameter with a host name that doesn't fit",
loadfile, lineno, PARAM_PASSIVECLIENT);
continue;
}
}
result = pcap_strlcat(hostlist, host, sizeof(hostlist));
if (result >= sizeof(hostlist))
{




*eos = '\0';
rpcapd_log(LOGPRIO_ERROR,
"%s, line %u has a %s parameter with a host name that doesn't fit",
loadfile, lineno, PARAM_PASSIVECLIENT);
continue;
}
}
else if (strcmp(param, PARAM_NULLAUTHPERMIT) == 0)
{
char *setting;








setting = ptr;
toklen = strcspn(ptr, " \t#\r\n");
ptr += toklen;
if (toklen == 0)
{







rpcapd_log(LOGPRIO_ERROR,
"%s, line %u has a parameter but no value",
loadfile, lineno);
continue;
}
*ptr++ = '\0';





if (strcmp(setting, "YES") == 0)
nullAuthAllowed = 1;
else
nullAuthAllowed = 0;
}
else
{
rpcapd_log(LOGPRIO_ERROR,
"%s, line %u has an unknown parameter %s",
loadfile, lineno, param);
continue;
}
}

done:

for (int i = num_active_clients; i < MAX_ACTIVE_LIST; i++)
{
activelist[i].address[0] = 0;
activelist[i].port[0] = 0;
num_active_clients++;
}

rpcapd_log(LOGPRIO_DEBUG, "New passive host list: %s", hostlist);
fclose(fp);
}
}

int fileconf_save(const char *savefile)
{
FILE *fp;

if ((fp = fopen(savefile, "w")) != NULL)
{
char *token;
char temphostlist[MAX_HOST_LIST + 1];
int i = 0;
char *lasts;

fprintf(fp, "#Configuration file help.\n\n");


fprintf(fp, "#Hosts which are allowed to connect to this server (passive mode)\n");
fprintf(fp, "#Format: PassiveClient = <name or address>\n\n");

strncpy(temphostlist, hostlist, MAX_HOST_LIST);
temphostlist[MAX_HOST_LIST] = 0;

token = pcap_strtok_r(temphostlist, RPCAP_HOSTLIST_SEP, &lasts);
while(token != NULL)
{
fprintf(fp, "%s = %s\n", PARAM_PASSIVECLIENT, token);
token = pcap_strtok_r(NULL, RPCAP_HOSTLIST_SEP, &lasts);
}



fprintf(fp, "\n\n");
fprintf(fp, "#Hosts to which this server is trying to connect to (active mode)\n");
fprintf(fp, "#Format: ActiveClient = <name or address>, <port | DEFAULT>\n\n");


while ((i < MAX_ACTIVE_LIST) && (activelist[i].address[0] != 0))
{
fprintf(fp, "%s = %s, %s\n", PARAM_ACTIVECLIENT,
activelist[i].address, activelist[i].port);
i++;
}


fprintf(fp, "\n\n");
fprintf(fp, "#Permit NULL authentication: YES or NO\n\n");

fprintf(fp, "%s = %s\n", PARAM_NULLAUTHPERMIT,
nullAuthAllowed ? "YES" : "NO");

fclose(fp);
return 0;
}
else
{
return -1;
}

}








static char *skipws(char *ptr)
{
while (isascii((unsigned char)*ptr) && isspace((unsigned char)*ptr)) {
if (*ptr == '\r' || *ptr == '\n')
return NULL;
*ptr++ = '\0';
}
return ptr;
}
