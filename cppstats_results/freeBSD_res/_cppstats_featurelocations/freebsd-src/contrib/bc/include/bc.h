


































#if !defined(BC_BC_H)
#define BC_BC_H

#if BC_ENABLED

#include <limits.h>
#include <stdbool.h>

#include <status.h>
#include <lex.h>
#include <parse.h>





void bc_main(int argc, char *argv[]);



extern const char bc_help[];
extern const char bc_lib[];
extern const char* bc_lib_name;


#if BC_ENABLE_EXTRA_MATH
extern const char bc_lib2[];
extern const char* bc_lib2_name;
#endif




typedef struct BcLexKeyword {



uchar data;


const char name[14];
} BcLexKeyword;



#define BC_LEX_CHAR_MSB(bit) ((bit) << (CHAR_BIT - 1))


#define BC_LEX_KW_POSIX(kw) ((kw)->data & (BC_LEX_CHAR_MSB(1)))


#define BC_LEX_KW_LEN(kw) ((size_t) ((kw)->data & ~(BC_LEX_CHAR_MSB(1))))


#define BC_LEX_KW_ENTRY(a, b, c) { .data = ((b) & ~(BC_LEX_CHAR_MSB(1))) | BC_LEX_CHAR_MSB(c), .name = a }


#if BC_ENABLE_EXTRA_MATH



#define BC_LEX_NKWS (35)

#else



#define BC_LEX_NKWS (31)

#endif


extern const BcLexKeyword bc_lex_kws[];
extern const size_t bc_lex_kws_len;






void bc_lex_token(BcLex *l);















#define BC_PARSE_TOP_FLAG_PTR(p) ((uint16_t*) bc_vec_top(&(p)->flags))







#define BC_PARSE_TOP_FLAG(p) (*(BC_PARSE_TOP_FLAG_PTR(p)))






#define BC_PARSE_FLAG_BRACE (UINTMAX_C(1)<<0)
#define BC_PARSE_BRACE(p) (BC_PARSE_TOP_FLAG(p) & BC_PARSE_FLAG_BRACE)



#define BC_PARSE_FLAG_FUNC_INNER (UINTMAX_C(1)<<1)
#define BC_PARSE_FUNC_INNER(p) (BC_PARSE_TOP_FLAG(p) & BC_PARSE_FLAG_FUNC_INNER)




#define BC_PARSE_FLAG_FUNC (UINTMAX_C(1)<<2)
#define BC_PARSE_FUNC(p) (BC_PARSE_TOP_FLAG(p) & BC_PARSE_FLAG_FUNC)



#define BC_PARSE_FLAG_BODY (UINTMAX_C(1)<<3)
#define BC_PARSE_BODY(p) (BC_PARSE_TOP_FLAG(p) & BC_PARSE_FLAG_BODY)



#define BC_PARSE_FLAG_LOOP (UINTMAX_C(1)<<4)
#define BC_PARSE_LOOP(p) (BC_PARSE_TOP_FLAG(p) & BC_PARSE_FLAG_LOOP)




#define BC_PARSE_FLAG_LOOP_INNER (UINTMAX_C(1)<<5)
#define BC_PARSE_LOOP_INNER(p) (BC_PARSE_TOP_FLAG(p) & BC_PARSE_FLAG_LOOP_INNER)


#define BC_PARSE_FLAG_IF (UINTMAX_C(1)<<6)
#define BC_PARSE_IF(p) (BC_PARSE_TOP_FLAG(p) & BC_PARSE_FLAG_IF)



#define BC_PARSE_FLAG_ELSE (UINTMAX_C(1)<<7)
#define BC_PARSE_ELSE(p) (BC_PARSE_TOP_FLAG(p) & BC_PARSE_FLAG_ELSE)




#define BC_PARSE_FLAG_IF_END (UINTMAX_C(1)<<8)
#define BC_PARSE_IF_END(p) (BC_PARSE_TOP_FLAG(p) & BC_PARSE_FLAG_IF_END)







#define BC_PARSE_NO_EXEC(p) ((p)->flags.len != 1 || BC_PARSE_TOP_FLAG(p) != 0)







#define BC_PARSE_DELIMITER(t) ((t) == BC_LEX_SCOLON || (t) == BC_LEX_NLINE || (t) == BC_LEX_EOF)








#define BC_PARSE_BLOCK_STMT(f) ((f) & (BC_PARSE_FLAG_ELSE | BC_PARSE_FLAG_LOOP_INNER))










#define BC_PARSE_OP(p, l) (((p) & ~(BC_LEX_CHAR_MSB(1))) | (BC_LEX_CHAR_MSB(l)))






#define BC_PARSE_OP_DATA(t) bc_parse_ops[((t) - BC_LEX_OP_INC)]






#define BC_PARSE_OP_LEFT(op) (BC_PARSE_OP_DATA(op) & BC_LEX_CHAR_MSB(1))







#define BC_PARSE_OP_PREC(op) (BC_PARSE_OP_DATA(op) & ~(BC_LEX_CHAR_MSB(1)))















#define BC_PARSE_EXPR_ENTRY(e1, e2, e3, e4, e5, e6, e7, e8) ((UINTMAX_C(e1) << 7) | (UINTMAX_C(e2) << 6) | (UINTMAX_C(e3) << 5) | (UINTMAX_C(e4) << 4) | (UINTMAX_C(e5) << 3) | (UINTMAX_C(e6) << 2) | (UINTMAX_C(e7) << 1) | (UINTMAX_C(e8) << 0))









#define BC_PARSE_EXPR(i) (bc_parse_exprs[(((i) & (uchar) ~(0x07)) >> 3)] & (1 << (7 - ((i) & 0x07))))









#define BC_PARSE_TOP_OP(p) (*((BcLexType*) bc_vec_top(&(p)->ops)))













#define BC_PARSE_LEAF(prev, bin_last, rparen) (!(bin_last) && ((rparen) || bc_parse_inst_isLeaf(prev)))









#if BC_ENABLE_EXTRA_MATH
#define BC_PARSE_INST_VAR(t) ((t) >= BC_INST_VAR && (t) <= BC_INST_SEED && (t) != BC_INST_ARRAY)

#else
#define BC_PARSE_INST_VAR(t) ((t) >= BC_INST_VAR && (t) <= BC_INST_SCALE && (t) != BC_INST_ARRAY)

#endif








#define BC_PARSE_PREV_PREFIX(p) ((p) >= BC_INST_NEG && (p) <= BC_INST_BOOL_NOT)






#define BC_PARSE_OP_PREFIX(t) ((t) == BC_LEX_OP_BOOL_NOT || (t) == BC_LEX_NEG)









#define BC_PARSE_TOKEN_INST(t) ((uchar) ((t) - BC_LEX_NEG + BC_INST_NEG))






#define BC_PARSE_IS_KEYWORD(t) ((t) >= BC_LEX_KW_AUTO && (t) <= BC_LEX_KW_ELSE)








typedef struct BcParseNext {


uchar len;


uchar tokens[4];

} BcParseNext;


#define BC_PARSE_NEXT_TOKENS(...) .tokens = { __VA_ARGS__ }



#define BC_PARSE_NEXT(a, ...) { .len = (uchar) (a), BC_PARSE_NEXT_TOKENS(__VA_ARGS__) }




typedef enum BcParseStatus {

BC_PARSE_STATUS_SUCCESS,
BC_PARSE_STATUS_EMPTY_EXPR,

} BcParseStatus;








void bc_parse_expr(BcParse *p, uint8_t flags);






void bc_parse_parse(BcParse *p);








void bc_parse_endif(BcParse *p);


extern const char bc_sig_msg[];
extern const uchar bc_sig_msg_len;



extern const uint8_t bc_parse_exprs[];


extern const uchar bc_parse_ops[];






extern const BcParseNext bc_parse_next_expr;



extern const BcParseNext bc_parse_next_arg;



extern const BcParseNext bc_parse_next_print;








extern const BcParseNext bc_parse_next_rel;



extern const BcParseNext bc_parse_next_elem;



extern const BcParseNext bc_parse_next_for;



extern const BcParseNext bc_parse_next_read;



extern const BcParseNext bc_parse_next_builtin;

#else



#define BC_PARSE_NO_EXEC(p) (0)

#endif

#endif
