

























#pragma ident "%Z%%M% %I% %E% SMI"










#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "ctftools.h"

char *progname;
int debug_level = DEBUG_LEVEL;

static void
usage(void)
{
fprintf(stderr, "Usage: %s ctf_file stab_file\n", progname);
}

int
main(int argc, char **argv)
{
tdata_t *ctftd, *stabrtd, *stabtd, *difftd;
char *ctfname, *stabname;
int new;

progname = argv[0];

if (argc != 3) {
usage();
exit(2);
}

ctfname = argv[1];
stabname = argv[2];

stabrtd = tdata_new();
stabtd = tdata_new();
difftd = tdata_new();

if (read_stabs(stabrtd, stabname, 0) != 0)
merge_into_master(stabrtd, stabtd, NULL, 1);
else if (read_ctf(&stabname, 1, NULL, read_ctf_save_cb, &stabtd, 0)
== 0)
terminate("%s doesn't have stabs or CTF\n", stabname);

if (read_ctf(&ctfname, 1, NULL, read_ctf_save_cb, &ctftd, 0) == 0)
terminate("%s doesn't contain CTF data\n", ctfname);

merge_into_master(stabtd, ctftd, difftd, 0);

if ((new = hash_count(difftd->td_iihash)) != 0) {
(void) hash_iter(difftd->td_iihash, (int (*)())iidesc_dump,
NULL);
terminate("%s grew by %d\n", stabname, new);
}

return (0);
}
