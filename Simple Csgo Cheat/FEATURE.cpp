
#include <iostream>
#include <vector>
#include "FEATURE.h"
//#include "Overlay.h"
class BoneMatrix
{
public:
	static Vector3 GetBonePos(int boneBase, int boneID)
	{
		Vector3 bonePos;
		bonePos.x = *(float*)((boneBase + 0x30 * boneID + 0x0C));
		bonePos.y = *(float*)((boneBase + 0x30 * boneID + 0x1C));
		bonePos.z = *(float*)((boneBase + 0x30 * boneID + 0x2C));
		return bonePos;
	}
};

bool glowFlags[] = { true,false };
Vector3 Subtract(Vector3 src, Vector3 dst) {
	Vector3 difference = { src.x - dst.x,src.y - dst.y,src.z - dst.z };
	return difference;
}

float DotProduct(Vector3 src, Vector3 dst) {
	return src.x * dst.x + src.y * dst.y + src.z * dst.z;
}

float Magnitude(Vector3 src) { // or hypotenous from the x and y cordinates
	return sqrtf(src.x * src.x + src.y * src.y + src.z * src.z);
}

float Distance(Vector3 src, Vector3 dst) {
	return Magnitude(Subtract(src, dst));
}

void VectorAngles(Vector3 input, Vector3 *Output) {
	float	tmp, yaw, pitch;

	if (input.y == 0 && input.x == 0)
	{
		yaw = 0;
		if (input.z > 0)
			pitch = 270;
		else
			pitch = 90;
	}
	else
	{
		yaw = (atan2(input.y, input.x) * 180 / 3.14f);
		if (yaw < 0)
			yaw += 360;

		tmp = sqrt(input.x * input.x + input.y * input.y);
		pitch = (atan2(-input.z, tmp) * 180 / 3.14f);
		if (pitch < 0)
			pitch += 360;
	}
	Output->x = pitch;
	Output->y = yaw;
	Output->z = 0;
}

Vector3 CalcAngles(Vector3 src, Vector3 dst) {
	Vector3 Delta[3] = { (src.x - dst.x), (src.y - dst.y), (src.z - dst.z) };
	Vector3 angles = { 0,0,0 };
	angles.x = atan(Delta->z / sqrt(Delta->x * Delta->x + Delta->y * Delta->y)) * RADPI;
	angles.y = atan(Delta->y / Delta->x) * RADPI;
	angles.z = 0.0f;
	if (Delta->x >= 0.0) angles.y += 180.0f;

	return angles;
}

// Find Angles Using CalcAngle //
Vector3 CalcAnglesPunch(Vector3 src, Vector3 dst, Vector2 AimPunch) {
	Vector3 Delta = { (dst.x - src.x), (dst.y - src.y), (dst.z - src.z) };
	Vector3 angles={0,0,0};
	VectorAngles(Delta, &angles);
	if (angles.x > 180)angles.x -= 360;
	if (angles.x < -180) angles.x += 360;
	if (angles.y > 180) angles.y -= 360;
	if (angles.y < -180) angles.y += 360;
	return { angles.x - AimPunch.x * 2.f, angles.y - AimPunch.y * 2.f, 0.f };
}

// Screen To World
float ScrToWorld(float x, float y, float z, float Ex, float Ey, float Ez) {
	return (sqrt((Ex - x) * (Ex - x) + (Ey - y) * (Ey - y) + (Ez - z) * (Ez - z)));
}


bool FEATURE::WorldToScreen(const Vector3 In, Vector3* vOut,int width, int height) {
	Matrix4x4 vMatrix;
	//UINT clientState = Memory::Read<UINT>((void*)(Offsets::Modules::engineDll + Offsets::Player::dwClientState));
	vMatrix = Memory::Read<Matrix4x4>((void*)(Offsets::Modules::panoramaDll + Offsets::Player::dwViewMatrix));

	vOut->x = vMatrix.a * In.x + vMatrix.b * In.y + vMatrix.c * In.z + vMatrix.d;
	vOut->y = vMatrix.e * In.x + vMatrix.f * In.y + vMatrix.g * In.z + vMatrix.h;

	float w = vMatrix.i * In.x + vMatrix.j * In.y + vMatrix.k * In.z + vMatrix.l;

	if (w < 0.01) {
		return false;
	}

	float invw = 1.0f / w;

	vOut->x *= invw;
	vOut->y *= invw;


	float x = width / 2;
	float y = height / 2;

	x += 0.5 * vOut->x * width + 0.5;
	y -= 0.5 * vOut->y * height + 0.5;

	vOut->x = x;
	vOut->y = y;

}
Vector3 GetPlayerPos(UINT entityBase) {
	Vector3 pos = Memory::Read<Vector3>((void*)(entityBase + Offsets::Player::vecOrigin));
	Vector3 view = Memory::Read<Vector3>((void*)(entityBase + Offsets::Player::vecViewOffsets));

	pos.x += view.x;
	pos.y += view.y;
	pos.z += view.z;

	return pos;
}

Vector3 FEATURE::GetPlayerBonePos(UINT entityBase, int boneid) {
	UINT bonematrix = Memory::Read<UINT>((void*)(entityBase + Offsets::Player::m_dwBoneMatrix));
	BoneBase bMatrix = Memory::Read<BoneBase>((void*)(bonematrix + (0x30*boneid)));
	Vector3 pos = { bMatrix.x, bMatrix.y, bMatrix.z };
	

	return pos;
}



// Get FOV
float GetFov(Vector3 srcAngles, Vector3 localAngles) {
	Vector3 srcA = { srcAngles.x, srcAngles.y,srcAngles.z };
	srcA.x -= localAngles.x;
	srcA.y -= localAngles.y;
	
	// Normalize 
	if (srcA.x > 180)srcA.x -= 360;
	if (srcA.x < -180)srcA.x += 360;
	if (srcA.y > 180)srcA.y -= 360;
	if (srcA.y < -180)srcA.y += 360;

	Vector3 back = { srcA.x, srcA.y, 0.0f};

	// normalize BACK
	if (back.x > 180)back.x -= 360;
	if (back.y > 180)back.y -= 360;
	if (back.x < -180)back.x += 360;
	if (back.y < -180)back.y += 360;
	if (back.x > 89.0f)back.x = 89.0f;
	else if (back.x < -89.0f) back.x = -89.0f;
	if (back.y > 180.0f)back.y = 180.0f;
	else if (back.y < -180.0f)back.y = -180.0f;

	return back.y;


}
Vector3 GetClosestBone(UINT entityBase) {
	float fov = 100000;
	
	for (int i = 0; i < 5; i++)
	{
		Vector3 bonePos = FEATURE::GetPlayerBonePos(entityBase, Offsets::BoneIdArray[i]);
		Vector3 playerPos = GetPlayerPos(Offsets::DynamicOffsets::LocalPlayer);
		if (abs(GetFov(playerPos, bonePos)) < fov) {
			fov = abs(GetFov(playerPos, bonePos));
		}

	}
	for (int i = 0; i < 5; i++)
	{
		Vector3 bonePos = FEATURE::GetPlayerBonePos(entityBase, Offsets::BoneIdArray[i]);
		Vector3 playerPos = GetPlayerPos(Offsets::DynamicOffsets::LocalPlayer);
		if (abs(GetFov(playerPos, bonePos)) == fov) {
			return bonePos;
		}
	}

	return  FEATURE::GetPlayerBonePos(entityBase, 6);
}
bool isEntitySpotted(UINT entity) {
	int mask = Memory::Read<int> ((void*)(entity + Offsets::Player::m_bSpottedByMask));
	int base = Memory::Read<int>((void*)(Offsets::DynamicOffsets::LocalPlayer + 0x64)) - 1;
	return (mask & (1 << base)) > 0;
}

DWORD GetPlayerWeapon(UINT entity) {
	int WeaponId = Memory::Read<DWORD>((void*)(entity+Offsets::Player::m_hActiveWeapon));
	WeaponId &= 0xFFF;
	UINT WeaponEnt = Memory::Read<UINT>((void*)(Offsets::Modules::panoramaDll + Offsets::ENTITY_LIST+(WeaponId-1)*16));
	DWORD WeaponIndex = Memory::Read<DWORD>((void *)(WeaponEnt + Offsets::Player::m_iItemDefinitionIndex));

	if (WeaponIndex == W_DECOY || WeaponIndex == W_FLASH || WeaponIndex == W_SMOKE || WeaponIndex == W_MOLTOV || WeaponIndex == W_GRENADE || WeaponIndex == W_INCENDARY) {
		return IS_GRENADE;
	}
	else if (WeaponIndex == W_M4A4 || WeaponIndex == W_AK47 || WeaponIndex == W_FAMAS || WeaponIndex == W_GALIL) {
		return IS_AR;
	}
	else if (WeaponIndex == W_USP || WeaponIndex == W_P250 || WeaponIndex == W_P2000 || WeaponIndex == W_GLOCK || WeaponIndex == W_FIVESEVEN || WeaponIndex == W_DEAGLE || WeaponIndex == W_DUALS || WeaponIndex == W_TEC9 || WeaponIndex == W_CZ75) {
		return IS_PISTOL;
	}
	else if (WeaponIndex == W_AWP || WeaponIndex == W_SSG08) {
		return IS_SNIPER;
	}
	else if (WeaponIndex == W_XM || WeaponIndex == W_NOVA || WeaponIndex == W_MAG7 || WeaponIndex == W_SAWEDOFF) {
		return IS_SHOTGUN;
	}
	else if (WeaponIndex > 100) {
		return IS_UNKNOWN;
	}
	else {
		return IS_SMG;
	}

}

// Get the best target to shoot kill
UINT GetBestTarget(Vector3 viewAngles) {
	UINT target = NULL;
	float shortDist;
	float low = 1000000;
	//get player positions//
	
	for (int i = 0; i < Offsets::DynamicOffsets::eLIST.size(); i++)
	{
		DWORD playerTeam = Memory::Read<DWORD>((void*)(Offsets::DynamicOffsets::eLIST[i] + Offsets::Player::team));

		DWORD health = Memory::Read<DWORD>((void*)(Offsets::DynamicOffsets::eLIST[i] + Offsets::Player::health));
		BOOL ignore = true;
		if (Offsets::DynamicOffsets::aimTeammates) {
			ignore = false;
		}
		else {
			if (playerTeam != Offsets::DynamicOffsets::playerTeam) {
				ignore = false;
			}
		}
		if (!ignore && health>0) {
			Vector3 PlayerPos = GetPlayerPos(Offsets::DynamicOffsets::LocalPlayer);
			bool isSpotted = isEntitySpotted(Offsets::DynamicOffsets::eLIST[i]);
			bool eTmTAttack = false;
			if (!ignore)eTmTAttack = true;
			else if (isSpotted)eTmTAttack = true;
			//Vector3 EntityPos = GetPlayerBonePos(Offsets::DynamicOffsets::eLIST[i]);
			Vector3 EntityPos = GetClosestBone(Offsets::DynamicOffsets::eLIST[i]);
			shortDist = ScrToWorld(PlayerPos.x, PlayerPos.y, PlayerPos.z, EntityPos.x, EntityPos.y, EntityPos.z);
			Vector3 calAngles = CalcAngles(PlayerPos, EntityPos);
			float entityFov = abs(GetFov(calAngles, viewAngles));
			
			if (shortDist < low && (isSpotted || Offsets::DynamicOffsets::aimTeammates) && entityFov < Offsets::DynamicOffsets::aimFOV) {
				low = shortDist;
				target = Offsets::DynamicOffsets::eLIST[i];
			}
		}
		
	}
	return target;
}

void SmoothAngles(float x, float y, Vector3 *src, Vector3 *back, Vector3 lAngles, float smoothness) {
	Vector2 smoothd;
	src->x -= lAngles.x;
	src->y -= lAngles.y;

	// Normalize 
	if (src->x > 180)src->x -= 360;
	if (src->x < -180)src->x += 360;
	if (src->y > 180)src->y -= 360;
	if (src->y < -180)src->y += 360;

	smoothd.x = (src->x - x) * smoothness;
	smoothd.y = (src->y - y) * smoothness;

	back->x = lAngles.x + smoothd.x;
	back->y = lAngles.y + smoothd.y;
	back->z = lAngles.z;

	// normalize BACK
	if (back->x > 180)back->x -= 360;
	if (back->x < -180)back->x += 360;
	if (back->y > 180)back->y -= 360;
	if (back->y < -180)back->y += 360;
	if (back->x > 89.0f)back->x = 89.0f;
	else if (back->x < -89.0f) back->x = -89.0f;
	if (back->y > 180.0f)back->y = 180.0f;
	else if (back->y < -180.0f)back->y = -180.0f;

	back->z = 0.0f;

}

FEATURE::FEATURE()
{
}


FEATURE::~FEATURE()
{
}

void FEATURE::glowPlayers()
{
	//std::cout << "COUnt: " << Memory::Read<DWORD>((void*)(Offsets::DynamicOffsets::glowObjectManager + 0x4));
	//PLAYER::getPlayers();
	for (unsigned int i = 0; i < Offsets::DynamicOffsets::eLIST.size(); i++) {

		int index = Memory::Read<int>((void*)(Offsets::DynamicOffsets::eLIST[i] + Offsets::Player::glow));
		// Write Memory To Glow 
		Memory::Write((void *)(Offsets::DynamicOffsets::glowObjectManager + index * 0x38 + Offsets::GlowObject::colour), sizeof(float)*4,(Offsets::DynamicOffsets::eTeam[i]== Offsets::DynamicOffsets::playerTeam)? Offsets::colorTeam : Offsets::colorEnemy);
		// Set flag to true to glow player 
		Memory::Write((void *)(Offsets::DynamicOffsets::glowObjectManager + index * 0x38 + Offsets::GlowObject::trueFlg), sizeof(bool) * 2, glowFlags);
	}
}



void MouseMove(double x, double y)
{
	double fx = x * (65535.0 / 1920.0);
	double fy = y * (65535.0 / 1080.0);
	INPUT input = { 0 };
	input.type = INPUT_MOUSE;
	input.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE;
	input.mi.dx = (int)(fx + 0.5);
	input.mi.dy = (int)(fy + 0.5);
	SendInput(1, &input, sizeof(INPUT));
}

void AimDeltaAngle(double dPitch, double dYaw, double sens)
{
	dYaw = -dYaw;
	double xMove1, xMove2, yMove1, yMove2;
	if (dPitch < 0)
	{
		yMove1 = ((dPitch / 2.0) / 0.132) * (0.1 / sens);
	}
	else
	{
		yMove1 = ((dPitch / 2.0) / 0.133) * (0.1 / sens);
	}

	yMove2 = ((dPitch / 2.0) / 0.134) * (0.1 / sens);

	xMove1 = ((dYaw / 2.0) / 0.076) * (0.1 / sens);
	xMove2 = ((dYaw / 2.0) / 0.074) * (0.1 / sens);

	MouseMove(xMove1, yMove1);
	MouseMove(xMove2, yMove2);
}


Vector3 FindSmoothAngels(Vector3 Target, Vector3 Viewangles)
{
	float angel;
	float angelhelper;
	Vector3 SmoothAimAngles;

	Vector2 SmoothMy;
	SmoothMy.x = Viewangles.x + 180;
	SmoothMy.y = Viewangles.y + 180;

	Vector2 SmoothEnm;
	SmoothEnm.x = Target.x + 180;
	SmoothEnm.y = Target.y + 180;

	angel = SmoothEnm.x - SmoothMy.x;
	angelhelper = angel / Offsets::DynamicOffsets::aimSmoothness;
	SmoothAimAngles.x = Viewangles.x + angelhelper;

	angel = SmoothEnm.y - SmoothMy.y;
	angelhelper = angel / Offsets::DynamicOffsets::aimSmoothness;
	SmoothAimAngles.y = Viewangles.y + angelhelper;

	return SmoothAimAngles;
}

void FEATURE::Aimbot() {
	// Aimbot //

	// Get Client State first 
	UINT clientState = Memory::Read<UINT>((void *)(Offsets::Modules::engineDll + Offsets::Player::dwClientState));
	

	if (GetAsyncKeyState(VK_LBUTTON) & 0x8000) {
		UINT target = NULL;
		Vector3 viewAngles = Memory::Read<Vector3>((void*)(clientState + Offsets::Player::dwClientState_ViewAngles));

		//std::cout << " Target: " << target << std::endl;
		DWORD Weapon = GetPlayerWeapon(Offsets::DynamicOffsets::LocalPlayer);
		if (Weapon!=IS_GRENADE) {

			Vector2 AimPunch = { 0,0 }; 
			
			if (Weapon == IS_SMG || Weapon == IS_AR) {
				AimPunch = Memory::Read<Vector2>((void*)(Offsets::DynamicOffsets::LocalPlayer + Offsets::Player::m_aimPunchAngle));
			}
			//AimPunch.x = AimPunch.x * 2.0f;
			//AimPunch.y = AimPunch.y * 2.0f;
			target = GetBestTarget(viewAngles);
			if (target != NULL) {
				Vector3 PlayerPosI = GetClosestBone(target);
				Vector3 PlayerPos0 = GetPlayerPos(Offsets::DynamicOffsets::LocalPlayer);
				Vector3 entityAngles = CalcAnglesPunch(PlayerPos0, PlayerPosI,AimPunch);
				// smooth angles //
				Vector3 smoothAngles = { 0.f,0.f,0.f };
				//smoothAngles = FindSmoothAngels(entityAngles,viewAngles);
				SmoothAngles(AimPunch.x, AimPunch.y, &entityAngles, &smoothAngles, viewAngles, Offsets::DynamicOffsets::aimSmoothness);
				float fo = std::hypotf(entityAngles.x, entityAngles.y);
				if (fo < Offsets::DynamicOffsets::aimFOV) {
					Memory::Write<float>((void*)(clientState + Offsets::Player::dwClientState_ViewAngles), smoothAngles.x);
					Memory::Write<float>((void*)(clientState + Offsets::Player::dwClientState_ViewAngles + 0x4), smoothAngles.y);
				}
			}

		}
		Sleep(10);
	}
}

void FEATURE::BunnyHop() {
	UINT clientState = Memory::Read<UINT>((void*)(Offsets::Modules::engineDll + Offsets::Player::dwClientState));
	{
		DWORD health = Memory::Read<DWORD>((void*)(Offsets::DynamicOffsets::LocalPlayer + Offsets::Player::health));
		if (health > 0) {
			int flags = Memory::Read<int>((void*)(Offsets::DynamicOffsets::LocalPlayer + Offsets::Player::flags));
			if (flags & (1 << 0)) {
				int jump = 0x5;
				Memory::Write<int>((void*)(Offsets::Modules::panoramaDll + Offsets::Player::dwForceJump), jump);
				Sleep(20);
				jump = 0x4;
				Memory::Write<int>((void*)(Offsets::Modules::panoramaDll + Offsets::Player::dwForceJump), jump);
			}
			Sleep(2);
		}
	}

}

void FEATURE::Radar() {
	for (int i = 0; i < Offsets::DynamicOffsets::eLIST.size(); i++)
	{
		if (Offsets::DynamicOffsets::eTeam[i] != Offsets::DynamicOffsets::playerTeam) {
			int isSpotted = Memory::Read<int>((void*)(Offsets::DynamicOffsets::eLIST[i] + Offsets::Player::m_bSpotted));
			if (isSpotted!=1) {
				Memory::Write<int>((void*)(Offsets::DynamicOffsets::eLIST[i] + Offsets::Player::m_bSpotted), 1);
			}
		}
	}
}

void FEATURE::RageBot() {
	
}

void FEATURE::TriggerBot() {
	// get crosshair id //
	int CrossHairId=0;
	CrossHairId = Memory::Read<int>((void*)(Offsets::DynamicOffsets::LocalPlayer + Offsets::Player::crosshairId));

	bool fire = false;
	if (CrossHairId > 0 && CrossHairId <65 ) {
		//std::cout << "Enemy: " << CrossHairId << std::endl;
		//std::cout << "Size : " << Offsets::DynamicOffsets::eId.size() << " id: " << CrossHairId << std::endl;
		
		DWORD player = Memory::Read<DWORD>((void*)(Offsets::DynamicOffsets::eLISTBASE + ((CrossHairId-1) * 0x10)));
		DWORD playerTeam = Memory::Read<DWORD>((void*)(player+Offsets::Player::team));
		// check if crosshair exists in Enemy Id's List //
		//std::cout << "Player Team: " << playerTeam << " My team: " << Offsets::DynamicOffsets::playerTeam << std::endl;
		if (playerTeam != Offsets::DynamicOffsets::playerTeam){
			//std::cout << "Enemy: " << CrossHairId - 1 << std::endl;
			fire = true;
		}
		//std::cout << " Here: " << std::endl;
		if (fire) {
			mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
			//Memory::Write<int>((void *)(Offsets::Modules::panoramaDll + Offsets::Player::attack1), 5);
			Sleep(Offsets::TriggerBotTime);
			//Memory::Write<int>((void *)(Offsets::Modules::panoramaDll + Offsets::Player::attack1), 4);
			mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
			
		}

	}
}

void FEATURE::NoFlash() {
	Memory::Write<float>((void*)(Offsets::DynamicOffsets::LocalPlayer + 0xA3F0),0.0f);

}
void FEATURE::NoRecoil() {
	Vector2 punch, view, oldPunch, newAngles;
	oldPunch.x = oldPunch.y = 0;
	view.x = view.y = 0;
	DWORD shotsFired = Memory::Read<DWORD>((void*)(Offsets::DynamicOffsets::LocalPlayer + Offsets::Player::m_iShotsFired));
	if (GetAsyncKeyState(VK_LBUTTON)) {
		std::cout << "Fire IN the hole " << std::endl;
		
		punch = Memory::Read<Vector2>((void*)(Offsets::DynamicOffsets::LocalPlayer + Offsets::Player::m_aimPunchAngle));
		std::cout << "punch: x=" << punch.x << "punch: y="  << punch.y << std::endl;
		unsigned int clientState = Memory::Read<unsigned int>((void*)(Offsets::Modules::engineDll + Offsets::Player::dwClientState));
		view = Memory::Read<Vector2>((void*)(clientState + Offsets::Player::dwClientState_ViewAngles));
		std::cout << "view: x=" << view.x << "punch: y=" << view.y << std::endl;
		newAngles.x = (view.x + oldPunch.x) - (punch.x * 2.0f);
		newAngles.y = (view.y + oldPunch.y) - (punch.y * 2.0f);
		std::cout << "x: " << newAngles.x << " y: " << newAngles.y << std::endl;
		// normalize the angles
		while (newAngles.x > 180) {
			newAngles.x -= 360;
		}
		while (newAngles.y < -180) {
			newAngles.y += 360;
		}

		if (newAngles.x > 89.0f) {
			newAngles.x = 89.0f;
		}
		if (newAngles.y < -89.0f) {
			newAngles.y = -89.0f;
		}


		oldPunch.x = punch.x * 2.0f;
		oldPunch.y = punch.y * 2.0f;
		std::cout << "x: " << newAngles.x << " y: " << newAngles.y << std::endl;
		Memory::Write<Vector2>((void*)(clientState + Offsets::Player::dwClientState_ViewAngles), { 0,0 });

		//Sleep(10);
		
	}
	else {
		oldPunch.x = oldPunch.y = 0;
	}
}
