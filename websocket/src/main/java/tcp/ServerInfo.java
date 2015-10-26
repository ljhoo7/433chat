package tcp;

public class ServerInfo {
	public String ip;
	public int interPort;
	public int clientPort;
	public boolean isWeb;
	
	ServerInfo(String ip, int interPort, int clientPort, int temp){
		this.ip = ip;
		this.interPort =interPort;
		this.clientPort = clientPort;
		
		if (temp==0){
			isWeb = false;
		}else{
			isWeb = true;
		}
	}
}
