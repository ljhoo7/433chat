package tcp;

public class ServerInfo {
	public String ip;
	public int interPort;
	public int clientPort;
	
	ServerInfo(String ip, int interPort, int clientPort){
		this.ip = ip;
		this.interPort =interPort;
		this.clientPort = clientPort;
	}
}
