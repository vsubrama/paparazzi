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
package papabench.scj.schedule;

import java.io.PrintStream;

import javax.realtime.RelativeTime;
import javax.safetycritical.CyclicSchedule;
import javax.safetycritical.PeriodicEventHandler;

import papabench.core.autopilot.conf.PapaBenchAutopilotConf.AltitudeControlTaskConf;
import papabench.core.autopilot.conf.PapaBenchAutopilotConf.ClimbControlTaskConf;
import papabench.core.autopilot.conf.PapaBenchAutopilotConf.NavigationTaskConf;
import papabench.core.autopilot.conf.PapaBenchAutopilotConf.RadioControlTaskConf;
import papabench.core.autopilot.conf.PapaBenchAutopilotConf.StabilizationTaskConf;
import papabench.core.commons.conf.CommonTaskConfiguration;
import papabench.core.fbw.conf.PapaBenchFBWConf.CheckFailsafeTaskConf;
import papabench.core.fbw.conf.PapaBenchFBWConf.CheckMega128ValuesTaskConf;
import papabench.core.fbw.conf.PapaBenchFBWConf.SendDataToAutopilotTaskConf;
import papabench.core.fbw.conf.PapaBenchFBWConf.TestPPMTaskConf;
import papabench.core.simulator.conf.PapaBenchSimulatorConf.SimulatorFlightModelTaskConf;
import papabench.core.simulator.conf.PapaBenchSimulatorConf.SimulatorGPSTaskConf;
import papabench.core.simulator.conf.PapaBenchSimulatorConf.SimulatorIRTaskConf;
import papabench.scj.utils.ParametersFactory;

/**
 * PapaBench specific schedule.
 * 
 * @see PapabenchCyclicSchedule
 * 
 * Differences: 
 *  - LinkFBWSendTask deleted (it was 
 * 
 * FIXME extract shared parts with {@link PapabenchCyclicSchedule} to abstract factory.
 * 
 * @author Michal Malohlava
 *
 */
//@SCJAllowed
public class SimplifiedPapabenchCyclicSchedule extends CyclicSchedule {
	
	// Minor cycle takes 25ms 
	// Each task period should be a multiple of this minor 
	public static final int MINOR_CYCLE_PERIOD = 25; // ms	
	public static final int MAJOR_CYCLE_PERIOD = 250; // ms
	public static final int NUMBER_OF_FRAMES = MAJOR_CYCLE_PERIOD / MINOR_CYCLE_PERIOD;
	
	// it would be nicer to generate it in the code below, however we can generate schedule by hand
	// tasks in array for each time frame should be sorted according to PRIORITY
	// WARNING !!!if you add a new task then reflects dependencies between tasks !!!
	public static final String[][] TIMELINE_SCHEDULE = {
		// 0 ms		
		{ SimulatorGPSTaskConf.NAME, SimulatorIRTaskConf.NAME, TestPPMTaskConf.NAME, SendDataToAutopilotTaskConf.NAME, RadioControlTaskConf.NAME, NavigationTaskConf.NAME, AltitudeControlTaskConf.NAME, ClimbControlTaskConf.NAME, StabilizationTaskConf.NAME, CheckFailsafeTaskConf.NAME, CheckMega128ValuesTaskConf.NAME, SimulatorFlightModelTaskConf.NAME }, // 250ms
		// 25ms
		{ TestPPMTaskConf.NAME, SendDataToAutopilotTaskConf.NAME, RadioControlTaskConf.NAME, SimulatorFlightModelTaskConf.NAME }, // 25ms
		// 50ms
		{ SimulatorIRTaskConf.NAME, TestPPMTaskConf.NAME, SendDataToAutopilotTaskConf.NAME, RadioControlTaskConf.NAME, StabilizationTaskConf.NAME, CheckFailsafeTaskConf.NAME, CheckMega128ValuesTaskConf.NAME, SimulatorFlightModelTaskConf.NAME }, // 50ms
		// 75 ms
		{ TestPPMTaskConf.NAME, SendDataToAutopilotTaskConf.NAME, RadioControlTaskConf.NAME, SimulatorFlightModelTaskConf.NAME }, // 75ms
		// 100 ms
		{ SimulatorIRTaskConf.NAME, TestPPMTaskConf.NAME, SendDataToAutopilotTaskConf.NAME, RadioControlTaskConf.NAME, StabilizationTaskConf.NAME,  CheckFailsafeTaskConf.NAME, CheckMega128ValuesTaskConf.NAME, SimulatorFlightModelTaskConf.NAME  }, // 100ms
		// 125 ms
		{ TestPPMTaskConf.NAME, SendDataToAutopilotTaskConf.NAME, RadioControlTaskConf.NAME, SimulatorFlightModelTaskConf.NAME }, // 125ms
		// 150 ms
		{ SimulatorIRTaskConf.NAME, TestPPMTaskConf.NAME, SendDataToAutopilotTaskConf.NAME, RadioControlTaskConf.NAME, StabilizationTaskConf.NAME, CheckFailsafeTaskConf.NAME, CheckMega128ValuesTaskConf.NAME, SimulatorFlightModelTaskConf.NAME  }, // 150ms
		// 175 ms
		{ TestPPMTaskConf.NAME, SendDataToAutopilotTaskConf.NAME, RadioControlTaskConf.NAME, SimulatorFlightModelTaskConf.NAME }, // 175ms
		// 200 ms
		{ SimulatorIRTaskConf.NAME, TestPPMTaskConf.NAME, SendDataToAutopilotTaskConf.NAME, RadioControlTaskConf.NAME, StabilizationTaskConf.NAME, CheckFailsafeTaskConf.NAME, CheckMega128ValuesTaskConf.NAME, SimulatorFlightModelTaskConf.NAME  }, // 200ms
		// 225 ms
		{ TestPPMTaskConf.NAME, SendDataToAutopilotTaskConf.NAME, RadioControlTaskConf.NAME, SimulatorFlightModelTaskConf.NAME }, // 225ms
	};
	
	/**
	 * Factory method to obtain a schedule for given periodic event handlers
	 *  
	 * @param peh
	 * @return
	 */
	//@SCJRestricted(Restric.INITIALIZATION)
//	@SCJAllowed
	public static SimplifiedPapabenchCyclicSchedule generateSchedule(PeriodicEventHandler[] pehs) {
		// CHECKME: is it ok to allocate schedule Frame here (MissionMemory scope) ?
		Frame[] frames = new Frame[NUMBER_OF_FRAMES];
		
		int frameStart = 0;
		for(int i = 0; i < NUMBER_OF_FRAMES; i++) {
			int pehsInFrame = TIMELINE_SCHEDULE[i].length;
			PeriodicEventHandler[] framePehs = new PeriodicEventHandler[pehsInFrame];
			
			for(int  j = 0; j < pehsInFrame; j++) {
				framePehs[j] = getPEHByName(pehs, TIMELINE_SCHEDULE[i][j]);				
			}
						
			RelativeTime offset = ParametersFactory.getRelativeTime(frameStart);			
			frames[i] = new Frame(offset, framePehs);			
			
			// next frame
			frameStart += MINOR_CYCLE_PERIOD;
		}
		
		// DEBUG
		printSchedule(frames, System.err);
				
		// compute number of frames and 
		return new SimplifiedPapabenchCyclicSchedule(frames);
	}
	
	protected static PeriodicEventHandler getPEHByName(PeriodicEventHandler[] pehs, String name) {
		for(int i = 0; i < pehs.length; i++) {
			// get a PEH by the given name
			// all PapaBench PEH has to implement interface CommonTaskConfiguration, else there is a bug in implementation
			if (((CommonTaskConfiguration) pehs[i]).getTaskName().equals(name)) {
				return pehs[i];
			}
		}
		
		throw new IllegalArgumentException("Schedule preparation failed - cannot find periodic event handler for name " + name);
	}
	
	public static void printSchedule(Frame[] frames, PrintStream os) {
		StringBuilder builder = new StringBuilder(frames.length * 50);
		builder.append("Schedule: \n");
		for (int i = 0; i < frames.length; i++) {
			Frame frame = frames[i];
			builder.append("\tFrame #").append(i).append(":").append(frame.getDuration().getMilliseconds()).append("ms\n");
			builder.append("\t\ttasks [ ");
			PeriodicEventHandler[] handlers = frame.getHandlers();
			for (int j = 0; j < handlers.length; j++) {
				
				builder.append(((CommonTaskConfiguration) handlers[j]).getTaskName()).append(" ");				
			}
			builder.append("]\n");
			
		}
		
		os.print(builder.toString());
	}
	
//	@SCJAllowed
	public SimplifiedPapabenchCyclicSchedule(Frame[] frames) {
		super(frames);		
	}	
}
