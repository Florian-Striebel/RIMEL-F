

























#include "_libdwarf.h"

ELFTC_VCSID("$Id: libdwarf_elf_access.c 2070 2011-10-27 03:05:32Z jkoshy $");

int
_dwarf_elf_get_section_info(void *obj, Dwarf_Half ndx,
Dwarf_Obj_Access_Section *ret_section, int *error)
{
Dwarf_Elf_Object *e;
GElf_Shdr *sh;

e = obj;
assert(e != NULL);

if (ret_section == NULL) {
if (error)
*error = DW_DLE_ARGUMENT;
return (DW_DLV_ERROR);
}

if (ndx >= e->eo_seccnt) {
if (error)
*error = DW_DLE_NO_ENTRY;
return (DW_DLV_NO_ENTRY);
}

sh = &e->eo_shdr[ndx];

ret_section->addr = sh->sh_addr;
ret_section->size = sh->sh_size;

ret_section->name = elf_strptr(e->eo_elf, e->eo_strndx, sh->sh_name);
if (ret_section->name == NULL) {
if (error)
*error = DW_DLE_ELF;
return (DW_DLV_ERROR);
}

return (DW_DLV_OK);
}

Dwarf_Endianness
_dwarf_elf_get_byte_order(void *obj)
{
Dwarf_Elf_Object *e;

e = obj;
assert(e != NULL);

switch (e->eo_ehdr.e_ident[EI_DATA]) {
case ELFDATA2MSB:
return (DW_OBJECT_MSB);

case ELFDATA2LSB:
case ELFDATANONE:
default:
return (DW_OBJECT_LSB);
}
}

Dwarf_Small
_dwarf_elf_get_length_size(void *obj)
{
Dwarf_Elf_Object *e;

e = obj;
assert(e != NULL);

if (gelf_getclass(e->eo_elf) == ELFCLASS32)
return (4);
else if (e->eo_ehdr.e_machine == EM_MIPS)
return (8);
else
return (4);
}

Dwarf_Small
_dwarf_elf_get_pointer_size(void *obj)
{
Dwarf_Elf_Object *e;

e = obj;
assert(e != NULL);

if (gelf_getclass(e->eo_elf) == ELFCLASS32)
return (4);
else
return (8);
}

Dwarf_Unsigned
_dwarf_elf_get_section_count(void *obj)
{
Dwarf_Elf_Object *e;

e = obj;
assert(e != NULL);

return (e->eo_seccnt);
}

int
_dwarf_elf_load_section(void *obj, Dwarf_Half ndx, Dwarf_Small** ret_data,
int *error)
{
Dwarf_Elf_Object *e;
Dwarf_Elf_Data *ed;

e = obj;
assert(e != NULL);

if (ret_data == NULL) {
if (error)
*error = DW_DLE_ARGUMENT;
return (DW_DLV_ERROR);
}

if (ndx >= e->eo_seccnt) {
if (error)
*error = DW_DLE_NO_ENTRY;
return (DW_DLV_NO_ENTRY);
}

ed = &e->eo_data[ndx];

if (ed->ed_alloc != NULL)
*ret_data = ed->ed_alloc;
else {
if (ed->ed_data == NULL) {
if (error)
*error = DW_DLE_NO_ENTRY;
return (DW_DLV_NO_ENTRY);
}
*ret_data = ed->ed_data->d_buf;
}

return (DW_DLV_OK);
}
