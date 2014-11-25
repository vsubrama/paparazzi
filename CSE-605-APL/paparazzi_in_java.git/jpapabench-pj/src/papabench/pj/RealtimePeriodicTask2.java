package papabench.pj;

import javax.realtime.RealtimeThread;
import javax.realtime.ReleaseParameters;
import javax.realtime.SchedulingParameters;

/**
 * TODO comment
 * 
 * @author Michal Malohlava
 * 
 */
public class RealtimePeriodicTask2 extends RealtimeThread {

	protected Runnable targetRunnable;
	int priority;

	public RealtimePeriodicTask2(Runnable targetRunnable, int p) {
		super();

		this.targetRunnable = targetRunnable;
		this.priority = p;
	}

	@Override
	public void run() {
		System.out.println("start: Thread_" + priority + " : " + RealtimePeriodicTask2.currentThread().getName());
		while (true) {
			do {
				targetRunnable.run();
			} while (this.waitForNextPeriod());
		}
	}
}
