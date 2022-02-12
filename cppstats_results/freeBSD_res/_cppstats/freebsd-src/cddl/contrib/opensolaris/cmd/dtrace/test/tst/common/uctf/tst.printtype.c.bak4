



















#include <unistd.h>

typedef struct final_fantasy_info {
int ff_gameid;
int ff_partysize;
int ff_hassummons;
} final_fantasy_info_t;

static int
ff_getgameid(final_fantasy_info_t *f __unused)
{
return (0);
}

static int
ff_getpartysize(final_fantasy_info_t *f __unused)
{
return (0);
}

static int
ff_getsummons(final_fantasy_info_t *f __unused)
{
return (0);
}

int
main(void)
{
final_fantasy_info_t ffiii, ffx, ffi;

ffi.ff_gameid = 1;
ffi.ff_partysize = 4;
ffi.ff_hassummons = 0;

ffiii.ff_gameid = 6;
ffiii.ff_partysize = 4;
ffiii.ff_hassummons = 1;

ffx.ff_gameid = 10;
ffx.ff_partysize = 3;
ffx.ff_hassummons = 1;

for (;;) {
ff_getgameid(&ffi);
ff_getpartysize(&ffx);
ff_getsummons(&ffiii);
sleep(1);
}

return (0);
}
