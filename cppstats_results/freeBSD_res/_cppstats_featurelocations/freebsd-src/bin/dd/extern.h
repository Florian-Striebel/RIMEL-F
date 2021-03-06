





































void block(void);
void block_close(void);
void dd_out(int);
void def(void);
void def_close(void);
void jcl(char **);
void pos_in(void);
void pos_out(void);
double secs_elapsed(void);
void progress(void);
void summary(void);
void sigalarm_handler(int);
void siginfo_handler(int);
void terminate(int);
void unblock(void);
void unblock_close(void);

extern IO in, out;
extern STAT st;
extern void (*cfunc)(void);
extern uintmax_t cpy_cnt;
extern size_t cbsz;
extern uint64_t ddflags;
extern size_t speed;
extern uintmax_t files_cnt;
extern const u_char *ctab;
extern const u_char a2e_32V[], a2e_POSIX[];
extern const u_char e2a_32V[], e2a_POSIX[];
extern const u_char a2ibm_32V[], a2ibm_POSIX[];
extern u_char casetab[];
extern char fill_char;
extern volatile sig_atomic_t need_summary;
extern volatile sig_atomic_t need_progress;
