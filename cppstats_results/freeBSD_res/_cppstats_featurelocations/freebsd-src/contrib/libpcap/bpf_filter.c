







































#if defined(HAVE_CONFIG_H)
#include <config.h>
#endif

#include <pcap/pcap-inttypes.h>
#include "pcap-types.h"

#if !defined(_WIN32)
#include <sys/param.h>
#include <sys/types.h>
#include <sys/time.h>
#endif

#include <pcap-int.h>

#include <stdlib.h>

#define int32 bpf_int32
#define u_int32 bpf_u_int32

#if !defined(LBL_ALIGN)






#if defined(sparc) || defined(__sparc__) || defined(mips) || defined(ibm032) || defined(__alpha) || defined(__hpux) || defined(__arm__)


#define LBL_ALIGN
#endif
#endif

#if !defined(LBL_ALIGN)
#if !defined(_WIN32)
#include <netinet/in.h>
#endif

#define EXTRACT_SHORT(p) ((u_short)ntohs(*(u_short *)p))
#define EXTRACT_LONG(p) (ntohl(*(u_int32 *)p))
#else
#define EXTRACT_SHORT(p)((u_short)((u_short)*((u_char *)p+0)<<8|(u_short)*((u_char *)p+1)<<0))



#define EXTRACT_LONG(p)((u_int32)*((u_char *)p+0)<<24|(u_int32)*((u_char *)p+1)<<16|(u_int32)*((u_char *)p+2)<<8|(u_int32)*((u_char *)p+3)<<0)




#endif

#if defined(__linux__)
#include <linux/types.h>
#include <linux/if_packet.h>
#include <linux/filter.h>
#endif

enum {
BPF_S_ANC_NONE,
BPF_S_ANC_VLAN_TAG,
BPF_S_ANC_VLAN_TAG_PRESENT,
};













u_int
bpf_filter_with_aux_data(const struct bpf_insn *pc, const u_char *p,
u_int wirelen, u_int buflen, const struct bpf_aux_data *aux_data)
{
register u_int32 A, X;
register bpf_u_int32 k;
u_int32 mem[BPF_MEMWORDS];

if (pc == 0)



return (u_int)-1;
A = 0;
X = 0;
--pc;
for (;;) {
++pc;
switch (pc->code) {

default:
abort();
case BPF_RET|BPF_K:
return (u_int)pc->k;

case BPF_RET|BPF_A:
return (u_int)A;

case BPF_LD|BPF_W|BPF_ABS:
k = pc->k;
if (k > buflen || sizeof(int32_t) > buflen - k) {
return 0;
}
A = EXTRACT_LONG(&p[k]);
continue;

case BPF_LD|BPF_H|BPF_ABS:
k = pc->k;
if (k > buflen || sizeof(int16_t) > buflen - k) {
return 0;
}
A = EXTRACT_SHORT(&p[k]);
continue;

case BPF_LD|BPF_B|BPF_ABS:
switch (pc->k) {

#if defined(SKF_AD_VLAN_TAG_PRESENT)
case SKF_AD_OFF + SKF_AD_VLAN_TAG:
if (!aux_data)
return 0;
A = aux_data->vlan_tag;
break;

case SKF_AD_OFF + SKF_AD_VLAN_TAG_PRESENT:
if (!aux_data)
return 0;
A = aux_data->vlan_tag_present;
break;
#endif
default:
k = pc->k;
if (k >= buflen) {
return 0;
}
A = p[k];
break;
}
continue;

case BPF_LD|BPF_W|BPF_LEN:
A = wirelen;
continue;

case BPF_LDX|BPF_W|BPF_LEN:
X = wirelen;
continue;

case BPF_LD|BPF_W|BPF_IND:
k = X + pc->k;
if (pc->k > buflen || X > buflen - pc->k ||
sizeof(int32_t) > buflen - k) {
return 0;
}
A = EXTRACT_LONG(&p[k]);
continue;

case BPF_LD|BPF_H|BPF_IND:
k = X + pc->k;
if (X > buflen || pc->k > buflen - X ||
sizeof(int16_t) > buflen - k) {
return 0;
}
A = EXTRACT_SHORT(&p[k]);
continue;

case BPF_LD|BPF_B|BPF_IND:
k = X + pc->k;
if (pc->k >= buflen || X >= buflen - pc->k) {
return 0;
}
A = p[k];
continue;

case BPF_LDX|BPF_MSH|BPF_B:
k = pc->k;
if (k >= buflen) {
return 0;
}
X = (p[pc->k] & 0xf) << 2;
continue;

case BPF_LD|BPF_IMM:
A = pc->k;
continue;

case BPF_LDX|BPF_IMM:
X = pc->k;
continue;

case BPF_LD|BPF_MEM:
A = mem[pc->k];
continue;

case BPF_LDX|BPF_MEM:
X = mem[pc->k];
continue;

case BPF_ST:
mem[pc->k] = A;
continue;

case BPF_STX:
mem[pc->k] = X;
continue;

case BPF_JMP|BPF_JA:




pc += (bpf_int32)pc->k;
continue;

case BPF_JMP|BPF_JGT|BPF_K:
pc += (A > pc->k) ? pc->jt : pc->jf;
continue;

case BPF_JMP|BPF_JGE|BPF_K:
pc += (A >= pc->k) ? pc->jt : pc->jf;
continue;

case BPF_JMP|BPF_JEQ|BPF_K:
pc += (A == pc->k) ? pc->jt : pc->jf;
continue;

case BPF_JMP|BPF_JSET|BPF_K:
pc += (A & pc->k) ? pc->jt : pc->jf;
continue;

case BPF_JMP|BPF_JGT|BPF_X:
pc += (A > X) ? pc->jt : pc->jf;
continue;

case BPF_JMP|BPF_JGE|BPF_X:
pc += (A >= X) ? pc->jt : pc->jf;
continue;

case BPF_JMP|BPF_JEQ|BPF_X:
pc += (A == X) ? pc->jt : pc->jf;
continue;

case BPF_JMP|BPF_JSET|BPF_X:
pc += (A & X) ? pc->jt : pc->jf;
continue;

case BPF_ALU|BPF_ADD|BPF_X:
A += X;
continue;

case BPF_ALU|BPF_SUB|BPF_X:
A -= X;
continue;

case BPF_ALU|BPF_MUL|BPF_X:
A *= X;
continue;

case BPF_ALU|BPF_DIV|BPF_X:
if (X == 0)
return 0;
A /= X;
continue;

case BPF_ALU|BPF_MOD|BPF_X:
if (X == 0)
return 0;
A %= X;
continue;

case BPF_ALU|BPF_AND|BPF_X:
A &= X;
continue;

case BPF_ALU|BPF_OR|BPF_X:
A |= X;
continue;

case BPF_ALU|BPF_XOR|BPF_X:
A ^= X;
continue;

case BPF_ALU|BPF_LSH|BPF_X:
if (X < 32)
A <<= X;
else
A = 0;
continue;

case BPF_ALU|BPF_RSH|BPF_X:
if (X < 32)
A >>= X;
else
A = 0;
continue;

case BPF_ALU|BPF_ADD|BPF_K:
A += pc->k;
continue;

case BPF_ALU|BPF_SUB|BPF_K:
A -= pc->k;
continue;

case BPF_ALU|BPF_MUL|BPF_K:
A *= pc->k;
continue;

case BPF_ALU|BPF_DIV|BPF_K:
A /= pc->k;
continue;

case BPF_ALU|BPF_MOD|BPF_K:
A %= pc->k;
continue;

case BPF_ALU|BPF_AND|BPF_K:
A &= pc->k;
continue;

case BPF_ALU|BPF_OR|BPF_K:
A |= pc->k;
continue;

case BPF_ALU|BPF_XOR|BPF_K:
A ^= pc->k;
continue;

case BPF_ALU|BPF_LSH|BPF_K:
A <<= pc->k;
continue;

case BPF_ALU|BPF_RSH|BPF_K:
A >>= pc->k;
continue;

case BPF_ALU|BPF_NEG:








A = (0U - A);
continue;

case BPF_MISC|BPF_TAX:
X = A;
continue;

case BPF_MISC|BPF_TXA:
A = X;
continue;
}
}
}

u_int
bpf_filter(const struct bpf_insn *pc, const u_char *p, u_int wirelen,
u_int buflen)
{
return bpf_filter_with_aux_data(pc, p, wirelen, buflen, NULL);
}













int
bpf_validate(const struct bpf_insn *f, int len)
{
u_int i, from;
const struct bpf_insn *p;

if (len < 1)
return 0;

for (i = 0; i < (u_int)len; ++i) {
p = &f[i];
switch (BPF_CLASS(p->code)) {



case BPF_LD:
case BPF_LDX:
switch (BPF_MODE(p->code)) {
case BPF_IMM:
break;
case BPF_ABS:
case BPF_IND:
case BPF_MSH:





break;
case BPF_MEM:
if (p->k >= BPF_MEMWORDS)
return 0;
break;
case BPF_LEN:
break;
default:
return 0;
}
break;
case BPF_ST:
case BPF_STX:
if (p->k >= BPF_MEMWORDS)
return 0;
break;
case BPF_ALU:
switch (BPF_OP(p->code)) {
case BPF_ADD:
case BPF_SUB:
case BPF_MUL:
case BPF_OR:
case BPF_AND:
case BPF_XOR:
case BPF_LSH:
case BPF_RSH:
case BPF_NEG:
break;
case BPF_DIV:
case BPF_MOD:




if (BPF_SRC(p->code) == BPF_K && p->k == 0)
return 0;
break;
default:
return 0;
}
break;
case BPF_JMP:


























from = i + 1;
switch (BPF_OP(p->code)) {
case BPF_JA:
if (from + p->k >= (u_int)len)
return 0;
break;
case BPF_JEQ:
case BPF_JGT:
case BPF_JGE:
case BPF_JSET:
if (from + p->jt >= (u_int)len || from + p->jf >= (u_int)len)
return 0;
break;
default:
return 0;
}
break;
case BPF_RET:
break;
case BPF_MISC:
break;
default:
return 0;
}
}
return BPF_CLASS(f[len - 1].code) == BPF_RET;
}
