package papabench.pj.juav;

import java.util.List;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Map;
import java.util.Map.Entry;
import java.util.TreeMap;

import papabench.core.autopilot.data.Attitude;
import papabench.core.autopilot.data.Position3D;
import papabench.core.autopilot.devices.GPSDevice;

public class OutputMsgInfo {
	private static int NAVIGATION_REF_EAST = 360285;
	private static int NAVIGATION_REF_NORTH = 4813595;
	private static int AC_ID = 4;
	private Map<String, Integer> indexMap = new TreeMap<String, Integer>();
	private final String[] OTHER_MSG_TYPE = new String[] { "ALIVE",
			"PPRZ_MODE", "DOWNLINK", "ATTITUDE", "ESTIMATOR", "AIRSPEED",
			"BAT", "ENERGY", "DL_VALUE", "DESIRED", "CALIBRATION",
			"STATE_FILTER_STATUS", "FBW_STATUS", "NAVIGATION_REF",
			"NAVIGATION", "WP_MOVED", "CIRCLE", "GPS", "PPM" };
	private final int[] OTHER_MSG_TRIGGER_SLOT = new int[] { 12, 24, 6, 0, 3,
			36, 18, 30, 60, 0, 48, 30, 12, 0, 30, 24, 66, 24, 24 };
	private final int[] OTHER_MSG_RESET_PERIODIC = new int[] { 306, 294, 306,
			6, 30, 60, 66, 144, 90, 12, 126, 300, 120, 540, 60, 30, 183, 63, 30 };
	private int[] OTHER_MSG_COUNTER;

	private static OutputMsgInfo instance = null;
	private int numOtherMsg = 0;
	private String MD5SUM_STR;

	private OutputMsgInfo() {
		int[] MD5SUM = new int[] { 0167, 0236, 0063, 0006, 0132, 0326, 0344,
				0124, 0260, 0002, 0301, 0364, 0277, 0252, 0205, 0333 };
		String convert = Arrays.toString(MD5SUM);
		convert = convert.replace(",\\s", ",");
		convert = convert.replace(", ", ",");
		MD5SUM_STR = convert.substring(1, convert.length() - 1);
		OTHER_MSG_COUNTER = new int[OTHER_MSG_TYPE.length];
		for (int i = 0; i < OTHER_MSG_TYPE.length; i++) {
			indexMap.put(OTHER_MSG_TYPE[i], i);
			OTHER_MSG_COUNTER[i] = 0;
		}
	}

	public static OutputMsgInfo getInstance() {
		if (instance == null) {
			instance = new OutputMsgInfo();
		}
		return instance;
	}

	public List<String> getOutputMsg() {
		List<String> msgList = new ArrayList<String>();
		// Update each counter
		for (Entry<String, Integer> entry : indexMap.entrySet()) {
			if (OTHER_MSG_COUNTER[entry.getValue()] == OTHER_MSG_TRIGGER_SLOT[entry
					.getValue()]) {
				// Get the String from this particular counter
				String msg = otherMsgFactory(entry.getKey());
				if (msg.length() > 0)
					msgList.add(msg);
			}
			if (OTHER_MSG_COUNTER[entry.getValue()] >= OTHER_MSG_RESET_PERIODIC[entry
					.getValue()]) {
				OTHER_MSG_COUNTER[entry.getValue()] = 0;
			} else {
				OTHER_MSG_COUNTER[entry.getValue()]++;
			}
		}
		return msgList;
	}

	public String generateCommand() {
		MessageHelper msgHelper = MessageHelper.getInstance();
		int throttle = msgHelper.getGaz();
		int roll = msgHelper.getRoll();
		int pitch = msgHelper.getPitch();
		String command = AC_ID + " COMMANDS" + " " + throttle + "," + roll
				+ "," + (pitch * -1) + ",9700\n";
		return command;
	}

	public String generateGPS() {
		String msg = "" + AC_ID + " " + "GPS" + " ";
		MessageHelper helper = MessageHelper.getInstance();
		// week : 1, utm:31, gps_error:1
		GPSDevice gps = helper.getGPSDevice();
		int utmEest = (int) ((gps.getEast() /*+ NAVIGATION_REF_EAST*/) * 100);
		// int utmNorth = (int) ((gps.getUtmNorth() + NAVIGATION_REF_NORTH) *
		// 100);
		int utmNorth = (int) ((gps.getNorth() /* + NAVIGATION_REF_NORTH */) * 100);
		int course1 = (int) (gps.getCourse() * 180 / Math.PI * 100);
		int mode = 3;// jpapabench set by 1<<5, which doesn't make any
						// sense
		msg += mode + " " + utmEest + " " + utmNorth + " " + course1 + " "
				+ (int) (gps.getAltitude() * 1000) + " "
				+ (int) (gps.getSpeed() * 100) + " "
				+ (int) (gps.getClimb() * 100) + " 1 "
				+ (int) (gps.getTow() * 100) + " 31 1";
		return msg;
	}

	public String otherMsgFactory(String msgType) {
		numOtherMsg += 1;
		if (indexMap.containsKey(msgType)) {
			int index = indexMap.get(msgType);
			String msg = "" + AC_ID + " " + msgType + " ";
			MessageHelper helper = MessageHelper.getInstance();
			switch (index) {
			case 0: // ALIVE
				msg += MD5SUM_STR;
				break;
			// PPRZ_MODE
			case 1:
				/*
				 * ppz + vertial_mode + lateral_mode + horizontal_mode(NA) +
				 * rc_setting_mode(always:0) + mcu(fbw stat) with radio status
				 */
				msg += helper.getPPZMode() + " " + helper.getVCtlMode() + " "
						+ helper.getLatMode() + " 0 0 6";
				break;
			case 2: // DOWNLINK
				/*
				 * num incomplete msg + sending rate (byts/second) + num sending
				 * msg
				 */
				msg += "0 900" + " " + numOtherMsg;
				break;
			case 3: // ATTITUDE
				Attitude attitude = helper.getState().getAttitude();
				msg += attitude.phi + " " + attitude.psi + " " + attitude.theta;
				break;
			case 4: // ESTIMATOR
				float zDot = helper.getState().getZDot();
				float posZ = helper.getState().getPosition().z;
				msg += posZ + " " + zDot;
				break;
			case 5: // AIRSPEED, skipped
				msg = "";
				break;
			case 6: // BAT
				int throttle = helper.getGaz();
				int voltage = 120;
				int flight_time = (int) helper.getState().getTime();
				// amp:83, kill_auto_throttle, block_time, stage_time:0;
				// energy:768
				// Statically calculated
				msg += throttle + " " + voltage + " 83 " + flight_time
						+ " 0 0 0 768";
				break;
			case 7: // ENERGY
				msg += "12.0 0.83 768 9.972"; // bat amp, energy, power
				break;
			case 8: // DL_VALUE, ignored
				// configuration message from GCT
				msg = "";
				break;
			case 9: // DESIRED
				// roll, pitch, course, x, y, altitude, climb, airspeed
				float roll = helper.getNavigator().getDesiredRoll();
				float pitch = helper.getNavigator().getDesiredPitch();
				float course = helper.getNavigator().getDesiredCourse();
				float x = helper.getNavigator().getDesiredPosition().x;
				float y = helper.getNavigator().getDesiredPosition().y;
				float altitude = helper.getNavigator().getDesiredAltitude();
				float climb = helper.getNavigator().getPreClimb();
				float airspeed = 13.0f;
				msg += roll + " " + pitch + " " + course + " " + x + " " + y
						+ " " + altitude + " " + climb + " " + airspeed;
				break;
			case 10: // CALIBRATION, skipped
				// &v_ctl_auto_throttle_sum_err, &v_ctl_auto_throttle_submode
				// climb_sum_err, climb_gaz_submode
				msg = "";
				break;
			case 11: // STATE_FILTER_STATUS, skipped
				// &mde, &val
				// state_filter_mode, value
				msg = "";
				break;
			case 12: // FBW_STATUS
				// rc_status, frame_rate, mode, vsupply, current
				int voltage1 = helper.getVoltSupply();
				msg += "0 0 " + helper.getFBWMode() + " " + voltage1 + " 831";
				break;
			case 13: // NAVIGATION_REF
				// &nav_utm_east0, &nav_utm_north0, &nav_utm_zone0
				// utm_east, utm_north, utm_zone, ground_alt
				msg += "0 0 31";// 125";
				break;
			case 14: // NAVIGATION
				// &nav_block, &nav_stage, &(pos->x), &(pos->y), &dist2_to_wp,
				// &dist2_to_home, &_circle_count, &nav_oval_count
				int curBlock = helper.getCurBlock();
				int curStage = helper.getCurStage();
				float posx = helper.getState().getPosition().x;
				float posy = helper.getState().getPosition().y;
				double distTOhome = Math.sqrt(posx * posx + posy * posy);

				msg += curBlock + " " + curStage + " " + posx + " " + posy
						+ " 0 " + distTOhome + " 0 0";
				break;
			case 15: // WP_MOVED
				Position3D wayPoints = helper.getWayPoint();
				int id = helper.getWayPintId();
				msg += id + " " + (wayPoints.x + NAVIGATION_REF_EAST) + " "
						+ (wayPoints.y + NAVIGATION_REF_NORTH) + " "
						+ wayPoints.z + " 31";
				break;
			case 16: // CIRCLE, ignored
				// &nav_circle_x, &nav_circle_y, &nav_circle_radius
				// center_east, center_north, radius
				msg = "";
				break;
			case 17: // GPS
				// week : 1, utm:31, gps_error:1
				GPSDevice gps = helper.getGPSDevice();
				int utmEest = (int) ((gps.getEast() + NAVIGATION_REF_EAST) * 100);
				int utmNorth = (int) ((gps.getUtmNorth() + NAVIGATION_REF_NORTH) * 100);
				int course1 = (int) (gps.getCourse() * 180 / Math.PI * 100);
				int mode = 3;// jpapabench set by 1<<5, which doesn't make any
								// sense
				msg += mode + " " + utmEest + " " + utmNorth + " " + course1
						+ " " + (int) (gps.getAltitude() * 1000) + " "
						+ (int) (gps.getSpeed() * 100) + " "
						+ (int) (gps.getClimb() * 100) + " 1 "
						+ (int) (gps.getTow() * 100) + " 31 1";
				break;
			case 18: // PPM, ingore
				/*
				 * &radio_control.frame_rate :: ppm_rate, //
				 * DownlinkPutUint8ByAddr
				 * 
				 * PPM_NB_CHANNEL :: nb_values //DownlinkPutUint16Array
				 * ppm_pulses_usec :: values
				 */
				msg = "";
				break;
			default:
				break;
			}
			msg = msg + "\n";
			return msg;
		} else {
			return "";
		}
	}

	public String generateDL_Value(int index, float val) {

		String msg = "" + AC_ID + " DL_VALUE " + index + " " + val;
		return msg;
		//
		// #define DOWNLINK_SEND_DL_VALUE(_trans, _dev, index, value){ \
		// if (DownlinkCheckFreeSpace(_trans, _dev, DownlinkSizeOf(_trans, _dev,
		// 0+1+4))) {\
		// DownlinkCountBytes(_trans, _dev, DownlinkSizeOf(_trans, _dev,
		// 0+1+4)); \
		// DownlinkStartMessage(_trans, _dev, "DL_VALUE", DL_DL_VALUE, 0+1+4) \
		// DownlinkPutUint8ByAddr(_trans, _dev, (index)); \
		// DownlinkPutFloatByAddr(_trans, _dev, (value)); \
		// DownlinkEndMessage(_trans, _dev ) \
		// } else \
		// DownlinkOverrun(_trans, _dev ); \
		// }
	}
}
