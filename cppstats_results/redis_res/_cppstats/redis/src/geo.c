#include "geo.h"
#include "geohash_helper.h"
#include "debugmacro.h"
#include "pqsort.h"
unsigned char *zzlFirstInRange(unsigned char *zl, zrangespec *range);
int zslValueLteMax(double value, zrangespec *spec);
geoArray *geoArrayCreate(void) {
geoArray *ga = zmalloc(sizeof(*ga));
ga->array = NULL;
ga->buckets = 0;
ga->used = 0;
return ga;
}
geoPoint *geoArrayAppend(geoArray *ga) {
if (ga->used == ga->buckets) {
ga->buckets = (ga->buckets == 0) ? 8 : ga->buckets*2;
ga->array = zrealloc(ga->array,sizeof(geoPoint)*ga->buckets);
}
geoPoint *gp = ga->array+ga->used;
ga->used++;
return gp;
}
void geoArrayFree(geoArray *ga) {
size_t i;
for (i = 0; i < ga->used; i++) sdsfree(ga->array[i].member);
zfree(ga->array);
zfree(ga);
}
int decodeGeohash(double bits, double *xy) {
GeoHashBits hash = { .bits = (uint64_t)bits, .step = GEO_STEP_MAX };
return geohashDecodeToLongLatWGS84(hash, xy);
}
int extractLongLatOrReply(client *c, robj **argv, double *xy) {
int i;
for (i = 0; i < 2; i++) {
if (getDoubleFromObjectOrReply(c, argv[i], xy + i, NULL) !=
C_OK) {
return C_ERR;
}
}
if (xy[0] < GEO_LONG_MIN || xy[0] > GEO_LONG_MAX ||
xy[1] < GEO_LAT_MIN || xy[1] > GEO_LAT_MAX) {
addReplyErrorFormat(c,
"-ERR invalid longitude,latitude pair %f,%f\r\n",xy[0],xy[1]);
return C_ERR;
}
return C_OK;
}
int longLatFromMember(robj *zobj, robj *member, double *xy) {
double score = 0;
if (zsetScore(zobj, member->ptr, &score) == C_ERR) return C_ERR;
if (!decodeGeohash(score, xy)) return C_ERR;
return C_OK;
}
double extractUnitOrReply(client *c, robj *unit) {
char *u = unit->ptr;
if (!strcasecmp(u, "m")) {
return 1;
} else if (!strcasecmp(u, "km")) {
return 1000;
} else if (!strcasecmp(u, "ft")) {
return 0.3048;
} else if (!strcasecmp(u, "mi")) {
return 1609.34;
} else {
addReplyError(c,
"unsupported unit provided. please use M, KM, FT, MI");
return -1;
}
}
int extractDistanceOrReply(client *c, robj **argv,
double *conversion, double *radius) {
double distance;
if (getDoubleFromObjectOrReply(c, argv[0], &distance,
"need numeric radius") != C_OK) {
return C_ERR;
}
if (distance < 0) {
addReplyError(c,"radius cannot be negative");
return C_ERR;
}
if (radius) *radius = distance;
double to_meters = extractUnitOrReply(c,argv[1]);
if (to_meters < 0) {
return C_ERR;
}
if (conversion) *conversion = to_meters;
return C_OK;
}
int extractBoxOrReply(client *c, robj **argv, double *conversion,
double *width, double *height) {
double h, w;
if ((getDoubleFromObjectOrReply(c, argv[0], &w, "need numeric width") != C_OK) ||
(getDoubleFromObjectOrReply(c, argv[1], &h, "need numeric height") != C_OK)) {
return C_ERR;
}
if (h < 0 || w < 0) {
addReplyError(c, "height or width cannot be negative");
return C_ERR;
}
if (height) *height = h;
if (width) *width = w;
double to_meters = extractUnitOrReply(c,argv[2]);
if (to_meters < 0) {
return C_ERR;
}
if (conversion) *conversion = to_meters;
return C_OK;
}
void addReplyDoubleDistance(client *c, double d) {
char dbuf[128];
int dlen = snprintf(dbuf, sizeof(dbuf), "%.4f", d);
addReplyBulkCBuffer(c, dbuf, dlen);
}
int geoAppendIfWithinShape(geoArray *ga, GeoShape *shape, double score, sds member) {
double distance = 0, xy[2];
if (!decodeGeohash(score,xy)) return C_ERR;
if (shape->type == CIRCULAR_TYPE) {
if (!geohashGetDistanceIfInRadiusWGS84(shape->xy[0], shape->xy[1], xy[0], xy[1],
shape->t.radius*shape->conversion, &distance)) return C_ERR;
} else if (shape->type == RECTANGLE_TYPE) {
if (!geohashGetDistanceIfInRectangle(shape->t.r.width * shape->conversion,
shape->t.r.height * shape->conversion,
shape->xy[0], shape->xy[1], xy[0], xy[1], &distance))
return C_ERR;
}
geoPoint *gp = geoArrayAppend(ga);
gp->longitude = xy[0];
gp->latitude = xy[1];
gp->dist = distance;
gp->member = member;
gp->score = score;
return C_OK;
}
int geoGetPointsInRange(robj *zobj, double min, double max, GeoShape *shape, geoArray *ga, unsigned long limit) {
zrangespec range = { .min = min, .max = max, .minex = 0, .maxex = 1 };
size_t origincount = ga->used;
sds member;
if (zobj->encoding == OBJ_ENCODING_LISTPACK) {
unsigned char *zl = zobj->ptr;
unsigned char *eptr, *sptr;
unsigned char *vstr = NULL;
unsigned int vlen = 0;
long long vlong = 0;
double score = 0;
if ((eptr = zzlFirstInRange(zl, &range)) == NULL) {
return 0;
}
sptr = lpNext(zl, eptr);
while (eptr) {
score = zzlGetScore(sptr);
if (!zslValueLteMax(score, &range))
break;
vstr = lpGetValue(eptr, &vlen, &vlong);
member = (vstr == NULL) ? sdsfromlonglong(vlong) :
sdsnewlen(vstr,vlen);
if (geoAppendIfWithinShape(ga,shape,score,member)
== C_ERR) sdsfree(member);
if (ga->used && limit && ga->used >= limit) break;
zzlNext(zl, &eptr, &sptr);
}
} else if (zobj->encoding == OBJ_ENCODING_SKIPLIST) {
zset *zs = zobj->ptr;
zskiplist *zsl = zs->zsl;
zskiplistNode *ln;
if ((ln = zslFirstInRange(zsl, &range)) == NULL) {
return 0;
}
while (ln) {
sds ele = ln->ele;
if (!zslValueLteMax(ln->score, &range))
break;
ele = sdsdup(ele);
if (geoAppendIfWithinShape(ga,shape,ln->score,ele)
== C_ERR) sdsfree(ele);
if (ga->used && limit && ga->used >= limit) break;
ln = ln->level[0].forward;
}
}
return ga->used - origincount;
}
void scoresOfGeoHashBox(GeoHashBits hash, GeoHashFix52Bits *min, GeoHashFix52Bits *max) {
*min = geohashAlign52Bits(hash);
hash.bits++;
*max = geohashAlign52Bits(hash);
}
int membersOfGeoHashBox(robj *zobj, GeoHashBits hash, geoArray *ga, GeoShape *shape, unsigned long limit) {
GeoHashFix52Bits min, max;
scoresOfGeoHashBox(hash,&min,&max);
return geoGetPointsInRange(zobj, min, max, shape, ga, limit);
}
int membersOfAllNeighbors(robj *zobj, const GeoHashRadius *n, GeoShape *shape, geoArray *ga, unsigned long limit) {
GeoHashBits neighbors[9];
unsigned int i, count = 0, last_processed = 0;
int debugmsg = 0;
neighbors[0] = n->hash;
neighbors[1] = n->neighbors.north;
neighbors[2] = n->neighbors.south;
neighbors[3] = n->neighbors.east;
neighbors[4] = n->neighbors.west;
neighbors[5] = n->neighbors.north_east;
neighbors[6] = n->neighbors.north_west;
neighbors[7] = n->neighbors.south_east;
neighbors[8] = n->neighbors.south_west;
for (i = 0; i < sizeof(neighbors) / sizeof(*neighbors); i++) {
if (HASHISZERO(neighbors[i])) {
if (debugmsg) D("neighbors[%d] is zero",i);
continue;
}
if (debugmsg) {
GeoHashRange long_range, lat_range;
geohashGetCoordRange(&long_range,&lat_range);
GeoHashArea myarea = {{0}};
geohashDecode(long_range, lat_range, neighbors[i], &myarea);
D("neighbors[%d]:\n",i);
D("area.longitude.min: %f\n", myarea.longitude.min);
D("area.longitude.max: %f\n", myarea.longitude.max);
D("area.latitude.min: %f\n", myarea.latitude.min);
D("area.latitude.max: %f\n", myarea.latitude.max);
D("\n");
}
if (last_processed &&
neighbors[i].bits == neighbors[last_processed].bits &&
neighbors[i].step == neighbors[last_processed].step)
{
if (debugmsg)
D("Skipping processing of %d, same as previous\n",i);
continue;
}
if (ga->used && limit && ga->used >= limit) break;
count += membersOfGeoHashBox(zobj, neighbors[i], ga, shape, limit);
last_processed = i;
}
return count;
}
static int sort_gp_asc(const void *a, const void *b) {
const struct geoPoint *gpa = a, *gpb = b;
if (gpa->dist > gpb->dist)
return 1;
else if (gpa->dist == gpb->dist)
return 0;
else
return -1;
}
static int sort_gp_desc(const void *a, const void *b) {
return -sort_gp_asc(a, b);
}
void geoaddCommand(client *c) {
int xx = 0, nx = 0, longidx = 2;
int i;
while (longidx < c->argc) {
char *opt = c->argv[longidx]->ptr;
if (!strcasecmp(opt,"nx")) nx = 1;
else if (!strcasecmp(opt,"xx")) xx = 1;
else if (!strcasecmp(opt,"ch")) { }
else break;
longidx++;
}
if ((c->argc - longidx) % 3 || (xx && nx)) {
addReplyErrorObject(c,shared.syntaxerr);
return;
}
int elements = (c->argc - longidx) / 3;
int argc = longidx+elements*2;
robj **argv = zcalloc(argc*sizeof(robj*));
argv[0] = createRawStringObject("zadd",4);
for (i = 1; i < longidx; i++) {
argv[i] = c->argv[i];
incrRefCount(argv[i]);
}
for (i = 0; i < elements; i++) {
double xy[2];
if (extractLongLatOrReply(c, (c->argv+longidx)+(i*3),xy) == C_ERR) {
for (i = 0; i < argc; i++)
if (argv[i]) decrRefCount(argv[i]);
zfree(argv);
return;
}
GeoHashBits hash;
geohashEncodeWGS84(xy[0], xy[1], GEO_STEP_MAX, &hash);
GeoHashFix52Bits bits = geohashAlign52Bits(hash);
robj *score = createObject(OBJ_STRING, sdsfromlonglong(bits));
robj *val = c->argv[longidx + i * 3 + 2];
argv[longidx+i*2] = score;
argv[longidx+1+i*2] = val;
incrRefCount(val);
}
replaceClientCommandVector(c,argc,argv);
zaddCommand(c);
}
#define SORT_NONE 0
#define SORT_ASC 1
#define SORT_DESC 2
#define RADIUS_COORDS (1<<0)
#define RADIUS_MEMBER (1<<1)
#define RADIUS_NOSTORE (1<<2)
#define GEOSEARCH (1<<3)
#define GEOSEARCHSTORE (1<<4)
void georadiusGeneric(client *c, int srcKeyIndex, int flags) {
robj *storekey = NULL;
int storedist = 0;
robj *zobj = lookupKeyRead(c->db, c->argv[srcKeyIndex]);
if (checkType(c, zobj, OBJ_ZSET)) return;
int base_args;
GeoShape shape = {0};
if (flags & RADIUS_COORDS) {
base_args = 6;
shape.type = CIRCULAR_TYPE;
if (extractLongLatOrReply(c, c->argv + 2, shape.xy) == C_ERR) return;
if (extractDistanceOrReply(c, c->argv+base_args-2, &shape.conversion, &shape.t.radius) != C_OK) return;
} else if ((flags & RADIUS_MEMBER) && !zobj) {
base_args = 5;
} else if (flags & RADIUS_MEMBER) {
base_args = 5;
shape.type = CIRCULAR_TYPE;
robj *member = c->argv[2];
if (longLatFromMember(zobj, member, shape.xy) == C_ERR) {
addReplyError(c, "could not decode requested zset member");
return;
}
if (extractDistanceOrReply(c, c->argv+base_args-2, &shape.conversion, &shape.t.radius) != C_OK) return;
} else if (flags & GEOSEARCH) {
base_args = 2;
if (flags & GEOSEARCHSTORE) {
base_args = 3;
storekey = c->argv[1];
}
} else {
addReplyError(c, "Unknown georadius search type");
return;
}
int withdist = 0, withhash = 0, withcoords = 0;
int frommember = 0, fromloc = 0, byradius = 0, bybox = 0;
int sort = SORT_NONE;
int any = 0;
long long count = 0;
if (c->argc > base_args) {
int remaining = c->argc - base_args;
for (int i = 0; i < remaining; i++) {
char *arg = c->argv[base_args + i]->ptr;
if (!strcasecmp(arg, "withdist")) {
withdist = 1;
} else if (!strcasecmp(arg, "withhash")) {
withhash = 1;
} else if (!strcasecmp(arg, "withcoord")) {
withcoords = 1;
} else if (!strcasecmp(arg, "any")) {
any = 1;
} else if (!strcasecmp(arg, "asc")) {
sort = SORT_ASC;
} else if (!strcasecmp(arg, "desc")) {
sort = SORT_DESC;
} else if (!strcasecmp(arg, "count") && (i+1) < remaining) {
if (getLongLongFromObjectOrReply(c, c->argv[base_args+i+1],
&count, NULL) != C_OK) return;
if (count <= 0) {
addReplyError(c,"COUNT must be > 0");
return;
}
i++;
} else if (!strcasecmp(arg, "store") &&
(i+1) < remaining &&
!(flags & RADIUS_NOSTORE) &&
!(flags & GEOSEARCH))
{
storekey = c->argv[base_args+i+1];
storedist = 0;
i++;
} else if (!strcasecmp(arg, "storedist") &&
(i+1) < remaining &&
!(flags & RADIUS_NOSTORE) &&
!(flags & GEOSEARCH))
{
storekey = c->argv[base_args+i+1];
storedist = 1;
i++;
} else if (!strcasecmp(arg, "storedist") &&
(flags & GEOSEARCH) &&
(flags & GEOSEARCHSTORE))
{
storedist = 1;
} else if (!strcasecmp(arg, "frommember") &&
(i+1) < remaining &&
flags & GEOSEARCH &&
!fromloc)
{
if (zobj == NULL) {
frommember = 1;
i++;
continue;
}
if (longLatFromMember(zobj, c->argv[base_args+i+1], shape.xy) == C_ERR) {
addReplyError(c, "could not decode requested zset member");
return;
}
frommember = 1;
i++;
} else if (!strcasecmp(arg, "fromlonlat") &&
(i+2) < remaining &&
flags & GEOSEARCH &&
!frommember)
{
if (extractLongLatOrReply(c, c->argv+base_args+i+1, shape.xy) == C_ERR) return;
fromloc = 1;
i += 2;
} else if (!strcasecmp(arg, "byradius") &&
(i+2) < remaining &&
flags & GEOSEARCH &&
!bybox)
{
if (extractDistanceOrReply(c, c->argv+base_args+i+1, &shape.conversion, &shape.t.radius) != C_OK)
return;
shape.type = CIRCULAR_TYPE;
byradius = 1;
i += 2;
} else if (!strcasecmp(arg, "bybox") &&
(i+3) < remaining &&
flags & GEOSEARCH &&
!byradius)
{
if (extractBoxOrReply(c, c->argv+base_args+i+1, &shape.conversion, &shape.t.r.width,
&shape.t.r.height) != C_OK) return;
shape.type = RECTANGLE_TYPE;
bybox = 1;
i += 3;
} else {
addReplyErrorObject(c,shared.syntaxerr);
return;
}
}
}
if (storekey && (withdist || withhash || withcoords)) {
addReplyErrorFormat(c,
"%s is not compatible with WITHDIST, WITHHASH and WITHCOORD options",
flags & GEOSEARCHSTORE? "GEOSEARCHSTORE": "STORE option in GEORADIUS");
return;
}
if ((flags & GEOSEARCH) && !(frommember || fromloc)) {
addReplyErrorFormat(c,
"exactly one of FROMMEMBER or FROMLONLAT can be specified for %s",
(char *)c->argv[0]->ptr);
return;
}
if ((flags & GEOSEARCH) && !(byradius || bybox)) {
addReplyErrorFormat(c,
"exactly one of BYRADIUS and BYBOX can be specified for %s",
(char *)c->argv[0]->ptr);
return;
}
if (any && !count) {
addReplyErrorFormat(c, "the ANY argument requires COUNT argument");
return;
}
if (zobj == NULL) {
if (storekey) {
if (dbDelete(c->db, storekey)) {
signalModifiedKey(c, c->db, storekey);
notifyKeyspaceEvent(NOTIFY_GENERIC, "del", storekey, c->db->id);
server.dirty++;
}
addReply(c, shared.czero);
} else {
addReply(c, shared.emptyarray);
}
return;
}
if (count != 0 && sort == SORT_NONE && !any) sort = SORT_ASC;
GeoHashRadius georadius = geohashCalculateAreasByShapeWGS84(&shape);
geoArray *ga = geoArrayCreate();
membersOfAllNeighbors(zobj, &georadius, &shape, ga, any ? count : 0);
if (ga->used == 0 && storekey == NULL) {
addReply(c,shared.emptyarray);
geoArrayFree(ga);
return;
}
long result_length = ga->used;
long returned_items = (count == 0 || result_length < count) ?
result_length : count;
long option_length = 0;
if (sort != SORT_NONE) {
int (*sort_gp_callback)(const void *a, const void *b) = NULL;
if (sort == SORT_ASC) {
sort_gp_callback = sort_gp_asc;
} else if (sort == SORT_DESC) {
sort_gp_callback = sort_gp_desc;
}
if (returned_items == result_length) {
qsort(ga->array, result_length, sizeof(geoPoint), sort_gp_callback);
} else {
pqsort(ga->array, result_length, sizeof(geoPoint), sort_gp_callback,
0, (returned_items - 1));
}
}
if (storekey == NULL) {
if (withdist)
option_length++;
if (withcoords)
option_length++;
if (withhash)
option_length++;
addReplyArrayLen(c, returned_items);
int i;
for (i = 0; i < returned_items; i++) {
geoPoint *gp = ga->array+i;
gp->dist /= shape.conversion;
if (option_length)
addReplyArrayLen(c, option_length + 1);
addReplyBulkSds(c,gp->member);
gp->member = NULL;
if (withdist)
addReplyDoubleDistance(c, gp->dist);
if (withhash)
addReplyLongLong(c, gp->score);
if (withcoords) {
addReplyArrayLen(c, 2);
addReplyHumanLongDouble(c, gp->longitude);
addReplyHumanLongDouble(c, gp->latitude);
}
}
} else {
robj *zobj;
zset *zs;
int i;
size_t maxelelen = 0, totelelen = 0;
if (returned_items) {
zobj = createZsetObject();
zs = zobj->ptr;
}
for (i = 0; i < returned_items; i++) {
zskiplistNode *znode;
geoPoint *gp = ga->array+i;
gp->dist /= shape.conversion;
double score = storedist ? gp->dist : gp->score;
size_t elelen = sdslen(gp->member);
if (maxelelen < elelen) maxelelen = elelen;
totelelen += elelen;
znode = zslInsert(zs->zsl,score,gp->member);
serverAssert(dictAdd(zs->dict,gp->member,&znode->score) == DICT_OK);
gp->member = NULL;
}
if (returned_items) {
zsetConvertToListpackIfNeeded(zobj,maxelelen,totelelen);
setKey(c,c->db,storekey,zobj,0);
decrRefCount(zobj);
notifyKeyspaceEvent(NOTIFY_ZSET,flags & GEOSEARCH ? "geosearchstore" : "georadiusstore",storekey,
c->db->id);
server.dirty += returned_items;
} else if (dbDelete(c->db,storekey)) {
signalModifiedKey(c,c->db,storekey);
notifyKeyspaceEvent(NOTIFY_GENERIC,"del",storekey,c->db->id);
server.dirty++;
}
addReplyLongLong(c, returned_items);
}
geoArrayFree(ga);
}
void georadiusCommand(client *c) {
georadiusGeneric(c, 1, RADIUS_COORDS);
}
void georadiusbymemberCommand(client *c) {
georadiusGeneric(c, 1, RADIUS_MEMBER);
}
void georadiusroCommand(client *c) {
georadiusGeneric(c, 1, RADIUS_COORDS|RADIUS_NOSTORE);
}
void georadiusbymemberroCommand(client *c) {
georadiusGeneric(c, 1, RADIUS_MEMBER|RADIUS_NOSTORE);
}
void geosearchCommand(client *c) {
georadiusGeneric(c, 1, GEOSEARCH);
}
void geosearchstoreCommand(client *c) {
georadiusGeneric(c, 2, GEOSEARCH|GEOSEARCHSTORE);
}
void geohashCommand(client *c) {
char *geoalphabet= "0123456789bcdefghjkmnpqrstuvwxyz";
int j;
robj *zobj = lookupKeyRead(c->db, c->argv[1]);
if (checkType(c, zobj, OBJ_ZSET)) return;
addReplyArrayLen(c,c->argc-2);
for (j = 2; j < c->argc; j++) {
double score;
if (!zobj || zsetScore(zobj, c->argv[j]->ptr, &score) == C_ERR) {
addReplyNull(c);
} else {
double xy[2];
if (!decodeGeohash(score,xy)) {
addReplyNull(c);
continue;
}
GeoHashRange r[2];
GeoHashBits hash;
r[0].min = -180;
r[0].max = 180;
r[1].min = -90;
r[1].max = 90;
geohashEncode(&r[0],&r[1],xy[0],xy[1],26,&hash);
char buf[12];
int i;
for (i = 0; i < 11; i++) {
int idx;
if (i == 10) {
idx = 0;
} else {
idx = (hash.bits >> (52-((i+1)*5))) & 0x1f;
}
buf[i] = geoalphabet[idx];
}
buf[11] = '\0';
addReplyBulkCBuffer(c,buf,11);
}
}
}
void geoposCommand(client *c) {
int j;
robj *zobj = lookupKeyRead(c->db, c->argv[1]);
if (checkType(c, zobj, OBJ_ZSET)) return;
addReplyArrayLen(c,c->argc-2);
for (j = 2; j < c->argc; j++) {
double score;
if (!zobj || zsetScore(zobj, c->argv[j]->ptr, &score) == C_ERR) {
addReplyNullArray(c);
} else {
double xy[2];
if (!decodeGeohash(score,xy)) {
addReplyNullArray(c);
continue;
}
addReplyArrayLen(c,2);
addReplyHumanLongDouble(c,xy[0]);
addReplyHumanLongDouble(c,xy[1]);
}
}
}
void geodistCommand(client *c) {
double to_meter = 1;
if (c->argc == 5) {
to_meter = extractUnitOrReply(c,c->argv[4]);
if (to_meter < 0) return;
} else if (c->argc > 5) {
addReplyErrorObject(c,shared.syntaxerr);
return;
}
robj *zobj = NULL;
if ((zobj = lookupKeyReadOrReply(c, c->argv[1], shared.null[c->resp]))
== NULL || checkType(c, zobj, OBJ_ZSET)) return;
double score1, score2, xyxy[4];
if (zsetScore(zobj, c->argv[2]->ptr, &score1) == C_ERR ||
zsetScore(zobj, c->argv[3]->ptr, &score2) == C_ERR)
{
addReplyNull(c);
return;
}
if (!decodeGeohash(score1,xyxy) || !decodeGeohash(score2,xyxy+2))
addReplyNull(c);
else
addReplyDoubleDistance(c,
geohashGetDistance(xyxy[0],xyxy[1],xyxy[2],xyxy[3]) / to_meter);
}
