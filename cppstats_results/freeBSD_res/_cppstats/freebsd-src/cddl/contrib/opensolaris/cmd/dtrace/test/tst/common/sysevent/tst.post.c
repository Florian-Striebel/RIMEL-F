#pragma ident "%Z%%M% %I% %E% SMI"
#include <libsysevent.h>
#include <stdio.h>
int
main(int argc, char **argv)
{
sysevent_id_t id;
for (;;) {
if (sysevent_post_event("class_dtest", "subclass_dtest",
"vendor_dtest", "publisher_dtest", NULL, &id) != 0) {
(void) fprintf(stderr, "failed to post sysevent\n");
return (1);
}
sleep(1);
}
}
