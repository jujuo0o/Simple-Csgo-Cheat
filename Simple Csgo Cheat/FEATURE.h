#pragma once
#include "Memory.h"
#include "OffsetManager.h"
#include "PLAYER.h"
#define RADPI 57.295779513082;


#define IS_GRENADE 1
#define IS_AR 2
#define IS_PISTOL 4
#define W_CZ75 63
#define W_DECOY 47
#define W_SSG08 40
#define W_MP9 34
#define IS_SNIPER 3
#define W_MP7 33
#define IS_SHOTGUN 6
#define IS_SMG 5
#define IS_UNKNOWN 7
#define W_AWP 9
#define W_FAMAS 10
#define W_NOVA 35
#define W_XM 25
#define W_GRENADE 44
#define W_MOLTOV 46
#define W_INCENDARY 48
#define W_P90 19
#define W_MAC10 17
#define W_FLASH 43
#define W_DEAGLE 1
#define W_USP 61
#define W_P250 36
#define W_DUALS 2
#define W_SMOKE 45
#define W_GLOCK 4
#define W_TEC9 30
#define W_UMP 24
#define W_SAWEDOFF 29
#define W_MAG7 27
#define W_GALIL 137
#define W_AK47 7
#define W_M4A4 16
#define W_M4A1S  60
#define W_P2000 32
#define W_FIVESEVEN 3
#define W_NEGEV 28

struct Vector3 {
	float x;
	float y;
	float z;
};

struct Vector2 {
	float x;
	float y;
};

struct BoneBase {
	byte padding[12];
	float x;
	byte padding2[12];
	float y;
	byte padding3[12];
	float z;

};

struct Matrix4x4 {
	float a;
	float b;
	float c;
	float d;
	float e;
	float f;
	float g;
	float h;
	float i;
	float j;
	float k;
	float l;
};

class FEATURE
{
public:
	FEATURE();
	~FEATURE();
	
	static void glowPlayers();
	static void Aimbot();
	static void TriggerBot();
	static void NoFlash();
	static void NoRecoil();
	static void BunnyHop();
	static void Radar();
	static void RageBot();
	static Vector3 GetPlayerBonePos(UINT , int );
	static bool WorldToScreen(const Vector3 In, Vector3* out,int width,int height);
};


