






















#if !defined(DLG_INTERNALS_H_included)
#define DLG_INTERNALS_H_included 1

#include <dialog.h>

#define TableSize(name) (sizeof(name)/sizeof((name)[0]))


#define resizeit(name, NAME) name = ((NAME >= old_##NAME) ? (NAME - (old_##NAME - old_##name)) : old_##name)




#define AddLastKey() if (dialog_vars.last_key) { if (dlg_need_separator()) dlg_add_separator(); dlg_add_last_key(-1); }









#endif
