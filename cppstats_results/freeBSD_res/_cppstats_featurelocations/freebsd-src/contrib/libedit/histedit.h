






































#if !defined(_HISTEDIT_H_)
#define _HISTEDIT_H_

#define LIBEDIT_MAJOR 2
#define LIBEDIT_MINOR 11

#include <sys/types.h>
#include <stdio.h>

#if defined(__cplusplus)
extern "C" {
#endif





typedef struct editline EditLine;




typedef struct lineinfo {
const char *buffer;
const char *cursor;
const char *lastchar;
} LineInfo;





#define CC_NORM 0
#define CC_NEWLINE 1
#define CC_EOF 2
#define CC_ARGHACK 3
#define CC_REFRESH 4
#define CC_CURSOR 5
#define CC_ERROR 6
#define CC_FATAL 7
#define CC_REDISPLAY 8
#define CC_REFRESH_BEEP 9




EditLine *el_init(const char *, FILE *, FILE *, FILE *);
EditLine *el_init_fd(const char *, FILE *, FILE *, FILE *,
int, int, int);
void el_end(EditLine *);
void el_reset(EditLine *);




const char *el_gets(EditLine *, int *);
int el_getc(EditLine *, char *);
void el_push(EditLine *, const char *);




void el_beep(EditLine *);





int el_parse(EditLine *, int, const char **);




int el_set(EditLine *, int, ...);
int el_get(EditLine *, int, ...);
unsigned char _el_fn_complete(EditLine *, int);
unsigned char _el_fn_sh_complete(EditLine *, int);


















#define EL_PROMPT 0
#define EL_TERMINAL 1
#define EL_EDITOR 2
#define EL_SIGNAL 3
#define EL_BIND 4
#define EL_TELLTC 5
#define EL_SETTC 6
#define EL_ECHOTC 7
#define EL_SETTY 8
#define EL_ADDFN 9

#define EL_HIST 10
#define EL_EDITMODE 11
#define EL_RPROMPT 12
#define EL_GETCFN 13
#define EL_CLIENTDATA 14
#define EL_UNBUFFERED 15
#define EL_PREP_TERM 16
#define EL_GETTC 17
#define EL_GETFP 18
#define EL_SETFP 19
#define EL_REFRESH 20
#define EL_PROMPT_ESC 21
#define EL_RPROMPT_ESC 22
#define EL_RESIZE 23
#define EL_ALIAS_TEXT 24
#define EL_SAFEREAD 25

#define EL_BUILTIN_GETCFN (NULL)




int el_source(EditLine *, const char *);






void el_resize(EditLine *);




const LineInfo *el_line(EditLine *);
int el_insertstr(EditLine *, const char *);
void el_deletestr(EditLine *, int);






typedef struct history History;

typedef struct HistEvent {
int num;
const char *str;
} HistEvent;




History * history_init(void);
void history_end(History *);

int history(History *, HistEvent *, int, ...);

#define H_FUNC 0
#define H_SETSIZE 1
#define H_GETSIZE 2
#define H_FIRST 3
#define H_LAST 4
#define H_PREV 5
#define H_NEXT 6
#define H_CURR 8
#define H_SET 7
#define H_ADD 9
#define H_ENTER 10
#define H_APPEND 11
#define H_END 12
#define H_NEXT_STR 13
#define H_PREV_STR 14
#define H_NEXT_EVENT 15
#define H_PREV_EVENT 16
#define H_LOAD 17
#define H_SAVE 18
#define H_CLEAR 19
#define H_SETUNIQUE 20
#define H_GETUNIQUE 21
#define H_DEL 22
#define H_NEXT_EVDATA 23
#define H_DELDATA 24
#define H_REPLACE 25
#define H_SAVE_FP 26
#define H_NSAVE_FP 27







typedef struct tokenizer Tokenizer;




Tokenizer *tok_init(const char *);
void tok_end(Tokenizer *);
void tok_reset(Tokenizer *);
int tok_line(Tokenizer *, const LineInfo *,
int *, const char ***, int *, int *);
int tok_str(Tokenizer *, const char *,
int *, const char ***);




#include <wchar.h>
#include <wctype.h>




typedef struct lineinfow {
const wchar_t *buffer;
const wchar_t *cursor;
const wchar_t *lastchar;
} LineInfoW;

typedef int (*el_rfunc_t)(EditLine *, wchar_t *);

const wchar_t *el_wgets(EditLine *, int *);
int el_wgetc(EditLine *, wchar_t *);
void el_wpush(EditLine *, const wchar_t *);

int el_wparse(EditLine *, int, const wchar_t **);

int el_wset(EditLine *, int, ...);
int el_wget(EditLine *, int, ...);

int el_cursor(EditLine *, int);
const LineInfoW *el_wline(EditLine *);
int el_winsertstr(EditLine *, const wchar_t *);
#define el_wdeletestr el_deletestr




typedef struct histeventW {
int num;
const wchar_t *str;
} HistEventW;

typedef struct historyW HistoryW;

HistoryW * history_winit(void);
void history_wend(HistoryW *);

int history_w(HistoryW *, HistEventW *, int, ...);




typedef struct tokenizerW TokenizerW;


TokenizerW *tok_winit(const wchar_t *);
void tok_wend(TokenizerW *);
void tok_wreset(TokenizerW *);
int tok_wline(TokenizerW *, const LineInfoW *,
int *, const wchar_t ***, int *, int *);
int tok_wstr(TokenizerW *, const wchar_t *,
int *, const wchar_t ***);

#if defined(__cplusplus)
}
#endif

#endif
