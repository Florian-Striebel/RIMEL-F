















#include "less.h"
#if MSDOS_COMPILER==WIN32C
#include <errno.h>
#include <windows.h>
#endif

#if HAVE_STAT_INO
#include <sys/stat.h>
extern dev_t curr_dev;
extern ino_t curr_ino;
#endif

typedef POSITION BLOCKNUM;

public int ignore_eoi;







struct bufnode {
struct bufnode *next, *prev;
struct bufnode *hnext, *hprev;
};

#define LBUFSIZE 8192
struct buf {
struct bufnode node;
BLOCKNUM block;
unsigned int datasize;
unsigned char data[LBUFSIZE];
};
#define bufnode_buf(bn) ((struct buf *) bn)





#define BUFHASH_SIZE 1024
struct filestate {
struct bufnode buflist;
struct bufnode hashtbl[BUFHASH_SIZE];
int file;
int flags;
POSITION fpos;
int nbufs;
BLOCKNUM block;
unsigned int offset;
POSITION fsize;
};

#define ch_bufhead thisfile->buflist.next
#define ch_buftail thisfile->buflist.prev
#define ch_nbufs thisfile->nbufs
#define ch_block thisfile->block
#define ch_offset thisfile->offset
#define ch_fpos thisfile->fpos
#define ch_fsize thisfile->fsize
#define ch_flags thisfile->flags
#define ch_file thisfile->file

#define END_OF_CHAIN (&thisfile->buflist)
#define END_OF_HCHAIN(h) (&thisfile->hashtbl[h])
#define BUFHASH(blk) ((blk) & (BUFHASH_SIZE-1))




#define FOR_BUFS(bn) for (bn = ch_bufhead; bn != END_OF_CHAIN; bn = bn->next)


#define BUF_RM(bn) (bn)->next->prev = (bn)->prev; (bn)->prev->next = (bn)->next;



#define BUF_INS_HEAD(bn) (bn)->next = ch_bufhead; (bn)->prev = END_OF_CHAIN; ch_bufhead->prev = (bn); ch_bufhead = (bn);





#define BUF_INS_TAIL(bn) (bn)->next = END_OF_CHAIN; (bn)->prev = ch_buftail; ch_buftail->next = (bn); ch_buftail = (bn);








#define FOR_BUFS_IN_CHAIN(h,bn) for (bn = thisfile->hashtbl[h].hnext; bn != END_OF_HCHAIN(h); bn = bn->hnext)



#define BUF_HASH_RM(bn) (bn)->hnext->hprev = (bn)->hprev; (bn)->hprev->hnext = (bn)->hnext;



#define BUF_HASH_INS(bn,h) (bn)->hnext = thisfile->hashtbl[h].hnext; (bn)->hprev = END_OF_HCHAIN(h); thisfile->hashtbl[h].hnext->hprev = (bn); thisfile->hashtbl[h].hnext = (bn);





static struct filestate *thisfile;
static int ch_ungotchar = -1;
static int maxbufs = -1;

extern int autobuf;
extern int sigs;
extern int secure;
extern int screen_trashed;
extern int follow_mode;
extern constant char helpdata[];
extern constant int size_helpdata;
extern IFILE curr_ifile;
#if LOGFILE
extern int logfile;
extern char *namelogfile;
#endif

static int ch_addbuf();





int
ch_get(VOID_PARAM)
{
struct buf *bp;
struct bufnode *bn;
int n;
int slept;
int h;
POSITION pos;
POSITION len;

if (thisfile == NULL)
return (EOI);





if (ch_bufhead != END_OF_CHAIN)
{
bp = bufnode_buf(ch_bufhead);
if (ch_block == bp->block && ch_offset < bp->datasize)
return bp->data[ch_offset];
}

slept = FALSE;




h = BUFHASH(ch_block);
FOR_BUFS_IN_CHAIN(h, bn)
{
bp = bufnode_buf(bn);
if (bp->block == ch_block)
{
if (ch_offset >= bp->datasize)



break;
goto found;
}
}
if (bn == END_OF_HCHAIN(h))
{







if (ch_buftail == END_OF_CHAIN ||
bufnode_buf(ch_buftail)->block != -1)
{






if ((autobuf && !(ch_flags & CH_CANSEEK)) ||
(maxbufs < 0 || ch_nbufs < maxbufs))
if (ch_addbuf())



autobuf = OPT_OFF;
}
bn = ch_buftail;
bp = bufnode_buf(bn);
BUF_HASH_RM(bn);
bp->block = ch_block;
bp->datasize = 0;
BUF_HASH_INS(bn, h);
}

read_more:
pos = (ch_block * LBUFSIZE) + bp->datasize;
if ((len = ch_length()) != NULL_POSITION && pos >= len)



return (EOI);

if (pos != ch_fpos)
{





if (!(ch_flags & CH_CANSEEK))
return ('?');
if (lseek(ch_file, (off_t)pos, SEEK_SET) == BAD_LSEEK)
{
error("seek error", NULL_PARG);
clear_eol();
return (EOI);
}
ch_fpos = pos;
}






if (ch_ungotchar != -1)
{
bp->data[bp->datasize] = ch_ungotchar;
n = 1;
ch_ungotchar = -1;
} else if (ch_flags & CH_HELPFILE)
{
bp->data[bp->datasize] = helpdata[ch_fpos];
n = 1;
} else
{
n = iread(ch_file, &bp->data[bp->datasize],
(unsigned int)(LBUFSIZE - bp->datasize));
}

if (n == READ_INTR)
return (EOI);
if (n < 0)
{
#if MSDOS_COMPILER==WIN32C
if (errno != EPIPE)
#endif
{
error("read error", NULL_PARG);
clear_eol();
}
n = 0;
}

#if LOGFILE



if (!secure && logfile >= 0 && n > 0)
write(logfile, (char *) &bp->data[bp->datasize], n);
#endif

ch_fpos += n;
bp->datasize += n;





if (n == 0)
{
ch_fsize = pos;
if (ignore_eoi)
{




if (!slept)
{
PARG parg;
parg.p_string = wait_message();
ierror("%s", &parg);
}
sleep_ms(2);
slept = TRUE;

#if HAVE_STAT_INO
if (follow_mode == FOLLOW_NAME)
{




struct stat st;
POSITION curr_pos = ch_tell();
int r = stat(get_filename(curr_ifile), &st);
if (r == 0 && (st.st_ino != curr_ino ||
st.st_dev != curr_dev ||
(curr_pos != NULL_POSITION && st.st_size < curr_pos)))
{


screen_trashed = 2;
return (EOI);
}
}
#endif
}
if (sigs)
return (EOI);
}

found:
if (ch_bufhead != bn)
{




BUF_RM(bn);
BUF_INS_HEAD(bn);




BUF_HASH_RM(bn);
BUF_HASH_INS(bn, h);
}

if (ch_offset >= bp->datasize)




goto read_more;

return (bp->data[ch_offset]);
}





public void
ch_ungetchar(c)
int c;
{
if (c != -1 && ch_ungotchar != -1)
error("ch_ungetchar overrun", NULL_PARG);
ch_ungotchar = c;
}

#if LOGFILE




public void
end_logfile(VOID_PARAM)
{
static int tried = FALSE;

if (logfile < 0)
return;
if (!tried && ch_fsize == NULL_POSITION)
{
tried = TRUE;
ierror("Finishing logfile", NULL_PARG);
while (ch_forw_get() != EOI)
if (ABORT_SIGS())
break;
}
close(logfile);
logfile = -1;
free(namelogfile);
namelogfile = NULL;
}






public void
sync_logfile(VOID_PARAM)
{
struct buf *bp;
struct bufnode *bn;
int warned = FALSE;
BLOCKNUM block;
BLOCKNUM nblocks;

nblocks = (ch_fpos + LBUFSIZE - 1) / LBUFSIZE;
for (block = 0; block < nblocks; block++)
{
int wrote = FALSE;
FOR_BUFS(bn)
{
bp = bufnode_buf(bn);
if (bp->block == block)
{
write(logfile, (char *) bp->data, bp->datasize);
wrote = TRUE;
break;
}
}
if (!wrote && !warned)
{
error("Warning: log file is incomplete",
NULL_PARG);
warned = TRUE;
}
}
}

#endif




static int
buffered(block)
BLOCKNUM block;
{
struct buf *bp;
struct bufnode *bn;
int h;

h = BUFHASH(block);
FOR_BUFS_IN_CHAIN(h, bn)
{
bp = bufnode_buf(bn);
if (bp->block == block)
return (TRUE);
}
return (FALSE);
}





public int
ch_seek(pos)
POSITION pos;
{
BLOCKNUM new_block;
POSITION len;

if (thisfile == NULL)
return (0);

len = ch_length();
if (pos < ch_zero() || (len != NULL_POSITION && pos > len))
return (1);

new_block = pos / LBUFSIZE;
if (!(ch_flags & CH_CANSEEK) && pos != ch_fpos && !buffered(new_block))
{
if (ch_fpos > pos)
return (1);
while (ch_fpos < pos)
{
if (ch_forw_get() == EOI)
return (1);
if (ABORT_SIGS())
return (1);
}
return (0);
}



ch_block = new_block;
ch_offset = pos % LBUFSIZE;
return (0);
}




public int
ch_end_seek(VOID_PARAM)
{
POSITION len;

if (thisfile == NULL)
return (0);

if (ch_flags & CH_CANSEEK)
ch_fsize = filesize(ch_file);

len = ch_length();
if (len != NULL_POSITION)
return (ch_seek(len));




while (ch_forw_get() != EOI)
if (ABORT_SIGS())
return (1);
return (0);
}




public int
ch_end_buffer_seek(VOID_PARAM)
{
struct buf *bp;
struct bufnode *bn;
POSITION buf_pos;
POSITION end_pos;

if (thisfile == NULL || (ch_flags & CH_CANSEEK))
return (ch_end_seek());

end_pos = 0;
FOR_BUFS(bn)
{
bp = bufnode_buf(bn);
buf_pos = (bp->block * LBUFSIZE) + bp->datasize;
if (buf_pos > end_pos)
end_pos = buf_pos;
}

return (ch_seek(end_pos));
}






public int
ch_beg_seek(VOID_PARAM)
{
struct bufnode *bn;
struct bufnode *firstbn;




if (ch_seek(ch_zero()) == 0)
return (0);





firstbn = ch_bufhead;
if (firstbn == END_OF_CHAIN)
return (1);
FOR_BUFS(bn)
{
if (bufnode_buf(bn)->block < bufnode_buf(firstbn)->block)
firstbn = bn;
}
ch_block = bufnode_buf(firstbn)->block;
ch_offset = 0;
return (0);
}




public POSITION
ch_length(VOID_PARAM)
{
if (thisfile == NULL)
return (NULL_POSITION);
if (ignore_eoi)
return (NULL_POSITION);
if (ch_flags & CH_HELPFILE)
return (size_helpdata);
if (ch_flags & CH_NODATA)
return (0);
return (ch_fsize);
}




public POSITION
ch_tell(VOID_PARAM)
{
if (thisfile == NULL)
return (NULL_POSITION);
return (ch_block * LBUFSIZE) + ch_offset;
}




public int
ch_forw_get(VOID_PARAM)
{
int c;

if (thisfile == NULL)
return (EOI);
c = ch_get();
if (c == EOI)
return (EOI);
if (ch_offset < LBUFSIZE-1)
ch_offset++;
else
{
ch_block ++;
ch_offset = 0;
}
return (c);
}




public int
ch_back_get(VOID_PARAM)
{
if (thisfile == NULL)
return (EOI);
if (ch_offset > 0)
ch_offset --;
else
{
if (ch_block <= 0)
return (EOI);
if (!(ch_flags & CH_CANSEEK) && !buffered(ch_block-1))
return (EOI);
ch_block--;
ch_offset = LBUFSIZE-1;
}
return (ch_get());
}





public void
ch_setbufspace(bufspace)
int bufspace;
{
if (bufspace < 0)
maxbufs = -1;
else
{
maxbufs = ((bufspace * 1024) + LBUFSIZE-1) / LBUFSIZE;
if (maxbufs < 1)
maxbufs = 1;
}
}




public void
ch_flush(VOID_PARAM)
{
struct bufnode *bn;

if (thisfile == NULL)
return;

if (!(ch_flags & CH_CANSEEK))
{




ch_fsize = NULL_POSITION;
return;
}




FOR_BUFS(bn)
{
bufnode_buf(bn)->block = -1;
}




ch_fsize = filesize(ch_file);




ch_fpos = 0;
ch_block = 0;
ch_offset = 0;

#if 1






if (ch_fsize == 0)
{
ch_fsize = NULL_POSITION;
ch_flags &= ~CH_CANSEEK;
}
#endif

if (lseek(ch_file, (off_t)0, SEEK_SET) == BAD_LSEEK)
{





error("seek error to 0", NULL_PARG);
}
}





static int
ch_addbuf(VOID_PARAM)
{
struct buf *bp;
struct bufnode *bn;





bp = (struct buf *) calloc(1, sizeof(struct buf));
if (bp == NULL)
return (1);
ch_nbufs++;
bp->block = -1;
bn = &bp->node;

BUF_INS_TAIL(bn);
BUF_HASH_INS(bn, 0);
return (0);
}




static void
init_hashtbl(VOID_PARAM)
{
int h;

for (h = 0; h < BUFHASH_SIZE; h++)
{
thisfile->hashtbl[h].hnext = END_OF_HCHAIN(h);
thisfile->hashtbl[h].hprev = END_OF_HCHAIN(h);
}
}




static void
ch_delbufs(VOID_PARAM)
{
struct bufnode *bn;

while (ch_bufhead != END_OF_CHAIN)
{
bn = ch_bufhead;
BUF_RM(bn);
free(bufnode_buf(bn));
}
ch_nbufs = 0;
init_hashtbl();
}




public int
seekable(f)
int f;
{
#if MSDOS_COMPILER
extern int fd0;
if (f == fd0 && !isatty(fd0))
{




return (0);
}
#endif
return (lseek(f, (off_t)1, SEEK_SET) != BAD_LSEEK);
}





public void
ch_set_eof(VOID_PARAM)
{
if (ch_fsize != NULL_POSITION && ch_fsize < ch_fpos)
ch_fsize = ch_fpos;
}





public void
ch_init(f, flags)
int f;
int flags;
{



thisfile = (struct filestate *) get_filestate(curr_ifile);
if (thisfile == NULL)
{



thisfile = (struct filestate *)
ecalloc(1, sizeof(struct filestate));
thisfile->buflist.next = thisfile->buflist.prev = END_OF_CHAIN;
thisfile->nbufs = 0;
thisfile->flags = flags;
thisfile->fpos = 0;
thisfile->block = 0;
thisfile->offset = 0;
thisfile->file = -1;
thisfile->fsize = NULL_POSITION;
init_hashtbl();



if ((flags & CH_CANSEEK) && !seekable(f))
ch_flags &= ~CH_CANSEEK;
set_filestate(curr_ifile, (void *) thisfile);
}
if (thisfile->file == -1)
thisfile->file = f;
ch_flush();
}




public void
ch_close(VOID_PARAM)
{
int keepstate = FALSE;

if (thisfile == NULL)
return;

if ((ch_flags & (CH_CANSEEK|CH_POPENED|CH_HELPFILE)) && !(ch_flags & CH_KEEPOPEN))
{



ch_delbufs();
} else
keepstate = TRUE;
if (!(ch_flags & CH_KEEPOPEN))
{






if (!(ch_flags & (CH_POPENED|CH_HELPFILE)))
close(ch_file);
ch_file = -1;
} else
keepstate = TRUE;
if (!keepstate)
{



free(thisfile);
thisfile = NULL;
set_filestate(curr_ifile, (void *) NULL);
}
}




public int
ch_getflags(VOID_PARAM)
{
if (thisfile == NULL)
return (0);
return (ch_flags);
}

#if 0
public void
ch_dump(struct filestate *fs)
{
struct buf *bp;
struct bufnode *bn;
unsigned char *s;

if (fs == NULL)
{
printf(" --no filestate\n");
return;
}
printf(" file %d, flags %x, fpos %x, fsize %x, blk/off %x/%x\n",
fs->file, fs->flags, fs->fpos,
fs->fsize, fs->block, fs->offset);
printf(" %d bufs:\n", fs->nbufs);
for (bn = fs->next; bn != &fs->buflist; bn = bn->next)
{
bp = bufnode_buf(bn);
printf("%x: blk %x, size %x \"",
bp, bp->block, bp->datasize);
for (s = bp->data; s < bp->data + 30; s++)
if (*s >= ' ' && *s < 0x7F)
printf("%c", *s);
else
printf(".");
printf("\"\n");
}
}
#endif
