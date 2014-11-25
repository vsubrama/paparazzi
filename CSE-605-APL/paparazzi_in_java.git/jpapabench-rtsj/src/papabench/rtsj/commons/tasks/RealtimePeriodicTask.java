package papabench.rtsj.commons.tasks;

import javax.realtime.AbsoluteTime;
import javax.realtime.AsyncEventHandler;
import javax.realtime.PeriodicParameters;
import javax.realtime.RealtimeThread;
import javax.realtime.RelativeTime;

public class RealtimePeriodicTask extends RealtimeThread {
	protected Runnable targetRunnable;

	public RealtimePeriodicTask(Runnable targetRunnable) {
		super();
		this.targetRunnable = targetRunnable;
	}
	
	public void attachReleaseParams(long start, long periodic){
		this.setReleaseParameters(new PeriodicParameters(new AbsoluteTime(start, 0), new RelativeTime(periodic,0)));
	}
	
	public void attachReleaseParams(long start, long periodic, String name, AsyncEventHandler missingHandler){
		this.setReleaseParameters(new PeriodicParameters(new AbsoluteTime(start, 0), new RelativeTime(periodic,0), null, null, null, missingHandler));
	}

	@Override
	public void run() {

		while (true) {
			do {
				targetRunnable.run();
			} while (this.waitForNextPeriod());

		}
	}
}