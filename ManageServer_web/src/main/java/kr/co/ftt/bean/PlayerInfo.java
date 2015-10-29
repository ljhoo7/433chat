package kr.co.ftt.bean;

public class PlayerInfo {
	private int agent;
	private int client_socket;
	private short room_num;
	private String nickname;
	
	public PlayerInfo(byte[] str) {
		
	}
	public PlayerInfo(int agent,int client_socket, short room_num, String nickname) {
		super();
		this.agent = agent;
		this.client_socket = client_socket;
		this.room_num = room_num;
		this.nickname = nickname;
	}
	
	public int getAgent() {
		return agent;
	}
	public void setAgent(int agent) {
		this.agent = agent;
	}
	public int getClient_socket() {
		return client_socket;
	}
	public void setClient_socket(int client_socket) {
		this.client_socket = client_socket;
	}
	public short getRoom_num() {
		return room_num;
	}
	public void setRoom_num(short room_num) {
		this.room_num = room_num;
	}
	public String getNickname() {
		return nickname;
	}
	public void setNickname(String nickname) {
		this.nickname = nickname;
	}
	
	public void print(){
		System.out.println("client_socket:"+client_socket+"/room_num:"+room_num+"/nickname:"+nickname);
	}
}
