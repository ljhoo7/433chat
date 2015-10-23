package websocket;
import java.io.IOException;

import javax.websocket.Session;

public class User {
	private Session session;
	private String nickname;
		
	public User(Session session, String nickname) {
		super();
		this.session = session;
		this.nickname = nickname;
	}	
	public User(Session session) {
		super();
		this.session = session;
	}	
	public Session getSession() {
		return session;
	}
	public String getNickname() {
		return nickname;
	}
	public void setNickname(String nickname) {
		this.nickname = nickname;
	}


}
