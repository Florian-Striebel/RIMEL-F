




















#if defined(HAVE_CONFIG_H)
#include <config.h>
#endif

#include <pcap.h>
#include <stdio.h>

#include "optimize.h"

void
bpf_dump(const struct bpf_program *p, int option)
{
const struct bpf_insn *insn;
int i;
int n = p->bf_len;

insn = p->bf_insns;
if (option > 2) {
printf("%d\n", n);
for (i = 0; i < n; ++insn, ++i) {
printf("%u %u %u %u\n", insn->code,
insn->jt, insn->jf, insn->k);
}
return ;
}
if (option > 1) {
for (i = 0; i < n; ++insn, ++i)
printf("{ 0x%x, %d, %d, 0x%08x },\n",
insn->code, insn->jt, insn->jf, insn->k);
return;
}
for (i = 0; i < n; ++insn, ++i) {
#if defined(BDEBUG)
if (i < NBIDS && bids[i] > 0)
printf("[%02d]", bids[i] - 1);
else
printf(" -- ");
#endif
puts(bpf_image(insn, i));
}
}
