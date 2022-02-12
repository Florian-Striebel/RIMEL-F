#include <sys/param.h>
#include <sys/disk.h>
#include <stdlib.h>
typedef struct ddi_devid {
char devid[DISK_IDENT_SIZE];
} ddi_devid_t;
typedef struct devid_nmlist {
char devname[MAXPATHLEN];
dev_t dev;
} devid_nmlist_t;
int devid_str_decode(char *devidstr, ddi_devid_t *retdevid,
char **retminor_name);
int devid_deviceid_to_nmlist(char *search_path, ddi_devid_t devid,
char *minor_name, devid_nmlist_t **retlist);
void devid_str_free(char *str);
void devid_free(ddi_devid_t devid);
void devid_free_nmlist(devid_nmlist_t *list);
int devid_get(int fd, ddi_devid_t *retdevid);
int devid_get_minor_name(int fd, char **retminor_name);
char *devid_str_encode(ddi_devid_t devid, char *minor_name);
