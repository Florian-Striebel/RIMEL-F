













































































#if !defined(BC_HISTORY_H)
#define BC_HISTORY_H

#if !defined(BC_ENABLE_HISTORY)
#define BC_ENABLE_HISTORY (1)
#endif

#if BC_ENABLE_HISTORY

#include <stdbool.h>
#include <stddef.h>

#include <signal.h>

#if !defined(_WIN32)
#include <termios.h>
#include <time.h>
#include <unistd.h>
#include <sys/select.h>
#else

#if !defined(WIN32_LEAN_AND_MEAN)
#define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>
#include <io.h>
#include <conio.h>

#define strncasecmp _strnicmp
#define strcasecmp _stricmp

#endif

#include <status.h>
#include <vector.h>
#include <read.h>

#if BC_DEBUG_CODE
#include <file.h>
#endif


#define BC_HIST_DEF_COLS (80)


#define BC_HIST_MAX_LEN (128)


#define BC_HIST_MAX_LINE (4095)


#define BC_HIST_SEQ_SIZE (64)





#define BC_HIST_BUF_LEN(h) ((h)->buf.len - 1)







#define BC_HIST_READ(s, n) (bc_history_read((s), (n)) == -1)


#define BC_HIST_NEXT (false)
#define BC_HIST_PREV (true)

#if BC_DEBUG_CODE



#define BC_HISTORY_DEBUG_BUF_SIZE (1024)

#define lndebug(...) do { if (bc_history_debug_fp.fd == 0) { bc_history_debug_buf = bc_vm_malloc(BC_HISTORY_DEBUG_BUF_SIZE); bc_file_init(&bc_history_debug_fp, open("/tmp/lndebug.txt", O_APPEND), BC_HISTORY_DEBUG_BUF_SIZE); bc_file_printf(&bc_history_debug_fp, "[%zu %zu %zu] p: %d, rows: %d, " "rpos: %d, max: %zu, oldmax: %d\n", l->len, l->pos, l->oldcolpos, plen, rows, rpos, l->maxrows, old_rows); } bc_file_printf(&bc_history_debug_fp, ", " __VA_ARGS__); bc_file_flush(&bc_history_debug_fp); } while (0)















#else
#define lndebug(fmt, ...)
#endif



typedef enum BcHistoryAction {

BC_ACTION_NULL = 0,
BC_ACTION_CTRL_A = 1,
BC_ACTION_CTRL_B = 2,
BC_ACTION_CTRL_C = 3,
BC_ACTION_CTRL_D = 4,
BC_ACTION_CTRL_E = 5,
BC_ACTION_CTRL_F = 6,
BC_ACTION_CTRL_H = 8,
BC_ACTION_TAB = 9,
BC_ACTION_LINE_FEED = 10,
BC_ACTION_CTRL_K = 11,
BC_ACTION_CTRL_L = 12,
BC_ACTION_ENTER = 13,
BC_ACTION_CTRL_N = 14,
BC_ACTION_CTRL_P = 16,
BC_ACTION_CTRL_S = 19,
BC_ACTION_CTRL_T = 20,
BC_ACTION_CTRL_U = 21,
BC_ACTION_CTRL_W = 23,
BC_ACTION_CTRL_Z = 26,
BC_ACTION_ESC = 27,
BC_ACTION_CTRL_BSLASH = 28,
BC_ACTION_BACKSPACE = 127

} BcHistoryAction;





typedef struct BcHistory {


BcVec buf;


BcVec history;


BcVec extras;


const char *prompt;


size_t plen;


size_t pcol;


size_t pos;


size_t oldcolpos;


size_t cols;


size_t idx;

#if !defined(_WIN32)

struct termios orig_termios;
#else

DWORD orig_in;


DWORD orig_out;
#endif




bool rawMode;


bool badTerm;

#if !defined(_WIN32)

fd_set rdset;


struct timespec ts;


sigset_t sigmask;
#endif

} BcHistory;












BcStatus bc_history_line(BcHistory *h, BcVec *vec, const char *prompt);





void bc_history_init(BcHistory *h);





void bc_history_free(BcHistory *h);





void bc_history_string_free(void *str);


extern const char *bc_history_bad_terms[];


extern const char bc_history_tab[];
extern const size_t bc_history_tab_len;


extern const char bc_history_ctrlc[];


extern const uint32_t bc_history_wchars[][2];
extern const size_t bc_history_wchars_len;
extern const uint32_t bc_history_combo_chars[];
extern const size_t bc_history_combo_chars_len;

#if BC_DEBUG_CODE


extern BcFile bc_history_debug_fp;
extern char *bc_history_debug_buf;





void bc_history_printKeyCodes(BcHistory* h);

#endif

#endif

#endif
