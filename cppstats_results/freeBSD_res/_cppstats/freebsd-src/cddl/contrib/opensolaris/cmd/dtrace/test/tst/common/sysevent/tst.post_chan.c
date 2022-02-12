#include <libsysevent.h>
#include <stdio.h>
int
main(int argc, char **argv)
{
evchan_t *ch;
if (sysevent_evc_bind("channel_dtest", &ch,
EVCH_CREAT | EVCH_HOLD_PEND) != 0) {
(void) fprintf(stderr, "failed to bind to sysevent channel\n");
return (1);
}
for (;;) {
if (sysevent_evc_publish(ch, "class_dtest", "subclass_dtest",
"vendor_dtest", "publisher_dtest", NULL, EVCH_SLEEP) != 0) {
(void) sysevent_evc_unbind(ch);
(void) fprintf(stderr, "failed to publisth sysevent\n");
return (1);
}
sleep(1);
}
}
