





#if !defined(_FIDO_NETLINK_H)
#define _FIDO_NETLINK_H

#include <sys/socket.h>

#include <linux/genetlink.h>
#include <linux/netlink.h>
#include <linux/nfc.h>

#include <stdlib.h>
#include <stdint.h>

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct fido_nl {
int fd;
uint16_t nfc_type;
uint32_t nfc_mcastgrp;
struct sockaddr_nl saddr;
} fido_nl_t;

fido_nl_t *fido_nl_new(void);
void fido_nl_free(struct fido_nl **);
int fido_nl_power_nfc(struct fido_nl *, uint32_t);
int fido_nl_get_nfc_target(struct fido_nl *, uint32_t , uint32_t *);

#if defined(FIDO_FUZZ)
void set_netlink_io_functions(ssize_t (*)(int, void *, size_t),
ssize_t (*)(int, const void *, size_t));
#endif

#if defined(__cplusplus)
}
#endif

#endif
