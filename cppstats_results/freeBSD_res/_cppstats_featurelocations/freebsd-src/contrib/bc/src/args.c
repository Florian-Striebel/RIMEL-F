


































#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#if !defined(_WIN32)
#include <unistd.h>
#endif

#include <vector.h>
#include <read.h>
#include <args.h>
#include <opt.h>





static void bc_args_exprs(const char *str) {
BC_SIG_ASSERT_LOCKED;
if (vm.exprs.v == NULL) bc_vec_init(&vm.exprs, sizeof(uchar), BC_DTOR_NONE);
bc_vec_concat(&vm.exprs, str);
bc_vec_concat(&vm.exprs, "\n");
}






static void bc_args_file(const char *file) {

char *buf;

BC_SIG_ASSERT_LOCKED;

vm.file = file;

buf = bc_read_file(file);

assert(buf != NULL);

bc_args_exprs(buf);
free(buf);
}

#if BC_ENABLED






static void bc_args_redefine(const char *keyword) {

size_t i;

BC_SIG_ASSERT_LOCKED;

for (i = 0; i < bc_lex_kws_len; ++i) {

const BcLexKeyword *kw = bc_lex_kws + i;

if (!strcmp(keyword, kw->name)) {

if (BC_LEX_KW_POSIX(kw)) break;

vm.redefined_kws[i] = true;

return;
}
}

bc_error(BC_ERR_FATAL_ARG, 0, keyword);
}

#endif

void bc_args(int argc, char *argv[], bool exit_exprs) {

int c;
size_t i;
bool do_exit = false, version = false;
BcOpt opts;

BC_SIG_ASSERT_LOCKED;

bc_opt_init(&opts, argv);



while ((c = bc_opt_parse(&opts, bc_args_lopt)) != -1) {

switch (c) {

case 'e':
{

if (vm.no_exprs)
bc_verr(BC_ERR_FATAL_OPTION, "-e (--expression)");


bc_args_exprs(opts.optarg);
vm.exit_exprs = (exit_exprs || vm.exit_exprs);

break;
}

case 'f':
{

if (!strcmp(opts.optarg, "-")) vm.no_exprs = true;
else {


if (vm.no_exprs)
bc_verr(BC_ERR_FATAL_OPTION, "-f (--file)");


bc_args_file(opts.optarg);
vm.exit_exprs = (exit_exprs || vm.exit_exprs);
}

break;
}

case 'h':
{
bc_vm_info(vm.help);
do_exit = true;
break;
}

case 'i':
{
vm.flags |= BC_FLAG_I;
break;
}

case 'z':
{
vm.flags |= BC_FLAG_Z;
break;
}

case 'L':
{
vm.line_len = 0;
break;
}

case 'P':
{
vm.flags &= ~(BC_FLAG_P);
break;
}

case 'R':
{
vm.flags &= ~(BC_FLAG_R);
break;
}

#if BC_ENABLED
case 'g':
{
assert(BC_IS_BC);
vm.flags |= BC_FLAG_G;
break;
}

case 'l':
{
assert(BC_IS_BC);
vm.flags |= BC_FLAG_L;
break;
}

case 'q':
{
assert(BC_IS_BC);
vm.flags &= ~(BC_FLAG_Q);
break;
}

case 'r':
{
bc_args_redefine(opts.optarg);
break;
}

case 's':
{
assert(BC_IS_BC);
vm.flags |= BC_FLAG_S;
break;
}

case 'w':
{
assert(BC_IS_BC);
vm.flags |= BC_FLAG_W;
break;
}
#endif

case 'V':
case 'v':
{
do_exit = version = true;
break;
}

#if DC_ENABLED
case 'x':
{
assert(BC_IS_DC);
vm.flags |= DC_FLAG_X;
break;
}
#endif

#if !defined(NDEBUG)


case '?':
case ':':
default:
{
BC_UNREACHABLE
abort();
}
#endif
}
}

if (version) bc_vm_info(NULL);
if (do_exit) {
vm.status = (sig_atomic_t) BC_STATUS_QUIT;
BC_JMP;
}


if (!BC_IS_BC || vm.exprs.len > 1) vm.flags &= ~(BC_FLAG_Q);



if (opts.optind < (size_t) argc && vm.files.v == NULL)
bc_vec_init(&vm.files, sizeof(char*), BC_DTOR_NONE);


for (i = opts.optind; i < (size_t) argc; ++i)
bc_vec_push(&vm.files, argv + i);
}
