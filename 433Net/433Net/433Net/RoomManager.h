#include "utilities.h"
#include "Player.h"

class Room{
public:
	int roomNumber;
	std::list<Player*> players;

public:
	Room(int roomNumber){
		this->roomNumber = roomNumber;
	}
	~Room(){
		std::list<Player*>::iterator iter;
		for (iter = players.begin(); iter != players.end(); iter++){
			playerQuit(*iter);
		}
	}
	
	void playerEnter(Player* player){
		player->roomNum = roomNumber;

		/* 입장했다고 알리기 */
		players.push_back(player);
	}

	void playerQuit(Player* player){
		player->roomNum = -1;
		players.remove(player);

		/* 나갔다고 알리기 */
	}
	
	void broadcast_msg(char* msg, int size){
		std::list<Player*>::iterator iter;
		for (iter = players.begin(); iter != players.end(); iter++){
			(*iter)->send_msg(msg, size);
		}
	}


};


class RoomManager{
public:
	std::list<Room*> rooms;

public:
	void createRoom(Player* p){
		int i = 0;
		std::list<Room*>::iterator iter;
		for (iter = rooms.begin(); iter != rooms.end(); iter++){
			if ((*iter)->roomNumber == i){
				i++;
				continue;
			}
			else{
				
				break;
			}
		}
		Room* room = new Room(i);
		enterRoom(p, i);
		rooms.insert(iter, room);
	}
	Room* findRoom(int roomNumber){
		std::list<Room*>::iterator iter;
		for (iter = rooms.begin(); iter != rooms.end(); iter++){
			if ((*iter)->roomNumber == roomNumber){
				return (*iter);
			}
		}
		return NULL;
	}

	void enterRoom(Player* p, int roomNumber){
		Room* room = findRoom(roomNumber);
		if (room == NULL) return;
		p->roomNum = roomNumber;
		room->playerEnter(p);
	}

	void destroyRoom(int roomNumber){
		Room* room = findRoom(roomNumber);
		rooms.remove(room);
		
		delete room;
	}
	
};