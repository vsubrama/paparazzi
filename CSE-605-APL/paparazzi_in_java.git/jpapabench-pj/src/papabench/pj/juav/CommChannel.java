package papabench.pj.juav;

import java.util.List;

public interface CommChannel {
	public void init();

	public void sendOtherMsg(List<String> msgList);

	public void sendOtherMsg(String msg);

	public void sendCommandMsg(String msg);

	public String readChannel(int index);

	public String readChannel(String key);
}
