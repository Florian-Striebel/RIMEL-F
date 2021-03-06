#include <stddef.h>
#include <stdint.h>
#include <stdint.h>
#if defined(__cplusplus)
extern "C" {
#endif
#define HASHISZERO(r) (!(r).bits && !(r).step)
#define RANGEISZERO(r) (!(r).max && !(r).min)
#define RANGEPISZERO(r) (r == NULL || RANGEISZERO(*r))
#define GEO_STEP_MAX 26
#define GEO_LAT_MIN -85.05112878
#define GEO_LAT_MAX 85.05112878
#define GEO_LONG_MIN -180
#define GEO_LONG_MAX 180
typedef enum {
GEOHASH_NORTH = 0,
GEOHASH_EAST,
GEOHASH_WEST,
GEOHASH_SOUTH,
GEOHASH_SOUTH_WEST,
GEOHASH_SOUTH_EAST,
GEOHASH_NORT_WEST,
GEOHASH_NORT_EAST
} GeoDirection;
typedef struct {
uint64_t bits;
uint8_t step;
} GeoHashBits;
typedef struct {
double min;
double max;
} GeoHashRange;
typedef struct {
GeoHashBits hash;
GeoHashRange longitude;
GeoHashRange latitude;
} GeoHashArea;
typedef struct {
GeoHashBits north;
GeoHashBits east;
GeoHashBits west;
GeoHashBits south;
GeoHashBits north_east;
GeoHashBits south_east;
GeoHashBits north_west;
GeoHashBits south_west;
} GeoHashNeighbors;
#define CIRCULAR_TYPE 1
#define RECTANGLE_TYPE 2
typedef struct {
int type;
double xy[2];
double conversion;
double bounds[4];
union {
double radius;
struct {
double height;
double width;
} r;
} t;
} GeoShape;
void geohashGetCoordRange(GeoHashRange *long_range, GeoHashRange *lat_range);
int geohashEncode(const GeoHashRange *long_range, const GeoHashRange *lat_range,
double longitude, double latitude, uint8_t step,
GeoHashBits *hash);
int geohashEncodeType(double longitude, double latitude,
uint8_t step, GeoHashBits *hash);
int geohashEncodeWGS84(double longitude, double latitude, uint8_t step,
GeoHashBits *hash);
int geohashDecode(const GeoHashRange long_range, const GeoHashRange lat_range,
const GeoHashBits hash, GeoHashArea *area);
int geohashDecodeType(const GeoHashBits hash, GeoHashArea *area);
int geohashDecodeWGS84(const GeoHashBits hash, GeoHashArea *area);
int geohashDecodeAreaToLongLat(const GeoHashArea *area, double *xy);
int geohashDecodeToLongLatType(const GeoHashBits hash, double *xy);
int geohashDecodeToLongLatWGS84(const GeoHashBits hash, double *xy);
void geohashNeighbors(const GeoHashBits *hash, GeoHashNeighbors *neighbors);
#if defined(__cplusplus)
}
#endif
