


































#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include <locale.h>

#if !defined(_WIN32)
#include <libgen.h>
#endif

#include <setjmp.h>

#include <version.h>
#include <status.h>
#include <vm.h>
#include <bc.h>
#include <dc.h>

int main(int argc, char *argv[]) {

char *name;
size_t len = strlen(BC_EXECPREFIX);


vm.locale = setlocale(LC_ALL, "");


bc_pledge(bc_pledge_start, NULL);



name = strrchr(argv[0], BC_FILE_SEP);
vm.name = (name == NULL) ? argv[0] : name + 1;


if (strlen(vm.name) > len) vm.name += len;

BC_SIG_LOCK;



bc_vec_init(&vm.jmp_bufs, sizeof(sigjmp_buf), BC_DTOR_NONE);

BC_SETJMP_LOCKED(exit);

#if !DC_ENABLED
bc_main(argc, argv);
#elif !BC_ENABLED
dc_main(argc, argv);
#else

if (BC_IS_BC) bc_main(argc, argv);
else dc_main(argc, argv);
#endif

exit:
BC_SIG_MAYLOCK;


return bc_vm_atexit((int) vm.status);
}
