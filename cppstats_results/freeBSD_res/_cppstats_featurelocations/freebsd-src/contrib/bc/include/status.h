


































#if !defined(BC_STATUS_H)
#define BC_STATUS_H

#include <stdint.h>


#if defined(BC_TEST_OPENBSD)
#if defined(__OpenBSD__)
#error On OpenBSD without _BSD_SOURCE
#endif
#endif

#if !defined(BC_ENABLED)
#define BC_ENABLED (1)
#endif

#if !defined(DC_ENABLED)
#define DC_ENABLED (1)
#endif

#if !defined(BC_ENABLE_LIBRARY)
#define BC_ENABLE_LIBRARY (0)
#endif


#if BC_ENABLE_AFL
#if !defined(__AFL_HAVE_MANUAL_CONTROL)
#error Must compile with afl-clang-fast or afl-clang-lto for fuzzing
#endif
#endif

#if !defined(BC_ENABLE_MEMCHECK)
#define BC_ENABLE_MEMCHECK (0)
#endif





#define BC_UNUSED(e) ((void) (e))



#if !defined(BC_LIKELY)





#define BC_LIKELY(e) (e)

#endif



#if !defined(BC_UNLIKELY)





#define BC_UNLIKELY(e) (e)

#endif





#define BC_ERR(e) BC_UNLIKELY(e)





#define BC_NO_ERR(s) BC_LIKELY(s)


#if !defined(BC_DEBUG_CODE)
#define BC_DEBUG_CODE (0)
#endif


#if __STDC_VERSION__ >= 201100L

#include <stdnoreturn.h>
#define BC_NORETURN _Noreturn
#define BC_C11 (1)

#else

#define BC_NORETURN
#define BC_MUST_RETURN
#define BC_C11 (0)

#endif

#define BC_HAS_UNREACHABLE (0)
#define BC_HAS_COMPUTED_GOTO (0)




#if defined(__clang__) || defined(__GNUC__)

#if defined(__has_attribute)

#if __has_attribute(fallthrough)
#define BC_FALLTHROUGH __attribute__((fallthrough));
#else
#define BC_FALLTHROUGH
#endif

#if defined(__GNUC__)

#if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 5)
#undef BC_HAS_UNREACHABLE
#define BC_HAS_UNREACHABLE (1)
#endif

#else

#if __clang_major__ >= 4
#undef BC_HAS_UNREACHABLE
#define BC_HAS_UNREACHABLE (1)
#endif

#endif

#else
#define BC_FALLTHROUGH
#endif
#else
#define BC_FALLTHROUGH
#endif

#if BC_HAS_UNREACHABLE

#define BC_UNREACHABLE __builtin_unreachable();

#else

#if defined(_WIN32)

#define BC_UNREACHABLE __assume(0);

#else

#define BC_UNREACHABLE

#endif

#endif

#if defined(__GNUC__)

#if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 5)

#undef BC_HAS_COMPUTED_GOTO
#define BC_HAS_COMPUTED_GOTO (1)

#endif

#endif

#if defined(__clang__)

#if __clang_major__ >= 4

#undef BC_HAS_COMPUTED_GOTO
#define BC_HAS_COMPUTED_GOTO (1)

#endif

#endif

#if defined(BC_NO_COMPUTED_GOTO)

#undef BC_HAS_COMPUTED_GOTO
#define BC_HAS_COMPUTED_GOTO (0)

#endif

#if defined(__GNUC__)
#if defined(__OpenBSD__)

#define inline
#endif
#endif


#if !defined(SIZE_MAX)
#define SIZE_MAX __SIZE_MAX__
#endif
#if !defined(UINTMAX_C)
#define UINTMAX_C __UINTMAX_C
#endif
#if !defined(UINT32_C)
#define UINT32_C __UINT32_C
#endif
#if !defined(UINT_FAST32_MAX)
#define UINT_FAST32_MAX __UINT_FAST32_MAX__
#endif
#if !defined(UINT16_MAX)
#define UINT16_MAX __UINT16_MAX__
#endif
#if !defined(SIG_ATOMIC_MAX)
#define SIG_ATOMIC_MAX __SIG_ATOMIC_MAX__
#endif


#include <bcl.h>



#if BC_ENABLED

#if !defined(BC_DEFAULT_BANNER)
#define BC_DEFAULT_BANNER (0)
#endif

#endif

#if !defined(BC_DEFAULT_SIGINT_RESET)
#define BC_DEFAULT_SIGINT_RESET (1)
#endif

#if !defined(BC_DEFAULT_TTY_MODE)
#define BC_DEFAULT_TTY_MODE (1)
#endif

#if !defined(BC_DEFAULT_PROMPT)
#define BC_DEFAULT_PROMPT BC_DEFAULT_TTY_MODE
#endif

#if !defined(BC_DEFAULT_EXPR_EXIT)
#define BC_DEFAULT_EXPR_EXIT (1)
#endif


#if !defined(DC_DEFAULT_SIGINT_RESET)
#define DC_DEFAULT_SIGINT_RESET (1)
#endif

#if !defined(DC_DEFAULT_TTY_MODE)
#define DC_DEFAULT_TTY_MODE (0)
#endif

#if !defined(DC_DEFAULT_HISTORY)
#define DC_DEFAULT_HISTORY DC_DEFAULT_TTY_MODE
#endif

#if !defined(DC_DEFAULT_PROMPT)
#define DC_DEFAULT_PROMPT DC_DEFAULT_TTY_MODE
#endif

#if !defined(DC_DEFAULT_EXPR_EXIT)
#define DC_DEFAULT_EXPR_EXIT (1)
#endif



typedef enum BcStatus {


BC_STATUS_SUCCESS = 0,


BC_STATUS_ERROR_MATH,


BC_STATUS_ERROR_PARSE,


BC_STATUS_ERROR_EXEC,


BC_STATUS_ERROR_FATAL,


BC_STATUS_EOF,


BC_STATUS_QUIT,

} BcStatus;


typedef enum BcErr {




BC_ERR_MATH_NEGATIVE,


BC_ERR_MATH_NON_INTEGER,


BC_ERR_MATH_OVERFLOW,


BC_ERR_MATH_DIVIDE_BY_ZERO,




BC_ERR_FATAL_ALLOC_ERR,


BC_ERR_FATAL_IO_ERR,


BC_ERR_FATAL_FILE_ERR,


BC_ERR_FATAL_BIN_FILE,


BC_ERR_FATAL_PATH_DIR,


BC_ERR_FATAL_OPTION,


BC_ERR_FATAL_OPTION_NO_ARG,


BC_ERR_FATAL_OPTION_ARG,


BC_ERR_FATAL_ARG,




BC_ERR_EXEC_IBASE,


BC_ERR_EXEC_OBASE,


BC_ERR_EXEC_SCALE,


BC_ERR_EXEC_READ_EXPR,


BC_ERR_EXEC_REC_READ,


BC_ERR_EXEC_TYPE,


BC_ERR_EXEC_STACK,


BC_ERR_EXEC_STACK_REGISTER,


BC_ERR_EXEC_PARAMS,


BC_ERR_EXEC_UNDEF_FUNC,


BC_ERR_EXEC_VOID_VAL,




BC_ERR_PARSE_EOF,


BC_ERR_PARSE_CHAR,


BC_ERR_PARSE_STRING,


BC_ERR_PARSE_COMMENT,


BC_ERR_PARSE_TOKEN,

#if BC_ENABLED


BC_ERR_PARSE_EXPR,


BC_ERR_PARSE_EMPTY_EXPR,


BC_ERR_PARSE_PRINT,


BC_ERR_PARSE_FUNC,


BC_ERR_PARSE_ASSIGN,


BC_ERR_PARSE_NO_AUTO,


BC_ERR_PARSE_DUP_LOCAL,


BC_ERR_PARSE_BLOCK,


BC_ERR_PARSE_RET_VOID,


BC_ERR_PARSE_REF_VAR,




BC_ERR_POSIX_NAME_LEN,


BC_ERR_POSIX_COMMENT,


BC_ERR_POSIX_KW,


BC_ERR_POSIX_DOT,


BC_ERR_POSIX_RET,


BC_ERR_POSIX_BOOL,


BC_ERR_POSIX_REL_POS,



BC_ERR_POSIX_MULTIREL,


BC_ERR_POSIX_FOR,



BC_ERR_POSIX_FUNC_AFTER_SEMICOLON,


BC_ERR_POSIX_EXP_NUM,


BC_ERR_POSIX_REF,


BC_ERR_POSIX_VOID,


BC_ERR_POSIX_BRACE,


BC_ERR_POSIX_EXPR_STRING,

#endif


BC_ERR_NELEMS,

#if BC_ENABLED


BC_ERR_POSIX_START = BC_ERR_POSIX_NAME_LEN,


BC_ERR_POSIX_END = BC_ERR_POSIX_EXPR_STRING,

#endif

} BcErr;





#define BC_ERR_IDX_MATH (0)


#define BC_ERR_IDX_PARSE (1)


#define BC_ERR_IDX_EXEC (2)


#define BC_ERR_IDX_FATAL (3)


#define BC_ERR_IDX_NELEMS (4)


#if BC_ENABLED


#define BC_ERR_IDX_WARN (BC_ERR_IDX_NELEMS)

#endif




#if BC_DEBUG_CODE
#define BC_JMP bc_vm_jmp(__func__)
#else
#define BC_JMP bc_vm_jmp()
#endif


#define BC_SIG_EXC BC_UNLIKELY(vm.status != (sig_atomic_t) BC_STATUS_SUCCESS || vm.sig)



#define BC_NO_SIG_EXC BC_LIKELY(vm.status == (sig_atomic_t) BC_STATUS_SUCCESS && !vm.sig)


#if !defined(NDEBUG)





#define BC_SIG_ASSERT_LOCKED do { assert(vm.sig_lock); } while (0)





#define BC_SIG_ASSERT_NOT_LOCKED do { assert(vm.sig_lock == 0); } while (0)

#else





#define BC_SIG_ASSERT_LOCKED





#define BC_SIG_ASSERT_NOT_LOCKED

#endif


#define BC_SIG_LOCK do { BC_SIG_ASSERT_NOT_LOCKED; vm.sig_lock = 1; } while (0)






#define BC_SIG_UNLOCK do { BC_SIG_ASSERT_LOCKED; vm.sig_lock = 0; if (vm.sig) BC_JMP; } while (0)










#define BC_SIG_MAYLOCK do { vm.sig_lock = 1; } while (0)






#define BC_SIG_MAYUNLOCK do { vm.sig_lock = 0; if (vm.sig) BC_JMP; } while (0)










#define BC_SIG_TRYLOCK(v) do { v = vm.sig_lock; vm.sig_lock = 1; } while (0)









#define BC_SIG_TRYUNLOCK(v) do { vm.sig_lock = (v); if (!(v) && vm.sig) BC_JMP; } while (0)















#define BC_SETJMP(l) do { sigjmp_buf sjb; BC_SIG_LOCK; bc_vec_grow(&vm.jmp_bufs, 1); if (sigsetjmp(sjb, 0)) { assert(BC_SIG_EXC); goto l; } bc_vec_push(&vm.jmp_bufs, &sjb); BC_SIG_UNLOCK; } while (0)



















#define BC_SETJMP_LOCKED(l) do { sigjmp_buf sjb; BC_SIG_ASSERT_LOCKED; if (sigsetjmp(sjb, 0)) { assert(BC_SIG_EXC); goto l; } bc_vec_push(&vm.jmp_bufs, &sjb); } while (0)














#define BC_LONGJMP_CONT do { BC_SIG_ASSERT_LOCKED; if (!vm.sig_pop) bc_vec_pop(&vm.jmp_bufs); vm.sig_lock = 0; if (BC_SIG_EXC) BC_JMP; } while (0)











#define BC_UNSETJMP do { BC_SIG_ASSERT_LOCKED; bc_vec_pop(&vm.jmp_bufs); } while (0)








#define BC_LONGJMP_STOP do { vm.sig_pop = 0; vm.sig = 0; } while (0)






#if BC_ENABLE_LIBRARY







#define bc_error(e, l, ...) (bc_vm_handleError((e)))





#define bc_err(e) (bc_vm_handleError((e)))





#define bc_verr(e, ...) (bc_vm_handleError((e)))

#else







#define bc_error(e, l, ...) (bc_vm_handleError((e), (l), __VA_ARGS__))





#define bc_err(e) (bc_vm_handleError((e), 0))





#define bc_verr(e, ...) (bc_vm_handleError((e), 0, __VA_ARGS__))

#endif






#define BC_STATUS_IS_ERROR(s) ((s) >= BC_STATUS_ERROR_MATH && (s) <= BC_STATUS_ERROR_FATAL)




#if BC_DEBUG_CODE
#define BC_FUNC_ENTER do { size_t bc_func_enter_i; for (bc_func_enter_i = 0; bc_func_enter_i < vm.func_depth; ++bc_func_enter_i) { bc_file_puts(&vm.ferr, bc_flush_none, " "); } vm.func_depth += 1; bc_file_printf(&vm.ferr, "Entering %s\n", __func__); bc_file_flush(&vm.ferr, bc_flush_none); } while (0);












#define BC_FUNC_EXIT do { size_t bc_func_enter_i; vm.func_depth -= 1; for (bc_func_enter_i = 0; bc_func_enter_i < vm.func_depth; ++bc_func_enter_i) { bc_file_puts(&vm.ferr, bc_flush_none, " "); } bc_file_printf(&vm.ferr, "Leaving %s\n", __func__); bc_file_flush(&vm.ferr, bc_flush_none); } while (0);











#else
#define BC_FUNC_ENTER
#define BC_FUNC_EXIT
#endif

#endif
