









#include <stdio.h>
#include <string.h>
#include <time.h>

#include <bsddialog.h>

int main()
{
int output;
unsigned int yy, mm, dd;
struct bsddialog_conf conf;
time_t cal;
struct tm *localtm;

time(&cal);
localtm = localtime(&cal);
yy = localtm->tm_year + 1900;
mm = localtm->tm_mon + 1;
dd = localtm->tm_mday;

bsddialog_initconf(&conf);
conf.title = "datebox";
conf.bottomtitle = "Press TAB and arrows";

if (bsddialog_init() < 0)
return -1;

output = bsddialog_datebox(&conf, "Example", 10, 50, &yy, &mm, &dd);

bsddialog_end();

switch (output) {
case BSDDIALOG_OK:
printf("Date: %u/%u/%u", yy, mm, dd);
break;
case BSDDIALOG_ESC:
printf("ESC\n");
break;
case BSDDIALOG_CANCEL:
printf("Cancel");
break;
case BSDDIALOG_ERROR:
printf("Error: %s", bsddialog_geterror());
break;
}
printf("\n");

return output;
}
