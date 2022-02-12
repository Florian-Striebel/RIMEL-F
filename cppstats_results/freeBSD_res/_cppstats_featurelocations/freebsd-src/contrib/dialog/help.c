






















#include <dialog.h>




int
dialog_helpfile(const char *title,
const char *file,
int height,
int width)
{
int result = DLG_EXIT_ERROR;
DIALOG_VARS save;

if (!dialog_vars.in_helpfile && file != 0 && *file != '\0') {
dlg_save_vars(&save);

dialog_vars.no_label = NULL;
dialog_vars.ok_label = NULL;
dialog_vars.help_button = FALSE;
dialog_vars.extra_button = FALSE;
dialog_vars.nook = FALSE;

dialog_vars.in_helpfile = TRUE;

result = dialog_textbox(title, file, height, width);

dlg_restore_vars(&save);
}
return (result);
}
