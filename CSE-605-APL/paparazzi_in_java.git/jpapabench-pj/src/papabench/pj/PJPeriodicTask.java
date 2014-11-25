package papabench.pj;

/**
 * This is just a simple representation of plain Java periodic thread. 
 * 
 * It carries information about periodic invocation, but it does not do it.
 *
 * @author Michal Malohlava
 *
 */

import javax.realtime.RealtimeThread;

public class PJPeriodicTask {
	
	private Runnable taskHandler;
	private int priority;
	private int releaseMs;
	private int periodMs;


	
	public PJPeriodicTask(Runnable taskHandler, int priority, int releaseMs, int periodMs) {
	        this.taskHandler = taskHandler;
		this.priority = priority;
		this.releaseMs = releaseMs;
		this.periodMs = periodMs;
	}

	public Runnable getTaskHandler() {
		return taskHandler;
	}

	public int getPriority() {
		return priority;
	}

	public int getReleaseMs() {
		return releaseMs;
	}

	public int getPeriodMs() {
		return periodMs;
	}
}
