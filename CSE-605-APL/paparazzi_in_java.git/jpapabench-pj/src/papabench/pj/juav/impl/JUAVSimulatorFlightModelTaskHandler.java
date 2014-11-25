/* $Id: SimulatorFlightModelTaskHandler.java 606 2010-11-02 19:52:33Z parizek $
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
package papabench.pj.juav.impl;

import static papabench.core.commons.conf.RadioConf.MAX_THRUST;
import static papabench.core.commons.conf.RadioConf.MIN_THRUST;

import java.util.Arrays;

import javax.realtime.RealtimeThread;

import com.fiji.fivm.Time;
import com.fiji.fivm.r1.fivmRuntime;

import papabench.core.autopilot.data.Attitude;
import papabench.core.autopilot.data.Position3D;
import papabench.core.autopilot.modules.AutopilotModule;
import papabench.core.fbw.conf.PapaBenchFBWConf.TestPPMTaskConf;
import papabench.core.fbw.modules.FBWModule;
import papabench.core.simulator.conf.SimulatorConf;
import papabench.core.simulator.conf.PapaBenchSimulatorConf.SimulatorFlightModelTaskConf;
import papabench.core.simulator.devices.SimulatedDevice;
import papabench.core.simulator.model.FlightModel;
import papabench.core.utils.LogUtils;
import papabench.core.utils.MathUtils;
import papabench.pj.juav.InputMsgInfo;
import papabench.pj.juav.Timing;
import papabench.pj.utility.GPSConversion;

/**
 * Main control task handler of simulator. It periodically updates simulator
 * flight model.
 * 
 * @author Michal Malohlava
 * 
 */
public class JUAVSimulatorFlightModelTaskHandler implements Runnable {

	private FlightModel flightModel;
	private AutopilotModule autopilotModule;
	private FBWModule fbwModule;
	private boolean isExe = false;
	private final boolean NPSSimData = true; // Should we process using NPS
												// data or internal state
												// update?

	public JUAVSimulatorFlightModelTaskHandler(FlightModel flightModel,
			AutopilotModule autopilotModule, FBWModule fbwModule) {
		this.flightModel = flightModel;
		this.autopilotModule = autopilotModule;
		this.fbwModule = fbwModule;
	}

	public void run() {

		Timing.getInstance().begin();

		if (NPSSimData) {

			SimulatedDevice irSensor = (SimulatedDevice) autopilotModule
					.getIRDevice();
			SimulatedDevice gpsSensor = (SimulatedDevice) autopilotModule
					.getGPSDevice();

			double[] sensorVectorIR = InputMsgInfo.getInstance().getSimIr();

			// Catch any poorly formed messages here
			int processIR = 1;
			for (int i = 0; i < 6; i++) {
				if (Double.isNaN(sensorVectorIR[i])) {
					processIR = 0;
					break;
				}
			}

			if (processIR > 0) {

				float phi = (float) sensorVectorIR[0];
				MathUtils.symmetricalLimiter(phi, SimulatorConf.MAX_PHI);
				float psi = (float) sensorVectorIR[1];
				float theta = (float) sensorVectorIR[2];

				Attitude irAttitude = new Attitude(phi, psi, theta);
				flightModel.getState().setAttitude(irAttitude);
				phi = (float) sensorVectorIR[3];
				MathUtils.symmetricalLimiter(phi, SimulatorConf.MAX_PHI_DOT);
				psi = (float) sensorVectorIR[4];
				theta = (float) sensorVectorIR[5];

				irSensor.setIrLeft((int) phi);
				irSensor.setIrFront((int) psi);

				Attitude rotSpeed = new Attitude(phi, psi, theta);
				flightModel.getState().setRotSpeed(rotSpeed);
				// System.out.println(irAttitude.toString());
				// System.out.println(rotSpeed.toString());

			}

			double[] sensorVectorGPS = InputMsgInfo.getInstance().getSimGPS();

			// Catch any poorly formed messages here
			int processGPS = 1;
			for (int i = 0; i < 8; i++) {
				if (Double.isNaN(sensorVectorGPS[i])) {
					processGPS = 0;
					break;
				}
			}

			if (processGPS > 0) {

				// TODO Find a better place for these things
				int NAVIGATION_REF_EAST = 360285;
				int NAVIGATION_REF_NORTH = 4813595;

				// TODO These x,y points are in RADS. Figure out what it should
				// be internally and then convert them.
				float lat = (float) sensorVectorGPS[0];
				lat = (float) Math.toDegrees(lat);
				float lon = (float) sensorVectorGPS[1];
				lon = (float) Math.toDegrees(lon);
				float z = (float) sensorVectorGPS[2];

				// Convert from
				float x = GPSConversion.getXfromLatLon(lat, lon);
				float y = GPSConversion.getYfromLatLon(lat, lon);

				Position3D p1 = GPSConversion.ecef_of_lla_f(lat, lon, z);
				Position3D p = new Position3D(x, y, z);
				System.out.println(p.toString());
				System.out.println(p1.toString());

				flightModel.getState().setPosition(p);
				float time = (float) sensorVectorGPS[6];
				flightModel.getState().setTime(time);
				flightModel.getState().setAirSpeed((float) sensorVectorGPS[4]);
				flightModel.getState().setTime((float) sensorVectorGPS[6]);

				// Allow the GPS data to be internally calculated with a minimal
				// set of input data
				// In this case, we will update the flightmodel position, and
				// current speed and allow the gps to determine where it is
				// Itself
				gpsSensor.update(flightModel);

				/*
				 * gpsSensor.setAltitude((float) sensorVectorGPS[2]);
				 * gpsSensor.setClimb((float) sensorVectorGPS[5]);
				 * gpsSensor.setCourse((float) sensorVectorGPS[3]);
				 * gpsSensor.setMode((int) sensorVectorGPS[7]);
				 * gpsSensor.setSpeed((float) sensorVectorGPS[4]);
				 * gpsSensor.setTow((float) sensorVectorGPS[6]);
				 * gpsSensor.setEast((float) sensorVectorGPS[0]);
				 * gpsSensor.setNorth((float) sensorVectorGPS[1]); // This is
				 * set to true after every internal simulator update // state,
				 * set to true for the NPS data
				 * gpsSensor.setPositionAvailable(true);
				 */
			}

			// Finally, process the setting (which includes the launch command)
			InputMsgInfo.getInstance().processSimSettings();

		} else {

			SimulatedDevice sensors = (SimulatedDevice) fbwModule
					.getServosController();

			sensors.update(flightModel);
			/*
			 * update all of the attributes on state
			 */
			flightModel.updateState();
		}

		Timing.getInstance().finish();
	}
}
