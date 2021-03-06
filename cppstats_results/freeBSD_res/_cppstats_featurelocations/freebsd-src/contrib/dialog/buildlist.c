






















#include <dlg_internals.h>
#include <dlg_keys.h>





#define sLEFT (-2)
#define sRIGHT (-1)

#define KEY_LEFTCOL '^'
#define KEY_RIGHTCOL '$'

#define MIN_HIGH (1 + (5 * MARGIN))

typedef struct {
WINDOW *win;
int box_y;
int box_x;
int top_index;
int cur_index;
DIALOG_LISTITEM **ip;
} MY_DATA;

#if 0
#define TRACE(p) dlg_trace_msg p
#else
#define TRACE(p)
#endif

#define okIndex(all,index) ((index) >= 0 && (index) < (all)->item_no)

#define myItem(p,n) ((p)->ip)[n]
#define mySide(n) ((n)?"right":"left")

typedef struct {
DIALOG_LISTITEM *items;
int base_y;
int base_x;
int use_height;
int use_width;
int item_no;
int check_x;
int item_x;
MY_DATA list[2];
} ALL_DATA;





static int
index2row(ALL_DATA * all, int choice, int selected)
{
MY_DATA *data = all->list + selected;
int result = -1;

if (okIndex(all, choice)) {
int row;

for (row = 0; row < all->item_no; ++row) {
TRACE(("!... choice %d: %p vs row %d: %p\n",
choice, all->items + choice,
row, myItem(data, row)));
if (myItem(data, row) == all->items + choice) {
result = row;
break;
}
}
}
TRACE(("! index2row(choice %d, %s) = %d\n", choice, mySide(selected), result));
return result;
}




static int
row2index(ALL_DATA * all, int row, int selected)
{
MY_DATA *data = all->list + selected;
int result = -1;
int n;
for (n = 0; n < all->item_no; ++n) {
TRACE(("!... row %d: %p vs choice %d: %p\n",
row, myItem(data, row),
n, all->items + n));
if (myItem(data, row) == all->items + n) {
result = n;
break;
}
}
TRACE(("! row2index(row %d, %s) = %d\n", row, mySide(selected), result));
return result;
}





static void
print_item(ALL_DATA * all,
WINDOW *win,
DIALOG_LISTITEM * item,
int row,
int selected)
{
chtype save = dlg_get_attrs(win);
int i;
bool both = (!dialog_vars.no_tags && !dialog_vars.no_items);
bool first = TRUE;
int climit = (all->item_x - all->check_x - 1);
const char *show = (dialog_vars.no_items
? item->name
: item->text);


dlg_attrset(win, menubox_attr);
(void) wmove(win, row, 0);
for (i = 0; i < getmaxx(win); i++)
(void) waddch(win, ' ');

(void) wmove(win, row, all->check_x);
dlg_attrset(win, menubox_attr);

if (both) {
dlg_print_listitem(win, item->name, climit, first, selected);
(void) waddch(win, ' ');
first = FALSE;
}

(void) wmove(win, row, all->item_x);
climit = (getmaxx(win) - all->item_x + 1);
dlg_print_listitem(win, show, climit, first, selected);

if (selected) {
dlg_item_help(item->help);
}
dlg_attrset(win, save);
}




static void
print_1_list(ALL_DATA * all,
int choice,
int selected)
{
MY_DATA *data = all->list + selected;
DIALOG_LISTITEM *target = (okIndex(all, choice)
? all->items + choice
: 0);
WINDOW *win = data->win;
int i, j;
int last = 0;
int top_row = index2row(all, data->top_index, selected);
int max_rows = getmaxy(win);

TRACE(("! print_1_list %d %s, top %d\n", choice, mySide(selected), top_row));
for (i = j = 0; j < max_rows; i++) {
int ii = i + top_row;
if (ii < 0) {
continue;
} else if (myItem(data, ii)) {
print_item(all,
win,
myItem(data, ii),
j, myItem(data, ii) == target);
last = ++j;
} else {
break;
}
}
if (wmove(win, last, 0) != ERR) {
while (waddch(win, ' ') != ERR) {
;
}
}
(void) wnoutrefresh(win);
}





static int
prev_item(ALL_DATA * all, int choice, int selected)
{
int result = choice;
int row = index2row(all, choice, selected);
if (row > 0) {
row--;
result = row2index(all, row, selected);
}
TRACE(("! prev_item choice %d, %s = %d\n", choice, mySide(selected), result));
return result;
}




static bool
stop_prev(ALL_DATA * all, int choice, int selected)
{
return (prev_item(all, choice, selected) == choice);
}

static bool
check_hotkey(DIALOG_LISTITEM * items, int choice, int selected)
{
bool result = FALSE;

if ((items[choice].state != 0) == selected) {
if (dlg_match_char(dlg_last_getc(),
(dialog_vars.no_tags
? items[choice].text
: items[choice].name))) {
result = TRUE;
}
}
return result;
}





static int
next_item(ALL_DATA * all, int choice, int selected)
{
MY_DATA *data = all->list + selected;
int result = choice;
int row = index2row(all, choice, selected);
TRACE(("! given item %d, testing next-item on row %d\n", choice, row + 1));
if (myItem(data, row + 1)) {
result = row2index(all, row + 1, selected);
}
TRACE(("! next_item(%d, %s) ->%d\n", choice, mySide(selected), result));
return result;
}




static int
first_item(ALL_DATA * all, int selected)
{
MY_DATA *data = all->list + selected;
int result = -1;

if (myItem(data, 0) != 0) {
int n;

for (n = 0; n < all->item_no; ++n) {
if (myItem(data, 0) == &all->items[n]) {
result = n;
break;
}
}
}
TRACE(("! first_item %s = %d\n", mySide(selected), result));
return result;
}




static int
last_item(ALL_DATA * all, int selected)
{
MY_DATA *data = all->list + selected;
int result = -1;
int n;

for (n = 0; myItem(data, n) != 0; ++n) {
result = n;
}
if (result >= 0) {
result = row2index(all, result, selected);
}
TRACE(("! last_item %s = %d\n", mySide(selected), result));
return result;
}

static int
skip_rows(ALL_DATA * all, int row, int skip, int selected)
{
MY_DATA *data = all->list + selected;
int result = row;

if (skip > 0) {
int n;

for (n = row + 1; (n < all->item_no) && (n <= row + skip); ++n) {
if (myItem(data, n) == 0)
break;
result = n;
}
} else if (skip < 0) {
result -= skip;
if (result < 0)
result = 0;
}
TRACE(("! skip_rows row %d, skip %d, %s = %d\n",
row, skip, mySide(selected), result));
return result;
}




static int
closest_item(ALL_DATA * all, int choice, int selected)
{
int prev = choice;
int next = choice;
int result = choice;
int n;

for (n = choice; n >= 0; --n) {
if ((all->items[n].state != 0) == selected) {
prev = n;
break;
}
}
for (n = choice; n < all->item_no; ++n) {
if ((all->items[n].state != 0) == selected) {
next = n;
break;
}
}
if (prev != choice) {
result = prev;
if (next != choice) {
if ((choice - prev) > (next - choice)) {
result = next;
}
}
} else if (next != choice) {
result = next;
}
TRACE(("! XXX closest item choice %d, %s = %d\n",
choice, mySide(selected), result));
return result;
}

static void
print_both(ALL_DATA * all,
int choice)
{
int selected;
int cur_y, cur_x;
WINDOW *dialog = wgetparent(all->list[0].win);

TRACE(("! print_both %d\n", choice));
getyx(dialog, cur_y, cur_x);
for (selected = 0; selected < 2; ++selected) {
MY_DATA *data = all->list + selected;
WINDOW *win = data->win;
int thumb_top = index2row(all, data->top_index, selected);
int thumb_max = index2row(all, -1, selected);
int thumb_end = thumb_top + getmaxy(win);

print_1_list(all, choice, selected);

dlg_mouse_setcode(selected * KEY_MAX);
dlg_draw_scrollbar(dialog,
(long) (data->top_index),
(long) (thumb_top),
(long) MIN(thumb_end, thumb_max),
(long) thumb_max,
data->box_x + all->check_x,
data->box_x + getmaxx(win),
data->box_y,
data->box_y + getmaxy(win) + 1,
menubox_border2_attr,
menubox_border_attr);
}
(void) wmove(dialog, cur_y, cur_x);
dlg_mouse_setcode(0);
}

static void
set_top_item(ALL_DATA * all, int choice, int selected)
{
if (choice != all->list[selected].top_index) {
DLG_TRACE(("#set top of %s column to %d\n",
mySide(selected),
choice));
all->list[selected].top_index = choice;
}
}





static void
fix_top_item(ALL_DATA * all, int cur_item, int selected)
{
int top_item = all->list[selected].top_index;
int cur_row = index2row(all, cur_item, selected);
int top_row = index2row(all, top_item, selected);

if (cur_row < top_row) {
top_item = cur_item;
} else if ((cur_row - top_row) >= all->use_height) {
top_item = row2index(all, cur_row + 1 - all->use_height, selected);
}
if (cur_row < all->use_height) {
top_item = row2index(all, 0, selected);
}
DLG_TRACE(("#fix_top_item(cur_item %d, %s) ->top_item %d\n",
cur_item, mySide(selected), top_item));
set_top_item(all, top_item, selected);
}

static void
append_right_side(ALL_DATA * all, int choice)
{
MY_DATA *data = &all->list[1];
int j;
for (j = 0; j < all->item_no; ++j) {
if (myItem(data, j) == 0) {
myItem(data, j) = &all->items[choice];
break;
}
}
}

static void
amend_right_side(ALL_DATA * all, int choice)
{
MY_DATA *data = &all->list[1];
int j, k;
for (j = 0; j < all->item_no; ++j) {
if (myItem(data, j) == &all->items[choice]) {
for (k = j; k < all->item_no; ++k) {
if ((myItem(data, k) = myItem(data, k + 1)) == 0)
break;
}
break;
}
}
}

static void
fill_one_side(ALL_DATA * all, int selected)
{
int i, j;
MY_DATA *data = all->list + selected;

for (i = j = 0; j < all->item_no; ++j) {
myItem(data, i) = 0;
if ((all->items[j].state != 0) == selected) {
myItem(data, i) = all->items + j;
TRACE(("! %s item[%d] %p = all[%d] %p\n",
mySide(selected),
i, myItem(data, i),
j, all->items + j));
++i;
}
}
myItem(data, i) = 0;
}

static void
fill_both_sides(ALL_DATA * all)
{
int k;

for (k = 0; k < 2; ++k) {
fill_one_side(all, k);
}
}






int
dlg_buildlist(const char *title,
const char *cprompt,
int height,
int width,
int list_height,
int item_no,
DIALOG_LISTITEM * items,
const char *states,
int order_mode,
int *current_item)
{
#define THIS_FUNC "dlg_buildlist"

static DLG_KEYS_BINDING binding[] = {
HELPKEY_BINDINGS,
ENTERKEY_BINDINGS,
DLG_KEYS_DATA( DLGK_FIELD_NEXT, KEY_RIGHT ),
DLG_KEYS_DATA( DLGK_FIELD_NEXT, TAB ),
DLG_KEYS_DATA( DLGK_FIELD_PREV, KEY_BTAB ),
DLG_KEYS_DATA( DLGK_FIELD_PREV, KEY_LEFT ),
DLG_KEYS_DATA( DLGK_ITEM_FIRST, KEY_HOME ),
DLG_KEYS_DATA( DLGK_ITEM_LAST, KEY_END ),
DLG_KEYS_DATA( DLGK_ITEM_LAST, KEY_LL ),
DLG_KEYS_DATA( DLGK_ITEM_NEXT, '+' ),
DLG_KEYS_DATA( DLGK_ITEM_NEXT, KEY_DOWN ),
DLG_KEYS_DATA( DLGK_ITEM_NEXT, CHR_NEXT ),
DLG_KEYS_DATA( DLGK_ITEM_PREV, '-' ),
DLG_KEYS_DATA( DLGK_ITEM_PREV, KEY_UP ),
DLG_KEYS_DATA( DLGK_ITEM_PREV, CHR_PREVIOUS ),
DLG_KEYS_DATA( DLGK_PAGE_NEXT, KEY_NPAGE ),
DLG_KEYS_DATA( DLGK_PAGE_NEXT, DLGK_MOUSE(KEY_NPAGE) ),
DLG_KEYS_DATA( DLGK_PAGE_NEXT, DLGK_MOUSE(KEY_NPAGE+KEY_MAX) ),
DLG_KEYS_DATA( DLGK_PAGE_PREV, KEY_PPAGE ),
DLG_KEYS_DATA( DLGK_PAGE_PREV, DLGK_MOUSE(KEY_PPAGE) ),
DLG_KEYS_DATA( DLGK_PAGE_PREV, DLGK_MOUSE(KEY_PPAGE+KEY_MAX) ),
DLG_KEYS_DATA( DLGK_GRID_LEFT, KEY_LEFTCOL ),
DLG_KEYS_DATA( DLGK_GRID_RIGHT, KEY_RIGHTCOL ),
TOGGLEKEY_BINDINGS,
END_KEYS_BINDING
};


#if defined(KEY_RESIZE)
int old_height = height;
int old_width = width;
#endif
ALL_DATA all;
MY_DATA *data = all.list;
int i, j, k, key2, found, x, y, cur_x, cur_y;
int key, fkey;
bool save_visit = dialog_state.visit_items;
int button;
int cur_item;
int name_width, text_width, full_width, list_width;
int result = DLG_EXIT_UNKNOWN;
int num_states;
bool first = TRUE;
WINDOW *dialog;
char *prompt;
const char **buttons = dlg_ok_labels();
const char *widget_name = "buildlist";

dialog_state.plain_buttons = TRUE;




if (dialog_state.visit_cols)
dialog_state.visit_cols = 2;

memset(&all, 0, sizeof(all));
all.items = items;
all.item_no = item_no;
for (k = 0; k < 2; ++k) {
data[k].ip = dlg_calloc(DIALOG_LISTITEM *, (item_no + 2));
}
fill_both_sides(&all);

if (dialog_vars.default_item != 0) {
cur_item = dlg_default_listitem(items);
} else {
if ((cur_item = first_item(&all, 0)) < 0)
cur_item = first_item(&all, 1);
}
button = (dialog_state.visit_items
? (items[cur_item].state ? sRIGHT : sLEFT)
: dlg_default_button());

dlg_does_output();

#if defined(KEY_RESIZE)
retry:
#endif

prompt = dlg_strclone(cprompt);
dlg_tab_correct_str(prompt);

all.use_height = list_height;
all.use_width = (2 * (dlg_calc_list_width(item_no, items)
+ 4
+ 2 * MARGIN)
+ 1);
all.use_width = MAX(26, all.use_width);
if (all.use_height == 0) {

dlg_auto_size(title, prompt, &height, &width, MIN_HIGH, all.use_width);
dlg_calc_listh(&height, &all.use_height, item_no);
} else {
dlg_auto_size(title, prompt,
&height, &width,
MIN_HIGH + all.use_height, all.use_width);
}
dlg_button_layout(buttons, &width);
dlg_print_size(height, width);
dlg_ctl_size(height, width);


if (states == 0 || strlen(states) < 2)
states = " *";
num_states = (int) strlen(states);

x = dlg_box_x_ordinate(width);
y = dlg_box_y_ordinate(height);

dialog = dlg_new_window(height, width, y, x);
dlg_register_window(dialog, widget_name, binding);
dlg_register_buttons(dialog, widget_name, buttons);

dlg_mouse_setbase(all.base_x = x, all.base_y = y);

dlg_draw_box2(dialog, 0, 0, height, width, dialog_attr, border_attr, border2_attr);
dlg_draw_bottom_box2(dialog, border_attr, border2_attr, dialog_attr);
dlg_draw_title(dialog, title);

dlg_attrset(dialog, dialog_attr);
dlg_print_autowrap(dialog, prompt, height, width);

list_width = (width - 6 * MARGIN - 2) / 2;
getyx(dialog, cur_y, cur_x);
data[0].box_y = cur_y + 1;
data[0].box_x = MARGIN + 1;
data[1].box_y = cur_y + 1;
data[1].box_x = data[0].box_x + 1 + 2 * MARGIN + list_width;





all.use_height = height - MIN_HIGH - cur_y;
if (all.use_height <= 0)
all.use_height = 1;

for (k = 0; k < 2; ++k) {

data[k].win = dlg_sub_window(dialog, all.use_height, list_width,
y + data[k].box_y + 1,
x + data[k].box_x + 1);


dlg_draw_box(dialog, data[k].box_y, data[k].box_x,
all.use_height + 2 * MARGIN,
list_width + 2 * MARGIN,
menubox_border_attr, menubox_border2_attr);
}

text_width = 0;
name_width = 0;

for (i = 0; i < item_no; i++) {
text_width = MAX(text_width, dlg_count_columns(items[i].text));
name_width = MAX(name_width, dlg_count_columns(items[i].name));
}





all.use_width = (list_width - 6 * MARGIN);
if (dialog_vars.no_tags && !dialog_vars.no_items) {
full_width = MIN(all.use_width, text_width);
} else if (dialog_vars.no_items) {
full_width = MIN(all.use_width, name_width);
} else {
if (text_width >= 0
&& name_width >= 0
&& all.use_width > 0
&& text_width + name_width > all.use_width) {
int need = (int) (0.25 * all.use_width);
if (name_width > need) {
int want = (int) (all.use_width * ((double) name_width) /
(text_width + name_width));
name_width = (want > need) ? want : need;
}
text_width = all.use_width - name_width;
}
full_width = text_width + name_width;
}

all.check_x = (all.use_width - full_width) / 2;
all.item_x = ((dialog_vars.no_tags
? 0
: (dialog_vars.no_items
? 0
: (name_width + 2)))
+ all.check_x);


j = MIN(all.use_height, item_no);
for (i = 0; i < 2; ++i) {
if ((items[cur_item].state != 0) == i) {
int top_item = cur_item - j + 1;
if (top_item < 0)
top_item = 0;
while ((items[top_item].state != 0) != i)
++top_item;
set_top_item(&all, top_item, i);
} else {
set_top_item(&all, 0, i);
}
}


for (i = 0; i < 2; ++i) {
dlg_mouse_mkbigregion(data[i].box_y + 1,
data[i].box_x,
all.use_height,
list_width + 2,
2 * KEY_MAX + (i * (1 + all.use_height)),
1, 1, 1 );
}

dlg_draw_buttons(dialog, height - 2, 0, buttons, button, FALSE, width);

while (result == DLG_EXIT_UNKNOWN) {
int which = (items[cur_item].state != 0);
MY_DATA *moi = data + which;
int at_top = index2row(&all, moi->top_index, which);
int at_end = index2row(&all, -1, which);
int at_bot = skip_rows(&all, at_top, all.use_height, which);
int was_mouse;

DLG_TRACE(("#** state %d:%d top %d (%d:%d:%d) %s\n",
cur_item, item_no - 1,
moi->top_index,
at_top, at_bot, at_end,
mySide(which)));

if (first) {
print_both(&all, cur_item);
dlg_trace_win(dialog);
first = FALSE;
}

if (button < 0) {
int cur_row = index2row(&all, cur_item, which);
cur_y = (data[which].box_y
+ cur_row
+ 1);
if (at_top > 0)
cur_y -= at_top;
cur_x = (data[which].box_x
+ all.check_x + 1);
DLG_TRACE(("#...visit row %d (%d,%d)\n", cur_row, cur_y, cur_x));
wmove(dialog, cur_y, cur_x);
}

key = dlg_mouse_wgetch(dialog, &fkey);
if (dlg_result_key(key, fkey, &result)) {
if (!dlg_button_key(result, &button, &key, &fkey))
break;
}

was_mouse = (fkey && is_DLGK_MOUSE(key));
if (was_mouse)
key -= M_EVENT;

if (!was_mouse) {
;
} else if (key >= 2 * KEY_MAX) {
i = (key - 2 * KEY_MAX) % (1 + all.use_height);
j = (key - 2 * KEY_MAX) / (1 + all.use_height);
k = row2index(&all, i + at_top, j);
DLG_TRACE(("#MOUSE column %d, row %d ->item %d\n", j, i, k));
if (k >= 0 && j < 2) {
if (j != which) {



moi = data + j;
fix_top_item(&all, k, j);
}
which = j;
at_top = index2row(&all, moi->top_index, which);
at_bot = skip_rows(&all, at_top, all.use_height, which);
cur_item = k;
print_both(&all, cur_item);
key = DLGK_TOGGLE;
} else {
beep();
continue;
}
fkey = FALSE;
} else if (key >= KEY_MIN) {
if (key > KEY_MAX) {
if (which == 0) {
key = KEY_RIGHTCOL;
fkey = FALSE;
} else {
key -= KEY_MAX;
}
} else {
if (which == 1) {
key = KEY_LEFTCOL;
fkey = FALSE;
}
}
key = dlg_lookup_key(dialog, key, &fkey);
}






if (key == DLGK_TOGGLE) {
int new_choice;
int new_state = items[cur_item].state + 1;

if ((new_choice = next_item(&all, cur_item, which)) == cur_item) {
new_choice = prev_item(&all, cur_item, which);
}
DLG_TRACE(("#cur_item %d, new_choice:%d\n", cur_item, new_choice));

if (new_state >= num_states)
new_state = 0;

items[cur_item].state = new_state;
if (order_mode) {
fill_one_side(&all, 0);
if (new_state) {
append_right_side(&all, cur_item);
} else {
amend_right_side(&all, cur_item);
}
} else {
fill_both_sides(&all);
}
if (cur_item == moi->top_index) {
set_top_item(&all, new_choice, which);
}

if (new_choice >= 0) {
fix_top_item(&all, cur_item, !which);
cur_item = new_choice;
}
print_both(&all, cur_item);
dlg_trace_win(dialog);
continue;
}






found = FALSE;
if (!fkey) {
if (button < 0 || !dialog_state.visit_items) {
for (j = cur_item + 1; j < item_no; j++) {
if (check_hotkey(items, j, which)) {
found = TRUE;
i = j;
break;
}
}
if (!found) {
for (j = 0; j <= cur_item; j++) {
if (check_hotkey(items, j, which)) {
found = TRUE;
i = j;
break;
}
}
}
if (found)
dlg_flush_getc();
} else if ((j = dlg_char_to_button(key, buttons)) >= 0) {
button = j;
ungetch('\n');
continue;
}
}





if (!found
&& (key <= '9')
&& (key > '0')
&& (key - '1' < at_bot)) {
found = TRUE;
i = key - '1';
}

if (!found && fkey) {
switch (key) {
case DLGK_FIELD_PREV:
if ((button == sRIGHT) && dialog_state.visit_items) {
key = DLGK_GRID_LEFT;
button = sLEFT;
} else {
button = dlg_prev_button(buttons, button);
dlg_draw_buttons(dialog, height - 2, 0, buttons, button,
FALSE, width);
if (button == sRIGHT) {
key = DLGK_GRID_RIGHT;
} else {
continue;
}
}
break;
case DLGK_FIELD_NEXT:
if ((button == sLEFT) && dialog_state.visit_items) {
key = DLGK_GRID_RIGHT;
button = sRIGHT;
} else {
button = dlg_next_button(buttons, button);
dlg_draw_buttons(dialog, height - 2, 0, buttons, button,
FALSE, width);
if (button == sLEFT) {
key = DLGK_GRID_LEFT;
} else {
continue;
}
}
break;
}
}

if (!found && fkey) {
i = cur_item;
found = TRUE;
switch (key) {
case DLGK_GRID_LEFT:
i = closest_item(&all, cur_item, 0);
fix_top_item(&all, i, 0);
break;
case DLGK_GRID_RIGHT:
if (order_mode) {
i = last_item(&all, 1);
} else {
i = closest_item(&all, cur_item, 1);
}
fix_top_item(&all, i, 1);
break;
case DLGK_PAGE_PREV:
if (cur_item > moi->top_index) {
i = moi->top_index;
} else if (moi->top_index != 0) {
int temp = at_top;
if ((temp -= all.use_height) < 0)
temp = 0;
i = row2index(&all, temp, which);
}
break;
case DLGK_PAGE_NEXT:
if ((at_end - at_bot) < all.use_height) {
i = next_item(&all,
row2index(&all, at_end, which),
which);
} else {
i = next_item(&all,
row2index(&all, at_bot, which),
which);
at_top = at_bot;
set_top_item(&all,
next_item(&all,
row2index(&all, at_top, which),
which),
which);
at_bot = skip_rows(&all, at_top, all.use_height, which);
at_bot = MIN(at_bot, at_end);
}
break;
case DLGK_ITEM_FIRST:
i = first_item(&all, which);
break;
case DLGK_ITEM_LAST:
i = last_item(&all, which);
break;
case DLGK_ITEM_PREV:
i = prev_item(&all, cur_item, which);
if (stop_prev(&all, cur_item, which))
continue;
break;
case DLGK_ITEM_NEXT:
i = next_item(&all, cur_item, which);
break;
default:
found = FALSE;
break;
}
}

if (found) {
if (i != cur_item) {
int now_at = index2row(&all, i, which);
int oops = item_no;
int old_item;

DLG_TRACE(("#<--CHOICE %d\n", i));
DLG_TRACE(("#<--topITM %d\n", moi->top_index));
DLG_TRACE(("#<--now_at %d\n", now_at));
DLG_TRACE(("#<--at_top %d\n", at_top));
DLG_TRACE(("#<--at_bot %d\n", at_bot));

if (now_at >= at_bot) {
while (now_at >= at_bot) {
if ((at_bot - at_top) >= all.use_height) {
set_top_item(&all,
next_item(&all, moi->top_index, which),
which);
}
at_top = index2row(&all, moi->top_index, which);
at_bot = skip_rows(&all, at_top, all.use_height, which);

DLG_TRACE(("#...at_bot %d (now %d vs %d)\n",
at_bot, now_at, at_end));
DLG_TRACE(("#...topITM %d\n", moi->top_index));
DLG_TRACE(("#...at_top %d (diff %d)\n", at_top,
at_bot - at_top));

if (at_bot >= at_end) {





if ((at_bot - at_top) > all.use_height) {
set_top_item(&all,
next_item(&all, moi->top_index, which),
which);
} else if (at_top > 0 &&
(at_bot - at_top) >= all.use_height) {
set_top_item(&all,
next_item(&all, moi->top_index, which),
which);
}
break;
}
if (--oops < 0) {
DLG_TRACE(("#OOPS-forward\n"));
break;
}
}
} else if (now_at < at_top) {
while (now_at < at_top) {
old_item = moi->top_index;
set_top_item(&all,
prev_item(&all, moi->top_index, which),
which);
at_top = index2row(&all, moi->top_index, which);

DLG_TRACE(("#...at_top %d (now %d)\n", at_top, now_at));
DLG_TRACE(("#...topITM %d\n", moi->top_index));

if (moi->top_index >= old_item)
break;
if (at_top <= now_at)
break;
if (--oops < 0) {
DLG_TRACE(("#OOPS-backward\n"));
break;
}
}
}
DLG_TRACE(("#-->now_at %d\n", now_at));
cur_item = i;
print_both(&all, cur_item);
}
dlg_trace_win(dialog);
continue;
}

if (fkey) {
switch (key) {
case DLGK_ENTER:
result = dlg_enter_buttoncode(button);
break;
case DLGK_LEAVE:
result = dlg_ok_buttoncode(button);
break;
#if defined(KEY_RESIZE)
case KEY_RESIZE:
dlg_will_resize(dialog);

height = old_height;
width = old_width;
free(prompt);
_dlg_resize_cleanup(dialog);

first = TRUE;
goto retry;
#endif
default:
if (was_mouse) {
if ((key2 = dlg_ok_buttoncode(key)) >= 0) {
result = key2;
break;
}
beep();
}
}
} else if (key > 0) {
beep();
}
}






if (order_mode) {
DIALOG_LISTITEM *redo;
int row;
int choice;
int new_item = cur_item;

redo = dlg_calloc(DIALOG_LISTITEM, (size_t) item_no + 1);
assert_ptr(redo, THIS_FUNC);

j = 0;
for (k = 0; k < 2; ++k) {
for (row = 0; row < item_no; ++row) {
if (myItem(all.list + k, row) == 0)
break;
choice = row2index(&all, row, k);
if (choice == cur_item)
new_item = j;
redo[j++] = items[choice];
}
}

cur_item = new_item;
memcpy(items, redo, sizeof(DIALOG_LISTITEM) * (size_t) (item_no + 1));

free(redo);
}

for (k = 0; k < 2; ++k) {
free(data[k].ip);
}

dialog_state.visit_cols = save_visit;
dlg_del_window(dialog);
dlg_mouse_free_regions();
free(prompt);

*current_item = cur_item;
return result;
#undef THIS_FUNC
}




int
dialog_buildlist(const char *title,
const char *cprompt,
int height,
int width,
int list_height,
int item_no,
char **items,
int order_mode)
{
#define THIS_FUNC "dialog_buildlist"
int result;
int i, j;
DIALOG_LISTITEM *listitems;
bool separate_output = dialog_vars.separate_output;
bool show_status = FALSE;
int current = 0;
char *help_result;

DLG_TRACE(("#buildlist args:\n"));
DLG_TRACE2S("title", title);
DLG_TRACE2S("message", cprompt);
DLG_TRACE2N("height", height);
DLG_TRACE2N("width", width);
DLG_TRACE2N("lheight", list_height);
DLG_TRACE2N("llength", item_no);

DLG_TRACE2N("order", order_mode != 0);

listitems = dlg_calloc(DIALOG_LISTITEM, (size_t) item_no + 1);
assert_ptr(listitems, THIS_FUNC);

for (i = j = 0; i < item_no; ++i) {
listitems[i].name = items[j++];
listitems[i].text = (dialog_vars.no_items
? dlg_strempty()
: items[j++]);
listitems[i].state = !dlg_strcmp(items[j++], "on");
listitems[i].help = ((dialog_vars.item_help)
? items[j++]
: dlg_strempty());
}
dlg_align_columns(&listitems[0].text, (int) sizeof(DIALOG_LISTITEM), item_no);

result = dlg_buildlist(title,
cprompt,
height,
width,
list_height,
item_no,
listitems,
NULL,
order_mode,
&current);

switch (result) {
case DLG_EXIT_OK:
case DLG_EXIT_EXTRA:
show_status = TRUE;
break;
case DLG_EXIT_HELP:
dlg_add_help_listitem(&result, &help_result, &listitems[current]);
if ((show_status = dialog_vars.help_status)) {
if (separate_output) {
dlg_add_string(help_result);
} else {
dlg_add_quoted(help_result);
}
} else {
dlg_add_string(help_result);
}
break;
}

if (show_status) {
for (i = 0; i < item_no; i++) {
if (listitems[i].state) {
if (dlg_need_separator())
dlg_add_separator();
if (separate_output) {
dlg_add_string(listitems[i].name);
} else {
dlg_add_quoted(listitems[i].name);
}
}
}
AddLastKey();
}

dlg_free_columns(&listitems[0].text, (int) sizeof(DIALOG_LISTITEM), item_no);
free(listitems);
return result;
#undef THIS_FUNC
}
