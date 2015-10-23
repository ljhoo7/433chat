package tcp;


import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;

public class ListenSocket extends Thread {
	int port;
	public ListenSocket(int port){
		this.port = port;
	}
	
	public void run() {
		System.out.print("listen start port : ");
		System.out.println(port);
		 ServerSocket serverSocket = null;
         
	        try {
	            serverSocket = new ServerSocket(port);
	            while(true){
	                Socket socket = serverSocket.accept();
	                 
	                if(socket != null){
	                    InterSocket interSocket = new InterSocket(socket, -1, false);
	                }
	            }
	        } catch (IOException e1) {
	            // TODO Auto-generated catch block
	            e1.printStackTrace();
	        }
	}
}
