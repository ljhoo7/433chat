package kr.co.ftt.ManageServer_receiver.util;

public class CurrentTime {
	
	public static String getCurrentTime(){
		
		java.util.Date dt = new java.util.Date();

		java.text.SimpleDateFormat sdf = 
		     new java.text.SimpleDateFormat("yyyy-MM-dd HH:mm:ss");

		return sdf.format(dt);
	}

}
