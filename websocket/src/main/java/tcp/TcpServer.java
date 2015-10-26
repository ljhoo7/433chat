package tcp;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.io.OutputStream;
import java.io.UnsupportedEncodingException;
import java.net.ConnectException;
import java.net.Socket;
import java.net.SocketException;
import java.net.UnknownHostException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.ArrayList;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

import websocket.CommandHandler;
import websocket.Room;
import websocket.Server;
import websocket.User;

enum ssType
{
	pkt_create(0),
	pkt_destroy(1),
	pkt_join(2),
	pkt_leave(3),
	pkt_chat(4),
	pkt_connect(5),
	pkt_disconnect(6),
	pkt_heartbeats(7),
	pkt_heartbeats_response(8),
	pkt_room_info_send(9),
	pkt_player_info_send(10),
	pkt_room_info_success(11),
	pkt_room_info_failure(12),
	pkt_player_info_success(13),
	pkt_player_info_failure(14),
	pkt_sync_req(15);
	
	private int value;
	
	private ssType(int value){
		this.value = value;
	}
};

public class TcpServer extends Thread implements Runnable {
	static ArrayList<InterSocket> sockets = new ArrayList<InterSocket>();
	static String ipAddress; // 접속을 요청할 Server의 IP 주소를 저장할 변수
	static final int port = 1; // 접속을 요청할 Server의 port 번호와 동일하게 지정
	static ArrayList<PlayerInfo> playerList = new ArrayList<PlayerInfo>();
	static ArrayList<ServerInfo> serverList = new ArrayList<ServerInfo>();
	
	public static AgentSocket agentSocket;
	
	public static Lock socketLock = new ReentrantLock();;
	
	public static int serverNum = 3;
	
	public static int[] protocolSize = {12, 12, 32, 36, 32, 12, 12, 2, 2, 6, 6, 2, 2, 2, 2, 2};
	
	static ListenSocket listenSocket;
	
	public static void AddSocket(InterSocket socket){
		socketLock.lock();
		sockets.add(socket);
		socketLock.unlock();
	}
	
	public static void DeleteSocket(InterSocket socket){
		socketLock.lock();
		int n = playerList.size()-1;
		for (int i=n-1; i>=0; i--){
			PlayerInfo p = playerList.get(i);
			if (p.getServernum()==socket.serverNum){
				playerList.remove(p);
				
				if (p.getRoom_num() != -1){
					Server.msgHandler.broadcastNoticeToRoom
					((new Short(p.getRoom_num())).toString(), p.getNickname() + "님이 퇴장하셨습니다.");
				}
			}
		}
		
		sockets.remove(socket);
		socketLock.unlock();
	}
	
	public static void AddPlayer(PlayerInfo info){
		socketLock.lock();
		playerList.add(info);
		socketLock.unlock();
	}
	
	public static void DeletePlayer(PlayerInfo info){
		socketLock.lock();
		playerList.remove(info);
		socketLock.unlock();
	}
	
	public static PlayerInfo FindPlayer(int client_socket, int server_num){
		socketLock.lock();
		for (int i=0; i<playerList.size(); i++){
			PlayerInfo g = playerList.get(i);
			if (g.getClient_socket() == client_socket 
					&& g.getServernum() == server_num){
				socketLock.unlock();
				return g;
			}
		}
		socketLock.unlock();
		return null;
	}
	
	public static PlayerInfo FindPlayer(int client_socket){
		socketLock.lock();
		for (int i=0; i<playerList.size(); i++){
			PlayerInfo g = playerList.get(i);
			if (g.getClient_socket() == client_socket){
				socketLock.unlock();
				return g;
			}
		}
		socketLock.unlock();
		return null;
	}

	
	@Override
	public void run() {
		BufferedReader in;
		try {
			 File path = new File(".");
			 System.out.println(path.getAbsolutePath());
			in = new BufferedReader(new FileReader("ServerInfo.txt"));
			String s;
			
		    while ((s = in.readLine()) != null) {
		      System.out.println(s);
		      String[] ret = s.split(" ");
		      serverList.add(new ServerInfo(ret[0], Integer.parseInt(ret[1]), Integer.parseInt(ret[2]), Integer.parseInt(ret[3])));
		    }
		    in.close();
		    
		    agentSocket = new AgentSocket(null, serverNum);
		    
		    for (int i=0; i<serverList.size(); i++){
		    	if (i==serverNum) continue;
		    	InterSocket socket = new InterSocket(null, i, true);
		    	if (socket.connect(serverList.get(i).ip, serverList.get(i).interPort)){
		    		System.out.println("server connect to " + (new Integer(i)).toString() + " successed!");
		    	}else{
		    		System.out.println("server connect to " + (new Integer(i)).toString() + " failed!");
		    	}
		    	
		    }
		    
		   
	    	if (agentSocket.connect("127.0.0.1", 7000)){
	    		System.out.println("agent client connect");
	    	}else{
	    		System.out.println("agent client failed");
	    	}
		    
		    
		    listenSocket = new ListenSocket(serverList.get(serverNum).interPort);
		    listenSocket.start();
		} catch (FileNotFoundException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	    
	}
	

	public TcpServer(String ip) {
		ipAddress = ip; // 생성자의 IP Address를 ipAddress 맴버변수에 저장
	}
	
	public static void SendAll(CPacket msg){		
		socketLock.lock();
		
		for(InterSocket sock : sockets){
			sock.send(msg);
		}
		socketLock.unlock();
	}
		

}
