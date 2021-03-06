


































#if !defined(BC_PARSE_H)
#define BC_PARSE_H

#include <limits.h>
#include <stdbool.h>
#include <stdint.h>

#include <status.h>
#include <vector.h>
#include <lex.h>
#include <lang.h>









#define BC_PARSE_REL (UINTMAX_C(1)<<0)


#define BC_PARSE_PRINT (UINTMAX_C(1)<<1)


#define BC_PARSE_NOCALL (UINTMAX_C(1)<<2)


#define BC_PARSE_NOREAD (UINTMAX_C(1)<<3)



#define BC_PARSE_ARRAY (UINTMAX_C(1)<<4)


#define BC_PARSE_NEEDVAL (UINTMAX_C(1)<<5)







#define BC_PARSE_IS_INITED(p, prg) ((p)->prog == (prg))

#if BC_ENABLED






#define BC_PARSE_CAN_PARSE(p) ((p).l.t != BC_LEX_EOF && (p).l.t != BC_LEX_KW_DEFINE)


#else






#define BC_PARSE_CAN_PARSE(p) ((p).l.t != BC_LEX_EOF)

#endif







#define bc_parse_push(p, i) (bc_vec_pushByte(&(p)->func->code, (uchar) (i)))








#define bc_parse_pushIndex(p, idx) (bc_vec_pushIndex(&(p)->func->code, (idx)))







#define bc_parse_err(p, e) (bc_vm_handleError((e), (p)->l.line))








#define bc_parse_verr(p, e, ...) (bc_vm_handleError((e), (p)->l.line, __VA_ARGS__))



struct BcParse;
struct BcProgram;





typedef void (*BcParseParse)(struct BcParse* p);







typedef void (*BcParseExpr)(struct BcParse* p, uint8_t flags);


typedef struct BcParse {


BcLex l;

#if BC_ENABLED



BcVec flags;




BcVec exits;




BcVec conds;






BcVec ops;






BcVec buf;
#endif


struct BcProgram *prog;



BcFunc *func;


size_t fidx;

#if BC_ENABLED


bool auto_part;
#endif

} BcParse;







void bc_parse_init(BcParse *p, struct BcProgram *prog, size_t func);






void bc_parse_free(BcParse *p);






void bc_parse_reset(BcParse *p);





void bc_parse_addString(BcParse *p);





void bc_parse_number(BcParse *p);






void bc_parse_updateFunc(BcParse *p, size_t fidx);








void bc_parse_pushName(const BcParse* p, char *name, bool var);







void bc_parse_text(BcParse *p, const char *text, bool is_stdin);




extern const char bc_parse_zero[2];
extern const char bc_parse_one[2];

#endif
