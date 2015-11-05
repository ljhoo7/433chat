package kr.co.ftt.ManageServer_receiver.util;

import kr.co.ftt.ManageServer_receiver.InputCommandHandler;
import kr.co.ftt.ManageServer_receiver.Manager;
import kr.co.ftt.ManageServer_receiver.OutputCommandHandler;
import kr.co.ftt.ManageServer_receiver.ServerThreadList;

public class AutoRequester implements Runnable {

	public AutoRequester() {

	}

	public void run() {
		while (true) {
			JDBCConnect.getInstance().insertDateTime(InputCommandHandler.setTimestamp());
			
			Manager.toggleRecvFlag();
			JDBCConnect.getInstance().updateRecvState(Manager.recvFlag);
			
			for (int i = 0; i < ServerThreadList.threadList.size(); i++) {
				OutputCommandHandler.getInstance().request(ServerThreadList.threadList.get(i).os);
			}
			Manager.toggleRecvFlag();
			JDBCConnect.getInstance().updateRecvState(Manager.recvFlag);
		}
	}

}
