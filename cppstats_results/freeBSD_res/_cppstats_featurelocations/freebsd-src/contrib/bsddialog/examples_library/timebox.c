









#include <stdio.h>
#include <string.h>
#include <time.h>

#include <bsddialog.h>

int main()
{
int output;
unsigned int hh, mm, ss;
struct bsddialog_conf conf;
time_t clock;
struct tm *localtm;

time(&clock);
localtm = localtime(&clock);
hh = localtm->tm_hour;
mm = localtm->tm_min;
ss = localtm->tm_sec;

bsddialog_initconf(&conf);
conf.title = "timebox";
conf.bottomtitle = "Press TAB and arrows";

if (bsddialog_init() < 0)
return -1;

output = bsddialog_timebox(&conf, "Example", 10, 50, &hh, &mm, &ss);

bsddialog_end();

switch (output) {
case BSDDIALOG_OK:
printf("Time: [%u:%u:%u]\n", hh, mm, ss);
break;
case BSDDIALOG_ESC:
printf("ESC\n");
break;
case BSDDIALOG_CANCEL:
printf("Cancel\n");
break;
case BSDDIALOG_ERROR:
printf("Error: %s\n", bsddialog_geterror());
break;
}

return output;
}
