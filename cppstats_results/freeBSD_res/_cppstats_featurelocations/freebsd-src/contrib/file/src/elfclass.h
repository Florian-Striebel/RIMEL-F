

























if (nbytes <= sizeof(elfhdr))
return 0;

u.l = 1;
(void)memcpy(&elfhdr, buf, sizeof elfhdr);
swap = (u.c[sizeof(int32_t) - 1] + 1) != elfhdr.e_ident[EI_DATA];

type = elf_getu16(swap, elfhdr.e_type);
notecount = ms->elf_notes_max;
switch (type) {
#if defined(ELFCORE)
case ET_CORE:
phnum = elf_getu16(swap, elfhdr.e_phnum);
if (phnum > ms->elf_phnum_max)
return toomany(ms, "program headers", phnum);
flags |= FLAGS_IS_CORE;
if (dophn_core(ms, clazz, swap, fd,
CAST(off_t, elf_getu(swap, elfhdr.e_phoff)), phnum,
CAST(size_t, elf_getu16(swap, elfhdr.e_phentsize)),
fsize, &flags, &notecount) == -1)
return -1;
break;
#endif
case ET_EXEC:
case ET_DYN:
phnum = elf_getu16(swap, elfhdr.e_phnum);
if (phnum > ms->elf_phnum_max)
return toomany(ms, "program", phnum);
shnum = elf_getu16(swap, elfhdr.e_shnum);
if (shnum > ms->elf_shnum_max)
return toomany(ms, "section", shnum);
if (dophn_exec(ms, clazz, swap, fd,
CAST(off_t, elf_getu(swap, elfhdr.e_phoff)), phnum,
CAST(size_t, elf_getu16(swap, elfhdr.e_phentsize)),
fsize, shnum, &flags, &notecount) == -1)
return -1;

case ET_REL:
shnum = elf_getu16(swap, elfhdr.e_shnum);
if (shnum > ms->elf_shnum_max)
return toomany(ms, "section headers", shnum);
if (doshn(ms, clazz, swap, fd,
CAST(off_t, elf_getu(swap, elfhdr.e_shoff)), shnum,
CAST(size_t, elf_getu16(swap, elfhdr.e_shentsize)),
fsize, elf_getu16(swap, elfhdr.e_machine),
CAST(int, elf_getu16(swap, elfhdr.e_shstrndx)),
&flags, &notecount) == -1)
return -1;
break;

default:
break;
}
if (notecount == 0)
return toomany(ms, "notes", ms->elf_notes_max);
return 1;
