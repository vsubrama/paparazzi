package papabench.pj.juav;

import java.io.BufferedWriter;
import java.io.IOException;
import java.io.OutputStreamWriter;
import java.net.Socket;

public class SendingThread implements Runnable {
	int counter;
	Socket s;
	String name;
	int periodic;
	
	public SendingThread(Socket s, String name, int periodic) {
		this.counter = 0;
		this.s = s;
		this.name = name;
		this.periodic = periodic;
	}

	public void run() {
		try {
			BufferedWriter writer = new BufferedWriter(new OutputStreamWriter(s.getOutputStream()));
			while (true) {
				writer.write(name + " " + counter+"\n");
				//System.out.println("out ===>" + name + " " + counter);
				writer.flush();
				counter++;
				Thread.sleep(periodic);
			}
		} catch (IOException e) {
			e.printStackTrace();
		} catch (InterruptedException e) {
			e.printStackTrace();
		}
	}

}
