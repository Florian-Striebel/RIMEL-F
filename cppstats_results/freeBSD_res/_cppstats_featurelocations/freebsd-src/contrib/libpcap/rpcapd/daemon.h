































#if !defined(__DAEMON_H__)
#define __DAEMON_H__






int daemon_serviceloop(SOCKET sockctrl, int isactive, char *passiveClients,
int nullAuthAllowed);

void sleep_secs(int secs);

#endif
