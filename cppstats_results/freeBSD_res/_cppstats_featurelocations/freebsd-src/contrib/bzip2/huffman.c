




















#include "bzlib_private.h"


#define WEIGHTOF(zz0) ((zz0) & 0xffffff00)
#define DEPTHOF(zz1) ((zz1) & 0x000000ff)
#define MYMAX(zz2,zz3) ((zz2) > (zz3) ? (zz2) : (zz3))

#define ADDWEIGHTS(zw1,zw2) (WEIGHTOF(zw1)+WEIGHTOF(zw2)) | (1 + MYMAX(DEPTHOF(zw1),DEPTHOF(zw2)))



#define UPHEAP(z) { Int32 zz, tmp; zz = z; tmp = heap[zz]; while (weight[tmp] < weight[heap[zz >> 1]]) { heap[zz] = heap[zz >> 1]; zz >>= 1; } heap[zz] = tmp; }










#define DOWNHEAP(z) { Int32 zz, yy, tmp; zz = z; tmp = heap[zz]; while (True) { yy = zz << 1; if (yy > nHeap) break; if (yy < nHeap && weight[heap[yy+1]] < weight[heap[yy]]) yy++; if (weight[tmp] < weight[heap[yy]]) break; heap[zz] = heap[yy]; zz = yy; } heap[zz] = tmp; }


















void BZ2_hbMakeCodeLengths ( UChar *len,
Int32 *freq,
Int32 alphaSize,
Int32 maxLen )
{




Int32 nNodes, nHeap, n1, n2, i, j, k;
Bool tooLong;

Int32 heap [ BZ_MAX_ALPHA_SIZE + 2 ];
Int32 weight [ BZ_MAX_ALPHA_SIZE * 2 ];
Int32 parent [ BZ_MAX_ALPHA_SIZE * 2 ];

for (i = 0; i < alphaSize; i++)
weight[i+1] = (freq[i] == 0 ? 1 : freq[i]) << 8;

while (True) {

nNodes = alphaSize;
nHeap = 0;

heap[0] = 0;
weight[0] = 0;
parent[0] = -2;

for (i = 1; i <= alphaSize; i++) {
parent[i] = -1;
nHeap++;
heap[nHeap] = i;
UPHEAP(nHeap);
}

AssertH( nHeap < (BZ_MAX_ALPHA_SIZE+2), 2001 );

while (nHeap > 1) {
n1 = heap[1]; heap[1] = heap[nHeap]; nHeap--; DOWNHEAP(1);
n2 = heap[1]; heap[1] = heap[nHeap]; nHeap--; DOWNHEAP(1);
nNodes++;
parent[n1] = parent[n2] = nNodes;
weight[nNodes] = ADDWEIGHTS(weight[n1], weight[n2]);
parent[nNodes] = -1;
nHeap++;
heap[nHeap] = nNodes;
UPHEAP(nHeap);
}

AssertH( nNodes < (BZ_MAX_ALPHA_SIZE * 2), 2002 );

tooLong = False;
for (i = 1; i <= alphaSize; i++) {
j = 0;
k = i;
while (parent[k] >= 0) { k = parent[k]; j++; }
len[i-1] = j;
if (j > maxLen) tooLong = True;
}

if (! tooLong) break;


















for (i = 1; i <= alphaSize; i++) {
j = weight[i] >> 8;
j = 1 + (j / 2);
weight[i] = j << 8;
}
}
}



void BZ2_hbAssignCodes ( Int32 *code,
UChar *length,
Int32 minLen,
Int32 maxLen,
Int32 alphaSize )
{
Int32 n, vec, i;

vec = 0;
for (n = minLen; n <= maxLen; n++) {
for (i = 0; i < alphaSize; i++)
if (length[i] == n) { code[i] = vec; vec++; };
vec <<= 1;
}
}



void BZ2_hbCreateDecodeTables ( Int32 *limit,
Int32 *base,
Int32 *perm,
UChar *length,
Int32 minLen,
Int32 maxLen,
Int32 alphaSize )
{
Int32 pp, i, j, vec;

pp = 0;
for (i = minLen; i <= maxLen; i++)
for (j = 0; j < alphaSize; j++)
if (length[j] == i) { perm[pp] = j; pp++; };

for (i = 0; i < BZ_MAX_CODE_LEN; i++) base[i] = 0;
for (i = 0; i < alphaSize; i++) base[length[i]+1]++;

for (i = 1; i < BZ_MAX_CODE_LEN; i++) base[i] += base[i-1];

for (i = 0; i < BZ_MAX_CODE_LEN; i++) limit[i] = 0;
vec = 0;

for (i = minLen; i <= maxLen; i++) {
vec += (base[i+1] - base[i]);
limit[i] = vec-1;
vec <<= 1;
}
for (i = minLen + 1; i <= maxLen; i++)
base[i] = ((limit[i-1] + 1) << 1) - base[i];
}





