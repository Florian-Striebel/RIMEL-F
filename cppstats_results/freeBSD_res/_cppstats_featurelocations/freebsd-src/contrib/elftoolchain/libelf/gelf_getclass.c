

























#include <gelf.h>

#include "_libelf.h"

ELFTC_VCSID("$Id: gelf_getclass.c 3174 2015-03-27 17:13:41Z emaste $");

int
gelf_getclass(Elf *e)
{
return (e != NULL ? e->e_class : ELFCLASSNONE);
}
