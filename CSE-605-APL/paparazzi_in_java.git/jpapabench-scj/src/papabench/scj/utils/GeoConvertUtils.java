/* $Id$
 * 
 * This file is a part of jPapaBench providing a Java implementation 
 * of PapaBench project.
 * Copyright (C) 2010  Michal Malohlava <michal.malohlava_at_d3s.mff.cuni.cz>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 * 
 */
package papabench.scj.utils;

import papabench.core.commons.data.GeoPosition;
import papabench.core.commons.data.UTMPosition;

/**
 * @author Michal Malohlava
 *
 */
final public class GeoConvertUtils {
	
	public static class Ellipsoid {		
		public Ellipsoid(float dx, float dy, float dz, float a, float df, float e) {			
			this.dx = dx; this.dy = dy;	this.dz = dz;
			this.a = a; this.df = df; this.e = e;
		}
		public float dx;
		public float dy;
		public float dz;
		public float a;
		public float df;
		public float e;
	}
	
	public static final Ellipsoid WGS84 = new Ellipsoid(0, 0, 0, 6378137.0f, 0.0033528106647474805f, 0.08181919106f); 
	
	
	public static final UTMPosition utmFromWGS84(GeoPosition geoPos) {
//		 let ellipsoid =  ellipsoid_of geo in
//		  let k0 = 0.9996
//		  and xs = 500000. in
//		  let e = ellipsoid.e
//		  and n = k0 *. ellipsoid.a in
//		  let c = serie5 coeff_proj_mercator e in
//		  
//		  fun ({posn_long = lambda; posn_lat = phi} as pos) ->
//		    if not (valid_geo pos) then
//		      invalid_arg "Latlong.utm_of";
//		    let lambda_deg = truncate (floor ((Rad>>Deg)lambda)) in
//		    let zone = (lambda_deg + 180) / 6 + 1 in
//		    let lambda_c = (Deg>>Rad) (float (lambda_deg - ((lambda_deg mod 6)+6)mod 6 + 3)) in
//		    let ll = latitude_isometrique phi e
//		    and dl = lambda -. lambda_c in
//		    let phi' = asin (sin dl /. cosh ll) in
//		    let ll' = latitude_isometrique phi' 0. in
//		    let lambda' = atan (sinh ll /. cos dl) in
//		    let z = C.make lambda' ll' in
//		    let z' = ref (C.scal c.(0) z) in
//		    for k = 1 to Array.length c - 1 do
//		      z' := C.add !z' (C.scal c.(k) (C.sin (C.scal (float (2*k)) z)))
//		    done;
//		    z' := C.scal n !z';
//		    { utm_zone = zone; utm_x = xs +. C.im !z'; utm_y = C.re !z' };;
		float k0 = 0.9996f;
		float xs = 500000;
		float e = WGS84.e;
		float f = k0 * WGS84.a;
//		float c = FIXME
		return null;
	}
}
