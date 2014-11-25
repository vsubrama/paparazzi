/* $Id: PapaBenchRTSJApplication.java 616 2010-11-07 11:58:37Z parizek $
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
package papabench.rtsj;

import javax.realtime.RealtimeThread;

import papabench.core.AbstractPapaBenchApplication;
import papabench.core.commons.data.FlightPlan;
import papabench.core.commons.data.impl.*;
import papabench.rtsj.utils.ParametersFactory;

/**
 * PapaBench RTSJ application launcher class.
 * 
 * @author Michal Malohlava
 *
 */
public class PapaBenchRTSJApplication extends AbstractPapaBenchApplication {
	
	public static void main(String[] args) {
		RealtimeThread launchThread = new RealtimeThread() {
			public void run() {
				RTSJPapaBench papaBench = new PapaBenchRTSJImpl();						
				FlightPlan plan = new RoundTripFlightPlan();
				System.out.println("Flight plan: " + plan.getName());
				
				papaBench.setFlightPlan(plan);
				papaBench.init();
				
				papaBench.start();
				
//			ImmortalMemory immortalMemory = ImmortalMemory.instance();
//				
//				immortalMemory.enter(new Runnable() {
//					
//					public void run() {
//						RTSJPapaBench papaBench = new PapaBenchRTSJImpl();						
//						FlightPlan plan = new Simple07FlightPlan();
//						System.out.println("Flight plan: " + plan.getName());
//						
//						papaBench.setFlightPlan(plan);
//						papaBench.init();
//						
//						papaBench.start();
//					}
//				});				
			};
		};
		//launchThread.
		launchThread.setSchedulingParameters(ParametersFactory.getPriorityParameters(11));
		launchThread.start();
		
		try {
			launchThread.join();
		} catch (InterruptedException e) {			
			e.printStackTrace();
		}
	}
}
