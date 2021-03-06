


































































#if !defined(_NET_BPF_H_) && !defined(_NET_BPF_H_INCLUDED) && !defined(_BPF_H_) && !defined(_H_BPF) && !defined(lib_pcap_bpf_h)
#define lib_pcap_bpf_h

#include <pcap/funcattrs.h>

#if defined(__cplusplus)
extern "C" {
#endif


#define BPF_RELEASE 199606

#if defined(MSDOS)
typedef long bpf_int32;
typedef unsigned long bpf_u_int32;
#else
typedef int bpf_int32;
typedef u_int bpf_u_int32;
#endif







#if !defined(__NetBSD__)
#define BPF_ALIGNMENT sizeof(bpf_int32)
#else
#define BPF_ALIGNMENT sizeof(long)
#endif
#define BPF_WORDALIGN(x) (((x)+(BPF_ALIGNMENT-1))&~(BPF_ALIGNMENT-1))




struct bpf_program {
u_int bf_len;
struct bpf_insn *bf_insns;
};

#include <pcap/dlt.h>















#define BPF_CLASS(code) ((code) & 0x07)
#define BPF_LD 0x00
#define BPF_LDX 0x01
#define BPF_ST 0x02
#define BPF_STX 0x03
#define BPF_ALU 0x04
#define BPF_JMP 0x05
#define BPF_RET 0x06
#define BPF_MISC 0x07


#define BPF_SIZE(code) ((code) & 0x18)
#define BPF_W 0x00
#define BPF_H 0x08
#define BPF_B 0x10

#define BPF_MODE(code) ((code) & 0xe0)
#define BPF_IMM 0x00
#define BPF_ABS 0x20
#define BPF_IND 0x40
#define BPF_MEM 0x60
#define BPF_LEN 0x80
#define BPF_MSH 0xa0




#define BPF_OP(code) ((code) & 0xf0)
#define BPF_ADD 0x00
#define BPF_SUB 0x10
#define BPF_MUL 0x20
#define BPF_DIV 0x30
#define BPF_OR 0x40
#define BPF_AND 0x50
#define BPF_LSH 0x60
#define BPF_RSH 0x70
#define BPF_NEG 0x80
#define BPF_MOD 0x90
#define BPF_XOR 0xa0






#define BPF_JA 0x00
#define BPF_JEQ 0x10
#define BPF_JGT 0x20
#define BPF_JGE 0x30
#define BPF_JSET 0x40











#define BPF_SRC(code) ((code) & 0x08)
#define BPF_K 0x00
#define BPF_X 0x08


#define BPF_RVAL(code) ((code) & 0x18)
#define BPF_A 0x10



#define BPF_MISCOP(code) ((code) & 0xf8)
#define BPF_TAX 0x00
















#define BPF_TXA 0x80



















struct bpf_insn {
u_short code;
u_char jt;
u_char jf;
bpf_u_int32 k;
};




#define BPF_STMT(code, k) { (u_short)(code), 0, 0, k }
#define BPF_JUMP(code, k, jt, jf) { (u_short)(code), jt, jf, k }

PCAP_API int bpf_validate(const struct bpf_insn *, int);
PCAP_API u_int bpf_filter(const struct bpf_insn *, const u_char *, u_int, u_int);




#define BPF_MEMWORDS 16

#if defined(__cplusplus)
}
#endif

#endif
