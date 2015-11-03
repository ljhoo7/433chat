package kr.co.ftt.ManageServer_receiver.util;

import kr.co.ftt.ManageServer_receiver.InputCommandHandler;
import kr.co.ftt.ManageServer_receiver.ManageServer;
import kr.co.ftt.ManageServer_receiver.OutputCommandHandler;
import kr.co.ftt.ManageServer_receiver.ServerThreadPool;

public class AutoRequester implements Runnable {

	public AutoRequester() {

	}

	public void run() {
		while (true) {
			JDBCConnect.getInstance().insertDateTime(InputCommandHandler.setTimestamp());
			
			ManageServer.toggleRecvFlag();
			JDBCConnect.getInstance().updateRecvState(ManageServer.getRecvFlag());
			
			for (int i = 0; i < ServerThreadPool.threadList.size(); i++) {
				OutputCommandHandler.getInstance().request(ServerThreadPool.threadList.get(i).getOutputStream());
			}
			ManageServer.toggleRecvFlag();
			JDBCConnect.getInstance().updateRecvState(ManageServer.getRecvFlag());
		}
	}

}
