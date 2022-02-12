

























#include "_libdwarf.h"

ELFTC_VCSID("$Id: dwarf_pro_attr.c 2074 2011-10-27 03:34:33Z jkoshy $");

Dwarf_P_Attribute
dwarf_add_AT_location_expr(Dwarf_P_Debug dbg, Dwarf_P_Die die, Dwarf_Half attr,
Dwarf_P_Expr loc_expr, Dwarf_Error *error)
{
Dwarf_Attribute at;

if (dbg == NULL || die == NULL || loc_expr == NULL) {
DWARF_SET_ERROR(dbg, error, DW_DLE_ARGUMENT);
return (DW_DLV_BADADDR);
}

if (_dwarf_attr_alloc(die, &at, error) != DW_DLE_NONE)
return (DW_DLV_BADADDR);

at->at_die = die;
at->at_attrib = attr;
at->at_expr = loc_expr;

if (_dwarf_expr_into_block(loc_expr, error) != DW_DLE_NONE) {
free(at);
return (DW_DLV_BADADDR);
}
at->u[0].u64 = loc_expr->pe_length;
at->u[1].u8p = loc_expr->pe_block;
if (loc_expr->pe_length <= UCHAR_MAX)
at->at_form = DW_FORM_block1;
else if (loc_expr->pe_length <= USHRT_MAX)
at->at_form = DW_FORM_block2;
else if (loc_expr->pe_length <= UINT_MAX)
at->at_form = DW_FORM_block4;
else
at->at_form = DW_FORM_block;

STAILQ_INSERT_TAIL(&die->die_attr, at, at_next);

return (at);
}

Dwarf_P_Attribute
dwarf_add_AT_name(Dwarf_P_Die die, char *name, Dwarf_Error *error)
{
Dwarf_Attribute at;

if (_dwarf_add_string_attr(die, &at, DW_AT_name, name, error) !=
DW_DLE_NONE)
return (DW_DLV_BADADDR);

return (at);
}

Dwarf_P_Attribute
dwarf_add_AT_comp_dir(Dwarf_P_Die die, char *dir, Dwarf_Error *error)
{
Dwarf_Attribute at;

if (_dwarf_add_string_attr(die, &at, DW_AT_comp_dir, dir, error) !=
DW_DLE_NONE)
return (DW_DLV_BADADDR);

return (at);
}

Dwarf_P_Attribute
dwarf_add_AT_producer(Dwarf_P_Die die, char *producer, Dwarf_Error *error)
{
Dwarf_Attribute at;

if (_dwarf_add_string_attr(die, &at, DW_AT_producer, producer, error) !=
DW_DLE_NONE)
return (DW_DLV_BADADDR);

return (at);
}

Dwarf_P_Attribute
dwarf_add_AT_const_value_signedint(Dwarf_P_Die die, Dwarf_Signed value,
Dwarf_Error *error)
{
Dwarf_Attribute at;
Dwarf_Debug dbg;

dbg = die != NULL ? die->die_dbg : NULL;

if (die == NULL) {
DWARF_SET_ERROR(dbg, error, DW_DLE_ARGUMENT);
return (DW_DLV_BADADDR);
}

if (_dwarf_attr_alloc(die, &at, error) != DW_DLE_NONE)
return (DW_DLV_BADADDR);

at->at_die = die;
at->at_attrib = DW_AT_const_value;
at->at_form = DW_FORM_sdata;
at->u[0].s64 = value;

STAILQ_INSERT_TAIL(&die->die_attr, at, at_next);

return (at);
}

Dwarf_P_Attribute
dwarf_add_AT_const_value_unsignedint(Dwarf_P_Die die, Dwarf_Unsigned value,
Dwarf_Error *error)
{
Dwarf_Attribute at;
Dwarf_Debug dbg;

dbg = die != NULL ? die->die_dbg : NULL;

if (die == NULL) {
DWARF_SET_ERROR(dbg, error, DW_DLE_ARGUMENT);
return (DW_DLV_BADADDR);
}

if (_dwarf_attr_alloc(die, &at, error) != DW_DLE_NONE)
return (DW_DLV_BADADDR);

at->at_die = die;
at->at_attrib = DW_AT_const_value;
at->at_form = DW_FORM_udata;
at->u[0].u64 = value;

STAILQ_INSERT_TAIL(&die->die_attr, at, at_next);

return (at);
}

Dwarf_P_Attribute
dwarf_add_AT_const_value_string(Dwarf_P_Die die, char *string,
Dwarf_Error *error)
{
Dwarf_Attribute at;

if (_dwarf_add_string_attr(die, &at, DW_AT_const_value, string,
error) != DW_DLE_NONE)
return (DW_DLV_BADADDR);

return (at);
}

Dwarf_P_Attribute
dwarf_add_AT_targ_address(Dwarf_P_Debug dbg, Dwarf_P_Die die, Dwarf_Half attr,
Dwarf_Unsigned pc_value, Dwarf_Signed sym_index, Dwarf_Error *error)
{

return (dwarf_add_AT_targ_address_b(dbg, die, attr, pc_value, sym_index,
error));
}

Dwarf_P_Attribute
dwarf_add_AT_targ_address_b(Dwarf_P_Debug dbg, Dwarf_P_Die die, Dwarf_Half attr,
Dwarf_Unsigned pc_value, Dwarf_Unsigned sym_index, Dwarf_Error *error)
{
Dwarf_Attribute at;

if (dbg == NULL || die == NULL) {
DWARF_SET_ERROR(dbg, error, DW_DLE_ARGUMENT);
return (DW_DLV_BADADDR);
}

if (_dwarf_attr_alloc(die, &at, error) != DW_DLE_NONE)
return (DW_DLV_BADADDR);

at->at_die = die;
at->at_attrib = attr;
at->at_form = DW_FORM_addr;
at->at_relsym = sym_index;
at->u[0].u64 = pc_value;

STAILQ_INSERT_TAIL(&die->die_attr, at, at_next);

return (at);
}

Dwarf_P_Attribute
dwarf_add_AT_dataref(Dwarf_P_Debug dbg, Dwarf_P_Die die, Dwarf_Half attr,
Dwarf_Unsigned pc_value, Dwarf_Unsigned sym_index, Dwarf_Error *error)
{
Dwarf_Attribute at;
int ret;

if (dbg == NULL || die == NULL) {
DWARF_SET_ERROR(dbg, error, DW_DLE_ARGUMENT);
return (DW_DLV_BADADDR);
}

ret = _dwarf_add_AT_dataref(dbg, die, attr, pc_value, sym_index,
NULL, &at, error);
if (ret != DW_DLE_NONE)
return (DW_DLV_BADADDR);

return (at);

}

Dwarf_P_Attribute
dwarf_add_AT_ref_address(Dwarf_P_Debug dbg, Dwarf_P_Die die, Dwarf_Half attr,
Dwarf_Unsigned pc_value, Dwarf_Unsigned sym_index, Dwarf_Error *error)
{
Dwarf_Attribute at;

if (dbg == NULL || die == NULL) {
DWARF_SET_ERROR(dbg, error, DW_DLE_ARGUMENT);
return (DW_DLV_BADADDR);
}

if (_dwarf_attr_alloc(die, &at, error) != DW_DLE_NONE)
return (DW_DLV_BADADDR);

at->at_die = die;
at->at_attrib = attr;
at->at_form = DW_FORM_ref_addr;
at->at_relsym = sym_index;
at->u[0].u64 = pc_value;

STAILQ_INSERT_TAIL(&die->die_attr, at, at_next);

return (at);
}

Dwarf_P_Attribute
dwarf_add_AT_unsigned_const(Dwarf_P_Debug dbg, Dwarf_P_Die die, Dwarf_Half attr,
Dwarf_Unsigned value, Dwarf_Error *error)
{
Dwarf_Attribute at;

if (dbg == NULL || die == NULL) {
DWARF_SET_ERROR(dbg, error, DW_DLE_ARGUMENT);
return (DW_DLV_BADADDR);
}

if (_dwarf_attr_alloc(die, &at, error) != DW_DLE_NONE)
return (DW_DLV_BADADDR);

at->at_die = die;
at->at_attrib = attr;
at->u[0].u64 = value;

if (value <= UCHAR_MAX)
at->at_form = DW_FORM_data1;
else if (value <= USHRT_MAX)
at->at_form = DW_FORM_data2;
else if (value <= UINT_MAX)
at->at_form = DW_FORM_data4;
else
at->at_form = DW_FORM_data8;

STAILQ_INSERT_TAIL(&die->die_attr, at, at_next);

return (at);
}

Dwarf_P_Attribute
dwarf_add_AT_signed_const(Dwarf_P_Debug dbg, Dwarf_P_Die die, Dwarf_Half attr,
Dwarf_Signed value, Dwarf_Error *error)
{
Dwarf_Attribute at;

if (dbg == NULL || die == NULL) {
DWARF_SET_ERROR(dbg, error, DW_DLE_ARGUMENT);
return (DW_DLV_BADADDR);
}

if (_dwarf_attr_alloc(die, &at, error) != DW_DLE_NONE)
return (DW_DLV_BADADDR);

at->at_die = die;
at->at_attrib = attr;
at->u[0].u64 = value;

if (value >= SCHAR_MIN && value <= SCHAR_MAX)
at->at_form = DW_FORM_data1;
else if (value >= SHRT_MIN && value <= SHRT_MAX)
at->at_form = DW_FORM_data2;
else if (value >= INT_MIN && value <= INT_MAX)
at->at_form = DW_FORM_data4;
else
at->at_form = DW_FORM_data8;

STAILQ_INSERT_TAIL(&die->die_attr, at, at_next);

return (at);
}

Dwarf_P_Attribute
dwarf_add_AT_reference(Dwarf_P_Debug dbg, Dwarf_P_Die die, Dwarf_Half attr,
Dwarf_P_Die ref_die, Dwarf_Error *error)
{
Dwarf_Attribute at;

if (dbg == NULL || die == NULL) {
DWARF_SET_ERROR(dbg, error, DW_DLE_ARGUMENT);
return (DW_DLV_BADADDR);
}

if (_dwarf_attr_alloc(die, &at, error) != DW_DLE_NONE)
return (DW_DLV_BADADDR);

at->at_die = die;
at->at_attrib = attr;
if (dbg->dbg_offset_size == 4)
at->at_form = DW_FORM_ref4;
else
at->at_form = DW_FORM_ref8;

at->at_refdie = ref_die;

STAILQ_INSERT_TAIL(&die->die_attr, at, at_next);

return (at);
}

Dwarf_P_Attribute
dwarf_add_AT_flag(Dwarf_P_Debug dbg, Dwarf_P_Die die, Dwarf_Half attr,
Dwarf_Small flag, Dwarf_Error *error)
{
Dwarf_Attribute at;

if (dbg == NULL || die == NULL) {
DWARF_SET_ERROR(dbg, error, DW_DLE_ARGUMENT);
return (DW_DLV_BADADDR);
}

if (_dwarf_attr_alloc(die, &at, error) != DW_DLE_NONE)
return (DW_DLV_BADADDR);

at->at_die = die;
at->at_attrib = attr;
at->at_form = DW_FORM_flag;
at->u[0].u64 = flag ? 1 : 0;

STAILQ_INSERT_TAIL(&die->die_attr, at, at_next);

return (at);
}

Dwarf_P_Attribute
dwarf_add_AT_string(Dwarf_P_Debug dbg, Dwarf_P_Die die, Dwarf_Half attr,
char *string, Dwarf_Error *error)
{
Dwarf_Attribute at;

if (dbg == NULL || die == NULL) {
DWARF_SET_ERROR(dbg, error, DW_DLE_ARGUMENT);
return (DW_DLV_BADADDR);
}



if (_dwarf_add_string_attr(die, &at, attr, string, error) !=
DW_DLE_NONE)
return (DW_DLV_BADADDR);

return (at);
}
