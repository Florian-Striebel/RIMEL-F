

























#include <sys/param.h>
#include <string.h>
#include <libelftc.h>

#include "_libelftc.h"

ELFTC_VCSID("$Id: elftc_bfdtarget.c 3174 2015-03-27 17:13:41Z emaste $");

Elftc_Bfd_Target *
elftc_bfd_find_target(const char *tgt_name)
{
Elftc_Bfd_Target *tgt;

for (tgt = _libelftc_targets; tgt->bt_name; tgt++)
if (!strcmp(tgt_name, tgt->bt_name))
return (tgt);

return (NULL);
}

Elftc_Bfd_Target_Flavor
elftc_bfd_target_flavor(Elftc_Bfd_Target *tgt)
{

return (tgt->bt_type);
}

unsigned int
elftc_bfd_target_byteorder(Elftc_Bfd_Target *tgt)
{

return (tgt->bt_byteorder);
}

unsigned int
elftc_bfd_target_class(Elftc_Bfd_Target *tgt)
{

return (tgt->bt_elfclass);
}

unsigned int
elftc_bfd_target_machine(Elftc_Bfd_Target *tgt)
{

return (tgt->bt_machine);
}

unsigned int
elftc_bfd_target_osabi(Elftc_Bfd_Target *tgt)
{

return (tgt->bt_osabi);
}
