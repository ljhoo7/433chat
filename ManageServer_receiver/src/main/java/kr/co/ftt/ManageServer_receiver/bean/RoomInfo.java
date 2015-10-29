package kr.co.ftt.ManageServer_receiver.bean;

public class RoomInfo {
	private short room_num;

	public RoomInfo(byte byteArrRoom) {
		super();
		this.room_num = byteArrRoom;
	}
	
	public short getRoom_num() {
		return room_num;
	}

	public void setRoom_num(short room_num) {
		this.room_num = room_num;
	}
	
}
