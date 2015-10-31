package kr.co.ftt.ManageServer_receiver.util;

import kr.co.ftt.ManageServer_receiver.InputCommandHandler;
import kr.co.ftt.ManageServer_receiver.ManageServer;
import kr.co.ftt.ManageServer_receiver.OutputCommandHandler;
import kr.co.ftt.ManageServer_receiver.ServerThreadPool;

public class AutoReceiver implements Runnable {

	public AutoReceiver() {

	}

	public void run() {
		while (true) {
			JDBCConnect.getInstance().insertDateTime(InputCommandHandler.setTimestamp());
			
			ManageServer.toggleRecvFlag();
			JDBCConnect.getInstance().updateRecvState(ManageServer.getRecvFlag());
			
			for (int i = 0; i < ServerThreadPool.arrayList.size(); i++) {
				OutputCommandHandler.getInstance().request(ServerThreadPool.arrayList.get(i).getOutputStream());
				try {
					Thread.sleep(300);
				} catch (InterruptedException e) {
					e.printStackTrace();
				}
			}
			try {
				Thread.sleep(100);
				ManageServer.toggleRecvFlag();
				JDBCConnect.getInstance().updateRecvState(ManageServer.getRecvFlag());
				Thread.sleep(3500);
			} catch (InterruptedException e) {
				e.printStackTrace();
			}
		}
	}

}
