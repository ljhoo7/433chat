package websocket;

import java.io.IOException;
import java.util.ArrayList;
import java.util.LinkedList;

import javax.servlet.ServletContextEvent;
import javax.websocket.OnClose;
import javax.websocket.OnMessage;
import javax.websocket.OnOpen;
import javax.websocket.Session;
import javax.websocket.server.ServerEndpoint;

import tcp.CPacket;
import tcp.PlayerInfo;
import tcp.TcpServer;

/**
 * @ServerEndpoint gives the relative name for the end point This will be
 *                 accessed via ws://localhost:8080/EchoChamber/echo Where
 *                 "localhost" is the address of the host, "EchoChamber" is the
 *                 name of the package and "echo" is the address to access this
 *                 class from the server
 */
@ServerEndpoint("/echo")
public class Server implements javax.servlet.ServletContextListener {

	public static final LinkedList<Room> roomlist = new LinkedList<Room>();

	public static final CommandHandler comHandler = new CommandHandler(roomlist); // MAX_CAPACITY
	public static final MessageSendHandler msgHandler = new MessageSendHandler(roomlist);

	/**
	 * @OnOpen allows us to intercept the creation of a new session. The session
	 *         class allows us to send data to the user. In the method onOpen,
	 *         we'll let the user know that the handshake was successful.
	 */

	@OnOpen
	public void onOpen(Session session) {

		System.out.println("[System] : " + session.getId() + " has opened a connection");
		comHandler.addUser(session);

		CPacket msg = CPacket.create((short) ssType.pkt_connect.ordinal());
		msg.push((short) 0);// padding
		msg.push(Integer.parseInt(session.getId()));
		msg.push(TcpServer.serverNum);
		TcpServer.SendAll(msg);
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
		System.out.println(("[System] : Input Message from " + session.getId() + ":" + message));
		// 만일 명령어가 /set으로 시작하면 캐치해서 설정 한다.
		if (message.startsWith("/set")) {
			comHandler.parseCommand(message.substring(4).trim(), session);
		} else {
			msgHandler.broadcastMsgToRoom(session, message);

			int length = 0;

			CPacket msg = CPacket.create((short) ssType.pkt_chat.ordinal());

			User user = msgHandler.getUser(session);
			Room room = msgHandler.getRoom(session);

			msg.push((short) 0);// length
			msg.push(Short.parseShort(room.getRoomname()));
			length += 2;

			msg.push((short) 0);// padding
			length += 2;

			msg.push(0); // token;
			length += 4;
			
			byte[] nick = new byte[20];
			String n = user.getNickname();
			if (n == null)
				n = "";
			for (int i = 0; i < n.length(); i++) {
				nick[i] = (byte) n.charAt(i);
			}

			msg.push(nick, 20);
			length += 20;
			
			
			byte[] chat = new byte[message.length()];
			for (int i = 0; i < chat.length; i++) {
				chat[i] = (byte) message.charAt(i);
			}
			msg.push(chat, message.length());
			msg.push((byte)0);
			length+=message.length() + 1;
			/*msg.push(message);
			length += message.length() + 1;*/
			length+=4;
			msg.record_size(2, (short) length);

			System.out.println(length);
			TcpServer.SendAll(msg);
		}
	}

	/**
	 * The user closes the connection.
	 * 
	 * Note: you can't send messages to the client from this method
	 */
	@OnClose
	public void onClose(Session session) {
		// 유저 삭제 명령을 전송
		comHandler.deleteUser(session);
		CPacket msg = CPacket.create((short) ssType.pkt_disconnect.ordinal());
		msg.push((short) 0);// padding
		msg.push(Integer.parseInt(session.getId()));
		msg.push(TcpServer.serverNum);
		TcpServer.SendAll(msg);
		System.out.println("[System] : Session " + session.getId() + " has ended");
	}

	public void contextDestroyed(ServletContextEvent arg0) {
		// TODO Auto-generated method stub

	}

	public void contextInitialized(ServletContextEvent arg0) {
		roomlist.add(new Room("-1"));
		/*
		 * for (int i = 0; i < 11; i++) { roomlist.add(new Room(i + "")); }
		 */
		System.out.println("[System] : 방(채널) 생성완료");
		comHandler.setMaxCapacity(10);

		// 이하는 TCP 설정
		TcpServer tcpServer = new TcpServer("127.0.0.1");
		tcpServer.start();
	}

	public static int getUserCnt() {
		int cnt = 0;
		for (Room r : roomlist) {
			cnt += r.getUserlist().size();
		}
		return cnt;
	}

	public static int getRoomCnt() {
		return roomlist.size();
	}
}