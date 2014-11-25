package papabench.pj;

import javax.realtime.AbsoluteTime;
import javax.realtime.AsyncEventHandler;
import javax.realtime.HighResolutionTime;
import javax.realtime.PeriodicParameters;
import javax.realtime.PriorityParameters;
import javax.realtime.RealtimeThread;
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
import papabench.pj.juav.MessageHelper;
import papabench.pj.juav.Timing;
import papabench.pj.juav.impl.JUAVRadioControlTaskHandler;
import papabench.pj.juav.impl.JUAVReportingTaskHandler;
import papabench.pj.juav.impl.JUAVSimulatorFlightModelTaskHandler;

public class PapaBenchRTSJImpl implements PJPapaBench {

	private AutopilotModule autopilotModule;
	private FBWModule fbwModule;
	private FlightPlan flightPlan;

	private static final int AUTOPILOT_TASKS_COUNT = 7;
	private static final int FBW_TASKS_COUNT = 4;
	private static final int SIMULATOR_TASKS_COUNT = 3;

	private PJPeriodicTask[] autopilotTasks;
	private PJPeriodicTask[] fbwTasks;
	private PJPeriodicTask[] simulatorTasks;

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
		// - MC0 - autopilot
		autopilotModule = PapaBenchPJFactory.createAutopilotModule(this);

		// - MC1 - FBW
		fbwModule = PapaBenchPJFactory.createFBWModule();

		// Create & configure SPIBusChannel and connect both PapaBench modules
		SPIBusChannel spiBusChannel = PapaBenchPJFactory.createSPIBusChannel();
		spiBusChannel.init();
		autopilotModule.setSPIBus(spiBusChannel.getMasterEnd()); // = MC0: SPI
																	// master
																	// mode
		fbwModule.setSPIBus(spiBusChannel.getSlaveEnd()); // = MC1: SPI slave
															// mode

		// setup flight plan
		assert (this.flightPlan != null);
		autopilotModule.getNavigator().setFlightPlan(this.flightPlan);

		// initialize both modules - if the modules are badly initialized the
		// runtime exception is thrown
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
		FlightModel flightModel = PapaBenchPJFactory.createSimulator();
		flightModel.init();

		// Register simulator tasks
		createSimulatorTasks(flightModel, autopilotModule, fbwModule);
		// init message helper
		MessageHelper.getInstance().init(autopilotModule, fbwModule,
				flightPlan, flightModel);
		createConnection();
	}

	private void createConnection() {
		MessageHelper.getInstance();
	}

	protected void createAutopilotTasks(AutopilotModule autopilotModule) {
		autopilotTasks = new PJPeriodicTask[AUTOPILOT_TASKS_COUNT];
		autopilotTasks[0] = new PJPeriodicTask(new AltitudeControlTaskHandler(
				autopilotModule), AltitudeControlTaskConf.PRIORITY,
				AltitudeControlTaskConf.RELEASE_MS,
				AltitudeControlTaskConf.PERIOD_MS);
		autopilotTasks[1] = new PJPeriodicTask(new ClimbControlTaskHandler(
				autopilotModule), ClimbControlTaskConf.PRIORITY,
				ClimbControlTaskConf.RELEASE_MS, ClimbControlTaskConf.PERIOD_MS);
		autopilotTasks[2] = new PJPeriodicTask(new LinkFBWSendTaskHandler(
				autopilotModule), LinkFBWSendTaskConf.PRIORITY,
				LinkFBWSendTaskConf.RELEASE_MS, LinkFBWSendTaskConf.PERIOD_MS);
		autopilotTasks[3] = new PJPeriodicTask(new NavigationTaskHandler(
				autopilotModule), NavigationTaskConf.PRIORITY,
				NavigationTaskConf.RELEASE_MS, NavigationTaskConf.PERIOD_MS);
		autopilotTasks[4] = new PJPeriodicTask(new JUAVRadioControlTaskHandler(
				autopilotModule), RadioControlTaskConf.PRIORITY,
				RadioControlTaskConf.RELEASE_MS, RadioControlTaskConf.PERIOD_MS);
		autopilotTasks[5] = new PJPeriodicTask(new JUAVReportingTaskHandler(
				autopilotModule), ReportingTaskConf.PRIORITY,
				ReportingTaskConf.RELEASE_MS, ReportingTaskConf.PERIOD_MS);

		// StabilizationTask allocates messages which are sent to FBW unit ->
		// allocate them in scope memory
		autopilotTasks[6] = new PJPeriodicTask(new StabilizationTaskHandler(
				autopilotModule), StabilizationTaskConf.PRIORITY,
				StabilizationTaskConf.RELEASE_MS,
				StabilizationTaskConf.PERIOD_MS);
		// autopilotTasks[6] = new ScopedMemoryRealtimePeriodicTask(new
		// StabilizationTaskHandler(autopilotModule),
		// StabilizationTaskConf.PRIORITY, StabilizationTaskConf.RELEASE_MS,
		// StabilizationTaskConf.PERIOD_MS);
	}

	protected void createFBWTasks(FBWModule fbwModule) {
		fbwTasks = new PJPeriodicTask[FBW_TASKS_COUNT];
		fbwTasks[0] = new PJPeriodicTask(
				new CheckFailsafeTaskHandler(fbwModule),
				CheckFailsafeTaskConf.PRIORITY,
				CheckFailsafeTaskConf.RELEASE_MS,
				CheckFailsafeTaskConf.PERIOD_MS);
		fbwTasks[1] = new PJPeriodicTask(new CheckMega128ValuesTaskHandler(
				fbwModule), CheckMega128ValuesTaskConf.PRIORITY,
				CheckMega128ValuesTaskConf.RELEASE_MS,
				CheckMega128ValuesTaskConf.PERIOD_MS);
		fbwTasks[2] = new PJPeriodicTask(new SendDataToAutopilotTaskHandler(
				fbwModule), SendDataToAutopilotTaskConf.PRIORITY,
				SendDataToAutopilotTaskConf.RELEASE_MS,
				SendDataToAutopilotTaskConf.PERIOD_MS);
		fbwTasks[3] = new PJPeriodicTask(new TestPPMTaskHandler(fbwModule),
				TestPPMTaskConf.PRIORITY, TestPPMTaskConf.RELEASE_MS,
				TestPPMTaskConf.PERIOD_MS);
	}

	protected void createSimulatorTasks(FlightModel flightModel,
			AutopilotModule autopilotModule, FBWModule fbwModule) {
		simulatorTasks = new PJPeriodicTask[SIMULATOR_TASKS_COUNT];
		simulatorTasks[0] = new PJPeriodicTask(
				new JUAVSimulatorFlightModelTaskHandler(flightModel,
						autopilotModule, fbwModule),
				SimulatorFlightModelTaskConf.PRIORITY,
				SimulatorFlightModelTaskConf.RELEASE_MS,
				SimulatorFlightModelTaskConf.PERIOD_MS);
		simulatorTasks[1] = new PJPeriodicTask(new SimulatorGPSTaskHandler(
				flightModel, autopilotModule), SimulatorGPSTaskConf.PRIORITY,
				SimulatorGPSTaskConf.RELEASE_MS, SimulatorGPSTaskConf.PERIOD_MS);
		simulatorTasks[2] = new PJPeriodicTask(new SimulatorIRTaskHandler(
				flightModel, autopilotModule), SimulatorIRTaskConf.PRIORITY,
				SimulatorIRTaskConf.RELEASE_MS, SimulatorIRTaskConf.PERIOD_MS);
	}

	public void start() {
		// FIXME put here rendez-vous for all tasks

		try {
			for (int i = 0; i < SIMULATOR_TASKS_COUNT - 1; i++) {
				start(simulatorTasks[i]);
			}
			RealtimeThread current = new RealtimeThread(
					simulatorTasks[2].getTaskHandler());
			current.setSchedulingParameters(new PriorityParameters(
					simulatorTasks[2].getPriority()));
			current.start();
			RealtimeThread.sleep(5);
			for (int i = 0; i < FBW_TASKS_COUNT; i++) {
				start(fbwTasks[i]);
			}
			RealtimeThread.sleep(5);
			for (int i = 0; i < AUTOPILOT_TASKS_COUNT; i++) {
				start(autopilotTasks[i]);
			}

		} catch (InterruptedException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}

	protected void start(PJPeriodicTask pjPeriodicTask) {
		RealtimePeriodicTask2 current = new RealtimePeriodicTask2(
				pjPeriodicTask.getTaskHandler(), pjPeriodicTask.getPriority());
		HighResolutionTime start = new AbsoluteTime(0, 0);
		RelativeTime periodic = new RelativeTime(pjPeriodicTask.getPeriodMs(),
				0);
		AsyncEventHandler aeh = new AsyncEventHandler(new MissDeadlineHandler(
				"Thread_" + pjPeriodicTask.getPriority()));
		current.setReleaseParameters(new PeriodicParameters(start, periodic,
				null, null, null, aeh));
		current.setSchedulingParameters(new PriorityParameters(pjPeriodicTask
				.getPriority()));
		current.start();
	}

	public void shutdown() {
		// FIXME the task should be properly shutdown
		System.exit(0);
	}

	public class MissDeadlineHandler implements Runnable {
		String name;
		boolean isMissing;

		public MissDeadlineHandler(String name) {
			this.name = name;
			isMissing = false;
		}

		public void run() {
			if (!isMissing) {
				isMissing = true;
				System.out.println(name + "missing deadline");
			}

		}
	}
}
