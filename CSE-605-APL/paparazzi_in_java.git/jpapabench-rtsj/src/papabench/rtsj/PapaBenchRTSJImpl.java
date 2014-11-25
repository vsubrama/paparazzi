/* $Id: PapaBenchRTSJImpl.java 616 2010-11-07 11:58:37Z parizek $
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

import javax.realtime.AbsoluteTime;
import javax.realtime.PeriodicParameters;
import javax.realtime.PriorityParameters;
import javax.realtime.RelativeTime;

import papabench.core.autopilot.conf.PapaBenchAutopilotConf.AltitudeControlTaskConf;
import papabench.core.autopilot.conf.PapaBenchAutopilotConf.ClimbControlTaskConf;
import papabench.core.autopilot.conf.PapaBenchAutopilotConf.LinkFBWSendTaskConf;
import papabench.core.autopilot.conf.PapaBenchAutopilotConf.NavigationTaskConf;
import papabench.core.autopilot.conf.PapaBenchAutopilotConf.RadioControlTaskConf;
import papabench.core.autopilot.conf.PapaBenchAutopilotConf.ReportingTaskConf;
import papabench.core.autopilot.conf.PapaBenchAutopilotConf.StabilizationTaskConf;
import papabench.core.autopilot.modules.AutopilotModule;
import papabench.core.autopilot.tasks.handlers.AltitudeControlTaskHandler;
import papabench.core.autopilot.tasks.handlers.ClimbControlTaskHandler;
import papabench.core.autopilot.tasks.handlers.LinkFBWSendTaskHandler;
import papabench.core.autopilot.tasks.handlers.NavigationTaskHandler;
import papabench.core.autopilot.tasks.handlers.RadioControlTaskHandler;
import papabench.core.autopilot.tasks.handlers.ReportingTaskHandler;
import papabench.core.autopilot.tasks.handlers.StabilizationTaskHandler;
import papabench.core.bus.SPIBusChannel;
import papabench.core.commons.data.FlightPlan;
import papabench.core.fbw.conf.PapaBenchFBWConf.CheckFailsafeTaskConf;
import papabench.core.fbw.conf.PapaBenchFBWConf.CheckMega128ValuesTaskConf;
import papabench.core.fbw.conf.PapaBenchFBWConf.SendDataToAutopilotTaskConf;
import papabench.core.fbw.conf.PapaBenchFBWConf.TestPPMTaskConf;
import papabench.core.fbw.modules.FBWModule;
import papabench.core.fbw.tasks.handlers.CheckFailsafeTaskHandler;
import papabench.core.fbw.tasks.handlers.CheckMega128ValuesTaskHandler;
import papabench.core.fbw.tasks.handlers.SendDataToAutopilotTaskHandler;
import papabench.core.fbw.tasks.handlers.TestPPMTaskHandler;
import papabench.core.simulator.conf.PapaBenchSimulatorConf.SimulatorFlightModelTaskConf;
import papabench.core.simulator.conf.PapaBenchSimulatorConf.SimulatorGPSTaskConf;
import papabench.core.simulator.conf.PapaBenchSimulatorConf.SimulatorIRTaskConf;
import papabench.core.simulator.model.FlightModel;
import papabench.core.simulator.tasks.handlers.SimulatorFlightModelTaskHandler;
import papabench.core.simulator.tasks.handlers.SimulatorGPSTaskHandler;
import papabench.core.simulator.tasks.handlers.SimulatorIRTaskHandler;
import papabench.rtsj.commons.tasks.RealtimePeriodicTask;

/**
 * RTSJ-based implementation of PapaBench.
 * 
 * @author Michal Malohlava
 *
 */
public class PapaBenchRTSJImpl implements RTSJPapaBench {
	
	private AutopilotModule autopilotModule;
	private FBWModule fbwModule;	
	private FlightPlan flightPlan;
	
	private static final int AUTOPILOT_TASKS_COUNT = 7;
	private static final int FBW_TASKS_COUNT = 4;
	private static final int SIMULATOR_TASKS_COUNT = 3;
	
	private RealtimePeriodicTask[] autopilotTasks;
	private RealtimePeriodicTask[] fbwTasks;
	private RealtimePeriodicTask[] simulatorTasks;

	public AutopilotModule getAutopilotModule() {
		return autopilotModule;
	}

	public FBWModule getFBWModule() {
		return fbwModule;
	}

	public void setFlightPlan(FlightPlan flightPlan) {
		this.flightPlan = flightPlan;		
	}

	public void init() {	
		// Allocate and initialize global objects: 
		//  - MC0 - autopilot
		autopilotModule = PapaBenchRTSJFactory.createAutopilotModule(this);
				
		//  - MC1 - FBW
		fbwModule = PapaBenchRTSJFactory.createFBWModule();		
		
		// Create & configure SPIBusChannel and connect both PapaBench modules
		SPIBusChannel spiBusChannel = PapaBenchRTSJFactory.createSPIBusChannel();
		spiBusChannel.init();
		autopilotModule.setSPIBus(spiBusChannel.getMasterEnd()); // = MC0: SPI master mode
		fbwModule.setSPIBus(spiBusChannel.getSlaveEnd()); // = MC1: SPI slave mode
		
		// setup flight plan
		assert(this.flightPlan != null);
		autopilotModule.getNavigator().setFlightPlan(this.flightPlan);
		
		// initialize both modules - if the modules are badly initialized the runtime exception is thrown
		autopilotModule.init();
		fbwModule.init();
		
		// Register interrupt handlers
		/*
		 * TODO
		 */
		
		// Register period threads
		createAutopilotTasks(autopilotModule);
		createFBWTasks(fbwModule);
		
		// Create a flight simulator
		FlightModel flightModel = PapaBenchRTSJFactory.createSimulator();
		flightModel.init();
		
		// Register simulator tasks
		createSimulatorTasks(flightModel, autopilotModule, fbwModule);			

	}
	
	protected void createAutopilotTasks(AutopilotModule autopilotModule) {
		autopilotTasks = new RealtimePeriodicTask[AUTOPILOT_TASKS_COUNT];
		autopilotTasks[0] = new RealtimePeriodicTask(new AltitudeControlTaskHandler(autopilotModule));//AltitudeControlTaskConf.PRIORITY, AltitudeControlTaskConf.RELEASE_MS, AltitudeControlTaskConf.PERIOD_MS);
		autopilotTasks[0].setSchedulingParameters(new PriorityParameters(AltitudeControlTaskConf.PRIORITY));
		autopilotTasks[0].setReleaseParameters(new PeriodicParameters(new AbsoluteTime(AltitudeControlTaskConf.RELEASE_MS, 0), new RelativeTime(AltitudeControlTaskConf.PERIOD_MS,0), null, null, null, null));
		
		autopilotTasks[1] = new RealtimePeriodicTask(new ClimbControlTaskHandler(autopilotModule)); //ClimbControlTaskConf.PRIORITY, ClimbControlTaskConf.RELEASE_MS, ClimbControlTaskConf.PERIOD_MS);
		autopilotTasks[1].setSchedulingParameters(new PriorityParameters(ClimbControlTaskConf.PRIORITY));
		autopilotTasks[1].setReleaseParameters(new PeriodicParameters(new AbsoluteTime(ClimbControlTaskConf.RELEASE_MS, 0), new RelativeTime(ClimbControlTaskConf.PERIOD_MS,0), null, null, null, null));
		
		autopilotTasks[2] = new RealtimePeriodicTask(new LinkFBWSendTaskHandler(autopilotModule)); //LinkFBWSendTaskConf.PRIORITY, LinkFBWSendTaskConf.RELEASE_MS, LinkFBWSendTaskConf.PERIOD_MS);
		autopilotTasks[2].setSchedulingParameters(new PriorityParameters(LinkFBWSendTaskConf.PRIORITY));
		autopilotTasks[2].setReleaseParameters(new PeriodicParameters(new AbsoluteTime(LinkFBWSendTaskConf.RELEASE_MS, 0), new RelativeTime(LinkFBWSendTaskConf.PERIOD_MS,0), null, null, null, null));
		
		autopilotTasks[3] = new RealtimePeriodicTask(new NavigationTaskHandler(autopilotModule)); //NavigationTaskConf.PRIORITY, NavigationTaskConf.RELEASE_MS, NavigationTaskConf.PERIOD_MS);
		autopilotTasks[3].setSchedulingParameters(new PriorityParameters(NavigationTaskConf.PRIORITY));
		autopilotTasks[3].setReleaseParameters(new PeriodicParameters(new AbsoluteTime(NavigationTaskConf.RELEASE_MS, 0), new RelativeTime(NavigationTaskConf.PERIOD_MS,0), null, null, null, null));
		
		autopilotTasks[4] = new RealtimePeriodicTask(new RadioControlTaskHandler(autopilotModule)); //RadioControlTaskConf.PRIORITY, RadioControlTaskConf.RELEASE_MS, RadioControlTaskConf.PERIOD_MS);
		autopilotTasks[4].setSchedulingParameters(new PriorityParameters(RadioControlTaskConf.PRIORITY));
		autopilotTasks[4].setReleaseParameters(new PeriodicParameters(new AbsoluteTime(RadioControlTaskConf.RELEASE_MS, 0), new RelativeTime(RadioControlTaskConf.PERIOD_MS,0), null, null, null, null));
		
		autopilotTasks[5] = new RealtimePeriodicTask(new ReportingTaskHandler(autopilotModule)); // ReportingTaskConf.PRIORITY, ReportingTaskConf.RELEASE_MS, ReportingTaskConf.PERIOD_MS);
		autopilotTasks[5].setSchedulingParameters(new PriorityParameters(ReportingTaskConf.PRIORITY));
		autopilotTasks[5].setReleaseParameters(new PeriodicParameters(new AbsoluteTime(ReportingTaskConf.RELEASE_MS, 0), new RelativeTime(ReportingTaskConf.PERIOD_MS,0), null, null, null, null));
		// StabilizationTask allocates messages which are sent to FBW unit -> allocate them in scope memory
		autopilotTasks[6] = new RealtimePeriodicTask(new StabilizationTaskHandler(autopilotModule));// StabilizationTaskConf.PRIORITY, StabilizationTaskConf.RELEASE_MS, StabilizationTaskConf.PERIOD_MS);
		autopilotTasks[6].setSchedulingParameters(new PriorityParameters(StabilizationTaskConf.PRIORITY));
		autopilotTasks[6].setReleaseParameters(new PeriodicParameters(new AbsoluteTime(StabilizationTaskConf.RELEASE_MS, 0), new RelativeTime(StabilizationTaskConf.PERIOD_MS,0), null, null, null, null));
		//autopilotTasks[6] = new ScopedMemoryRealtimePeriodicTask(new StabilizationTaskHandler(autopilotModule), StabilizationTaskConf.PRIORITY, StabilizationTaskConf.RELEASE_MS, StabilizationTaskConf.PERIOD_MS);		
	}
	
	
	protected void createFBWTasks(FBWModule fbwModule) {
		fbwTasks = new RealtimePeriodicTask[FBW_TASKS_COUNT];
		fbwTasks[0] = new RealtimePeriodicTask(new CheckFailsafeTaskHandler(fbwModule)); // CheckFailsafeTaskConf.PRIORITY, CheckFailsafeTaskConf.RELEASE_MS, CheckFailsafeTaskConf.PERIOD_MS);
		fbwTasks[0].setSchedulingParameters(new PriorityParameters(CheckFailsafeTaskConf.PRIORITY));
		fbwTasks[0].setReleaseParameters(new PeriodicParameters(new AbsoluteTime(CheckFailsafeTaskConf.RELEASE_MS, 0), new RelativeTime(CheckFailsafeTaskConf.PERIOD_MS,0), null, null, null, null));
		
		fbwTasks[1] = new RealtimePeriodicTask(new CheckMega128ValuesTaskHandler(fbwModule));// CheckMega128ValuesTaskConf.PRIORITY, CheckMega128ValuesTaskConf.RELEASE_MS, CheckMega128ValuesTaskConf.PERIOD_MS);
		fbwTasks[1].setSchedulingParameters(new PriorityParameters(CheckMega128ValuesTaskConf.PRIORITY));
		fbwTasks[1].setReleaseParameters(new PeriodicParameters(new AbsoluteTime(CheckMega128ValuesTaskConf.RELEASE_MS, 0), new RelativeTime(CheckMega128ValuesTaskConf.PERIOD_MS,0), null, null, null, null));
		//fbwTasks[1] = new ScopedMemoryRealtimePeriodicTask(new CheckMega128ValuesTaskHandler(fbwModule), CheckMega128ValuesTaskConf.PRIORITY, CheckMega128ValuesTaskConf.RELEASE_MS, CheckMega128ValuesTaskConf.PERIOD_MS);
		// the following task use scope memory to allocate data
		fbwTasks[2] = new RealtimePeriodicTask(new SendDataToAutopilotTaskHandler(fbwModule));// SendDataToAutopilotTaskConf.PRIORITY, SendDataToAutopilotTaskConf.RELEASE_MS, SendDataToAutopilotTaskConf.PERIOD_MS);
		fbwTasks[2].setSchedulingParameters(new PriorityParameters(SendDataToAutopilotTaskConf.PRIORITY));
		fbwTasks[2].setReleaseParameters(new PeriodicParameters(new AbsoluteTime(SendDataToAutopilotTaskConf.RELEASE_MS, 0), new RelativeTime(SendDataToAutopilotTaskConf.PERIOD_MS,0), null, null, null, null));
		//fbwTasks[2] = new ScopedMemoryRealtimePeriodicTask(new SendDataToAutopilotTaskHandler(fbwModule), SendDataToAutopilotTaskConf.PRIORITY, SendDataToAutopilotTaskConf.RELEASE_MS, SendDataToAutopilotTaskConf.PERIOD_MS);
		fbwTasks[3] = new RealtimePeriodicTask(new TestPPMTaskHandler(fbwModule)); //, TestPPMTaskConf.PRIORITY, TestPPMTaskConf.RELEASE_MS, TestPPMTaskConf.PERIOD_MS);
		fbwTasks[3].setSchedulingParameters(new PriorityParameters(TestPPMTaskConf.PRIORITY));
		fbwTasks[3].setReleaseParameters(new PeriodicParameters(new AbsoluteTime(TestPPMTaskConf.RELEASE_MS, 0), new RelativeTime(TestPPMTaskConf.PERIOD_MS,0), null, null, null, null));
	}
	
	
	protected void createSimulatorTasks(FlightModel flightModel, AutopilotModule autopilotModule, FBWModule fbwModule) {
		simulatorTasks = new RealtimePeriodicTask[SIMULATOR_TASKS_COUNT];
		simulatorTasks[0] = new RealtimePeriodicTask(new SimulatorFlightModelTaskHandler(flightModel,autopilotModule,fbwModule)); //, SimulatorFlightModelTaskConf.PRIORITY, SimulatorFlightModelTaskConf.RELEASE_MS, SimulatorFlightModelTaskConf.PERIOD_MS);
		simulatorTasks[0].setSchedulingParameters(new PriorityParameters(SimulatorFlightModelTaskConf.PRIORITY));
		simulatorTasks[0].setReleaseParameters(new PeriodicParameters(new AbsoluteTime(SimulatorFlightModelTaskConf.RELEASE_MS, 0), new RelativeTime(SimulatorFlightModelTaskConf.PERIOD_MS,0), null, null, null, null));
		
		simulatorTasks[1] = new RealtimePeriodicTask(new SimulatorGPSTaskHandler(flightModel,autopilotModule)); //, SimulatorGPSTaskConf.PRIORITY, SimulatorGPSTaskConf.RELEASE_MS, SimulatorGPSTaskConf.PERIOD_MS);
		simulatorTasks[1].setSchedulingParameters(new PriorityParameters(SimulatorGPSTaskConf.PRIORITY));
		simulatorTasks[0].setReleaseParameters(new PeriodicParameters(new AbsoluteTime(SimulatorGPSTaskConf.RELEASE_MS, 0), new RelativeTime(SimulatorGPSTaskConf.PERIOD_MS,0), null, null, null, null));
		
		simulatorTasks[2] = new RealtimePeriodicTask(new SimulatorIRTaskHandler(flightModel,autopilotModule)); //, SimulatorIRTaskConf.PRIORITY, SimulatorIRTaskConf.RELEASE_MS, SimulatorIRTaskConf.PERIOD_MS);		
		simulatorTasks[2].setSchedulingParameters(new PriorityParameters(SimulatorIRTaskConf.PRIORITY));
		simulatorTasks[0].setReleaseParameters(new PeriodicParameters(new AbsoluteTime(SimulatorIRTaskConf.RELEASE_MS, 0), new RelativeTime(SimulatorIRTaskConf.PERIOD_MS,0), null, null, null, null));
	}

	public void start() {
		// FIXME put here rendez-vous for all tasks		
		for (int i = 0; i < SIMULATOR_TASKS_COUNT; i++) {
			simulatorTasks[i].start();
		}		
		for (int i = 0; i < FBW_TASKS_COUNT; i++) {
			fbwTasks[i].start();
		}
		for (int i = 0; i < AUTOPILOT_TASKS_COUNT; i++) {
			autopilotTasks[i].start();
		}		
	}

	public void shutdown() {
		// FIXME the task should be properly shutdown
		
		System.exit(0);		
	}
	
	public class MissDeadlineHandler implements Runnable{
		String name;
		boolean isMissing;
		public MissDeadlineHandler(String name){
			this.name = name;
			isMissing = false;
		}
		
		@Override
		public void run() {
			if( !isMissing ){
				isMissing = true;
				System.out.println(name + "missing deadline");
			}
		}
		
	}
}
