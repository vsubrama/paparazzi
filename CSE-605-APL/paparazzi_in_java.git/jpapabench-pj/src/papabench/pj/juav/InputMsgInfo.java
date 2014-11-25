package papabench.pj.juav;

import java.util.ArrayList;
import java.util.List;
import java.util.Arrays;

import papabench.core.autopilot.devices.GPSDevice;
import papabench.core.autopilot.devices.IRDevice;

/*
 * Info on the incoming messages
 * 
 * 
 * :::GPS:::
 * There are two types
 * 
 * 1: JSBSim
 * IvySendMsg("NPS_SEN_NICE_GPS %d %d %f %f %f %f %f %f", 6, AC_ID, lat, lon, alt, course, gspeed, climb, time);
 * 
 * 
 * 2: NPS
 * IvySendMsg( "NPS_SEND_GPS_INT %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d", 
 * 6, AC_ID, gps.ecef_pos.x, gps.ecef_pos.y, gps.ecef_pos.z, gps.lla_pos.lat, gps.lla_pos.lon, 
 * gps.lla_pos.alt, gps.hmsl, gps.ecef_vel.x, gps.ecef_vel.y, gps.ecef_vel.z, gps.pacc, gps.sacc, 
 * gps.tow, gps.pdop, gps.num_sv, gps.fix);
 * 
 * 
 * The JSBSim data is slightly better aligned to the SimulatorGPSDeviceImpl.java data
 * alt = altitude
 * course = course
 * climb = climb
 * time = tow
 * gspeed = speed ??
 * lat = north? east?
 * lon = east? north?
 * 
 * This seems like a better match to the JSBSim data.
 * Note, The working C version has not been run with the JSBSim code, so it may work or it may not
 * This will be a crucial component to getting a valid comparison of both systems
 * 
 * 
 * :::IR:::
 * This is only one type of data from the Simulator. This is the JSBSim data and has no direct correlation (?) to the 
 * NPS based simulation.
 * 
 * IvySendMsg("NPS_SEN_NICE_IR %d %d %f %f %f %f %f %f", 6, AC_ID, 
 * fdm.ltp_to_body_eulers.phi, fdm.ltp_to_body_eulers.theta, fdm.ltp_to_body_eulers.psi, 
 * fdm.body_ecef_rotvel.p, fdm.body_ecef_rotvel.q, fdm.body_ecef_rotvel.r);
 * 
 * The JpapaBench SimulatorIRDeviceImpl.java has
 * irRoll
 * irPitch
 * irContrast
 * irRollNeutral
 * irPitchNeutral
 * simulIrRoll
 * simulIrPitch
 * irLeft
 * irFront
 * irTop
 * 
 * It may be a matter of taking these values and directly overwriting the JpapaBench attitude information
 * This is bad because it does not allow the autopilot to determine where it is itself but hopefully will
 * allow the simulation to actually run 
 * */

public class InputMsgInfo {
	/* The messages we may need to support */
	private static final String DL_PING = "DL_PING";
	private static final String DL_ACINFO = "DL_ACINFO";
	private static final String DL_SETTING = "DL_SETTING";
	private static final String DL_VALUE = "DL_VALUE";
	private static final String GET_DL_SETTING = "GET_DL_SETTING"; // Skipping
	private static final String DL_BLOCK = "BLOCK";
	private static final String MOVE_WP = "MOVE_WP";
	private static final String NPS_SEN_NICE_IR = "NPS_SEN_NICE_IR";
	private static final String NPS_SEN_NICE_GPS = "NPS_SEN_NICE_GPS";
	private static InputMsgInfo ref;

	// private static String tempIR;

	// XXX For now ignoring the other NPS_SIMULATOR data sets. We don't know how
	// XXX to relate them to this java implementation right now so ...

	public static InputMsgInfo getInstance() {
		if (ref == null) {
			ref = new InputMsgInfo();
		}
		return ref;
	}

	int getIndex(String key) {
		if (key.equals(DL_PING)) {
			return 2;
		} else if (key.equals(NPS_SEN_NICE_IR)) {
			return 0;
		} else if (key.equals(NPS_SEN_NICE_GPS)) {
			return 1;
		} else if (key.equals(MOVE_WP)) {
			return 2;
		} else if (key.equals(DL_SETTING)) {
			return 2;
		} else if (key.equals(GET_DL_SETTING)) {
			return 2;
		}
		return -1;
	}

	String getMessage(String key) {
		return MessageHelper.getInstance().getCommChannel().readChannel(key);
	}

	String getMessage(int key) {
		if (key < 0)
			return "";

		return MessageHelper.getInstance().getCommChannel().readChannel(key);
	}

	// Local split method as fiji java does not support it
	List<String> split(String s) {
		return split(s, ' ');
	}

	// Local split method as fiji java does not support it
	List<String> split(String s, char regex) {
		List<String> l = new ArrayList<String>();

		int startIndex = 0;

		// finding all occurrences forward: Method #1
		for (int i = s.indexOf(regex); i != -1; i = s.indexOf(regex, i + 1)) {
			l.add(s.substring(startIndex, i));
			startIndex = i + 1;
		}

		return l;
	}

	public double[] getSimIr() {
		double[] val = new double[6];
		for (int i = 0; i < val.length; i++) {
			val[i] = Double.NaN;
		}
		// tempIR += getMessage(getIndex(NPS_SEN_NICE_IR));
		// // If there is no space newline sequence, then there is no full
		// message
		// if (!tempIR.contains(" \n")) {
		// return val;
		// }
		// List<String> IR = split(tempIR, '\n');
		//
		// // Remove the about to be processed data
		// if (IR.size() > 0) {
		// String sub = tempIR.substring(tempIR.indexOf('\n', IR.size()));
		// tempIR = sub;
		// } else
		// return val;

		String IR = getMessage(NPS_SEN_NICE_IR);
		// System.out.println("IR {" + IR + "}");

		List<String> str = split(IR, ' '); // Should have 10 values

		// System.out.println(str.toString());
		if (str.size() < 9)
			return val;

		val[0] = Double.parseDouble(str.get(3));
		val[1] = Double.parseDouble(str.get(4));
		val[2] = Double.parseDouble(str.get(5));
		val[3] = Double.parseDouble(str.get(6));
		val[4] = Double.parseDouble(str.get(7));
		val[5] = Double.parseDouble(str.get(8));

		// System.out.println("IR {");
		// for (int i = 0; i < val.length; i++) {
		// System.out.println(val[i] + ",");
		// }
		// System.out.println(" }");
		return val;
	}

	public double[] getSimGPS() {
		double[] val = new double[8];
		for (int i = 0; i < val.length; i++) {
			val[i] = Double.NaN;
		}
		String GPS = getMessage(NPS_SEN_NICE_GPS);
		// System.out.println("GPS {" + GPS + "}");

		List<String> str = split(GPS);

		if (str.size() < 10) {
			return val;
		}

		double lat = Double.parseDouble(str.get(3));
		double lon = Double.parseDouble(str.get(4));
		double alt = Double.parseDouble(str.get(5));
		double course = Double.parseDouble(str.get(6));
		double gspeed = Double.parseDouble(str.get(7)); // m/s
		double climb = Double.parseDouble(str.get(8));
		double time = Double.parseDouble(str.get(9));

		/*
		 * Taken from jsbsim_gps.c
		 * 
		 * The GPS Device is expecting x and y position We have Lat and Lon.
		 */
		int fix = 3; // GPS fix = 3d
		course = course * 1e7;
		// alt = alt * 1000; // This may be in mm, converting to m
		climb = climb * -100; // Invert and convert to m/s from cm/s

		val[0] = lat;
		val[1] = lon;
		val[2] = alt;
		val[3] = course;
		val[4] = gspeed;
		val[5] = climb;
		val[6] = time;
		val[7] = 3.0;

		System.out.print("GPS { ");
		for (int i = 0; i < val.length; i++) {
			System.out.print(val[i] + ", ");
		}
		System.out.println(" }");
		return val;
	}

	void processPING() {
		String PING = getMessage(DL_PING);
		// TODO If the message is valid, send a response.
		// TODO Send the PONG message back
	}

	// void processACINFO() {
	// String ACINFO = getMessage(getIndex(DL_ACINFO));
	// String str[] = ACINFO.split(" ");
	// int course = Integer.parseInt(str[4]);
	// int utm_east = Integer.parseInt(str[5]);
	// int utm_north = Integer.parseInt(str[6]);
	// int alt = Integer.parseInt(str[7]);
	// int itow = Integer.parseInt(str[8]);
	// int speed = Integer.parseInt(str[9]);
	// int climb = Integer.parseInt(str[10]);
	// int ac_id = Integer.parseInt(str[11]);
	// // TODO Figure out how / where to use this data
	// }
	//
	// void processBlock() {
	// String BLOCK = getMessage(getIndex(DL_BLOCK));
	// String str[] = BLOCK.split(" ");
	// int block_id = Integer.parseInt(str[3]);
	// int ac_id = Integer.parseInt(str[4]);
	// // TODO Verify these indices
	// // TODO We should send a NAVIGATION message back in response
	// }
	//
	// void processWayPoint() {
	// String WP = getMessage(getIndex(MOVE_WP));
	// String str[] = WP.split(" ");
	// int wp_id = Integer.parseInt(str[2]);
	// int ac_id = Integer.parseInt(str[3]);
	// int lat = Integer.parseInt(str[4]);
	// int lon = Integer.parseInt(str[5]);
	// int alt = Integer.parseInt(str[6]);
	// // TODO Verify and process this data. Possibly add these waypoints to
	// // the system???
	// }

	/**
	 * This processes settings from the GCS, including the launch setting and
	 * others.
	 */
	public void processSimSettings() {
		String SETTING = getMessage(DL_VALUE);
		if (SETTING == null)
			return;

		String str[] = SETTING.split(" ");
		if (str.length < 3)
			return;
		int index = Integer.parseInt(str[2]);
		// int ac_id = Integer.parseInt(str[4]);
		float value = Float.parseFloat(str[3]);
		switch (index) {
		case 8:
			System.out.println("SETTINGS: LAUNCH!!!");
			// Send back the setting acknowledgement
			MessageHelper
					.getInstance()
					.getCommChannel()
					.sendOtherMsg(
							OutputMsgInfo.getInstance().generateDL_Value(index,
									value));
			MessageHelper.getInstance().launch();
			// TODO THIS IS THE LAUNCH COMMAND HERE
			/*
			 * The sequence for a launch: The plane must be in the correct pose,
			 * this means it should be on the launch platform pointing upwards
			 * at some angle (15') with an initial zero velocity in all
			 * directions
			 * 
			 * As soon as the launch command is given, the launcher propels the
			 * plane at an initial velocity of 20m/s on its current angle.
			 * 
			 * The autopilot applies a climb throttle(lots of power) and starts
			 * to climb towards the climb waypoint. The pitch, roll and throttle
			 * should be being automatically adjusted by the autopilot as the
			 * plane is climbing. As the target is neared, the pitch should
			 * level off. Once the safe altitude is reached, the plane should
			 * move to the next navigation stage which for our simple case
			 * should be waypoint 1.
			 * 
			 * Requirements: 1: Initialise everything to the appropriate values
			 * 2: Wait to launch until the command is received from the GCS 3:
			 * Upon receipt of the launch command, set velocity to 20m/s 4:
			 * Engage the climb mode until the safe velocity is reached.
			 */
			break;

		default:
			System.out.println("SETTINGS: DEFAULT");
			break;
		}
	}
	// #define DlSetting(_idx, _value) { \
	// switch (_idx) { \
	// case 0: telemetry_mode_Ap = _value; break;\
	// case 1: telemetry_mode_Fbw = _value; break;\
	// case 2: flight_altitude = _value; break;\
	// case 3: nav_course = _value; break;\
	// case 4: nav_IncreaseShift( _value ); _value = nav_shift; break;\
	// case 5: autopilot_ResetFlightTimeAndLaunch( _value ); _value =
	// autopilot_flight_time; break;\
	// case 6: nav_SetNavRadius( _value ); _value = nav_radius; break;\
	// case 7: pprz_mode = _value; break;\
	// case 8: launch = _value; break;\
	// case 9: kill_throttle = _value; break;\
	// case 10: gps_Reset( _value ); _value = gps.reset; break;\
	// case 11: ap_state->command_roll_trim = _value; break;\
	// case 12: ap_state->command_pitch_trim = _value; break;\
	// case 13: ap_state->command_yaw_trim = _value; break;\
	// case 14: h_ctl_roll_pgain = _value; break;\
	// case 15: h_ctl_roll_max_setpoint = _value; break;\
	// case 16: h_ctl_pitch_pgain = _value; break;\
	// case 17: h_ctl_pitch_dgain = _value; break;\
	// case 18: h_ctl_elevator_of_roll = _value; break;\
	// case 19: h_ctl_aileron_of_throttle = _value; break;\
	// case 20: h_ctl_roll_attitude_gain = _value; break;\
	// case 21: h_ctl_roll_rate_gain = _value; break;\
	// case 22: v_ctl_altitude_pgain = _value; break;\
	// case 23: guidance_v_SetCruiseThrottle( _value ); _value =
	// v_ctl_auto_throttle_cruise_throttle; break;\
	// case 24: v_ctl_auto_throttle_pgain = _value; break;\
	// case 25: v_ctl_auto_throttle_igain = _value; break;\
	// case 26: v_ctl_auto_throttle_dgain = _value; break;\
	// case 27: v_ctl_auto_throttle_climb_throttle_increment = _value;
	// break;\
	// case 28: v_ctl_auto_throttle_pitch_of_vz_pgain = _value; break;\
	// case 29: v_ctl_auto_throttle_pitch_of_vz_dgain = _value; break;\
	// case 30: v_ctl_auto_pitch_pgain = _value; break;\
	// case 31: v_ctl_auto_pitch_igain = _value; break;\
	// case 32: h_ctl_course_pgain = _value; break;\
	// case 33: h_ctl_course_dgain = _value; break;\
	// case 34: h_ctl_course_pre_bank_correction = _value; break;\
	// case 35: nav_glide_pitch_trim = _value; break;\
	// case 36: h_ctl_roll_slew = _value; break;\
	// case 37: nav_radius = _value; break;\
	// case 38: nav_course = _value; break;\
	// case 39: nav_mode = _value; break;\
	// case 40: nav_climb = _value; break;\
	// case 41: fp_pitch = _value; break;\
	// case 42: nav_IncreaseShift( _value ); _value = nav_shift; break;\
	// case 43: nav_ground_speed_setpoint = _value; break;\
	// case 44: nav_ground_speed_pgain = _value; break;\
	// case 45: nav_survey_shift = _value; break;\
	// case 46: v_ctl_auto_throttle_dash_trim = _value; break;\
	// case 47: v_ctl_auto_throttle_loiter_trim = _value; break;\
	// case 48: ins_roll_neutral = _value; break;\
	// case 49: ins_pitch_neutral = _value; break;\
	// case 50: digital_cam_servo_servo_cam_ctrl_periodic_status = _value;
	// break;\
	// default: break;\
	// }\
	// }
}
