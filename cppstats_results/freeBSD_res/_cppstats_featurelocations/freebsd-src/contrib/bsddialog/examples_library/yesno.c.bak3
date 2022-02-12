









#include <stdio.h>
#include <string.h>

#include <bsddialog.h>

int main()
{
int output;
struct bsddialog_conf conf;

bsddialog_initconf(&conf);
conf.title = "yesno";

if (bsddialog_init() < 0)
return -1;

output = bsddialog_yesno(&conf, "Example", 7, 25);

bsddialog_end();

return output;
}
