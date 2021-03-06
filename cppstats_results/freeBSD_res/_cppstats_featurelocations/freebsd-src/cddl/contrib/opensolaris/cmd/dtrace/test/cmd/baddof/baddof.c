

























#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/varargs.h>
#include <errno.h>
#include <math.h>
#include <dtrace.h>

void
fatal(char *fmt, ...)
{
va_list ap;

va_start(ap, fmt);

fprintf(stderr, "%s: ", "baddof");
vfprintf(stderr, fmt, ap);

if (fmt[strlen(fmt) - 1] != '\n')
fprintf(stderr, ": %s\n", strerror(errno));

exit(1);
}

#define LEAP_DISTANCE 20

void
corrupt(int fd, unsigned char *buf, int len)
{
static int ttl, valid;
int bit, i;
unsigned char saved;
int val[LEAP_DISTANCE], pos[LEAP_DISTANCE];
int new, rv;

again:
printf("valid DOF #%d\n", valid++);





for (bit = 0; bit < len * 8; bit++) {
saved = buf[bit / 8];
buf[bit / 8] ^= (1 << (bit % 8));

if ((bit % 100) == 0)
printf("%d\n", bit);

if ((rv = ioctl(fd, DTRACEIOC_ENABLE, buf)) == -1) {



buf[bit / 8] = saved;
continue;
}







ttl += rv;

if (ttl < 10000) {
buf[bit / 8] = saved;
continue;
}

printf("enabled %d probes; resetting device.\n", ttl);
close(fd);

new = open("/devices/pseudo/dtrace@0:dtrace", O_RDWR);

if (new == -1)
fatal("couldn't open DTrace pseudo device");

if (new != fd) {
dup2(new, fd);
close(new);
}

ttl = 0;
buf[bit / 8] = saved;
}

for (;;) {





for (i = 0; i < LEAP_DISTANCE; i++) {



bit = lrand48() % (len * 8);

val[i] = buf[bit / 8];
pos[i] = bit / 8;
buf[bit / 8] ^= (1 << (bit % 8));
}




if ((rv = ioctl(fd, DTRACEIOC_ENABLE, buf)) > 0) {



ttl += rv;
goto again;
}






for (i = LEAP_DISTANCE - 1; i >= 0; i--)
buf[pos[i]] = val[i];
}
}

int
main(int argc, char **argv)
{
char *filename = argv[1];
dtrace_hdl_t *dtp;
dtrace_prog_t *pgp;
int err, fd, len;
FILE *fp;
unsigned char *dof, *copy;

if (argc < 2)
fatal("expected D script as argument\n");

if ((fp = fopen(filename, "r")) == NULL)
fatal("couldn't open %s", filename);




if ((dtp = dtrace_open(DTRACE_VERSION, 0, &err)) == NULL) {
fatal("cannot open dtrace library: %s\n",
dtrace_errmsg(NULL, err));
}

pgp = dtrace_program_fcompile(dtp, fp, 0, 0, NULL);
fclose(fp);

if (pgp == NULL) {
fatal("failed to compile script %s: %s\n", filename,
dtrace_errmsg(dtp, dtrace_errno(dtp)));
}

dof = dtrace_dof_create(dtp, pgp, 0);
len = ((dof_hdr_t *)dof)->dofh_loadsz;

if ((copy = malloc(len)) == NULL)
fatal("could not allocate copy of %d bytes", len);

for (;;) {
bcopy(dof, copy, len);



fd = open("/devices/pseudo/dtrace@0:dtrace", O_RDWR);

if (fd == -1)
fatal("couldn't open DTrace pseudo device");

corrupt(fd, copy, len);
close(fd);
}


return (0);
}
