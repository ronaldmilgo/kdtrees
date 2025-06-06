#ifndef __LOCATION_H__
#define __LOCATION_H__

/**
 * A location on a sphere given by latitude and longitude.  Latitudes
 * must be between -90.0 and 90.0 and longitudes must be between -180.0
 * and 180.0.  Both ranges inlcude both endpoints.
 */
typedef struct _location
{
  double lat;
  double lon;
} location;


/**
 * Determines if the given location is valid.  A location is valid if the
 * latitude is between -90 and 90 degrees (inclusive) and the longitude is
 * between -180.0 and 180.0 degrees (inclusive).
 *
 * @param l a pointer to a location, non-NULL
 * @return true if the location is valid, false otherwise
 */
int location_validate(const location *l);


/**
 * Compares points by latitude, breaking ties by longitude.
 *
 * @param l1 a pointer to a location, non-NULL
 * @param l2 a pointer to a location, non-NULL
 * @return a negative number if l1's latitude is < l2's or if equal
 * and l1's longitude is < l2's; 0 if l1 and l2 have equal coordinates
 * (which is not the same as are the same point); positive otherwise
 */
int location_compare_latitude(const location *l1, const location *l2);


/**
 * Compares points by longitude, breaking ties by latitude.
 *
 * @param l1 a pointer to a location, non-NULL
 * @param l2 a pointer to a location, non-NULL
 * @return a negative number if l1's longitude is < l2's or if equal
 * and l1's latitude is < l2's; 0 if l1 and l2 have equal coordinates
 * (which is not the same as are the same point); positive otherwise
 */
int location_compare_longitude(const location *l1, const location *l2);


/**
 * Returns the distance between the two locations on the Earth's surface,
 * assuming an oblate spheroid model of earth with semi-major axis 6378.1km
 * and flattening 1/298.25.  If the algorithm fails on the oblate spheroid,
 * then it falls back to a spherical model with radius 6371km.  A return
 * value of NaN indicates an invalid location.
 *
 * @param l1 a valid location
 * @param l2 a valid location
 * @return the distance between those points
 */
double location_distance(const location *l1, const location *l2);

#endif
