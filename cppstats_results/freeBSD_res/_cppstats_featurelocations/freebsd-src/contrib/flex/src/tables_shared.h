#if defined(FLEX_SCANNER)






































#if !defined(yyskel_static)
#define yyskel_static static
#endif
#else
#if !defined(yyskel_static)
#define yyskel_static
#endif
#endif














#if !defined(YYTBL_MAGIC)
#define YYTBL_MAGIC 0xF13C57B1
#endif


#if !defined(yypad64)
#define yypad64(n) ((8-((n)%8))%8)
#endif


#if !defined(YYTABLES_TYPES)
#define YYTABLES_TYPES



enum yytbl_id {
YYTD_ID_ACCEPT = 0x01,
YYTD_ID_BASE = 0x02,
YYTD_ID_CHK = 0x03,
YYTD_ID_DEF = 0x04,
YYTD_ID_EC = 0x05,
YYTD_ID_META = 0x06,
YYTD_ID_NUL_TRANS = 0x07,
YYTD_ID_NXT = 0x08,
YYTD_ID_RULE_CAN_MATCH_EOL = 0x09,
YYTD_ID_START_STATE_LIST = 0x0A,
YYTD_ID_TRANSITION = 0x0B,
YYTD_ID_ACCLIST = 0x0C
};


enum yytbl_flags {

YYTD_DATA8 = 0x01,
YYTD_DATA16 = 0x02,
YYTD_DATA32 = 0x04,


YYTD_PTRANS = 0x08,


YYTD_STRUCT = 0x10
};


struct yytbl_hdr {
flex_uint32_t th_magic;
flex_uint32_t th_hsize;
flex_uint32_t th_ssize;
flex_uint16_t th_flags;
char *th_version;
char *th_name;
};


struct yytbl_data {
flex_uint16_t td_id;
flex_uint16_t td_flags;
flex_uint32_t td_hilen;
flex_uint32_t td_lolen;
void *td_data;
};
#endif


#if !defined(YYTDFLAGS2BYTES)
#define YYTDFLAGS2BYTES(td_flags)(((td_flags) & YYTD_DATA8)? sizeof(flex_int8_t):(((td_flags) & YYTD_DATA16)? sizeof(flex_int16_t):sizeof(flex_int32_t)))





#endif

#if defined(FLEX_SCANNER)
%not-for-header
#endif
yyskel_static flex_int32_t yytbl_calc_total_len (const struct yytbl_data *tbl);
#if defined(FLEX_SCANNER)
%ok-for-header
#endif


