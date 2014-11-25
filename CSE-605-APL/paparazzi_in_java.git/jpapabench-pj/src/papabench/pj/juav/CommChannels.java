package papabench.pj.juav;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.net.InetSocketAddress;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import com.fiji.fivm.r1.Import;
import com.fiji.fivm.r1.Pointer;
import com.fiji.fivm.r1.fivmRuntime;

public class CommChannels implements CommChannel {
	private final String remoteIP = "10.42.0.1";
	private final int remotePort = 8081;
	private final int localPort = 8080;
	private final int numInput = 13;
	private final String[] OutputName = new String[] { "COMMAND", "OTHERS" };
	private final int[] OutputPeriodic = new int[] { 100, 50 };
	private Map<String, Socket> inputConn;
	private Map<String, Socket> outputConn;

	public CommChannels() {
		this.inputConn = new HashMap<String, Socket>();
		this.outputConn = new HashMap<String, Socket>();
		// ethernet_init();
	}

	public void init() {
		System.out.println("Connections should be established from C version");
	}

	public void sendOtherMsg(List<String> msgList) {

		for (String msg : msgList) {

			Pointer p = fivmRuntime.getCString(msg);
			int length = fivmRuntime.lengthOfCString(p);
			ethernet_write(p, length, 1);

			// System.out.println(">> " + msg);

			// Free the buffer here
			fivmRuntime.returnBuffer(p);
		}

	}

	public void sendCommandMsg(String msg) {

		Pointer p = fivmRuntime.getCString(msg);
		int length = fivmRuntime.lengthOfCString(p);
		ethernet_write(p, length, 0);

		// System.out.println(">> !! " + msg);

		// Free the buffer here
		fivmRuntime.returnBuffer(p);

	}

	public String readChannel(int index) {
		String line = "";

		Pointer p = fivmRuntime.getBuffer(256);
		int size = ethernet_readline(p, index);

		if (size > 0) {
			line = fivmRuntime.fromCStringFull(p);
			// System.out.println("<< " + line);
		} else {
			// System.out.println("Read of Channel: " + index + " failed");
		}

		fivmRuntime.returnBuffer(p);

		return line;

	}

	@Import
	static native void ethernet_write(Pointer p, int len, int skIdx);

	@Import
	static native int ethernet_readline(Pointer p, int skIdx);

	@Override
	public String readChannel(String key) {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public void sendOtherMsg(String msg) {
		// TODO Auto-generated method stub
		
	}
}
