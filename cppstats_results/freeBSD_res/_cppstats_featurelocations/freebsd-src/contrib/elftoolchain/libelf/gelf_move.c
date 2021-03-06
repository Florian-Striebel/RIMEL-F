

























#include <assert.h>
#include <gelf.h>
#include <limits.h>
#include <stdint.h>

#include "_libelf.h"

ELFTC_VCSID("$Id: gelf_move.c 3732 2019-04-22 11:08:38Z jkoshy $");

GElf_Move *
gelf_getmove(Elf_Data *ed, int ndx, GElf_Move *dst)
{
int ec;
Elf *e;
size_t msz;
Elf_Scn *scn;
uint32_t sh_type;
Elf32_Move *move32;
Elf64_Move *move64;
struct _Libelf_Data *d;

d = (struct _Libelf_Data *) ed;

if (d == NULL || ndx < 0 || dst == NULL ||
(scn = d->d_scn) == NULL ||
(e = scn->s_elf) == NULL) {
LIBELF_SET_ERROR(ARGUMENT, 0);
return (NULL);
}

ec = e->e_class;
assert(ec == ELFCLASS32 || ec == ELFCLASS64);

if (ec == ELFCLASS32)
sh_type = scn->s_shdr.s_shdr32.sh_type;
else
sh_type = scn->s_shdr.s_shdr64.sh_type;

if (_libelf_xlate_shtype(sh_type) != ELF_T_MOVE) {
LIBELF_SET_ERROR(ARGUMENT, 0);
return (NULL);
}

if ((msz = _libelf_msize(ELF_T_MOVE, ec, e->e_version)) == 0)
return (NULL);

assert(ndx >= 0);

if (msz * (size_t) ndx >= d->d_data.d_size) {
LIBELF_SET_ERROR(ARGUMENT, 0);
return (NULL);
}

if (ec == ELFCLASS32) {

move32 = (Elf32_Move *) d->d_data.d_buf + ndx;

dst->m_value = move32->m_value;
dst->m_info = (Elf64_Xword) move32->m_info;
dst->m_poffset = (Elf64_Xword) move32->m_poffset;
dst->m_repeat = move32->m_repeat;
dst->m_stride = move32->m_stride;
} else {

move64 = (Elf64_Move *) d->d_data.d_buf + ndx;

*dst = *move64;
}

return (dst);
}

int
gelf_update_move(Elf_Data *ed, int ndx, GElf_Move *gm)
{
int ec;
Elf *e;
size_t msz;
Elf_Scn *scn;
uint32_t sh_type;
Elf32_Move *move32;
Elf64_Move *move64;
struct _Libelf_Data *d;

d = (struct _Libelf_Data *) ed;

if (d == NULL || ndx < 0 || gm == NULL ||
(scn = d->d_scn) == NULL ||
(e = scn->s_elf) == NULL) {
LIBELF_SET_ERROR(ARGUMENT, 0);
return (0);
}

ec = e->e_class;
assert(ec == ELFCLASS32 || ec == ELFCLASS64);

if (ec == ELFCLASS32)
sh_type = scn->s_shdr.s_shdr32.sh_type;
else
sh_type = scn->s_shdr.s_shdr64.sh_type;

if (_libelf_xlate_shtype(sh_type) != ELF_T_MOVE) {
LIBELF_SET_ERROR(ARGUMENT, 0);
return (0);
}

if ((msz = _libelf_msize(ELF_T_MOVE, ec, e->e_version)) == 0)
return (0);

assert(ndx >= 0);

if (msz * (size_t) ndx >= d->d_data.d_size) {
LIBELF_SET_ERROR(ARGUMENT, 0);
return (0);
}

if (ec == ELFCLASS32) {
move32 = (Elf32_Move *) d->d_data.d_buf + ndx;

move32->m_value = gm->m_value;
LIBELF_COPY_U32(move32, gm, m_info);
LIBELF_COPY_U32(move32, gm, m_poffset);
move32->m_repeat = gm->m_repeat;
move32->m_stride = gm->m_stride;

} else {
move64 = (Elf64_Move *) d->d_data.d_buf + ndx;

*move64 = *gm;
}

return (1);
}
