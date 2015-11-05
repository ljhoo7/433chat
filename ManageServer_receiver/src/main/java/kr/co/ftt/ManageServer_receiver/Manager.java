package kr.co.ftt.ManageServer_receiver;
import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;

import kr.co.ftt.ManageServer_receiver.util.AutoRequester;
import kr.co.ftt.ManageServer_receiver.util.JDBCConnect;
 
public class Manager {

	public static int genButtonFlag=0; //0:Generate ,1:Generating...
	public static int recvFlag=0;
	public static int autoRequesterFlag=0;

	public static void main(String[] args) {		
              
		WebLinkServer webLinkServer = new WebLinkServer();
        Thread thread = new Thread(webLinkServer);
        thread.start();
        
        try {
            //init flag state to DB
            JDBCConnect.getInstance().updateGenState(genButtonFlag);
            JDBCConnect.getInstance().updateRecvState(recvFlag);

            ServerSocket serverSocket = new ServerSocket(7000);
            
            while(true){
        		System.out.println("Agent Accept Wait..");
        		
                Socket socket = serverSocket.accept();
                if(socket != null){
                    ServerThread serverThread = new ServerThread(socket);
                                        
                    thread = new Thread(serverThread);
                    thread.start();
                    ServerThreadList.getInstance().threadList.add(serverThread);
                    
                }
                
                //Create Once
                if(autoRequesterFlag==0){
                	autoRequesterFlag=1;
                	AutoRequester autoRequester = new AutoRequester();
                	thread = new Thread(autoRequester);
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