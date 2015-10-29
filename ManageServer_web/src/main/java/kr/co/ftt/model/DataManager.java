package kr.co.ftt.model;

import java.util.List;
import java.util.Map;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.jdbc.core.JdbcTemplate;

import kr.co.ftt.db.Constant;

public class DataManager {
	
	
	public JdbcTemplate template;

	@Autowired
	public void setTemplate(JdbcTemplate template) {
		this.template = template;
		Constant.template = this.template;
	}
	
    private static DataManager dataManager = new DataManager();

    public static DataManager getInstance(){
		if (dataManager == null) {
			dataManager = new DataManager(); 
		}
		return dataManager;
	}
    
    public List<Map<String, Object>> selectToList(String sql){
    	List<Map<String, Object>> testlist = template.queryForList(sql);
		return testlist;
    }
    
    public String selectToCount(String sql){
    	Map<String, Object> testlist = template.queryForMap(sql);    	
		return testlist.get("count(*)").toString();
    }
    
    public String selectToGenState(String sql){
    	Map<String, Object> testlist = template.queryForMap(sql);    	
		return testlist.get("gen_state").toString();
    }    
    
    public String selectToRecvState(String sql){
    	Map<String, Object> testlist = template.queryForMap(sql);    	
		return testlist.get("receive_time").toString();
    }
}
