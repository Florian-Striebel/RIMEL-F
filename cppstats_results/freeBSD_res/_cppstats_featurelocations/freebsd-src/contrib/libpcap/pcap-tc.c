






























#if defined(HAVE_CONFIG_H)
#include <config.h>
#endif

#include <pcap.h>
#include <pcap-int.h>

#include "pcap-tc.h"

#include <malloc.h>
#include <memory.h>
#include <string.h>
#include <errno.h>

#if defined(_WIN32)
#include <tchar.h>
#endif

typedef TC_STATUS (TC_CALLCONV *TcFcnQueryPortList) (PTC_PORT *ppPorts, PULONG pLength);
typedef TC_STATUS (TC_CALLCONV *TcFcnFreePortList) (TC_PORT *pPorts);

typedef PCHAR (TC_CALLCONV *TcFcnStatusGetString) (TC_STATUS status);

typedef PCHAR (TC_CALLCONV *TcFcnPortGetName) (TC_PORT port);
typedef PCHAR (TC_CALLCONV *TcFcnPortGetDescription) (TC_PORT port);

typedef TC_STATUS (TC_CALLCONV *TcFcnInstanceOpenByName) (PCHAR name, PTC_INSTANCE pInstance);
typedef TC_STATUS (TC_CALLCONV *TcFcnInstanceClose) (TC_INSTANCE instance);
typedef TC_STATUS (TC_CALLCONV *TcFcnInstanceSetFeature) (TC_INSTANCE instance, ULONG feature, ULONG value);
typedef TC_STATUS (TC_CALLCONV *TcFcnInstanceQueryFeature) (TC_INSTANCE instance, ULONG feature, PULONG pValue);
typedef TC_STATUS (TC_CALLCONV *TcFcnInstanceReceivePackets) (TC_INSTANCE instance, PTC_PACKETS_BUFFER pBuffer);
typedef HANDLE (TC_CALLCONV *TcFcnInstanceGetReceiveWaitHandle) (TC_INSTANCE instance);
typedef TC_STATUS (TC_CALLCONV *TcFcnInstanceTransmitPackets) (TC_INSTANCE instance, TC_PACKETS_BUFFER pBuffer);
typedef TC_STATUS (TC_CALLCONV *TcFcnInstanceQueryStatistics) (TC_INSTANCE instance, PTC_STATISTICS pStatistics);

typedef TC_STATUS (TC_CALLCONV *TcFcnPacketsBufferCreate) (ULONG size, PTC_PACKETS_BUFFER pBuffer);
typedef VOID (TC_CALLCONV *TcFcnPacketsBufferDestroy) (TC_PACKETS_BUFFER buffer);
typedef TC_STATUS (TC_CALLCONV *TcFcnPacketsBufferQueryNextPacket)(TC_PACKETS_BUFFER buffer, PTC_PACKET_HEADER pHeader, PVOID *ppData);
typedef TC_STATUS (TC_CALLCONV *TcFcnPacketsBufferCommitNextPacket)(TC_PACKETS_BUFFER buffer, PTC_PACKET_HEADER pHeader, PVOID pData);

typedef VOID (TC_CALLCONV *TcFcnStatisticsDestroy) (TC_STATISTICS statistics);
typedef TC_STATUS (TC_CALLCONV *TcFcnStatisticsUpdate) (TC_STATISTICS statistics);
typedef TC_STATUS (TC_CALLCONV *TcFcnStatisticsQueryValue) (TC_STATISTICS statistics, ULONG counterId, PULONGLONG pValue);

typedef enum LONG
{
TC_API_UNLOADED = 0,
TC_API_LOADED,
TC_API_CANNOT_LOAD,
TC_API_LOADING
}
TC_API_LOAD_STATUS;


typedef struct _TC_FUNCTIONS
{
TC_API_LOAD_STATUS LoadStatus;
#if defined(_WIN32)
HMODULE hTcApiDllHandle;
#endif
TcFcnQueryPortList QueryPortList;
TcFcnFreePortList FreePortList;
TcFcnStatusGetString StatusGetString;

TcFcnPortGetName PortGetName;
TcFcnPortGetDescription PortGetDescription;

TcFcnInstanceOpenByName InstanceOpenByName;
TcFcnInstanceClose InstanceClose;
TcFcnInstanceSetFeature InstanceSetFeature;
TcFcnInstanceQueryFeature InstanceQueryFeature;
TcFcnInstanceReceivePackets InstanceReceivePackets;
#if defined(_WIN32)
TcFcnInstanceGetReceiveWaitHandle InstanceGetReceiveWaitHandle;
#endif
TcFcnInstanceTransmitPackets InstanceTransmitPackets;
TcFcnInstanceQueryStatistics InstanceQueryStatistics;

TcFcnPacketsBufferCreate PacketsBufferCreate;
TcFcnPacketsBufferDestroy PacketsBufferDestroy;
TcFcnPacketsBufferQueryNextPacket PacketsBufferQueryNextPacket;
TcFcnPacketsBufferCommitNextPacket PacketsBufferCommitNextPacket;

TcFcnStatisticsDestroy StatisticsDestroy;
TcFcnStatisticsUpdate StatisticsUpdate;
TcFcnStatisticsQueryValue StatisticsQueryValue;
}
TC_FUNCTIONS;

static pcap_if_t* TcCreatePcapIfFromPort(TC_PORT port);
static int TcSetDatalink(pcap_t *p, int dlt);
static int TcGetNonBlock(pcap_t *p);
static int TcSetNonBlock(pcap_t *p, int nonblock);
static void TcCleanup(pcap_t *p);
static int TcInject(pcap_t *p, const void *buf, int size);
static int TcRead(pcap_t *p, int cnt, pcap_handler callback, u_char *user);
static int TcStats(pcap_t *p, struct pcap_stat *ps);
static int TcSetFilter(pcap_t *p, struct bpf_program *fp);
#if defined(_WIN32)
static struct pcap_stat *TcStatsEx(pcap_t *p, int *pcap_stat_size);
static int TcSetBuff(pcap_t *p, int dim);
static int TcSetMode(pcap_t *p, int mode);
static int TcSetMinToCopy(pcap_t *p, int size);
static HANDLE TcGetReceiveWaitHandle(pcap_t *p);
static int TcOidGetRequest(pcap_t *p, bpf_u_int32 oid, void *data, size_t *lenp);
static int TcOidSetRequest(pcap_t *p, bpf_u_int32 oid, const void *data, size_t *lenp);
static u_int TcSendqueueTransmit(pcap_t *p, pcap_send_queue *queue, int sync);
static int TcSetUserBuffer(pcap_t *p, int size);
static int TcLiveDump(pcap_t *p, char *filename, int maxsize, int maxpacks);
static int TcLiveDumpEnded(pcap_t *p, int sync);
static PAirpcapHandle TcGetAirPcapHandle(pcap_t *p);
#endif

#if defined(_WIN32)
TC_FUNCTIONS g_TcFunctions =
{
TC_API_UNLOADED,
NULL,
NULL,
NULL,
NULL,
NULL,
NULL,
NULL,
NULL,
NULL,
NULL,
NULL,
NULL,
NULL,
NULL,
NULL,
NULL,
NULL,
NULL,
NULL,
NULL,
NULL
};
#else
TC_FUNCTIONS g_TcFunctions =
{
TC_API_LOADED,
TcQueryPortList,
TcFreePortList,
TcStatusGetString,
TcPortGetName,
TcPortGetDescription,
TcInstanceOpenByName,
TcInstanceClose,
TcInstanceSetFeature,
TcInstanceQueryFeature,
TcInstanceReceivePackets,
#if defined(_WIN32)
TcInstanceGetReceiveWaitHandle,
#endif
TcInstanceTransmitPackets,
TcInstanceQueryStatistics,
TcPacketsBufferCreate,
TcPacketsBufferDestroy,
TcPacketsBufferQueryNextPacket,
TcPacketsBufferCommitNextPacket,
TcStatisticsDestroy,
TcStatisticsUpdate,
TcStatisticsQueryValue,
};
#endif

#define MAX_TC_PACKET_SIZE 9500

#pragma pack(push, 1)

#define PPH_PH_FLAG_PADDING ((UCHAR)0x01)
#define PPH_PH_VERSION ((UCHAR)0x00)

typedef struct _PPI_PACKET_HEADER
{
UCHAR PphVersion;
UCHAR PphFlags;
USHORT PphLength;
ULONG PphDlt;
}
PPI_PACKET_HEADER, *PPPI_PACKET_HEADER;

typedef struct _PPI_FIELD_HEADER
{
USHORT PfhType;
USHORT PfhLength;
}
PPI_FIELD_HEADER, *PPPI_FIELD_HEADER;


#define PPI_FIELD_TYPE_AGGREGATION_EXTENSION ((UCHAR)0x08)

typedef struct _PPI_FIELD_AGGREGATION_EXTENSION
{
ULONG InterfaceId;
}
PPI_FIELD_AGGREGATION_EXTENSION, *PPPI_FIELD_AGGREGATION_EXTENSION;


#define PPI_FIELD_TYPE_802_3_EXTENSION ((UCHAR)0x09)

#define PPI_FLD_802_3_EXT_FLAG_FCS_PRESENT ((ULONG)0x00000001)

typedef struct _PPI_FIELD_802_3_EXTENSION
{
ULONG Flags;
ULONG Errors;
}
PPI_FIELD_802_3_EXTENSION, *PPPI_FIELD_802_3_EXTENSION;

typedef struct _PPI_HEADER
{
PPI_PACKET_HEADER PacketHeader;
PPI_FIELD_HEADER AggregationFieldHeader;
PPI_FIELD_AGGREGATION_EXTENSION AggregationField;
PPI_FIELD_HEADER Dot3FieldHeader;
PPI_FIELD_802_3_EXTENSION Dot3Field;
}
PPI_HEADER, *PPPI_HEADER;
#pragma pack(pop)

#if defined(_WIN32)







HMODULE LoadLibrarySafe(LPCTSTR lpFileName)
{
TCHAR path[MAX_PATH];
TCHAR fullFileName[MAX_PATH];
UINT res;
HMODULE hModule = NULL;
do
{
res = GetSystemDirectory(path, MAX_PATH);

if (res == 0)
{



break;
}

if (res > MAX_PATH)
{



SetLastError(ERROR_INSUFFICIENT_BUFFER);
break;
}

if (res + 1 + _tcslen(lpFileName) + 1 < MAX_PATH)
{
memcpy(fullFileName, path, res * sizeof(TCHAR));
fullFileName[res] = _T('\\');
memcpy(&fullFileName[res + 1], lpFileName, (_tcslen(lpFileName) + 1) * sizeof(TCHAR));

hModule = LoadLibrary(fullFileName);
}
else
{
SetLastError(ERROR_INSUFFICIENT_BUFFER);
}

}while(FALSE);

return hModule;
}







TC_API_LOAD_STATUS LoadTcFunctions(void)
{
TC_API_LOAD_STATUS currentStatus;

do
{
currentStatus = InterlockedCompareExchange((LONG*)&g_TcFunctions.LoadStatus, TC_API_LOADING, TC_API_UNLOADED);

while(currentStatus == TC_API_LOADING)
{
currentStatus = InterlockedCompareExchange((LONG*)&g_TcFunctions.LoadStatus, TC_API_LOADING, TC_API_LOADING);
Sleep(10);
}





if(currentStatus == TC_API_LOADED)
{
return TC_API_LOADED;
}

if (currentStatus == TC_API_CANNOT_LOAD)
{
return TC_API_CANNOT_LOAD;
}

currentStatus = TC_API_CANNOT_LOAD;

g_TcFunctions.hTcApiDllHandle = LoadLibrarySafe("TcApi.dll");
if (g_TcFunctions.hTcApiDllHandle == NULL) break;

g_TcFunctions.QueryPortList = (TcFcnQueryPortList) GetProcAddress(g_TcFunctions.hTcApiDllHandle, "TcQueryPortList");
g_TcFunctions.FreePortList = (TcFcnFreePortList) GetProcAddress(g_TcFunctions.hTcApiDllHandle, "TcFreePortList");

g_TcFunctions.StatusGetString = (TcFcnStatusGetString) GetProcAddress(g_TcFunctions.hTcApiDllHandle, "TcStatusGetString");

g_TcFunctions.PortGetName = (TcFcnPortGetName) GetProcAddress(g_TcFunctions.hTcApiDllHandle, "TcPortGetName");
g_TcFunctions.PortGetDescription = (TcFcnPortGetDescription) GetProcAddress(g_TcFunctions.hTcApiDllHandle, "TcPortGetDescription");

g_TcFunctions.InstanceOpenByName = (TcFcnInstanceOpenByName) GetProcAddress(g_TcFunctions.hTcApiDllHandle, "TcInstanceOpenByName");
g_TcFunctions.InstanceClose = (TcFcnInstanceClose) GetProcAddress(g_TcFunctions.hTcApiDllHandle, "TcInstanceClose");
g_TcFunctions.InstanceSetFeature = (TcFcnInstanceSetFeature) GetProcAddress(g_TcFunctions.hTcApiDllHandle, "TcInstanceSetFeature");
g_TcFunctions.InstanceQueryFeature = (TcFcnInstanceQueryFeature) GetProcAddress(g_TcFunctions.hTcApiDllHandle, "TcInstanceQueryFeature");
g_TcFunctions.InstanceReceivePackets = (TcFcnInstanceReceivePackets) GetProcAddress(g_TcFunctions.hTcApiDllHandle, "TcInstanceReceivePackets");
g_TcFunctions.InstanceGetReceiveWaitHandle = (TcFcnInstanceGetReceiveWaitHandle)GetProcAddress(g_TcFunctions.hTcApiDllHandle, "TcInstanceGetReceiveWaitHandle");
g_TcFunctions.InstanceTransmitPackets = (TcFcnInstanceTransmitPackets)GetProcAddress(g_TcFunctions.hTcApiDllHandle, "TcInstanceTransmitPackets");
g_TcFunctions.InstanceQueryStatistics = (TcFcnInstanceQueryStatistics)GetProcAddress(g_TcFunctions.hTcApiDllHandle, "TcInstanceQueryStatistics");

g_TcFunctions.PacketsBufferCreate = (TcFcnPacketsBufferCreate) GetProcAddress(g_TcFunctions.hTcApiDllHandle, "TcPacketsBufferCreate");
g_TcFunctions.PacketsBufferDestroy = (TcFcnPacketsBufferDestroy) GetProcAddress(g_TcFunctions.hTcApiDllHandle, "TcPacketsBufferDestroy");
g_TcFunctions.PacketsBufferQueryNextPacket = (TcFcnPacketsBufferQueryNextPacket)GetProcAddress(g_TcFunctions.hTcApiDllHandle, "TcPacketsBufferQueryNextPacket");
g_TcFunctions.PacketsBufferCommitNextPacket = (TcFcnPacketsBufferCommitNextPacket)GetProcAddress(g_TcFunctions.hTcApiDllHandle, "TcPacketsBufferCommitNextPacket");

g_TcFunctions.StatisticsDestroy = (TcFcnStatisticsDestroy) GetProcAddress(g_TcFunctions.hTcApiDllHandle, "TcStatisticsDestroy");
g_TcFunctions.StatisticsUpdate = (TcFcnStatisticsUpdate) GetProcAddress(g_TcFunctions.hTcApiDllHandle, "TcStatisticsUpdate");
g_TcFunctions.StatisticsQueryValue = (TcFcnStatisticsQueryValue) GetProcAddress(g_TcFunctions.hTcApiDllHandle, "TcStatisticsQueryValue");

if ( g_TcFunctions.QueryPortList == NULL
|| g_TcFunctions.FreePortList == NULL
|| g_TcFunctions.StatusGetString == NULL
|| g_TcFunctions.PortGetName == NULL
|| g_TcFunctions.PortGetDescription == NULL
|| g_TcFunctions.InstanceOpenByName == NULL
|| g_TcFunctions.InstanceClose == NULL
|| g_TcFunctions.InstanceSetFeature == NULL
|| g_TcFunctions.InstanceQueryFeature == NULL
|| g_TcFunctions.InstanceReceivePackets == NULL
|| g_TcFunctions.InstanceGetReceiveWaitHandle == NULL
|| g_TcFunctions.InstanceTransmitPackets == NULL
|| g_TcFunctions.InstanceQueryStatistics == NULL
|| g_TcFunctions.PacketsBufferCreate == NULL
|| g_TcFunctions.PacketsBufferDestroy == NULL
|| g_TcFunctions.PacketsBufferQueryNextPacket == NULL
|| g_TcFunctions.PacketsBufferCommitNextPacket == NULL
|| g_TcFunctions.StatisticsDestroy == NULL
|| g_TcFunctions.StatisticsUpdate == NULL
|| g_TcFunctions.StatisticsQueryValue == NULL
)
{
break;
}




currentStatus = TC_API_LOADED;
}while(FALSE);

if (currentStatus != TC_API_LOADED)
{
if (g_TcFunctions.hTcApiDllHandle != NULL)
{
FreeLibrary(g_TcFunctions.hTcApiDllHandle);
g_TcFunctions.hTcApiDllHandle = NULL;
}
}

InterlockedExchange((LONG*)&g_TcFunctions.LoadStatus, currentStatus);

return currentStatus;
}
#else

TC_API_LOAD_STATUS LoadTcFunctions(void)
{
return TC_API_LOADED;
}
#endif




struct pcap_tc {
TC_INSTANCE TcInstance;
TC_PACKETS_BUFFER TcPacketsBuffer;
ULONG TcAcceptedCount;
u_char *PpiPacket;
};

int
TcFindAllDevs(pcap_if_list_t *devlist, char *errbuf)
{
TC_API_LOAD_STATUS loadStatus;
ULONG numPorts;
PTC_PORT pPorts = NULL;
TC_STATUS status;
int result = 0;
pcap_if_t *dev;
ULONG i;

do
{
loadStatus = LoadTcFunctions();

if (loadStatus != TC_API_LOADED)
{
result = 0;
break;
}




status = g_TcFunctions.QueryPortList(&pPorts, &numPorts);

if (status != TC_SUCCESS)
{
result = 0;
break;
}

for (i = 0; i < numPorts; i++)
{



dev = TcCreatePcapIfFromPort(pPorts[i]);

if (dev != NULL)
add_dev(devlist, dev->name, dev->flags, dev->description, errbuf);
}

if (numPorts > 0)
{



status = g_TcFunctions.FreePortList(pPorts);
}

}while(FALSE);

return result;
}

static pcap_if_t* TcCreatePcapIfFromPort(TC_PORT port)
{
CHAR *name;
CHAR *description;
pcap_if_t *newIf = NULL;

newIf = (pcap_if_t*)malloc(sizeof(*newIf));
if (newIf == NULL)
{
return NULL;
}

memset(newIf, 0, sizeof(*newIf));

name = g_TcFunctions.PortGetName(port);
description = g_TcFunctions.PortGetDescription(port);

newIf->name = (char*)malloc(strlen(name) + 1);
if (newIf->name == NULL)
{
free(newIf);
return NULL;
}

newIf->description = (char*)malloc(strlen(description) + 1);
if (newIf->description == NULL)
{
free(newIf->name);
free(newIf);
return NULL;
}

strcpy(newIf->name, name);
strcpy(newIf->description, description);

newIf->addresses = NULL;
newIf->next = NULL;
newIf->flags = 0;

return newIf;

}

static int
TcActivate(pcap_t *p)
{
struct pcap_tc *pt = p->priv;
TC_STATUS status;
ULONG timeout;
PPPI_HEADER pPpiHeader;

if (p->opt.rfmon)
{




return PCAP_ERROR_RFMON_NOTSUP;
}

pt->PpiPacket = malloc(sizeof(PPI_HEADER) + MAX_TC_PACKET_SIZE);

if (pt->PpiPacket == NULL)
{
pcap_snprintf(p->errbuf, PCAP_ERRBUF_SIZE, "Error allocating memory");
return PCAP_ERROR;
}









if (p->snapshot <= 0 || p->snapshot > MAXIMUM_SNAPLEN)
p->snapshot = MAXIMUM_SNAPLEN;




pPpiHeader = (PPPI_HEADER)pt->PpiPacket;
pPpiHeader->PacketHeader.PphDlt = DLT_EN10MB;
pPpiHeader->PacketHeader.PphLength = sizeof(PPI_HEADER);
pPpiHeader->PacketHeader.PphFlags = 0;
pPpiHeader->PacketHeader.PphVersion = 0;

pPpiHeader->AggregationFieldHeader.PfhLength = sizeof(PPI_FIELD_AGGREGATION_EXTENSION);
pPpiHeader->AggregationFieldHeader.PfhType = PPI_FIELD_TYPE_AGGREGATION_EXTENSION;

pPpiHeader->Dot3FieldHeader.PfhLength = sizeof(PPI_FIELD_802_3_EXTENSION);
pPpiHeader->Dot3FieldHeader.PfhType = PPI_FIELD_TYPE_802_3_EXTENSION;

status = g_TcFunctions.InstanceOpenByName(p->opt.device, &pt->TcInstance);

if (status != TC_SUCCESS)
{

pcap_snprintf(p->errbuf, PCAP_ERRBUF_SIZE, "Error opening TurboCap adapter: %s", g_TcFunctions.StatusGetString(status));
return PCAP_ERROR;
}

p->linktype = DLT_EN10MB;
p->dlt_list = (u_int *) malloc(sizeof(u_int) * 2);



if (p->dlt_list != NULL) {
p->dlt_list[0] = DLT_EN10MB;
p->dlt_list[1] = DLT_PPI;
p->dlt_count = 2;
}














status = g_TcFunctions.InstanceSetFeature(pt->TcInstance, TC_INST_FT_RX_STATUS, 1);

if (status != TC_SUCCESS)
{
pcap_snprintf(p->errbuf, PCAP_ERRBUF_SIZE,"Error enabling reception on a TurboCap instance: %s", g_TcFunctions.StatusGetString(status));
goto bad;
}




status = g_TcFunctions.InstanceSetFeature(pt->TcInstance, TC_INST_FT_TX_STATUS, 1);




p->inject_op = TcInject;





if (p->opt.timeout == 0)
{
timeout = 0xFFFFFFFF;
}
else
if (p->opt.timeout < 0)
{



timeout = 10;
}
else
{
timeout = p->opt.timeout;
}

status = g_TcFunctions.InstanceSetFeature(pt->TcInstance, TC_INST_FT_READ_TIMEOUT, timeout);

if (status != TC_SUCCESS)
{
pcap_snprintf(p->errbuf, PCAP_ERRBUF_SIZE,"Error setting the read timeout a TurboCap instance: %s", g_TcFunctions.StatusGetString(status));
goto bad;
}

p->read_op = TcRead;
p->setfilter_op = TcSetFilter;
p->setdirection_op = NULL;
p->set_datalink_op = TcSetDatalink;
p->getnonblock_op = TcGetNonBlock;
p->setnonblock_op = TcSetNonBlock;
p->stats_op = TcStats;
#if defined(_WIN32)
p->stats_ex_op = TcStatsEx;
p->setbuff_op = TcSetBuff;
p->setmode_op = TcSetMode;
p->setmintocopy_op = TcSetMinToCopy;
p->getevent_op = TcGetReceiveWaitHandle;
p->oid_get_request_op = TcOidGetRequest;
p->oid_set_request_op = TcOidSetRequest;
p->sendqueue_transmit_op = TcSendqueueTransmit;
p->setuserbuffer_op = TcSetUserBuffer;
p->live_dump_op = TcLiveDump;
p->live_dump_ended_op = TcLiveDumpEnded;
p->get_airpcap_handle_op = TcGetAirPcapHandle;
#else
p->selectable_fd = -1;
#endif

p->cleanup_op = TcCleanup;

return 0;
bad:
TcCleanup(p);
return PCAP_ERROR;
}

pcap_t *
TcCreate(const char *device, char *ebuf, int *is_ours)
{
ULONG numPorts;
PTC_PORT pPorts = NULL;
TC_STATUS status;
int is_tc;
ULONG i;
pcap_t *p;

if (LoadTcFunctions() != TC_API_LOADED)
{




*is_ours = 0;
return NULL;
}




status = g_TcFunctions.QueryPortList(&pPorts, &numPorts);

if (status != TC_SUCCESS)
{




*is_ours = 0;
return NULL;
}

is_tc = FALSE;
for (i = 0; i < numPorts; i++)
{
if (strcmp(g_TcFunctions.PortGetName(pPorts[i]), device) == 0)
{
is_tc = TRUE;
break;
}
}

if (numPorts > 0)
{



(void)g_TcFunctions.FreePortList(pPorts);
}

if (!is_tc)
{
*is_ours = 0;
return NULL;
}


*is_ours = 1;

p = pcap_create_common(ebuf, sizeof (struct pcap_tc));
if (p == NULL)
return NULL;

p->activate_op = TcActivate;







p->getnonblock_op = TcGetNonBlock;
p->setnonblock_op = TcSetNonBlock;
return p;
}

static int TcSetDatalink(pcap_t *p, int dlt)
{











return 0;
}

static int TcGetNonBlock(pcap_t *p)
{
pcap_snprintf(p->errbuf, PCAP_ERRBUF_SIZE,
"Non-blocking mode isn't supported for TurboCap ports");
return -1;
}

static int TcSetNonBlock(pcap_t *p, int nonblock)
{
pcap_snprintf(p->errbuf, PCAP_ERRBUF_SIZE,
"Non-blocking mode isn't supported for TurboCap ports");
return -1;
}

static void TcCleanup(pcap_t *p)
{
struct pcap_tc *pt = p->priv;

if (pt->TcPacketsBuffer != NULL)
{
g_TcFunctions.PacketsBufferDestroy(pt->TcPacketsBuffer);
pt->TcPacketsBuffer = NULL;
}
if (pt->TcInstance != NULL)
{



g_TcFunctions.InstanceClose(pt->TcInstance);
pt->TcInstance = NULL;
}

if (pt->PpiPacket != NULL)
{
free(pt->PpiPacket);
pt->PpiPacket = NULL;
}

pcap_cleanup_live_common(p);
}


static int TcInject(pcap_t *p, const void *buf, int size)
{
struct pcap_tc *pt = p->priv;
TC_STATUS status;
TC_PACKETS_BUFFER buffer;
TC_PACKET_HEADER header;

if (size >= 0xFFFF)
{
pcap_snprintf(p->errbuf, PCAP_ERRBUF_SIZE, "send error: the TurboCap API does not support packets larger than 64k");
return -1;
}

status = g_TcFunctions.PacketsBufferCreate(sizeof(TC_PACKET_HEADER) + TC_ALIGN_USHORT_TO_64BIT((USHORT)size), &buffer);

if (status != TC_SUCCESS)
{
pcap_snprintf(p->errbuf, PCAP_ERRBUF_SIZE, "send error: TcPacketsBufferCreate failure: %s (%08x)", g_TcFunctions.StatusGetString(status), status);
return -1;
}




memset(&header, 0, sizeof(header));

header.Length = (USHORT)size;
header.CapturedLength = header.Length;

status = g_TcFunctions.PacketsBufferCommitNextPacket(buffer, &header, (PVOID)buf);

if (status == TC_SUCCESS)
{
status = g_TcFunctions.InstanceTransmitPackets(pt->TcInstance, buffer);

if (status != TC_SUCCESS)
{
pcap_snprintf(p->errbuf, PCAP_ERRBUF_SIZE, "send error: TcInstanceTransmitPackets failure: %s (%08x)", g_TcFunctions.StatusGetString(status), status);
}
}
else
{
pcap_snprintf(p->errbuf, PCAP_ERRBUF_SIZE, "send error: TcPacketsBufferCommitNextPacket failure: %s (%08x)", g_TcFunctions.StatusGetString(status), status);
}

g_TcFunctions.PacketsBufferDestroy(buffer);

if (status != TC_SUCCESS)
{
return -1;
}
else
{
return 0;
}
}

static int TcRead(pcap_t *p, int cnt, pcap_handler callback, u_char *user)
{
struct pcap_tc *pt = p->priv;
TC_STATUS status;
int n = 0;




if (p->break_loop)
{





p->break_loop = 0;
return -2;
}

if (pt->TcPacketsBuffer == NULL)
{
status = g_TcFunctions.InstanceReceivePackets(pt->TcInstance, &pt->TcPacketsBuffer);
if (status != TC_SUCCESS)
{
pcap_snprintf(p->errbuf, PCAP_ERRBUF_SIZE, "read error, TcInstanceReceivePackets failure: %s (%08x)", g_TcFunctions.StatusGetString(status), status);
return -1;
}
}

while (TRUE)
{
struct pcap_pkthdr hdr;
TC_PACKET_HEADER tcHeader;
PVOID data;
ULONG filterResult;










if (p->break_loop)
{
if (n == 0)
{
p->break_loop = 0;
return -2;
}
else
{
return n;
}
}

if (pt->TcPacketsBuffer == NULL)
{
break;
}

status = g_TcFunctions.PacketsBufferQueryNextPacket(pt->TcPacketsBuffer, &tcHeader, &data);

if (status == TC_ERROR_END_OF_BUFFER)
{
g_TcFunctions.PacketsBufferDestroy(pt->TcPacketsBuffer);
pt->TcPacketsBuffer = NULL;
break;
}

if (status != TC_SUCCESS)
{
pcap_snprintf(p->errbuf, PCAP_ERRBUF_SIZE, "read error, TcPacketsBufferQueryNextPacket failure: %s (%08x)", g_TcFunctions.StatusGetString(status), status);
return -1;
}


if (p->fcode.bf_insns)
{
filterResult = bpf_filter(p->fcode.bf_insns, data, tcHeader.Length, tcHeader.CapturedLength);

if (filterResult == 0)
{
continue;
}

if (filterResult > tcHeader.CapturedLength)
{
filterResult = tcHeader.CapturedLength;
}
}
else
{
filterResult = tcHeader.CapturedLength;
}

pt->TcAcceptedCount ++;

hdr.ts.tv_sec = (bpf_u_int32)(tcHeader.Timestamp / (ULONGLONG)(1000 * 1000 * 1000));
hdr.ts.tv_usec = (bpf_u_int32)((tcHeader.Timestamp % (ULONGLONG)(1000 * 1000 * 1000)) / 1000);

if (p->linktype == DLT_EN10MB)
{
hdr.caplen = filterResult;
hdr.len = tcHeader.Length;
(*callback)(user, &hdr, data);
}
else
{
PPPI_HEADER pPpiHeader = (PPPI_HEADER)pt->PpiPacket;
PVOID data2 = pPpiHeader + 1;

pPpiHeader->AggregationField.InterfaceId = TC_PH_FLAGS_RX_PORT_ID(tcHeader.Flags);
pPpiHeader->Dot3Field.Errors = tcHeader.Errors;
if (tcHeader.Flags & TC_PH_FLAGS_CHECKSUM)
{
pPpiHeader->Dot3Field.Flags = PPI_FLD_802_3_EXT_FLAG_FCS_PRESENT;
}
else
{
pPpiHeader->Dot3Field.Flags = 0;
}

if (filterResult <= MAX_TC_PACKET_SIZE)
{
memcpy(data2, data, filterResult);
hdr.caplen = sizeof(PPI_HEADER) + filterResult;
hdr.len = sizeof(PPI_HEADER) + tcHeader.Length;
}
else
{
memcpy(data2, data, MAX_TC_PACKET_SIZE);
hdr.caplen = sizeof(PPI_HEADER) + MAX_TC_PACKET_SIZE;
hdr.len = sizeof(PPI_HEADER) + tcHeader.Length;
}

(*callback)(user, &hdr, pt->PpiPacket);

}

if (++n >= cnt && cnt > 0)
{
return n;
}
}

return n;
}

static int
TcStats(pcap_t *p, struct pcap_stat *ps)
{
struct pcap_tc *pt = p->priv;
TC_STATISTICS statistics;
TC_STATUS status;
ULONGLONG counter;
struct pcap_stat s;

status = g_TcFunctions.InstanceQueryStatistics(pt->TcInstance, &statistics);

if (status != TC_SUCCESS)
{
pcap_snprintf(p->errbuf, PCAP_ERRBUF_SIZE, "TurboCap error in TcInstanceQueryStatistics: %s (%08x)", g_TcFunctions.StatusGetString(status), status);
return -1;
}

memset(&s, 0, sizeof(s));

status = g_TcFunctions.StatisticsQueryValue(statistics, TC_COUNTER_INSTANCE_TOTAL_RX_PACKETS, &counter);
if (status != TC_SUCCESS)
{
pcap_snprintf(p->errbuf, PCAP_ERRBUF_SIZE, "TurboCap error in TcStatisticsQueryValue: %s (%08x)", g_TcFunctions.StatusGetString(status), status);
return -1;
}
if (counter <= (ULONGLONG)0xFFFFFFFF)
{
s.ps_recv = (ULONG)counter;
}
else
{
s.ps_recv = 0xFFFFFFFF;
}

status = g_TcFunctions.StatisticsQueryValue(statistics, TC_COUNTER_INSTANCE_RX_DROPPED_PACKETS, &counter);
if (status != TC_SUCCESS)
{
pcap_snprintf(p->errbuf, PCAP_ERRBUF_SIZE, "TurboCap error in TcStatisticsQueryValue: %s (%08x)", g_TcFunctions.StatusGetString(status), status);
return -1;
}
if (counter <= (ULONGLONG)0xFFFFFFFF)
{
s.ps_ifdrop = (ULONG)counter;
s.ps_drop = (ULONG)counter;
}
else
{
s.ps_ifdrop = 0xFFFFFFFF;
s.ps_drop = 0xFFFFFFFF;
}

#if defined(_WIN32) && defined(ENABLE_REMOTE)
s.ps_capt = pt->TcAcceptedCount;
#endif
*ps = s;

return 0;
}





static int
TcSetFilter(pcap_t *p, struct bpf_program *fp)
{
if(!fp)
{
strncpy(p->errbuf, "setfilter: No filter specified", sizeof(p->errbuf));
return -1;
}


if (install_bpf_program(p, fp) < 0)
{
return -1;
}

return 0;
}

#if defined(_WIN32)
static struct pcap_stat *
TcStatsEx(pcap_t *p, int *pcap_stat_size)
{
struct pcap_tc *pt = p->priv;
TC_STATISTICS statistics;
TC_STATUS status;
ULONGLONG counter;

*pcap_stat_size = sizeof (p->stat);

status = g_TcFunctions.InstanceQueryStatistics(pt->TcInstance, &statistics);

if (status != TC_SUCCESS)
{
pcap_snprintf(p->errbuf, PCAP_ERRBUF_SIZE, "TurboCap error in TcInstanceQueryStatistics: %s (%08x)", g_TcFunctions.StatusGetString(status), status);
return NULL;
}

memset(&p->stat, 0, sizeof(p->stat));

status = g_TcFunctions.StatisticsQueryValue(statistics, TC_COUNTER_INSTANCE_TOTAL_RX_PACKETS, &counter);
if (status != TC_SUCCESS)
{
pcap_snprintf(p->errbuf, PCAP_ERRBUF_SIZE, "TurboCap error in TcStatisticsQueryValue: %s (%08x)", g_TcFunctions.StatusGetString(status), status);
return NULL;
}
if (counter <= (ULONGLONG)0xFFFFFFFF)
{
p->stat.ps_recv = (ULONG)counter;
}
else
{
p->stat.ps_recv = 0xFFFFFFFF;
}

status = g_TcFunctions.StatisticsQueryValue(statistics, TC_COUNTER_INSTANCE_RX_DROPPED_PACKETS, &counter);
if (status != TC_SUCCESS)
{
pcap_snprintf(p->errbuf, PCAP_ERRBUF_SIZE, "TurboCap error in TcStatisticsQueryValue: %s (%08x)", g_TcFunctions.StatusGetString(status), status);
return NULL;
}
if (counter <= (ULONGLONG)0xFFFFFFFF)
{
p->stat.ps_ifdrop = (ULONG)counter;
p->stat.ps_drop = (ULONG)counter;
}
else
{
p->stat.ps_ifdrop = 0xFFFFFFFF;
p->stat.ps_drop = 0xFFFFFFFF;
}

#if defined(_WIN32) && defined(ENABLE_REMOTE)
p->stat.ps_capt = pt->TcAcceptedCount;
#endif

return &p->stat;
}


static int
TcSetBuff(pcap_t *p, int dim)
{





return 0;
}

static int
TcSetMode(pcap_t *p, int mode)
{
if (mode != MODE_CAPT)
{
pcap_snprintf(p->errbuf, PCAP_ERRBUF_SIZE, "Mode %u not supported by TurboCap devices. TurboCap only supports capture.", mode);
return -1;
}

return 0;
}

static int
TcSetMinToCopy(pcap_t *p, int size)
{
struct pcap_tc *pt = p->priv;
TC_STATUS status;

if (size < 0)
{
pcap_snprintf(p->errbuf, PCAP_ERRBUF_SIZE, "Mintocopy cannot be less than 0.");
return -1;
}

status = g_TcFunctions.InstanceSetFeature(pt->TcInstance, TC_INST_FT_MINTOCOPY, (ULONG)size);

if (status != TC_SUCCESS)
{
pcap_snprintf(p->errbuf, PCAP_ERRBUF_SIZE, "TurboCap error setting the mintocopy: %s (%08x)", g_TcFunctions.StatusGetString(status), status);
}

return 0;
}

static HANDLE
TcGetReceiveWaitHandle(pcap_t *p)
{
struct pcap_tc *pt = p->priv;

return g_TcFunctions.InstanceGetReceiveWaitHandle(pt->TcInstance);
}

static int
TcOidGetRequest(pcap_t *p, bpf_u_int32 oid _U_, void *data _U_, size_t *lenp _U_)
{
pcap_snprintf(p->errbuf, PCAP_ERRBUF_SIZE,
"An OID get request cannot be performed on a TurboCap device");
return PCAP_ERROR;
}

static int
TcOidSetRequest(pcap_t *p, bpf_u_int32 oid _U_, const void *data _U_,
size_t *lenp _U_)
{
pcap_snprintf(p->errbuf, PCAP_ERRBUF_SIZE,
"An OID set request cannot be performed on a TurboCap device");
return PCAP_ERROR;
}

static u_int
TcSendqueueTransmit(pcap_t *p, pcap_send_queue *queue _U_, int sync _U_)
{
pcap_snprintf(p->errbuf, PCAP_ERRBUF_SIZE,
"Packets cannot be bulk transmitted on a TurboCap device");
return 0;
}

static int
TcSetUserBuffer(pcap_t *p, int size _U_)
{
pcap_snprintf(p->errbuf, PCAP_ERRBUF_SIZE,
"The user buffer cannot be set on a TurboCap device");
return -1;
}

static int
TcLiveDump(pcap_t *p, char *filename _U_, int maxsize _U_, int maxpacks _U_)
{
pcap_snprintf(p->errbuf, PCAP_ERRBUF_SIZE,
"Live packet dumping cannot be performed on a TurboCap device");
return -1;
}

static int
TcLiveDumpEnded(pcap_t *p, int sync _U_)
{
pcap_snprintf(p->errbuf, PCAP_ERRBUF_SIZE,
"Live packet dumping cannot be performed on a TurboCap device");
return -1;
}

static PAirpcapHandle
TcGetAirPcapHandle(pcap_t *p _U_)
{
return NULL;
}
#endif
