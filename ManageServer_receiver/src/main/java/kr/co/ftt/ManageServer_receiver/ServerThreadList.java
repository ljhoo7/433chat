package kr.co.ftt.ManageServer_receiver;

import java.util.LinkedList;
import java.util.List;
 
public class ServerThreadList {
	
	private static ServerThreadList instance;

	public static ServerThreadList getInstance() {
		if (instance == null) {
			instance = new ServerThreadList();
		}
		return instance;
	}

    public static List<ServerThread> threadList = new LinkedList<ServerThread>();
	
	private static int agentNum=0;
	
	public static int getNextAgnetNum(){
		return ++agentNum;
	}	

    public static ServerThread getServerThread(String agentName){
    	for(ServerThread thread: threadList){
    		if(thread.agentName.equals(agentName)){
    			return thread;
    		}
    	}
		return null;
    }
}