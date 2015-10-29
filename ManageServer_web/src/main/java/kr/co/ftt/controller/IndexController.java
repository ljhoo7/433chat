package kr.co.ftt.controller;

import java.text.DateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.HashMap;
import java.util.List;
import java.util.Locale;
import java.util.Map;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.jdbc.core.JdbcTemplate;
import org.springframework.stereotype.Controller;
import org.springframework.ui.Model;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestMethod;
import org.springframework.web.servlet.ModelAndView;

import kr.co.ftt.bean.ServerInfo;
import kr.co.ftt.model.DataManager;

@Controller
public class IndexController {
	@Autowired
	JdbcTemplate template;
	
	ArrayList<ServerInfo> serverList;
	private static final Logger logger = LoggerFactory.getLogger(IndexController.class);
	
	@RequestMapping(value = "/", method = RequestMethod.GET)
	public ModelAndView home(ModelAndView mav) {
		
		DataManager.getInstance().setTemplate(template);
		
		while(true){
			String recvState=DataManager.getInstance().selectToRecvState("select * from state");

			if(Integer.parseInt(recvState)==0){
				try {
					Thread.sleep(50);
				} catch (InterruptedException e) {
					e.printStackTrace();
				}
				break;
			}
		}
		
		String serverCnt = DataManager.getInstance().selectToCount("select count(*) from (SELECT * FROM manage.serverinfo where serverinfo_timestamp =(SELECT timestampinfo_date FROM timestampinfo ORDER BY timestampinfo_date DESC LIMIT 1) group by serverinfo_servernum)a;");
		String userCnt = DataManager.getInstance().selectToCount("select count(*) from manage.userinfo where userinfo_timestamp =(SELECT timestampinfo_date FROM timestampinfo ORDER BY timestampinfo_date DESC LIMIT 1)");
		String roomCnt = DataManager.getInstance().selectToCount("select count(*) from(SELECT * FROM manage.roominfo where roominfo_timestamp =(SELECT timestampinfo_date FROM timestampinfo ORDER BY timestampinfo_date DESC LIMIT 1) group by roominfo_room_num)a");
		String agentCnt = DataManager.getInstance().selectToCount("select count(*) from (SELECT * FROM manage.agentinfo where agentinfo_timestamp =(SELECT timestampinfo_date FROM timestampinfo ORDER BY timestampinfo_date DESC LIMIT 1) group by agentinfo_agentnum)a");

		String[][] serverList=new String[Integer.parseInt(agentCnt)][Integer.parseInt(serverCnt)+1];
		String[][] userList=new String[Integer.parseInt(agentCnt)][Integer.parseInt(userCnt)+1];
		String[][] clientSocket=new String[Integer.parseInt(agentCnt)][Integer.parseInt(userCnt)+1];
		String[][] clientServer=new String[Integer.parseInt(agentCnt)][Integer.parseInt(userCnt)+1];
		String[] agentName=new String[Integer.parseInt(agentCnt)+1];
		
		String[] overview = new String[4];
		overview[0]=serverCnt;//server
		overview[1]=userCnt;//user
		overview[2]=roomCnt;//room
		overview[3]=agentCnt;//Agent
		
		//agentName
		List<Map<String, Object>> agentNameList = DataManager.getInstance().selectToList("SELECT agentinfo_agentnum FROM manage.agentinfo where agentinfo_timestamp =(SELECT timestampinfo_date FROM timestampinfo ORDER BY timestampinfo_date DESC LIMIT 1) group by agentinfo_agentnum");
		for(int i=0;i<Integer.parseInt(agentCnt);i++){
			agentName[i]=agentNameList.get(i).get("agentinfo_agentnum").toString();			
			
			//serverList
			List<Map<String, Object>> serverNameList = DataManager.getInstance().selectToList("SELECT serverinfo_servernum FROM manage.serverinfo where serverinfo_timestamp =(SELECT timestampinfo_date FROM timestampinfo ORDER BY timestampinfo_date DESC LIMIT 1) and serverinfo_agentname = "+agentName[i]+" group by serverinfo_servernum");
			int j=1; //0은 총 숫자
			for(Map<String, Object> server : serverNameList){
				serverList[i][j++]=server.get("serverinfo_servernum").toString();
			}
			serverList[i][0]=(j-1)+"";

			//userList
			List<Map<String, Object>> userNameList = DataManager.getInstance().selectToList("SELECT userinfo_nickname FROM manage.userinfo where userinfo_timestamp =(SELECT timestampinfo_date FROM timestampinfo ORDER BY timestampinfo_date DESC LIMIT 1) and userinfo_agent_num = "+agentName[i]);
			j=1; //0은 총 숫자
			for(Map<String, Object> user : userNameList){
				userList[i][j++]=user.get("userinfo_nickname").toString();
			}
			userList[i][0]=(j-1)+"";

			//clientSocket
			List<Map<String, Object>> clientSocketList = DataManager.getInstance().selectToList("SELECT userinfo_client_socket FROM manage.userinfo where userinfo_timestamp =(SELECT timestampinfo_date FROM timestampinfo ORDER BY timestampinfo_date DESC LIMIT 1) and userinfo_agent_num = "+agentName[i]);
			j=1; //0은 총 숫자
			for(Map<String, Object> user : clientSocketList){
				clientSocket[i][j++]=user.get("userinfo_client_socket").toString();
			}
			clientSocket[i][0]=(j-1)+"";

			//clientServer
			List<Map<String, Object>> clientServerList = DataManager.getInstance().selectToList("SELECT userinfo_server_num FROM manage.userinfo where userinfo_timestamp =(SELECT timestampinfo_date FROM timestampinfo ORDER BY timestampinfo_date DESC LIMIT 1) and userinfo_agent_num = "+agentName[i]);
			j=1; //0은 총 숫자
			for(Map<String, Object> user : clientServerList){
				clientServer[i][j++]=user.get("userinfo_server_num").toString();
			}
			clientServer[i][0]=(j-1)+"";
		}

		// 전체정보 받아오는 부분
		String[] allServer = new String[Integer.parseInt(serverCnt)];
		String[] allRoomname = new String[Integer.parseInt(roomCnt)];
		String[][] allUser = new String[Integer.parseInt(roomCnt)][Integer.parseInt(userCnt)+1];
		
		List<Map<String, Object>> allServerList = DataManager.getInstance().selectToList("SELECT serverinfo_servernum FROM manage.serverinfo where serverinfo_timestamp =(SELECT timestampinfo_date FROM timestampinfo ORDER BY timestampinfo_date DESC LIMIT 1) group by serverinfo_servernum");
		int i=0;
		for(Map<String, Object> server : allServerList){
			allServer[i++]=server.get("serverinfo_servernum").toString();
		}
		List<Map<String, Object>> allRoomList = DataManager.getInstance().selectToList("SELECT roominfo_room_num FROM manage.roominfo where roominfo_timestamp =(SELECT timestampinfo_date FROM timestampinfo ORDER BY timestampinfo_date DESC LIMIT 1) group by roominfo_room_num");
		
		for(int roomIndex=0;roomIndex<Integer.parseInt(roomCnt);roomIndex++){
			allRoomname[roomIndex]=allRoomList.get(roomIndex).get("roominfo_room_num")+"";
			
			List<Map<String, Object>> allUserList = DataManager.getInstance().selectToList("SELECT * FROM manage.userinfo where userinfo_timestamp =(SELECT timestampinfo_date FROM timestampinfo ORDER BY timestampinfo_date DESC LIMIT 1) and userinfo_room_num="+allRoomname[roomIndex]);
			int userIndex=1; //0은 총 숫자
			for(Map<String, Object> user : allUserList){
				allUser[roomIndex][userIndex++]=user.get("userinfo_nickname").toString();
			}
			allUser[roomIndex][0]=(userIndex-1)+"";			
		}
		
		//notify
		List<Map<String, Object>> notifyList = DataManager.getInstance().selectToList("SELECT * FROM notification ORDER BY notification_timestamp DESC LIMIT 10");
		
		String[][] notify = new String[11][3];
		
		int notifyIndex = 1;
		for(Map<String, Object> note : notifyList){
			notify[notifyIndex][0]=note.get("notification_type").toString();
			notify[notifyIndex][1]=note.get("notification_message").toString();
			notify[notifyIndex++][2]=note.get("notification_timestamp").toString();
		}
		notify[0][0]=(notifyIndex-1)+"";
		
		String genState=DataManager.getInstance().selectToGenState("select * from state");

		mav.addObject("genState", genState);
		mav.addObject("notify", notify);
		mav.addObject("clientServer", clientServer);
		mav.addObject("clientSocket", clientSocket);
		mav.addObject("userList", userList);
		mav.addObject("allRoomname", allRoomname);
		mav.addObject("allUser", allUser);
		mav.addObject("allServer", allServer);
		mav.addObject("serverList", serverList);
		mav.addObject("agentName", agentName);
		mav.addObject("overview", overview);
		mav.setViewName("index");
		
	
		return mav;
	}
	
}
