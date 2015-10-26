package tcp;

import java.io.*;
import java.net.*;
import java.nio.*;

import websocket.Room;
import websocket.Server;
import websocket.User;

enum sagType
{
	pt_room_info_changed(0),
	pt_user_info_changed(1),
	pt_total_room_info(2),
	pt_total_user_info(3),
	pt_health_ack(4),
	
	pt_user_out(5),
	pt_room_destroy(6),
	pt_kill_server(7),
	pt_health_check(8);
	
	private int value;
	
	private sagType(int value){
		this.value = value;
	}
};

public class AgentSocket{
	Socket socket;
	InputStream inStream;
    OutputStream outStream;
    int serverNum;
    boolean end = false;
    public boolean isConnected = false;
    
    AgentSocketThread recieveThread;
    
    public byte[] readBuffer = new byte[8092];
    
    public AgentSocket(Socket socket, int serverNum){
    	this.socket = socket;
    	this.serverNum = serverNum;
    	end = false;
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
    	try {
        	socket = new Socket(hostName, portNumber);
        	if (socket==null){
        		System.out.println("connect failed " + (new Integer(serverNum)).toString());
        		return false;
        	}
 			
        	inStream = socket.getInputStream();
			outStream = socket.getOutputStream();
			
			System.out.println("new connect socket " + (new Integer(serverNum)).toString());
			
			outStream.write(TcpServer.serverNum);
			MakeSync();
			
			recieveThread = new AgentSocketThread(this);
			recieveThread.start();
			
			isConnected = true;
			
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
        
        if (protocol==sagType.pt_user_out.ordinal()){
        	System.out.println("pt out request!");
        	
        	if (!read_until(2, 6)) return;
        	CPacket msg = new CPacket(readBuffer);
        	msg.pop_int();// delete padding
        	int client_socket = msg.pop_int();
        	
        	
        	Server.comHandler.killSession(client_socket);
        	/* user out! */
        }else if (protocol == sagType.pt_room_destroy.ordinal()){
        	System.out.println("room destroy request");
        	
        	if (!read_until(2, 2)) return;
        	CPacket msg = new CPacket(readBuffer);
        	msg.pop_short(); // delete padding
        	int room_num = msg.pop_short();
        	
        	Server.comHandler.destroyRoom(room_num+"");
        	/* delete room */
        	
        }else if (protocol==sagType.pt_kill_server.ordinal()){
        	System.out.println("kill_server_send!");
        	
        	/* kill server */
        }else if (protocol==sagType.pt_health_check.ordinal()){
        	System.out.println("health check send!");
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
    	CPacket msg = CPacket.create((short)sagType.pt_total_user_info.ordinal());
    	msg.push((short)0);
    
    	short playerCnt = 0;
    			
    	for (Room r :  Server.roomlist ){
			for (User p : r.getUserlist()){
	    		msg.push(Short.parseShort(r.getRoomname()));
	    		msg.push((short)0);
	    		msg.push(Integer.parseInt(p.getSession().getId()));
	    		byte[] nick = new byte[20];
	    		String n = p.getNickname();
	    		if (n==null) n="";
	    		for (int i=0; i<n.length(); i++){
	    			nick[i] = (byte)n.charAt(i);
	    		}
	    		msg.push(nick, 20);
	    		playerCnt++;
			}
		}
    	System.out.println(playerCnt);
    	msg.record_size(2, playerCnt);
    	
    	send(msg);
    }

	void SendRoomInfo(){
		CPacket msg = CPacket.create((short)sagType.pt_total_room_info.ordinal());
		msg.push((short)0);
		short roomCnt = 0;
		
		for (Room r :  Server.roomlist ){
			if (r.getRoomname() != "-1"){
				msg.push(Short.parseShort(r.getRoomname()));
				roomCnt++;
			}
		}
		
    	msg.record_size(2, roomCnt);
    	
		send(msg);
	}
	
	
	public void SendUserInfo(User u, Room r, byte type){
		CPacket msg = CPacket.create((short)sagType.pt_user_info_changed.ordinal());
      	msg.push((short) 0);// padding
      	msg.push(Integer.parseInt(u.getSession().getId()));
      	msg.push(Short.parseShort(r.getRoomname()));
       
      	byte[] nick = new byte[20];
      	String n = u.getNickname();
      	if (n == null)
          n = "";
      	for (int i = 0; i < n.length(); i++) {
      		nick[i] = (byte) n.charAt(i);
      	}
      	msg.push(nick, 20);
      	msg.push((byte)type);
      	msg.push((byte)0);
      	 
      	send(msg);
	}
	
	public void SendRoomInfo(Room r, int type){
		CPacket msg = CPacket.create((short)sagType.pt_room_info_changed.ordinal());
	   	msg.push(Short.parseShort(r.getRoomname()));
	   	msg.push((short)type);
	   	 
	   	send(msg);
	}
	
	public void SendRoomInfo(short r, int type){
		CPacket msg = CPacket.create((short)sagType.pt_room_info_changed.ordinal());
	   	msg.push(r);
	   	msg.push((short)type);
	   	 
	   	send(msg);
	}
}

class AgentSocketThread extends Thread {
	AgentSocket client;


    public AgentSocketThread(AgentSocket cli){
        this.client = cli;
    }

    public void run() {
        while(!client.end){
            client.recieve();
        }
    } // run
} // Thread1

