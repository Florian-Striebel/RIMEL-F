









#include <stdio.h>
#include <string.h>

#include <bsddialog.h>

int main()
{
int output;
struct bsddialog_conf conf;

bsddialog_initconf(&conf);
conf.title = "infobox";

if (bsddialog_init() < 0)
return -1;

output = bsddialog_infobox(&conf, "Example", 7, 20);

bsddialog_end();

return output;
}
