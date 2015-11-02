package kr.co.ftt.ManageServer_receiver;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.Socket;
import java.net.SocketException;

public class ServerThread implements Runnable {

	private Socket socket;
	private String agentName;
	private InputStream is;
	private OutputStream os;

	public ServerThread(Socket socket) {
		this.socket = socket;
		System.out.println(socket.getInetAddress().getHostAddress() + " agent Connected");
		try {
			is = socket.getInputStream();
			os = socket.getOutputStream();
		} catch (IOException e) {
			e.printStackTrace();
		}
	}

	public void setAgentName(String agentName) {
		this.agentName = agentName;
	}

	public String getAgentName() {
		return agentName;
	}
	
	public void run() {
		try {
			byte[] input;
			
			setAgentName(ServerThreadPool.getNextAgnetNum()+"");

			while (true) {
				//System.out.println(getAgentName()+"Agent input wait...");
				input = new byte[2];
				is.read(input, 0, 2);

				short type = input[0];
				
				InputCommandHandler.getInstance().typeInput(type, this);
			}
		} catch (SocketException e){
			System.out.println(getAgentName() + "Agent Disconnected");
		} catch (Exception e) {
			e.printStackTrace();
		} finally {
			if (socket != null) {
				try {
					socket.close();
					System.out.println(getAgentName()+"Agent closed");
				} catch (IOException e) {
					e.printStackTrace();
				}
			}
			ServerThreadPool.remove(this);
		}
	}
	
	public InputStream getInputStream(){
		return is;
	}
	public OutputStream getOutputStream(){
		return os;
	}
}