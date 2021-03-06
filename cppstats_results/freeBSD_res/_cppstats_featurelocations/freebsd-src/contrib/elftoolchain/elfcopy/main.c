

























#include <sys/param.h>
#include <sys/stat.h>

#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <libelftc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "elfcopy.h"

ELFTC_VCSID("$Id: main.c 3757 2019-06-28 01:15:28Z emaste $");

enum options
{
ECP_ADD_GNU_DEBUGLINK,
ECP_ADD_SECTION,
ECP_CHANGE_ADDR,
ECP_CHANGE_SEC_ADDR,
ECP_CHANGE_SEC_LMA,
ECP_CHANGE_SEC_VMA,
ECP_CHANGE_START,
ECP_CHANGE_WARN,
ECP_GAP_FILL,
ECP_GLOBALIZE_SYMBOL,
ECP_GLOBALIZE_SYMBOLS,
ECP_KEEP_SYMBOLS,
ECP_KEEP_GLOBAL_SYMBOLS,
ECP_LOCALIZE_HIDDEN,
ECP_LOCALIZE_SYMBOLS,
ECP_NO_CHANGE_WARN,
ECP_ONLY_DEBUG,
ECP_ONLY_DWO,
ECP_PAD_TO,
ECP_PREFIX_ALLOC,
ECP_PREFIX_SEC,
ECP_PREFIX_SYM,
ECP_REDEF_SYMBOL,
ECP_REDEF_SYMBOLS,
ECP_RENAME_SECTION,
ECP_SET_OSABI,
ECP_SET_SEC_FLAGS,
ECP_SET_START,
ECP_SREC_FORCE_S3,
ECP_SREC_LEN,
ECP_STRIP_DWO,
ECP_STRIP_SYMBOLS,
ECP_STRIP_UNNEEDED,
ECP_WEAKEN_ALL,
ECP_WEAKEN_SYMBOLS
};

static struct option mcs_longopts[] =
{
{ "help", no_argument, NULL, 'h' },
{ "version", no_argument, NULL, 'V' },
{ NULL, 0, NULL, 0 }
};

static struct option strip_longopts[] =
{
{"discard-all", no_argument, NULL, 'x'},
{"discard-locals", no_argument, NULL, 'X'},
{"help", no_argument, NULL, 'h'},
{"input-target", required_argument, NULL, 'I'},
{"keep-symbol", required_argument, NULL, 'K'},
{"only-keep-debug", no_argument, NULL, ECP_ONLY_DEBUG},
{"output-file", required_argument, NULL, 'o'},
{"output-target", required_argument, NULL, 'O'},
{"preserve-dates", no_argument, NULL, 'p'},
{"remove-section", required_argument, NULL, 'R'},
{"strip-all", no_argument, NULL, 's'},
{"strip-debug", no_argument, NULL, 'S'},
{"strip-symbol", required_argument, NULL, 'N'},
{"strip-unneeded", no_argument, NULL, ECP_STRIP_UNNEEDED},
{"version", no_argument, NULL, 'V'},
{"wildcard", no_argument, NULL, 'w'},
{NULL, 0, NULL, 0}
};

static struct option elfcopy_longopts[] =
{
{"add-gnu-debuglink", required_argument, NULL, ECP_ADD_GNU_DEBUGLINK},
{"add-section", required_argument, NULL, ECP_ADD_SECTION},
{"adjust-section-vma", required_argument, NULL, ECP_CHANGE_SEC_ADDR},
{"adjust-vma", required_argument, NULL, ECP_CHANGE_ADDR},
{"adjust-start", required_argument, NULL, ECP_CHANGE_START},
{"adjust-warnings", no_argument, NULL, ECP_CHANGE_WARN},
{"binary-architecture", required_argument, NULL, 'B'},
{"change-addresses", required_argument, NULL, ECP_CHANGE_ADDR},
{"change-section-address", required_argument, NULL,
ECP_CHANGE_SEC_ADDR},
{"change-section-lma", required_argument, NULL, ECP_CHANGE_SEC_LMA},
{"change-section-vma", required_argument, NULL, ECP_CHANGE_SEC_VMA},
{"change-start", required_argument, NULL, ECP_CHANGE_START},
{"change-warnings", no_argument, NULL, ECP_CHANGE_WARN},
{"discard-all", no_argument, NULL, 'x'},
{"discard-locals", no_argument, NULL, 'X'},
{"extract-dwo", no_argument, NULL, ECP_ONLY_DWO},
{"gap-fill", required_argument, NULL, ECP_GAP_FILL},
{"globalize-symbol", required_argument, NULL, ECP_GLOBALIZE_SYMBOL},
{"globalize-symbols", required_argument, NULL, ECP_GLOBALIZE_SYMBOLS},
{"help", no_argument, NULL, 'h'},
{"input-target", required_argument, NULL, 'I'},
{"keep-symbol", required_argument, NULL, 'K'},
{"keep-symbols", required_argument, NULL, ECP_KEEP_SYMBOLS},
{"keep-global-symbol", required_argument, NULL, 'G'},
{"keep-global-symbols", required_argument, NULL,
ECP_KEEP_GLOBAL_SYMBOLS},
{"localize-hidden", no_argument, NULL, ECP_LOCALIZE_HIDDEN},
{"localize-symbol", required_argument, NULL, 'L'},
{"localize-symbols", required_argument, NULL, ECP_LOCALIZE_SYMBOLS},
{"no-adjust-warnings", no_argument, NULL, ECP_NO_CHANGE_WARN},
{"no-change-warnings", no_argument, NULL, ECP_NO_CHANGE_WARN},
{"only-keep-debug", no_argument, NULL, ECP_ONLY_DEBUG},
{"only-section", required_argument, NULL, 'j'},
{"osabi", required_argument, NULL, ECP_SET_OSABI},
{"output-target", required_argument, NULL, 'O'},
{"pad-to", required_argument, NULL, ECP_PAD_TO},
{"preserve-dates", no_argument, NULL, 'p'},
{"prefix-alloc-sections", required_argument, NULL, ECP_PREFIX_ALLOC},
{"prefix-sections", required_argument, NULL, ECP_PREFIX_SEC},
{"prefix-symbols", required_argument, NULL, ECP_PREFIX_SYM},
{"redefine-sym", required_argument, NULL, ECP_REDEF_SYMBOL},
{"redefine-syms", required_argument, NULL, ECP_REDEF_SYMBOLS},
{"remove-section", required_argument, NULL, 'R'},
{"rename-section", required_argument, NULL, ECP_RENAME_SECTION},
{"set-section-flags", required_argument, NULL, ECP_SET_SEC_FLAGS},
{"set-start", required_argument, NULL, ECP_SET_START},
{"srec-forceS3", no_argument, NULL, ECP_SREC_FORCE_S3},
{"srec-len", required_argument, NULL, ECP_SREC_LEN},
{"strip-all", no_argument, NULL, 'S'},
{"strip-debug", no_argument, 0, 'g'},
{"strip-dwo", no_argument, NULL, ECP_STRIP_DWO},
{"strip-symbol", required_argument, NULL, 'N'},
{"strip-symbols", required_argument, NULL, ECP_STRIP_SYMBOLS},
{"strip-unneeded", no_argument, NULL, ECP_STRIP_UNNEEDED},
{"version", no_argument, NULL, 'V'},
{"weaken", no_argument, NULL, ECP_WEAKEN_ALL},
{"weaken-symbol", required_argument, NULL, 'W'},
{"weaken-symbols", required_argument, NULL, ECP_WEAKEN_SYMBOLS},
{"wildcard", no_argument, NULL, 'w'},
{NULL, 0, NULL, 0}
};

static struct {
const char *name;
int value;
} sec_flags[] = {
{"alloc", SF_ALLOC},
{"load", SF_LOAD},
{"noload", SF_NOLOAD},
{"readonly", SF_READONLY},
{"debug", SF_DEBUG},
{"code", SF_CODE},
{"data", SF_DATA},
{"rom", SF_ROM},
{"share", SF_SHARED},
{"contents", SF_CONTENTS},
{NULL, 0}
};

static struct {
const char *name;
int abi;
} osabis[] = {
{"sysv", ELFOSABI_SYSV},
{"hpus", ELFOSABI_HPUX},
{"netbsd", ELFOSABI_NETBSD},
{"linux", ELFOSABI_LINUX},
{"hurd", ELFOSABI_HURD},
{"86open", ELFOSABI_86OPEN},
{"solaris", ELFOSABI_SOLARIS},
{"aix", ELFOSABI_AIX},
{"irix", ELFOSABI_IRIX},
{"freebsd", ELFOSABI_FREEBSD},
{"tru64", ELFOSABI_TRU64},
{"modesto", ELFOSABI_MODESTO},
{"openbsd", ELFOSABI_OPENBSD},
{"openvms", ELFOSABI_OPENVMS},
{"nsk", ELFOSABI_NSK},
{"cloudabi", ELFOSABI_CLOUDABI},
{"arm", ELFOSABI_ARM},
{"standalone", ELFOSABI_STANDALONE},
{NULL, 0}
};

static int copy_from_tempfile(const char *src, const char *dst,
int infd, int *outfd, int in_place);
static void create_file(struct elfcopy *ecp, const char *src,
const char *dst);
static void elfcopy_main(struct elfcopy *ecp, int argc, char **argv);
static void elfcopy_usage(void);
static void mcs_main(struct elfcopy *ecp, int argc, char **argv);
static void mcs_usage(void);
static void parse_sec_address_op(struct elfcopy *ecp, int optnum,
const char *optname, char *s);
static void parse_sec_flags(struct sec_action *sac, char *s);
static void parse_symlist_file(struct elfcopy *ecp, const char *fn,
unsigned int op);
static void print_version(void);
static void set_input_target(struct elfcopy *ecp, const char *target_name);
static void set_osabi(struct elfcopy *ecp, const char *abi);
static void set_output_target(struct elfcopy *ecp, const char *target_name);
static void strip_main(struct elfcopy *ecp, int argc, char **argv);
static void strip_usage(void);










































void
create_elf(struct elfcopy *ecp)
{
struct section *shtab;
GElf_Ehdr ieh;
GElf_Ehdr oeh;
size_t ishnum;

ecp->flags |= SYMTAB_INTACT;
ecp->flags &= ~SYMTAB_EXIST;


if (gelf_getehdr(ecp->ein, &ieh) == NULL)
errx(EXIT_FAILURE, "gelf_getehdr() failed: %s",
elf_errmsg(-1));
if ((ecp->iec = gelf_getclass(ecp->ein)) == ELFCLASSNONE)
errx(EXIT_FAILURE, "getclass() failed: %s",
elf_errmsg(-1));

if (ecp->oec == ELFCLASSNONE)
ecp->oec = ecp->iec;
if (ecp->oed == ELFDATANONE)
ecp->oed = ieh.e_ident[EI_DATA];

if (gelf_newehdr(ecp->eout, ecp->oec) == NULL)
errx(EXIT_FAILURE, "gelf_newehdr failed: %s",
elf_errmsg(-1));
if (gelf_getehdr(ecp->eout, &oeh) == NULL)
errx(EXIT_FAILURE, "gelf_getehdr() failed: %s",
elf_errmsg(-1));

memcpy(oeh.e_ident, ieh.e_ident, sizeof(ieh.e_ident));
oeh.e_ident[EI_CLASS] = ecp->oec;
oeh.e_ident[EI_DATA] = ecp->oed;
if (ecp->abi != -1)
oeh.e_ident[EI_OSABI] = ecp->abi;
oeh.e_flags = ieh.e_flags;
oeh.e_machine = ieh.e_machine;
oeh.e_type = ieh.e_type;
oeh.e_entry = ieh.e_entry;
oeh.e_version = ieh.e_version;

ecp->flags &= ~(EXECUTABLE | DYNAMIC | RELOCATABLE);
if (ieh.e_type == ET_EXEC)
ecp->flags |= EXECUTABLE;
else if (ieh.e_type == ET_DYN)
ecp->flags |= DYNAMIC;
else if (ieh.e_type == ET_REL)
ecp->flags |= RELOCATABLE;
else
errx(EXIT_FAILURE, "unsupported e_type");

if (!elf_getshnum(ecp->ein, &ishnum))
errx(EXIT_FAILURE, "elf_getshnum failed: %s",
elf_errmsg(-1));
if (ishnum > 0 && (ecp->secndx = calloc(ishnum,
sizeof(*ecp->secndx))) == NULL)
err(EXIT_FAILURE, "calloc failed");


setup_phdr(ecp);







create_scn(ecp);


adjust_addr(ecp);





if (ecp->strip == STRIP_DEBUG ||
ecp->strip == STRIP_UNNEEDED ||
ecp->flags & WEAKEN_ALL ||
ecp->flags & LOCALIZE_HIDDEN ||
ecp->flags & DISCARD_LOCAL ||
ecp->flags & DISCARD_LLABEL ||
ecp->prefix_sym != NULL ||
!STAILQ_EMPTY(&ecp->v_symop))
ecp->flags &= ~SYMTAB_INTACT;






if ((ecp->flags & SYMTAB_EXIST) != 0)
create_symtab(ecp);





if (gelf_update_ehdr(ecp->eout, &oeh) == 0)
errx(EXIT_FAILURE, "gelf_update_ehdr() failed: %s",
elf_errmsg(-1));








copy_content(ecp);






update_shdr(ecp, 1);


if (gelf_getehdr(ecp->eout, &oeh) == NULL)
errx(EXIT_FAILURE, "gelf_getehdr() failed: %s",
elf_errmsg(-1));














if (oeh.e_ident[EI_OSABI] == ELFOSABI_FREEBSD)
shtab = insert_shtab(ecp, 1);
else
shtab = insert_shtab(ecp, 0);






resync_sections(ecp);


oeh.e_shoff = shtab->off;


if (ecp->ophnum > 0) {
oeh.e_phoff = gelf_fsize(ecp->eout, ELF_T_EHDR, 1, EV_CURRENT);
if (oeh.e_phoff == 0)
errx(EXIT_FAILURE, "gelf_fsize() failed: %s",
elf_errmsg(-1));
}




if (ecp->change_addr != 0)
oeh.e_entry += ecp->change_addr;
if (ecp->flags & SET_START)
oeh.e_entry = ecp->set_start;
if (ecp->change_start != 0)
oeh.e_entry += ecp->change_start;





if (gelf_update_ehdr(ecp->eout, &oeh) == 0)
errx(EXIT_FAILURE, "gelf_update_ehdr() failed: %s",
elf_errmsg(-1));

if (ecp->ophnum > 0)
copy_phdr(ecp);


if (elf_update(ecp->eout, ELF_C_WRITE) < 0)
errx(EXIT_FAILURE, "elf_update() failed: %s",
elf_errmsg(-1));


free_elf(ecp);
}

void
free_elf(struct elfcopy *ecp)
{
struct segment *seg, *seg_temp;
struct section *sec, *sec_temp;


if (!STAILQ_EMPTY(&ecp->v_seg)) {
STAILQ_FOREACH_SAFE(seg, &ecp->v_seg, seg_list, seg_temp) {
STAILQ_REMOVE(&ecp->v_seg, seg, segment, seg_list);
free(seg);
}
}


free_symtab(ecp);


elftc_string_table_destroy(ecp->shstrtab->strtab);


if (!TAILQ_EMPTY(&ecp->v_sec)) {
TAILQ_FOREACH_SAFE(sec, &ecp->v_sec, sec_list, sec_temp) {
TAILQ_REMOVE(&ecp->v_sec, sec, sec_list);
if (sec->buf != NULL)
free(sec->buf);
if (sec->newname != NULL)
free(sec->newname);
if (sec->pad != NULL)
free(sec->pad);
free(sec);
}
}

ecp->symtab = NULL;
ecp->strtab = NULL;
ecp->shstrtab = NULL;

if (ecp->secndx != NULL) {
free(ecp->secndx);
ecp->secndx = NULL;
}
}






int
cleanup_tempfile(char *fn)
{
int errno_save, retval;

if (fn == NULL)
return 0;
errno_save = errno;
if ((retval = unlink(fn)) < 0) {
warn("unlink tempfile %s failed", fn);
errno = errno_save;
return retval;
}
return 0;
}


void
create_tempfile(const char *src, char **fn, int *fd)
{
static const char _TEMPDIR[] = "/tmp/";
static const char _TEMPFILE[] = "ecp.XXXXXXXX";
const char *tmpdir;
char *tmpf;
size_t tlen, slen, plen;

if (fn == NULL || fd == NULL)
return;
for (;;) {
if (src == NULL) {

tmpdir = getenv("TMPDIR");
if (tmpdir == NULL || *tmpdir == '\0')
tmpdir = _TEMPDIR;
tlen = strlen(tmpdir);
slen = tmpdir[tlen - 1] == '/' ? 0 : 1;
} else {

if ((tmpdir = strrchr(src, '/')) == NULL) {

tlen = 0;
} else {

tlen = ++tmpdir - src;
tmpdir = src;
}
slen = 0;
}
plen = strlen(_TEMPFILE) + 1;
tmpf = malloc(tlen + slen + plen);
if (tmpf == NULL)
err(EXIT_FAILURE, "malloc failed");
if (tlen > 0)
memcpy(tmpf, tmpdir, tlen);
if (slen > 0)
tmpf[tlen] = '/';

memcpy(tmpf + tlen + slen, _TEMPFILE, plen);
if ((*fd = mkstemp(tmpf)) != -1)
break;
if (errno != EACCES || src == NULL)
err(EXIT_FAILURE, "mkstemp %s failed", tmpf);

free(tmpf);
src = NULL;
}
if (fchmod(*fd, 0644) == -1)
err(EXIT_FAILURE, "fchmod %s failed", tmpf);
*fn = tmpf;
}







static int
copy_from_tempfile(const char *src, const char *dst, int infd, int *outfd,
int in_place)
{
int tmpfd;




if (in_place == 0) {
if (rename(src, dst) >= 0) {
*outfd = infd;
return (0);
} else if (errno != EXDEV && errno != EACCES)
return (-1);







if (errno != EACCES && unlink(dst) < 0)
return (-1);
}

if ((tmpfd = open(dst, O_CREAT | O_TRUNC | O_WRONLY, 0755)) < 0)
return (-1);

if (elftc_copyfile(infd, tmpfd) < 0) {
(void) close(tmpfd);
return (-1);
}





if (unlink(src) < 0) {
(void) close(tmpfd);
return (-1);
}

(void) close(infd);




*outfd = tmpfd;

return (0);
}

static void
create_file(struct elfcopy *ecp, const char *src, const char *dst)
{
struct stat sb;
char *tempfile, *elftemp;
int efd, ifd, ofd, ofd0, tfd;
int in_place;

tempfile = NULL;

if (src == NULL)
errx(EXIT_FAILURE, "internal: src == NULL");
if ((ifd = open(src, O_RDONLY)) == -1)
err(EXIT_FAILURE, "open %s failed", src);

if (fstat(ifd, &sb) == -1)
err(EXIT_FAILURE, "fstat %s failed", src);

if (dst == NULL)
create_tempfile(src, &tempfile, &ofd);
else
if ((ofd = open(dst, O_RDWR|O_CREAT, 0755)) == -1)
err(EXIT_FAILURE, "open %s failed", dst);

#if !defined(LIBELF_AR)

if (ac_detect_ar(ifd)) {
ac_create_ar(ecp, ifd, ofd);
goto copy_done;
}
#endif

if (lseek(ifd, 0, SEEK_SET) < 0) {
cleanup_tempfile(tempfile);
err(EXIT_FAILURE, "lseek failed");
}





if (ecp->itf != ETF_ELF) {






if (ecp->otf != ETF_ELF) {
if (ecp->oec == ELFCLASSNONE)
ecp->oec = ELFCLASS64;
if (ecp->oed == ELFDATANONE)
ecp->oed = ELFDATA2LSB;
}
create_tempfile(src, &elftemp, &efd);
if ((ecp->eout = elf_begin(efd, ELF_C_WRITE, NULL)) == NULL) {
cleanup_tempfile(elftemp);
cleanup_tempfile(tempfile);
errx(EXIT_FAILURE, "elf_begin() failed: %s",
elf_errmsg(-1));
}
elf_flagelf(ecp->eout, ELF_C_SET, ELF_F_LAYOUT);
if (ecp->itf == ETF_BINARY)
create_elf_from_binary(ecp, ifd, src);
else if (ecp->itf == ETF_IHEX)
create_elf_from_ihex(ecp, ifd);
else if (ecp->itf == ETF_SREC)
create_elf_from_srec(ecp, ifd);
else {
cleanup_tempfile(elftemp);
cleanup_tempfile(tempfile);
errx(EXIT_FAILURE, "Internal: invalid target flavour");
}
elf_end(ecp->eout);


close(ifd);
if ((ifd = open(elftemp, O_RDONLY)) == -1) {
cleanup_tempfile(elftemp);
cleanup_tempfile(tempfile);
err(EXIT_FAILURE, "open %s failed", src);
}
close(efd);
if (cleanup_tempfile(elftemp) < 0) {
cleanup_tempfile(tempfile);
err(EXIT_FAILURE, "unlink %s failed", elftemp);
}
free(elftemp);
elftemp = NULL;
}

if ((ecp->ein = elf_begin(ifd, ELF_C_READ, NULL)) == NULL) {
cleanup_tempfile(tempfile);
if (fstat(ifd, &sb) == 0 && sb.st_size == 0)
errx(EXIT_FAILURE, "file format not recognized");
errx(EXIT_FAILURE, "elf_begin() failed: %s",
elf_errmsg(-1));
}

switch (elf_kind(ecp->ein)) {
case ELF_K_NONE:
cleanup_tempfile(tempfile);
errx(EXIT_FAILURE, "file format not recognized");
case ELF_K_ELF:
if ((ecp->eout = elf_begin(ofd, ELF_C_WRITE, NULL)) == NULL) {
cleanup_tempfile(tempfile);
errx(EXIT_FAILURE, "elf_begin() failed: %s",
elf_errmsg(-1));
}


elf_flagelf(ecp->eout, ELF_C_SET, ELF_F_LAYOUT);




create_elf(ecp);
elf_end(ecp->eout);




if (ecp->otf != ETF_ELF) {




if (cleanup_tempfile(tempfile) < 0)
errx(EXIT_FAILURE, "unlink %s failed",
tempfile);
free(tempfile);
create_tempfile(src, &tempfile, &ofd0);





if (lseek(ofd, 0, SEEK_SET) < 0) {
cleanup_tempfile(tempfile);
err(EXIT_FAILURE,
"lseek failed for the output object");
}




switch (ecp->otf) {
case ETF_BINARY:
create_binary(ofd, ofd0);
break;
case ETF_IHEX:
create_ihex(ofd, ofd0);
break;
case ETF_SREC:
create_srec(ecp, ofd, ofd0,
dst != NULL ? dst : src);
break;
case ETF_PE:
case ETF_EFI:
#if WITH_PE
create_pe(ecp, ofd, ofd0);
#else
cleanup_tempfile(tempfile);
errx(EXIT_FAILURE, "PE/EFI support not enabled"
" at compile time");
#endif
break;
default:
cleanup_tempfile(tempfile);
errx(EXIT_FAILURE, "Internal: unsupported"
" output flavour %d", ecp->oec);
}

close(ofd);
ofd = ofd0;
}

break;

case ELF_K_AR:

break;
default:
cleanup_tempfile(tempfile);
errx(EXIT_FAILURE, "file format not supported");
}

elf_end(ecp->ein);

#if !defined(LIBELF_AR)
copy_done:
#endif

if (tempfile != NULL) {
in_place = 0;
if (dst == NULL) {
dst = src;
if (lstat(dst, &sb) != -1 &&
(sb.st_nlink > 1 || S_ISLNK(sb.st_mode)))
in_place = 1;
}

if (copy_from_tempfile(tempfile, dst, ofd,
&tfd, in_place) < 0) {
cleanup_tempfile(tempfile);
err(EXIT_FAILURE, "creation of %s failed", dst);
}


free(tempfile);
tempfile = NULL;

ofd = tfd;
}

if (strcmp(dst, "/dev/null") && fchmod(ofd, sb.st_mode) == -1)
err(EXIT_FAILURE, "fchmod %s failed", dst);

if ((ecp->flags & PRESERVE_DATE) &&
elftc_set_timestamps(dst, &sb) < 0)
err(EXIT_FAILURE, "setting timestamps failed");

close(ifd);
close(ofd);
}

static void
elfcopy_main(struct elfcopy *ecp, int argc, char **argv)
{
struct sec_action *sac;
const char *infile, *outfile;
char *fn, *s;
int opt;

while ((opt = getopt_long(argc, argv, "dB:gG:I:j:K:L:N:O:pR:s:SwW:xXV",
elfcopy_longopts, NULL)) != -1) {
switch(opt) {
case 'B':

break;
case 'R':
sac = lookup_sec_act(ecp, optarg, 1);
if (sac->copy != 0)
errx(EXIT_FAILURE,
"both copy and remove specified");
sac->remove = 1;
ecp->flags |= SEC_REMOVE;
break;
case 'S':
ecp->strip = STRIP_ALL;
break;
case 'g':
ecp->strip = STRIP_DEBUG;
break;
case 'G':
ecp->flags |= KEEP_GLOBAL;
add_to_symop_list(ecp, optarg, NULL, SYMOP_KEEPG);
break;
case 'I':
case 's':
set_input_target(ecp, optarg);
break;
case 'j':
sac = lookup_sec_act(ecp, optarg, 1);
if (sac->remove != 0)
errx(EXIT_FAILURE,
"both copy and remove specified");
sac->copy = 1;
ecp->flags |= SEC_COPY;
break;
case 'K':
add_to_symop_list(ecp, optarg, NULL, SYMOP_KEEP);
break;
case 'L':
add_to_symop_list(ecp, optarg, NULL, SYMOP_LOCALIZE);
break;
case 'N':
add_to_symop_list(ecp, optarg, NULL, SYMOP_STRIP);
break;
case 'O':
set_output_target(ecp, optarg);
break;
case 'p':
ecp->flags |= PRESERVE_DATE;
break;
case 'V':
print_version();
break;
case 'w':
ecp->flags |= WILDCARD;
break;
case 'W':
add_to_symop_list(ecp, optarg, NULL, SYMOP_WEAKEN);
break;
case 'x':
ecp->flags |= DISCARD_LOCAL;
break;
case 'X':
ecp->flags |= DISCARD_LLABEL;
break;
case ECP_ADD_GNU_DEBUGLINK:
ecp->debuglink = optarg;
break;
case ECP_ADD_SECTION:
add_section(ecp, optarg);
break;
case ECP_CHANGE_ADDR:
ecp->change_addr = (int64_t) strtoll(optarg, NULL, 0);
break;
case ECP_CHANGE_SEC_ADDR:
parse_sec_address_op(ecp, opt, "--change-section-addr",
optarg);
break;
case ECP_CHANGE_SEC_LMA:
parse_sec_address_op(ecp, opt, "--change-section-lma",
optarg);
break;
case ECP_CHANGE_SEC_VMA:
parse_sec_address_op(ecp, opt, "--change-section-vma",
optarg);
break;
case ECP_CHANGE_START:
ecp->change_start = (int64_t) strtoll(optarg, NULL, 0);
break;
case ECP_CHANGE_WARN:

break;
case ECP_GAP_FILL:
ecp->fill = (uint8_t) strtoul(optarg, NULL, 0);
ecp->flags |= GAP_FILL;
break;
case ECP_GLOBALIZE_SYMBOL:
add_to_symop_list(ecp, optarg, NULL, SYMOP_GLOBALIZE);
break;
case ECP_GLOBALIZE_SYMBOLS:
parse_symlist_file(ecp, optarg, SYMOP_GLOBALIZE);
break;
case ECP_KEEP_SYMBOLS:
parse_symlist_file(ecp, optarg, SYMOP_KEEP);
break;
case ECP_KEEP_GLOBAL_SYMBOLS:
parse_symlist_file(ecp, optarg, SYMOP_KEEPG);
break;
case ECP_LOCALIZE_HIDDEN:
ecp->flags |= LOCALIZE_HIDDEN;
break;
case ECP_LOCALIZE_SYMBOLS:
parse_symlist_file(ecp, optarg, SYMOP_LOCALIZE);
break;
case ECP_NO_CHANGE_WARN:
ecp->flags |= NO_CHANGE_WARN;
break;
case ECP_ONLY_DEBUG:
ecp->strip = STRIP_NONDEBUG;
break;
case ECP_ONLY_DWO:
ecp->strip = STRIP_NONDWO;
break;
case ECP_PAD_TO:
ecp->pad_to = (uint64_t) strtoull(optarg, NULL, 0);
break;
case ECP_PREFIX_ALLOC:
ecp->prefix_alloc = optarg;
break;
case ECP_PREFIX_SEC:
ecp->prefix_sec = optarg;
break;
case ECP_PREFIX_SYM:
ecp->prefix_sym = optarg;
break;
case ECP_REDEF_SYMBOL:
if ((s = strchr(optarg, '=')) == NULL)
errx(EXIT_FAILURE,
"illegal format for --redefine-sym");
*s++ = '\0';
add_to_symop_list(ecp, optarg, s, SYMOP_REDEF);
break;
case ECP_REDEF_SYMBOLS:
parse_symlist_file(ecp, optarg, SYMOP_REDEF);
break;
case ECP_RENAME_SECTION:
if ((fn = strchr(optarg, '=')) == NULL)
errx(EXIT_FAILURE,
"illegal format for --rename-section");
*fn++ = '\0';


if ((s = strchr(fn, ',')) != NULL)
*s++ = '\0';

sac = lookup_sec_act(ecp, optarg, 1);
sac->rename = 1;
sac->newname = fn;
if (s != NULL)
parse_sec_flags(sac, s);
break;
case ECP_SET_OSABI:
set_osabi(ecp, optarg);
break;
case ECP_SET_SEC_FLAGS:
if ((s = strchr(optarg, '=')) == NULL)
errx(EXIT_FAILURE,
"illegal format for --set-section-flags");
*s++ = '\0';
sac = lookup_sec_act(ecp, optarg, 1);
parse_sec_flags(sac, s);
break;
case ECP_SET_START:
ecp->flags |= SET_START;
ecp->set_start = (uint64_t) strtoull(optarg, NULL, 0);
break;
case ECP_SREC_FORCE_S3:
ecp->flags |= SREC_FORCE_S3;
break;
case ECP_SREC_LEN:
ecp->flags |= SREC_FORCE_LEN;
ecp->srec_len = strtoul(optarg, NULL, 0);
break;
case ECP_STRIP_DWO:
ecp->strip = STRIP_DWO;
break;
case ECP_STRIP_SYMBOLS:
parse_symlist_file(ecp, optarg, SYMOP_STRIP);
break;
case ECP_STRIP_UNNEEDED:
ecp->strip = STRIP_UNNEEDED;
break;
case ECP_WEAKEN_ALL:
ecp->flags |= WEAKEN_ALL;
break;
case ECP_WEAKEN_SYMBOLS:
parse_symlist_file(ecp, optarg, SYMOP_WEAKEN);
break;
default:
elfcopy_usage();
}
}

argc -= optind;
argv += optind;

if (argc == 0 || argc > 2)
elfcopy_usage();

infile = argv[0];
outfile = NULL;
if (argc > 1)
outfile = argv[1];

create_file(ecp, infile, outfile);
}

static void
mcs_main(struct elfcopy *ecp, int argc, char **argv)
{
struct sec_action *sac;
const char *string;
int append, delete, compress, name, print;
int opt, i;

append = delete = compress = name = print = 0;
string = NULL;
while ((opt = getopt_long(argc, argv, "a:cdhn:pV", mcs_longopts,
NULL)) != -1) {
switch(opt) {
case 'a':
append = 1;
string = optarg;
break;
case 'c':
compress = 1;
break;
case 'd':
delete = 1;
break;
case 'n':
name = 1;
(void)lookup_sec_act(ecp, optarg, 1);
break;
case 'p':
print = 1;
break;
case 'V':
print_version();
break;
case 'h':
default:
mcs_usage();
}
}

argc -= optind;
argv += optind;

if (argc == 0)
mcs_usage();


if (!append && !compress && !delete && !print)
mcs_usage();








if (delete) {
append = compress = print = 0;
ecp->flags |= SEC_REMOVE;
}
if (append)
ecp->flags |= SEC_APPEND;
if (compress)
ecp->flags |= SEC_COMPRESS;
if (print)
ecp->flags |= SEC_PRINT;


if (!name)
(void)lookup_sec_act(ecp, ".comment", 1);

STAILQ_FOREACH(sac, &ecp->v_sac, sac_list) {
sac->append = append;
sac->compress = compress;
sac->print = print;
sac->remove = delete;
sac->string = string;
}

for (i = 0; i < argc; i++) {

if (print && !append && !compress && !delete)
create_file(ecp, argv[i], "/dev/null");
else
create_file(ecp, argv[i], NULL);
}
}

static void
strip_main(struct elfcopy *ecp, int argc, char **argv)
{
struct sec_action *sac;
const char *outfile;
int opt;
int i;

outfile = NULL;
while ((opt = getopt_long(argc, argv, "hI:K:N:o:O:pR:sSdgVxXw",
strip_longopts, NULL)) != -1) {
switch(opt) {
case 'R':
sac = lookup_sec_act(ecp, optarg, 1);
sac->remove = 1;
ecp->flags |= SEC_REMOVE;
break;
case 's':
ecp->strip = STRIP_ALL;
break;
case 'S':
case 'g':
case 'd':
ecp->strip = STRIP_DEBUG;
break;
case 'I':

break;
case 'K':
add_to_symop_list(ecp, optarg, NULL, SYMOP_KEEP);
break;
case 'N':
add_to_symop_list(ecp, optarg, NULL, SYMOP_STRIP);
break;
case 'o':
outfile = optarg;
break;
case 'O':
set_output_target(ecp, optarg);
break;
case 'p':
ecp->flags |= PRESERVE_DATE;
break;
case 'V':
print_version();
break;
case 'w':
ecp->flags |= WILDCARD;
break;
case 'x':
ecp->flags |= DISCARD_LOCAL;
break;
case 'X':
ecp->flags |= DISCARD_LLABEL;
break;
case ECP_ONLY_DEBUG:
ecp->strip = STRIP_NONDEBUG;
break;
case ECP_STRIP_UNNEEDED:
ecp->strip = STRIP_UNNEEDED;
break;
case 'h':
default:
strip_usage();
}
}

argc -= optind;
argv += optind;

if (ecp->strip == 0 &&
((ecp->flags & DISCARD_LOCAL) == 0) &&
((ecp->flags & DISCARD_LLABEL) == 0) &&
lookup_symop_list(ecp, NULL, SYMOP_STRIP) == NULL)
ecp->strip = STRIP_ALL;
if (argc == 0)
strip_usage();




if (outfile != NULL && argc != 1)
strip_usage();

for (i = 0; i < argc; i++)
create_file(ecp, argv[i], outfile);
}

static void
parse_sec_flags(struct sec_action *sac, char *s)
{
const char *flag;
int found, i;

for (flag = strtok(s, ","); flag; flag = strtok(NULL, ",")) {
found = 0;
for (i = 0; sec_flags[i].name != NULL; i++)
if (strcasecmp(sec_flags[i].name, flag) == 0) {
sac->flags |= sec_flags[i].value;
found = 1;
break;
}
if (!found)
errx(EXIT_FAILURE, "unrecognized section flag %s",
flag);
}
}

static void
parse_sec_address_op(struct elfcopy *ecp, int optnum, const char *optname,
char *s)
{
struct sec_action *sac;
const char *name;
char *v;
char op;

name = v = s;
do {
v++;
} while (*v != '\0' && *v != '=' && *v != '+' && *v != '-');
if (*v == '\0' || *(v + 1) == '\0')
errx(EXIT_FAILURE, "invalid format for %s", optname);
op = *v;
*v++ = '\0';
sac = lookup_sec_act(ecp, name, 1);
switch (op) {
case '=':
if (optnum == ECP_CHANGE_SEC_LMA ||
optnum == ECP_CHANGE_SEC_ADDR) {
sac->setlma = 1;
sac->lma = (uint64_t) strtoull(v, NULL, 0);
}
if (optnum == ECP_CHANGE_SEC_VMA ||
optnum == ECP_CHANGE_SEC_ADDR) {
sac->setvma = 1;
sac->vma = (uint64_t) strtoull(v, NULL, 0);
}
break;
case '+':
if (optnum == ECP_CHANGE_SEC_LMA ||
optnum == ECP_CHANGE_SEC_ADDR)
sac->lma_adjust = (int64_t) strtoll(v, NULL, 0);
if (optnum == ECP_CHANGE_SEC_VMA ||
optnum == ECP_CHANGE_SEC_ADDR)
sac->vma_adjust = (int64_t) strtoll(v, NULL, 0);
break;
case '-':
if (optnum == ECP_CHANGE_SEC_LMA ||
optnum == ECP_CHANGE_SEC_ADDR)
sac->lma_adjust = (int64_t) -strtoll(v, NULL, 0);
if (optnum == ECP_CHANGE_SEC_VMA ||
optnum == ECP_CHANGE_SEC_ADDR)
sac->vma_adjust = (int64_t) -strtoll(v, NULL, 0);
break;
default:
break;
}
}

static void
parse_symlist_file(struct elfcopy *ecp, const char *fn, unsigned int op)
{
struct symfile *sf;
struct stat sb;
FILE *fp;
char *data, *p, *line, *end, *e, *n;

if (stat(fn, &sb) == -1)
err(EXIT_FAILURE, "stat %s failed", fn);


STAILQ_FOREACH(sf, &ecp->v_symfile, symfile_list) {
if (sf->dev == sb.st_dev && sf->ino == sb.st_ino)
goto process_symfile;
}

if ((fp = fopen(fn, "r")) == NULL)
err(EXIT_FAILURE, "can not open %s", fn);
if ((data = malloc(sb.st_size + 1)) == NULL)
err(EXIT_FAILURE, "malloc failed");
if (sb.st_size > 0)
if (fread(data, sb.st_size, 1, fp) != 1)
err(EXIT_FAILURE, "fread failed");
fclose(fp);
data[sb.st_size] = '\0';

if ((sf = calloc(1, sizeof(*sf))) == NULL)
err(EXIT_FAILURE, "malloc failed");
sf->dev = sb.st_dev;
sf->ino = sb.st_ino;
sf->size = sb.st_size + 1;
sf->data = data;

process_symfile:






end = sf->data + sf->size;
line = NULL;
for(p = sf->data; p < end; p++) {
if ((*p == '\t' || *p == ' ') && line == NULL)
continue;
if (*p == '\r' || *p == '\n' || *p == '\0') {
*p = '\0';
if (line == NULL)
continue;


if (*line == '#') {
line = NULL;
continue;
}

e = p - 1;
while(e != line && (*e == '\t' || *e == ' '))
*e-- = '\0';
if (op != SYMOP_REDEF)
add_to_symop_list(ecp, line, NULL, op);
else {
if (strlen(line) < 3)
errx(EXIT_FAILURE,
"illegal format for"
" --redefine-sym");
for(n = line + 1; n < e; n++) {
if (*n == ' ' || *n == '\t') {
while(*n == ' ' || *n == '\t')
*n++ = '\0';
break;
}
}
if (n >= e)
errx(EXIT_FAILURE,
"illegal format for"
" --redefine-sym");
add_to_symop_list(ecp, line, n, op);
}
line = NULL;
continue;
}

if (line == NULL)
line = p;
}
}

static void
set_input_target(struct elfcopy *ecp, const char *target_name)
{
Elftc_Bfd_Target *tgt;

if ((tgt = elftc_bfd_find_target(target_name)) == NULL)
errx(EXIT_FAILURE, "%s: invalid target name", target_name);
ecp->itf = elftc_bfd_target_flavor(tgt);
}

static void
set_output_target(struct elfcopy *ecp, const char *target_name)
{
Elftc_Bfd_Target *tgt;

if ((tgt = elftc_bfd_find_target(target_name)) == NULL)
errx(EXIT_FAILURE, "%s: invalid target name", target_name);
ecp->otf = elftc_bfd_target_flavor(tgt);
if (ecp->otf == ETF_ELF) {
ecp->oec = elftc_bfd_target_class(tgt);
ecp->oed = elftc_bfd_target_byteorder(tgt);
ecp->oem = elftc_bfd_target_machine(tgt);
ecp->abi = elftc_bfd_target_osabi(tgt);
}
if (ecp->otf == ETF_EFI || ecp->otf == ETF_PE)
ecp->oem = elftc_bfd_target_machine(tgt);

ecp->otgt = target_name;
}

static void
set_osabi(struct elfcopy *ecp, const char *abi)
{
int i, found;

found = 0;
for (i = 0; osabis[i].name != NULL; i++)
if (strcasecmp(osabis[i].name, abi) == 0) {
ecp->abi = osabis[i].abi;
found = 1;
break;
}
if (!found)
errx(EXIT_FAILURE, "unrecognized OSABI %s", abi);
}

#define ELFCOPY_USAGE_MESSAGE "Usage: %s [options] infile [outfile]\nTransform object files.\n\nOptions:\n-d | -g | --strip-debug Remove debugging information from the output.\n-j SECTION | --only-section=SECTION\nCopy only the named section to the output.\n-p | --preserve-dates Preserve access and modification times.\n-w | --wildcard Use shell-style patterns to name symbols.\n-x | --discard-all Do not copy non-globals to the output.\n-I FORMAT | --input-target=FORMAT\nSpecify object format for the input file.\n-K SYM | --keep-symbol=SYM Copy symbol SYM to the output.\n-L SYM | --localize-symbol=SYM\nMake symbol SYM local to the output file.\n-N SYM | --strip-symbol=SYM Do not copy symbol SYM to the output.\n-O FORMAT | --output-target=FORMAT\nSpecify object format for the output file.\nFORMAT should be a target name understood by\nelftc_bfd_find_target(3).\n-R NAME | --remove-section=NAME\nRemove the named section.\n-S | --strip-all Remove all symbol and relocation information\nfrom the output.\n-V | --version Print a version identifier and exit.\n-W SYM | --weaken-symbol=SYM Mark symbol SYM as weak in the output.\n-X | --discard-locals Do not copy compiler generated symbols to\nthe output.\n--add-section NAME=FILE Add the contents of FILE to the ELF object as\na new section named NAME.\n--adjust-section-vma SECTION{=,+,-}VAL | \\\n--change-section-address SECTION{=,+,-}VAL\nSet or adjust the VMA and the LMA of the\nnamed section by VAL.\n--adjust-start=INCR | --change-start=INCR\nAdd INCR to the start address for the ELF\nobject.\n--adjust-vma=INCR | --change-addresses=INCR\nIncrease the VMA and LMA of all sections by\nINCR.\n--adjust-warning | --change-warnings\nIssue warnings for non-existent sections.\n--change-section-lma SECTION{=,+,-}VAL\nSet or adjust the LMA address of the named\nsection by VAL.\n--change-section-vma SECTION{=,+,-}VAL\nSet or adjust the VMA address of the named\nsection by VAL.\n--gap-fill=VAL Fill the gaps between sections with bytes\nof value VAL.\n--localize-hidden Make all hidden symbols local to the output\nfile.\n--no-adjust-warning| --no-change-warnings\nDo not issue warnings for non-existent\nsections.\n--only-keep-debug Copy only debugging information.\n--output-target=FORMAT Use the specified format for the output.\n--pad-to=ADDRESS Pad the output object up to the given address.\n--prefix-alloc-sections=STRING\nPrefix the section names of all the allocated\nsections with STRING.\n--prefix-sections=STRING Prefix the section names of all the sections\nwith STRING.\n--prefix-symbols=STRING Prefix the symbol names of all the symbols\nwith STRING.\n--rename-section OLDNAME=NEWNAME[,FLAGS]\nRename and optionally change section flags.\n--set-section-flags SECTION=FLAGS\nSet section flags for the named section.\nSupported flags are: 'alloc', 'code',\n'contents', 'data', 'debug', 'load',\n'noload', 'readonly', 'rom', and 'shared'.\n--set-start=ADDRESS Set the start address of the ELF object.\n--srec-forceS3 Only generate S3 S-Records.\n--srec-len=LEN Set the maximum length of a S-Record line.\n--strip-unneeded Do not copy relocation information.\n"












































































static void
elfcopy_usage(void)
{
(void) fprintf(stderr, ELFCOPY_USAGE_MESSAGE, ELFTC_GETPROGNAME());
exit(EXIT_FAILURE);
}

#define MCS_USAGE_MESSAGE "Usage: %s [options] file...\nManipulate the comment section in an ELF object.\n\nOptions:\n-a STRING Append 'STRING' to the comment section.\n-c Remove duplicate entries from the comment section.\n-d Delete the comment section.\n-h | --help Print a help message and exit.\n-n NAME Operate on the ELF section with name 'NAME'.\n-p Print the contents of the comment section.\n-V | --version Print a version identifier and exit.\n"











static void
mcs_usage(void)
{
(void) fprintf(stderr, MCS_USAGE_MESSAGE, ELFTC_GETPROGNAME());
exit(EXIT_FAILURE);
}

#define STRIP_USAGE_MESSAGE "Usage: %s [options] file...\nDiscard information from ELF objects.\n\nOptions:\n-d | -g | -S | --strip-debug Remove debugging symbols.\n-h | --help Print a help message.\n-o FILE | --output-file FILE Write output to FILE.\n--only-keep-debug Keep debugging information only.\n-p | --preserve-dates Preserve access and modification times.\n-s | --strip-all Remove all symbols.\n--strip-unneeded Remove symbols not needed for relocation\nprocessing.\n-w | --wildcard Use shell-style patterns to name symbols.\n-x | --discard-all Discard all non-global symbols.\n-I TGT| --input-target=TGT (Accepted, but ignored).\n-K SYM | --keep-symbol=SYM Keep symbol 'SYM' in the output.\n-N SYM | --strip-symbol=SYM Remove symbol 'SYM' from the output.\n-O TGT | --output-target=TGT Set the output file format to 'TGT'.\n-R SEC | --remove-section=SEC Remove the section named 'SEC'.\n-V | --version Print a version identifier and exit.\n-X | --discard-locals Remove compiler-generated local symbols.\n"





















static void
strip_usage(void)
{
(void) fprintf(stderr, STRIP_USAGE_MESSAGE, ELFTC_GETPROGNAME());
exit(EXIT_FAILURE);
}

static void
print_version(void)
{
(void) printf("%s (%s)\n", ELFTC_GETPROGNAME(), elftc_version());
exit(EXIT_SUCCESS);
}




static int
strrcmp(const char *s, const char *end)
{
size_t endlen, slen;

slen = strlen(s);
endlen = strlen(end);

if (slen >= endlen)
s += slen - endlen;
return (strcmp(s, end));
}

int
main(int argc, char **argv)
{
struct elfcopy *ecp;

if (elf_version(EV_CURRENT) == EV_NONE)
errx(EXIT_FAILURE, "ELF library initialization failed: %s",
elf_errmsg(-1));

ecp = calloc(1, sizeof(*ecp));
if (ecp == NULL)
err(EXIT_FAILURE, "calloc failed");

ecp->itf = ecp->otf = ETF_ELF;
ecp->iec = ecp->oec = ELFCLASSNONE;
ecp->oed = ELFDATANONE;
ecp->abi = -1;

ecp->nos = 1;
ecp->fill = 0;

STAILQ_INIT(&ecp->v_seg);
STAILQ_INIT(&ecp->v_sac);
STAILQ_INIT(&ecp->v_sadd);
STAILQ_INIT(&ecp->v_symop);
STAILQ_INIT(&ecp->v_symfile);
STAILQ_INIT(&ecp->v_arobj);
TAILQ_INIT(&ecp->v_sec);

if ((ecp->progname = ELFTC_GETPROGNAME()) == NULL)
ecp->progname = "elfcopy";

if (strrcmp(ecp->progname, "strip") == 0)
strip_main(ecp, argc, argv);
else if (strrcmp(ecp->progname, "mcs") == 0)
mcs_main(ecp, argc, argv);
else {
if (strrcmp(ecp->progname, "elfcopy") != 0 &&
strrcmp(ecp->progname, "objcopy") != 0)
warnx("program mode not known, defaulting to elfcopy");
elfcopy_main(ecp, argc, argv);
}

free_sec_add(ecp);
free_sec_act(ecp);
free(ecp);

exit(EXIT_SUCCESS);
}
