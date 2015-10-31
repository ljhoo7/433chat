package kr.co.ftt.ManageServer_receiver;

import java.io.IOException;
import java.util.ArrayList;
import java.util.LinkedList;
import java.util.PriorityQueue;
import java.util.concurrent.LinkedBlockingDeque;

import kr.co.ftt.ManageServer_receiver.util.CurrentTime;
import kr.co.ftt.ManageServer_receiver.util.JDBCConnect;

public class InputCommandHandler {
	public final static short user_out = 0;
	public final static short room_destroy = 1;
	public final static short kill_server = 2;
	public final static short request = 3;

	byte[] byteArrCnt = new byte[2];
	byte[] byteArrRoom = new byte[2];
	byte[] byteArrUser = new byte[28];
	byte[] byteArrInterserver = new byte[8];
	byte[] byteArrAgentNum = new byte[2];
	byte[] byteArrServerNum = new byte[4];
	byte[] byteArrFailSignal = new byte[2];
	
	private static int serverInputCnt = 0;
	private static String timestamp;
	
	public static String getTimestamp() {
		return timestamp;
	}
	
	public static String setTimestamp(){
		timestamp=CurrentTime.getCurrentTime();
		return timestamp;
	}
	
	short obj_cnt;
	
	private static InputCommandHandler instance;

	private InputCommandHandler() {
	}

	public static InputCommandHandler getInstance() {
		if (instance == null) {
			instance = new InputCommandHandler();
		}
		return instance;
	}

	public void typeInput(int type, ServerThread serverThread) {
		switch (type) {
		case 0:
			byteArrCnt = new byte[2];
			try {
				serverThread.getInputStream().read(byteArrCnt, 0, 2);
			} catch (IOException e2) {
				e2.printStackTrace();
			}
			obj_cnt = byteArrCnt[0];
			System.out.println("들어온 토탈룸 cnt:"+obj_cnt);
			for (int i = 0; i < obj_cnt; i++) {
				try {
					serverThread.getInputStream().read(byteArrRoom, 0, 2);
				} catch (IOException e) {
					e.printStackTrace();
				}
				DataManager.addRoomInfo(byteArrRoom);
			}
			// total_room_info
			break;
		case 1:
			byteArrCnt = new byte[2];
			try {
				serverThread.getInputStream().read(byteArrCnt, 0, 2);
			} catch (IOException e2) {
				e2.printStackTrace();
			}
			obj_cnt = byteArrCnt[0];
			
			System.out.println("들어온 토탈서버 cnt:"+obj_cnt);
			DataManager.addAgentInfo(serverThread.getAgentName());
			
			//total_server_count
			break;
		case 2:
			byteArrCnt = new byte[2];
			byteArrServerNum = new byte[4];
			byteArrUser = new byte[28];
			try {
				serverThread.getInputStream().read(byteArrServerNum, 0, 4);
			} catch (IOException e2) {
				e2.printStackTrace();
			}
			DataManager.addServerInfo(byteArrServerNum,serverThread.getAgentName());
			
			try {
				serverThread.getInputStream().read(byteArrCnt, 0, 2);
			} catch (IOException e2) {
				e2.printStackTrace();
			}

			obj_cnt = byteArrCnt[0];

			System.out.println("들어온 유저인포 cnt:"+obj_cnt);
			
			for (int i = 0; i < obj_cnt; i++) {
				try {
					serverThread.getInputStream().read(byteArrUser, 0, 28);
				} catch (IOException e) {
					e.printStackTrace();
				}
				DataManager.addUserInfo(byteArrServerNum,byteArrUser,serverThread.getAgentName());
			}
			// total_user_info
			break;
		case 3:
			System.out.println("user_out_success");
			// user_out_success
			break;
		case 4:
			byteArrFailSignal = new byte[2];
			try {
				serverThread.getInputStream().read(byteArrFailSignal, 0, 2);
			} catch (IOException e2) {
				e2.printStackTrace();
			}
			System.out.println("user_out_fail / type:"+byteArrFailSignal[0]);
			// user_out_fail
			break;
		case 5:
			System.out.println("room_destroy_success");
			// room_destroy_success
			break;
		case 6:
			byteArrFailSignal = new byte[2];
			try {
				serverThread.getInputStream().read(byteArrFailSignal, 0, 2);
			} catch (IOException e2) {
				e2.printStackTrace();
			}
			System.out.println("room_destory_fail / type:"+byteArrFailSignal[0]);
			// room_destory_fail
			break;
		case 7:
			System.out.println("kill_server_success");
			DataManager.addNotify(0);
			// kill_server_success
			break;
		case 8:
			System.out.println("kill_server_fail");
			byteArrFailSignal = new byte[2];
			try {
				serverThread.getInputStream().read(byteArrFailSignal, 0, 2);
			} catch (IOException e2) {
				e2.printStackTrace();
			}
			System.out.println("room_destory_fail / type:"+byteArrFailSignal[0]);
			DataManager.addNotify(1);
			// kill_server_fail
			break;
		case 9:
			System.out.println("generate_server_success");
			DataManager.addNotify(2);
			ManageServer.toggleGenbuttonFlag();
	        JDBCConnect.getInstance().updateGenState(ManageServer.getGenButtonFlag());
			// kill_server_fail
			break;
		case 10:
			System.out.println("generate_server_fail");
			byteArrFailSignal = new byte[2];
			try {
				serverThread.getInputStream().read(byteArrFailSignal, 0, 2);
			} catch (IOException e2) {
				e2.printStackTrace();
			}
			System.out.println("generate_server_fail / type:"+byteArrFailSignal[0]);
			DataManager.addNotify(3);
			ManageServer.toggleGenbuttonFlag();
	        JDBCConnect.getInstance().updateGenState(ManageServer.getGenButtonFlag());

			// kill_server_fail
			break;		
		}
	}
}
