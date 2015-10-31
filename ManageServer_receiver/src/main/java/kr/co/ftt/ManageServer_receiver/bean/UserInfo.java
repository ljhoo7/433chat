package kr.co.ftt.ManageServer_receiver.bean;

public class UserInfo {
	private int server_num;
	private int client_socket;
	private String nickname;
	private short room_num;
	
	public UserInfo(int server_num,int client_socket, String nickname,short room_num) {
		super();
		this.client_socket = client_socket;
		this.room_num = room_num;
		this.nickname = nickname;
	}
	public int getServer_num() {
		return server_num;
	}
	public void setServer_num(int server_num) {
		this.server_num = server_num;
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
