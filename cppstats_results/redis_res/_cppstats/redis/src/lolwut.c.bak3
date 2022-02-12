


































#include "server.h"
#include "lolwut.h"
#include <math.h>

void lolwut5Command(client *c);
void lolwut6Command(client *c);



void lolwutUnstableCommand(client *c) {
sds rendered = sdsnew("Redis ver. ");
rendered = sdscat(rendered,REDIS_VERSION);
rendered = sdscatlen(rendered,"\n",1);
addReplyVerbatim(c,rendered,sdslen(rendered),"txt");
sdsfree(rendered);
}


void lolwutCommand(client *c) {
char *v = REDIS_VERSION;
char verstr[64];

if (c->argc >= 3 && !strcasecmp(c->argv[1]->ptr,"version")) {
long ver;
if (getLongFromObjectOrReply(c,c->argv[2],&ver,NULL) != C_OK) return;
snprintf(verstr,sizeof(verstr),"%u.0.0",(unsigned int)ver);
v = verstr;




c->argv += 2;
c->argc -= 2;
}

if ((v[0] == '5' && v[1] == '.' && v[2] != '9') ||
(v[0] == '4' && v[1] == '.' && v[2] == '9'))
lolwut5Command(c);
else if ((v[0] == '6' && v[1] == '.' && v[2] != '9') ||
(v[0] == '5' && v[1] == '.' && v[2] == '9'))
lolwut6Command(c);
else
lolwutUnstableCommand(c);


if (v == verstr) {
c->argv -= 2;
c->argc += 2;
}
}







lwCanvas *lwCreateCanvas(int width, int height, int bgcolor) {
lwCanvas *canvas = zmalloc(sizeof(*canvas));
canvas->width = width;
canvas->height = height;
canvas->pixels = zmalloc((size_t)width*height);
memset(canvas->pixels,bgcolor,(size_t)width*height);
return canvas;
}


void lwFreeCanvas(lwCanvas *canvas) {
zfree(canvas->pixels);
zfree(canvas);
}





void lwDrawPixel(lwCanvas *canvas, int x, int y, int color) {
if (x < 0 || x >= canvas->width ||
y < 0 || y >= canvas->height) return;
canvas->pixels[x+y*canvas->width] = color;
}


int lwGetPixel(lwCanvas *canvas, int x, int y) {
if (x < 0 || x >= canvas->width ||
y < 0 || y >= canvas->height) return 0;
return canvas->pixels[x+y*canvas->width];
}


void lwDrawLine(lwCanvas *canvas, int x1, int y1, int x2, int y2, int color) {
int dx = abs(x2-x1);
int dy = abs(y2-y1);
int sx = (x1 < x2) ? 1 : -1;
int sy = (y1 < y2) ? 1 : -1;
int err = dx-dy, e2;

while(1) {
lwDrawPixel(canvas,x1,y1,color);
if (x1 == x2 && y1 == y2) break;
e2 = err*2;
if (e2 > -dy) {
err -= dy;
x1 += sx;
}
if (e2 < dx) {
err += dx;
y1 += sy;
}
}
}




















void lwDrawSquare(lwCanvas *canvas, int x, int y, float size, float angle, int color) {
int px[4], py[4];





size /= 1.4142135623;
size = round(size);


float k = M_PI/4 + angle;
for (int j = 0; j < 4; j++) {
px[j] = round(sin(k) * size + x);
py[j] = round(cos(k) * size + y);
k += M_PI/2;
}


for (int j = 0; j < 4; j++)
lwDrawLine(canvas,px[j],py[j],px[(j+1)%4],py[(j+1)%4],color);
}
