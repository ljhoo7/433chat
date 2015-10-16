import java.io.IOException;
import java.util.LinkedList;

import javax.servlet.ServletContextEvent;
import javax.websocket.OnClose;
import javax.websocket.OnMessage;
import javax.websocket.OnOpen;
import javax.websocket.Session;
import javax.websocket.server.ServerEndpoint;

/**
 * @ServerEndpoint gives the relative name for the end point This will be
 *                 accessed via ws://localhost:8080/EchoChamber/echo Where
 *                 "localhost" is the address of the host, "EchoChamber" is the
 *                 name of the package and "echo" is the address to access this
 *                 class from the server
 */
@ServerEndpoint("/echo")
public class Server implements javax.servlet.ServletContextListener{

	private static final LinkedList<Room> roomlist = new LinkedList<Room>();

	private static final CommandHandler comHandler= new CommandHandler(roomlist); //MAX_CAPACITY
	private static final MessageSendHandler msgHandler= new MessageSendHandler(roomlist);
	
	/**
	 * @OnOpen allows us to intercept the creation of a new session. The session
	 *         class allows us to send data to the user. In the method onOpen,
	 *         we'll let the user know that the handshake was successful.
	 */

	@OnOpen
	public void onOpen(Session session) {

		System.out.println("[System] : " + session.getId()
				+ " has opened a connection");
		comHandler.addUser(session);
		msgHandler.broadcastToUser(session, "대기방에 접속했습니다. /set room 명령어로 방을 이동 할 수 있습니다.");
	}

	/**
	 * When a user sends a message to the server, this method will intercept the
	 * message and allow us to react to it. For now the message is read as a
	 * String.
	 */
	@OnMessage
 	public void onMessage(String message, Session session) {
		// 명령어가 들어오는 경우
		System.out.println(("[System] : Input Message from "
				+ session.getId() + ":" + message));
		// 만일 명령어가 /set으로 시작하면 캐치해서 설정 한다.
		if (message.startsWith("/set")) {
			comHandler.parseCommand(message.substring(4).trim(),session);
		} else {
			msgHandler.broadcastMsgToRoom(session, message);
		}
	}

	/**
	 * The user closes the connection.
	 * 
	 * Note: you can't send messages to the client from this method
	 */
	@OnClose
	public void onClose(Session session) {
		//유저 삭제 명령을 전송
		comHandler.deleteUser(session);
		System.out.println("[System] : Session " + session.getId() + " has ended");
		
	}

	@Override
	public void contextDestroyed(ServletContextEvent arg0) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void contextInitialized(ServletContextEvent arg0) {
		
			for (int i = 0; i < 11; i++) {
				roomlist.add(new Room(i + ""));
			}
			System.out.println("[System] : 방(채널) 생성완료");
			comHandler.setMaxCapacity(3);
			
	}


}