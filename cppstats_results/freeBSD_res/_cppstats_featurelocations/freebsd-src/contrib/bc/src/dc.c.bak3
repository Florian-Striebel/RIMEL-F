


































#if DC_ENABLED

#include <string.h>

#include <dc.h>
#include <vm.h>






void dc_main(int argc, char *argv[]) {



vm.read_ret = BC_INST_POP_EXEC;
vm.help = dc_help;
vm.sigmsg = dc_sig_msg;
vm.siglen = dc_sig_msg_len;

vm.next = dc_lex_token;
vm.parse = dc_parse_parse;
vm.expr = dc_parse_expr;

bc_vm_boot(argc, argv);
}
#endif
