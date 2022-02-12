









#include <stdio.h>
#include <string.h>

#include <bsddialog.h>

int main()
{
int i, output;
struct bsddialog_conf conf;
struct bsddialog_menuitem items[5] = {
{"", false, 0, "Name 1", "Desc 1", "Bottom Desc 1"},
{"", true, 0, "Name 2", "Desc 2", "Bottom Desc 2"},
{"", false, 0, "Name 3", "Desc 3", "Bottom Desc 3"},
{"", true, 0, "Name 4", "Desc 4", "Bottom Desc 4"},
{"", false, 0, "Name 5", "Desc 5", "Bottom Desc 5"}
};

bsddialog_initconf(&conf);
conf.title = "radiolist";

if (bsddialog_init() < 0)
return -1;

output = bsddialog_buildlist(&conf, "Example", 15, 30, 5, 5, items, NULL);

bsddialog_end();

printf("Buildlist:\n");
for (i=0; i<5; i++)
printf(" [%c] %s\n", items[i].on ? 'X' : ' ', items[i].name);


return output;
}
