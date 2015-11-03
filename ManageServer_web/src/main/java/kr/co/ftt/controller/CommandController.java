package kr.co.ftt.controller;

import java.text.DateFormat;
import java.util.Date;
import java.util.List;
import java.util.Locale;
import java.util.Map;

import javax.servlet.http.HttpServletRequest;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.http.client.ClientHttpRequest;
import org.springframework.jdbc.core.JdbcTemplate;
import org.springframework.stereotype.Controller;
import org.springframework.ui.Model;
import org.springframework.web.bind.annotation.PathVariable;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestMethod;
import org.springframework.web.servlet.ModelAndView;

import kr.co.ftt.model.DataManager;
import kr.co.ftt.tcp.Client;

/**
 * Handles requests for the application home page.
 */
@Controller
public class CommandController {

	@Autowired
	JdbcTemplate template;

	private static final Logger logger = LoggerFactory.getLogger(CommandController.class);


	@RequestMapping(value = "/get", method = RequestMethod.GET)
	public String get(HttpServletRequest request) {
		Client.getInstance().pt_request_total_info();
		System.out.println("get");
		
		return "return";
	}

	@RequestMapping(value = "/out/{serverNum}/{playerSocket}", method = RequestMethod.GET)
	public String outPlayer(ModelAndView mav, @PathVariable int serverNum, @PathVariable String playerSocket,
			HttpServletRequest request) {
		Client.getInstance().pt_user_out((short) 1, serverNum, Integer.parseInt(playerSocket));

		return "return";
	}
	
	@RequestMapping(value = "/destroy/{roomNum}", method = RequestMethod.GET)
	public String destroyRoom(ModelAndView mav, @PathVariable String roomNum, HttpServletRequest request) {
		Client.getInstance().msag_room_destroy((short) 1, Short.parseShort(roomNum));

		return "return";
	}

	@RequestMapping(value = "/kill/{agentNum}/{serverNum}", method = RequestMethod.GET)
	public String connectInterServer(ModelAndView mav,@PathVariable short agentNum,@PathVariable int serverNum, HttpServletRequest request) {
		Client.getInstance().msag_kill_server(agentNum,serverNum);

		return "return";
	}
	
	@RequestMapping(value = "/generate/{agentNum}", method = RequestMethod.GET)
	public String connectInterServer(ModelAndView mav,@PathVariable short agentNum,HttpServletRequest request) {
		Client.getInstance().generate(agentNum);

		return "return";
	}
}
