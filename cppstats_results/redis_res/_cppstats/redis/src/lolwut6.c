#include "server.h"
#include "lolwut.h"
static sds renderCanvas(lwCanvas *canvas) {
sds text = sdsempty();
for (int y = 0; y < canvas->height; y++) {
for (int x = 0; x < canvas->width; x++) {
int color = lwGetPixel(canvas,x,y);
char *ce;
switch(color) {
case 0: ce = "0;30;40m"; break;
case 1: ce = "0;90;100m"; break;
case 2: ce = "0;37;47m"; break;
case 3: ce = "0;97;107m"; break;
default: ce = "0;30;40m"; break;
}
text = sdscatprintf(text,"\033[%s \033[0m",ce);
}
if (y != canvas->height-1) text = sdscatlen(text,"\n",1);
}
return text;
}
struct skyscraper {
int xoff;
int width;
int height;
int windows;
int color;
};
void generateSkyscraper(lwCanvas *canvas, struct skyscraper *si) {
int starty = canvas->height-1;
int endy = starty - si->height + 1;
for (int y = starty; y >= endy; y--) {
for (int x = si->xoff; x < si->xoff+si->width; x++) {
if (y == endy && (x <= si->xoff+1 || x >= si->xoff+si->width-2))
continue;
int color = si->color;
if (si->windows &&
x > si->xoff+1 &&
x < si->xoff+si->width-2 &&
y > endy+1 &&
y < starty-1)
{
int relx = x - (si->xoff+1);
int rely = y - (endy+1);
if (relx/2 % 2 && rely % 2) {
do {
color = 1 + rand() % 2;
} while (color == si->color);
if (relx % 2) color = lwGetPixel(canvas,x-1,y);
}
}
lwDrawPixel(canvas,x,y,color);
}
}
}
void generateSkyline(lwCanvas *canvas) {
struct skyscraper si;
for (int color = 2; color >= 1; color--) {
si.color = color;
for (int offset = -10; offset < canvas->width;) {
offset += rand() % 8;
si.xoff = offset;
si.width = 10 + rand()%9;
if (color == 2)
si.height = canvas->height/2 + rand()%canvas->height/2;
else
si.height = canvas->height/2 + rand()%canvas->height/3;
si.windows = 0;
generateSkyscraper(canvas, &si);
if (color == 2)
offset += si.width/2;
else
offset += si.width+1;
}
}
si.color = 0;
for (int offset = -10; offset < canvas->width;) {
offset += rand() % 8;
si.xoff = offset;
si.width = 5 + rand()%14;
if (si.width % 4) si.width += (si.width % 3);
si.height = canvas->height/3 + rand()%canvas->height/2;
si.windows = 1;
generateSkyscraper(canvas, &si);
offset += si.width+5;
}
}
void lolwut6Command(client *c) {
long cols = 80;
long rows = 20;
if (c->argc > 1 &&
getLongFromObjectOrReply(c,c->argv[1],&cols,NULL) != C_OK)
return;
if (c->argc > 2 &&
getLongFromObjectOrReply(c,c->argv[2],&rows,NULL) != C_OK)
return;
if (cols < 1) cols = 1;
if (cols > 1000) cols = 1000;
if (rows < 1) rows = 1;
if (rows > 1000) rows = 1000;
lwCanvas *canvas = lwCreateCanvas(cols,rows,3);
generateSkyline(canvas);
sds rendered = renderCanvas(canvas);
rendered = sdscat(rendered,
"\nDedicated to the 8 bit game developers of past and present.\n"
"Original 8 bit image from Plaguemon by hikikomori. Redis ver. ");
rendered = sdscat(rendered,REDIS_VERSION);
rendered = sdscatlen(rendered,"\n",1);
addReplyVerbatim(c,rendered,sdslen(rendered),"txt");
sdsfree(rendered);
lwFreeCanvas(canvas);
}
