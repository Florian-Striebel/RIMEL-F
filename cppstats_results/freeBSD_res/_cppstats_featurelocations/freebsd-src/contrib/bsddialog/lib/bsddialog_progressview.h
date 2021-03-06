


























#if !defined(_LIBBSDDIALOG_PROGRESSVIEW_H_)
#define _LIBBSDDIALOG_PROGRESSVIEW_H_

extern bool bsddialog_interruptprogview;
extern bool bsddialog_abortprogview;
extern int bsddialog_total_progview;

struct bsddialog_fileminibar {
char *path;
char *label;
int status;
long long size;
long long read;
};

struct bsddialog_progviewconf {
char *fmtbottomstr;
unsigned int refresh;
int (*callback)(struct bsddialog_fileminibar *minibar);
};

int
bsddialog_progressview (struct bsddialog_conf *conf, char * text, int rows,
int cols, struct bsddialog_progviewconf *pvconf, unsigned int nminibar,
struct bsddialog_fileminibar *minibar);

#endif
