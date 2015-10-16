import java.io.IOException;
import java.util.LinkedList;

import javax.websocket.Session;

public class MessageSendHandler extends Handler{
	public MessageSendHandler(LinkedList<Room> roomlist){
		super(roomlist);
	}
	
	public void broadcastNoticeToRoom(Session session,String string) {
		for (User user : getRoom(session).getUserlist()) {
			try {
				user.getSession().getBasicRemote().sendText(string);
			} catch (IOException ex) {
				ex.printStackTrace();
			}
		}
	}
	
	public void broadcastMsgToRoom(Session session, String string) {
		for (User user : getRoom(session).getUserlist()) {
			try {
				user.getSession().getBasicRemote().sendText(getUser(session).getNickname()+" : "+string);
			} catch (IOException ex) {
				ex.printStackTrace();
			}
		}
	}

	public void broadcastToUser(Session session, String string) {
		try {
			session.getBasicRemote().sendText(string);
		} catch (IOException ex) {
			ex.printStackTrace();
		}
	}
}
