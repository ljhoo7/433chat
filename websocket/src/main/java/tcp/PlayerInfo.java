package tcp;

public class PlayerInfo {
	private int client_socket;
	private short room_num;
	private String nickname;
	private int token;
	
	private int server_num;
	
	public PlayerInfo(byte[] str) {
		
	}
	public PlayerInfo(int client_socket, short room_num, String nickname, int token, int server_num) {
		super();
		this.client_socket = client_socket;
		this.room_num = room_num;
		this.nickname = nickname;
		this.token = token;
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
	public int getToken() {
		return token;
	}
	public void setToken(int token) {
		this.token = token;
	}
	
	public int getServernum(){
		return this.server_num;
	}
	
	public void setServernum(int serverNum){
		this.server_num = serverNum;
	}
	
	public void print(){
		System.out.println("client_socket:"+client_socket+"/room_num:"+room_num+"/nickname:"+nickname+"/token:"+token);
	}
}
