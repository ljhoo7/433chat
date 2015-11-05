package kr.co.ftt.ManageServer_receiver;

import java.io.IOException;
import java.io.OutputStream;

import kr.co.ftt.ManageServer_receiver.util.CPacket;
import kr.co.ftt.ManageServer_receiver.util.JDBCConnect;

public class OutputCommandHandler {
	public final static int TYPE_SIZE = 2;
	public final static int INT_SIZE = 4;
	public final static int SHORT_SIZE = 2;
	public final static int LONG_SIZE = 4;	
	
	public final static short user_out = 11;
	public final static short room_destroy = 12;
	public final static short kill_server = 13;
	public final static short request = 14;
	public final static short generate = 15;

	byte[] byteArrCnt=new byte[2];
	byte[] byteArrRoom=new byte[2];
	byte[] byteArrUser=new byte[22];
	byte[] byteArrInterserver=new byte[8];
	byte[] byteArrAgentNum=new byte[2];
	
	CPacket msg=new CPacket();
	
	private static OutputCommandHandler instance;
	
	private OutputCommandHandler(){
	}
	
	public static OutputCommandHandler getInstance() {
		if (instance == null) {
			instance = new OutputCommandHandler();
		}
		return instance;
	}
	
	public void user_out(int serverNum,int clientSocket,OutputStream os){
		msg=new CPacket();
		msg.push(user_out);

		msg.push(serverNum);
		msg.push(clientSocket);
		try {
			os.write(msg.buffer,0,TYPE_SIZE+INT_SIZE+INT_SIZE);
			os.flush();
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
	
	public void room_destroy (short roomNum,OutputStream os){
		msg=new CPacket();
		msg.push(room_destroy);
		msg.push(roomNum);
		try {
			for(int i=0;i<TYPE_SIZE+SHORT_SIZE;i++){
				System.out.println(msg.buffer[i]);
			}
			os.write(msg.buffer,0,TYPE_SIZE+SHORT_SIZE);
			os.flush();
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
	
	public void kill_server (int serverNum,OutputStream os){
		msg=new CPacket();
		msg.push(kill_server);
		msg.push(serverNum);
		try {
			os.write(msg.buffer,0,TYPE_SIZE+INT_SIZE);
			os.flush();
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
	
	public void request(OutputStream os){
		msg=new CPacket();
		msg.push(request);
		try {
			os.write(msg.buffer,0,TYPE_SIZE);
			os.flush();
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
	public void generate(OutputStream os){
		Manager.toggleGenbuttonFlag();
        JDBCConnect.getInstance().updateGenState(Manager.genButtonFlag);
		msg=new CPacket();
		msg.push(generate);
		try {
			os.write(msg.buffer,0,TYPE_SIZE);
			os.flush();
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
}
