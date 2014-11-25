package papabench.pj.juav;

import java.util.ArrayList;
import java.util.List;

public class Timing {
	static Timing ref;
	long start, end;
	int counter = 0;

	private static List<String> items = new ArrayList<String>();
	static boolean isFinish = false;
	public static String outputFile = "";

	public static void addItem(String str) {
		// System.out.println("Result collector add:" + str );
		if (!isFinish)
			items.add(str);
	}

	public void dumpResult() {
		if (!isFinish) {
			isFinish = true;
			for (String item : items) {
				System.out.println(item);
			}
		}
	}

	public static Timing getInstance() {
		if (ref == null)
			ref = new Timing();
		return ref;
	}

	public void begin() {
		start = System.nanoTime();
	}

	public void finish() {
		end = System.nanoTime();
		addItem(start + " :: " + end);
		counter++;
		if (counter > 200) {
			dumpResult();
		}
	}
}
