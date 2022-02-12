


































#if !defined(BC_VM_H)
#define BC_VM_H

#include <assert.h>
#include <stddef.h>
#include <limits.h>

#include <signal.h>

#if BC_ENABLE_NLS

#if defined(_WIN32)
#error NLS is not supported on Windows.
#endif

#include <nl_types.h>

#endif

#include <version.h>
#include <status.h>
#include <num.h>
#include <lex.h>
#include <parse.h>
#include <program.h>
#include <history.h>
#include <bc.h>


#if !BC_ENABLE_LIBRARY
#include <file.h>
#endif


#if !BC_ENABLED && !DC_ENABLED
#error Must define BC_ENABLED, DC_ENABLED, or both
#endif



#if CHAR_BIT < 6
#error CHAR_BIT must be at least 6.
#endif



#if !defined(BC_ENABLE_NLS)
#define BC_ENABLE_NLS (0)
#endif

#if !defined(MAINEXEC)
#define MAINEXEC bc
#endif

#if !defined(_WIN32)
#if !defined(EXECPREFIX)
#define EXECPREFIX
#endif
#else
#undef EXECPREFIX
#endif





#define GEN_STR(V) #V






#define GEN_STR2(V) GEN_STR(V)



#define BC_VERSION GEN_STR2(VERSION)



#define BC_MAINEXEC GEN_STR2(MAINEXEC)



#define BC_BUILD_TYPE GEN_STR2(BUILD_TYPE)


#if !defined(_WIN32)
#define BC_EXECPREFIX GEN_STR2(EXECPREFIX)
#else
#define BC_EXECPREFIX ""
#endif

#if !BC_ENABLE_LIBRARY

#if DC_ENABLED


#define DC_FLAG_X (UINTMAX_C(1)<<0)

#endif

#if BC_ENABLED


#define BC_FLAG_W (UINTMAX_C(1)<<1)


#define BC_FLAG_S (UINTMAX_C(1)<<2)


#define BC_FLAG_L (UINTMAX_C(1)<<3)


#define BC_FLAG_G (UINTMAX_C(1)<<4)

#endif


#define BC_FLAG_Q (UINTMAX_C(1)<<5)


#define BC_FLAG_I (UINTMAX_C(1)<<6)


#define BC_FLAG_P (UINTMAX_C(1)<<7)


#define BC_FLAG_R (UINTMAX_C(1)<<8)


#define BC_FLAG_Z (UINTMAX_C(1)<<9)


#define BC_FLAG_TTYIN (UINTMAX_C(1)<<10)


#define BC_FLAG_TTY (UINTMAX_C(1)<<11)


#define BC_FLAG_SIGINT (UINTMAX_C(1)<<12)


#define BC_FLAG_EXPR_EXIT (UINTMAX_C(1)<<13)


#define BC_TTYIN (vm.flags & BC_FLAG_TTYIN)


#define BC_TTY (vm.flags & BC_FLAG_TTY)


#define BC_SIGINT (vm.flags & BC_FLAG_SIGINT)

#if BC_ENABLED


#define BC_S (vm.flags & BC_FLAG_S)


#define BC_W (vm.flags & BC_FLAG_W)


#define BC_L (vm.flags & BC_FLAG_L)


#define BC_G (vm.flags & BC_FLAG_G)

#endif

#if DC_ENABLED


#define DC_X (vm.flags & DC_FLAG_X)

#endif


#define BC_I (vm.flags & BC_FLAG_I)


#define BC_P (vm.flags & BC_FLAG_P)


#define BC_R (vm.flags & BC_FLAG_R)


#define BC_Z (vm.flags & BC_FLAG_Z)


#define BC_EXPR_EXIT (vm.flags & BC_FLAG_EXPR_EXIT)

#if BC_ENABLED


#define BC_IS_POSIX (BC_S || BC_W)

#if DC_ENABLED


#define BC_IS_BC (vm.name[0] != 'd')


#define BC_IS_DC (vm.name[0] == 'd')

#else


#define BC_IS_BC (1)


#define BC_IS_DC (0)

#endif

#else


#define BC_IS_POSIX (0)


#define BC_IS_BC (0)


#define BC_IS_DC (1)

#endif


#define BC_PROMPT (BC_P)

#else

#define BC_Z (vm.leading_zeroes)

#endif








#define BC_MAX(a, b) ((a) > (b) ? (a) : (b))








#define BC_MIN(a, b) ((a) < (b) ? (a) : (b))


#define BC_MAX_OBASE ((BcBigDig) (BC_BASE_POW))


#define BC_MAX_DIM ((BcBigDig) (SIZE_MAX - 1))


#define BC_MAX_SCALE ((BcBigDig) (BC_NUM_BIGDIG_MAX - 1))


#define BC_MAX_STRING ((BcBigDig) (BC_NUM_BIGDIG_MAX - 1))


#define BC_MAX_NAME BC_MAX_STRING


#define BC_MAX_NUM BC_MAX_SCALE

#if BC_ENABLE_EXTRA_MATH


#define BC_MAX_RAND ((BcBigDig) (((BcRand) 0) - 1))

#endif


#define BC_MAX_EXP ((ulong) (BC_NUM_BIGDIG_MAX))


#define BC_MAX_VARS ((ulong) (SIZE_MAX - 1))


#define BC_VM_BUF_SIZE (1<<12)


#define BC_VM_STDOUT_BUF_SIZE (1<<11)


#define BC_VM_STDERR_BUF_SIZE (1<<10)


#define BC_VM_STDIN_BUF_SIZE (BC_VM_STDERR_BUF_SIZE - 1)


#define BC_VM_MAX_TEMPS (1 << 9)


#define BC_VM_ONE_CAP (1)






#define BC_VM_SAFE_RESULT(r) ((r)->t >= BC_RESULT_TEMP)


#define BC_VM_INVALID_CATALOG ((nl_catd) -1)








#define BC_VM_MUL_OVERFLOW(a, b, r) ((r) >= SIZE_MAX || ((a) != 0 && (r) / (a) != (b)))




typedef struct BcVm {





volatile sig_atomic_t status;






volatile sig_atomic_t sig_pop;

#if !BC_ENABLE_LIBRARY


BcParse prs;


BcProgram prog;


BcVec line_buf;



BcVec buffer;


BcParse read_prs;


BcVec read_buf;

#endif



BcVec jmp_bufs;


size_t temps_len;

#if BC_ENABLE_LIBRARY


BcVec ctxts;


BcVec out;


BcRNG rng;


BclError err;


bool abrt;


bool leading_zeroes;


unsigned int refs;





volatile sig_atomic_t running;

#endif

#if !BC_ENABLE_LIBRARY



const char* file;


const char *sigmsg;

#endif





volatile sig_atomic_t sig_lock;






volatile sig_atomic_t sig;

#if !BC_ENABLE_LIBRARY


uchar siglen;


uchar read_ret;


uint16_t flags;




uint16_t nchars;


uint16_t line_len;



bool no_exprs;


bool exit_exprs;


bool eof;


bool is_stdin;

#if BC_ENABLED



bool no_redefine;

#endif

#endif


BcBigDig maxes[BC_PROG_GLOBALS_LEN + BC_ENABLE_EXTRA_MATH];

#if !BC_ENABLE_LIBRARY


BcVec files;


BcVec exprs;



const char *name;


const char *help;

#if BC_ENABLE_HISTORY


BcHistory history;

#endif


BcLexNext next;


BcParseParse parse;


BcParseExpr expr;


const char *func_header;


const char *err_ids[BC_ERR_IDX_NELEMS + BC_ENABLED];


const char *err_msgs[BC_ERR_NELEMS];


const char *locale;

#endif


BcBigDig last_base;


BcBigDig last_pow;


BcBigDig last_exp;


BcBigDig last_rem;

#if !BC_ENABLE_LIBRARY



char *env_args_buffer;


BcVec env_args;


BcNum zero;

#endif


BcNum one;


BcNum max;


BcNum max2;


BcDig max_num[BC_NUM_BIGDIG_LOG10];


BcDig max2_num[BC_NUM_BIGDIG_LOG10];


BcDig one_num[BC_VM_ONE_CAP];

#if !BC_ENABLE_LIBRARY


BcDig zero_num[BC_VM_ONE_CAP];


BcFile fout;


BcFile ferr;

#if BC_ENABLE_NLS


nl_catd catalog;

#endif


char *buf;


size_t buf_len;



BcVec main_const_slab;


BcVec main_slabs;



BcVec other_slabs;

#if BC_ENABLED



bool redefined_kws[BC_LEX_NKWS];

#endif
#endif

#if BC_DEBUG_CODE


size_t func_depth;

#endif

} BcVm;





void bc_vm_info(const char* const help);






void bc_vm_boot(int argc, char *argv[]);





void bc_vm_init(void);




void bc_vm_shutdown(void);





void bc_vm_addTemp(BcDig *num);





BcDig* bc_vm_takeTemp(void);




void bc_vm_freeTemps(void);

#if !BC_ENABLE_HISTORY





#define bc_vm_putchar(c, t) bc_vm_putchar(c)

#endif





void bc_vm_printf(const char *fmt, ...);






void bc_vm_putchar(int c, BcFlushType type);








size_t bc_vm_arraySize(size_t n, size_t size);







size_t bc_vm_growSize(size_t a, size_t b);






void* bc_vm_malloc(size_t n);








void* bc_vm_realloc(void *ptr, size_t n);






char* bc_vm_strdup(const char *str);






bool bc_vm_readLine(bool clear);






void bc_pledge(const char *promises, const char *execpromises);






char* bc_vm_getenv(const char* var);





void bc_vm_getenvFree(char* val);

#if BC_DEBUG_CODE





void bc_vm_jmp(const char *f);
#else




void bc_vm_jmp(void);

#endif

#if BC_ENABLE_LIBRARY







void bc_vm_handleError(BcErr e);





void bc_vm_fatalError(BcErr e);




void bc_vm_atexit(void);

#else








void bc_vm_handleError(BcErr e, size_t line, ...);





#if !BC_ENABLE_MEMCHECK
BC_NORETURN
#endif
void bc_vm_fatalError(BcErr e);





int bc_vm_atexit(int status);
#endif


extern const char bc_copyright[];


extern const char* const bc_err_line;


extern const char* const bc_err_func_header;


extern const char *bc_errs[];


extern const uchar bc_err_ids[];


extern const char* const bc_err_msgs[];


extern const char bc_pledge_start[];

#if BC_ENABLE_HISTORY


extern const char bc_pledge_end_history[];

#endif


extern const char bc_pledge_end[];


extern BcVm vm;


extern char output_bufs[BC_VM_BUF_SIZE];

#endif
