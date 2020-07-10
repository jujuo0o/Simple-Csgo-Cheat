#pragma once
#include <Windows.h>
#include <deque>
struct Offsets {
	// Trigger Bot Time 
	static int TriggerBotTime;
	static unsigned int ENTITY_LIST;
	static unsigned int LOCALPLAYER_OFFSET;
	static unsigned int GLOWOBJECTMANAGER;
	static float colorTeam[4];
	static float colorEnemy[4];
	static DWORD BoneIdArray[3];
	//static float colorEnemy[4];
	static struct colorEnemy {
		static float r;
		static float g;
		static float b;
		static float a;
	};
	struct Modules {
		static unsigned int panoramaDll;
		static unsigned int engineDll;
	};

	static struct Signatures {
		const char* dwLocalPlayer[5] = { "client.dll", "\x8D\x34\x85\x00\x00\x00\x00\x89\x15\x00\x00\x00\x00\x8B\x41\x08\x8B\x48\x04\x83\xF9\xFF", "xxx????xx????xxxxxxxxx","3","4" };
		const char* dwGlowObjectManager[5] = { "client.dll", "\xA1\x00\x00\x00\x00\xA8\x01\x75\x4B", "x????xxxx","1","4" };
		const char* dwEntityList[5] = { "client.dll", "\xBB\x00\x00\x00\x00\x83\xFF\x01\x0F\x8C\x00\x00\x00\x00\x3B\xF8", "x????xxxxx????xx","1","0" };

	};

	struct Player {
		const static unsigned int health = 0x100;
		const static unsigned int glow = 0xA438;
		const static unsigned int team = 0xF4;
		const static unsigned int dormant = 0xED;
		const static unsigned int lifeState = 0x25F;
		const static unsigned int flags = 0x104;
		const static unsigned int m_FlashMaxAlpha = 0xA41C;
		const static unsigned int vecOrigin = 0x138;
		const static unsigned int vecViewOffsets = 0x108;
		const static unsigned int crosshairId = 0xB3E4;
		const static unsigned int attack1 = 0x31807A8;
		const static unsigned int m_aimPunchAngle = 0x302C;
		const static unsigned int dwClientState_ViewAngles = 0x4D88;
		const static unsigned int dwClientState = 0x58ADD4;
		const static unsigned int m_hActiveWeapon = 0x2EF8;
		const static unsigned int m_iItemDefinitionIndex = 0x2FAA;
		const static unsigned int m_iShotsFired = 0xA390;
		const static unsigned int m_dwBoneMatrix = 0x26A8;
		const static unsigned int dwViewMatrix = 0x4D40BA4;
		const static unsigned int m_bSpotted = 0x93D;
		const static unsigned int dwForceJump = 0x51F8EF4;
		const static unsigned int m_bSpottedByMask = 0x980;
	};
	struct DynamicOffsets {
		static unsigned int LocalPlayer;
		static unsigned int glowObjectManager;
		static unsigned int eLISTBASE;
		static std::deque<unsigned int> eLIST;
		static std::deque<int>eTeam;
		static std::deque<int>eId;
		static int playerTeam;
		static bool aimTeammates;
		static float aimFOV;
		static float aimSmoothness;
	};
	struct GlowObject {
		const static unsigned int colour = 0x4;
		const static unsigned int trueFlg = 0x24;
	};
};


class OffsetManager
{
public:
	OffsetManager();
	~OffsetManager();
};

