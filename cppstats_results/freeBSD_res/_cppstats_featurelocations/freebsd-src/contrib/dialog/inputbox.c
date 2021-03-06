

























#include <dlg_internals.h>
#include <dlg_keys.h>

#define sTEXT -1

#define NAVIGATE_BINDINGS DLG_KEYS_DATA( DLGK_FIELD_NEXT, KEY_DOWN ), DLG_KEYS_DATA( DLGK_FIELD_NEXT, KEY_RIGHT ), DLG_KEYS_DATA( DLGK_FIELD_NEXT, TAB ), DLG_KEYS_DATA( DLGK_FIELD_PREV, KEY_BTAB ), DLG_KEYS_DATA( DLGK_FIELD_PREV, KEY_LEFT ), DLG_KEYS_DATA( DLGK_FIELD_PREV, KEY_UP )







#define BTN_HIGH 1
#define HDR_HIGH 1
#define MIN_HIGH (HDR_HIGH + (MARGIN * 2 + 1) + (BTN_HIGH + MARGIN * 2))
#define MIN_WIDE 26




int
dialog_inputbox(const char *title, const char *cprompt, int height, int width,
const char *init, const int password)
{

static DLG_KEYS_BINDING binding[] = {
HELPKEY_BINDINGS,
ENTERKEY_BINDINGS,
NAVIGATE_BINDINGS,
TOGGLEKEY_BINDINGS,
END_KEYS_BINDING
};
static DLG_KEYS_BINDING binding2[] = {
INPUTSTR_BINDINGS,
HELPKEY_BINDINGS,
ENTERKEY_BINDINGS,
NAVIGATE_BINDINGS,

END_KEYS_BINDING
};


#if defined(KEY_RESIZE)
int old_height = height;
int old_width = width;
#endif
int xorg, yorg;
int x, y, box_y, box_x, box_width;
int show_buttons;
int col_offset = 0;
int chr_offset = 0;
int key, fkey, code;
int result = DLG_EXIT_UNKNOWN;
int state;
bool first;
bool edited;
char *input;
WINDOW *dialog;
WINDOW *editor;
char *prompt = dlg_strclone(cprompt);
const char **buttons = dlg_ok_labels();

dlg_does_output();

DLG_TRACE(("#inputbox args:\n"));
DLG_TRACE2S("title", title);
DLG_TRACE2S("message", cprompt);
DLG_TRACE2N("height", height);
DLG_TRACE2N("width", width);
DLG_TRACE2S("init", init);
DLG_TRACE2N("password", password);

dlg_tab_correct_str(prompt);


input = dlg_set_result(init);
edited = FALSE;

#if defined(KEY_RESIZE)
retry:
#endif
show_buttons = TRUE;
state = dialog_vars.default_button >= 0 ? dlg_default_button() : sTEXT;
first = (state == sTEXT);
key = fkey = 0;

if (init != NULL) {
dlg_auto_size(title, prompt, &height, &width, MIN_HIGH,
MIN(MAX(dlg_count_columns(init) + 7, MIN_WIDE),
SCOLS - (dialog_vars.begin_set ?
dialog_vars.begin_x : 0)));
chr_offset = (int) strlen(init);
} else {
dlg_auto_size(title, prompt, &height, &width, MIN_HIGH, MIN_WIDE);
}
dlg_button_layout(buttons, &width);
dlg_print_size(height, width);
dlg_ctl_size(height, width);

xorg = dlg_box_x_ordinate(width);
yorg = dlg_box_y_ordinate(height);

dialog = dlg_new_window(height, width, yorg, xorg);
dlg_register_window(dialog, "inputbox", binding);
dlg_register_buttons(dialog, "inputbox", buttons);

dlg_mouse_setbase(xorg, yorg);

dlg_draw_box2(dialog, 0, 0, height, width, dialog_attr, border_attr, border2_attr);
dlg_draw_bottom_box2(dialog, border_attr, border2_attr, dialog_attr);
dlg_draw_title(dialog, title);

dlg_attrset(dialog, dialog_attr);
dlg_draw_helpline(dialog, FALSE);
dlg_print_autowrap(dialog, prompt, height, width);


box_width = width - 6;
getyx(dialog, y, x);
(void) x;
box_y = y + 2;
box_x = (width - box_width) / 2;
dlg_mouse_mkregion(y + 1, box_x - 1, 3, box_width + 2, 'i');
dlg_draw_box(dialog, y + 1, box_x - 1, 3, box_width + 2,
border_attr, border2_attr);


editor = dlg_sub_window(dialog, 1, box_width, yorg + box_y, xorg + box_x);
dlg_register_window(editor, "inputbox2", binding2);

if (*input != '\0') {
dlg_show_string(editor, input, chr_offset, inputbox_attr,
0, 0, box_width, (bool) (password != 0), first);
wsyncup(editor);
wcursyncup(editor);
}

while (result == DLG_EXIT_UNKNOWN) {



if (show_buttons) {
show_buttons = FALSE;
col_offset = dlg_edit_offset(input, chr_offset, box_width);
(void) wmove(dialog, box_y, box_x + col_offset);
dlg_draw_buttons(dialog, height - 2, 0, buttons, state, FALSE, width);
}

if (!first) {
if (*input != '\0' && !edited) {
dlg_show_string(editor, input, chr_offset, inputbox_attr,
0, 0, box_width, (bool) (password != 0), first);
wmove(editor, 0, chr_offset);
wsyncup(editor);
wcursyncup(editor);
}
key = dlg_mouse_wgetch((state == sTEXT) ? editor : dialog, &fkey);
if (dlg_result_key(key, fkey, &result)) {
if (!dlg_button_key(result, &code, &key, &fkey))
break;
}
}





if (fkey
&& is_DLGK_MOUSE(key)
&& (code = dlg_ok_buttoncode(key - M_EVENT)) >= 0) {
result = code;
continue;
}

if (state == sTEXT) {
int edit = dlg_edit_string(input, &chr_offset, key, fkey, first);

if (edit) {
dlg_show_string(editor, input, chr_offset, inputbox_attr,
0, 0, box_width, (bool) (password != 0), first);
wsyncup(editor);
wcursyncup(editor);
first = FALSE;
edited = TRUE;
continue;
} else if (first) {
first = FALSE;
continue;
}
}


if (!fkey && (code = dlg_char_to_button(key, buttons)) >= 0) {
dlg_del_window(dialog);
result = dlg_ok_buttoncode(code);
continue;
}


if (fkey) {
switch (key) {
case DLGK_MOUSE('i'):
state = 0;

case DLGK_FIELD_PREV:
show_buttons = TRUE;
state = dlg_prev_ok_buttonindex(state, sTEXT);
break;
case DLGK_FIELD_NEXT:
show_buttons = TRUE;
state = dlg_next_ok_buttonindex(state, sTEXT);
break;
case DLGK_TOGGLE:
case DLGK_ENTER:
dlg_del_window(dialog);
result = (state >= 0) ? dlg_enter_buttoncode(state) : DLG_EXIT_OK;
break;
case DLGK_LEAVE:
if (state >= 0)
result = dlg_ok_buttoncode(state);
break;
#if defined(KEY_RESIZE)
case KEY_RESIZE:
dlg_will_resize(dialog);

height = old_height;
width = old_width;

_dlg_resize_cleanup(dialog);
goto retry;
#endif
default:
beep();
break;
}
} else if (key > 0) {
beep();
}
}
AddLastKey();

dlg_unregister_window(editor);
dlg_del_window(dialog);
dlg_mouse_free_regions();
free(prompt);
return result;
}
