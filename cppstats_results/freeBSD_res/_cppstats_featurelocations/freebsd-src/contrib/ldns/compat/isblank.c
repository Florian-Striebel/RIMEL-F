


#if HAVE_CONFIG_H
#include <ldns/config.h>
#endif

int isblank(int c);


int
isblank(int c)
{
return (c == ' ') || (c == '\t');
}
