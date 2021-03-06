#if defined(__cplusplus)
extern "C" {
#endif
typedef enum {
D_UNKNOWN,
D_SYNTAX,
D_EMPTY,
D_TYPE_ERR,
D_TYPE_MEMBER,
D_ASRELO,
D_CG_EXPR,
D_CG_DYN,
D_ATTR_MIN,
D_ID_OFLOW,
D_PDESC_ZERO,
D_PDESC_INVAL,
D_PRED_SCALAR,
D_FUNC_IDENT,
D_FUNC_UNDEF,
D_FUNC_IDKIND,
D_OFFSETOF_TYPE,
D_OFFSETOF_BITFIELD,
D_SIZEOF_TYPE,
D_SIZEOF_BITFIELD,
D_STRINGOF_TYPE,
D_OP_IDENT,
D_OP_INT,
D_OP_SCALAR,
D_OP_ARITH,
D_OP_WRITE,
D_OP_LVAL,
D_OP_INCOMPAT,
D_OP_VFPTR,
D_OP_ARRFUN,
D_OP_PTR,
D_OP_SOU,
D_OP_INCOMPLETE,
D_OP_DYN,
D_OP_ACT,
D_AGG_REDEF,
D_AGG_FUNC,
D_AGG_MDIM,
D_ARR_BADREF,
D_ARR_LOCAL,
D_DIV_ZERO,
D_DEREF_NONPTR,
D_DEREF_VOID,
D_DEREF_FUNC,
D_ADDROF_LVAL,
D_ADDROF_VAR,
D_ADDROF_BITFIELD,
D_XLATE_REDECL,
D_XLATE_NOCONV,
D_XLATE_NONE,
D_XLATE_SOU,
D_XLATE_INCOMPAT,
D_XLATE_MEMB,
D_CAST_INVAL,
D_PRAGERR,
D_PRAGCTL_INVAL,
D_PRAGMA_INVAL,
D_PRAGMA_UNUSED,
D_PRAGMA_MALFORM,
D_PRAGMA_OPTSET,
D_PRAGMA_SCOPE,
D_PRAGMA_DEPEND,
D_MACRO_UNDEF,
D_MACRO_OFLOW,
D_MACRO_UNUSED,
D_INT_OFLOW,
D_INT_DIGIT,
D_STR_NL,
D_CHR_NL,
D_CHR_NULL,
D_CHR_OFLOW,
D_IDENT_BADREF,
D_IDENT_UNDEF,
D_IDENT_AMBIG,
D_SYM_BADREF,
D_SYM_NOTYPES,
D_SYM_MODEL,
D_VAR_UNDEF,
D_VAR_UNSUP,
D_PROTO_LEN,
D_PROTO_ARG,
D_ARGS_MULTI,
D_ARGS_XLATOR,
D_ARGS_NONE,
D_ARGS_TYPE,
D_ARGS_IDX,
D_REGS_IDX,
D_KEY_TYPE,
D_PRINTF_DYN_PROTO,
D_PRINTF_DYN_TYPE,
D_PRINTF_AGG_CONV,
D_PRINTF_ARG_PROTO,
D_PRINTF_ARG_TYPE,
D_PRINTF_ARG_EXTRA,
D_PRINTF_ARG_FMT,
D_PRINTF_FMT_EMPTY,
D_DECL_CHARATTR,
D_DECL_VOIDATTR,
D_DECL_SIGNINT,
D_DECL_LONGINT,
D_DECL_IDENT,
D_DECL_CLASS,
D_DECL_BADCLASS,
D_DECL_PARMCLASS,
D_DECL_COMBO,
D_DECL_ARRSUB,
D_DECL_ARRNULL,
D_DECL_ARRBIG,
D_DECL_IDRED,
D_DECL_TYPERED,
D_DECL_MNAME,
D_DECL_SCOPE,
D_DECL_BFCONST,
D_DECL_BFSIZE,
D_DECL_BFTYPE,
D_DECL_ENCONST,
D_DECL_ENOFLOW,
D_DECL_USELESS,
D_DECL_LOCASSC,
D_DECL_VOIDOBJ,
D_DECL_DYNOBJ,
D_DECL_INCOMPLETE,
D_DECL_PROTO_VARARGS,
D_DECL_PROTO_TYPE,
D_DECL_PROTO_VOID,
D_DECL_PROTO_NAME,
D_DECL_PROTO_FORM,
D_COMM_COMM,
D_COMM_DREC,
D_SPEC_SPEC,
D_SPEC_COMM,
D_SPEC_DREC,
D_AGG_COMM,
D_AGG_SPEC,
D_AGG_NULL,
D_AGG_SCALAR,
D_ACT_SPEC,
D_EXIT_SPEC,
D_DREC_COMM,
D_PRINTA_PROTO,
D_PRINTA_AGGARG,
D_PRINTA_AGGBAD,
D_PRINTA_AGGKEY,
D_PRINTA_AGGPROTO,
D_TRACE_VOID,
D_TRACE_DYN,
D_TRACE_AGG,
D_PRINT_VOID,
D_PRINT_DYN,
D_PRINT_AGG,
D_TRACEMEM_ADDR,
D_TRACEMEM_SIZE,
D_TRACEMEM_ARGS,
D_TRACEMEM_DYNSIZE,
D_STACK_PROTO,
D_STACK_SIZE,
D_USTACK_FRAMES,
D_USTACK_STRSIZE,
D_USTACK_PROTO,
D_LQUANT_BASETYPE,
D_LQUANT_BASEVAL,
D_LQUANT_LIMTYPE,
D_LQUANT_LIMVAL,
D_LQUANT_MISMATCH,
D_LQUANT_STEPTYPE,
D_LQUANT_STEPVAL,
D_LQUANT_STEPLARGE,
D_LQUANT_STEPSMALL,
D_QUANT_PROTO,
D_PROC_OFF,
D_PROC_ALIGN,
D_PROC_NAME,
D_PROC_GRAB,
D_PROC_DYN,
D_PROC_LIB,
D_PROC_FUNC,
D_PROC_CREATEFAIL,
D_PROC_NODEV,
D_PROC_BADPID,
D_PROC_BADPROV,
D_PROC_USDT,
D_CLEAR_PROTO,
D_CLEAR_AGGARG,
D_CLEAR_AGGBAD,
D_NORMALIZE_PROTO,
D_NORMALIZE_SCALAR,
D_NORMALIZE_AGGARG,
D_NORMALIZE_AGGBAD,
D_TRUNC_PROTO,
D_TRUNC_SCALAR,
D_TRUNC_AGGARG,
D_TRUNC_AGGBAD,
D_PROV_BADNAME,
D_PROV_INCOMPAT,
D_PROV_PRDUP,
D_PROV_PRARGLEN,
D_PROV_PRXLATOR,
D_FREOPEN_INVALID,
D_LQUANT_MATCHBASE,
D_LQUANT_MATCHLIM,
D_LQUANT_MATCHSTEP,
D_LLQUANT_FACTORTYPE,
D_LLQUANT_FACTORVAL,
D_LLQUANT_FACTORMATCH,
D_LLQUANT_LOWTYPE,
D_LLQUANT_LOWVAL,
D_LLQUANT_LOWMATCH,
D_LLQUANT_HIGHTYPE,
D_LLQUANT_HIGHVAL,
D_LLQUANT_HIGHMATCH,
D_LLQUANT_NSTEPTYPE,
D_LLQUANT_NSTEPVAL,
D_LLQUANT_NSTEPMATCH,
D_LLQUANT_MAGRANGE,
D_LLQUANT_FACTORNSTEPS,
D_LLQUANT_FACTOREVEN,
D_LLQUANT_FACTORSMALL,
D_LLQUANT_MAGTOOBIG,
D_NOREG,
D_PRINTM_ADDR,
D_PRINTM_SIZE,
} dt_errtag_t;
extern const char *dt_errtag(dt_errtag_t);
#if defined(__cplusplus)
}
#endif
