


























#if !defined(_LIBBSDDIALOG_THEME_H_)
#define _LIBBSDDIALOG_THEME_H_


struct bsddialog_theme {
struct {
int color;
} terminal;
struct {
int color;
unsigned int h;
unsigned int w;
} shadow;
struct {
int color;
bool delimtitle;
int titlecolor;
int lineraisecolor;
int linelowercolor;
int bottomtitlecolor;
} dialog;
struct {
unsigned int hmargin;
} text;
struct {
int arrowcolor;
int selectorcolor;
int f_namecolor;
int namecolor;
int f_desccolor;
int desccolor;
int namesepcolor;
int descsepcolor;
int f_shortcutcolor;
int shortcutcolor;
} menu;
struct {
int f_fieldcolor;
int fieldcolor;
int readonlycolor;
} form;
struct {
int f_color;
int color;
} bar;
struct {
unsigned int space;
int leftch;
int rightch;
int delimcolor;
int f_delimcolor;
int color;
int f_color;
int shortcutcolor;
int f_shortcutcolor;
} button;
};

enum bsddialog_default_theme {
BSDDIALOG_THEME_BLACKWHITE,
BSDDIALOG_THEME_BSDDIALOG,
BSDDIALOG_THEME_DEFAULT,
BSDDIALOG_THEME_DIALOG,
};

enum bsddialog_color {
BSDDIALOG_BLACK = 0,
BSDDIALOG_RED,
BSDDIALOG_GREEN,
BSDDIALOG_YELLOW,
BSDDIALOG_BLUE,
BSDDIALOG_MAGENTA,
BSDDIALOG_CYAN,
BSDDIALOG_WHITE,
};

#define BSDDIALOG_BOLD 1U
#define BSDDIALOG_REVERSE 2U
#define BSDDIALOG_UNDERLINE 4U

int
bsddialog_color(enum bsddialog_color foreground,
enum bsddialog_color background, unsigned int flags);
int bsddialog_get_theme(struct bsddialog_theme *theme);
int bsddialog_set_default_theme(enum bsddialog_default_theme theme);
int bsddialog_set_theme(struct bsddialog_theme *theme);

#endif
