package kr.co.ftt.ManageServer_receiver;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.LinkedList;
import java.util.List;

import kr.co.ftt.ManageServer_receiver.bean.RoomInfo;
import kr.co.ftt.ManageServer_receiver.bean.UserInfo;
import kr.co.ftt.ManageServer_receiver.util.JDBCConnect;

public class DataManager {
	
	public static List<RoomInfo> roomList = new LinkedList<RoomInfo>();
	public static List<UserInfo> userList = new LinkedList<UserInfo>();	

	public static void addRoomInfo(byte[] byteArrRoom) {
		roomList.add(new RoomInfo(byteArrRoom[0]));
		JDBCConnect.getInstance().insertRoomInfo(InputCommandHandler.getTimestamp(), byteArrRoom[0]);
		
		System.out.println("[roomInsert]"+byteArrRoom[0]);
	}

	public static void addUserInfo(byte[] byteArrServerNum, byte[] byteArrUser, String agent_num) {
		short room_num=byteArrUser[0];
		byte[] byteArrCilentSocket={byteArrUser[4],byteArrUser[5],byteArrUser[6],byteArrUser[7]};
		int cilentSocket=byteArrayToInt(byteArrCilentSocket);
		
		//nickname
		StringBuffer nickname = new StringBuffer();
		for (int nickIndex = 8; nickIndex<26; nickIndex++) {
			if(byteArrUser[nickIndex]==0)
				break;
			nickname.append((char) byteArrUser[nickIndex]);
		}
		
		
		byte[] byteArrServerNumTmp={byteArrServerNum[0],byteArrServerNum[1],byteArrServerNum[2],byteArrServerNum[3]};
		int serverNum=byteArrayToInt(byteArrServerNumTmp);
		userList.add(new UserInfo(serverNum,cilentSocket,nickname.toString(),room_num));
		
		//db insert	
		JDBCConnect.getInstance().insertUserInfo(InputCommandHandler.getTimestamp(), cilentSocket, nickname.toString(), room_num, agent_num, serverNum);
		System.out.println("[userInsert]"+InputCommandHandler.getTimestamp()+"/"+cilentSocket+"/"+ nickname.toString()+"/"+ room_num +"/"+ agent_num +"/"+ serverNum);
	}

	public static void addServerInfo(byte[] byteArrServerNum, String agentName) {
		byte[] byteArrServerNumTmp={byteArrServerNum[0],byteArrServerNum[1],byteArrServerNum[2],byteArrServerNum[3]};
		int serverNum=byteArrayToInt(byteArrServerNumTmp);
		
		JDBCConnect.getInstance().insertServerInfo(InputCommandHandler.getTimestamp(), agentName, serverNum);
	}

	public static void addNotify(int i) {
		String msg = null;
		switch(i){
		case 0:
			msg="Server Kill Success";
			break;
			//serverkill_s
		case 1:
			msg="Server Kill Fail";	
			break;			
			//serverkill_f
		case 2:
			msg="Server Generate Success";
			break;
			//servergen_s
		case 3:
			msg="Server Generate Fail";
			break;
			//servergen_f
		}

		JDBCConnect.getInstance().insertNotify(InputCommandHandler.getTimestamp(), i, msg);
		
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

	public static void addAgentInfo(String agentName) {
		JDBCConnect.getInstance().insertAgentInfo(InputCommandHandler.getTimestamp(), agentName);
		
	}


}
