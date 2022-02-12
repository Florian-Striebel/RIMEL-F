









#include <stdio.h>
#include <string.h>

#include <bsddialog.h>

int main()
{
int input;
struct bsddialog_conf conf;


bsddialog_initconf(&conf);
conf.title = "msgbox";


if (bsddialog_init() == BSDDIALOG_ERROR) {
printf("Error: %s\n", bsddialog_geterror());
return -1;
}
input = bsddialog_msgbox(&conf, "Example", 7, 20);
bsddialog_end();


printf("User input: ");
switch (input) {
case BSDDIALOG_ERROR: printf("Error %s\n", bsddialog_geterror()); break;
case BSDDIALOG_OK: printf("OK\n"); break;
case BSDDIALOG_ESC: printf("ESC\n"); break;
}

return input;
}
