package kr.co.ftt.ManageServer_receiver;
import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;

import kr.co.ftt.ManageServer_receiver.util.AutoReceiver;
import kr.co.ftt.ManageServer_receiver.util.JDBCConnect;
 
public class ManageServer {

	private static int genButtonFlag=0; //0이면 생성가능 1이면 생성불가(success/fail이 들어왔을 때 변경가능)
	private static int recvFlag=0;
	
	public static int getRecvFlag() {
		return recvFlag;
	}
    public static int getGenButtonFlag() {
		return genButtonFlag;
	}
	public static void main(String[] args) {
		
        ServerSocket serverSocket = null;
        
        int autoFlag=0;
        
        WebLinkServer webLink = new WebLinkServer();
        
        Thread thread = new Thread(webLink);
        thread.start();
        
        try {        	
            serverSocket = new ServerSocket(7000);
            JDBCConnect.getInstance().updateGenState(genButtonFlag);
            JDBCConnect.getInstance().updateRecvState(recvFlag);
            while(true){
        		System.out.println("Agent 접속대기");
        		
                Socket socket = serverSocket.accept();
                System.out.println("socket.hashCode()"+socket.hashCode());
                if(socket != null){
                    ServerThread serverThread = new ServerThread(socket);
                                        
                    thread = new Thread(serverThread);
                    thread.start();
                    ServerThreadPool.add(serverThread);
                    
                }
                if(autoFlag==0){
                	autoFlag=1;
                	AutoReceiver autoReceiver = new AutoReceiver();
                	thread = new Thread(autoReceiver);
                	thread.start();                	
                }

            }
        } catch (IOException e1) {
            e1.printStackTrace();
        }
         
         
    }
    public static void toggleGenbuttonFlag(){
    	if(genButtonFlag==0){
    		genButtonFlag=1;
    	}
    	else{
    		genButtonFlag=0;
    	}
    }
    public static void toggleRecvFlag(){
    	if(recvFlag==0){
    		recvFlag=1;
    	}
    	else{
    		recvFlag=0;
    	}
    }
}