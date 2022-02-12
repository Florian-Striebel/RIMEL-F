

































#include "config.h"
#if !defined(lint) && !defined(SCCSID)
#if 0
static char sccsid[] = "@(#)map.c 8.1 (Berkeley) 6/4/93";
#else
__RCSID("$NetBSD: map.c,v 1.54 2021/08/29 09:41:59 christos Exp $");
#endif
#endif




#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "el.h"
#include "common.h"
#include "emacs.h"
#include "vi.h"
#include "fcns.h"
#include "func.h"
#include "help.h"
#include "parse.h"

static void map_print_key(EditLine *, el_action_t *, const wchar_t *);
static void map_print_some_keys(EditLine *, el_action_t *, wint_t, wint_t);
static void map_print_all_keys(EditLine *);
static void map_init_nls(EditLine *);
static void map_init_meta(EditLine *);




static const el_action_t el_map_emacs[] = {
EM_SET_MARK,
ED_MOVE_TO_BEG,
ED_PREV_CHAR,
ED_IGNORE,
EM_DELETE_OR_LIST,
ED_MOVE_TO_END,
ED_NEXT_CHAR,
ED_UNASSIGNED,
EM_DELETE_PREV_CHAR,
ED_UNASSIGNED,
ED_NEWLINE,
ED_KILL_LINE,
ED_CLEAR_SCREEN,
ED_NEWLINE,
ED_NEXT_HISTORY,
ED_IGNORE,
ED_PREV_HISTORY,
ED_IGNORE,
EM_INC_SEARCH_PREV,
ED_IGNORE,
ED_TRANSPOSE_CHARS,
EM_KILL_LINE,
ED_QUOTED_INSERT,
ED_DELETE_PREV_WORD,
ED_SEQUENCE_LEAD_IN,
EM_YANK,
ED_IGNORE,
EM_META_NEXT,
ED_IGNORE,
ED_IGNORE,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_DIGIT,
ED_DIGIT,
ED_DIGIT,
ED_DIGIT,
ED_DIGIT,
ED_DIGIT,
ED_DIGIT,
ED_DIGIT,
ED_DIGIT,
ED_DIGIT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
EM_DELETE_PREV_CHAR,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_DELETE_PREV_WORD,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_CLEAR_SCREEN,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
EM_COPY_PREV_WORD,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_ARGUMENT_DIGIT,
ED_ARGUMENT_DIGIT,
ED_ARGUMENT_DIGIT,
ED_ARGUMENT_DIGIT,
ED_ARGUMENT_DIGIT,
ED_ARGUMENT_DIGIT,
ED_ARGUMENT_DIGIT,
ED_ARGUMENT_DIGIT,
ED_ARGUMENT_DIGIT,
ED_ARGUMENT_DIGIT,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_PREV_WORD,
EM_CAPITOL_CASE,
EM_DELETE_NEXT_WORD,
ED_UNASSIGNED,
EM_NEXT_WORD,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
EM_LOWER_CASE,
ED_UNASSIGNED,
ED_SEARCH_NEXT_HISTORY,
ED_SEQUENCE_LEAD_IN,
ED_SEARCH_PREV_HISTORY,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
EM_UPPER_CASE,
ED_UNASSIGNED,
EM_COPY_REGION,
ED_COMMAND,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_SEQUENCE_LEAD_IN,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_PREV_WORD,
EM_CAPITOL_CASE,
EM_DELETE_NEXT_WORD,
ED_UNASSIGNED,
EM_NEXT_WORD,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
EM_LOWER_CASE,
ED_UNASSIGNED,
ED_SEARCH_NEXT_HISTORY,
ED_UNASSIGNED,
ED_SEARCH_PREV_HISTORY,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
EM_UPPER_CASE,
ED_UNASSIGNED,
EM_COPY_REGION,
ED_COMMAND,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_DELETE_PREV_WORD

};








static const el_action_t el_map_vi_insert[] = {
#if defined(KSHVI)
ED_UNASSIGNED,
ED_INSERT,
ED_INSERT,
ED_INSERT,
VI_LIST_OR_EOF,
ED_INSERT,
ED_INSERT,
ED_INSERT,
VI_DELETE_PREV_CHAR,
ED_INSERT,
ED_NEWLINE,
ED_INSERT,
ED_INSERT,
ED_NEWLINE,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_IGNORE,
ED_INSERT,
ED_IGNORE,
ED_INSERT,
VI_KILL_LINE_PREV,
ED_QUOTED_INSERT,
ED_DELETE_PREV_WORD,

ED_INSERT,
ED_INSERT,
ED_INSERT,
VI_COMMAND_MODE,
ED_IGNORE,
ED_INSERT,
ED_INSERT,
ED_INSERT,
#else






ED_UNASSIGNED,
ED_MOVE_TO_BEG,
ED_PREV_CHAR,
ED_IGNORE,
VI_LIST_OR_EOF,
ED_MOVE_TO_END,
ED_NEXT_CHAR,
ED_UNASSIGNED,
VI_DELETE_PREV_CHAR,
ED_UNASSIGNED,
ED_NEWLINE,
ED_KILL_LINE,
ED_CLEAR_SCREEN,
ED_NEWLINE,
ED_NEXT_HISTORY,
ED_IGNORE,
ED_PREV_HISTORY,
ED_IGNORE,
ED_REDISPLAY,
ED_IGNORE,
ED_TRANSPOSE_CHARS,
VI_KILL_LINE_PREV,
ED_QUOTED_INSERT,
ED_DELETE_PREV_WORD,
ED_UNASSIGNED,
ED_IGNORE,
ED_IGNORE,
VI_COMMAND_MODE,
ED_IGNORE,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
#endif
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
VI_DELETE_PREV_CHAR,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT,
ED_INSERT
};

static const el_action_t el_map_vi_command[] = {
ED_UNASSIGNED,
ED_MOVE_TO_BEG,
ED_UNASSIGNED,
ED_IGNORE,
ED_UNASSIGNED,
ED_MOVE_TO_END,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_DELETE_PREV_CHAR,
ED_UNASSIGNED,
ED_NEWLINE,
ED_KILL_LINE,
ED_CLEAR_SCREEN,
ED_NEWLINE,
ED_NEXT_HISTORY,
ED_IGNORE,
ED_PREV_HISTORY,
ED_IGNORE,
ED_REDISPLAY,
ED_IGNORE,
ED_UNASSIGNED,
VI_KILL_LINE_PREV,
ED_UNASSIGNED,
ED_DELETE_PREV_WORD,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
EM_META_NEXT,
ED_IGNORE,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_NEXT_CHAR,
ED_UNASSIGNED,
ED_UNASSIGNED,
VI_COMMENT_OUT,
ED_MOVE_TO_END,
VI_MATCH,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_NEXT_HISTORY,
VI_REPEAT_PREV_CHAR,
ED_PREV_HISTORY,
VI_REDO,
VI_SEARCH_PREV,
VI_ZERO,
ED_ARGUMENT_DIGIT,
ED_ARGUMENT_DIGIT,
ED_ARGUMENT_DIGIT,
ED_ARGUMENT_DIGIT,
ED_ARGUMENT_DIGIT,
ED_ARGUMENT_DIGIT,
ED_ARGUMENT_DIGIT,
ED_ARGUMENT_DIGIT,
ED_ARGUMENT_DIGIT,
ED_COMMAND,
VI_REPEAT_NEXT_CHAR,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
VI_SEARCH_NEXT,
VI_ALIAS,
VI_ADD_AT_EOL,
VI_PREV_BIG_WORD,
VI_CHANGE_TO_EOL,
ED_KILL_LINE,
VI_END_BIG_WORD,
VI_PREV_CHAR,
VI_TO_HISTORY_LINE,
ED_UNASSIGNED,
VI_INSERT_AT_BOL,
ED_SEARCH_NEXT_HISTORY,
ED_SEARCH_PREV_HISTORY,
ED_UNASSIGNED,
ED_UNASSIGNED,
VI_REPEAT_SEARCH_PREV,
ED_SEQUENCE_LEAD_IN,
VI_PASTE_PREV,
ED_UNASSIGNED,
VI_REPLACE_MODE,
VI_SUBSTITUTE_LINE,
VI_TO_PREV_CHAR,
VI_UNDO_LINE,
ED_UNASSIGNED,
VI_NEXT_BIG_WORD,
ED_DELETE_PREV_CHAR,
VI_YANK_END,
ED_UNASSIGNED,
ED_SEQUENCE_LEAD_IN,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_MOVE_TO_BEG,
VI_HISTORY_WORD,
ED_UNASSIGNED,
VI_ADD,
VI_PREV_WORD,
VI_CHANGE_META,
VI_DELETE_META,
VI_END_WORD,
VI_NEXT_CHAR,
ED_UNASSIGNED,
ED_PREV_CHAR,
VI_INSERT,
ED_NEXT_HISTORY,
ED_PREV_HISTORY,
ED_NEXT_CHAR,
ED_UNASSIGNED,
VI_REPEAT_SEARCH_NEXT,
ED_UNASSIGNED,
VI_PASTE_NEXT,
ED_UNASSIGNED,
VI_REPLACE_CHAR,
VI_SUBSTITUTE_CHAR,
VI_TO_NEXT_CHAR,
VI_UNDO,
VI_HISTEDIT,
VI_NEXT_WORD,
ED_DELETE_NEXT_CHAR,
VI_YANK,
ED_UNASSIGNED,
ED_UNASSIGNED,
VI_TO_COLUMN,
ED_UNASSIGNED,
VI_CHANGE_CASE,
ED_DELETE_PREV_CHAR,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_SEQUENCE_LEAD_IN,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_SEQUENCE_LEAD_IN,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED,
ED_UNASSIGNED
};





libedit_private int
map_init(EditLine *el)
{




#if defined(MAP_DEBUG)
if (sizeof(el_map_emacs) != N_KEYS * sizeof(el_action_t))
EL_ABORT((el->el_errfile, "Emacs map incorrect\n"));
if (sizeof(el_map_vi_command) != N_KEYS * sizeof(el_action_t))
EL_ABORT((el->el_errfile, "Vi command map incorrect\n"));
if (sizeof(el_map_vi_insert) != N_KEYS * sizeof(el_action_t))
EL_ABORT((el->el_errfile, "Vi insert map incorrect\n"));
#endif

el->el_map.alt = el_calloc(N_KEYS, sizeof(*el->el_map.alt));
if (el->el_map.alt == NULL)
return -1;
el->el_map.key = el_calloc(N_KEYS, sizeof(*el->el_map.key));
if (el->el_map.key == NULL)
return -1;
el->el_map.emacs = el_map_emacs;
el->el_map.vic = el_map_vi_command;
el->el_map.vii = el_map_vi_insert;
el->el_map.help = el_calloc(EL_NUM_FCNS, sizeof(*el->el_map.help));
if (el->el_map.help == NULL)
return -1;
(void) memcpy(el->el_map.help, el_func_help,
sizeof(*el->el_map.help) * EL_NUM_FCNS);
el->el_map.func = el_calloc(EL_NUM_FCNS, sizeof(*el->el_map.func));
if (el->el_map.func == NULL)
return -1;
memcpy(el->el_map.func, el_func, sizeof(*el->el_map.func)
* EL_NUM_FCNS);
el->el_map.nfunc = EL_NUM_FCNS;

#if defined(VIDEFAULT)
map_init_vi(el);
#else
map_init_emacs(el);
#endif
return 0;
}





libedit_private void
map_end(EditLine *el)
{

el_free(el->el_map.alt);
el->el_map.alt = NULL;
el_free(el->el_map.key);
el->el_map.key = NULL;
el->el_map.emacs = NULL;
el->el_map.vic = NULL;
el->el_map.vii = NULL;
el_free(el->el_map.help);
el->el_map.help = NULL;
el_free(el->el_map.func);
el->el_map.func = NULL;
}





static void
map_init_nls(EditLine *el)
{
int i;

el_action_t *map = el->el_map.key;

for (i = 0200; i <= 0377; i++)
if (iswprint(i))
map[i] = ED_INSERT;
}





static void
map_init_meta(EditLine *el)
{
wchar_t buf[3];
int i;
el_action_t *map = el->el_map.key;
el_action_t *alt = el->el_map.alt;

for (i = 0; i <= 0377 && map[i] != EM_META_NEXT; i++)
continue;

if (i > 0377) {
for (i = 0; i <= 0377 && alt[i] != EM_META_NEXT; i++)
continue;
if (i > 0377) {
i = 033;
if (el->el_map.type == MAP_VI)
map = alt;
} else
map = alt;
}
buf[0] = (wchar_t)i;
buf[2] = 0;
for (i = 0200; i <= 0377; i++)
switch (map[i]) {
case ED_INSERT:
case ED_UNASSIGNED:
case ED_SEQUENCE_LEAD_IN:
break;
default:
buf[1] = i & 0177;
keymacro_add(el, buf, keymacro_map_cmd(el, (int) map[i]), XK_CMD);
break;
}
map[(int) buf[0]] = ED_SEQUENCE_LEAD_IN;
}





libedit_private void
map_init_vi(EditLine *el)
{
int i;
el_action_t *key = el->el_map.key;
el_action_t *alt = el->el_map.alt;
const el_action_t *vii = el->el_map.vii;
const el_action_t *vic = el->el_map.vic;

el->el_map.type = MAP_VI;
el->el_map.current = el->el_map.key;

keymacro_reset(el);

for (i = 0; i < N_KEYS; i++) {
key[i] = vii[i];
alt[i] = vic[i];
}

map_init_meta(el);
map_init_nls(el);

tty_bind_char(el, 1);
terminal_bind_arrow(el);
}





libedit_private void
map_init_emacs(EditLine *el)
{
int i;
wchar_t buf[3];
el_action_t *key = el->el_map.key;
el_action_t *alt = el->el_map.alt;
const el_action_t *emacs = el->el_map.emacs;

el->el_map.type = MAP_EMACS;
el->el_map.current = el->el_map.key;
keymacro_reset(el);

for (i = 0; i < N_KEYS; i++) {
key[i] = emacs[i];
alt[i] = ED_UNASSIGNED;
}

map_init_meta(el);
map_init_nls(el);

buf[0] = CONTROL('X');
buf[1] = CONTROL('X');
buf[2] = 0;
keymacro_add(el, buf, keymacro_map_cmd(el, EM_EXCHANGE_MARK), XK_CMD);

tty_bind_char(el, 1);
terminal_bind_arrow(el);
}





libedit_private int
map_set_editor(EditLine *el, wchar_t *editor)
{

if (wcscmp(editor, L"emacs") == 0) {
map_init_emacs(el);
return 0;
}
if (wcscmp(editor, L"vi") == 0) {
map_init_vi(el);
return 0;
}
return -1;
}





libedit_private int
map_get_editor(EditLine *el, const wchar_t **editor)
{

if (editor == NULL)
return -1;
switch (el->el_map.type) {
case MAP_EMACS:
*editor = L"emacs";
return 0;
case MAP_VI:
*editor = L"vi";
return 0;
}
return -1;
}





static void
map_print_key(EditLine *el, el_action_t *map, const wchar_t *in)
{
char outbuf[EL_BUFSIZ];
el_bindings_t *bp, *ep;

if (in[0] == '\0' || in[1] == '\0') {
(void) keymacro__decode_str(in, outbuf, sizeof(outbuf), "");
ep = &el->el_map.help[el->el_map.nfunc];
for (bp = el->el_map.help; bp < ep; bp++)
if (bp->func == map[(unsigned char) *in]) {
(void) fprintf(el->el_outfile,
"%s\t->\t%ls\n", outbuf, bp->name);
return;
}
} else
keymacro_print(el, in);
}





static void
map_print_some_keys(EditLine *el, el_action_t *map, wint_t first, wint_t last)
{
el_bindings_t *bp, *ep;
wchar_t firstbuf[2], lastbuf[2];
char unparsbuf[EL_BUFSIZ], extrabuf[EL_BUFSIZ];

firstbuf[0] = first;
firstbuf[1] = 0;
lastbuf[0] = last;
lastbuf[1] = 0;
if (map[first] == ED_UNASSIGNED) {
if (first == last) {
(void) keymacro__decode_str(firstbuf, unparsbuf,
sizeof(unparsbuf), STRQQ);
(void) fprintf(el->el_outfile,
"%-15s-> is undefined\n", unparsbuf);
}
return;
}
ep = &el->el_map.help[el->el_map.nfunc];
for (bp = el->el_map.help; bp < ep; bp++) {
if (bp->func == map[first]) {
if (first == last) {
(void) keymacro__decode_str(firstbuf, unparsbuf,
sizeof(unparsbuf), STRQQ);
(void) fprintf(el->el_outfile, "%-15s-> %ls\n",
unparsbuf, bp->name);
} else {
(void) keymacro__decode_str(firstbuf, unparsbuf,
sizeof(unparsbuf), STRQQ);
(void) keymacro__decode_str(lastbuf, extrabuf,
sizeof(extrabuf), STRQQ);
(void) fprintf(el->el_outfile,
"%-4s to %-7s-> %ls\n",
unparsbuf, extrabuf, bp->name);
}
return;
}
}
#if defined(MAP_DEBUG)
if (map == el->el_map.key) {
(void) keymacro__decode_str(firstbuf, unparsbuf,
sizeof(unparsbuf), STRQQ);
(void) fprintf(el->el_outfile,
"BUG!!! %s isn't bound to anything.\n", unparsbuf);
(void) fprintf(el->el_outfile, "el->el_map.key[%d] == %d\n",
first, el->el_map.key[first]);
} else {
(void) keymacro__decode_str(firstbuf, unparsbuf,
sizeof(unparsbuf), STRQQ);
(void) fprintf(el->el_outfile,
"BUG!!! %s isn't bound to anything.\n", unparsbuf);
(void) fprintf(el->el_outfile, "el->el_map.alt[%d] == %d\n",
first, el->el_map.alt[first]);
}
#endif
EL_ABORT((el->el_errfile, "Error printing keys\n"));
}





static void
map_print_all_keys(EditLine *el)
{
int prev, i;

(void) fprintf(el->el_outfile, "Standard key bindings\n");
prev = 0;
for (i = 0; i < N_KEYS; i++) {
if (el->el_map.key[prev] == el->el_map.key[i])
continue;
map_print_some_keys(el, el->el_map.key, prev, i - 1);
prev = i;
}
map_print_some_keys(el, el->el_map.key, prev, i - 1);

(void) fprintf(el->el_outfile, "Alternative key bindings\n");
prev = 0;
for (i = 0; i < N_KEYS; i++) {
if (el->el_map.alt[prev] == el->el_map.alt[i])
continue;
map_print_some_keys(el, el->el_map.alt, prev, i - 1);
prev = i;
}
map_print_some_keys(el, el->el_map.alt, prev, i - 1);

(void) fprintf(el->el_outfile, "Multi-character bindings\n");
keymacro_print(el, L"");
(void) fprintf(el->el_outfile, "Arrow key bindings\n");
terminal_print_arrow(el, L"");
}





libedit_private int
map_bind(EditLine *el, int argc, const wchar_t **argv)
{
el_action_t *map;
int ntype, rem;
const wchar_t *p;
wchar_t inbuf[EL_BUFSIZ];
wchar_t outbuf[EL_BUFSIZ];
const wchar_t *in = NULL;
wchar_t *out;
el_bindings_t *bp, *ep;
int cmd;
int key;

if (argv == NULL)
return -1;

map = el->el_map.key;
ntype = XK_CMD;
key = rem = 0;
for (argc = 1; (p = argv[argc]) != NULL; argc++)
if (p[0] == '-')
switch (p[1]) {
case 'a':
map = el->el_map.alt;
break;

case 's':
ntype = XK_STR;
break;
case 'k':
key = 1;
break;

case 'r':
rem = 1;
break;

case 'v':
map_init_vi(el);
return 0;

case 'e':
map_init_emacs(el);
return 0;

case 'l':
ep = &el->el_map.help[el->el_map.nfunc];
for (bp = el->el_map.help; bp < ep; bp++)
(void) fprintf(el->el_outfile,
"%ls\n\t%ls\n",
bp->name, bp->description);
return 0;
default:
(void) fprintf(el->el_errfile,
"%ls: Invalid switch `%lc'.\n",
argv[0], (wint_t)p[1]);
}
else
break;

if (argv[argc] == NULL) {
map_print_all_keys(el);
return 0;
}
if (key)
in = argv[argc++];
else if ((in = parse__string(inbuf, argv[argc++])) == NULL) {
(void) fprintf(el->el_errfile,
"%ls: Invalid \\ or ^ in instring.\n",
argv[0]);
return -1;
}
if (rem) {
if (key) {
(void) terminal_clear_arrow(el, in);
return -1;
}
if (in[1])
(void) keymacro_delete(el, in);
else if (map[(unsigned char) *in] == ED_SEQUENCE_LEAD_IN)
(void) keymacro_delete(el, in);
else
map[(unsigned char) *in] = ED_UNASSIGNED;
return 0;
}
if (argv[argc] == NULL) {
if (key)
terminal_print_arrow(el, in);
else
map_print_key(el, map, in);
return 0;
}
#if defined(notyet)
if (argv[argc + 1] != NULL) {
bindkeymacro_usage();
return -1;
}
#endif

switch (ntype) {
case XK_STR:
if ((out = parse__string(outbuf, argv[argc])) == NULL) {
(void) fprintf(el->el_errfile,
"%ls: Invalid \\ or ^ in outstring.\n", argv[0]);
return -1;
}
if (key)
terminal_set_arrow(el, in, keymacro_map_str(el, out), ntype);
else
keymacro_add(el, in, keymacro_map_str(el, out), ntype);
map[(unsigned char) *in] = ED_SEQUENCE_LEAD_IN;
break;

case XK_CMD:
if ((cmd = parse_cmd(el, argv[argc])) == -1) {
(void) fprintf(el->el_errfile,
"%ls: Invalid command `%ls'.\n",
argv[0], argv[argc]);
return -1;
}
if (key)
terminal_set_arrow(el, in, keymacro_map_cmd(el, cmd), ntype);
else {
if (in[1]) {
keymacro_add(el, in, keymacro_map_cmd(el, cmd), ntype);
map[(unsigned char) *in] = ED_SEQUENCE_LEAD_IN;
} else {
keymacro_clear(el, map, in);
map[(unsigned char) *in] = (el_action_t)cmd;
}
}
break;


default:
EL_ABORT((el->el_errfile, "Bad XK_ type %d\n", ntype));
break;
}
return 0;
}





libedit_private int
map_addfunc(EditLine *el, const wchar_t *name, const wchar_t *help,
el_func_t func)
{
void *p;
size_t nf = el->el_map.nfunc + 1;

if (name == NULL || help == NULL || func == NULL)
return -1;

if ((p = el_realloc(el->el_map.func, nf *
sizeof(*el->el_map.func))) == NULL)
return -1;
el->el_map.func = p;
if ((p = el_realloc(el->el_map.help, nf * sizeof(*el->el_map.help)))
== NULL)
return -1;
el->el_map.help = p;

nf = (size_t)el->el_map.nfunc;
el->el_map.func[nf] = func;

el->el_map.help[nf].name = name;
el->el_map.help[nf].func = (int)nf;
el->el_map.help[nf].description = help;
el->el_map.nfunc++;

return 0;
}
