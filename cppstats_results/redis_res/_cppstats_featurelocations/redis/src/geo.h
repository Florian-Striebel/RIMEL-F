#if !defined(__GEO_H__)
#define __GEO_H__

#include "server.h"



typedef struct geoPoint {
double longitude;
double latitude;
double dist;
double score;
char *member;
} geoPoint;

typedef struct geoArray {
struct geoPoint *array;
size_t buckets;
size_t used;
} geoArray;

#endif
