
































#include "rpcapd.h"
#include <pcap.h>
#include "fmtutils.h"
#include "portability.h"
#include "fileconf.h"
#include "log.h"

static SERVICE_STATUS_HANDLE service_status_handle;
static SERVICE_STATUS service_status;

static void WINAPI svc_main(DWORD argc, char **argv);
static void update_svc_status(DWORD state, DWORD progress_indicator);

int svc_start(void)
{
int rc;
SERVICE_TABLE_ENTRY ste[] =
{
{ PROGRAM_NAME, svc_main },
{ NULL, NULL }
};
char string[PCAP_ERRBUF_SIZE];



if ((rc = StartServiceCtrlDispatcher(ste)) == 0) {
pcap_fmt_errmsg_for_win32_err(string, sizeof (string),
GetLastError(), "StartServiceCtrlDispatcher() failed");
rpcapd_log(LOGPRIO_ERROR, "%s", string);
}

return rc;
}

void WINAPI svc_control_handler(DWORD Opcode)
{
switch(Opcode)
{
case SERVICE_CONTROL_STOP:














send_shutdown_notification();

update_svc_status(SERVICE_STOP_PENDING, 0);
break;











case SERVICE_CONTROL_PAUSE:
update_svc_status(SERVICE_PAUSED, 0);
break;

case SERVICE_CONTROL_CONTINUE:
update_svc_status(SERVICE_RUNNING, 0);



send_reread_configuration_notification();
break;

case SERVICE_CONTROL_INTERROGATE:


update_svc_status(SERVICE_RUNNING, 0);
MessageBox(NULL, "Not implemented", "warning", MB_OK);
break;

case SERVICE_CONTROL_PARAMCHANGE:



send_reread_configuration_notification();
break;
}


return;
}

void WINAPI svc_main(DWORD argc, char **argv)
{
service_status_handle = RegisterServiceCtrlHandler(PROGRAM_NAME, svc_control_handler);

if (!service_status_handle)
return;

service_status.dwServiceType = SERVICE_WIN32_OWN_PROCESS | SERVICE_INTERACTIVE_PROCESS;
service_status.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_PAUSE_CONTINUE | SERVICE_ACCEPT_PARAMCHANGE;

update_svc_status(SERVICE_RUNNING, 0);




main_startup();




update_svc_status(SERVICE_STOPPED, 0);
}

static void
update_svc_status(DWORD state, DWORD progress_indicator)
{
service_status.dwWin32ExitCode = NO_ERROR;
service_status.dwCurrentState = state;
service_status.dwCheckPoint = progress_indicator;
service_status.dwWaitHint = 0;
SetServiceStatus(service_status_handle, &service_status);
}





