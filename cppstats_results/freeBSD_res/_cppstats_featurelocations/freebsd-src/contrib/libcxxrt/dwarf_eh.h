
































#include <assert.h>







#if !defined(_GNU_SOURCE)
#define _GNU_SOURCE 1
#include "unwind.h"
#undef _GNU_SOURCE
#else
#include "unwind.h"
#endif

#include <stdint.h>


typedef unsigned char *dw_eh_ptr_t;


#define DW_EH_PE_signed 0x08

enum dwarf_data_encoding
{

DW_EH_PE_absptr = 0x00,

DW_EH_PE_uleb128 = 0x01,

DW_EH_PE_udata2 = 0x02,

DW_EH_PE_udata4 = 0x03,

DW_EH_PE_udata8 = 0x04,

DW_EH_PE_sleb128 = DW_EH_PE_uleb128 | DW_EH_PE_signed,

DW_EH_PE_sdata2 = DW_EH_PE_udata2 | DW_EH_PE_signed,

DW_EH_PE_sdata4 = DW_EH_PE_udata4 | DW_EH_PE_signed,

DW_EH_PE_sdata8 = DW_EH_PE_udata8 | DW_EH_PE_signed
};





static inline enum dwarf_data_encoding get_encoding(unsigned char x)
{
return static_cast<enum dwarf_data_encoding>(x & 0xf);
}








enum dwarf_data_relative
{

DW_EH_PE_omit = 0xff,

DW_EH_PE_pcrel = 0x10,

DW_EH_PE_textrel = 0x20,

DW_EH_PE_datarel = 0x30,

DW_EH_PE_funcrel = 0x40,

DW_EH_PE_aligned = 0x50,

DW_EH_PE_indirect = 0x80
};



static inline enum dwarf_data_relative get_base(unsigned char x)
{
return static_cast<enum dwarf_data_relative>(x & 0x70);
}



static int is_indirect(unsigned char x)
{
return ((x & DW_EH_PE_indirect) == DW_EH_PE_indirect);
}





static inline int dwarf_size_of_fixed_size_field(unsigned char type)
{
switch (get_encoding(type))
{
default: abort();
case DW_EH_PE_sdata2:
case DW_EH_PE_udata2: return 2;
case DW_EH_PE_sdata4:
case DW_EH_PE_udata4: return 4;
case DW_EH_PE_sdata8:
case DW_EH_PE_udata8: return 8;
case DW_EH_PE_absptr: return sizeof(void*);
}
}










static uint64_t read_leb128(dw_eh_ptr_t *data, int *b)
{
uint64_t uleb = 0;
unsigned int bit = 0;
unsigned char digit = 0;


do
{


assert(bit < sizeof(uint64_t) * 8);

digit = (**data) & 0x7f;

uleb += digit << bit;

bit += 7;

(*data)++;


} while ((*(*data - 1)) != digit);
*b = bit;

return uleb;
}






static int64_t read_uleb128(dw_eh_ptr_t *data)
{
int b;
return read_leb128(data, &b);
}






static int64_t read_sleb128(dw_eh_ptr_t *data)
{
int bits;

uint64_t uleb = read_leb128(data, &bits);

if ((uleb >> (bits-1)) == 1)
{

uleb |= static_cast<int64_t>(-1) << bits;
}
return static_cast<int64_t>(uleb);
}





static uint64_t read_value(char encoding, dw_eh_ptr_t *data)
{
enum dwarf_data_encoding type = get_encoding(encoding);
switch (type)
{

#define READ(dwarf, type) case dwarf:{type t;memcpy(&t, *data, sizeof t);*data += sizeof t;return static_cast<uint64_t>(t);}







READ(DW_EH_PE_udata2, uint16_t)
READ(DW_EH_PE_udata4, uint32_t)
READ(DW_EH_PE_udata8, uint64_t)
READ(DW_EH_PE_sdata2, int16_t)
READ(DW_EH_PE_sdata4, int32_t)
READ(DW_EH_PE_sdata8, int64_t)
READ(DW_EH_PE_absptr, intptr_t)
#undef READ

case DW_EH_PE_sleb128:
return read_sleb128(data);
case DW_EH_PE_uleb128:
return read_uleb128(data);
default: abort();
}
}








static uint64_t resolve_indirect_value(_Unwind_Context *c,
unsigned char encoding,
int64_t v,
dw_eh_ptr_t start)
{
switch (get_base(encoding))
{
case DW_EH_PE_pcrel:
v += reinterpret_cast<uint64_t>(start);
break;
case DW_EH_PE_textrel:
v += static_cast<uint64_t>(static_cast<uintptr_t>(_Unwind_GetTextRelBase(c)));
break;
case DW_EH_PE_datarel:
v += static_cast<uint64_t>(static_cast<uintptr_t>(_Unwind_GetDataRelBase(c)));
break;
case DW_EH_PE_funcrel:
v += static_cast<uint64_t>(static_cast<uintptr_t>(_Unwind_GetRegionStart(c)));
default:
break;
}




if (is_indirect(encoding))
{
v = static_cast<uint64_t>(reinterpret_cast<uintptr_t>(*reinterpret_cast<void**>(v)));
}
return v;
}





static inline void read_value_with_encoding(_Unwind_Context *context,
dw_eh_ptr_t *data,
uint64_t *out)
{
dw_eh_ptr_t start = *data;
unsigned char encoding = *((*data)++);

if (encoding == DW_EH_PE_omit) { return; }

*out = read_value(encoding, data);
*out = resolve_indirect_value(context, encoding, *out, start);
}











struct dwarf_eh_lsda
{


dw_eh_ptr_t region_start;

dw_eh_ptr_t landing_pads;

dw_eh_ptr_t type_table;

unsigned char type_table_encoding;

dw_eh_ptr_t call_site_table;

dw_eh_ptr_t action_table;

unsigned char callsite_encoding;
};





static inline struct dwarf_eh_lsda parse_lsda(_Unwind_Context *context,
unsigned char *data)
{
struct dwarf_eh_lsda lsda;

lsda.region_start = reinterpret_cast<dw_eh_ptr_t>(_Unwind_GetRegionStart(context));




uint64_t v = static_cast<uint64_t>(reinterpret_cast<uintptr_t>(lsda.region_start));
read_value_with_encoding(context, &data, &v);
lsda.landing_pads = reinterpret_cast<dw_eh_ptr_t>(static_cast<uintptr_t>(v));





lsda.type_table = 0;
lsda.type_table_encoding = *data++;
if (lsda.type_table_encoding != DW_EH_PE_omit)
{
v = read_uleb128(&data);
dw_eh_ptr_t type_table = data;
type_table += v;
lsda.type_table = type_table;

}
#if defined(__arm__) && !defined(__ARM_DWARF_EH__)
lsda.type_table_encoding = (DW_EH_PE_pcrel | DW_EH_PE_indirect);
#endif

lsda.callsite_encoding = static_cast<enum dwarf_data_encoding>(*(data++));


lsda.action_table = data;
uintptr_t callsite_size = static_cast<uintptr_t>(read_uleb128(&data));
lsda.action_table = data + callsite_size;

lsda.call_site_table = static_cast<dw_eh_ptr_t>(data);


return lsda;
}






struct dwarf_eh_action
{




dw_eh_ptr_t landing_pad;

dw_eh_ptr_t action_record;
};








static bool dwarf_eh_find_callsite(struct _Unwind_Context *context,
struct dwarf_eh_lsda *lsda,
struct dwarf_eh_action *result)
{
result->action_record = 0;
result->landing_pad = 0;

uint64_t ip = _Unwind_GetIP(context) - _Unwind_GetRegionStart(context);
unsigned char *callsite_table = static_cast<unsigned char*>(lsda->call_site_table);

while (callsite_table <= lsda->action_table)
{

uint64_t call_site_start, call_site_size, landing_pad, action;
call_site_start = read_value(lsda->callsite_encoding, &callsite_table);
call_site_size = read_value(lsda->callsite_encoding, &callsite_table);





if (call_site_start > ip) { break; }



landing_pad = read_value(lsda->callsite_encoding, &callsite_table);
action = read_uleb128(&callsite_table);









if (call_site_start < ip && ip <= call_site_start + call_site_size)
{
if (action)
{


result->action_record = lsda->action_table + action - 1;
}

if (landing_pad)
{

result->landing_pad = lsda->landing_pads + landing_pad;
}
return true;
}
}
return false;
}


#define EXCEPTION_CLASS(a,b,c,d,e,f,g,h) ((static_cast<uint64_t>(a) << 56) +(static_cast<uint64_t>(b) << 48) +(static_cast<uint64_t>(c) << 40) +(static_cast<uint64_t>(d) << 32) +(static_cast<uint64_t>(e) << 24) +(static_cast<uint64_t>(f) << 16) +(static_cast<uint64_t>(g) << 8) +(static_cast<uint64_t>(h)))









#define GENERIC_EXCEPTION_CLASS(e,f,g,h) (static_cast<uint32_t>(e) << 24) +(static_cast<uint32_t>(f) << 16) +(static_cast<uint32_t>(g) << 8) +(static_cast<uint32_t>(h))




