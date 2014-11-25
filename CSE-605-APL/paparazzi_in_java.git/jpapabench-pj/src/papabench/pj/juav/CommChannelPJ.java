package papabench.pj.juav;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.net.InetSocketAddress;
import java.net.ServerSocket;
import java.net.Socket;
import java.net.SocketTimeoutException;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

public class CommChannelPJ implements CommChannel {
	private final String remoteIP = "127.0.0.1";
	private final int remotePort = 8081;
	private final int localPort = 8080;
	private final int numInput = 4;
	private final String[] OutputName = new String[] { "COMMAND", "OTHERS" };
	private final int[] OutputPeriodic = new int[] { 100, 50 };
	private Map<String, Socket> inputConn;
	private Map<String, Socket> outputConn;

	public CommChannelPJ() {
		this.inputConn = new HashMap<String, Socket>();
		this.outputConn = new HashMap<String, Socket>();
	}

	public void init() {
		try {
			ServerSocket serverSocket = new ServerSocket(localPort);
			while (inputConn.size() < numInput) {
				Socket socket = serverSocket.accept();
				socket.setSoTimeout(10);
				System.out.println("Get connection...");
				BufferedReader reader = new BufferedReader(
						new InputStreamReader(socket.getInputStream()));
				String line = reader.readLine();
				inputConn.put(line, socket);
				System.out.println("Establish connection for " + line);
			}
			System.out.println("Finish input connections init");

			boolean isReady = false;
			while (!isReady) {
				Socket socket = new Socket();
				InetSocketAddress endPoint = new InetSocketAddress(remoteIP,
						remotePort);
				socket.connect(endPoint, 0);
				if (socket.isConnected()) {
					isReady = true;
					BufferedWriter writer = new BufferedWriter(
							new OutputStreamWriter(socket.getOutputStream()));
					writer.write("TEST\n");
					writer.flush();
				}
			}

			for (int i = 0; i < OutputName.length; i++) {
				Socket socket = new Socket();
				InetSocketAddress endPoint = new InetSocketAddress(remoteIP,
						remotePort);
				socket.connect(endPoint, 10);
				BufferedWriter writer = new BufferedWriter(
						new OutputStreamWriter(socket.getOutputStream()));
				writer.write(OutputName[i] + "\n");
				writer.flush();
				outputConn.put(OutputName[i], socket);

				// Thread st = new Thread(new SendingThread(socket,
				// OutputName[i],
				// OutputPeriodic[i]));
				// st.start();
				try {
					Thread.sleep(10);
				} catch (InterruptedException e) {
					e.printStackTrace();
				}
			}
			System.out.println("Finish output connections init "
					+ inputConn.size() + ":" + outputConn.size());

			// for (Map.Entry<String, Socket> conn : inputConn.entrySet()) {
			// Thread t = new Thread(new ReadingThread(conn.getValue()));
			// t.start();
			// }
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	public void sendOtherMsg(List<String> msgList) {
		try {
			BufferedWriter writer = new BufferedWriter(new OutputStreamWriter(
					outputConn.get("OTHERS").getOutputStream()));
			for (String msg : msgList) {
				// System.out.println(msg);
				writer.write(msg + "\n");
				writer.flush();
			}
		} catch (IOException e) {
			e.printStackTrace();
		}
	}

	public void sendOtherMsg(String msg) {
		try {
			BufferedWriter writer = new BufferedWriter(new OutputStreamWriter(
					outputConn.get("OTHERS").getOutputStream()));

			// System.out.println(msg);
			writer.write(msg + "\n");
			writer.flush();

		} catch (IOException e) {
			e.printStackTrace();
		}
	}

	public void sendCommandMsg(String msg) {
		try {
			BufferedWriter writer = new BufferedWriter(new OutputStreamWriter(
					outputConn.get("COMMAND").getOutputStream()));
			writer.write(msg + "\n");
			writer.flush();
			// writer.close();
		} catch (IOException e) {
			e.printStackTrace();
		}
	}

	public String readChannel(String key) {
		String line = "";
		try {
			BufferedReader reader = new BufferedReader(new InputStreamReader(
					inputConn.get(key).getInputStream()));
			line = reader.readLine();
		} catch (SocketTimeoutException e) {
			// TODO Auto-generated catch block
			// e.printStackTrace();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			// e.printStackTrace();
		}
		return line;
	}

	@Override
	public String readChannel(int index) {
		// TODO Auto-generated method stub
		return null;
	}
}