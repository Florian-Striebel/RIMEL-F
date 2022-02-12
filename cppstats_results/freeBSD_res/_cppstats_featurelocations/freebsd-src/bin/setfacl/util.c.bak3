

























#include <sys/cdefs.h>
__FBSDID("$FreeBSD$");

#include <err.h>
#include <stdlib.h>

#include "setfacl.h"

void *
zmalloc(size_t size)
{
void *ptr;

ptr = calloc(1, size);
if (ptr == NULL)
err(1, "calloc() failed");
return (ptr);
}

void *
zrealloc(void *ptr, size_t size)
{
void *newptr;

newptr = realloc(ptr, size);
if (newptr == NULL)
err(1, "realloc() failed");
return (newptr);
}

const char *
brand_name(int brand)
{
switch (brand) {
case ACL_BRAND_NFS4:
return "NFSv4";
case ACL_BRAND_POSIX:
return "POSIX.1e";
default:
return "unknown";
}
}

int
branding_mismatch(int brand1, int brand2)
{
if (brand1 == ACL_BRAND_UNKNOWN || brand2 == ACL_BRAND_UNKNOWN)
return (0);
if (brand1 != brand2)
return (1);
return (0);
}
