




































extern volatile sig_atomic_t pendingsig;
extern volatile sig_atomic_t pendingsig_waitcmd;

void clear_traps(void);
int have_traps(void);
void setsignal(int);
void ignoresig(int);
int issigchldtrapped(void);
void onsig(int);
void dotrap(void);
void trap_init(void);
void setinteractive(void);
void exitshell(int) __dead2;
void exitshell_savedstatus(void) __dead2;
