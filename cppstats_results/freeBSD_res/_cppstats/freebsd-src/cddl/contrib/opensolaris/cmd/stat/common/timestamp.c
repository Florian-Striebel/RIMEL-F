#include "statcommon.h"
#include <langinfo.h>
void
print_timestamp(uint_t timestamp_fmt)
{
time_t t = time(NULL);
if (timestamp_fmt == UDATE) {
(void) printf("%ld\n", t);
} else if (timestamp_fmt == DDATE) {
char dstr[64];
int len;
len = strftime(dstr, sizeof (dstr), "%+", localtime(&t));
if (len > 0)
(void) printf("%s\n", dstr);
}
}
