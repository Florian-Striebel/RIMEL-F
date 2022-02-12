




















#include "less.h"

extern IFILE curr_ifile;

struct ifile {
struct ifile *h_next;
struct ifile *h_prev;
char *h_filename;
char *h_rfilename;
void *h_filestate;
int h_index;
int h_hold;
char h_opened;
struct scrpos h_scrpos;
void *h_altpipe;
char *h_altfilename;
};





#define int_ifile(h) ((struct ifile *)(h))
#define ext_ifile(h) ((IFILE)(h))




static struct ifile anchor = { &anchor, &anchor, NULL, NULL, NULL, 0, 0, '\0',
{ NULL_POSITION, 0 } };
static int ifiles = 0;

static void
incr_index(p, incr)
struct ifile *p;
int incr;
{
for (; p != &anchor; p = p->h_next)
p->h_index += incr;
}




static void
link_ifile(p, prev)
struct ifile *p;
struct ifile *prev;
{



if (prev == NULL)
prev = &anchor;
p->h_next = prev->h_next;
p->h_prev = prev;
prev->h_next->h_prev = p;
prev->h_next = p;




p->h_index = prev->h_index + 1;
incr_index(p->h_next, 1);
ifiles++;
}




static void
unlink_ifile(p)
struct ifile *p;
{
p->h_next->h_prev = p->h_prev;
p->h_prev->h_next = p->h_next;
incr_index(p->h_next, -1);
ifiles--;
}







static struct ifile *
new_ifile(filename, prev)
char *filename;
struct ifile *prev;
{
struct ifile *p;




p = (struct ifile *) ecalloc(1, sizeof(struct ifile));
p->h_filename = save(filename);
p->h_rfilename = lrealpath(filename);
p->h_scrpos.pos = NULL_POSITION;
p->h_opened = 0;
p->h_hold = 0;
p->h_filestate = NULL;
p->h_altfilename = NULL;
p->h_altpipe = NULL;
link_ifile(p, prev);





mark_check_ifile(ext_ifile(p));
return (p);
}




public void
del_ifile(h)
IFILE h;
{
struct ifile *p;

if (h == NULL_IFILE)
return;




unmark(h);
if (h == curr_ifile)
curr_ifile = getoff_ifile(curr_ifile);
p = int_ifile(h);
unlink_ifile(p);
free(p->h_rfilename);
free(p->h_filename);
free(p);
}




public IFILE
next_ifile(h)
IFILE h;
{
struct ifile *p;

p = (h == NULL_IFILE) ? &anchor : int_ifile(h);
if (p->h_next == &anchor)
return (NULL_IFILE);
return (ext_ifile(p->h_next));
}




public IFILE
prev_ifile(h)
IFILE h;
{
struct ifile *p;

p = (h == NULL_IFILE) ? &anchor : int_ifile(h);
if (p->h_prev == &anchor)
return (NULL_IFILE);
return (ext_ifile(p->h_prev));
}




public IFILE
getoff_ifile(ifile)
IFILE ifile;
{
IFILE newifile;

if ((newifile = prev_ifile(ifile)) != NULL_IFILE)
return (newifile);
if ((newifile = next_ifile(ifile)) != NULL_IFILE)
return (newifile);
return (NULL_IFILE);
}




public int
nifile(VOID_PARAM)
{
return (ifiles);
}




static struct ifile *
find_ifile(filename)
char *filename;
{
struct ifile *p;
char *rfilename = lrealpath(filename);

for (p = anchor.h_next; p != &anchor; p = p->h_next)
{
if (strcmp(rfilename, p->h_rfilename) == 0)
{




if (strlen(filename) < strlen(p->h_filename))
{
free(p->h_filename);
p->h_filename = save(filename);
}
break;
}
}
free(rfilename);
if (p == &anchor)
p = NULL;
return (p);
}






public IFILE
get_ifile(filename, prev)
char *filename;
IFILE prev;
{
struct ifile *p;

if ((p = find_ifile(filename)) == NULL)
p = new_ifile(filename, int_ifile(prev));
return (ext_ifile(p));
}




public char *
get_filename(ifile)
IFILE ifile;
{
if (ifile == NULL)
return (NULL);
return (int_ifile(ifile)->h_filename);
}




public char *
get_real_filename(ifile)
IFILE ifile;
{
if (ifile == NULL)
return (NULL);
return (int_ifile(ifile)->h_rfilename);
}




public int
get_index(ifile)
IFILE ifile;
{
return (int_ifile(ifile)->h_index);
}




public void
store_pos(ifile, scrpos)
IFILE ifile;
struct scrpos *scrpos;
{
int_ifile(ifile)->h_scrpos = *scrpos;
}





public void
get_pos(ifile, scrpos)
IFILE ifile;
struct scrpos *scrpos;
{
*scrpos = int_ifile(ifile)->h_scrpos;
}




public void
set_open(ifile)
IFILE ifile;
{
int_ifile(ifile)->h_opened = 1;
}




public int
opened(ifile)
IFILE ifile;
{
return (int_ifile(ifile)->h_opened);
}

public void
hold_ifile(ifile, incr)
IFILE ifile;
int incr;
{
int_ifile(ifile)->h_hold += incr;
}

public int
held_ifile(ifile)
IFILE ifile;
{
return (int_ifile(ifile)->h_hold);
}

public void *
get_filestate(ifile)
IFILE ifile;
{
return (int_ifile(ifile)->h_filestate);
}

public void
set_filestate(ifile, filestate)
IFILE ifile;
void *filestate;
{
int_ifile(ifile)->h_filestate = filestate;
}

public void
set_altpipe(ifile, p)
IFILE ifile;
void *p;
{
int_ifile(ifile)->h_altpipe = p;
}

public void *
get_altpipe(ifile)
IFILE ifile;
{
return (int_ifile(ifile)->h_altpipe);
}

public void
set_altfilename(ifile, altfilename)
IFILE ifile;
char *altfilename;
{
struct ifile *p = int_ifile(ifile);
if (p->h_altfilename != NULL && p->h_altfilename != altfilename)
free(p->h_altfilename);
p->h_altfilename = altfilename;
}

public char *
get_altfilename(ifile)
IFILE ifile;
{
return (int_ifile(ifile)->h_altfilename);
}

#if 0
public void
if_dump(VOID_PARAM)
{
struct ifile *p;

for (p = anchor.h_next; p != &anchor; p = p->h_next)
{
printf("%x: %d. <%s> pos %d,%x\n",
p, p->h_index, p->h_filename,
p->h_scrpos.ln, p->h_scrpos.pos);
ch_dump(p->h_filestate);
}
}
#endif
