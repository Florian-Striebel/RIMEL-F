#include <sys/types.h>
#include <unistd.h>
#include <assert.h>
#define PRIV_SYS_CONFIG 0
static __inline int
priv_ineffect(int priv)
{
assert(priv == PRIV_SYS_CONFIG);
return (geteuid() == 0);
}
