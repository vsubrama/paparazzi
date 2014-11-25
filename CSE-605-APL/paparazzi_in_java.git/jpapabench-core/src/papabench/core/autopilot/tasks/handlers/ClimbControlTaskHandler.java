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
package papabench.core.autopilot.tasks.handlers;

import papabench.core.autopilot.conf.AutopilotMode;
import papabench.core.autopilot.conf.PapaBenchAutopilotConf.ClimbControlTaskConf;
import papabench.core.autopilot.conf.VerticalFlightMode;
import papabench.core.autopilot.conf.PapaBenchAutopilotConf.AltitudeControlTaskConf;
import papabench.core.autopilot.modules.AutopilotModule;
import papabench.core.autopilot.tasks.pids.ClimbPIDController;
import papabench.core.utils.LogUtils;

/**
 * Climb control task handler.
 * 
 * f = 40Hz
 * 
 * @author Michal Malohlava
 * 
 * @do not edit !
 *
 */
public class ClimbControlTaskHandler implements Runnable {
	
	private AutopilotModule autopilotModule;
	
	private ClimbPIDController pidController;
	private boolean isExe = false;
	
	public ClimbControlTaskHandler(AutopilotModule autopilotModule) {
		this.autopilotModule = autopilotModule;
		this.pidController = new ClimbPIDController();
	}

	public void run() {
//		if( !isExe ){
//			System.out.println("start: Thread_" + ClimbControlTaskConf.PRIORITY+" : " + Thread.currentThread().getName());
//			isExe = true;
//		}
		
		//System.out.println("ClimbControlTaskHandler start 222");
		AutopilotMode autopilotMode = autopilotModule.getAutopilotMode();
		VerticalFlightMode vfMode = autopilotModule.getVerticalFlightMode();
		
		if (autopilotMode == AutopilotMode.AUTO2
			|| autopilotMode == AutopilotMode.HOME) {
			//LogUtils.log(this, "----> vfMode :" + vfMode);
			if (vfMode == VerticalFlightMode.AUTO_CLIMB
				|| vfMode == VerticalFlightMode.AUTO_ALTITUDE
				|| vfMode == VerticalFlightMode.MODE_NB) {
				
				pidController.control(autopilotModule, autopilotModule.getEstimator(), autopilotModule.getNavigator());
			}
			
			if (vfMode == VerticalFlightMode.AUTO_GAZ) {
				autopilotModule.setGaz(autopilotModule.getNavigator().getDesiredGaz());
			}

			// switch off motor if the battery is to low
//			if (low_battery || (!estimator_flight_time && !launch)) {
//		   		 desired_gaz = 0.;
//		   	}  			
			
		//	System.out.println("... ClimbControlTaskHandler done 222");
		}
	}	
}
