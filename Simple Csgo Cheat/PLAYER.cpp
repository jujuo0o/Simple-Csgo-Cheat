#include "PLAYER.h"
#include <iostream>



PLAYER::PLAYER()
{
}


PLAYER::~PLAYER()
{
}

void PLAYER::getTeamId() {
	Offsets::DynamicOffsets::playerTeam = Memory::Read<int>((void*)(Offsets::DynamicOffsets::LocalPlayer + Offsets::Player::team));
}

void PLAYER::getPlayers() {
	PLAYER::getTeamId();
	Offsets::DynamicOffsets::eLIST.clear();
	Offsets::DynamicOffsets::eTeam.clear();
	// Get All Players //
	//std::cout << " Found Players: " << players << std::endl;

	for (int i = 1; i < 64; i++) {
		//  sizeof(int) * 4 * i)
		unsigned int player = Memory::Read<DWORD>((void*)((Offsets::DynamicOffsets::eLISTBASE + i * 0x10)));
		//std::cout << player << " health: " << Memory::Read<DWORD>((void*)(player + Offsets::Player::health)) << std::endl;
		if (!player) {
			continue;
		}
			//std::cout << "Found Players: " << player << std::endl;
		
			Offsets::DynamicOffsets::eLIST.push_back(player);
			Offsets::DynamicOffsets::eTeam.push_back(Memory::Read<int>((void *)(player + Offsets::Player::team)));
	}
	//std::cout << " Found Players: " << players << std::endl;
}

