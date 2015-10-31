package kr.co.ftt.ManageServer_receiver;

import java.io.IOException;
import java.io.InputStream;
import java.net.ServerSocket;
import java.net.Socket;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;

import kr.co.ftt.ManageServer_receiver.util.JDBCConnect;

public class WebLinkServer implements Runnable {

	private static WebLinkServer instance;
	static String ip = "127.0.0.1";
	static int port = 7001;
	
	InputStream is;
	ServerSocket serverSocket;

	public static WebLinkServer getInstance() {
		if (instance == null)
			instance = new WebLinkServer();
		return instance;
	}

	public WebLinkServer() {
		JDBCConnect.getInstance().insertDateTime(InputCommandHandler.setTimestamp());
		System.out.println("web 모듈 접속대기");
	}

	public void run() {
		Socket socket;
		try {
			serverSocket = new ServerSocket(port);
			socket = serverSocket.accept();
			is=socket.getInputStream();
			
			while(true){				
				System.out.println("web-receive서버: 입력대기");
				byte[] byteArrServerNum = null ;
				byte[] byteArrRoomNum =null;
				byte[] agentNum =null;
				byte[] input = new byte[2];
				is.read(input, 0, 2);
				short type = input[0];
				
				System.out.println("[웹->리시버 부분 inputType]"+input[0]);
				switch(type){
				case 5:
					int serverNum = 0,clientSocket = 0;
					agentNum = new byte[2];
					byteArrServerNum = new byte[4];
					try {
						is.read(agentNum, 0, 2);
						is.read(byteArrServerNum, 0, 4);
						serverNum=byteArrayToInt(byteArrServerNum);
						byteArrServerNum = new byte[4];
						is.read(byteArrServerNum, 0, 4);
						clientSocket=byteArrayToInt(byteArrServerNum);
					} catch (IOException e2) {
						e2.printStackTrace();
					}
					for(int i=0;i<ServerThreadPool.arrayList.size();i++){
						OutputCommandHandler.getInstance().user_out(serverNum, clientSocket, ServerThreadPool.arrayList.get(i).getOutputStream());
						try {
							Thread.sleep(200);
						} catch (InterruptedException e) {
							e.printStackTrace();
						}
					}
					break;
				case 1:
					byteArrRoomNum = new byte[2];
					is.read(byteArrRoomNum, 0, 2);
					short roomNum=byteArrRoomNum[0];
					OutputCommandHandler.getInstance().room_destroy(roomNum, ServerThreadPool.arrayList.get(0).getOutputStream());
					break;
				case 2:
					agentNum = new byte[2];
					is.read(agentNum, 0, 2);
					byteArrServerNum = new byte[4];
					is.read(byteArrServerNum, 0, 4);
					serverNum=byteArrayToInt(byteArrServerNum);
					OutputCommandHandler.getInstance().kill_server(serverNum, ServerThreadPool.getServerThread(agentNum[0]+"").getOutputStream());
					break;
				case 3:					
					JDBCConnect.getInstance().insertDateTime(InputCommandHandler.setTimestamp());
					
					for(int i=0;i<ServerThreadPool.arrayList.size();i++){
						OutputCommandHandler.getInstance().request(ServerThreadPool.arrayList.get(i).getOutputStream());
						try {
							System.out.println("Thread.sleep(1000);");
							Thread.sleep(1000);
						} catch (InterruptedException e) {
							e.printStackTrace();
						}
					}
					break;
				case 4:
					agentNum = new byte[2];
					is.read(agentNum, 0, 2);
					OutputCommandHandler.getInstance().generate(ServerThreadPool.getServerThread(agentNum[0]+"").getOutputStream());
					break;
				default:
					byte[] b=new byte[20];
					is.read(b);
				}
			}
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
	private static int byteArrayToInt(byte[] bytes) {
		final int size = Integer.SIZE / 8;
		ByteBuffer buff = ByteBuffer.allocate(size);
		final byte[] newBytes = new byte[size];
		for (int i = 0; i < size; i++) {
			if (i + bytes.length < size) {
				newBytes[i] = (byte) 0x00;
			} else {
				newBytes[i] = bytes[i + bytes.length - size];
			}
		}
		buff = ByteBuffer.wrap(newBytes);
		buff.order(ByteOrder.LITTLE_ENDIAN); // Endian에 맞게 세팅
		return buff.getInt();
	}
}
