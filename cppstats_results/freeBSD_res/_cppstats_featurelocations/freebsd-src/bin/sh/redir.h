





































#define REDIR_PUSH 01
#define REDIR_BACKQ 02

union node;
void redirect(union node *, int);
void popredir(void);
int fd0_redirected_p(void);
void clearredir(void);

