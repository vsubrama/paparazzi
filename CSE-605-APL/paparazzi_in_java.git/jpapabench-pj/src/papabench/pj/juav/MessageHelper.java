package papabench.pj.juav;

import papabench.core.autopilot.data.Position3D;
import papabench.core.autopilot.devices.GPSDevice;
import papabench.core.autopilot.devices.IRDevice;
import papabench.core.autopilot.modules.AutopilotModule;
import papabench.core.autopilot.modules.Navigator;
import papabench.core.commons.conf.RadioConf;
import papabench.core.commons.data.FlightPlan;
import papabench.core.fbw.modules.FBWModule;
import papabench.core.simulator.model.FlightModel;
import papabench.core.simulator.model.State;
import papabench.core.utils.PPRZUtils;

public class MessageHelper {
	private static MessageHelper instance = null;
	private static boolean isInitialized = false;

	private AutopilotModule autopilotModule;
	private FlightModel flightModel;
	private FBWModule fbwModule;
	private FlightPlan flightPlan;
	CommChannel channels;
	private int wayPointCounter = 0;
	private int wayPointId = 0;

	private MessageHelper() {
		channels = new CommChannelPJ();
		// channels = new CommChannels();
		channels.init();
	}

	public static MessageHelper getInstance() {
		if (instance == null) {
			instance = new MessageHelper();
		} else if (!isInitialized) {
			try {
				throw new Exception(
						"MessageHelper have not been initialized...");
			} catch (Exception e) {
				e.printStackTrace();
			}
			return null;
		}
		return instance;
	}

	public void init(AutopilotModule autopilot, FBWModule fbw,
			FlightPlan flightPlan, FlightModel flightModel) {
		this.autopilotModule = autopilot;
		this.fbwModule = fbw;
		this.flightPlan = flightPlan;
		this.flightModel = flightModel;
		this.isInitialized = true;

	}

	public CommChannel getCommChannel() {
		return channels;
	}

	public int getPPZMode() {
		int mode = 0;
		switch (autopilotModule.getAutopilotMode()) {
		case MANUAL:
			mode = 0;
			break;
		case AUTO1:
			mode = 1;
			break;
		case AUTO2:
			mode = 2;
			break;
		case HOME:
			mode = 3;
			break;
		case NB:
			mode = 5;
			break;
		}
		return mode;
	}

	public int getVCtlMode() {
		int mode = 0;
		switch (autopilotModule.getVerticalFlightMode()) {
		case MANUAL:
			mode = 0;
			break;
		case AUTO_GAZ:
			mode = 1;
			break;
		case AUTO_CLIMB:
			mode = 2;
			break;
		case AUTO_ALTITUDE:
			mode = 3;
			break;
		case MODE_NB:
			mode = 4;
			break;
		}
		return mode;
	}

	public int getLatMode() {
		int mode = 0;
		switch (autopilotModule.getLateralFlightMode()) {
		case MANUAL:
			mode = 0;
			break;
		case ROLL_RATE:
			mode = 1;
			break;
		case ROLL:
			mode = 2;
			break;
		case COURSE:
			mode = 3;
			break;
		case NB:
			mode = 4;
			break;
		}
		return mode;
	}

	public int getFBWMode() {
		// TODO Unsure of the correct mapping from this to the C version ...
		return fbwModule.getFBWMode().getValue();
	}

	public State getState() {
		return flightModel.getState();
	}

	public int getVoltSupply() {
		return autopilotModule.getVoltSupply();
	}

	public int getGaz() {
		return autopilotModule.getGaz();
		// return autopilotModule.getNavigator().getDesiredGaz();
		// return autopilotModule.getGaz();
	}

	public Navigator getNavigator() {
		return autopilotModule.getNavigator();
	}

	public int getCurBlock() {
		return flightPlan.getCurrentBlockNumber();
	}

	public int getCurStage() {
		return flightPlan.getCurrentStageNumber();
	}

	public Position3D getWayPoint() {
		Position3D[] wayPoints = flightPlan.getWPosition3ds();
		Position3D p = wayPoints[wayPointCounter];
		wayPointId = wayPointCounter;
		if (++wayPointCounter == wayPoints.length)
			wayPointCounter = 0;
		return p;
	}

	public int getWayPintId() {
		return wayPointId;
	}

	public IRDevice getIRDevice() {
		return autopilotModule.getIRDevice();
	}

	public GPSDevice getGPSDevice() {
		return autopilotModule.getGPSDevice();
	}

	public int getRoll() {
		return (int) (autopilotModule.getRoll() * RadioConf.MAX_PPRZ);
		// return (int) (autopilotModule.getNavigator().getDesiredRoll() *
		// RadioConf.MAX_PPRZ);
		// return (int) autopilotModule.getRoll();
	}

	public int getPitch() {
		return (int) (autopilotModule.getPitch() * RadioConf.MAX_PPRZ);
		// return (int) (autopilotModule.getNavigator().getDesiredPitch() *
		// RadioConf.MAX_PPRZ);
		// return (int) autopilotModule.getPitch();
	}

	/**
	 * Call to start the launch process for the autopilot
	 */
	public void launch() {
		autopilotModule.setLaunched(true);
	}

}
