































#if !defined(__RPCAPD_H__)
#define __RPCAPD_H__

#define PROGRAM_NAME "rpcapd"
#define SOCKET_MAXCONN 10
#if defined(_WIN32)
void send_shutdown_notification(void);
void send_reread_configuration_notification(void);
#endif
void main_startup(void);

#endif
