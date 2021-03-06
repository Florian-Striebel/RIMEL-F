
































#if defined(HAVE_CONFIG_H)
#include <config.h>
#endif

#include "ftmacros.h"






#include "sockutils.h"
#include "pcap-int.h"
#include "pcap-rpcap.h"
#include "rpcap-protocol.h"
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#if !defined(_WIN32)
#include <dirent.h>
#endif


#define PCAP_TEXT_SOURCE_FILE "File"
#define PCAP_TEXT_SOURCE_FILE_LEN (sizeof PCAP_TEXT_SOURCE_FILE - 1)

#define PCAP_TEXT_SOURCE_ADAPTER "Network adapter"
#define PCAP_TEXT_SOURCE_ADAPTER_LEN (sizeof "Network adapter" - 1)


#define PCAP_TEXT_SOURCE_ON_LOCAL_HOST "on local host"
#define PCAP_TEXT_SOURCE_ON_LOCAL_HOST_LEN (sizeof PCAP_TEXT_SOURCE_ON_LOCAL_HOST + 1)







int pcap_findalldevs_ex(const char *source, struct pcap_rmtauth *auth, pcap_if_t **alldevs, char *errbuf)
{
int type;
char name[PCAP_BUF_SIZE], path[PCAP_BUF_SIZE], filename[PCAP_BUF_SIZE];
size_t pathlen;
size_t stringlen;
pcap_t *fp;
char tmpstring[PCAP_BUF_SIZE + 1];
pcap_if_t *lastdev;
pcap_if_t *dev;


(*alldevs) = NULL;
lastdev = NULL;

if (strlen(source) > PCAP_BUF_SIZE)
{
pcap_snprintf(errbuf, PCAP_ERRBUF_SIZE, "The source string is too long. Cannot handle it correctly.");
return -1;
}










if (pcap_parsesrcstr(source, &type, NULL, NULL, NULL, errbuf) == -1)
return -1;

switch (type)
{
case PCAP_SRC_IFLOCAL:
if (pcap_parsesrcstr(source, &type, NULL, NULL, NULL, errbuf) == -1)
return -1;


tmpstring[PCAP_BUF_SIZE] = 0;


if (pcap_findalldevs(alldevs, errbuf) == -1)
return -1;

if (*alldevs == NULL)
{
pcap_snprintf(errbuf, PCAP_ERRBUF_SIZE,
"No interfaces found! Make sure libpcap/Npcap is properly installed"
" on the local machine.");
return -1;
}



dev = *alldevs;
while (dev)
{
char *localdesc, *desc;


if (pcap_createsrcstr(tmpstring, PCAP_SRC_IFLOCAL, NULL, NULL, dev->name, errbuf) == -1)
return -1;


free(dev->name);


dev->name = strdup(tmpstring);
if (dev->name == NULL)
{
pcap_fmt_errmsg_for_errno(errbuf,
PCAP_ERRBUF_SIZE, errno,
"malloc() failed");
pcap_freealldevs(*alldevs);
return -1;
}




if ((dev->description == NULL) || (dev->description[0] == 0))
localdesc = dev->name;
else
localdesc = dev->description;
if (pcap_asprintf(&desc, "%s '%s' %s",
PCAP_TEXT_SOURCE_ADAPTER, localdesc,
PCAP_TEXT_SOURCE_ON_LOCAL_HOST) == -1)
{
pcap_fmt_errmsg_for_errno(errbuf,
PCAP_ERRBUF_SIZE, errno,
"malloc() failed");
pcap_freealldevs(*alldevs);
return -1;
}


free(dev->description);
dev->description = desc;

dev = dev->next;
}

return 0;

case PCAP_SRC_FILE:
{
#if defined(_WIN32)
WIN32_FIND_DATA filedata;
HANDLE filehandle;
#else
struct dirent *filedata;
DIR *unixdir;
#endif

if (pcap_parsesrcstr(source, &type, NULL, NULL, name, errbuf) == -1)
return -1;


stringlen = strlen(name);


#if defined(_WIN32)
#define ENDING_CHAR '\\'
#else
#define ENDING_CHAR '/'
#endif

if (name[stringlen - 1] != ENDING_CHAR)
{
name[stringlen] = ENDING_CHAR;
name[stringlen + 1] = 0;

stringlen++;
}


pcap_snprintf(path, sizeof(path), "%s", name);
pathlen = strlen(path);

#if defined(_WIN32)

if (name[stringlen - 1] != '*')
{
name[stringlen] = '*';
name[stringlen + 1] = 0;
}

filehandle = FindFirstFile(name, &filedata);

if (filehandle == INVALID_HANDLE_VALUE)
{
pcap_snprintf(errbuf, PCAP_ERRBUF_SIZE, "Error when listing files: does folder '%s' exist?", path);
return -1;
}

#else

unixdir= opendir(path);


filedata= readdir(unixdir);

if (filedata == NULL)
{
pcap_snprintf(errbuf, PCAP_ERRBUF_SIZE, "Error when listing files: does folder '%s' exist?", path);
return -1;
}
#endif


do
{
#if defined(_WIN32)

if (pathlen + strlen(filedata.cFileName) >= sizeof(filename))
continue;
pcap_snprintf(filename, sizeof(filename), "%s%s", path, filedata.cFileName);
#else
if (pathlen + strlen(filedata->d_name) >= sizeof(filename))
continue;
pcap_snprintf(filename, sizeof(filename), "%s%s", path, filedata->d_name);
#endif

fp = pcap_open_offline(filename, errbuf);

if (fp)
{

dev = (pcap_if_t *)malloc(sizeof(pcap_if_t));
if (dev == NULL)
{
pcap_fmt_errmsg_for_errno(errbuf,
PCAP_ERRBUF_SIZE, errno,
"malloc() failed");
pcap_freealldevs(*alldevs);
return -1;
}


memset(dev, 0, sizeof(pcap_if_t));


if (lastdev == NULL)
{




*alldevs = dev;
}
else
{



lastdev->next = dev;
}

lastdev = dev;


if (pcap_createsrcstr(tmpstring, PCAP_SRC_FILE, NULL, NULL, filename, errbuf) == -1)
{
pcap_freealldevs(*alldevs);
return -1;
}

dev->name = strdup(tmpstring);
if (dev->name == NULL)
{
pcap_fmt_errmsg_for_errno(errbuf,
PCAP_ERRBUF_SIZE, errno,
"malloc() failed");
pcap_freealldevs(*alldevs);
return -1;
}




if (pcap_asprintf(&dev->description,
"%s '%s' %s", PCAP_TEXT_SOURCE_FILE,
filename, PCAP_TEXT_SOURCE_ON_LOCAL_HOST) == -1)
{
pcap_fmt_errmsg_for_errno(errbuf,
PCAP_ERRBUF_SIZE, errno,
"malloc() failed");
pcap_freealldevs(*alldevs);
return -1;
}

pcap_close(fp);
}
}
#if defined(_WIN32)
while (FindNextFile(filehandle, &filedata) != 0);
#else
while ( (filedata= readdir(unixdir)) != NULL);
#endif


#if defined(_WIN32)

FindClose(filehandle);
#endif

return 0;
}

case PCAP_SRC_IFREMOTE:
return pcap_findalldevs_ex_remote(source, auth, alldevs, errbuf);

default:
pcap_strlcpy(errbuf, "Source type not supported", PCAP_ERRBUF_SIZE);
return -1;
}
}

pcap_t *pcap_open(const char *source, int snaplen, int flags, int read_timeout, struct pcap_rmtauth *auth, char *errbuf)
{
char name[PCAP_BUF_SIZE];
int type;
pcap_t *fp;
int status;








if (source == NULL)
source = "any";

if (strlen(source) > PCAP_BUF_SIZE)
{
pcap_snprintf(errbuf, PCAP_ERRBUF_SIZE, "The source string is too long. Cannot handle it correctly.");
return NULL;
}





if (pcap_parsesrcstr(source, &type, NULL, NULL, name, errbuf) == -1)
return NULL;

switch (type)
{
case PCAP_SRC_FILE:
return pcap_open_offline(name, errbuf);

case PCAP_SRC_IFLOCAL:
fp = pcap_create(name, errbuf);
break;

case PCAP_SRC_IFREMOTE:






return pcap_open_rpcap(source, snaplen, flags, read_timeout, auth, errbuf);

default:
pcap_strlcpy(errbuf, "Source type not supported", PCAP_ERRBUF_SIZE);
return NULL;
}

if (fp == NULL)
return (NULL);
status = pcap_set_snaplen(fp, snaplen);
if (status < 0)
goto fail;
if (flags & PCAP_OPENFLAG_PROMISCUOUS)
{
status = pcap_set_promisc(fp, 1);
if (status < 0)
goto fail;
}
if (flags & PCAP_OPENFLAG_MAX_RESPONSIVENESS)
{
status = pcap_set_immediate_mode(fp, 1);
if (status < 0)
goto fail;
}
#if defined(_WIN32)










if (flags & PCAP_OPENFLAG_NOCAPTURE_LOCAL)
fp->opt.nocapture_local = 1;
#endif
status = pcap_set_timeout(fp, read_timeout);
if (status < 0)
goto fail;
status = pcap_activate(fp);
if (status < 0)
goto fail;
return fp;

fail:
if (status == PCAP_ERROR)
pcap_snprintf(errbuf, PCAP_ERRBUF_SIZE, "%s: %s",
name, fp->errbuf);
else if (status == PCAP_ERROR_NO_SUCH_DEVICE ||
status == PCAP_ERROR_PERM_DENIED ||
status == PCAP_ERROR_PROMISC_PERM_DENIED)
pcap_snprintf(errbuf, PCAP_ERRBUF_SIZE, "%s: %s (%s)",
name, pcap_statustostr(status), fp->errbuf);
else
pcap_snprintf(errbuf, PCAP_ERRBUF_SIZE, "%s: %s",
name, pcap_statustostr(status));
pcap_close(fp);
return NULL;
}

struct pcap_samp *pcap_setsampling(pcap_t *p)
{
return &p->rmt_samp;
}
