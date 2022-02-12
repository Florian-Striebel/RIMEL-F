


















































































































#if defined(HAVE_CONFIG_H)
#include "config.h"
#endif
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/param.h>
#if defined(HAVE_AR_H)
#include <ar.h>
#else
struct ar_hdr {
char ar_name[16];
char ar_date[12];
char ar_uid[6];
char ar_gid[6];
char ar_mode[8];
char ar_size[10];
#if !defined(ARFMAG)
#define ARFMAG "`\n"
#endif
char ar_fmag[2];
};
#endif
#if defined(HAVE_RANLIB_H) && !(defined(__ELF__) || defined(NO_RANLIB))
#include <ranlib.h>
#endif
#if defined(HAVE_UTIME_H)
#include <utime.h>
#endif

#include "make.h"
#include "dir.h"


MAKE_RCSID("$NetBSD: arch.c,v 1.205 2021/12/12 22:41:47 rillig Exp $");

typedef struct List ArchList;
typedef struct ListNode ArchListNode;

static ArchList archives;

typedef struct Arch {
char *name;
HashTable members;

char *fnametab;
size_t fnamesize;
} Arch;

static FILE *ArchFindMember(const char *, const char *,
struct ar_hdr *, const char *);
#if defined(__svr4__) || defined(__SVR4) || defined(__ELF__)
#define SVR4ARCHIVES
static int ArchSVR4Entry(Arch *, char *, size_t, FILE *);
#endif


#if defined(_AIX)
#define AR_NAME _ar_name.ar_name
#define AR_FMAG _ar_name.ar_fmag
#define SARMAG SAIAMAG
#define ARMAG AIAMAG
#define ARFMAG AIAFMAG
#endif
#if !defined(AR_NAME)
#define AR_NAME ar_name
#endif
#if !defined(AR_DATE)
#define AR_DATE ar_date
#endif
#if !defined(AR_SIZE)
#define AR_SIZE ar_size
#endif
#if !defined(AR_FMAG)
#define AR_FMAG ar_fmag
#endif
#if !defined(ARMAG)
#define ARMAG "!<arch>\n"
#endif
#if !defined(SARMAG)
#define SARMAG 8
#endif


#if defined(CLEANUP)
static void
ArchFree(void *ap)
{
Arch *a = ap;
HashIter hi;


HashIter_Init(&hi, &a->members);
while (HashIter_Next(&hi) != NULL)
free(hi.entry->value);

free(a->name);
free(a->fnametab);
HashTable_Done(&a->members);
free(a);
}
#endif


static char *
FullName(const char *archive, const char *member)
{
size_t len1 = strlen(archive);
size_t len3 = strlen(member);
char *result = bmake_malloc(len1 + 1 + len3 + 1 + 1);
memcpy(result, archive, len1);
memcpy(result + len1, "(", 1);
memcpy(result + len1 + 1, member, len3);
memcpy(result + len1 + 1 + len3, ")", 1 + 1);
return result;
}















bool
Arch_ParseArchive(char **pp, GNodeList *gns, GNode *scope)
{
char *cp;
GNode *gn;
MFStr libName;
FStr mem;
char saveChar;
bool expandLibName;



libName = MFStr_InitRefer(*pp);
expandLibName = false;

for (cp = libName.str; *cp != '(' && *cp != '\0';) {
if (*cp == '$') {


const char *nested_p = cp;
FStr result;
bool isError;


(void)Var_Parse(&nested_p, scope,
VARE_UNDEFERR, &result);

isError = result.str == var_Error;
FStr_Done(&result);
if (isError)
return false;

expandLibName = true;
cp += nested_p - cp;
} else
cp++;
}

*cp++ = '\0';
if (expandLibName) {
char *expanded;
(void)Var_Subst(libName.str, scope, VARE_UNDEFERR, &expanded);

libName = MFStr_InitOwn(expanded);
}


for (;;) {





bool doSubst = false;

pp_skip_whitespace(&cp);

mem = FStr_InitRefer(cp);
while (*cp != '\0' && *cp != ')' && !ch_isspace(*cp)) {
if (*cp == '$') {


FStr result;
bool isError;
const char *nested_p = cp;

(void)Var_Parse(&nested_p, scope,
VARE_UNDEFERR, &result);

isError = result.str == var_Error;
FStr_Done(&result);

if (isError)
return false;

doSubst = true;
cp += nested_p - cp;
} else {
cp++;
}
}







if (*cp == '\0') {
Parse_Error(PARSE_FATAL,
"No closing parenthesis "
"in archive specification");
return false;
}




if (cp == mem.str)
break;

saveChar = *cp;
*cp = '\0';














if (doSubst) {
char *fullName;
char *p, *expandedMem;
const char *unexpandedMem = mem.str;

(void)Var_Subst(mem.str, scope, VARE_UNDEFERR,
&expandedMem);

mem = FStr_InitOwn(expandedMem);





fullName = FullName(libName.str, mem.str);
p = fullName;

if (strcmp(mem.str, unexpandedMem) == 0) {






gn = Targ_GetNode(fullName);
gn->type |= OP_ARCHV;
Lst_Append(gns, gn);

} else if (!Arch_ParseArchive(&p, gns, scope)) {

free(fullName);

return false;
}
free(fullName);


} else if (Dir_HasWildcards(mem.str)) {
StringList members = LST_INIT;
SearchPath_Expand(&dirSearchPath, mem.str, &members);

while (!Lst_IsEmpty(&members)) {
char *member = Lst_Dequeue(&members);
char *fullname = FullName(libName.str, member);
free(member);

gn = Targ_GetNode(fullname);
free(fullname);

gn->type |= OP_ARCHV;
Lst_Append(gns, gn);
}
Lst_Done(&members);

} else {
char *fullname = FullName(libName.str, mem.str);
gn = Targ_GetNode(fullname);
free(fullname);








gn->type |= OP_ARCHV;
Lst_Append(gns, gn);
}
FStr_Done(&mem);

*cp = saveChar;
}

MFStr_Done(&libName);

cp++;

pp_skip_whitespace(&cp);
*pp = cp;
return true;
}















static struct ar_hdr *
ArchStatMember(const char *archive, const char *member, bool addToCache)
{
#define AR_MAX_NAME_LEN (sizeof arh.ar_name - 1)
FILE *arch;
size_t size;
char magic[SARMAG];
ArchListNode *ln;
Arch *ar;
struct ar_hdr arh;
char memName[MAXPATHLEN + 1];






member = str_basename(member);

for (ln = archives.first; ln != NULL; ln = ln->next) {
const Arch *a = ln->datum;
if (strcmp(a->name, archive) == 0)
break;
}

if (ln != NULL) {
struct ar_hdr *hdr;

ar = ln->datum;
hdr = HashTable_FindValue(&ar->members, member);
if (hdr != NULL)
return hdr;

{

char copy[AR_MAX_NAME_LEN + 1];
size_t len = strlen(member);

if (len > AR_MAX_NAME_LEN) {
snprintf(copy, sizeof copy, "%s", member);
hdr = HashTable_FindValue(&ar->members, copy);
}
return hdr;
}
}

if (!addToCache) {







static struct ar_hdr sarh;

arch = ArchFindMember(archive, member, &sarh, "r");
if (arch == NULL)
return NULL;

fclose(arch);
return &sarh;
}





arch = fopen(archive, "r");
if (arch == NULL)
return NULL;





if (fread(magic, SARMAG, 1, arch) != 1 ||
strncmp(magic, ARMAG, SARMAG) != 0) {
(void)fclose(arch);
return NULL;
}

ar = bmake_malloc(sizeof *ar);
ar->name = bmake_strdup(archive);
ar->fnametab = NULL;
ar->fnamesize = 0;
HashTable_Init(&ar->members);
memName[AR_MAX_NAME_LEN] = '\0';

while (fread(&arh, sizeof arh, 1, arch) == 1) {
char *nameend;


if (strncmp(arh.AR_FMAG, ARFMAG, sizeof arh.AR_FMAG) != 0)
goto badarch;








arh.AR_SIZE[sizeof arh.AR_SIZE - 1] = '\0';
size = (size_t)strtol(arh.AR_SIZE, NULL, 10);

memcpy(memName, arh.AR_NAME, sizeof arh.AR_NAME);
nameend = memName + AR_MAX_NAME_LEN;
while (nameend > memName && *nameend == ' ')
nameend--;
nameend[1] = '\0';

#if defined(SVR4ARCHIVES)




if (memName[0] == '/') {

switch (ArchSVR4Entry(ar, memName, size, arch)) {
case -1:
goto badarch;
case 0:
continue;
default:
break;
}
} else {
if (nameend[0] == '/')
nameend[0] = '\0';
}
#endif

#if defined(AR_EFMT1)




if (strncmp(memName, AR_EFMT1, sizeof AR_EFMT1 - 1) == 0 &&
ch_isdigit(memName[sizeof AR_EFMT1 - 1])) {

size_t elen = atoi(memName + sizeof AR_EFMT1 - 1);

if (elen > MAXPATHLEN)
goto badarch;
if (fread(memName, elen, 1, arch) != 1)
goto badarch;
memName[elen] = '\0';
if (fseek(arch, -(long)elen, SEEK_CUR) != 0)
goto badarch;
if (DEBUG(ARCH) || DEBUG(MAKE))
debug_printf(
"ArchStatMember: "
"Extended format entry for %s\n",
memName);
}
#endif

{
struct ar_hdr *cached_hdr = bmake_malloc(
sizeof *cached_hdr);
memcpy(cached_hdr, &arh, sizeof arh);
HashTable_Set(&ar->members, memName, cached_hdr);
}

if (fseek(arch, ((long)size + 1) & ~1, SEEK_CUR) != 0)
goto badarch;
}

fclose(arch);

Lst_Append(&archives, ar);





return HashTable_FindValue(&ar->members, member);

badarch:
fclose(arch);
HashTable_Done(&ar->members);
free(ar->fnametab);
free(ar);
return NULL;
}

#if defined(SVR4ARCHIVES)













static int
ArchSVR4Entry(Arch *ar, char *inout_name, size_t size, FILE *arch)
{
#define ARLONGNAMES1 "//"
#define ARLONGNAMES2 "/ARFILENAMES"
size_t entry;
char *ptr, *eptr;

if (strncmp(inout_name, ARLONGNAMES1, sizeof ARLONGNAMES1 - 1) == 0 ||
strncmp(inout_name, ARLONGNAMES2, sizeof ARLONGNAMES2 - 1) == 0) {

if (ar->fnametab != NULL) {
DEBUG0(ARCH,
"Attempted to redefine an SVR4 name table\n");
return -1;
}





ar->fnametab = bmake_malloc(size);
ar->fnamesize = size;

if (fread(ar->fnametab, size, 1, arch) != 1) {
DEBUG0(ARCH, "Reading an SVR4 name table failed\n");
return -1;
}
eptr = ar->fnametab + size;
for (entry = 0, ptr = ar->fnametab; ptr < eptr; ptr++)
if (*ptr == '/') {
entry++;
*ptr = '\0';
}
DEBUG1(ARCH, "Found svr4 archive name table with %lu entries\n",
(unsigned long)entry);
return 0;
}

if (inout_name[1] == ' ' || inout_name[1] == '\0')
return 2;

entry = (size_t)strtol(&inout_name[1], &eptr, 0);
if ((*eptr != ' ' && *eptr != '\0') || eptr == &inout_name[1]) {
DEBUG1(ARCH, "Could not parse SVR4 name %s\n", inout_name);
return 2;
}
if (entry >= ar->fnamesize) {
DEBUG2(ARCH, "SVR4 entry offset %s is greater than %lu\n",
inout_name, (unsigned long)ar->fnamesize);
return 2;
}

DEBUG2(ARCH, "Replaced %s with %s\n", inout_name, &ar->fnametab[entry]);

snprintf(inout_name, MAXPATHLEN + 1, "%s", &ar->fnametab[entry]);
return 1;
}
#endif


static bool
ArchiveMember_HasName(const struct ar_hdr *hdr,
const char *name, size_t namelen)
{
const size_t ar_name_len = sizeof hdr->AR_NAME;
const char *ar_name = hdr->AR_NAME;

if (strncmp(ar_name, name, namelen) != 0)
return false;

if (namelen >= ar_name_len)
return namelen == ar_name_len;


if (ar_name[namelen] == ' ')
return true;



if (ar_name[namelen] == '/' &&
(namelen == ar_name_len || ar_name[namelen + 1] == ' '))
return true;

return false;
}




















static FILE *
ArchFindMember(const char *archive, const char *member, struct ar_hdr *out_arh,
const char *mode)
{
FILE *arch;
int size;
char magic[SARMAG];
size_t len;

arch = fopen(archive, mode);
if (arch == NULL)
return NULL;





if (fread(magic, SARMAG, 1, arch) != 1 ||
strncmp(magic, ARMAG, SARMAG) != 0) {
fclose(arch);
return NULL;
}





member = str_basename(member);

len = strlen(member);

while (fread(out_arh, sizeof *out_arh, 1, arch) == 1) {

if (strncmp(out_arh->AR_FMAG, ARFMAG,
sizeof out_arh->AR_FMAG) != 0) {




fclose(arch);
return NULL;
}

DEBUG5(ARCH, "Reading archive %s member %.*s mtime %.*s\n",
archive,
(int)sizeof out_arh->AR_NAME, out_arh->AR_NAME,
(int)sizeof out_arh->ar_date, out_arh->ar_date);

if (ArchiveMember_HasName(out_arh, member, len)) {








if (fseek(arch, -(long)sizeof *out_arh, SEEK_CUR) !=
0) {
fclose(arch);
return NULL;
}
return arch;
}

#if defined(AR_EFMT1)




if (strncmp(out_arh->AR_NAME, AR_EFMT1, sizeof AR_EFMT1 - 1) ==
0 &&
(ch_isdigit(out_arh->AR_NAME[sizeof AR_EFMT1 - 1]))) {
size_t elen = atoi(
&out_arh->AR_NAME[sizeof AR_EFMT1 - 1]);
char ename[MAXPATHLEN + 1];

if (elen > MAXPATHLEN) {
fclose(arch);
return NULL;
}
if (fread(ename, elen, 1, arch) != 1) {
fclose(arch);
return NULL;
}
ename[elen] = '\0';
if (DEBUG(ARCH) || DEBUG(MAKE))
debug_printf(
"ArchFindMember: "
"Extended format entry for %s\n",
ename);
if (strncmp(ename, member, len) == 0) {

if (fseek(arch,
-(long)(sizeof(struct ar_hdr) - elen),
SEEK_CUR) != 0) {
fclose(arch);
return NULL;
}
return arch;
}
if (fseek(arch, -(long)elen, SEEK_CUR) != 0) {
fclose(arch);
return NULL;
}
}
#endif








out_arh->AR_SIZE[sizeof out_arh->AR_SIZE - 1] = '\0';
size = (int)strtol(out_arh->AR_SIZE, NULL, 10);
if (fseek(arch, (size + 1) & ~1L, SEEK_CUR) != 0) {
fclose(arch);
return NULL;
}
}

fclose(arch);
return NULL;
}














void
Arch_Touch(GNode *gn)
{
FILE *f;
struct ar_hdr arh;

f = ArchFindMember(GNode_VarArchive(gn), GNode_VarMember(gn), &arh,
"r+");
if (f == NULL)
return;

snprintf(arh.ar_date, sizeof arh.ar_date, "%-ld", (unsigned long)now);
(void)fwrite(&arh, sizeof arh, 1, f);
fclose(f);
}









void
Arch_TouchLib(GNode *gn MAKE_ATTR_UNUSED)
{
#if defined(RANLIBMAG)
FILE *f;
struct ar_hdr arh;
struct utimbuf times;

f = ArchFindMember(gn->path, RANLIBMAG, &arh, "r+");
if (f == NULL)
return;

snprintf(arh.ar_date, sizeof arh.ar_date, "%-ld", (unsigned long)now);
(void)fwrite(&arh, sizeof arh, 1, f);
fclose(f);

times.actime = times.modtime = now;
utime(gn->path, &times);
#endif
}





void
Arch_UpdateMTime(GNode *gn)
{
struct ar_hdr *arh;

arh = ArchStatMember(GNode_VarArchive(gn), GNode_VarMember(gn), true);
if (arh != NULL)
gn->mtime = (time_t)strtol(arh->ar_date, NULL, 10);
else
gn->mtime = 0;
}





void
Arch_UpdateMemberMTime(GNode *gn)
{
GNodeListNode *ln;

for (ln = gn->parents.first; ln != NULL; ln = ln->next) {
GNode *pgn = ln->datum;

if (pgn->type & OP_ARCHV) {








const char *nameStart = strchr(pgn->name, '(') + 1;
const char *nameEnd = strchr(nameStart, ')');
size_t nameLen = (size_t)(nameEnd - nameStart);

if (pgn->flags.remake &&
strncmp(nameStart, gn->name, nameLen) == 0) {
Arch_UpdateMTime(pgn);
gn->mtime = pgn->mtime;
}
} else if (pgn->flags.remake) {




gn->mtime = 0;
break;
}
}
}
















void
Arch_FindLib(GNode *gn, SearchPath *path)
{
char *libName = str_concat3("lib", gn->name + 2, ".a");
gn->path = Dir_FindFile(libName, path);
free(libName);

#if defined(LIBRARIES)
Var_Set(gn, TARGET, gn->name);
#else
Var_Set(gn, TARGET, GNode_Path(gn));
#endif
}


static bool
RanlibOODate(const GNode *gn MAKE_ATTR_UNUSED)
{
#if defined(RANLIBMAG)
struct ar_hdr *arh;
int tocModTime;

arh = ArchStatMember(gn->path, RANLIBMAG, false);

if (arh == NULL) {

if (DEBUG(ARCH) || DEBUG(MAKE))
debug_printf("no toc...");
return true;
}

tocModTime = (int)strtol(arh->ar_date, NULL, 10);

if (DEBUG(ARCH) || DEBUG(MAKE))
debug_printf("%s modified %s...",
RANLIBMAG, Targ_FmtTime(tocModTime));
return gn->youngestChild == NULL ||
gn->youngestChild->mtime > tocModTime;
#else
return false;
#endif
}































bool
Arch_LibOODate(GNode *gn)
{

if (gn->type & OP_PHONY) {
return true;
} else if (!GNode_IsTarget(gn) && Lst_IsEmpty(&gn->children)) {
return false;
} else if ((!Lst_IsEmpty(&gn->children) && gn->youngestChild == NULL) ||
(gn->mtime > now) ||
(gn->youngestChild != NULL &&
gn->mtime < gn->youngestChild->mtime)) {
return true;
} else {
return RanlibOODate(gn);
}
}


void
Arch_Init(void)
{
Lst_Init(&archives);
}


void
Arch_End(void)
{
#if defined(CLEANUP)
Lst_DoneCall(&archives, ArchFree);
#endif
}

bool
Arch_IsLib(GNode *gn)
{
static const char armag[] = "!<arch>\n";
char buf[sizeof armag - 1];
int fd;

if ((fd = open(gn->path, O_RDONLY)) == -1)
return false;

if (read(fd, buf, sizeof buf) != sizeof buf) {
(void)close(fd);
return false;
}

(void)close(fd);

return memcmp(buf, armag, sizeof buf) == 0;
}
