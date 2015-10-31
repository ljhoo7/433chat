package kr.co.ftt.ManageServer_receiver;
 
 
import java.util.ArrayList;
 
public class ServerThreadPool {
	private static int agentNum=0;
	
	public static int getNextAgnetNum(){
		return ++agentNum;
	}
	
    public static ArrayList<ServerThread> arrayList = new ArrayList<ServerThread>();
     
    public static void add(ServerThread thread){
        arrayList.add(thread);
    }
     
    public static void remove(ServerThread thread){
            arrayList.remove(thread);
    }
    
    public static ServerThread getServerThread(String agentName){
    	for(ServerThread thread: arrayList){
    		if(thread.getAgentName().equals(agentName)){
    			return thread;
    		}
    	}
		return null;
    }
}