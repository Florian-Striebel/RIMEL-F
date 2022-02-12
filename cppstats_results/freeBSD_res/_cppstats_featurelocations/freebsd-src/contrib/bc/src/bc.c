


































#if BC_ENABLED

#include <string.h>

#include <bc.h>
#include <vm.h>






void bc_main(int argc, char *argv[]) {



vm.read_ret = BC_INST_RET;
vm.help = bc_help;
vm.sigmsg = bc_sig_msg;
vm.siglen = bc_sig_msg_len;

vm.next = bc_lex_token;
vm.parse = bc_parse_parse;
vm.expr = bc_parse_expr;

bc_vm_boot(argc, argv);
}
#endif
