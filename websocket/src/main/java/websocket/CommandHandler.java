package websocket;

import java.io.IOException;
import java.util.LinkedList;

import javax.websocket.Session;

import tcp.CPacket;
import tcp.TcpServer;

enum ssType {
	pkt_create(0), pkt_destroy(1), pkt_join(2), pkt_leave(3), pkt_chat(4), pkt_connect(5), pkt_disconnect(
			6), pkt_heartbeats(7), pkt_heartbeats_response(8), pkt_room_info_send(9), pkt_player_info_send(
					10), pkt_room_info_success(11), pkt_room_info_failure(12), pkt_player_info_success(
							13), pkt_player_info_failure(14), pkt_sync_req(15);

	private int value;

	private ssType(int value) {
		this.value = value;
	}
};

public class CommandHandler extends Handler {

	private int maxCapacity;
	private final MessageSendHandler msgHandler;

	public CommandHandler(LinkedList<Room> roomlist) {
		super(roomlist);
		msgHandler = new MessageSendHandler(roomlist);
	}

	public void parseCommand(String command, Session session) {
		// 방
		if (command.startsWith("room")) {
			String roomname = command.substring(4).trim();
			int moveResult = moveRoom(session, roomname);
			if (moveResult == 0) {
				msgHandler.broadcastNoticeToRoom(session,
						getUser(session).getNickname() + "님이 " + getRoom(session).getRoomname() + "번 방에 접속했습니다");
			} else if (moveResult == 1) {
				msgHandler.broadcastToUser(session, "인원이 꽉 찬 방입니다. 다른 방으로 접속해주세요.");
			} else if (moveResult == 2) {
				msgHandler.broadcastToUser(session, "없는 방입니다. 다른 방번호를 입력해주세요");
			}
			// 이름
		} else if (command.startsWith("nickname")) {
			String nickname = command.substring(8).trim();
			if (getUser(session).getNickname() != null) {
				System.out.println("[System] : 닉네임 변경 : " + getUser(session).getSession().getId() + "/"
						+ getUser(session).getNickname());
				if (!(getUser(session).getNickname().equals(nickname)))
					msgHandler.broadcastNoticeToRoom(session,
							getUser(session).getNickname() + "님이 " + nickname + "(으)로 닉네임을 변경했습니다.");
			}
			getUser(session).setNickname(nickname);
			// 킥
		} else if (command.startsWith("kick")) {
			kickUserMove(session);
			msgHandler.broadcastToUser(session, "kick 명령어 입력. 대기방으로 접속합니다.");
			// 밤 생성
		} else if (command.startsWith("create")) {
			String roomname = command.substring(6).trim();
			if (createRoom(roomname)) {
				msgHandler.broadcastToUser(session, "'" + roomname + "' 방을 생성했습니다.");
				// 방을 생성함(TCP로 보냄)
				// TcpServer.pkt_create(session.getId(),roomname);
			} else {
				msgHandler.broadcastToUser(session, "'" + roomname + "' 방이 이미 존재합니다.");
			}

			// 방 폭파
		} else if (command.startsWith("destroy")) {
			String roomname = command.substring(7).trim();

			// 방을 제거함
			// TcpServer.pkt_create(session.getId(),roomname);
			if (destroyRoom(roomname)) { // 방에 제거에 대한 조건 추가해야함.
				msgHandler.broadcastToUser(session, "'" + roomname + "' 방을 제거했습니다.");
			} else {
				msgHandler.broadcastToUser(session, "'" + roomname + "' 방을 제거할 수 없습니다.");
			}
		}
	}

	public boolean createRoom(String roomname) {
		if (getRoom(roomname) == null) {
			getRoomlist().add(new Room(roomname));

			CPacket msg = CPacket.create((short) ssType.pkt_create.ordinal());
			msg.push((short) 0);// padding
			msg.push(0);
			msg.push(Short.parseShort(roomname));
			msg.push((short) 0); // padding

			TcpServer.SendAll(msg);

			System.out.print("create call : ");
			System.out.println(msg.position);
			return true;
		} else {
			return false;
		}
	}

	public boolean destroyRoom(String roomname) {
		if (getRoom(roomname) != null) {
			for (User user : getRoom(roomname).getUserlist()) {
				kickUserMove(user.getSession());
			}
			getRoomlist().remove(getRoom(roomname));

			CPacket msg = CPacket.create((short) ssType.pkt_destroy.ordinal());
			msg.push((short) 0);// padding
			msg.push(0);
			msg.push(Short.parseShort(roomname));
			msg.push((short) 0); // padding

			TcpServer.SendAll(msg);

			return true;
		} else {
			return false;
		}
	}

	public void addUser(Session session) {
		getRoom("-1").getUserlist().add(new User(session));

		// 새 대기방에 있는 유저를 추가함(TCP로 보냄)
		/*
		 * try { TcpServer.pkt_connect(session.getId(), 4); } catch (IOException
		 * e) { e.printStackTrace(); }
		 */
	}

	public void deleteUser(Session session) {
		User u = getUser(session);
		Room r = getRoom(session);
		r.getUserlist().remove(u);

		if (r.getRoomname() == "-1") {

		} else {
			System.out.println("leave call");
			
			CPacket msg = CPacket.create((short) ssType.pkt_leave.ordinal());
			msg.push((short) 0);// padding
			msg.push(Integer.parseInt(u.getSession().getId()));
			msg.push(Short.parseShort(r.getRoomname()));

			byte[] nick = new byte[20];
			String n = u.getNickname();
			if (n == null)
				n = "";
			for (int i = 0; i < n.length(); i++) {
				nick[i] = (byte) n.charAt(i);
			}

			msg.push(nick, 20);
			msg.push((short) 0);
			msg.push(0);
			TcpServer.SendAll(msg);
		}

	}

	public void deleteUser(Room room, Session session) {
		User u = getUser(session);
		Room r = room;
		r.getUserlist().remove(u);

		if (r.getRoomname() == "-1") {

		} else {
			CPacket msg = CPacket.create((short) ssType.pkt_leave.ordinal());
			msg.push((short) 0);// padding
			msg.push(Integer.parseInt(u.getSession().getId()));
			msg.push(Short.parseShort(r.getRoomname()));

			byte[] nick = new byte[20];
			String n = u.getNickname();
			if (n == null)
				n = "";
			for (int i = 0; i < n.length(); i++) {
				nick[i] = (byte) n.charAt(i);
			}

			msg.push(nick, 20);
			msg.push((short) 0);
			msg.push(0);
			
			TcpServer.SendAll(msg);
		}
	}

	// 유저가 소속된 방 변경
	public int moveRoom(Session session, String roomname) {
		if (getRoom(roomname) != null && !(getRoom(roomname).getRoomname().equals("-1"))) {
			if (getRoom(roomname).getUserlist().size() < maxCapacity) {
				if (!(getRoom(session).getRoomname().equals("-1")))
					msgHandler.broadcastMsgToRoom(session,
							getUser(session).getNickname() + "님이 " + getRoom(session).getRoomname() + "번 방을 떠났습니다.");
				Room tmpRoom = getRoom(session);
				getRoom(roomname).getUserlist().add(getUser(session));
				deleteUser(tmpRoom, session);

				CPacket msg = CPacket.create((short) ssType.pkt_join.ordinal());
				msg.push((short) 0);// padding
				msg.push(Integer.parseInt(session.getId()));
				msg.push(Short.parseShort(roomname));

				byte[] nick = new byte[20];
				String n = getUser(session).getNickname();
				if (n == null)
					n = "";
				for (int i = 0; i < n.length(); i++) {
					nick[i] = (byte) n.charAt(i);
				}

				msg.push(nick, 20);
				msg.push((short) 0);

				TcpServer.SendAll(msg);
				// 유저 방 접속(TCP로 보냄)
				/*
				 * try { TcpServer.pkt_join(session.getId(), roomname,
				 * getUser(session).getNickname()); } catch (IOException e) { //
				 * TODO Auto-generated catch block e.printStackTrace(); }
				 */

				return 0; // 정상 return
			} else {
				// 실패케이스
				return 1; // 방이 꽉참
			}
		}
		return 2;// 없는 방임
	}

	public void kickUserMove(Session session) {
		if (!(getRoom(session).getRoomname().equals("-1"))) {
			msgHandler.broadcastToUser(session, "[Kick Event] 대기방으로 이동합니다.");

			deleteUser(session);
			getRoom("-1").getUserlist().add(getUser(session));
		}
	}

	public int getMaxCapacity() {
		return maxCapacity;
	}

	public void setMaxCapacity(int maxCapacity) {
		this.maxCapacity = maxCapacity;
	}

}
