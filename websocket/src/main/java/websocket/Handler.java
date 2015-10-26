package websocket;
import java.util.LinkedList;

import javax.websocket.Session;

public class Handler {
	
	private final LinkedList<Room> roomlist;
	
	public Handler(LinkedList<Room> roomlist){
		this.roomlist=roomlist;
	}
	
	//get 관련
	public User getUser(Session session) {
		for (Room room :roomlist) {
			for(User user:room.getUserlist()){
				if(user.getSession()==session){
					return user;
				}
			}
		}
		return null;
	}
	
	public User getUser(String sessionId) {
		for (Room room :roomlist) {
			for(User user:room.getUserlist()){
				if(user.getSession().getId().equals(sessionId)){
					return user;
				}
			}
		}
		return null;
	}
	
	public Room getRoom(Session session) {
		for (Room room :roomlist) {
			for(User user:room.getUserlist()){
				if(user.getSession()==session){
					return room;
				}
			}
		}
		return null;
	}

	protected Room getRoom(String roomname) {
		for (Room room :roomlist) {
			if(room.getRoomname().equals(roomname))
				return room;
		}
		return null;
	}
	
	protected LinkedList<Room> getRoomlist() {
		return roomlist;
		
	}
}
