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
package papabench.pj.juav.impl;

import java.util.List;

import javax.realtime.RealtimeThread;

import papabench.core.autopilot.modules.AutopilotModule;
import papabench.core.simulator.devices.SimulatedDevice;
import papabench.pj.juav.CommChannel;
import papabench.pj.juav.CommChannels;
import papabench.pj.juav.InputMsgInfo;
import papabench.pj.juav.MessageHelper;
import papabench.pj.juav.OutputMsgInfo;

/**
 * Reports airplane state to ground central.
 * 
 * Notes: - called as 'downlink' in original PapaBench implementation
 * 
 * @author Michal Malohlava
 * 
 */
// @SCJAllowed
public class JUAVReportingTaskHandler implements Runnable {
	private boolean isExe = false;
	private AutopilotModule autopilotModule;
	int counter = -1;

	public JUAVReportingTaskHandler(AutopilotModule autopilotModule) {
		this.autopilotModule = autopilotModule;
	}

	public void run() {

		// System.out.println("ReportingTaskHandler start ... ReportingTaskHandler done 666");
		// This task has period 100ms => count the number of executions and send
		// different information according to a counter.
		// System.out.println( "Flight cycle: " +
		// autopilotModule.getEstimator().getFlightTime() +
		// " Block " +
		// autopilotModule.getNavigator().getFlightPlan().getCurrentBlockNumber()
		// +
		// " Stage " +
		// autopilotModule.getNavigator().getFlightPlan().getCurrentBlockNumber()
		// );

		if (counter != autopilotModule.getEstimator().getFlightTime()) {
			// System.out.println("Flight cycle: "
			// + autopilotModule.getEstimator().getFlightTime());
			// System.out.println("current: { "
			// + autopilotModule.getEstimator().getPosition() + " }");
			// System.out.println("desired: {"
			// + autopilotModule.getNavigator().getDesiredPosition().x
			// + ","
			// + autopilotModule.getNavigator().getDesiredPosition().y
			// + "," + autopilotModule.getNavigator().getDesiredCourse()
			// + "," + autopilotModule.getNavigator().getDesiredAltitude()
			// + " }");

			// SC Print out the position in a nice format
			// System.out.println(OutputMsgInfo.getInstance().generateGPS());

			counter = autopilotModule.getEstimator().getFlightTime();
		}

		// Data to be sent to the groundstation here
		CommChannel channels = MessageHelper.getInstance().getCommChannel();
		OutputMsgInfo outInfo = OutputMsgInfo.getInstance();
		List<String> msgList = outInfo.getOutputMsg();

		channels.sendOtherMsg(msgList);

		// send attitude (500ms)
		// send GPS position
		// send IR status
		// send ability to take-off
	}

	protected void sendGPS() {

	}

	protected void sendIRStatus() {
	}

	protected void sendTakeOff() {
	}
}
