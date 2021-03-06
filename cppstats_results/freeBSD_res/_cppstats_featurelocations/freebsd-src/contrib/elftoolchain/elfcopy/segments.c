

























#include <sys/queue.h>
#include <err.h>
#include <gelf.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "elfcopy.h"

ELFTC_VCSID("$Id: segments.c 3615 2018-05-17 04:12:24Z kaiwang27 $");

static void insert_to_inseg_list(struct segment *seg, struct section *sec);













int
add_to_inseg_list(struct elfcopy *ecp, struct section *s)
{
struct segment *seg;
int loadable;

if (ecp->ophnum == 0)
return (0);












loadable = 0;
STAILQ_FOREACH(seg, &ecp->v_seg, seg_list) {
if (s->off < seg->off || (s->vma < seg->vaddr && !s->pseudo))
continue;
if (s->off + s->sz > seg->off + seg->fsz &&
s->type != SHT_NOBITS)
continue;
if (s->vma + s->sz > seg->vaddr + seg->msz)
continue;
if (seg->type == PT_TLS && ((s->flags & SHF_TLS) == 0))
continue;

insert_to_inseg_list(seg, s);
if (seg->type == PT_LOAD)
s->seg = seg;
else if (seg->type == PT_TLS)
s->seg_tls = seg;
if (s->pseudo)
s->vma = seg->vaddr + (s->off - seg->off);
if (seg->paddr > 0)
s->lma = seg->paddr + (s->off - seg->off);
else
s->lma = 0;
loadable = 1;
}

return (loadable);
}

void
adjust_addr(struct elfcopy *ecp)
{
struct section *s, *s0;
struct segment *seg;
struct sec_action *sac;
uint64_t dl, vma, lma, start, end;
int found, i;




TAILQ_FOREACH(s, &ecp->v_sec, sec_list) {


if (!s->loadable)
continue;


if (ecp->change_addr != 0)
s->vma += ecp->change_addr;


if (ecp->change_addr != 0 && s->seg != NULL &&
s->seg->paddr > 0)
s->lma += ecp->change_addr;
}




TAILQ_FOREACH(s, &ecp->v_sec, sec_list) {

if (!s->loadable)
continue;





sac = lookup_sec_act(ecp, s->name, 0);
if (sac == NULL)
continue;
vma = s->vma;
if (sac->setvma)
vma = sac->vma;
if (sac->vma_adjust != 0)
vma += sac->vma_adjust;
if (vma == s->vma)
continue;





if (s->seg == NULL) {
s->vma = vma;
continue;
}











#if defined(DEBUG)
printf("VMA for section %s: %#jx\n", s->name, vma);
#endif

if (vma % s->align != 0)
errx(EXIT_FAILURE, "The VMA %#jx for "
"section %s is not aligned to %ju",
(uintmax_t) vma, s->name, (uintmax_t) s->align);

if (vma < s->vma) {

if (vma < s->vma - s->seg->vaddr)
errx(EXIT_FAILURE, "Not enough space to move "
"section %s VMA to %#jx", s->name,
(uintmax_t) vma);
start = vma - (s->vma - s->seg->vaddr);
if (s == s->seg->v_sec[s->seg->nsec - 1])
end = start + s->seg->msz;
else
end = s->seg->vaddr + s->seg->msz;
} else {

if (s == s->seg->v_sec[0])
start = vma;
else
start = s->seg->vaddr;
end = vma + (s->seg->vaddr + s->seg->msz - s->vma);
if (end < start)
errx(EXIT_FAILURE, "Not enough space to move "
"section %s VMA to %#jx", s->name,
(uintmax_t) vma);
}

#if defined(DEBUG)
printf("new extent for segment containing %s: (%#jx,%#jx)\n",
s->name, start, end);
#endif

STAILQ_FOREACH(seg, &ecp->v_seg, seg_list) {
if (seg == s->seg || seg->type != PT_LOAD)
continue;
if (start > seg->vaddr + seg->msz)
continue;
if (end < seg->vaddr)
continue;
errx(EXIT_FAILURE, "The extent of segment containing "
"section %s overlaps with segment(%#jx,%#jx)",
s->name, (uintmax_t) seg->vaddr,
(uintmax_t) (seg->vaddr + seg->msz));
}





if (vma < s->vma) {






dl = s->vma - vma;
for (i = 0; i < s->seg->nsec; i++) {
s0 = s->seg->v_sec[i];
s0->vma -= dl;
#if defined(DEBUG)
printf("section %s VMA set to %#jx\n",
s0->name, (uintmax_t) s0->vma);
#endif
if (s0 == s)
break;
}
for (i = i + 1; i < s->seg->nsec; i++) {
s0 = s->seg->v_sec[i];
s0->off += dl;
#if defined(DEBUG)
printf("section %s offset set to %#jx\n",
s0->name, (uintmax_t) s0->off);
#endif
}
} else {







dl = vma - s->vma;
for (i = 0; i < s->seg->nsec; i++)
if (s->seg->v_sec[i] == s)
break;
if (i >= s->seg->nsec)
errx(EXIT_FAILURE, "Internal: section `%s' not"
" found in its containing segement",
s->name);
for (; i < s->seg->nsec; i++) {
s0 = s->seg->v_sec[i];
s0->vma += dl;
#if defined(DEBUG)
printf("section %s VMA set to %#jx\n",
s0->name, (uintmax_t) s0->lma);
#endif
if (s != s->seg->v_sec[0]) {
s0->off += dl;
#if defined(DEBUG)
printf("section %s offset set to %#jx\n",
s0->name, (uintmax_t) s0->off);
#endif
}
}
}
}





if (ecp->pad_to != 0) {




s = NULL;
STAILQ_FOREACH(seg, &ecp->v_seg, seg_list) {
if (seg->type != PT_LOAD)
continue;
for (i = seg->nsec - 1; i >= 0; i--)
if (seg->v_sec[i]->type != SHT_NOBITS)
break;
if (i < 0)
continue;
if (s == NULL)
s = seg->v_sec[i];
else {
s0 = seg->v_sec[i];
if (s0->vma > s->vma)
s = s0;
}
}

if (s == NULL)
goto adjust_lma;


if (ecp->pad_to <= s->vma + s->sz)
goto adjust_lma;

s->pad_sz = ecp->pad_to - (s->vma + s->sz);
#if defined(DEBUG)
printf("pad section %s VMA to address %#jx by %#jx\n", s->name,
(uintmax_t) ecp->pad_to, (uintmax_t) s->pad_sz);
#endif
}


adjust_lma:




TAILQ_FOREACH(s, &ecp->v_sec, sec_list) {






if (!s->loadable || s->seg == NULL || s->seg->paddr == 0)
continue;





sac = lookup_sec_act(ecp, s->name, 0);
if (sac == NULL)
continue;
if (!sac->setlma && sac->lma_adjust == 0)
continue;
lma = s->lma;
if (sac->setlma)
lma = sac->lma;
if (sac->lma_adjust != 0)
lma += sac->lma_adjust;
if (lma == s->lma)
continue;

#if defined(DEBUG)
printf("LMA for section %s: %#jx\n", s->name, lma);
#endif


if (lma % s->align != 0)
errx(EXIT_FAILURE, "The LMA %#jx for "
"section %s is not aligned to %ju",
(uintmax_t) lma, s->name, (uintmax_t) s->align);





if (lma < s->lma) {





dl = s->lma - lma;
for (i = 0; i < s->seg->nsec; i++) {
s0 = s->seg->v_sec[i];
s0->lma -= dl;
#if defined(DEBUG)
printf("section %s LMA set to %#jx\n",
s0->name, (uintmax_t) s0->lma);
#endif
if (s0 == s)
break;
}
} else {





dl = lma - s->lma;
for (i = 0; i < s->seg->nsec; i++)
if (s->seg->v_sec[i] == s)
break;
if (i >= s->seg->nsec)
errx(EXIT_FAILURE, "Internal: section `%s' not"
" found in its containing segement",
s->name);
for (; i < s->seg->nsec; i++) {
s0 = s->seg->v_sec[i];
s0->lma += dl;
#if defined(DEBUG)
printf("section %s LMA set to %#jx\n",
s0->name, (uintmax_t) s0->lma);
#endif
}
}
}





if ((ecp->flags & NO_CHANGE_WARN) == 0) {
STAILQ_FOREACH(sac, &ecp->v_sac, sac_list) {
if (!sac->setvma && !sac->setlma &&
!sac->vma_adjust && !sac->lma_adjust)
continue;
found = 0;
TAILQ_FOREACH(s, &ecp->v_sec, sec_list) {
if (s->pseudo || s->name == NULL)
continue;
if (!strcmp(s->name, sac->name)) {
found = 1;
break;
}
}
if (!found)
warnx("cannot find section `%s'", sac->name);
}
}
}

static void
insert_to_inseg_list(struct segment *seg, struct section *sec)
{
struct section *s;
int i;

seg->nsec++;
seg->v_sec = realloc(seg->v_sec, seg->nsec * sizeof(*seg->v_sec));
if (seg->v_sec == NULL)
err(EXIT_FAILURE, "realloc failed");





for (i = seg->nsec - 1; i > 0; i--) {
s = seg->v_sec[i - 1];
if (sec->off >= s->off) {
seg->v_sec[i] = sec;
break;
} else
seg->v_sec[i] = s;
}
if (i == 0)
seg->v_sec[0] = sec;
}

void
setup_phdr(struct elfcopy *ecp)
{
struct segment *seg;
GElf_Phdr iphdr;
size_t iphnum, i;

if (elf_getphnum(ecp->ein, &iphnum) == 0)
errx(EXIT_FAILURE, "elf_getphnum failed: %s",
elf_errmsg(-1));

ecp->ophnum = ecp->iphnum = iphnum;
if (iphnum == 0)
return;


if (ecp->strip == STRIP_NONDEBUG) {
ecp->ophnum = 0;
return;
}

for (i = 0; i < iphnum; i++) {
if (gelf_getphdr(ecp->ein, i, &iphdr) != &iphdr)
errx(EXIT_FAILURE, "gelf_getphdr failed: %s",
elf_errmsg(-1));
if ((seg = calloc(1, sizeof(*seg))) == NULL)
err(EXIT_FAILURE, "calloc failed");
seg->vaddr = iphdr.p_vaddr;
seg->paddr = iphdr.p_paddr;
seg->off = iphdr.p_offset;
seg->fsz = iphdr.p_filesz;
seg->msz = iphdr.p_memsz;
seg->type = iphdr.p_type;
STAILQ_INSERT_TAIL(&ecp->v_seg, seg, seg_list);
}
}

void
copy_phdr(struct elfcopy *ecp)
{
struct segment *seg;
struct section *s;
GElf_Phdr iphdr, ophdr;
int i;

STAILQ_FOREACH(seg, &ecp->v_seg, seg_list) {
if (seg->type == PT_PHDR) {
if (!TAILQ_EMPTY(&ecp->v_sec)) {
s = TAILQ_FIRST(&ecp->v_sec);
if (s->pseudo) {
seg->vaddr = s->vma +
gelf_fsize(ecp->eout, ELF_T_EHDR,
1, EV_CURRENT);
seg->paddr = s->lma +
gelf_fsize(ecp->eout, ELF_T_EHDR,
1, EV_CURRENT);
}
}
seg->fsz = seg->msz = gelf_fsize(ecp->eout, ELF_T_PHDR,
ecp->ophnum, EV_CURRENT);
continue;
}

if (seg->nsec > 0) {
s = seg->v_sec[0];
seg->vaddr = s->vma;
seg->paddr = s->lma;
}

seg->fsz = seg->msz = 0;
for (i = 0; i < seg->nsec; i++) {
s = seg->v_sec[i];
seg->msz = s->vma + s->sz - seg->vaddr;
if (s->type != SHT_NOBITS)
seg->fsz = s->off + s->sz - seg->off;
}
}






if (gelf_newphdr(ecp->eout, ecp->ophnum) == NULL)
errx(EXIT_FAILURE, "gelf_newphdr() failed: %s",
elf_errmsg(-1));






if (elf_update(ecp->eout, ELF_C_NULL) < 0)
errx(EXIT_FAILURE, "elf_update() failed: %s", elf_errmsg(-1));





i = 0;
STAILQ_FOREACH(seg, &ecp->v_seg, seg_list) {
if (i >= ecp->iphnum)
break;
if (gelf_getphdr(ecp->ein, i, &iphdr) != &iphdr)
errx(EXIT_FAILURE, "gelf_getphdr failed: %s",
elf_errmsg(-1));
if (gelf_getphdr(ecp->eout, i, &ophdr) != &ophdr)
errx(EXIT_FAILURE, "gelf_getphdr failed: %s",
elf_errmsg(-1));

ophdr.p_type = iphdr.p_type;
ophdr.p_vaddr = seg->vaddr;
ophdr.p_paddr = seg->paddr;
ophdr.p_flags = iphdr.p_flags;
ophdr.p_align = iphdr.p_align;
ophdr.p_offset = seg->off;
ophdr.p_filesz = seg->fsz;
ophdr.p_memsz = seg->msz;
if (!gelf_update_phdr(ecp->eout, i, &ophdr))
errx(EXIT_FAILURE, "gelf_update_phdr failed: %s",
elf_errmsg(-1));

i++;
}
}
