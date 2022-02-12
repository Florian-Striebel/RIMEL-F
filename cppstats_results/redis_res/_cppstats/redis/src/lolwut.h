typedef struct lwCanvas {
int width;
int height;
char *pixels;
} lwCanvas;
lwCanvas *lwCreateCanvas(int width, int height, int bgcolor);
void lwFreeCanvas(lwCanvas *canvas);
void lwDrawPixel(lwCanvas *canvas, int x, int y, int color);
int lwGetPixel(lwCanvas *canvas, int x, int y);
void lwDrawLine(lwCanvas *canvas, int x1, int y1, int x2, int y2, int color);
void lwDrawSquare(lwCanvas *canvas, int x, int y, float size, float angle, int color);
