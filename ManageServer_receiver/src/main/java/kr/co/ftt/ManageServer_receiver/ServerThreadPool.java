package kr.co.ftt.ManageServer_receiver;

import java.util.LinkedList;
import java.util.List;
 
public class ServerThreadPool {
	private static int agentNum=0;
	
	public static int getNextAgnetNum(){
		return ++agentNum;
	}
	
    public static List<ServerThread> threadList = new LinkedList<ServerThread>();
     
    public static void add(ServerThread thread){
    	threadList.add(thread);
    }
     
    public static void remove(ServerThread thread){
    	threadList.remove(thread);
    }
    
    public static ServerThread getServerThread(String agentName){
    	for(ServerThread thread: threadList){
    		if(thread.getAgentName().equals(agentName)){
    			return thread;
    		}
    	}
		return null;
    }
}