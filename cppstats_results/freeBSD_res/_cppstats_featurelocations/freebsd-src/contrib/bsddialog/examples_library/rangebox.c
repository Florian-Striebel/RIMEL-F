









#include <stdio.h>
#include <string.h>

#include <bsddialog.h>

int main()
{
int value, output;
struct bsddialog_conf conf;

bsddialog_initconf(&conf);
conf.title = "rangebox";

if (bsddialog_init() < 0)
return -1;

value = 5;
output = bsddialog_rangebox(&conf, "Example", 8, 50, 0, 10, &value);

bsddialog_end();

printf("Value: %d", value);

return output;
}
