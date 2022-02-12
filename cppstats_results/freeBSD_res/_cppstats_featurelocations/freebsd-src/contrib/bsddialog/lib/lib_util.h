


























#if !defined(_LIBBSDDIALOG_UTIL_H_)
#define _LIBBSDDIALOG_UTIL_H_





#define HBORDER 1
#define HBORDERS (HBORDER + HBORDER)
#define VBORDER 1
#define VBORDERS (VBORDER + VBORDER)
#define PADDING(p) (p)


#define KEY_CTRL(x) ((x) & 0x1f)


#define GET_ASPECT_RATIO(conf) (conf->aspect_ratio > 0 ? conf->aspect_ratio : 9)


#define BSDDIALOG_DEBUG(y,x,fmt, ...) do { mvprintw(y, x, fmt, __VA_ARGS__); refresh(); } while (0)





const char *get_error_string(void);
void set_error_string(char *string);

#define RETURN_ERROR(str) do { set_error_string(str); return BSDDIALOG_ERROR; } while (0)





#define LABEL_cancel_label "Cancel"
#define LABEL_exit_label "EXIT"
#define LABEL_extra_label "Extra"
#define LABEL_help_label "Help"
#define LABEL_ok_label "OK"
#define BUTTONLABEL(l) (conf->button.l != NULL ? conf->button.l : LABEL_ ##l)

#define MAXBUTTONS 6
struct buttons {
unsigned int nbuttons;
char *label[MAXBUTTONS];
int value[MAXBUTTONS];
int curr;
unsigned int sizebutton;
};

void
get_buttons(struct bsddialog_conf *conf, struct buttons *bs, char *yesoklabel,
char *extralabel, char *nocancellabel, char *helplabel);

void
draw_button(WINDOW *window, int y, int x, int size, char *text, bool selected,
bool shortkey);

void
draw_buttons(WINDOW *window, int y, int cols, struct buttons bs, bool shortkey);


int f1help(struct bsddialog_conf *conf);


int hide_widget(int y, int x, int h, int w, bool withshadow);


int
get_text_properties(struct bsddialog_conf *conf, char *text, int *maxword,
int *maxline, int *nlines);

int widget_max_height(struct bsddialog_conf *conf);
int widget_max_width(struct bsddialog_conf *conf);

int
set_widget_size(struct bsddialog_conf *conf, int rows, int cols, int *h, int *w);

int
set_widget_position(struct bsddialog_conf *conf, int *y, int *x, int h, int w);


int
print_textpad(struct bsddialog_conf *conf, WINDOW *pad, int *rows, int cols,
char *text);

enum elevation { RAISED, LOWERED };

void
draw_borders(struct bsddialog_conf *conf, WINDOW *win, int rows, int cols,
enum elevation elev);

WINDOW *
new_boxed_window(struct bsddialog_conf *conf, int y, int x, int rows, int cols,
enum elevation elev);

int
new_widget_withtextpad(struct bsddialog_conf *conf, WINDOW **shadow,
WINDOW **widget, int y, int x, int h, int w, enum elevation elev,
WINDOW **textpad, int *htextpad, char *text, bool buttons);

int
update_widget_withtextpad(struct bsddialog_conf *conf, WINDOW *shadow,
WINDOW *widget, int h, int w, enum elevation elev, WINDOW *textpad,
int *htextpad, char *text, bool buttons);

void
end_widget_withtextpad(struct bsddialog_conf *conf, WINDOW *window, int h, int w,
WINDOW *textpad, WINDOW *shadow);

#endif
