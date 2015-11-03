package kr.co.ftt.ManageServer_receiver;

import java.io.IOException;
import java.io.InputStream;
import java.net.ServerSocket;
import java.net.Socket;

import kr.co.ftt.ManageServer_receiver.util.ByteArrayToInt;
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

	private WebLinkServer() {
		JDBCConnect.getInstance().insertDateTime(InputCommandHandler.setTimestamp());
		System.out.println("WebLinkServer Accept Wait..");
	}

	public void run() {
		Socket socket;
		try {
			serverSocket = new ServerSocket(port);
			socket = serverSocket.accept();
			is=socket.getInputStream();
			
			while(true){
				//System.out.println("web-receive server: Input Wait...");
				byte[] input = new byte[2];
				is.read(input, 0, 2);//input type
				short type = input[0];
				
				typeInput(type);
				
			}
		} catch (IOException e) {
			e.printStackTrace();
		}
	}

	private void typeInput(short type) {
		byte[] byteArrServerNum = null ;
		byte[] byteArrRoomNum =null;
		byte[] agentNum =null;
		switch(type){
		case 1:
			//room_destroy
			byteArrRoomNum = new byte[2];
			try {
				is.read(byteArrRoomNum, 0, 2);//input room number
			} catch (IOException e) {
				e.printStackTrace();
			}
			short roomNum=byteArrRoomNum[0];
			OutputCommandHandler.getInstance().room_destroy(roomNum, ServerThreadPool.threadList.get(0).getOutputStream());
			break;
		case 2:
			//kill_server
			agentNum = new byte[2];
			try {
				is.read(agentNum, 0, 2);//input agent number
			} catch (IOException e) {
				e.printStackTrace();
			}
			byteArrServerNum = new byte[4];
			try {
				is.read(byteArrServerNum, 0, 4); //input server number
			} catch (IOException e) {
				e.printStackTrace();
			}
			int serverNum=ByteArrayToInt.byteArrayToInt(byteArrServerNum);
			OutputCommandHandler.getInstance().kill_server(serverNum, ServerThreadPool.getServerThread(agentNum[0]+"").getOutputStream());
			break;
		case 3:
			//request
			JDBCConnect.getInstance().insertDateTime(InputCommandHandler.setTimestamp());
			for(int i=0;i<ServerThreadPool.threadList.size();i++){ //to all serverthread in threadpool
				OutputCommandHandler.getInstance().request(ServerThreadPool.threadList.get(i).getOutputStream());
			}
			break;
		case 4:
			//generate case
			agentNum = new byte[2];
			try {
				is.read(agentNum, 0, 2);//input agent number
			} catch (IOException e) {
				e.printStackTrace();
			}
			OutputCommandHandler.getInstance().generate(ServerThreadPool.getServerThread(agentNum[0]+"").getOutputStream());
			break;
		case 5:
			//user_out case
			serverNum = 0;
			int clientSocket = 0;
			agentNum = new byte[2];
			byteArrServerNum = new byte[4];
			try {
				is.read(agentNum, 0, 2);//input agnet number
				is.read(byteArrServerNum, 0, 4); //input server number
				serverNum=ByteArrayToInt.byteArrayToInt(byteArrServerNum);
				byteArrServerNum = new byte[4];
				is.read(byteArrServerNum, 0, 4); //input client number
				clientSocket=ByteArrayToInt.byteArrayToInt(byteArrServerNum);
			} catch (IOException e2) {
				e2.printStackTrace();
			}
			for(int i=0;i<ServerThreadPool.threadList.size();i++){  //to all serverthread in threadpool
				OutputCommandHandler.getInstance().user_out(serverNum, clientSocket, ServerThreadPool.threadList.get(i).getOutputStream());
			}
			break;
		default:
		}
		
	}
}
