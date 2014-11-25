package papabench.pj.utility;

import papabench.core.autopilot.data.Position3D;

public class GPSConversion {
	static boolean setOffsets = true;
	static float x_offset = 0;
	static float y_offset = 0;

	// earth semimajor axis in meters
	static final float EARTH_RADIUS = 6378137.0f;

	/**
	 * @param lat
	 *            in deg
	 * @param lon
	 *            in deg
	 * @return The X projection from the lat and lon coordinate in m
	 */
	static public float getXfromLatLon(float lat, float lon) {
		double x = Math.cos(lat) * Math.cos(lon);
		x *= EARTH_RADIUS;
		return (float) x;
	}

	/**
	 * 
	 * @param lat
	 *            in deg
	 * @param lon
	 *            in deg
	 * @return The y projection from the lat and lon coordinate in m
	 */
	static public float getYfromLatLon(float lat, float lon) {
		double y = Math.cos(lat) * Math.sin(lon);
		y *= EARTH_RADIUS;
		return (float) y;
	}

	static public Position3D ecef_of_lla_f(float lat, float lon, float alt) {

		final float a = 6378137.0f; /* earth semimajor axis in meters */
		final float f = 1.0f / 298.257223563f; /* reciprocal flattening */
		final float e2 = 2.0f * f - (f * f); /* first eccentricity squared */

		final float sin_lat = (float) Math.sin(lat);
		final float cos_lat = (float) Math.cos(lat);
		final float sin_lon = (float) Math.sin(lon);
		final float cos_lon = (float) Math.cos(lon);
		final float chi = (float) Math.sqrt(1.0f - (e2 * sin_lat * sin_lat));
		final float a_chi = a / chi;

		float x = (a_chi + alt) * cos_lat * cos_lon;
		float y = (a_chi + alt) * cos_lat * sin_lon;
		float z = (a_chi * (1.0f - e2) + alt) * sin_lat;

		// Set the offsets so that the starting position is 0,0,alt
		if (setOffsets) {
			x_offset = x;
			y_offset = y;
			setOffsets = false;
		}

		// Offset so that 0,0 is the starting position
		return new Position3D(x - x_offset, y - y_offset, alt);
	}

}
