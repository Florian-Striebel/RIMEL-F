









#include <stdio.h>
#include <string.h>

#include <bsddialog.h>

int main()
{
int output;
struct bsddialog_conf conf;

bsddialog_initconf(&conf);
conf.title = "pause";

if (bsddialog_init() < 0)
return -1;

output = bsddialog_pause(&conf, "Example", 8, 50, 10);

bsddialog_end();

switch (output) {
case BSDDIALOG_OK:
printf("OK\n");
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
case BSDDIALOG_TIMEOUT:
printf("Timeout\n");
break;
}

return output;
}
