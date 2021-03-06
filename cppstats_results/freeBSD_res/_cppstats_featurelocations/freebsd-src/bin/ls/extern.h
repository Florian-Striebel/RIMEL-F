

































#include <stdbool.h>

int acccmp(const FTSENT *, const FTSENT *);
int revacccmp(const FTSENT *, const FTSENT *);
int birthcmp(const FTSENT *, const FTSENT *);
int revbirthcmp(const FTSENT *, const FTSENT *);
int modcmp(const FTSENT *, const FTSENT *);
int revmodcmp(const FTSENT *, const FTSENT *);
int namecmp(const FTSENT *, const FTSENT *);
int revnamecmp(const FTSENT *, const FTSENT *);
int statcmp(const FTSENT *, const FTSENT *);
int revstatcmp(const FTSENT *, const FTSENT *);
int sizecmp(const FTSENT *, const FTSENT *);
int revsizecmp(const FTSENT *, const FTSENT *);

void printcol(const DISPLAY *);
void printlong(const DISPLAY *);
int printname(const char *);
void printscol(const DISPLAY *);
void printstream(const DISPLAY *);
void usage(void);
int prn_normal(const char *);
size_t len_octal(const char *, int);
int prn_octal(const char *);
int prn_printable(const char *);
#if defined(COLORLS)
void parsecolors(const char *cs);
void colorquit(int);

extern char *ansi_fgcol;
extern char *ansi_bgcol;
extern char *ansi_coloff;
extern char *attrs_off;
extern char *enter_bold;
extern char *enter_underline;

extern int colorflag;
extern bool explicitansi;

#define COLORFLAG_NEVER 0
#define COLORFLAG_AUTO 1
#define COLORFLAG_ALWAYS 2
#endif
extern int termwidth;
