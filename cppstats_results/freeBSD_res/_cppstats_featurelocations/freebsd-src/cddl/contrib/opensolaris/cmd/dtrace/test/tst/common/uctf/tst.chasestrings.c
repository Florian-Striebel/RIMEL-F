




















#include <unistd.h>

typedef struct zelda_info {
const char *zi_gamename;
int zi_ndungeons;
const char *zi_villain;
int zi_haszelda;
} zelda_info_t;

static int
has_princess(zelda_info_t *z)
{
return (z->zi_haszelda);
}

static int
has_dungeons(zelda_info_t *z)
{
return (z->zi_ndungeons != 0);
}

static const char *
has_villain(zelda_info_t *z)
{
return (z->zi_villain);
}

int
main(void)
{
zelda_info_t oot;
zelda_info_t la;
zelda_info_t lttp;

oot.zi_gamename = "Ocarina of Time";
oot.zi_ndungeons = 10;
oot.zi_villain = "Ganondorf";
oot.zi_haszelda = 1;

la.zi_gamename = "Link's Awakening";
la.zi_ndungeons = 9;
la.zi_villain = "Nightmare";
la.zi_haszelda = 0;

lttp.zi_gamename = "A Link to the Past";
lttp.zi_ndungeons = 12;
lttp.zi_villain = "Ganon";
lttp.zi_haszelda = 1;

for (;;) {
(void) has_princess(&oot);
(void) has_dungeons(&la);
(void) has_villain(&lttp);
sleep(1);
}

return (0);
}
