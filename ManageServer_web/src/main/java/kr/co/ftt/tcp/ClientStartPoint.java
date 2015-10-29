package kr.co.ftt.tcp;

import javax.servlet.ServletContextEvent;

public class ClientStartPoint implements javax.servlet.ServletContextListener {

	@Override
	public void contextDestroyed(ServletContextEvent arg0) {
		
	}

	@Override
	public void contextInitialized(ServletContextEvent arg0) {
		
		Client.getInstance().start();
		
	}

}
