


































#include "server.h"
#include "lolwut.h"
#include <math.h>














#include <stdio.h>
void lwTranslatePixelsGroup(int byte, char *output) {
int code = 0x2800 + byte;



output[0] = 0xE0 | (code >> 12);
output[1] = 0x80 | ((code >> 6) & 0x3F);
output[2] = 0x80 | (code & 0x3F);
}








lwCanvas *lwDrawSchotter(int console_cols, int squares_per_row, int squares_per_col) {

int canvas_width = console_cols*2;
int padding = canvas_width > 4 ? 2 : 0;
float square_side = (float)(canvas_width-padding*2) / squares_per_row;
int canvas_height = square_side * squares_per_col + padding*2;
lwCanvas *canvas = lwCreateCanvas(canvas_width, canvas_height, 0);

for (int y = 0; y < squares_per_col; y++) {
for (int x = 0; x < squares_per_row; x++) {
int sx = x * square_side + square_side/2 + padding;
int sy = y * square_side + square_side/2 + padding;


float angle = 0;
if (y > 1) {
float r1 = (float)rand() / (float) RAND_MAX / squares_per_col * y;
float r2 = (float)rand() / (float) RAND_MAX / squares_per_col * y;
float r3 = (float)rand() / (float) RAND_MAX / squares_per_col * y;
if (rand() % 2) r1 = -r1;
if (rand() % 2) r2 = -r2;
if (rand() % 2) r3 = -r3;
angle = r1;
sx += r2*square_side/3;
sy += r3*square_side/3;
}
lwDrawSquare(canvas,sx,sy,square_side,angle,1);
}
}

return canvas;
}






static sds renderCanvas(lwCanvas *canvas) {
sds text = sdsempty();
for (int y = 0; y < canvas->height; y += 4) {
for (int x = 0; x < canvas->width; x += 2) {


int byte = 0;
if (lwGetPixel(canvas,x,y)) byte |= (1<<0);
if (lwGetPixel(canvas,x,y+1)) byte |= (1<<1);
if (lwGetPixel(canvas,x,y+2)) byte |= (1<<2);
if (lwGetPixel(canvas,x+1,y)) byte |= (1<<3);
if (lwGetPixel(canvas,x+1,y+1)) byte |= (1<<4);
if (lwGetPixel(canvas,x+1,y+2)) byte |= (1<<5);
if (lwGetPixel(canvas,x,y+3)) byte |= (1<<6);
if (lwGetPixel(canvas,x+1,y+3)) byte |= (1<<7);
char unicode[3];
lwTranslatePixelsGroup(byte,unicode);
text = sdscatlen(text,unicode,3);
}
if (y != canvas->height-1) text = sdscatlen(text,"\n",1);
}
return text;
}








void lolwut5Command(client *c) {
long cols = 66;
long squares_per_row = 8;
long squares_per_col = 12;


if (c->argc > 1 &&
getLongFromObjectOrReply(c,c->argv[1],&cols,NULL) != C_OK)
return;

if (c->argc > 2 &&
getLongFromObjectOrReply(c,c->argv[2],&squares_per_row,NULL) != C_OK)
return;

if (c->argc > 3 &&
getLongFromObjectOrReply(c,c->argv[3],&squares_per_col,NULL) != C_OK)
return;



if (cols < 1) cols = 1;
if (cols > 1000) cols = 1000;
if (squares_per_row < 1) squares_per_row = 1;
if (squares_per_row > 200) squares_per_row = 200;
if (squares_per_col < 1) squares_per_col = 1;
if (squares_per_col > 200) squares_per_col = 200;


lwCanvas *canvas = lwDrawSchotter(cols,squares_per_row,squares_per_col);
sds rendered = renderCanvas(canvas);
rendered = sdscat(rendered,
"\nGeorg Nees - schotter, plotter on paper, 1968. Redis ver. ");
rendered = sdscat(rendered,REDIS_VERSION);
rendered = sdscatlen(rendered,"\n",1);
addReplyVerbatim(c,rendered,sdslen(rendered),"txt");
sdsfree(rendered);
lwFreeCanvas(canvas);
}
