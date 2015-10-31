package kr.co.ftt.tcp;
import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.io.OutputStream;
import java.io.UnsupportedEncodingException;
import java.net.Socket;
import java.net.UnknownHostException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.ArrayList;

public class Client extends Thread implements Runnable {
	Socket client = null;
	String ipAddress; // 접속을 요청할 Server의 IP 주소를 저장할 변수
	static final int port = 7001; // 접속을 요청할 Server의 port 번호와 동일하게 지정

	BufferedReader read;

	// 입력용 Stream
	public static InputStream is;

	// 출력용 Stream
	public static OutputStream os;

	private static Client instance;
	
	public static Client getInstance(){
		if(instance==null){
			instance=new Client("127.0.0.1");
		}
		return instance;
	}

	public Client(String ip) {
		ipAddress = ip; // 생성자의 IP Address를 ipAddress 맴버변수에 저장
		instance=this;
	}
	@Override
	public void run() {
		System.out.println("***** ManageServer_web이 ManageServer_receiver로 접속을 시도합니다 *****");

		// *** 접속할 Server의 IP Address와 port 번호 정보가 있는 Socket 생성 ***//
		try {
			client = new Socket(ipAddress, port);
		} catch (UnknownHostException e) {
			e.printStackTrace();
		} catch (IOException e) {
			e.printStackTrace();
		}
		try {
			is = client.getInputStream();
			os = client.getOutputStream();
		} catch (IOException e) {
			e.printStackTrace();
		}
		while (true) {
			int printFlag = 1;
			if (printFlag == 0) {
				byte[] type = new byte[2];
				try {
					is.read(type, 0, 2);
				} catch (IOException e) {
					e.printStackTrace();
				}
				InputStreamHandler(type);
			} else {
				byte[] str = new byte[50];
				System.out.println("=====input start====");
				try {
					is.read(str);
				} catch (IOException e) {
					e.printStackTrace();
				}
				for (byte b : str) {
					System.out.println("b:" + b);
				}
				System.out.println("=====input end====");
			}
		}
	}

	public static void main(String[] args) {
		Client client = new Client("127.0.0.1");
		client.start();
	}

	private void InputStreamHandler(byte[] type) {
		switch (type[0]) {
		case 0:			
		}
	}
	public void pt_user_out(short agentNum,int serverNum,int userSocket){ //agentNum에 맞도록 해줘야함..
		CPacket msg = new CPacket();
		msg.push((short) 5);
		msg.push(agentNum);
		msg.push(serverNum);
		msg.push(userSocket);
		
		try {
			os.write(msg.buffer,0,12);
			os.flush();
		} catch (IOException e) {
			e.printStackTrace();
		}
    }
    
    public void msag_room_destroy(short agentNum,short destroy){ //agentNum에 맞도록 해줘야함..
		CPacket msg = new CPacket();
		msg.push((short) 1);
		msg.push((short)destroy);
		
		try {
			os.write(msg.buffer,0,4);
			os.flush();
		} catch (IOException e) {
			e.printStackTrace();
		}
    }
    
    public void msag_kill_server(short agentNum, int serverNum){ //agentNum에 맞도록 해줘야함..
		CPacket msg = new CPacket();
		msg.push((short)2);
		msg.push(agentNum);
		msg.push(serverNum);
		
		try {
			os.write(msg.buffer,0,8);
			os.flush();
		} catch (IOException e) {
			e.printStackTrace();
		}
    }
    
    public void pt_request_total_info(){ //agentNum에 맞도록 해줘야함..
		CPacket msg = new CPacket();
		msg.push((short)3);    		
		try {
			os.write(msg.buffer,0,2);
			os.flush();
		} catch (IOException e) {
			e.printStackTrace();
		}
    }

	public void generate(short agentNum) {
		CPacket msg = new CPacket();
		msg.push((short)4);
		msg.push(agentNum);
		try {
			os.write(msg.buffer,0,4);
			os.flush();
		} catch (IOException e) {
			e.printStackTrace();
		}		
	}	
}
