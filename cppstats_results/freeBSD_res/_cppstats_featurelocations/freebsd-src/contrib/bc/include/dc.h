


































#if !defined(BC_DC_H)
#define BC_DC_H

#if DC_ENABLED

#include <status.h>
#include <lex.h>
#include <parse.h>





void dc_main(int argc, char *argv[]);


extern const char dc_help[];






void dc_lex_token(BcLex *l);









bool dc_lex_negCommand(BcLex *l);


extern const char dc_sig_msg[];
extern const uchar dc_sig_msg_len;



extern const uint8_t dc_lex_regs[];
extern const size_t dc_lex_regs_len;




extern const uint8_t dc_lex_tokens[];
extern const uint8_t dc_parse_insts[];






void dc_parse_parse(BcParse *p);








void dc_parse_expr(BcParse *p, uint8_t flags);

#endif

#endif
