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
package papabench.scj;

import papabench.core.PapaBench;
import papabench.core.autopilot.conf.PapaBenchAutopilotConf.AltitudeControlTaskConf;
import papabench.core.autopilot.conf.PapaBenchAutopilotConf.ClimbControlTaskConf;
import papabench.core.autopilot.conf.PapaBenchAutopilotConf.LinkFBWSendTaskConf;
import papabench.core.autopilot.conf.PapaBenchAutopilotConf.NavigationTaskConf;
import papabench.core.autopilot.conf.PapaBenchAutopilotConf.RadioControlTaskConf;
import papabench.core.autopilot.conf.PapaBenchAutopilotConf.ReportingTaskConf;
import papabench.core.autopilot.conf.PapaBenchAutopilotConf.StabilizationTaskConf;
import papabench.core.autopilot.modules.AutopilotModule;
import papabench.core.autopilot.modules.impl.AutopilotModuleImpl;
import papabench.core.autopilot.modules.impl.EstimatorModuleImpl;
import papabench.core.autopilot.modules.impl.LinkToFBWImpl;
import papabench.core.autopilot.modules.impl.NavigatorImpl;
import papabench.core.autopilot.tasks.handlers.AltitudeControlTaskHandler;
import papabench.core.autopilot.tasks.handlers.ClimbControlTaskHandler;
import papabench.core.autopilot.tasks.handlers.LinkFBWSendTaskHandler;
import papabench.core.autopilot.tasks.handlers.NavigationTaskHandler;
import papabench.core.autopilot.tasks.handlers.RadioControlTaskHandler;
import papabench.core.autopilot.tasks.handlers.ReportingTaskHandler;
import papabench.core.autopilot.tasks.handlers.StabilizationTaskHandler;
import papabench.core.bus.SPIBusChannel;
import papabench.core.bus.impl.SPIBusChannelImpl;
import papabench.core.fbw.conf.PapaBenchFBWConf.CheckFailsafeTaskConf;
import papabench.core.fbw.conf.PapaBenchFBWConf.CheckMega128ValuesTaskConf;
import papabench.core.fbw.conf.PapaBenchFBWConf.SendDataToAutopilotTaskConf;
import papabench.core.fbw.conf.PapaBenchFBWConf.TestPPMTaskConf;
import papabench.core.fbw.devices.impl.PPMDeviceImpl;
import papabench.core.fbw.modules.FBWModule;
import papabench.core.fbw.modules.impl.FBWModuleImpl;
import papabench.core.fbw.modules.impl.LinkToAutopilotImpl;
import papabench.core.fbw.tasks.handlers.CheckFailsafeTaskHandler;
import papabench.core.fbw.tasks.handlers.CheckMega128ValuesTaskHandler;
import papabench.core.fbw.tasks.handlers.SendDataToAutopilotTaskHandler;
import papabench.core.fbw.tasks.handlers.TestPPMTaskHandler;
import papabench.core.simulator.conf.PapaBenchSimulatorConf.SimulatorFlightModelTaskConf;
import papabench.core.simulator.conf.PapaBenchSimulatorConf.SimulatorGPSTaskConf;
import papabench.core.simulator.conf.PapaBenchSimulatorConf.SimulatorIRTaskConf;
import papabench.core.simulator.devices.impl.SimulatorGPSDeviceImpl;
import papabench.core.simulator.devices.impl.SimulatorIRDeviceImpl;
import papabench.core.simulator.devices.impl.SimulatorServosControllerImpl;
import papabench.core.simulator.model.FlightModel;
import papabench.core.simulator.model.impl.FlightModelImpl;
import papabench.core.simulator.tasks.handlers.SimulatorFlightModelTaskHandler;
import papabench.core.simulator.tasks.handlers.SimulatorGPSTaskHandler;
import papabench.core.simulator.tasks.handlers.SimulatorIRTaskHandler;
import papabench.scj.commons.tasks.PapaBenchSCJPeriodicTask;

/**
 * Factory creating an instance of PapaBench.
 * 
 * The factory does not initialize created modules !
 * 
 * @author Michal Malohlava
 *
 */
public class PapaBenchSCJFactory {
	
	public static AutopilotModule createAutopilotModule(PapaBench papaBench) {
		
		AutopilotModule autopilotModule = new AutopilotModuleImpl();
		
		autopilotModule.setLinkToFBW(new LinkToFBWImpl());
		autopilotModule.setGPSDevice(new SimulatorGPSDeviceImpl());
		autopilotModule.setIRDevice(new SimulatorIRDeviceImpl());
		autopilotModule.setNavigator(new NavigatorImpl());
		autopilotModule.setEstimator(new EstimatorModuleImpl());
		autopilotModule.setPapaBench(papaBench);
		
		return autopilotModule;
	}
	
	public static FBWModule createFBWModule() {
		FBWModule fbwModule = new FBWModuleImpl();
		
		fbwModule.setLinkToAutopilot(new LinkToAutopilotImpl());
		fbwModule.setPPMDevice(new PPMDeviceImpl());
		fbwModule.setServosController(new SimulatorServosControllerImpl());
		
		return fbwModule;		
	}
	
	public static SPIBusChannel createSPIBusChannel() {
		return new SPIBusChannelImpl();
	}
	
	public static FlightModel createSimulator() {
		return new FlightModelImpl();
	}
	
	public static void registerAutopilotPeriodicTasks(AutopilotModule autopilotModule) {
		// autopilot tasks
		new PapaBenchSCJPeriodicTask(new AltitudeControlTaskHandler(autopilotModule), AltitudeControlTaskConf.PRIORITY, AltitudeControlTaskConf.RELEASE_MS, AltitudeControlTaskConf.PERIOD_MS, AltitudeControlTaskConf.SIZE, AltitudeControlTaskConf.NAME);
		new PapaBenchSCJPeriodicTask(new ClimbControlTaskHandler(autopilotModule), ClimbControlTaskConf.PRIORITY, ClimbControlTaskConf.RELEASE_MS, ClimbControlTaskConf.PERIOD_MS, ClimbControlTaskConf.SIZE, ClimbControlTaskConf.NAME);
		new PapaBenchSCJPeriodicTask(new NavigationTaskHandler(autopilotModule), NavigationTaskConf.PRIORITY, NavigationTaskConf.RELEASE_MS, NavigationTaskConf.PERIOD_MS, NavigationTaskConf.SIZE, NavigationTaskConf.NAME);
		new PapaBenchSCJPeriodicTask(new StabilizationTaskHandler(autopilotModule), StabilizationTaskConf.PRIORITY, StabilizationTaskConf.RELEASE_MS, StabilizationTaskConf.PERIOD_MS, StabilizationTaskConf.SIZE, StabilizationTaskConf.NAME);
		new PapaBenchSCJPeriodicTask(new RadioControlTaskHandler(autopilotModule), RadioControlTaskConf.PRIORITY, RadioControlTaskConf.RELEASE_MS, RadioControlTaskConf.PERIOD_MS, RadioControlTaskConf.SIZE, RadioControlTaskConf.NAME);
		new PapaBenchSCJPeriodicTask(new LinkFBWSendTaskHandler(autopilotModule), LinkFBWSendTaskConf.PRIORITY, LinkFBWSendTaskConf.RELEASE_MS, LinkFBWSendTaskConf.PERIOD_MS, LinkFBWSendTaskConf.SIZE, LinkFBWSendTaskConf.NAME);
		new PapaBenchSCJPeriodicTask(new ReportingTaskHandler(autopilotModule), ReportingTaskConf.PRIORITY, ReportingTaskConf.RELEASE_MS, ReportingTaskConf.PERIOD_MS, ReportingTaskConf.SIZE, ReportingTaskConf.NAME);;		
	}
	
	public static void registerFBWPeriodicTasks(FBWModule fbwModule) {
		new PapaBenchSCJPeriodicTask(new CheckFailsafeTaskHandler(fbwModule), CheckFailsafeTaskConf.PRIORITY, CheckFailsafeTaskConf.RELEASE_MS, CheckFailsafeTaskConf.PERIOD_MS, CheckFailsafeTaskConf.SIZE, CheckFailsafeTaskConf.NAME);
		new PapaBenchSCJPeriodicTask(new CheckMega128ValuesTaskHandler(fbwModule), CheckMega128ValuesTaskConf.PRIORITY, CheckMega128ValuesTaskConf.RELEASE_MS, CheckMega128ValuesTaskConf.PERIOD_MS, CheckMega128ValuesTaskConf.SIZE, CheckMega128ValuesTaskConf.NAME);
		new PapaBenchSCJPeriodicTask(new SendDataToAutopilotTaskHandler(fbwModule), SendDataToAutopilotTaskConf.PRIORITY, SendDataToAutopilotTaskConf.RELEASE_MS, SendDataToAutopilotTaskConf.PERIOD_MS, SendDataToAutopilotTaskConf.SIZE, SendDataToAutopilotTaskConf.NAME);
		new PapaBenchSCJPeriodicTask(new TestPPMTaskHandler(fbwModule), TestPPMTaskConf.PRIORITY, TestPPMTaskConf.RELEASE_MS, TestPPMTaskConf.PERIOD_MS, TestPPMTaskConf.SIZE, TestPPMTaskConf.NAME); 
	}
	
	public static void registerSimulatorPeriodicTasks(FlightModel flightModel, AutopilotModule autopilotModule, FBWModule fbwModule) {
		new PapaBenchSCJPeriodicTask(new SimulatorFlightModelTaskHandler(flightModel, autopilotModule, fbwModule), SimulatorFlightModelTaskConf.PRIORITY, SimulatorFlightModelTaskConf.RELEASE_MS, SimulatorFlightModelTaskConf.PERIOD_MS, SimulatorFlightModelTaskConf.SIZE, SimulatorFlightModelTaskConf.NAME);
		new PapaBenchSCJPeriodicTask(new SimulatorGPSTaskHandler(flightModel, autopilotModule), SimulatorGPSTaskConf.PRIORITY, SimulatorGPSTaskConf.RELEASE_MS, SimulatorGPSTaskConf.PERIOD_MS, SimulatorGPSTaskConf.SIZE, SimulatorGPSTaskConf.NAME);
		new PapaBenchSCJPeriodicTask(new SimulatorIRTaskHandler(flightModel, autopilotModule), SimulatorIRTaskConf.PRIORITY, SimulatorIRTaskConf.RELEASE_MS, SimulatorIRTaskConf.PERIOD_MS, SimulatorIRTaskConf.SIZE, SimulatorIRTaskConf.NAME);
	}
	
	public static void registerAutopilotInterruptHandlers(AutopilotModule autopilotModule) {		
	}
	
	public static void registerFBWInterruptHandlers(FBWModule fbwModule) {		
	}
}
