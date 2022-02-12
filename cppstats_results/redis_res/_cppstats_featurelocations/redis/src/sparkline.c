































#include "server.h"

#include <math.h>



static char charset[] = "_-`";
static char charset_fill[] = "_o#";
static int charset_len = sizeof(charset)-1;
static int label_margin_top = 1;














struct sequence *createSparklineSequence(void) {
struct sequence *seq = zmalloc(sizeof(*seq));
seq->length = 0;
seq->samples = NULL;
return seq;
}


void sparklineSequenceAddSample(struct sequence *seq, double value, char *label) {
label = (label == NULL || label[0] == '\0') ? NULL : zstrdup(label);
if (seq->length == 0) {
seq->min = seq->max = value;
} else {
if (value < seq->min) seq->min = value;
else if (value > seq->max) seq->max = value;
}
seq->samples = zrealloc(seq->samples,sizeof(struct sample)*(seq->length+1));
seq->samples[seq->length].value = value;
seq->samples[seq->length].label = label;
seq->length++;
if (label) seq->labels++;
}


void freeSparklineSequence(struct sequence *seq) {
int j;

for (j = 0; j < seq->length; j++)
zfree(seq->samples[j].label);
zfree(seq->samples);
zfree(seq);
}








sds sparklineRenderRange(sds output, struct sequence *seq, int rows, int offset, int len, int flags) {
int j;
double relmax = seq->max - seq->min;
int steps = charset_len*rows;
int row = 0;
char *chars = zmalloc(len);
int loop = 1;
int opt_fill = flags & SPARKLINE_FILL;
int opt_log = flags & SPARKLINE_LOG_SCALE;

if (opt_log) {
relmax = log(relmax+1);
} else if (relmax == 0) {
relmax = 1;
}

while(loop) {
loop = 0;
memset(chars,' ',len);
for (j = 0; j < len; j++) {
struct sample *s = &seq->samples[j+offset];
double relval = s->value - seq->min;
int step;

if (opt_log) relval = log(relval+1);
step = (int) (relval*steps)/relmax;
if (step < 0) step = 0;
if (step >= steps) step = steps-1;

if (row < rows) {

int charidx = step-((rows-row-1)*charset_len);
loop = 1;
if (charidx >= 0 && charidx < charset_len) {
chars[j] = opt_fill ? charset_fill[charidx] :
charset[charidx];
} else if(opt_fill && charidx >= charset_len) {
chars[j] = '|';
}
} else {

if (seq->labels && row-rows < label_margin_top) {
loop = 1;
break;
}

if (s->label) {
int label_len = strlen(s->label);
int label_char = row - rows - label_margin_top;

if (label_len > label_char) {
loop = 1;
chars[j] = s->label[label_char];
}
}
}
}
if (loop) {
row++;
output = sdscatlen(output,chars,len);
output = sdscatlen(output,"\n",1);
}
}
zfree(chars);
return output;
}


sds sparklineRender(sds output, struct sequence *seq, int columns, int rows, int flags) {
int j;

for (j = 0; j < seq->length; j += columns) {
int sublen = (seq->length-j) < columns ? (seq->length-j) : columns;

if (j != 0) output = sdscatlen(output,"\n",1);
output = sparklineRenderRange(output, seq, rows, j, sublen, flags);
}
return output;
}

