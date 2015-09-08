#include "RoomManager.h"
#include "Client_Protocol.h"

extern RoomManager roomManager;

extern SOCKET the_other_sock;

Room::Room(int roomNumber){
	this->roomNumber = roomNumber;
}
Room::~Room(){
	std::list<Player*>::iterator iter;
	for (iter = players.begin(); iter != players.end(); iter++){
		playerQuit(*iter, false);
	}
}

void Room::playerEnter(Player* player){
	player->roomNum = roomNumber;

	t_join_alarm		tmpJoinAlarm;
	memcpy(tmpJoinAlarm.nickname, player->nickname.c_str(), 20);
	tmpJoinAlarm.room_num = roomNumber;
	tmpJoinAlarm.type = pkt_type::pt_join_alarm;

	this->broadcast_msg((char*)&tmpJoinAlarm, sizeof(t_join_alarm));
	std::cout << "join alarm message has been sent." << std::endl;

	/* informing the entered */
	players.push_back(player);
}


void Room::playerQuit(Player* player, bool msg){
	if (player->roomNum != this->roomNumber) return;
	t_leave_alarm		tmpLeaveAlarm;
	memcpy(tmpLeaveAlarm.nickname, player->nickname.c_str(), sizeof(tmpLeaveAlarm.nickname));
	tmpLeaveAlarm.room_num = this->roomNumber;
	tmpLeaveAlarm.type = pkt_type::pt_leave_alarm;


	player->roomNum = -1;
	player->nickname = "";
	players.remove(player);

	/* informing the exited */
	if (msg) this->broadcast_msg((char*)&tmpLeaveAlarm, sizeof(t_leave_alarm));
	std::cout << "leave alarm message has been sent." << std::endl;
}

void Room::broadcast_msg(char* msg, int size){
	std::list<Player*>::iterator iter;
	//printf("Room %d : %d persons are connecting...\n", roomNumber, players.size());
	for (iter = players.begin(); iter != players.end(); iter++){
		if ((*iter)->isMine) (*iter)->send_msg(msg, size);
	}
}