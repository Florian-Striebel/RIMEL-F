#include "fmacros.h"
#include "geohash_helper.h"
#include "debugmacro.h"
#include <math.h>
#define D_R (M_PI / 180.0)
#define R_MAJOR 6378137.0
#define R_MINOR 6356752.3142
#define RATIO (R_MINOR / R_MAJOR)
#define ECCENT (sqrt(1.0 - (RATIO *RATIO)))
#define COM (0.5 * ECCENT)
const double DEG_TO_RAD = 0.017453292519943295769236907684886;
const double EARTH_RADIUS_IN_METERS = 6372797.560856;
const double MERCATOR_MAX = 20037726.37;
const double MERCATOR_MIN = -20037726.37;
static inline double deg_rad(double ang) { return ang * D_R; }
static inline double rad_deg(double ang) { return ang / D_R; }
uint8_t geohashEstimateStepsByRadius(double range_meters, double lat) {
if (range_meters == 0) return 26;
int step = 1;
while (range_meters < MERCATOR_MAX) {
range_meters *= 2;
step++;
}
step -= 2;
if (lat > 66 || lat < -66) {
step--;
if (lat > 80 || lat < -80) step--;
}
if (step < 1) step = 1;
if (step > 26) step = 26;
return step;
}
int geohashBoundingBox(GeoShape *shape, double *bounds) {
if (!bounds) return 0;
double longitude = shape->xy[0];
double latitude = shape->xy[1];
double height = shape->conversion * (shape->type == CIRCULAR_TYPE ? shape->t.radius : shape->t.r.height/2);
double width = shape->conversion * (shape->type == CIRCULAR_TYPE ? shape->t.radius : shape->t.r.width/2);
const double lat_delta = rad_deg(height/EARTH_RADIUS_IN_METERS);
const double long_delta_top = rad_deg(width/EARTH_RADIUS_IN_METERS/cos(deg_rad(latitude+lat_delta)));
const double long_delta_bottom = rad_deg(width/EARTH_RADIUS_IN_METERS/cos(deg_rad(latitude-lat_delta)));
int southern_hemisphere = latitude < 0 ? 1 : 0;
bounds[0] = southern_hemisphere ? longitude-long_delta_bottom : longitude-long_delta_top;
bounds[2] = southern_hemisphere ? longitude+long_delta_bottom : longitude+long_delta_top;
bounds[1] = latitude - lat_delta;
bounds[3] = latitude + lat_delta;
return 1;
}
GeoHashRadius geohashCalculateAreasByShapeWGS84(GeoShape *shape) {
GeoHashRange long_range, lat_range;
GeoHashRadius radius;
GeoHashBits hash;
GeoHashNeighbors neighbors;
GeoHashArea area;
double min_lon, max_lon, min_lat, max_lat;
int steps;
geohashBoundingBox(shape, shape->bounds);
min_lon = shape->bounds[0];
min_lat = shape->bounds[1];
max_lon = shape->bounds[2];
max_lat = shape->bounds[3];
double longitude = shape->xy[0];
double latitude = shape->xy[1];
double radius_meters = shape->type == CIRCULAR_TYPE ? shape->t.radius :
sqrt((shape->t.r.width/2)*(shape->t.r.width/2) + (shape->t.r.height/2)*(shape->t.r.height/2));
radius_meters *= shape->conversion;
steps = geohashEstimateStepsByRadius(radius_meters,latitude);
geohashGetCoordRange(&long_range,&lat_range);
geohashEncode(&long_range,&lat_range,longitude,latitude,steps,&hash);
geohashNeighbors(&hash,&neighbors);
geohashDecode(long_range,lat_range,hash,&area);
int decrease_step = 0;
{
GeoHashArea north, south, east, west;
geohashDecode(long_range, lat_range, neighbors.north, &north);
geohashDecode(long_range, lat_range, neighbors.south, &south);
geohashDecode(long_range, lat_range, neighbors.east, &east);
geohashDecode(long_range, lat_range, neighbors.west, &west);
if (geohashGetDistance(longitude,latitude,longitude,north.latitude.max)
< radius_meters) decrease_step = 1;
if (geohashGetDistance(longitude,latitude,longitude,south.latitude.min)
< radius_meters) decrease_step = 1;
if (geohashGetDistance(longitude,latitude,east.longitude.max,latitude)
< radius_meters) decrease_step = 1;
if (geohashGetDistance(longitude,latitude,west.longitude.min,latitude)
< radius_meters) decrease_step = 1;
}
if (steps > 1 && decrease_step) {
steps--;
geohashEncode(&long_range,&lat_range,longitude,latitude,steps,&hash);
geohashNeighbors(&hash,&neighbors);
geohashDecode(long_range,lat_range,hash,&area);
}
if (steps >= 2) {
if (area.latitude.min < min_lat) {
GZERO(neighbors.south);
GZERO(neighbors.south_west);
GZERO(neighbors.south_east);
}
if (area.latitude.max > max_lat) {
GZERO(neighbors.north);
GZERO(neighbors.north_east);
GZERO(neighbors.north_west);
}
if (area.longitude.min < min_lon) {
GZERO(neighbors.west);
GZERO(neighbors.south_west);
GZERO(neighbors.north_west);
}
if (area.longitude.max > max_lon) {
GZERO(neighbors.east);
GZERO(neighbors.south_east);
GZERO(neighbors.north_east);
}
}
radius.hash = hash;
radius.neighbors = neighbors;
radius.area = area;
return radius;
}
GeoHashFix52Bits geohashAlign52Bits(const GeoHashBits hash) {
uint64_t bits = hash.bits;
bits <<= (52 - hash.step * 2);
return bits;
}
double geohashGetDistance(double lon1d, double lat1d, double lon2d, double lat2d) {
double lat1r, lon1r, lat2r, lon2r, u, v;
lat1r = deg_rad(lat1d);
lon1r = deg_rad(lon1d);
lat2r = deg_rad(lat2d);
lon2r = deg_rad(lon2d);
u = sin((lat2r - lat1r) / 2);
v = sin((lon2r - lon1r) / 2);
return 2.0 * EARTH_RADIUS_IN_METERS *
asin(sqrt(u * u + cos(lat1r) * cos(lat2r) * v * v));
}
int geohashGetDistanceIfInRadius(double x1, double y1,
double x2, double y2, double radius,
double *distance) {
*distance = geohashGetDistance(x1, y1, x2, y2);
if (*distance > radius) return 0;
return 1;
}
int geohashGetDistanceIfInRadiusWGS84(double x1, double y1, double x2,
double y2, double radius,
double *distance) {
return geohashGetDistanceIfInRadius(x1, y1, x2, y2, radius, distance);
}
int geohashGetDistanceIfInRectangle(double width_m, double height_m, double x1, double y1,
double x2, double y2, double *distance) {
double lon_distance = geohashGetDistance(x2, y2, x1, y2);
double lat_distance = geohashGetDistance(x2, y2, x2, y1);
if (lon_distance > width_m/2 || lat_distance > height_m/2) {
return 0;
}
*distance = geohashGetDistance(x1, y1, x2, y2);
return 1;
}
