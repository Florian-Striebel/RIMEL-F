










#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <bsddialog.h>

#define H BSDDIALOG_FIELDHIDDEN
#define RO BSDDIALOG_FIELDREADONLY

int main()
{
int i, output;
struct bsddialog_conf conf;
struct bsddialog_formitem items[3] = {
{"Input:", 1, 1, "value", 1, 11, 30, 50, NULL, 0, "desc 1"},
{"Input:", 2, 1, "read only", 2, 11, 30, 50, NULL, RO, "desc 2"},
{"Password:", 3, 1, "", 3, 11, 30, 50, NULL, H, "desc 3"}
};

bsddialog_initconf(&conf);
conf.title = "form";
conf.form.securech = '*';

if (bsddialog_init() < 0)
return -1;

output = bsddialog_form(&conf, "Example", 10, 50, 3, 3, items);

bsddialog_end();

if (output == BSDDIALOG_ERROR)
printf("Error: %s", bsddialog_geterror());

for (i=0; i<3; i++) {
printf("%s \"%s\"\n", items[i].label, items[i].value);
free(items[i].value);
}

return output;
}
