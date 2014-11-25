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
public class ThreadExecutor extends Thread {

	protected Runnable targetRunnable;
	int priority;
	int period;

	public ThreadExecutor(Runnable targetRunnable, int p, int period) {
		this.targetRunnable = targetRunnable;
		this.priority = p;
		this.period = period;
	}

	@Override
	public void run() {
		System.out.println("start: Thread_" + priority + " : " + ThreadExecutor.currentThread().getName());
		while (true) {
			targetRunnable.run();
			try {
				Thread.sleep(period);
			} catch (InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}
	}
}
