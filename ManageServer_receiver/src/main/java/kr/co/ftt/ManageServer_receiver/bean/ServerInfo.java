package kr.co.ftt.ManageServer_receiver.bean;

public class ServerInfo {
	private long ip;
	private int port;
	
	public ServerInfo(long ip, int port) {
		super();
		this.ip = ip;
		this.port = port;
	}
	
	public long getIp() {
		return ip;
	}
	public void setIp(long ip) {
		this.ip = ip;
	}
	public int getPort() {
		return port;
	}
	public void setPort(int port) {
		this.port = port;
	}
	
}
