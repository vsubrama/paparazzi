package papabench.pj.juav.impl;

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

import java.util.List;

import papabench.core.autopilot.conf.AutopilotMode;
import papabench.core.autopilot.conf.PapaBenchAutopilotConf.NavigationTaskConf;
import papabench.core.autopilot.conf.PapaBenchAutopilotConf.RadioControlTaskConf;
import papabench.core.autopilot.modules.AutopilotModule;
import papabench.core.autopilot.modules.LinkToFBW;
import papabench.core.commons.conf.RadioConf;
import papabench.core.commons.data.InterMCUMsg;
import papabench.core.utils.PPRZUtils;
import papabench.pj.juav.CommChannel;
import papabench.pj.juav.CommChannels;
import papabench.pj.juav.MessageHelper;
import papabench.pj.juav.OutputMsgInfo;

/**
 * Handler transfers commands to fly-by-wire unit.
 * 
 * f = ? Hz
 * 
 * @author Michal Malohlava
 * 
 */
// @SCJAllowed
public class JUAVRadioControlTaskHandler implements Runnable {

	private AutopilotModule autopilotModule;
	private boolean isExe = false;

	public JUAVRadioControlTaskHandler(AutopilotModule autopilotModule) {
		this.autopilotModule = autopilotModule;
	}

	public void run() {
		// if( !isExe ){
		// System.out.println("start: Thread_" +
		// RadioControlTaskConf.PRIORITY+" : " +
		// Thread.currentThread().getName());
		// isExe = true;
		// }

		LinkToFBW linkToFBW = autopilotModule.getLinkToFBW();

		InterMCUMsg msg = linkToFBW.getMessageFromFBW();

		AutopilotMode autopilotMode = autopilotModule.getAutopilotMode();// just
																			// auto2
		// LogUtils.log(this, "msg :"+ msg);
		// message is totally received
		if (msg.isValid()) {
			boolean modeChanged = false;

			if (msg.isRadioReallyLost()
					&& (autopilotMode == AutopilotMode.AUTO1 || autopilotMode == AutopilotMode.MANUAL)) {
				autopilotModule.setAutopilotMode(AutopilotMode.HOME);
				autopilotMode = AutopilotMode.HOME;
				modeChanged = true;
			}

			if (msg.isAveragedChannelsSent()) {
				// TODO setup modes, currently we are interested in in automatic
				// mode AUTO2
			}

			if (modeChanged) {
				// TODO sent telemetry data to the ground
			}

			if (autopilotMode == AutopilotMode.AUTO1) {
				autopilotModule.setRoll(PPRZUtils.floatOfPPRZ(
						msg.radioCommands.getRoll(), 0.0f, -0.6f));
				autopilotModule.setPitch(PPRZUtils.floatOfPPRZ(
						msg.radioCommands.getPitch(), 0.0f, 0.5f));
			}

			if (autopilotMode == AutopilotMode.AUTO1
					|| autopilotMode == AutopilotMode.MANUAL) {
				autopilotModule.setGaz(msg.radioCommands.getThrottle());
			}

			autopilotModule.setMC1PpmCpt(msg.ppmCpt);
			autopilotModule.setVoltSupply(msg.voltSupply);

			if (autopilotModule.getEstimator().getFlightTime() == 0) {
				// TODO FIXME here we should put ground_calibration !
				if (autopilotMode == AutopilotMode.AUTO2
						&& msg.radioCommands.getThrottle() > RadioConf.GAZ_THRESHOLD_TAKEOFF) {
					autopilotModule.setLaunched(true);
				}
			}
		}

		CommChannel channels = MessageHelper.getInstance().getCommChannel();
		OutputMsgInfo outInfo = OutputMsgInfo.getInstance();
		String cmd = outInfo.generateCommand();
		channels.sendCommandMsg(cmd);

	}
}
