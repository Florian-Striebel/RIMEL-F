





















#include <dialog.h>

#define quoted(a) #a
#define concat(a,b) a "-" quoted(b)
#define DLG_VERSION concat(DIALOG_VERSION,DIALOG_PATCHDATE)

const char *
dialog_version(void)
{
return DLG_VERSION;
}
