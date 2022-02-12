









#include "xo.h"
#include "xo_encoder.h"

static int
test_handler (XO_ENCODER_HANDLER_ARGS)
{
printf("op %s: [%s] [%s] [%#llx]\n", xo_encoder_op_name(op),
name ?: "", value ?: "", (unsigned long long) flags);

return 0;
}

int
xo_encoder_library_init (XO_ENCODER_INIT_ARGS)
{
arg->xei_version = XO_ENCODER_VERSION;
arg->xei_handler = test_handler;

return 0;
}
