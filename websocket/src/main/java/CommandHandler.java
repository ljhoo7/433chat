import java.util.LinkedList;

import javax.websocket.Session;

public class CommandHandler extends Handler {

	private int maxCapacity;
	private final MessageSendHandler msgHandler;

	public CommandHandler(LinkedList<Room> roomlist) {
		super(roomlist);
		msgHandler = new MessageSendHandler(roomlist);
	}

	public void parseCommand(String command, Session session) {
		if (command.startsWith("room")) {
			String roomname = command.substring(4).trim();
			int moveResult=moveRoom(session, roomname);
			if (moveResult==0) {
				msgHandler.broadcastNoticeToRoom(session, getUser(session).getNickname() + "님이 "+getRoom(session).getRoomname()+"번 방에 접속했습니다");
			} else if (moveResult==1){
				msgHandler.broadcastToUser(session, "인원이 꽉 찬 방입니다. 다른 방으로 접속해주세요.");
			} else if (moveResult==2){
				msgHandler.broadcastToUser(session, "없는 방입니다. 다른 방번호를 입력해주세요");
			}
		} else if (command.startsWith("nickname")) {
			String nickname = command.substring(8).trim();
			if (getUser(session).getNickname() != null) {
				System.out.println("[System] : 닉네임 변경 : " + getUser(session).getSession().getId() + "/"
						+ getUser(session).getNickname());
				msgHandler.broadcastNoticeToRoom(session,
						getUser(session).getNickname() + "님이 " + nickname + "(으)로 닉네임을 변경했습니다.");
			}
			getUser(session).setNickname(nickname);
		} else {
			// 추가 명령어
		}
	}

	public void addUser(Session session) {
		getRoomlist().get(0).getUserlist().add(new User(session));
	}

	public void deleteUser(Session session) {
		getRoom(session).getUserlist().remove(getUser(session));
	}

	// 유저가 소속된 방 변경
	public int moveRoom(Session session, String roomname) {
		if (getRoom(roomname) != null && !(getRoom(roomname).getRoomname().equals("0"))) {
			if (getRoom(roomname).getUserlist().size() < maxCapacity) {
				if(!(getRoom(roomname).getRoomname().equals("0")))
					msgHandler.broadcastMsgToRoom(session, getUser(session).getNickname()+"님이 "+getRoom(session).getRoomname()+"번 방을 떠났습니다.");
				getRoom(roomname).getUserlist().add(getUser(session));
				deleteUser(session);
				return 0; //정상 return
			} else {
				// 실패케이스
				return 1; //방이 꽉참
			}
		}
		return 2;//없는 방임
	}
	

	public int getMaxCapacity() {
		return maxCapacity;
	}

	public void setMaxCapacity(int maxCapacity) {
		this.maxCapacity = maxCapacity;
	}

}
