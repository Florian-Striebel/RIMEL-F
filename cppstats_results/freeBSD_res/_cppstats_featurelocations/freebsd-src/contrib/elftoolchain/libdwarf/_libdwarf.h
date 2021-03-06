




























#if !defined(__LIBDWARF_H_)
#define __LIBDWARF_H_

#include <sys/param.h>
#include <sys/queue.h>
#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gelf.h>
#include "dwarf.h"
#include "libdwarf.h"
#include "uthash.h"

#include "_elftc.h"

#define DWARF_DIE_HASH_SIZE 8191

struct _libdwarf_globals {
Dwarf_Handler errhand;
Dwarf_Ptr errarg;
int applyreloc;
};

extern struct _libdwarf_globals _libdwarf;

#define _DWARF_SET_ERROR(_d, _e, _err, _elf_err) _dwarf_set_error(_d, _e, _err, _elf_err, __func__, __LINE__)

#define DWARF_SET_ERROR(_d, _e, _err) _DWARF_SET_ERROR(_d, _e, _err, 0)

#define DWARF_SET_ELF_ERROR(_d, _e) _DWARF_SET_ERROR(_d, _e, DW_DLE_ELF, elf_errno())





#define WRITE_VALUE(value, bytes) dbg->write_alloc(&ds->ds_data, &ds->ds_cap, &ds->ds_size, (value), (bytes), error)


#define WRITE_ULEB128(value) _dwarf_write_uleb128_alloc(&ds->ds_data, &ds->ds_cap, &ds->ds_size, (value), error)


#define WRITE_SLEB128(value) _dwarf_write_sleb128_alloc(&ds->ds_data, &ds->ds_cap, &ds->ds_size, (value), error)


#define WRITE_STRING(string) _dwarf_write_string_alloc(&ds->ds_data, &ds->ds_cap, &ds->ds_size, (string), error)


#define WRITE_BLOCK(blk, size) _dwarf_write_block_alloc(&ds->ds_data, &ds->ds_cap, &ds->ds_size, (blk), (size), error)


#define WRITE_PADDING(byte, cnt) _dwarf_write_padding_alloc(&ds->ds_data, &ds->ds_cap, &ds->ds_size, (byte), (cnt), error)


#define RCHECK(expr) do { ret = expr; if (ret != DW_DLE_NONE) goto gen_fail; } while(0)






typedef struct _Dwarf_CU *Dwarf_CU;

struct _Dwarf_AttrDef {
Dwarf_Half ad_attrib;
Dwarf_Half ad_form;
uint64_t ad_offset;
STAILQ_ENTRY(_Dwarf_AttrDef) ad_next;
};

struct _Dwarf_Attribute {
Dwarf_Die at_die;
Dwarf_Die at_refdie;
uint64_t at_offset;
Dwarf_Half at_attrib;
Dwarf_Half at_form;
int at_indirect;
union {
uint64_t u64;
int64_t s64;
char *s;
uint8_t *u8p;
} u[2];
Dwarf_Block at_block;
Dwarf_Locdesc *at_ld;
Dwarf_P_Expr at_expr;
uint64_t at_relsym;
const char *at_relsec;
STAILQ_ENTRY(_Dwarf_Attribute) at_next;
};

struct _Dwarf_Abbrev {
uint64_t ab_entry;
uint64_t ab_tag;
uint8_t ab_children;
uint64_t ab_offset;
uint64_t ab_length;
uint64_t ab_atnum;
UT_hash_handle ab_hh;
STAILQ_HEAD(, _Dwarf_AttrDef) ab_attrdef;
};

struct _Dwarf_Die {
Dwarf_Die die_parent;
Dwarf_Die die_child;
Dwarf_Die die_left;
Dwarf_Die die_right;
uint64_t die_offset;
uint64_t die_next_off;
uint64_t die_abnum;
Dwarf_Abbrev die_ab;
Dwarf_Tag die_tag;
Dwarf_Debug die_dbg;
Dwarf_CU die_cu;
char *die_name;
Dwarf_Attribute *die_attrarray;
STAILQ_HEAD(, _Dwarf_Attribute) die_attr;
STAILQ_ENTRY(_Dwarf_Die) die_pro_next;
};

struct _Dwarf_P_Expr_Entry {
Dwarf_Loc ee_loc;
Dwarf_Unsigned ee_sym;
STAILQ_ENTRY(_Dwarf_P_Expr_Entry) ee_next;
};

struct _Dwarf_P_Expr {
Dwarf_Debug pe_dbg;
uint8_t *pe_block;
int pe_invalid;
Dwarf_Unsigned pe_length;
STAILQ_HEAD(, _Dwarf_P_Expr_Entry) pe_eelist;
STAILQ_ENTRY(_Dwarf_P_Expr) pe_next;
};

struct _Dwarf_Line {
Dwarf_LineInfo ln_li;
Dwarf_Addr ln_addr;
Dwarf_Unsigned ln_symndx;
Dwarf_Unsigned ln_fileno;
Dwarf_Unsigned ln_lineno;
Dwarf_Signed ln_column;
Dwarf_Bool ln_bblock;
Dwarf_Bool ln_stmt;
Dwarf_Bool ln_endseq;
STAILQ_ENTRY(_Dwarf_Line) ln_next;
};

struct _Dwarf_LineFile {
char *lf_fname;
char *lf_fullpath;
Dwarf_Unsigned lf_dirndx;
Dwarf_Unsigned lf_mtime;
Dwarf_Unsigned lf_size;
STAILQ_ENTRY(_Dwarf_LineFile) lf_next;
};

struct _Dwarf_LineInfo {
Dwarf_Unsigned li_length;
Dwarf_Half li_version;
Dwarf_Unsigned li_hdrlen;
Dwarf_Small li_minlen;
Dwarf_Small li_maxop;
Dwarf_Small li_defstmt;
int8_t li_lbase;
Dwarf_Small li_lrange;
Dwarf_Small li_opbase;
Dwarf_Small *li_oplen;
char **li_incdirs;
Dwarf_Unsigned li_inclen;
char **li_lfnarray;
Dwarf_Unsigned li_lflen;
STAILQ_HEAD(, _Dwarf_LineFile) li_lflist;
Dwarf_Line *li_lnarray;
Dwarf_Unsigned li_lnlen;
STAILQ_HEAD(, _Dwarf_Line) li_lnlist;
};

struct _Dwarf_NamePair {
Dwarf_NameTbl np_nt;
Dwarf_Die np_die;
Dwarf_Unsigned np_offset;
char *np_name;
STAILQ_ENTRY(_Dwarf_NamePair) np_next;
};

struct _Dwarf_NameTbl {
Dwarf_Unsigned nt_length;
Dwarf_Half nt_version;
Dwarf_CU nt_cu;
Dwarf_Off nt_cu_offset;
Dwarf_Unsigned nt_cu_length;
STAILQ_HEAD(, _Dwarf_NamePair) nt_nplist;
STAILQ_ENTRY(_Dwarf_NameTbl) nt_next;
};

struct _Dwarf_NameSec {
STAILQ_HEAD(, _Dwarf_NameTbl) ns_ntlist;
Dwarf_NamePair *ns_array;
Dwarf_Unsigned ns_len;
};

struct _Dwarf_Fde {
Dwarf_Debug fde_dbg;
Dwarf_Cie fde_cie;
Dwarf_FrameSec fde_fs;
Dwarf_Ptr fde_addr;
Dwarf_Unsigned fde_offset;
Dwarf_Unsigned fde_length;
Dwarf_Unsigned fde_cieoff;
Dwarf_Unsigned fde_initloc;
Dwarf_Unsigned fde_adrange;
Dwarf_Unsigned fde_auglen;
uint8_t *fde_augdata;
uint8_t *fde_inst;
Dwarf_Unsigned fde_instlen;
Dwarf_Unsigned fde_instcap;
Dwarf_Unsigned fde_symndx;
Dwarf_Unsigned fde_esymndx;
Dwarf_Addr fde_eoff;
STAILQ_ENTRY(_Dwarf_Fde) fde_next;
};

struct _Dwarf_Cie {
Dwarf_Debug cie_dbg;
Dwarf_Unsigned cie_index;
Dwarf_Unsigned cie_offset;
Dwarf_Unsigned cie_length;
Dwarf_Half cie_version;
uint8_t *cie_augment;
Dwarf_Unsigned cie_ehdata;
uint8_t cie_addrsize;
uint8_t cie_segmentsize;
Dwarf_Unsigned cie_caf;
Dwarf_Signed cie_daf;
Dwarf_Unsigned cie_ra;
Dwarf_Unsigned cie_auglen;
uint8_t *cie_augdata;
uint8_t cie_fde_encode;
Dwarf_Ptr cie_initinst;
Dwarf_Unsigned cie_instlen;
STAILQ_ENTRY(_Dwarf_Cie) cie_next;
};

struct _Dwarf_FrameSec {
STAILQ_HEAD(, _Dwarf_Cie) fs_cielist;
STAILQ_HEAD(, _Dwarf_Fde) fs_fdelist;
Dwarf_Cie *fs_ciearray;
Dwarf_Unsigned fs_cielen;
Dwarf_Fde *fs_fdearray;
Dwarf_Unsigned fs_fdelen;
};

struct _Dwarf_Arange {
Dwarf_ArangeSet ar_as;
Dwarf_Unsigned ar_address;
Dwarf_Unsigned ar_range;
Dwarf_Unsigned ar_symndx;
Dwarf_Unsigned ar_esymndx;
Dwarf_Addr ar_eoff;
STAILQ_ENTRY(_Dwarf_Arange) ar_next;
};

struct _Dwarf_ArangeSet {
Dwarf_Unsigned as_length;
Dwarf_Half as_version;
Dwarf_Off as_cu_offset;
Dwarf_CU as_cu;
Dwarf_Small as_addrsz;
Dwarf_Small as_segsz;
STAILQ_HEAD (, _Dwarf_Arange) as_arlist;
STAILQ_ENTRY(_Dwarf_ArangeSet) as_next;
};

struct _Dwarf_MacroSet {
Dwarf_Macro_Details *ms_mdlist;
Dwarf_Unsigned ms_cnt;
STAILQ_ENTRY(_Dwarf_MacroSet) ms_next;
};

struct _Dwarf_Rangelist {
Dwarf_CU rl_cu;
Dwarf_Unsigned rl_offset;
Dwarf_Ranges *rl_rgarray;
Dwarf_Unsigned rl_rglen;
STAILQ_ENTRY(_Dwarf_Rangelist) rl_next;
};

struct _Dwarf_CU {
Dwarf_Debug cu_dbg;
Dwarf_Off cu_offset;
uint32_t cu_length;
uint16_t cu_length_size;
uint16_t cu_version;
uint64_t cu_abbrev_offset;
uint64_t cu_abbrev_offset_cur;
int cu_abbrev_loaded;
uint64_t cu_abbrev_cnt;
uint64_t cu_lineno_offset;
uint8_t cu_pointer_size;
uint8_t cu_dwarf_size;
Dwarf_Sig8 cu_type_sig;
uint64_t cu_type_offset;
Dwarf_Off cu_next_offset;
uint64_t cu_1st_offset;
int cu_pass2;
Dwarf_LineInfo cu_lineinfo;
Dwarf_Abbrev cu_abbrev_hash;
Dwarf_Bool cu_is_info;
STAILQ_ENTRY(_Dwarf_CU) cu_next;
};

typedef struct _Dwarf_Section {
const char *ds_name;
Dwarf_Small *ds_data;
Dwarf_Unsigned ds_addr;
Dwarf_Unsigned ds_size;
} Dwarf_Section;

typedef struct _Dwarf_P_Section {
char *ds_name;
Dwarf_Small *ds_data;
Dwarf_Unsigned ds_size;
Dwarf_Unsigned ds_cap;
Dwarf_Unsigned ds_ndx;
Dwarf_Unsigned ds_symndx;
STAILQ_ENTRY(_Dwarf_P_Section) ds_next;
} *Dwarf_P_Section;

typedef struct _Dwarf_Rel_Entry {
unsigned char dre_type;
unsigned char dre_length;
Dwarf_Unsigned dre_offset;
Dwarf_Unsigned dre_addend;
Dwarf_Unsigned dre_symndx;
const char *dre_secname;
STAILQ_ENTRY(_Dwarf_Rel_Entry) dre_next;
} *Dwarf_Rel_Entry;

typedef struct _Dwarf_Rel_Section {
struct _Dwarf_P_Section *drs_ds;
struct _Dwarf_P_Section *drs_ref;
struct Dwarf_Relocation_Data_s *drs_drd;
STAILQ_HEAD(, _Dwarf_Rel_Entry) drs_dre;
Dwarf_Unsigned drs_drecnt;
Dwarf_Unsigned drs_size;
int drs_addend;
STAILQ_ENTRY(_Dwarf_Rel_Section) drs_next;
} *Dwarf_Rel_Section;

typedef struct {
Elf_Data *ed_data;
void *ed_alloc;
size_t ed_size;
} Dwarf_Elf_Data;

typedef struct {
Elf *eo_elf;
GElf_Ehdr eo_ehdr;
GElf_Shdr *eo_shdr;
Dwarf_Elf_Data *eo_data;
Dwarf_Unsigned eo_seccnt;
size_t eo_strndx;
Dwarf_Obj_Access_Methods eo_methods;
} Dwarf_Elf_Object;

struct _Dwarf_Debug {
Dwarf_Obj_Access_Interface *dbg_iface;
Dwarf_Section *dbg_section;
Dwarf_Section *dbg_info_sec;
Dwarf_Off dbg_info_off;
Dwarf_Section *dbg_types_sec;
Dwarf_Off dbg_types_off;
Dwarf_Unsigned dbg_seccnt;
int dbg_mode;
int dbg_pointer_size;
int dbg_offset_size;
int dbg_info_loaded;
int dbg_types_loaded;
Dwarf_Half dbg_machine;
Dwarf_Handler dbg_errhand;
Dwarf_Ptr dbg_errarg;
STAILQ_HEAD(, _Dwarf_CU) dbg_cu;
STAILQ_HEAD(, _Dwarf_CU) dbg_tu;
Dwarf_CU dbg_cu_current;
Dwarf_CU dbg_tu_current;
Dwarf_NameSec dbg_globals;
Dwarf_NameSec dbg_pubtypes;
Dwarf_NameSec dbg_weaks;
Dwarf_NameSec dbg_funcs;
Dwarf_NameSec dbg_vars;
Dwarf_NameSec dbg_types;
Dwarf_FrameSec dbg_frame;
Dwarf_FrameSec dbg_eh_frame;
STAILQ_HEAD(, _Dwarf_ArangeSet) dbg_aslist;
Dwarf_Arange *dbg_arange_array;
Dwarf_Unsigned dbg_arange_cnt;
char *dbg_strtab;
Dwarf_Unsigned dbg_strtab_cap;
Dwarf_Unsigned dbg_strtab_size;
STAILQ_HEAD(, _Dwarf_MacroSet) dbg_mslist;
STAILQ_HEAD(, _Dwarf_Rangelist) dbg_rllist;
uint64_t (*read)(uint8_t *, uint64_t *, int);
void (*write)(uint8_t *, uint64_t *, uint64_t, int);
int (*write_alloc)(uint8_t **, uint64_t *, uint64_t *,
uint64_t, int, Dwarf_Error *);
uint64_t (*decode)(uint8_t **, int);

Dwarf_Half dbg_frame_rule_table_size;
Dwarf_Half dbg_frame_rule_initial_value;
Dwarf_Half dbg_frame_cfa_value;
Dwarf_Half dbg_frame_same_value;
Dwarf_Half dbg_frame_undefined_value;

Dwarf_Regtable3 *dbg_internal_reg_table;





Dwarf_Unsigned dbgp_flags;
Dwarf_Unsigned dbgp_isa;
Dwarf_Callback_Func dbgp_func;
Dwarf_Callback_Func_b dbgp_func_b;
Dwarf_Die dbgp_root_die;
STAILQ_HEAD(, _Dwarf_Die) dbgp_dielist;
STAILQ_HEAD(, _Dwarf_P_Expr) dbgp_pelist;
Dwarf_LineInfo dbgp_lineinfo;
Dwarf_ArangeSet dbgp_as;
Dwarf_Macro_Details *dbgp_mdlist;
Dwarf_Unsigned dbgp_mdcnt;
STAILQ_HEAD(, _Dwarf_Cie) dbgp_cielist;
STAILQ_HEAD(, _Dwarf_Fde) dbgp_fdelist;
Dwarf_Unsigned dbgp_cielen;
Dwarf_Unsigned dbgp_fdelen;
Dwarf_NameTbl dbgp_pubs;
Dwarf_NameTbl dbgp_weaks;
Dwarf_NameTbl dbgp_funcs;
Dwarf_NameTbl dbgp_types;
Dwarf_NameTbl dbgp_vars;
STAILQ_HEAD(, _Dwarf_P_Section) dbgp_seclist;
Dwarf_Unsigned dbgp_seccnt;
Dwarf_P_Section dbgp_secpos;
Dwarf_P_Section dbgp_info;
STAILQ_HEAD(, _Dwarf_Rel_Section) dbgp_drslist;
Dwarf_Unsigned dbgp_drscnt;
Dwarf_Rel_Section dbgp_drspos;
};





int _dwarf_abbrev_add(Dwarf_CU, uint64_t, uint64_t, uint8_t,
uint64_t, Dwarf_Abbrev *, Dwarf_Error *);
void _dwarf_abbrev_cleanup(Dwarf_CU);
int _dwarf_abbrev_find(Dwarf_CU, uint64_t, Dwarf_Abbrev *,
Dwarf_Error *);
int _dwarf_abbrev_gen(Dwarf_P_Debug, Dwarf_Error *);
int _dwarf_abbrev_parse(Dwarf_Debug, Dwarf_CU, Dwarf_Unsigned *,
Dwarf_Abbrev *, Dwarf_Error *);
int _dwarf_add_AT_dataref(Dwarf_P_Debug, Dwarf_P_Die, Dwarf_Half,
Dwarf_Unsigned, Dwarf_Unsigned, const char *,
Dwarf_P_Attribute *, Dwarf_Error *);
int _dwarf_add_string_attr(Dwarf_P_Die, Dwarf_P_Attribute *,
Dwarf_Half, char *, Dwarf_Error *);
int _dwarf_alloc(Dwarf_Debug *, int, Dwarf_Error *);
void _dwarf_arange_cleanup(Dwarf_Debug);
int _dwarf_arange_gen(Dwarf_P_Debug, Dwarf_Error *);
int _dwarf_arange_init(Dwarf_Debug, Dwarf_Error *);
void _dwarf_arange_pro_cleanup(Dwarf_P_Debug);
int _dwarf_attr_alloc(Dwarf_Die, Dwarf_Attribute *, Dwarf_Error *);
Dwarf_Attribute _dwarf_attr_find(Dwarf_Die, Dwarf_Half);
int _dwarf_attr_gen(Dwarf_P_Debug, Dwarf_P_Section, Dwarf_Rel_Section,
Dwarf_CU, Dwarf_Die, int, Dwarf_Error *);
int _dwarf_attr_init(Dwarf_Debug, Dwarf_Section *, uint64_t *, int,
Dwarf_CU, Dwarf_Die, Dwarf_AttrDef, uint64_t, int,
Dwarf_Error *);
int _dwarf_attrdef_add(Dwarf_Debug, Dwarf_Abbrev, uint64_t,
uint64_t, uint64_t, Dwarf_AttrDef *, Dwarf_Error *);
uint64_t _dwarf_decode_lsb(uint8_t **, int);
uint64_t _dwarf_decode_msb(uint8_t **, int);
int64_t _dwarf_decode_sleb128(uint8_t **);
uint64_t _dwarf_decode_uleb128(uint8_t **);
void _dwarf_deinit(Dwarf_Debug);
int _dwarf_die_alloc(Dwarf_Debug, Dwarf_Die *, Dwarf_Error *);
int _dwarf_die_count_links(Dwarf_P_Die, Dwarf_P_Die,
Dwarf_P_Die, Dwarf_P_Die);
Dwarf_Die _dwarf_die_find(Dwarf_Die, Dwarf_Unsigned);
int _dwarf_die_gen(Dwarf_P_Debug, Dwarf_CU, Dwarf_Rel_Section,
Dwarf_Error *);
void _dwarf_die_link(Dwarf_P_Die, Dwarf_P_Die, Dwarf_P_Die,
Dwarf_P_Die, Dwarf_P_Die);
int _dwarf_die_parse(Dwarf_Debug, Dwarf_Section *, Dwarf_CU, int,
uint64_t, uint64_t, Dwarf_Die *, int, Dwarf_Error *);
void _dwarf_die_pro_cleanup(Dwarf_P_Debug);
void _dwarf_elf_deinit(Dwarf_Debug);
int _dwarf_elf_init(Dwarf_Debug, Elf *, Dwarf_Error *);
int _dwarf_elf_load_section(void *, Dwarf_Half, Dwarf_Small **,
int *);
Dwarf_Endianness _dwarf_elf_get_byte_order(void *);
Dwarf_Small _dwarf_elf_get_length_size(void *);
Dwarf_Small _dwarf_elf_get_pointer_size(void *);
Dwarf_Unsigned _dwarf_elf_get_section_count(void *);
int _dwarf_elf_get_section_info(void *, Dwarf_Half,
Dwarf_Obj_Access_Section *, int *);
void _dwarf_expr_cleanup(Dwarf_P_Debug);
int _dwarf_expr_into_block(Dwarf_P_Expr, Dwarf_Error *);
Dwarf_Section *_dwarf_find_next_types_section(Dwarf_Debug, Dwarf_Section *);
Dwarf_Section *_dwarf_find_section(Dwarf_Debug, const char *);
void _dwarf_frame_cleanup(Dwarf_Debug);
int _dwarf_frame_fde_add_inst(Dwarf_P_Fde, Dwarf_Small,
Dwarf_Unsigned, Dwarf_Unsigned, Dwarf_Error *);
int _dwarf_frame_gen(Dwarf_P_Debug, Dwarf_Error *);
int _dwarf_frame_get_fop(Dwarf_Debug, uint8_t, uint8_t *,
Dwarf_Unsigned, Dwarf_Frame_Op **, Dwarf_Signed *,
Dwarf_Error *);
int _dwarf_frame_get_internal_table(Dwarf_Fde, Dwarf_Addr,
Dwarf_Regtable3 **, Dwarf_Addr *, Dwarf_Error *);
int _dwarf_frame_interal_table_init(Dwarf_Debug, Dwarf_Error *);
void _dwarf_frame_params_init(Dwarf_Debug);
void _dwarf_frame_pro_cleanup(Dwarf_P_Debug);
int _dwarf_frame_regtable_copy(Dwarf_Debug, Dwarf_Regtable3 **,
Dwarf_Regtable3 *, Dwarf_Error *);
int _dwarf_frame_section_load(Dwarf_Debug, Dwarf_Error *);
int _dwarf_frame_section_load_eh(Dwarf_Debug, Dwarf_Error *);
int _dwarf_generate_sections(Dwarf_P_Debug, Dwarf_Error *);
Dwarf_Unsigned _dwarf_get_reloc_type(Dwarf_P_Debug, int);
int _dwarf_get_reloc_size(Dwarf_Debug, Dwarf_Unsigned);
void _dwarf_info_cleanup(Dwarf_Debug);
int _dwarf_info_first_cu(Dwarf_Debug, Dwarf_Error *);
int _dwarf_info_first_tu(Dwarf_Debug, Dwarf_Error *);
int _dwarf_info_gen(Dwarf_P_Debug, Dwarf_Error *);
int _dwarf_info_load(Dwarf_Debug, Dwarf_Bool, Dwarf_Bool,
Dwarf_Error *);
int _dwarf_info_next_cu(Dwarf_Debug, Dwarf_Error *);
int _dwarf_info_next_tu(Dwarf_Debug, Dwarf_Error *);
void _dwarf_info_pro_cleanup(Dwarf_P_Debug);
int _dwarf_init(Dwarf_Debug, Dwarf_Unsigned, Dwarf_Handler,
Dwarf_Ptr, Dwarf_Error *);
int _dwarf_lineno_gen(Dwarf_P_Debug, Dwarf_Error *);
int _dwarf_lineno_init(Dwarf_Die, uint64_t, Dwarf_Error *);
void _dwarf_lineno_cleanup(Dwarf_LineInfo);
void _dwarf_lineno_pro_cleanup(Dwarf_P_Debug);
int _dwarf_loc_fill_locdesc(Dwarf_Debug, Dwarf_Locdesc *,
uint8_t *, uint64_t, uint8_t, uint8_t, uint8_t,
Dwarf_Error *);
int _dwarf_loc_fill_locexpr(Dwarf_Debug, Dwarf_Locdesc **,
uint8_t *, uint64_t, uint8_t, uint8_t, uint8_t,
Dwarf_Error *);
int _dwarf_loc_add(Dwarf_Die, Dwarf_Attribute, Dwarf_Error *);
int _dwarf_loc_expr_add_atom(Dwarf_Debug, uint8_t *, uint8_t *,
Dwarf_Small, Dwarf_Unsigned, Dwarf_Unsigned, int *,
Dwarf_Error *);
int _dwarf_loclist_find(Dwarf_Debug, Dwarf_CU, uint64_t,
Dwarf_Locdesc ***, Dwarf_Signed *, Dwarf_Unsigned *,
Dwarf_Error *);
void _dwarf_macinfo_cleanup(Dwarf_Debug);
int _dwarf_macinfo_gen(Dwarf_P_Debug, Dwarf_Error *);
int _dwarf_macinfo_init(Dwarf_Debug, Dwarf_Error *);
void _dwarf_macinfo_pro_cleanup(Dwarf_P_Debug);
int _dwarf_nametbl_init(Dwarf_Debug, Dwarf_NameSec *,
Dwarf_Section *, Dwarf_Error *);
void _dwarf_nametbl_cleanup(Dwarf_NameSec *);
int _dwarf_nametbl_gen(Dwarf_P_Debug, const char *, Dwarf_NameTbl,
Dwarf_Error *);
void _dwarf_nametbl_pro_cleanup(Dwarf_NameTbl *);
int _dwarf_pro_callback(Dwarf_P_Debug, char *, int, Dwarf_Unsigned,
Dwarf_Unsigned, Dwarf_Unsigned, Dwarf_Unsigned,
Dwarf_Unsigned *, int *);
Dwarf_P_Section _dwarf_pro_find_section(Dwarf_P_Debug, const char *);
int _dwarf_ranges_add(Dwarf_Debug, Dwarf_CU, uint64_t,
Dwarf_Rangelist *, Dwarf_Error *);
void _dwarf_ranges_cleanup(Dwarf_Debug);
int _dwarf_ranges_find(Dwarf_Debug, uint64_t, Dwarf_Rangelist *);
uint64_t _dwarf_read_lsb(uint8_t *, uint64_t *, int);
uint64_t _dwarf_read_msb(uint8_t *, uint64_t *, int);
int64_t _dwarf_read_sleb128(uint8_t *, uint64_t *);
uint64_t _dwarf_read_uleb128(uint8_t *, uint64_t *);
char *_dwarf_read_string(void *, Dwarf_Unsigned, uint64_t *);
uint8_t *_dwarf_read_block(void *, uint64_t *, uint64_t);
int _dwarf_reloc_section_finalize(Dwarf_P_Debug, Dwarf_Rel_Section,
Dwarf_Error *);
int _dwarf_reloc_entry_add(Dwarf_P_Debug, Dwarf_Rel_Section,
Dwarf_P_Section, unsigned char, unsigned char,
Dwarf_Unsigned, Dwarf_Unsigned, Dwarf_Unsigned,
const char *, Dwarf_Error *);
int _dwarf_reloc_entry_add_pair(Dwarf_P_Debug, Dwarf_Rel_Section,
Dwarf_P_Section, unsigned char, Dwarf_Unsigned,
Dwarf_Unsigned, Dwarf_Unsigned, Dwarf_Unsigned,
Dwarf_Unsigned, Dwarf_Error *);
void _dwarf_reloc_cleanup(Dwarf_P_Debug);
int _dwarf_reloc_gen(Dwarf_P_Debug, Dwarf_Error *);
int _dwarf_reloc_section_gen(Dwarf_P_Debug, Dwarf_Rel_Section,
Dwarf_Error *);
int _dwarf_reloc_section_init(Dwarf_P_Debug, Dwarf_Rel_Section *,
Dwarf_P_Section, Dwarf_Error *);
void _dwarf_reloc_section_free(Dwarf_P_Debug, Dwarf_Rel_Section *);
void _dwarf_section_cleanup(Dwarf_P_Debug);
int _dwarf_section_callback(Dwarf_P_Debug, Dwarf_P_Section,
Dwarf_Unsigned, Dwarf_Unsigned, Dwarf_Unsigned,
Dwarf_Unsigned, Dwarf_Error *);
void _dwarf_section_free(Dwarf_P_Debug, Dwarf_P_Section *);
int _dwarf_section_init(Dwarf_P_Debug, Dwarf_P_Section *,
const char *, int, Dwarf_Error *);
void _dwarf_set_error(Dwarf_Debug, Dwarf_Error *, int, int,
const char *, int);
int _dwarf_strtab_add(Dwarf_Debug, char *, uint64_t *,
Dwarf_Error *);
void _dwarf_strtab_cleanup(Dwarf_Debug);
int _dwarf_strtab_gen(Dwarf_P_Debug, Dwarf_Error *);
char *_dwarf_strtab_get_table(Dwarf_Debug);
int _dwarf_strtab_init(Dwarf_Debug, Dwarf_Error *);
void _dwarf_type_unit_cleanup(Dwarf_Debug);
void _dwarf_write_block(void *, uint64_t *, uint8_t *, uint64_t);
int _dwarf_write_block_alloc(uint8_t **, uint64_t *, uint64_t *,
uint8_t *, uint64_t, Dwarf_Error *);
void _dwarf_write_lsb(uint8_t *, uint64_t *, uint64_t, int);
int _dwarf_write_lsb_alloc(uint8_t **, uint64_t *, uint64_t *,
uint64_t, int, Dwarf_Error *);
void _dwarf_write_msb(uint8_t *, uint64_t *, uint64_t, int);
int _dwarf_write_msb_alloc(uint8_t **, uint64_t *, uint64_t *,
uint64_t, int, Dwarf_Error *);
void _dwarf_write_padding(void *, uint64_t *, uint8_t, uint64_t);
int _dwarf_write_padding_alloc(uint8_t **, uint64_t *, uint64_t *,
uint8_t, uint64_t, Dwarf_Error *);
void _dwarf_write_string(void *, uint64_t *, char *);
int _dwarf_write_string_alloc(uint8_t **, uint64_t *, uint64_t *,
char *, Dwarf_Error *);
int _dwarf_write_sleb128(uint8_t *, uint8_t *, int64_t);
int _dwarf_write_sleb128_alloc(uint8_t **, uint64_t *, uint64_t *,
int64_t, Dwarf_Error *);
int _dwarf_write_uleb128(uint8_t *, uint8_t *, uint64_t);
int _dwarf_write_uleb128_alloc(uint8_t **, uint64_t *, uint64_t *,
uint64_t, Dwarf_Error *);

#endif
