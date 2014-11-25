package papabench.pj.juav;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.Socket;

public class ReadingThread implements Runnable {
	Socket s;
	
	public ReadingThread(Socket s){
		this.s = s;
	}
	
	public void run() {
		try {
			BufferedReader reader = new BufferedReader(new InputStreamReader(s.getInputStream()));
			while (true) {
				String line = reader.readLine();
				if(line != null){
					System.out.println(line);
				} 
				
				Thread.sleep(100);
			}
		} catch (IOException e) {
			e.printStackTrace();
		} catch (InterruptedException e) {
			e.printStackTrace();
		}
	}

}
