package kr.co.ftt.bean;

import java.util.ArrayList;

public class ServerInfo {
	
	int serverNum;
	int[] serverLink=new int[5];
	ArrayList<PlayerInfo> playerList=new ArrayList<PlayerInfo>();
	
	public ServerInfo(int serverNum) {
		super();
		this.serverNum = serverNum;
	}
	
	public int getServerNum() {
		return serverNum;
	}
	public void setServerNum(int serverNum) {
		this.serverNum = serverNum;
	}
	public int[] getServerLink() {
		return serverLink;
	}
	public void setServerLink(int[] serverLink) {
		this.serverLink = serverLink;
	}
	public ArrayList<PlayerInfo> getPlayerList() {
		return playerList;
	}
	public void setPlayerList(ArrayList<PlayerInfo> playerList) {
		this.playerList = playerList;
	}
	
}
