


#if HAVE_CONFIG_H
#include <ldns/config.h>
#endif

int isascii(int c);


int
isascii(int c)
{
return c >= 0 && c < 128;
}
