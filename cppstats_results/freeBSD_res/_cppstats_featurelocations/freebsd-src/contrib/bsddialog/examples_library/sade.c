









#include <bsddialog.h>
#include <stdio.h>
#include <string.h>


int main()
{
int i, output;
struct bsddialog_conf conf;
char *text = "Please review the disk setup. When complete, press the "
"Finish button";
struct bsddialog_menuitem items[5] = {
{"", false, 0, "ada0", "16 GB GPT", ""},
{"", false, 1, "ada0p1", "512 KB freebsd-boot", ""},
{"", false, 1, "ada0p2", "15 GB freebsd-ufs", ""},
{"", false, 1, "ada0p3", "819 MB freebsd-swap none", ""},
{"", false, 0, "ada1", "16 GB", ""}
};

bsddialog_initconf(&conf);

conf.title = "Partition Editor";
conf.menu.shortcut_buttons = true;
conf.menu.align_left = true;
conf.button.ok_label = "Create";
conf.button.with_extra = true;
conf.button.extra_label = "Delete";
conf.button.cancel_label = "Cancel";
conf.button.with_help = true;
conf.button.help_label = "Revert";
conf.button.generic1_label = "Auto";
conf.button.generic2_label = "Finish";
conf.button.default_label = "Finish";

if (bsddialog_init() == BSDDIALOG_ERROR)
return (1);
output = bsddialog_menu(&conf, text, 20, 0, 10, 5, items, NULL);
bsddialog_end();

printf("Menu:\n");
for (i=0; i<5; i++)
printf(" [%c] %s\n", items[i].on ? 'X' : ' ', items[i].name);


return (output);
}
