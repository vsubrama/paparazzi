/* $Id: NavigationTaskHandler.java 606 2010-11-02 19:52:33Z parizek $
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
package papabench.core.autopilot.tasks.handlers;

import papabench.core.autopilot.conf.AutopilotMode;
import papabench.core.autopilot.conf.LateralFlightMode;
import papabench.core.autopilot.conf.PapaBenchAutopilotConf.LinkFBWSendTaskConf;
import papabench.core.autopilot.conf.PapaBenchAutopilotConf.NavigationTaskConf;
import papabench.core.autopilot.modules.AutopilotModule;
import papabench.core.autopilot.tasks.pids.CoursePIDController;
import papabench.core.utils.LogUtils;

/**
 * Navigation task handler.
 * 
 * f = ? Hz
 * 
 * @author Michal Malohlava
 * 
 * @do not edit!
 */
// @SCJAllowed
public class NavigationTaskHandler implements Runnable {

	private AutopilotModule autopilotModule;

	private CoursePIDController coursePIDController;
	private boolean isExe= false;
	
	public NavigationTaskHandler(AutopilotModule autopilotModule) {
		this.autopilotModule = autopilotModule;

		this.coursePIDController = new CoursePIDController();
	}

	public void run() {
//		if( !isExe ){
//			System.out.println("start: Thread_" + NavigationTaskConf.PRIORITY+" : " + Thread.currentThread().getName());
//			isExe = true;
//		}
		
		// FIXME 4Hz is a frequency of this task -> update time every 4th call
		//System.out.println("NavigationTaskHandler start 444");
		autopilotModule.getEstimator().updateFlightTime();
		//System.out.println("\n============================ Navigation cycle: " + autopilotModule.getEstimator().getFlightTime());
		

		// FIXME following line should be in dedicated task: if
		// (gps_msg_received) => update state
		autopilotModule.getEstimator().updatePosition();

		autopilotModule.setLateralFlightMode(LateralFlightMode.COURSE);

		if (autopilotModule.getAutopilotMode() == AutopilotMode.HOME) {
			// nav_home()
		} else {
			// LogUtils.log(this,
			// "calling navigator to compute the navigation parameters, autopilot status:");
			// LogUtils.log(this, this.autopilotModule.toString());

			autopilotModule.getNavigator().autoNavigate();
		}

		// LogUtils.log(this, "course recomputation");
		courseComputation();

	    //System.out.println("Final autopilot status:");
		//System.out.println(this.autopilotModule.toString());
		
		//System.out.println(".... NavigationTaskHandler done 444");
	}

	protected void courseComputation() {
		AutopilotMode autopilotMode = autopilotModule.getAutopilotMode();
		if (autopilotMode == AutopilotMode.AUTO2 || autopilotMode == AutopilotMode.HOME) {

			LateralFlightMode lateralFlightMode = autopilotModule.getLateralFlightMode();
			if (lateralFlightMode == LateralFlightMode.COURSE || lateralFlightMode == LateralFlightMode.NB) {
				coursePIDController.control(autopilotModule, autopilotModule.getEstimator(), autopilotModule.getNavigator());
			}

			autopilotModule.setRoll(autopilotModule.getNavigator().getDesiredRoll());
		}
	}
}
