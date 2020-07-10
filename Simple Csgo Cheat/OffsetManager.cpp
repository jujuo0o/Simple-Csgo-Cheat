#include "OffsetManager.h"
#include "Memory.h"
#include "PLAYER.h"
#include <iostream>
using namespace std;


unsigned int Offsets::Modules::engineDll;
unsigned int Offsets::Modules::panoramaDll;
unsigned int Offsets::ENTITY_LIST;
unsigned int Offsets::GLOWOBJECTMANAGER;
unsigned int Offsets::LOCALPLAYER_OFFSET;
int Offsets::TriggerBotTime=130;
int Offsets::DynamicOffsets::playerTeam;
unsigned int Offsets::DynamicOffsets::LocalPlayer;
unsigned int Offsets::DynamicOffsets::glowObjectManager;
unsigned int Offsets::DynamicOffsets::eLISTBASE;
std::deque<unsigned int> Offsets::DynamicOffsets::eLIST;
std::deque<int> Offsets::DynamicOffsets::eTeam;
std::deque<int> Offsets::DynamicOffsets::eId;
bool Offsets::DynamicOffsets::aimTeammates=false;
float Offsets::DynamicOffsets::aimFOV=1;
float Offsets::DynamicOffsets::aimSmoothness=0.5f;
float Offsets::colorTeam[] = { 0, 128, 255, .7f };
float Offsets::colorEnemy[] = { 55, 55, 0, .7f };
DWORD Offsets::BoneIdArray[3] = {8,6,5 };
OffsetManager::OffsetManager()
{
}


OffsetManager::~OffsetManager()
{
}
