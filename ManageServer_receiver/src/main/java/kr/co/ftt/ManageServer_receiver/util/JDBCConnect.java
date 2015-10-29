package kr.co.ftt.ManageServer_receiver.util;


import java.sql.*;

public class JDBCConnect {

	private static JDBCConnect dbconnect;
	
	static Connection con = null;
	
	Statement st = null;
	ResultSet rs = null;
	
	static String dbAddress = "jdbc:mysql://localhost/manage";
	static String dbUser = "root";
	static String dbPass = "1234";	
	
	private JDBCConnect() {
		try {
			con = DriverManager.getConnection(dbAddress, dbUser, dbPass);
			st = con.createStatement();
		} catch (SQLException e) {
			e.printStackTrace();
		}
		
	}

	public static JDBCConnect getInstance() {
		if (dbconnect == null) {
			dbconnect = new JDBCConnect(); 
		}
		return dbconnect;
	}
	
	public void insertAgentInfo(String timestamp,String agentNum){
		String query = " insert into agentinfo (agentinfo_timestamp, agentinfo_agentnum)"
		        + " values (?, ?)";

		PreparedStatement preparedStmt;
		try {
			preparedStmt = JDBCConnect.getInstance().getConnection().prepareStatement(query);
			preparedStmt.setString(1, timestamp);
			preparedStmt.setString(2, agentNum);
			
			preparedStmt.execute();
		} catch (SQLException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
	}
	
	public void insertRoomInfo(String timestamp,int roomNum){
		String query = " insert into roominfo (roominfo_timestamp, roominfo_room_num)"
		        + " values (?, ?)";

		PreparedStatement preparedStmt;
		try {
			preparedStmt = JDBCConnect.getInstance().getConnection().prepareStatement(query);
			preparedStmt.setString(1, timestamp);
			preparedStmt.setInt(2, roomNum);
			
			preparedStmt.execute();
		} catch (SQLException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
	}
	
	public void insertServerInfo(String timestamp,String agentName, int serverNum){
		String query = " insert into serverinfo (serverinfo_timestamp, serverinfo_agentname, serverinfo_servernum)"
		        + " values (?, ?, ?)";

		PreparedStatement preparedStmt;
		try {
			preparedStmt = JDBCConnect.getInstance().getConnection().prepareStatement(query);
			preparedStmt.setString(1, timestamp);
			preparedStmt.setString(2, agentName);
			preparedStmt.setInt(3, serverNum);
			
			preparedStmt.execute();
		} catch (SQLException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
	}
	

	public void insertUserInfo(String timestamp,int client_socket,String nickname,int roomNum,String agentNum, int serverNum){
		String query = " insert into userinfo (userinfo_timestamp, userinfo_client_socket, userinfo_nickname, userinfo_room_num, userinfo_agent_num,userinfo_server_num)"
		        + " values (?, ?, ?, ?, ?, ?)";

		PreparedStatement preparedStmt;
		try {
			preparedStmt = JDBCConnect.getInstance().getConnection().prepareStatement(query);
			preparedStmt.setString(1, timestamp);
			preparedStmt.setInt(2, client_socket);
			preparedStmt.setString(3, nickname);
			preparedStmt.setInt(4, roomNum);
			preparedStmt.setString(5, agentNum);
			preparedStmt.setInt(6, serverNum);

			preparedStmt.execute();
		} catch (SQLException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}		
	}
	
	public void insertDateTime(String timestamp){
		String query = " insert into timestampinfo (timestampinfo_date)"
		        + " values (?)";

		PreparedStatement preparedStmt;
		try {
			preparedStmt = JDBCConnect.getInstance().getConnection().prepareStatement(query);
			preparedStmt.setString(1, timestamp);

			preparedStmt.execute();
		} catch (SQLException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}		
	}
	
	
	
	public Connection getConnection(){
		return con;
	}

	public void insertNotify(String timestamp, int i, String msg) {
		String query = " insert into notification (notification_type,notification_message,notification_timestamp)"
		        + " values (?,?,?)";

		PreparedStatement preparedStmt;
		try {
			preparedStmt = JDBCConnect.getInstance().getConnection().prepareStatement(query);
			preparedStmt.setInt(1, i);
			preparedStmt.setString(2, msg);
			preparedStmt.setString(3, timestamp);

			preparedStmt.execute();
		} catch (SQLException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}		
		
	}
	
	public void updateGenState(int i) {
		String query = " UPDATE state SET gen_state=?";

		PreparedStatement preparedStmt;
		try {
			preparedStmt = JDBCConnect.getInstance().getConnection().prepareStatement(query);
			preparedStmt.setInt(1, i);

			preparedStmt.execute();
		} catch (SQLException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}		
		
	}
	public void updateRecvState(int i) {
		String query = " UPDATE state SET receive_time=?";

		PreparedStatement preparedStmt;
		try {
			preparedStmt = JDBCConnect.getInstance().getConnection().prepareStatement(query);
			preparedStmt.setInt(1, i);

			preparedStmt.execute();
		} catch (SQLException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}		
		
	}
}