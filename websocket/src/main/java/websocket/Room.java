package websocket;
import java.io.IOException;
import java.util.LinkedList;

import javax.websocket.Session;

public class Room {
	private String roomname;
	private LinkedList<User> userlist = new LinkedList<User>();

	public Room(String roomname) {
		super();
		this.roomname = roomname;
	}
	public String getRoomname() {
		return roomname;
	}
	public void setRoomname(String roomname) {
		this.roomname = roomname;
	}
	public LinkedList<User> getUserlist() {
		return userlist;
	}
}
