






















#if !defined(DLG_KEYS_H_included)
#define DLG_KEYS_H_included 1


#include <dialog.h>

#if defined(USE_WIDE_CURSES)
#include <wctype.h>
#define dlg_toupper(ch) towupper((wint_t)ch)
#define dlg_isupper(ch) iswupper((wint_t)ch)
#else
#define dlg_toupper(ch) (((ch) > 0 && (ch) <= 255) ? toupper(ch) : (ch))
#define dlg_isupper(ch) (isalpha(ch) && isupper(ch))
#endif

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct {
int is_function_key;
int curses_key;
int dialog_key;
} DLG_KEYS_BINDING;

#define DLG_KEYS_DATA(dialog, curses) { (curses) >= KEY_MIN, curses, dialog }

#define END_KEYS_BINDING { -1, 0, 0 }




typedef enum {
DLGK_MIN = KEY_MAX + 1,

DLGK_OK,
DLGK_CANCEL,
DLGK_EXTRA,
DLGK_HELP,
DLGK_ESC,

DLGK_PAGE_FIRST,
DLGK_PAGE_LAST,
DLGK_PAGE_NEXT,
DLGK_PAGE_PREV,

DLGK_ITEM_FIRST,
DLGK_ITEM_LAST,
DLGK_ITEM_NEXT,
DLGK_ITEM_PREV,

DLGK_FIELD_FIRST,
DLGK_FIELD_LAST,
DLGK_FIELD_NEXT,
DLGK_FIELD_PREV,

DLGK_FORM_FIRST,
DLGK_FORM_LAST,
DLGK_FORM_NEXT,
DLGK_FORM_PREV,

DLGK_GRID_UP,
DLGK_GRID_DOWN,
DLGK_GRID_LEFT,
DLGK_GRID_RIGHT,

DLGK_DELETE_LEFT,
DLGK_DELETE_RIGHT,
DLGK_DELETE_ALL,

DLGK_ENTER,
DLGK_BEGIN,
DLGK_FINAL,
DLGK_SELECT,
DLGK_HELPFILE,
DLGK_TRACE,
DLGK_TOGGLE,
DLGK_LEAVE
} DLG_KEYS_ENUM;

#define is_DLGK_MOUSE(code) ((code) >= M_EVENT)
#define DLGK_MOUSE(code) ((code) + M_EVENT)

#define HELPKEY_BINDINGS DLG_KEYS_DATA( DLGK_HELPFILE, CHR_HELP ), DLG_KEYS_DATA( DLGK_HELPFILE, KEY_F(1) ), DLG_KEYS_DATA( DLGK_HELPFILE, KEY_HELP )




#define ENTERKEY_BINDINGS DLG_KEYS_DATA( DLGK_ENTER, '\n' ), DLG_KEYS_DATA( DLGK_ENTER, '\r' ), DLG_KEYS_DATA( DLGK_ENTER, KEY_ENTER ), DLG_KEYS_DATA( DLGK_LEAVE, CHR_LEAVE )






#define INPUTSTR_BINDINGS DLG_KEYS_DATA( DLGK_BEGIN, KEY_HOME ), DLG_KEYS_DATA( DLGK_DELETE_ALL, CHR_KILL ), DLG_KEYS_DATA( DLGK_DELETE_LEFT, CHR_BACKSPACE ), DLG_KEYS_DATA( DLGK_DELETE_LEFT, KEY_BACKSPACE ), DLG_KEYS_DATA( DLGK_DELETE_RIGHT, CHR_DELETE ), DLG_KEYS_DATA( DLGK_DELETE_RIGHT, KEY_DC ), DLG_KEYS_DATA( DLGK_FINAL, KEY_END ), DLG_KEYS_DATA( DLGK_GRID_LEFT, KEY_LEFT ), DLG_KEYS_DATA( DLGK_GRID_RIGHT, KEY_RIGHT )










#define SCROLL_FKEY_BINDINGS DLG_KEYS_DATA( DLGK_GRID_DOWN, KEY_DOWN ), DLG_KEYS_DATA( DLGK_GRID_UP, KEY_UP ), DLG_KEYS_DATA( DLGK_PAGE_FIRST, KEY_HOME ), DLG_KEYS_DATA( DLGK_PAGE_LAST, KEY_END ), DLG_KEYS_DATA( DLGK_PAGE_NEXT, KEY_NPAGE ), DLG_KEYS_DATA( DLGK_PAGE_PREV, KEY_PPAGE )







#define SCROLLKEY_BINDINGS SCROLL_FKEY_BINDINGS, DLG_KEYS_DATA( DLGK_GRID_DOWN, 'J' ), DLG_KEYS_DATA( DLGK_GRID_DOWN, 'j' ), DLG_KEYS_DATA( DLGK_GRID_UP, 'K' ), DLG_KEYS_DATA( DLGK_GRID_UP, 'k' ), DLG_KEYS_DATA( DLGK_PAGE_FIRST, 'g' ), DLG_KEYS_DATA( DLGK_PAGE_LAST, 'G' ), DLG_KEYS_DATA( DLGK_PAGE_NEXT, 'F' ), DLG_KEYS_DATA( DLGK_PAGE_NEXT, 'f' ), DLG_KEYS_DATA( DLGK_PAGE_PREV, 'B' ), DLG_KEYS_DATA( DLGK_PAGE_PREV, 'b' )












#define TRAVERSE_BINDINGS DLG_KEYS_DATA( DLGK_ENTER, CHR_SPACE ), DLG_KEYS_DATA( DLGK_FIELD_NEXT, KEY_DOWN ), DLG_KEYS_DATA( DLGK_FIELD_NEXT, KEY_RIGHT ), DLG_KEYS_DATA( DLGK_FIELD_NEXT, TAB ), DLG_KEYS_DATA( DLGK_FIELD_PREV, KEY_UP ), DLG_KEYS_DATA( DLGK_FIELD_PREV, KEY_BTAB ), DLG_KEYS_DATA( DLGK_FIELD_PREV, KEY_LEFT )








#define TOGGLEKEY_BINDINGS DLG_KEYS_DATA( DLGK_TOGGLE, CHR_SPACE )


extern int dlg_button_key(int , int * , int * , int * );
extern int dlg_lookup_key(WINDOW * , int , int * );
extern int dlg_ok_button_key(int , int * , int * , int * );
extern int dlg_result_key(int , int , int * );
extern void dlg_register_buttons(WINDOW * , const char * , const char ** );
extern void dlg_register_window(WINDOW * , const char * , DLG_KEYS_BINDING * );
extern void dlg_unregister_window(WINDOW * );

#if defined(HAVE_RC_FILE)
extern int dlg_parse_bindkey(char * );
extern void dlg_dump_keys(FILE * );
extern void dlg_dump_window_keys(FILE * , WINDOW * );
#endif

#if defined(__cplusplus)
}
#endif


#endif
