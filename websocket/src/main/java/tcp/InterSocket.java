package tcp;

import java.io.*;
import java.net.*;
import java.nio.*;

import websocket.Room;
import websocket.Server;
import websocket.User;

public class InterSocket{
	Socket socket;
	InputStream inStream;
    OutputStream outStream;
    int serverNum;
    boolean end = false;
    boolean isConnect;
    
    int isWeb;
    
    InterSocketThread recieveThread;
    
    public byte[] readBuffer = new byte[8092];
    
    public InterSocket(Socket socket, int serverNum, boolean isConnect){
    	this.socket = socket;
    	this.serverNum = serverNum;
    	end = false;
    	this.isConnect = isConnect;
    	if (socket!=null){
    		try {
    			inStream = socket.getInputStream();
    			outStream = socket.getOutputStream();
    			
    			if(!read_until(0, 4)) return;
    			this.serverNum = bytesToInteger(0);
    			System.out.println("new accept socket " + (new Integer(this.serverNum)).toString());
    			
    			TcpServer.AddSocket(this);
    			
    			recieveThread = new InterSocketThread(this);
    			recieveThread.start();
    			
    		} catch (IOException e) {
    			// TODO Auto-generated catch block
    			e.printStackTrace();
    		}
    	}
    }
    
    public int bytesToInteger(int offset){
    	ByteBuffer bb = ByteBuffer.allocate(4);
		bb.order(ByteOrder.LITTLE_ENDIAN);
        bb.put(readBuffer[offset]);
        bb.put(readBuffer[offset+1]);
        bb.put(readBuffer[offset+2]);
        bb.put(readBuffer[offset+3]);
        
        return bb.getInt(0);
    }
    
    public int bytesToShort(int offset){
    	ByteBuffer bb = ByteBuffer.allocate(2);
		bb.order(ByteOrder.LITTLE_ENDIAN);
        bb.put(readBuffer[offset]);
        bb.put(readBuffer[offset+1]);
        
        return bb.getShort(0);
    }
    
    public boolean connect(String hostName, int portNumber){
    	if (!isConnect) return false; 
        
        try {
        	socket = new Socket(hostName, portNumber);
        	if (socket==null){
        		System.out.println("connect failed " + (new Integer(serverNum)).toString());
        		return false;
        	}
 			
        	inStream = socket.getInputStream();
			outStream = socket.getOutputStream();
			
			System.out.println("new connect socket " + (new Integer(serverNum)).toString());
			TcpServer.AddSocket(this);
			
			outStream.write(TcpServer.serverNum);
			
			if (TcpServer.sockets.size() == 1){
				CPacket msg = CPacket.create((short)ssType.pkt_sync_req.ordinal());
				send(msg);
			}
			
			recieveThread = new InterSocketThread(this);
			recieveThread.start();
			
		} catch (IOException e) {
			// TODO Auto-generated catch block
			
			System.out.println("connect failed " + (new Integer(serverNum)).toString());
    		return false;
    		
		} catch (Exception e){
    		System.out.println("connect failed " + (new Integer(serverNum)).toString());
    		return false;
		}
        end = false;
        
        return true;
    }
    
    public boolean read_until(int offset, int size){
        int pos = 0;
        while (pos < size) {
            int read_size;
			try {
				read_size = inStream.read(readBuffer, offset, size);
				
				pos += read_size;
	            offset += read_size;
	            size -= read_size;
	            
	            if (read_size <= 0){
	            	disconnect();
	            	return false;
	            }
			} catch (IOException e) {
				// TODO Auto-generated catch block
				disconnect();
            	return false;
			}
        }
        
        return true;
    }
    
    public void recieve(){
    	if (!read_until(0, 2)) return;
    
        int protocol = (int)bytesToShort(0);
        
        if (protocol==ssType.pkt_connect.ordinal()){
        	System.out.println("connect requset");
        	
        	if (!read_until(2, TcpServer.protocolSize[protocol]-2)) return;
        	CPacket msg = new CPacket(readBuffer);
        	msg.pop_int(); // delete padding
        	int client_socket = msg.pop_int();
        	int server_num = msg.pop_short();
        	
        	
        	PlayerInfo p = new PlayerInfo(client_socket, (short) -1, "", 0, server_num);
        	TcpServer.playerList.add(p);
        }else if (protocol == ssType.pkt_disconnect.ordinal()){
        	System.out.println("disconnect request");
        	
        	if (!read_until(2, TcpServer.protocolSize[protocol]-2)) return;
        	CPacket msg = new CPacket(readBuffer);
        	msg.pop_int(); // delete padding
        	int client_socket = msg.pop_int();
        	int server_num = msg.pop_short();
        	
        	TcpServer.DeletePlayer(TcpServer.FindPlayer(client_socket, server_num));
        	
        }else if (protocol==ssType.pkt_player_info_send.ordinal()){
        	System.out.println("player_info_send!");
        	
        	if (!read_until(2, 2)) return;
        	int length = (int)bytesToShort(2);
        	if (!read_until(4, length)) return;
        	
        	CPacket msg = new CPacket(readBuffer);
        	msg.pop_int();
        	int playerCnt = (int)msg.pop_short();
        	
        	if (Server.getUserCnt() + playerCnt <= 1000){
        		for (int i=0; i<playerCnt; i++){
        			int client_socket = msg.pop_int();
        			int server_num = msg.pop_int();
        			short room_num = msg.pop_short();
        			String name = msg.pop_string(20);
        			msg.pop_short(); //padding;
        			int token = msg.pop_int();
        			
        			PlayerInfo p = new PlayerInfo(client_socket, room_num, name, token, server_num);
                	TcpServer.playerList.add(p);
                	
                	System.out.println("Add Player " + (new Integer(client_socket)).toString());
        		}
        		
        		CPacket msg2 = CPacket.create((short)ssType.pkt_player_info_success.ordinal());
    			send(msg2);
    			System.out.println("player info send success!");
        		
        	}else{
        		CPacket msg2 = CPacket.create((short)ssType.pkt_player_info_failure.ordinal());
    			send(msg2);
    			System.out.println("player info send fail!");
        	}
        }else if (protocol==ssType.pkt_room_info_send.ordinal()){
        	System.out.println("room_info_send!");
        	
        	if (!read_until(2, 2)) return;
        	int length = (int)bytesToShort(2);
        	if (!read_until(4, length)) return;
        	
        	CPacket msg = new CPacket(readBuffer);
        	msg.pop_int();
        	int roomCnt = (int)msg.pop_short();
        	
        	if (roomCnt + Server.getRoomCnt() <= 100){
        		for (int i=0; i<roomCnt; i++){
        			short room_num = msg.pop_short();
                	
                	if (Server.comHandler.createRoom((new Short(room_num)).toString())){
                		System.out.println("Add Room " + (new Short(room_num)).toString());
                	}
        		}
        		
        		for (PlayerInfo p : TcpServer.playerList){
        			Server.msgHandler.broadcastNoticeToRoom
        			((new Short(p.getRoom_num())).toString(), p.getNickname() + "님이 입장하셨습니다.");
        		}
        		
        		
        		CPacket msg2 = CPacket.create((short)ssType.pkt_room_info_success.ordinal());
    			send(msg2);
    			System.out.println("room info send success!");
        		
        	}else{
        		CPacket msg2 = CPacket.create((short)ssType.pkt_room_info_failure.ordinal());
    			send(msg2);
    			System.out.println("room info send fail!");
        	}
        }else if (protocol==ssType.pkt_room_info_success.ordinal()){
        	System.out.println("initial room sync success!!");
        	
        }else if (protocol==ssType.pkt_player_info_success.ordinal()){
        	System.out.println("initial player sync success!!");
        	
        }else if (protocol==ssType.pkt_room_info_failure.ordinal() 
        		|| protocol==ssType.pkt_player_info_failure.ordinal()){
        	System.out.println("sync fail!");
        	disconnect();
        	
        }else if (protocol==ssType.pkt_create.ordinal()){
        	System.out.println("create call!");
        	
        	if (!read_until(2, TcpServer.protocolSize[protocol]-2)) return;
        	CPacket msg = new CPacket(readBuffer);
        	msg.pop_int(); // delete padding
        	int client_socket = msg.pop_int();
        	short room_num = msg.pop_short();
        	
        	PlayerInfo p = TcpServer.FindPlayer(client_socket, serverNum);
        	if (p==null){
        		System.out.println("not available");
        	}else{
        		Server.comHandler.createRoom((new Short(room_num)).toString());
        		
        		if (TcpServer.agentSocket.isConnected){
	               	 TcpServer.agentSocket.SendRoomInfo(room_num, 1);
              }
        	}
        	
        }else if (protocol==ssType.pkt_destroy.ordinal()){
        	System.out.println("destroy call!");
        	
        	if (!read_until(2, TcpServer.protocolSize[protocol]-2)) return;
        	CPacket msg = new CPacket(readBuffer);
        	msg.pop_int(); // delete padding
        	int client_socket = msg.pop_int();
        	short room_num = msg.pop_short();
        	
        	PlayerInfo p = TcpServer.FindPlayer(client_socket, serverNum);
        	if (p==null){
        		System.out.println("not available");
        	}else{
        		Server.comHandler.destroyRoom((new Short(room_num)).toString());
        		
        		if (TcpServer.agentSocket.isConnected){
	               	 TcpServer.agentSocket.SendRoomInfo(room_num, 0);
               }
        	}
        	
        }else if (protocol==ssType.pkt_join.ordinal()){
        	System.out.println("join call!");
        	
        	if (!read_until(2, TcpServer.protocolSize[protocol]-2)) return;
        	CPacket msg = new CPacket(readBuffer);
        	msg.pop_int(); // delete padding
        	int client_socket = msg.pop_int();
        	short room_num = msg.pop_short();
        	String nickname = msg.pop_string(20);
        	
        	PlayerInfo p = TcpServer.FindPlayer(client_socket, serverNum);
        	p.setNickname(nickname);
        	p.setClient_socket(client_socket);
        	p.setRoom_num(room_num);
        	
        	Server.msgHandler.broadcastNoticeToRoom
			((new Short(p.getRoom_num())).toString(), p.getNickname() + "님이 입장하셨습니다.");
        	
        }else if (protocol==ssType.pkt_leave.ordinal()){
        	System.out.println("leave call!");
        	
        	if (!read_until(2, TcpServer.protocolSize[protocol]-2)) return;
        	CPacket msg = new CPacket(readBuffer);
        	msg.pop_int(); // delete padding
        	int client_socket = msg.pop_int();
        	short room_num = msg.pop_short();
        	String nickname = msg.pop_string(20);
        	
        	PlayerInfo p = TcpServer.FindPlayer(client_socket, serverNum);
        	
        	Server.msgHandler.broadcastNoticeToRoom
			((new Short(p.getRoom_num())).toString(), p.getNickname() + "님이 퇴장하셨습니다.");
        	
        	p.setNickname("");
        	p.setRoom_num((short)-1);
        	
        }else if (protocol==ssType.pkt_chat.ordinal()){
        	
        	if (!read_until(2, 2)) return;
        	int length = (int)bytesToShort(2);
        	if (!read_until(4, length-4)) return;
        	
        	CPacket msg = new CPacket(readBuffer);
        	msg.pop_int();
        	short room_num = msg.pop_short();
        	msg.pop_short();
        	
        	int token = msg.pop_int();
        	String nickname = msg.pop_string(20);
        	
        	String chat = msg.pop_string(length - TcpServer.protocolSize[protocol]);
        	Server.msgHandler.broadcastMsgToRoom
			((new Short(room_num)).toString(), nickname, chat);
        	
        }else if (protocol==ssType.pkt_sync_req.ordinal()){
        	MakeSync();
        }
        
        /* something to do! */
    }
    
    public void send(CPacket msg){
    
        try {
			outStream.write(msg.buffer, 0, msg.position);
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
    }
    
    public void disconnect(){
    	if (this.end) return;
    	
	    System.out.print("disconnect socket : ");
	    System.out.println(serverNum);
    	try {
			socket.close();
			TcpServer.DeleteSocket(this);
			/* disconnect process */
		} catch (IOException e) {
			// TODO Auto-generated catch block
			return;
		}
    	this.end = true;
    }
    
    public void MakeSync(){
    	SendRoomInfo();
    	System.out.println("sync : room info send!");
    	SendPlayerInfo();
    	System.out.println("sync2 : player info send!");
    }
    
    public void SendPlayerInfo(){
    	CPacket msg = CPacket.create((short)ssType.pkt_player_info_send.ordinal());
    	msg.push((short)0);
    	msg.push((short)0);
    	
    	short length = 0;
    	length+=2;
    	short playerCnt = 0;
    			
    	for (PlayerInfo p : TcpServer.playerList){
    		msg.push(p.getClient_socket());
    		msg.push(p.getServernum());
    		msg.push(p.getRoom_num());
    		byte[] nick = new byte[20];
    		String n = p.getNickname();
    		for (int i=0; i<n.length(); i++){
    			nick[i] = (byte)n.charAt(i);
    		}
    		msg.push(nick, 20);
    		msg.push((short)0); // padding
    		msg.push(p.getToken());
    		
    		length+=36;
    		playerCnt++;
    	}
    	
    	for (Room r :  Server.roomlist ){
			for (User p : r.getUserlist()){
				msg.push(Integer.parseInt(p.getSession().getId()));
	    		msg.push(TcpServer.serverNum);
	    		msg.push(Short.parseShort(r.getRoomname()));
	    		byte[] nick = new byte[20];
	    		String n = p.getNickname();
	    		if (n==null) n="";
	    		for (int i=0; i<n.length(); i++){
	    			nick[i] = (byte)n.charAt(i);
	    		}
	    		msg.push(nick, 20);
	    		msg.push((short)0); // padding
	    		msg.push(Integer.parseInt(p.getSession().getId()));
	    		
	    		length+=36;
	    		playerCnt++;
			}
		}
    	System.out.println(length);
    	System.out.println(playerCnt);
    	
    	msg.record_size(2, length);
    	msg.record_size(4, playerCnt);
    	
    	send(msg);
    }

	void SendRoomInfo(){
		CPacket msg = CPacket.create((short)ssType.pkt_room_info_send.ordinal());
		msg.push((short)0);
		msg.push((short)0);
		short length = 0;
		length+=2;
		short roomCnt = 0;
		
		for (Room r :  Server.roomlist ){
			if (r.getRoomname() != "-1"){
				msg.push(Short.parseShort(r.getRoomname()));
				length+=2;
				roomCnt++;
			}
		}
		
		msg.record_size(2, length);
    	msg.record_size(4, roomCnt);
    	
		send(msg);
	}
}

class InterSocketThread extends Thread {
	InterSocket client;


    public InterSocketThread(InterSocket cli){
        this.client = cli;
    }

    public void run() {
        while(!client.end){
            client.recieve();
        }
    } // run
} // Thread1

