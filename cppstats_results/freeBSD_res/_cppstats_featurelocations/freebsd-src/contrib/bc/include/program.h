


































#if !defined(BC_PROGRAM_H)
#define BC_PROGRAM_H

#include <assert.h>
#include <stddef.h>

#include <status.h>
#include <parse.h>
#include <lang.h>
#include <num.h>
#include <rand.h>


#define BC_PROG_GLOBALS_IBASE (0)


#define BC_PROG_GLOBALS_OBASE (1)


#define BC_PROG_GLOBALS_SCALE (2)

#if BC_ENABLE_EXTRA_MATH


#define BC_PROG_MAX_RAND (3)

#endif


#define BC_PROG_GLOBALS_LEN (3 + BC_ENABLE_EXTRA_MATH)

typedef struct BcProgram {


BcBigDig globals[BC_PROG_GLOBALS_LEN];


BcVec globals_v[BC_PROG_GLOBALS_LEN];

#if BC_ENABLE_EXTRA_MATH


BcRNG rng;

#endif


BcVec results;


BcVec stack;


BcVec *consts;


BcVec *strs;


BcVec fns;


BcVec fn_map;


BcVec vars;


BcVec var_map;


BcVec arrs;


BcVec arr_map;

#if DC_ENABLED





BcVec tail_calls;

#endif


BcNum strmb;

#if BC_ENABLED


BcNum last;

#endif




BcDig strmb_num[BC_NUM_BIGDIG_LOG10];

} BcProgram;







#define BC_PROG_STACK(s, n) ((s)->len >= ((size_t) (n)))






#define BC_PROG_GLOBAL_PTR(v) (bc_vec_top(v))






#define BC_PROG_GLOBAL(v) (*((BcBigDig*) BC_PROG_GLOBAL_PTR(v)))






#define BC_PROG_IBASE(p) ((p)->globals[BC_PROG_GLOBALS_IBASE])






#define BC_PROG_OBASE(p) ((p)->globals[BC_PROG_GLOBALS_OBASE])






#define BC_PROG_SCALE(p) ((p)->globals[BC_PROG_GLOBALS_SCALE])


#define BC_PROG_MAIN (0)


#define BC_PROG_READ (1)










#define bc_program_retire(p, nres, nops) (bc_vec_npopAt(&(p)->results, (nops), (p)->results.len - (nres + nops)))


#if DC_ENABLED


#define BC_PROG_REQ_FUNCS (2)

#if !BC_ENABLED



#define bc_program_copyToVar(p, name, t, last) bc_program_copyToVar(p, name, t)


#endif

#else



#define bc_program_pushVar(p, code, bgn, pop, copy) bc_program_pushVar(p, code, bgn)





#if defined(NDEBUG)
#define BC_PROG_NO_STACK_CHECK
#endif

#endif






#define BC_PROG_STR(n) ((n)->num == NULL && !(n)->cap)

#if BC_ENABLED







#define BC_PROG_NUM(r, n) ((r)->t != BC_RESULT_ARRAY && (r)->t != BC_RESULT_STR && !BC_PROG_STR(n))


#else







#define BC_PROG_NUM(r, n) ((r)->t != BC_RESULT_STR && !BC_PROG_STR(n))

#endif







typedef void (*BcProgramUnary)(BcResult *r, BcNum *n);





void bc_program_init(BcProgram *p);

#if !defined(NDEBUG)







void bc_program_free(BcProgram *p);

#endif

#if BC_DEBUG_CODE
#if BC_ENABLED && DC_ENABLED





void bc_program_code(const BcProgram *p);








void bc_program_printInst(const BcProgram *p, const char *code,
size_t *restrict bgn);





void bc_program_printStackDebug(BcProgram* p);

#endif
#endif








size_t bc_program_search(BcProgram *p, const char* id, bool var);







size_t bc_program_addString(BcProgram *p, const char *str, size_t fidx);








size_t bc_program_insertFunc(BcProgram *p, const char *name);





void bc_program_reset(BcProgram *p);





void bc_program_exec(BcProgram *p);






void bc_program_negate(BcResult *r, BcNum *n);






void bc_program_not(BcResult *r, BcNum *n);

#if BC_ENABLE_EXTRA_MATH






void bc_program_trunc(BcResult *r, BcNum *n);

#endif


extern const BcNumBinaryOp bc_program_ops[];


extern const BcNumBinaryOpReq bc_program_opReqs[];


extern const BcProgramUnary bc_program_unarys[];


extern const char bc_program_exprs_name[];


extern const char bc_program_stdin_name[];


extern const char bc_program_ready_msg[];


extern const size_t bc_program_ready_msg_len;


extern const char bc_program_esc_chars[];



extern const char bc_program_esc_seqs[];

#if BC_HAS_COMPUTED_GOTO

#if BC_DEBUG_CODE

#define BC_PROG_JUMP(inst, code, ip) do { inst = (uchar) (code)[(ip)->idx++]; bc_file_printf(&vm.ferr, "inst: %s\n", bc_inst_names[inst]); bc_file_flush(&vm.ferr, bc_flush_none); goto *bc_program_inst_lbls[inst]; } while (0)







#else

#define BC_PROG_JUMP(inst, code, ip) do { inst = (uchar) (code)[(ip)->idx++]; goto *bc_program_inst_lbls[inst]; } while (0)





#endif

#define BC_PROG_DIRECT_JUMP(l) goto lbl_ ##l;
#define BC_PROG_LBL(l) lbl_ ##l
#define BC_PROG_FALLTHROUGH

#if BC_C11

#define BC_PROG_LBLS_SIZE (sizeof(bc_program_inst_lbls) / sizeof(void*))
#define BC_PROG_LBLS_ASSERT static_assert(BC_PROG_LBLS_SIZE == BC_INST_INVALID + 1,"bc_program_inst_lbls[] mismatches the instructions")



#else

#define BC_PROG_LBLS_ASSERT

#endif

#if BC_ENABLED

#if DC_ENABLED

#if BC_ENABLE_EXTRA_MATH

#define BC_PROG_LBLS static const void* const bc_program_inst_lbls[] = { &&lbl_BC_INST_INC, &&lbl_BC_INST_DEC, &&lbl_BC_INST_NEG, &&lbl_BC_INST_BOOL_NOT, &&lbl_BC_INST_TRUNC, &&lbl_BC_INST_POWER, &&lbl_BC_INST_MULTIPLY, &&lbl_BC_INST_DIVIDE, &&lbl_BC_INST_MODULUS, &&lbl_BC_INST_PLUS, &&lbl_BC_INST_MINUS, &&lbl_BC_INST_PLACES, &&lbl_BC_INST_LSHIFT, &&lbl_BC_INST_RSHIFT, &&lbl_BC_INST_REL_EQ, &&lbl_BC_INST_REL_LE, &&lbl_BC_INST_REL_GE, &&lbl_BC_INST_REL_NE, &&lbl_BC_INST_REL_LT, &&lbl_BC_INST_REL_GT, &&lbl_BC_INST_BOOL_OR, &&lbl_BC_INST_BOOL_AND, &&lbl_BC_INST_ASSIGN_POWER, &&lbl_BC_INST_ASSIGN_MULTIPLY, &&lbl_BC_INST_ASSIGN_DIVIDE, &&lbl_BC_INST_ASSIGN_MODULUS, &&lbl_BC_INST_ASSIGN_PLUS, &&lbl_BC_INST_ASSIGN_MINUS, &&lbl_BC_INST_ASSIGN_PLACES, &&lbl_BC_INST_ASSIGN_LSHIFT, &&lbl_BC_INST_ASSIGN_RSHIFT, &&lbl_BC_INST_ASSIGN, &&lbl_BC_INST_ASSIGN_POWER_NO_VAL, &&lbl_BC_INST_ASSIGN_MULTIPLY_NO_VAL, &&lbl_BC_INST_ASSIGN_DIVIDE_NO_VAL, &&lbl_BC_INST_ASSIGN_MODULUS_NO_VAL, &&lbl_BC_INST_ASSIGN_PLUS_NO_VAL, &&lbl_BC_INST_ASSIGN_MINUS_NO_VAL, &&lbl_BC_INST_ASSIGN_PLACES_NO_VAL, &&lbl_BC_INST_ASSIGN_LSHIFT_NO_VAL, &&lbl_BC_INST_ASSIGN_RSHIFT_NO_VAL, &&lbl_BC_INST_ASSIGN_NO_VAL, &&lbl_BC_INST_NUM, &&lbl_BC_INST_VAR, &&lbl_BC_INST_ARRAY_ELEM, &&lbl_BC_INST_ARRAY, &&lbl_BC_INST_ZERO, &&lbl_BC_INST_ONE, &&lbl_BC_INST_LAST, &&lbl_BC_INST_IBASE, &&lbl_BC_INST_OBASE, &&lbl_BC_INST_SCALE, &&lbl_BC_INST_SEED, &&lbl_BC_INST_LENGTH, &&lbl_BC_INST_SCALE_FUNC, &&lbl_BC_INST_SQRT, &&lbl_BC_INST_ABS, &&lbl_BC_INST_IRAND, &&lbl_BC_INST_ASCIIFY, &&lbl_BC_INST_READ, &&lbl_BC_INST_RAND, &&lbl_BC_INST_MAXIBASE, &&lbl_BC_INST_MAXOBASE, &&lbl_BC_INST_MAXSCALE, &&lbl_BC_INST_MAXRAND, &&lbl_BC_INST_LINE_LENGTH, &&lbl_BC_INST_GLOBAL_STACKS, &&lbl_BC_INST_LEADING_ZERO, &&lbl_BC_INST_PRINT, &&lbl_BC_INST_PRINT_POP, &&lbl_BC_INST_STR, &&lbl_BC_INST_PRINT_STR, &&lbl_BC_INST_JUMP, &&lbl_BC_INST_JUMP_ZERO, &&lbl_BC_INST_CALL, &&lbl_BC_INST_RET, &&lbl_BC_INST_RET0, &&lbl_BC_INST_RET_VOID, &&lbl_BC_INST_HALT, &&lbl_BC_INST_POP, &&lbl_BC_INST_SWAP, &&lbl_BC_INST_MODEXP, &&lbl_BC_INST_DIVMOD, &&lbl_BC_INST_PRINT_STREAM, &&lbl_BC_INST_POP_EXEC, &&lbl_BC_INST_EXECUTE, &&lbl_BC_INST_EXEC_COND, &&lbl_BC_INST_PRINT_STACK, &&lbl_BC_INST_CLEAR_STACK, &&lbl_BC_INST_REG_STACK_LEN, &&lbl_BC_INST_STACK_LEN, &&lbl_BC_INST_DUPLICATE, &&lbl_BC_INST_LOAD, &&lbl_BC_INST_PUSH_VAR, &&lbl_BC_INST_PUSH_TO_VAR, &&lbl_BC_INST_QUIT, &&lbl_BC_INST_NQUIT, &&lbl_BC_INST_EXEC_STACK_LEN, &&lbl_BC_INST_INVALID, }





































































































#else

#define BC_PROG_LBLS static const void* const bc_program_inst_lbls[] = { &&lbl_BC_INST_INC, &&lbl_BC_INST_DEC, &&lbl_BC_INST_NEG, &&lbl_BC_INST_BOOL_NOT, &&lbl_BC_INST_POWER, &&lbl_BC_INST_MULTIPLY, &&lbl_BC_INST_DIVIDE, &&lbl_BC_INST_MODULUS, &&lbl_BC_INST_PLUS, &&lbl_BC_INST_MINUS, &&lbl_BC_INST_REL_EQ, &&lbl_BC_INST_REL_LE, &&lbl_BC_INST_REL_GE, &&lbl_BC_INST_REL_NE, &&lbl_BC_INST_REL_LT, &&lbl_BC_INST_REL_GT, &&lbl_BC_INST_BOOL_OR, &&lbl_BC_INST_BOOL_AND, &&lbl_BC_INST_ASSIGN_POWER, &&lbl_BC_INST_ASSIGN_MULTIPLY, &&lbl_BC_INST_ASSIGN_DIVIDE, &&lbl_BC_INST_ASSIGN_MODULUS, &&lbl_BC_INST_ASSIGN_PLUS, &&lbl_BC_INST_ASSIGN_MINUS, &&lbl_BC_INST_ASSIGN, &&lbl_BC_INST_ASSIGN_POWER_NO_VAL, &&lbl_BC_INST_ASSIGN_MULTIPLY_NO_VAL, &&lbl_BC_INST_ASSIGN_DIVIDE_NO_VAL, &&lbl_BC_INST_ASSIGN_MODULUS_NO_VAL, &&lbl_BC_INST_ASSIGN_PLUS_NO_VAL, &&lbl_BC_INST_ASSIGN_MINUS_NO_VAL, &&lbl_BC_INST_ASSIGN_NO_VAL, &&lbl_BC_INST_NUM, &&lbl_BC_INST_VAR, &&lbl_BC_INST_ARRAY_ELEM, &&lbl_BC_INST_ARRAY, &&lbl_BC_INST_ZERO, &&lbl_BC_INST_ONE, &&lbl_BC_INST_LAST, &&lbl_BC_INST_IBASE, &&lbl_BC_INST_OBASE, &&lbl_BC_INST_SCALE, &&lbl_BC_INST_LENGTH, &&lbl_BC_INST_SCALE_FUNC, &&lbl_BC_INST_SQRT, &&lbl_BC_INST_ABS, &&lbl_BC_INST_ASCIIFY, &&lbl_BC_INST_READ, &&lbl_BC_INST_MAXIBASE, &&lbl_BC_INST_MAXOBASE, &&lbl_BC_INST_MAXSCALE, &&lbl_BC_INST_LINE_LENGTH, &&lbl_BC_INST_GLOBAL_STACKS, &&lbl_BC_INST_LEADING_ZERO, &&lbl_BC_INST_PRINT, &&lbl_BC_INST_PRINT_POP, &&lbl_BC_INST_STR, &&lbl_BC_INST_PRINT_STR, &&lbl_BC_INST_JUMP, &&lbl_BC_INST_JUMP_ZERO, &&lbl_BC_INST_CALL, &&lbl_BC_INST_RET, &&lbl_BC_INST_RET0, &&lbl_BC_INST_RET_VOID, &&lbl_BC_INST_HALT, &&lbl_BC_INST_POP, &&lbl_BC_INST_SWAP, &&lbl_BC_INST_MODEXP, &&lbl_BC_INST_DIVMOD, &&lbl_BC_INST_PRINT_STREAM, &&lbl_BC_INST_POP_EXEC, &&lbl_BC_INST_EXECUTE, &&lbl_BC_INST_EXEC_COND, &&lbl_BC_INST_PRINT_STACK, &&lbl_BC_INST_CLEAR_STACK, &&lbl_BC_INST_REG_STACK_LEN, &&lbl_BC_INST_STACK_LEN, &&lbl_BC_INST_DUPLICATE, &&lbl_BC_INST_LOAD, &&lbl_BC_INST_PUSH_VAR, &&lbl_BC_INST_PUSH_TO_VAR, &&lbl_BC_INST_QUIT, &&lbl_BC_INST_NQUIT, &&lbl_BC_INST_EXEC_STACK_LEN, &&lbl_BC_INST_INVALID, }























































































#endif

#else

#if BC_ENABLE_EXTRA_MATH

#define BC_PROG_LBLS static const void* const bc_program_inst_lbls[] = { &&lbl_BC_INST_INC, &&lbl_BC_INST_DEC, &&lbl_BC_INST_NEG, &&lbl_BC_INST_BOOL_NOT, &&lbl_BC_INST_TRUNC, &&lbl_BC_INST_POWER, &&lbl_BC_INST_MULTIPLY, &&lbl_BC_INST_DIVIDE, &&lbl_BC_INST_MODULUS, &&lbl_BC_INST_PLUS, &&lbl_BC_INST_MINUS, &&lbl_BC_INST_PLACES, &&lbl_BC_INST_LSHIFT, &&lbl_BC_INST_RSHIFT, &&lbl_BC_INST_REL_EQ, &&lbl_BC_INST_REL_LE, &&lbl_BC_INST_REL_GE, &&lbl_BC_INST_REL_NE, &&lbl_BC_INST_REL_LT, &&lbl_BC_INST_REL_GT, &&lbl_BC_INST_BOOL_OR, &&lbl_BC_INST_BOOL_AND, &&lbl_BC_INST_ASSIGN_POWER, &&lbl_BC_INST_ASSIGN_MULTIPLY, &&lbl_BC_INST_ASSIGN_DIVIDE, &&lbl_BC_INST_ASSIGN_MODULUS, &&lbl_BC_INST_ASSIGN_PLUS, &&lbl_BC_INST_ASSIGN_MINUS, &&lbl_BC_INST_ASSIGN_PLACES, &&lbl_BC_INST_ASSIGN_LSHIFT, &&lbl_BC_INST_ASSIGN_RSHIFT, &&lbl_BC_INST_ASSIGN, &&lbl_BC_INST_ASSIGN_POWER_NO_VAL, &&lbl_BC_INST_ASSIGN_MULTIPLY_NO_VAL, &&lbl_BC_INST_ASSIGN_DIVIDE_NO_VAL, &&lbl_BC_INST_ASSIGN_MODULUS_NO_VAL, &&lbl_BC_INST_ASSIGN_PLUS_NO_VAL, &&lbl_BC_INST_ASSIGN_MINUS_NO_VAL, &&lbl_BC_INST_ASSIGN_PLACES_NO_VAL, &&lbl_BC_INST_ASSIGN_LSHIFT_NO_VAL, &&lbl_BC_INST_ASSIGN_RSHIFT_NO_VAL, &&lbl_BC_INST_ASSIGN_NO_VAL, &&lbl_BC_INST_NUM, &&lbl_BC_INST_VAR, &&lbl_BC_INST_ARRAY_ELEM, &&lbl_BC_INST_ARRAY, &&lbl_BC_INST_ZERO, &&lbl_BC_INST_ONE, &&lbl_BC_INST_LAST, &&lbl_BC_INST_IBASE, &&lbl_BC_INST_OBASE, &&lbl_BC_INST_SCALE, &&lbl_BC_INST_SEED, &&lbl_BC_INST_LENGTH, &&lbl_BC_INST_SCALE_FUNC, &&lbl_BC_INST_SQRT, &&lbl_BC_INST_ABS, &&lbl_BC_INST_IRAND, &&lbl_BC_INST_ASCIIFY, &&lbl_BC_INST_READ, &&lbl_BC_INST_RAND, &&lbl_BC_INST_MAXIBASE, &&lbl_BC_INST_MAXOBASE, &&lbl_BC_INST_MAXSCALE, &&lbl_BC_INST_MAXRAND, &&lbl_BC_INST_LINE_LENGTH, &&lbl_BC_INST_GLOBAL_STACKS, &&lbl_BC_INST_LEADING_ZERO, &&lbl_BC_INST_PRINT, &&lbl_BC_INST_PRINT_POP, &&lbl_BC_INST_STR, &&lbl_BC_INST_PRINT_STR, &&lbl_BC_INST_JUMP, &&lbl_BC_INST_JUMP_ZERO, &&lbl_BC_INST_CALL, &&lbl_BC_INST_RET, &&lbl_BC_INST_RET0, &&lbl_BC_INST_RET_VOID, &&lbl_BC_INST_HALT, &&lbl_BC_INST_POP, &&lbl_BC_INST_SWAP, &&lbl_BC_INST_MODEXP, &&lbl_BC_INST_DIVMOD, &&lbl_BC_INST_PRINT_STREAM, &&lbl_BC_INST_INVALID, }























































































#else

#define BC_PROG_LBLS static const void* const bc_program_inst_lbls[] = { &&lbl_BC_INST_INC, &&lbl_BC_INST_DEC, &&lbl_BC_INST_NEG, &&lbl_BC_INST_BOOL_NOT, &&lbl_BC_INST_POWER, &&lbl_BC_INST_MULTIPLY, &&lbl_BC_INST_DIVIDE, &&lbl_BC_INST_MODULUS, &&lbl_BC_INST_PLUS, &&lbl_BC_INST_MINUS, &&lbl_BC_INST_REL_EQ, &&lbl_BC_INST_REL_LE, &&lbl_BC_INST_REL_GE, &&lbl_BC_INST_REL_NE, &&lbl_BC_INST_REL_LT, &&lbl_BC_INST_REL_GT, &&lbl_BC_INST_BOOL_OR, &&lbl_BC_INST_BOOL_AND, &&lbl_BC_INST_ASSIGN_POWER, &&lbl_BC_INST_ASSIGN_MULTIPLY, &&lbl_BC_INST_ASSIGN_DIVIDE, &&lbl_BC_INST_ASSIGN_MODULUS, &&lbl_BC_INST_ASSIGN_PLUS, &&lbl_BC_INST_ASSIGN_MINUS, &&lbl_BC_INST_ASSIGN, &&lbl_BC_INST_ASSIGN_POWER_NO_VAL, &&lbl_BC_INST_ASSIGN_MULTIPLY_NO_VAL, &&lbl_BC_INST_ASSIGN_DIVIDE_NO_VAL, &&lbl_BC_INST_ASSIGN_MODULUS_NO_VAL, &&lbl_BC_INST_ASSIGN_PLUS_NO_VAL, &&lbl_BC_INST_ASSIGN_MINUS_NO_VAL, &&lbl_BC_INST_ASSIGN_NO_VAL, &&lbl_BC_INST_NUM, &&lbl_BC_INST_VAR, &&lbl_BC_INST_ARRAY_ELEM, &&lbl_BC_INST_ARRAY, &&lbl_BC_INST_ZERO, &&lbl_BC_INST_ONE, &&lbl_BC_INST_LAST, &&lbl_BC_INST_IBASE, &&lbl_BC_INST_OBASE, &&lbl_BC_INST_SCALE, &&lbl_BC_INST_LENGTH, &&lbl_BC_INST_SCALE_FUNC, &&lbl_BC_INST_SQRT, &&lbl_BC_INST_ABS, &&lbl_BC_INST_ASCIIFY, &&lbl_BC_INST_READ, &&lbl_BC_INST_MAXIBASE, &&lbl_BC_INST_MAXOBASE, &&lbl_BC_INST_MAXSCALE, &&lbl_BC_INST_LINE_LENGTH, &&lbl_BC_INST_GLOBAL_STACKS, &&lbl_BC_INST_LEADING_ZERO, &&lbl_BC_INST_PRINT, &&lbl_BC_INST_PRINT_POP, &&lbl_BC_INST_STR, &&lbl_BC_INST_PRINT_STR, &&lbl_BC_INST_JUMP, &&lbl_BC_INST_JUMP_ZERO, &&lbl_BC_INST_CALL, &&lbl_BC_INST_RET, &&lbl_BC_INST_RET0, &&lbl_BC_INST_RET_VOID, &&lbl_BC_INST_HALT, &&lbl_BC_INST_POP, &&lbl_BC_INST_SWAP, &&lbl_BC_INST_MODEXP, &&lbl_BC_INST_DIVMOD, &&lbl_BC_INST_PRINT_STREAM, &&lbl_BC_INST_INVALID, }









































































#endif

#endif

#else

#if BC_ENABLE_EXTRA_MATH

#define BC_PROG_LBLS static const void* const bc_program_inst_lbls[] = { &&lbl_BC_INST_NEG, &&lbl_BC_INST_BOOL_NOT, &&lbl_BC_INST_TRUNC, &&lbl_BC_INST_POWER, &&lbl_BC_INST_MULTIPLY, &&lbl_BC_INST_DIVIDE, &&lbl_BC_INST_MODULUS, &&lbl_BC_INST_PLUS, &&lbl_BC_INST_MINUS, &&lbl_BC_INST_PLACES, &&lbl_BC_INST_LSHIFT, &&lbl_BC_INST_RSHIFT, &&lbl_BC_INST_REL_EQ, &&lbl_BC_INST_REL_LE, &&lbl_BC_INST_REL_GE, &&lbl_BC_INST_REL_NE, &&lbl_BC_INST_REL_LT, &&lbl_BC_INST_REL_GT, &&lbl_BC_INST_BOOL_OR, &&lbl_BC_INST_BOOL_AND, &&lbl_BC_INST_ASSIGN_NO_VAL, &&lbl_BC_INST_NUM, &&lbl_BC_INST_VAR, &&lbl_BC_INST_ARRAY_ELEM, &&lbl_BC_INST_ARRAY, &&lbl_BC_INST_ZERO, &&lbl_BC_INST_ONE, &&lbl_BC_INST_IBASE, &&lbl_BC_INST_OBASE, &&lbl_BC_INST_SCALE, &&lbl_BC_INST_SEED, &&lbl_BC_INST_LENGTH, &&lbl_BC_INST_SCALE_FUNC, &&lbl_BC_INST_SQRT, &&lbl_BC_INST_ABS, &&lbl_BC_INST_IRAND, &&lbl_BC_INST_ASCIIFY, &&lbl_BC_INST_READ, &&lbl_BC_INST_RAND, &&lbl_BC_INST_MAXIBASE, &&lbl_BC_INST_MAXOBASE, &&lbl_BC_INST_MAXSCALE, &&lbl_BC_INST_MAXRAND, &&lbl_BC_INST_LINE_LENGTH, &&lbl_BC_INST_LEADING_ZERO, &&lbl_BC_INST_PRINT, &&lbl_BC_INST_PRINT_POP, &&lbl_BC_INST_STR, &&lbl_BC_INST_POP, &&lbl_BC_INST_SWAP, &&lbl_BC_INST_MODEXP, &&lbl_BC_INST_DIVMOD, &&lbl_BC_INST_PRINT_STREAM, &&lbl_BC_INST_POP_EXEC, &&lbl_BC_INST_EXECUTE, &&lbl_BC_INST_EXEC_COND, &&lbl_BC_INST_PRINT_STACK, &&lbl_BC_INST_CLEAR_STACK, &&lbl_BC_INST_REG_STACK_LEN, &&lbl_BC_INST_STACK_LEN, &&lbl_BC_INST_DUPLICATE, &&lbl_BC_INST_LOAD, &&lbl_BC_INST_PUSH_VAR, &&lbl_BC_INST_PUSH_TO_VAR, &&lbl_BC_INST_QUIT, &&lbl_BC_INST_NQUIT, &&lbl_BC_INST_EXEC_STACK_LEN, &&lbl_BC_INST_INVALID, }






































































#else

#define BC_PROG_LBLS static const void* const bc_program_inst_lbls[] = { &&lbl_BC_INST_NEG, &&lbl_BC_INST_BOOL_NOT, &&lbl_BC_INST_POWER, &&lbl_BC_INST_MULTIPLY, &&lbl_BC_INST_DIVIDE, &&lbl_BC_INST_MODULUS, &&lbl_BC_INST_PLUS, &&lbl_BC_INST_MINUS, &&lbl_BC_INST_REL_EQ, &&lbl_BC_INST_REL_LE, &&lbl_BC_INST_REL_GE, &&lbl_BC_INST_REL_NE, &&lbl_BC_INST_REL_LT, &&lbl_BC_INST_REL_GT, &&lbl_BC_INST_BOOL_OR, &&lbl_BC_INST_BOOL_AND, &&lbl_BC_INST_ASSIGN_NO_VAL, &&lbl_BC_INST_NUM, &&lbl_BC_INST_VAR, &&lbl_BC_INST_ARRAY_ELEM, &&lbl_BC_INST_ARRAY, &&lbl_BC_INST_ZERO, &&lbl_BC_INST_ONE, &&lbl_BC_INST_IBASE, &&lbl_BC_INST_OBASE, &&lbl_BC_INST_SCALE, &&lbl_BC_INST_LENGTH, &&lbl_BC_INST_SCALE_FUNC, &&lbl_BC_INST_SQRT, &&lbl_BC_INST_ABS, &&lbl_BC_INST_ASCIIFY, &&lbl_BC_INST_READ, &&lbl_BC_INST_MAXIBASE, &&lbl_BC_INST_MAXOBASE, &&lbl_BC_INST_MAXSCALE, &&lbl_BC_INST_LINE_LENGTH, &&lbl_BC_INST_LEADING_ZERO, &&lbl_BC_INST_PRINT, &&lbl_BC_INST_PRINT_POP, &&lbl_BC_INST_STR, &&lbl_BC_INST_POP, &&lbl_BC_INST_SWAP, &&lbl_BC_INST_MODEXP, &&lbl_BC_INST_DIVMOD, &&lbl_BC_INST_PRINT_STREAM, &&lbl_BC_INST_POP_EXEC, &&lbl_BC_INST_EXECUTE, &&lbl_BC_INST_EXEC_COND, &&lbl_BC_INST_PRINT_STACK, &&lbl_BC_INST_CLEAR_STACK, &&lbl_BC_INST_REG_STACK_LEN, &&lbl_BC_INST_STACK_LEN, &&lbl_BC_INST_DUPLICATE, &&lbl_BC_INST_LOAD, &&lbl_BC_INST_PUSH_VAR, &&lbl_BC_INST_PUSH_TO_VAR, &&lbl_BC_INST_QUIT, &&lbl_BC_INST_NQUIT, &&lbl_BC_INST_EXEC_STACK_LEN, &&lbl_BC_INST_INVALID, }






























































#endif

#endif

#else

#define BC_PROG_JUMP(inst, code, ip) break
#define BC_PROG_DIRECT_JUMP(l)
#define BC_PROG_LBL(l) case l
#define BC_PROG_FALLTHROUGH BC_FALLTHROUGH

#define BC_PROG_LBLS

#endif

#endif
