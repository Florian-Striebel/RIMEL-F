

























#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/acl.h>
#include <sys/stat.h>

#include "lib9p.h"
#include "lib9p_impl.h"
#include "genacl.h"
#include "fid.h"
#include "log.h"

typedef int econvertfn(acl_entry_t, struct l9p_ace *);

#if !defined(__APPLE__)
static struct l9p_acl *l9p_new_acl(uint32_t acetype, uint32_t aceasize);
static struct l9p_acl *l9p_growacl(struct l9p_acl *acl, uint32_t aceasize);
static int l9p_count_aces(acl_t sysacl);
static struct l9p_acl *l9p_sysacl_to_acl(int, acl_t, econvertfn *);
#endif
static bool l9p_ingroup(gid_t tid, gid_t gid, gid_t *gids, size_t ngids);
static int l9p_check_aces(int32_t mask, struct l9p_acl *acl, struct stat *st,
uid_t uid, gid_t gid, gid_t *gids, size_t ngids);

void
l9p_acl_free(struct l9p_acl *acl)
{

free(acl);
}




static bool
l9p_ingroup(gid_t tid, gid_t gid, gid_t *gids, size_t ngids)
{
size_t i;

if (tid == gid)
return (true);
for (i = 0; i < ngids; i++)
if (tid == gids[i])
return (true);
return (false);
}





















static int
l9p_check_aces(int32_t mask, struct l9p_acl *acl, struct stat *st,
uid_t uid, gid_t gid, gid_t *gids, size_t ngids)
{
uint32_t i;
struct l9p_ace *ace;
#if defined(ACE_DEBUG)
const char *acetype, *allowdeny;
bool show_tid;
#endif
bool match;
uid_t tid;

if (mask == 0)
return (0);

for (i = 0; mask != 0 && i < acl->acl_nace; i++) {
ace = &acl->acl_aces[i];
switch (ace->ace_type) {
case L9P_ACET_ACCESS_ALLOWED:
case L9P_ACET_ACCESS_DENIED:
break;
default:

continue;
}
#if defined(ACE_DEBUG)
show_tid = false;
#endif
if (ace->ace_flags & L9P_ACEF_OWNER) {
#if defined(ACE_DEBUG)
acetype = "OWNER@";
#endif
match = st->st_uid == uid;
} else if (ace->ace_flags & L9P_ACEF_GROUP) {
#if defined(ACE_DEBUG)
acetype = "GROUP@";
#endif
match = l9p_ingroup(st->st_gid, gid, gids, ngids);
} else if (ace->ace_flags & L9P_ACEF_EVERYONE) {
#if defined(ACE_DEBUG)
acetype = "EVERYONE@";
#endif
match = true;
} else {
if (ace->ace_idsize != sizeof(tid))
continue;
#if defined(ACE_DEBUG)
show_tid = true;
#endif
memcpy(&tid, &ace->ace_idbytes, sizeof(tid));
if (ace->ace_flags & L9P_ACEF_IDENTIFIER_GROUP) {
#if defined(ACE_DEBUG)
acetype = "group";
#endif
match = l9p_ingroup(tid, gid, gids, ngids);
} else {
#if defined(ACE_DEBUG)
acetype = "user";
#endif
match = tid == uid;
}
}






#if defined(ACE_DEBUG)
allowdeny = ace->ace_type == L9P_ACET_ACCESS_DENIED ?
"deny" : "allow";
#endif
if (match && (ace->ace_mask & (uint32_t)mask) != 0) {
#if defined(ACE_DEBUG)
if (show_tid)
L9P_LOG(L9P_DEBUG,
"ACE: %s %s %d: mask 0x%x ace_mask 0x%x",
allowdeny, acetype, (int)tid,
(u_int)mask, (u_int)ace->ace_mask);
else
L9P_LOG(L9P_DEBUG,
"ACE: %s %s: mask 0x%x ace_mask 0x%x",
allowdeny, acetype,
(u_int)mask, (u_int)ace->ace_mask);
#endif
if (ace->ace_type == L9P_ACET_ACCESS_DENIED)
return (-1);
mask &= ~ace->ace_mask;
#if defined(ACE_DEBUG)
L9P_LOG(L9P_DEBUG, "clear 0x%x: now mask=0x%x",
(u_int)ace->ace_mask, (u_int)mask);
#endif
} else {
#if defined(ACE_DEBUG)
if (show_tid)
L9P_LOG(L9P_DEBUG,
"ACE: SKIP %s %s %d: "
"match %d mask 0x%x ace_mask 0x%x",
allowdeny, acetype, (int)tid,
(int)match, (u_int)mask,
(u_int)ace->ace_mask);
else
L9P_LOG(L9P_DEBUG,
"ACE: SKIP %s %s: "
"match %d mask 0x%x ace_mask 0x%x",
allowdeny, acetype,
(int)match, (u_int)mask,
(u_int)ace->ace_mask);
#endif
}
}


#if defined(ACE_DEBUG)
L9P_LOG(L9P_DEBUG, "ACE: end of ACEs, mask now 0x%x: %s",
mask, mask ? "no-definitive-answer" : "ALLOW");
#endif
return (mask == 0 ? 1 : 0);
}



























int l9p_acl_check_access(int32_t opmask, struct l9p_acl_check_args *args)
{
struct l9p_acl *parent, *child;
struct stat *pstat, *cstat;
int32_t pop, cop;
size_t ngids;
uid_t uid;
gid_t gid, *gids;
int panswer, canswer;

assert(opmask != 0);
parent = args->aca_parent;
pstat = args->aca_pstat;
child = args->aca_child;
cstat = args->aca_cstat;
uid = args->aca_uid;
gid = args->aca_gid;
gids = args->aca_groups;
ngids = args->aca_ngroups;

#if defined(ACE_DEBUG)
L9P_LOG(L9P_DEBUG,
"l9p_acl_check_access: opmask=0x%x uid=%ld gid=%ld ngids=%zd",
(u_int)opmask, (long)uid, (long)gid, ngids);
#endif




if (uid == 0 && args->aca_superuser)
return (0);







if ((args->aca_aclmode & L9P_ACM_NFS_ACL) == 0)
parent = child = NULL;

assert(parent == NULL || parent->acl_acetype == L9P_ACLTYPE_NFSv4);
assert(parent == NULL || pstat != NULL);
assert(child == NULL || child->acl_acetype == L9P_ACLTYPE_NFSv4);
assert(child == NULL || cstat != NULL);
assert(pstat != NULL || cstat != NULL);












pop = cop = opmask;
if (parent != NULL || pstat != NULL) {












pop &= ~L9P_ACE_DELETE;
cop &= ~(L9P_ACE_DELETE_CHILD | L9P_ACE_LIST_DIRECTORY |
L9P_ACE_ADD_FILE | L9P_ACE_ADD_SUBDIRECTORY);
} else {








}
panswer = 0;
canswer = 0;
if (parent != NULL)
panswer = l9p_check_aces(pop, parent, pstat,
uid, gid, gids, ngids);
if (child != NULL)
canswer = l9p_check_aces(cop, child, cstat,
uid, gid, gids, ngids);

if (panswer || canswer) {




if (opmask == L9P_ACOP_UNLINK) {









if (panswer > 0 || canswer > 0)
return (0);
if (panswer < 0 && canswer < 0)
return (EPERM);

} else {





if (panswer)
return (panswer < 0 ? EPERM : 0);
return (canswer < 0 ? EPERM : 0);
}
}















if ((args->aca_aclmode & L9P_ACM_ZFS_ACL) &&
opmask == L9P_ACOP_UNLINK && parent != NULL) {
panswer = l9p_check_aces(L9P_ACE_ADD_FILE | L9P_ACE_EXECUTE,
parent, pstat, uid, gid, gids, ngids);
if (panswer)
return (panswer < 0 ? EPERM : 0);
}













if (args->aca_aclmode & L9P_ACM_STAT_MODE) {
struct stat *st;
int rwx, bits;

rwx = l9p_ace_mask_to_rwx(opmask);
if ((st = cstat) == NULL || (opmask & L9P_ACE_DELETE_CHILD))
st = pstat;
if (uid == st->st_uid)
bits = (st->st_mode >> 6) & 7;
else if (l9p_ingroup(st->st_gid, gid, gids, ngids))
bits = (st->st_mode >> 3) & 7;
else
bits = st->st_mode & 7;



if ((rwx & bits) == rwx)
return (0);
}


return (EPERM);
}









int
l9p_ace_mask_to_rwx(int32_t opmask)
{
int rwx = 0;

if (opmask &
(L9P_ACE_READ_DATA | L9P_ACE_READ_NAMED_ATTRS |
L9P_ACE_READ_ATTRIBUTES | L9P_ACE_READ_ACL))
rwx |= 4;
if (opmask &
(L9P_ACE_WRITE_DATA | L9P_ACE_APPEND_DATA |
L9P_ACE_ADD_FILE | L9P_ACE_ADD_SUBDIRECTORY |
L9P_ACE_DELETE | L9P_ACE_DELETE_CHILD |
L9P_ACE_WRITE_NAMED_ATTRS | L9P_ACE_WRITE_ATTRIBUTES |
L9P_ACE_WRITE_ACL))
rwx |= 2;
if (opmask & L9P_ACE_EXECUTE)
rwx |= 1;
return (rwx);
}

#if !defined(__APPLE__)



static struct l9p_acl *
l9p_new_acl(uint32_t acetype, uint32_t aceasize)
{
struct l9p_acl *ret;
size_t asize, size;

asize = aceasize * sizeof(struct l9p_ace);
size = sizeof(struct l9p_acl) + asize;
ret = malloc(size);
if (ret != NULL) {
ret->acl_acetype = acetype;
ret->acl_nace = 0;
ret->acl_aceasize = aceasize;
}
return (ret);
}








static struct l9p_acl *
l9p_growacl(struct l9p_acl *acl, uint32_t aceasize)
{
struct l9p_acl *tmp;
size_t asize, size;

if (acl->acl_aceasize < aceasize) {
asize = aceasize * sizeof(struct l9p_ace);
size = sizeof(struct l9p_acl) + asize;
tmp = realloc(acl, size);
if (tmp == NULL)
free(acl);
acl = tmp;
}
return (acl);
}








static int
l9p_count_aces(acl_t sysacl)
{
acl_entry_t entry;
uint32_t n;
int id;

id = ACL_FIRST_ENTRY;
for (n = 0; acl_get_entry(sysacl, id, &entry) == 1; n++)
id = ACL_NEXT_ENTRY;

return ((int)n);
}





static struct l9p_acl *
l9p_sysacl_to_acl(int acetype, acl_t sysacl, econvertfn *convert)
{
struct l9p_acl *acl;
acl_entry_t entry;
uint32_t n;
int error, id;

acl = l9p_new_acl((uint32_t)acetype, (uint32_t)l9p_count_aces(sysacl));
if (acl == NULL)
return (NULL);
id = ACL_FIRST_ENTRY;
for (n = 0;;) {
if (acl_get_entry(sysacl, id, &entry) != 1)
break;
acl = l9p_growacl(acl, n + 1);
if (acl == NULL)
return (NULL);
error = (*convert)(entry, &acl->acl_aces[n]);
id = ACL_NEXT_ENTRY;
if (error == 0)
n++;
}
acl->acl_nace = n;
return (acl);
}
#endif

#if defined(HAVE_POSIX_ACLS) && 0
struct l9p_acl *
l9p_posix_acl_to_acl(acl_t sysacl)
{
}
#endif

#if defined(HAVE_FREEBSD_ACLS)
static int
l9p_frombsdnfs4(acl_entry_t sysace, struct l9p_ace *ace)
{
acl_tag_t tag;
acl_entry_type_t entry_type;
acl_permset_t absdperm;
acl_flagset_t absdflag;
acl_perm_t bsdperm;
acl_flag_t bsdflag;
uint32_t flags, mask;
int error;
uid_t uid, *aid;

error = acl_get_tag_type(sysace, &tag);
if (error == 0)
error = acl_get_entry_type_np(sysace, &entry_type);
if (error == 0)
error = acl_get_flagset_np(sysace, &absdflag);
if (error == 0)
error = acl_get_permset(sysace, &absdperm);
if (error)
return (error);

flags = 0;
uid = 0;
aid = NULL;


switch (tag) {
case ACL_USER_OBJ:
flags |= L9P_ACEF_OWNER;
break;
case ACL_GROUP_OBJ:
flags |= L9P_ACEF_GROUP;
break;
case ACL_EVERYONE:
flags |= L9P_ACEF_EVERYONE;
break;
case ACL_GROUP:
flags |= L9P_ACEF_IDENTIFIER_GROUP;

case ACL_USER:
aid = acl_get_qualifier(sysace);
if (aid == NULL)
return (ENOMEM);
uid = *(uid_t *)aid;
free(aid);
aid = &uid;
break;
default:
return (EINVAL);
}

switch (entry_type) {

case ACL_ENTRY_TYPE_ALLOW:
ace->ace_type = L9P_ACET_ACCESS_ALLOWED;
break;

case ACL_ENTRY_TYPE_DENY:
ace->ace_type = L9P_ACET_ACCESS_DENIED;
break;

case ACL_ENTRY_TYPE_AUDIT:
ace->ace_type = L9P_ACET_SYSTEM_AUDIT;
break;

case ACL_ENTRY_TYPE_ALARM:
ace->ace_type = L9P_ACET_SYSTEM_ALARM;
break;

default:
return (EINVAL);
}


bsdflag = *absdflag;
if (bsdflag & ACL_ENTRY_FILE_INHERIT)
flags |= L9P_ACEF_FILE_INHERIT_ACE;
if (bsdflag & ACL_ENTRY_DIRECTORY_INHERIT)
flags |= L9P_ACEF_DIRECTORY_INHERIT_ACE;
if (bsdflag & ACL_ENTRY_NO_PROPAGATE_INHERIT)
flags |= L9P_ACEF_NO_PROPAGATE_INHERIT_ACE;
if (bsdflag & ACL_ENTRY_INHERIT_ONLY)
flags |= L9P_ACEF_INHERIT_ONLY_ACE;
if (bsdflag & ACL_ENTRY_SUCCESSFUL_ACCESS)
flags |= L9P_ACEF_SUCCESSFUL_ACCESS_ACE_FLAG;
if (bsdflag & ACL_ENTRY_FAILED_ACCESS)
flags |= L9P_ACEF_FAILED_ACCESS_ACE_FLAG;
ace->ace_flags = flags;









mask = 0;
bsdperm = *absdperm;
if (bsdperm & ACL_READ_DATA)
mask |= L9P_ACE_READ_DATA;
if (bsdperm & ACL_WRITE_DATA)
mask |= L9P_ACE_WRITE_DATA;
if (bsdperm & ACL_APPEND_DATA)
mask |= L9P_ACE_APPEND_DATA;
if (bsdperm & ACL_READ_NAMED_ATTRS)
mask |= L9P_ACE_READ_NAMED_ATTRS;
if (bsdperm & ACL_WRITE_NAMED_ATTRS)
mask |= L9P_ACE_WRITE_NAMED_ATTRS;
if (bsdperm & ACL_EXECUTE)
mask |= L9P_ACE_EXECUTE;
if (bsdperm & ACL_DELETE_CHILD)
mask |= L9P_ACE_DELETE_CHILD;
if (bsdperm & ACL_READ_ATTRIBUTES)
mask |= L9P_ACE_READ_ATTRIBUTES;
if (bsdperm & ACL_WRITE_ATTRIBUTES)
mask |= L9P_ACE_WRITE_ATTRIBUTES;



if (bsdperm & ACL_DELETE)
mask |= L9P_ACE_DELETE;
if (bsdperm & ACL_READ_ACL)
mask |= L9P_ACE_READ_ACL;
if (bsdperm & ACL_WRITE_ACL)
mask |= L9P_ACE_WRITE_ACL;
if (bsdperm & ACL_WRITE_OWNER)
mask |= L9P_ACE_WRITE_OWNER;
if (bsdperm & ACL_SYNCHRONIZE)
mask |= L9P_ACE_SYNCHRONIZE;
ace->ace_mask = mask;


if (aid == NULL)
ace->ace_idsize = 0;
else {
ace->ace_idsize = sizeof(uid);
memcpy(&ace->ace_idbytes[0], aid, sizeof(uid));
}

return (0);
}

struct l9p_acl *
l9p_freebsd_nfsv4acl_to_acl(acl_t sysacl)
{

return (l9p_sysacl_to_acl(L9P_ACLTYPE_NFSv4, sysacl, l9p_frombsdnfs4));
}
#endif

#if defined(HAVE_DARWIN_ACLS) && 0
struct l9p_acl *
l9p_darwin_nfsv4acl_to_acl(acl_t sysacl)
{
}
#endif
