#include <Windows.h>
#include <TlHelp32.h>


#include <dwmapi.h>
#include "FEATURE.h"
#include "Memory.h"
#include "About.h"
#include "OffsetManager.h"
#include "PLAYER.h"
#include <iostream>
#include <thread>
#include <commctrl.h>
#include <wchar.h>


#pragma comment(lib,"dwmapi.lib")
///////////////////////////////////////////////////////////////////
//////////////////////////// HANDLES /////////////////////////////
#define ID_TABCTRL 1
// ESP
#define ID_WALLHACK 1
#define ID_NOFLASH 2
#define ID_NOSMOKE 3
// AIM
#define ID_TRIGGERBOT 4
#define ID_AIMBOT 5
// SERVICE
#define ID_START 99
// BUFFER SIZE
#define BUF_SIZE 0x1000
// CONFIGURATION FILE NAME
#define CONFIG_INI "Config.ini"
// CONFIG PROPERTIES 
#define CONFIG_PROPS 3
///////////////////////////////////////////////////////////////////
/////////////////// SWITCHES AND VARIABLES ////////////////////////
bool threadSwitch;
bool startup = false;
bool wallhack = true;
bool noflash = false;
bool nosmoke = false;
bool triggerbot = false;
bool aimbot = false;
bool bunnyhop = false;
bool started = false;
bool radar = false;
bool offsetsFound=false;
bool aimRandom = true;
Offsets::Signatures SIGNATURES;
DWORD pid = NULL; //PROCESS ID //
///////////////////////////////////////////////////////////////////
//////////////////////////// HANDLES /////////////////////////////
HWND hTab;
HWND hWall;
HWND hFlash;
HWND hTriggerBotTime;
HWND hStart;
MSG msg;
HWND hwnd;
WNDCLASS wc;
HINSTANCE hOptions;
HINSTANCE hOpt;
HANDLE tArray = INVALID_HANDLE_VALUE;
HANDLE hTriggerBot = INVALID_HANDLE_VALUE;
HANDLE tGetPlayers = INVALID_HANDLE_VALUE;
HANDLE tAimbot = INVALID_HANDLE_VALUE;
HANDLE tBunnyHop = INVALID_HANDLE_VALUE;
///////////////////////////////////////////////////////////////////
//////////////// METHOD PROTOTYPES ///////////////////////////////
int Initialize();
void start();
bool autoConfig();
void setConfig(char*, char*);
bool strCmp(char*, const char*);
void writeConfigFile();
DWORD WINAPI hkThread(LPVOID lpParam);
DWORD WINAPI TriggerBotThread(LPVOID lparams);
DWORD WINAPI GetOffsets(LPVOID lparams);
DWORD WINAPI GetPlayer(LPVOID lpParam);
DWORD WINAPI AimBotThread(LPVOID lparams);
DWORD WINAPI BunnyHopThread(LPVOID lparams);
DWORD getPIDFromProcName(const char* pName);
int main(int argc, char*argv[]) {
	wallhack = true;
	triggerbot = true;
	bool started = false;
	Initialize();
	ABOUT::Banner();
	if (autoConfig()) {
		cout << "Config Loaded Successfully!" << endl;
	}
	else {
		cout << "Config.ini not found. Creating new Configuration File" << endl;
		cout << "Aim at teammates?(1/0)" << endl;
		DWORD aimT = 0;
		char am[10];
		cin >> am;
		aimT = strtod(am, NULL);
		if (aimT == 1) {
			Offsets::DynamicOffsets::aimTeammates = TRUE;
		}
		float FOV = 1.0f;
		cout << "Aim Fov(Current:" << Offsets::DynamicOffsets::aimFOV << ")";
		char fo[10];
		cin >> fo;
		FOV = strtof(fo, NULL);
		if (FOV >= 1.0f && FOV <= 90.0f) {
			Offsets::DynamicOffsets::aimFOV = FOV;
		}

		cout << "Aim Smoothness(Current:" << Offsets::DynamicOffsets::aimSmoothness << ")";
		float smoothie = 0;
		char sm[10];
		cin >> sm;
		smoothie = strtof(sm, NULL);
		if (smoothie >= 0.05f && smoothie <= 1.0f) {
			Offsets::DynamicOffsets::aimSmoothness = smoothie;
		}

		HANDLE hConfig = CreateFile(CONFIG_INI, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hConfig != INVALID_HANDLE_VALUE) {
			const char* keys[CONFIG_PROPS] = { "aim_fov ", "aim_smoothness ", "aim_at_teammates " };
			const char* vals[CONFIG_PROPS] = { fo,sm,am };
			char line[20];
			for (int l = 0; l < 3; l++)
			{
				char ret[20];
				LPSTR jj;
				int len1 = strlen(keys[l]);
				int len2 = strlen(vals[l]);
				int i = 0;
				for (i = 0; i < len1; i++)
				{

					ret[i] = keys[l][i];

				}
				int j = i;
				int k = 0;
				for (; k < len2; k++)
				{
					ret[j + k] = vals[l][k];
				}
				j += k;

				ret[j] = '\r';
				ret[j + 1] = '\n';
				int len = j + 1;
				DWORD ww;
				WriteFile(hConfig, ret, len + 1, &ww, NULL);
			}

			CloseHandle(hConfig);
		}
		else {
			cout << "Error Writing Config File!" << endl;
		}


	}
	
	cout << "///////////////////////////////////////////////////////" << endl;
	cout << "///                 AIMBOT CONFIG                   ///" << endl;
	cout << "/// AIM FOV        =                            " << Offsets::DynamicOffsets::aimFOV << "   ///" << endl;
	cout << "/// AIM SMOOTHNESS =                            " << Offsets::DynamicOffsets::aimSmoothness << " ///" << endl;
	cout << "/// AIM AT TEAM    =                            " << Offsets::DynamicOffsets::aimTeammates << "   ///" << endl;
	cout << "///////////////////////////////////////////////////////" << endl;
	

	while (true) {
		
		if (GetAsyncKeyState(VK_HOME) && offsetsFound) {
			start();
		}
		if (GetAsyncKeyState(VK_DELETE)) {
			exit(0);
		}
		Sleep(100);
	}
	

	return 0;
	
	
}

////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////// THREAD FUNCTION'S /////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

void writeConfigFile() {
	

}

void start() {
	if (started) {
		started = false;

		SuspendThread(tArray);
		SuspendThread(hTriggerBot);
		SuspendThread(tAimbot);

		WaitForSingleObject(tArray, WAIT_TIMEOUT);
		WaitForSingleObject(hTriggerBot, WAIT_TIMEOUT);
		WaitForSingleObject(tAimbot, WAIT_TIMEOUT);
		CloseHandle(tArray);
		CloseHandle(tAimbot);
		CloseHandle(hTriggerBot);
	}
	else {
		started = true;
		tArray = CreateThread(NULL, 0, hkThread, NULL, 0, NULL);

	}
}

int Initialize() {

	HWND wnd = FindWindowA(0, "Counter-Strike: Global Offensive");
	GetWindowThreadProcessId(wnd, &pid);
	if (pid == 0 || pid ==NULL) {
		MessageBox(hwnd, "Process Not Found", "Error", NULL);
		started = false;
		return -1;
	}
	Memory::pHandle = OpenProcess(PROCESS_ALL_ACCESS, 0, pid);
	if (Memory::pHandle == INVALID_HANDLE_VALUE) {
		MessageBox(hwnd, "Unable To Open Process", "Error", NULL);
		started = false;
		return -1;
	}
	HANDLE offsets = CreateThread(NULL, 0, GetOffsets, NULL, 0, NULL);
	return 0;
}


bool autoConfig() {
	HANDLE hConfig = CreateFile("Config.ini", GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hConfig != INVALID_HANDLE_VALUE) {

		LPSTR config = (LPSTR)malloc(BUF_SIZE);
		bool read = false;
		DWORD dd;
		LPSTR word = (LPSTR)malloc(BUF_SIZE);
		while (ReadFile(hConfig, (LPVOID)word, BUF_SIZE, &dd, NULL)) {
			read = true;
			if (dd == 0) {
				break;
			}
			strcpy(config, word);
		}

		int nl = 0;
		int i = 0;
		while (i < CONFIG_PROPS) {
			char key[20];
			char val[10];
			int j = nl + 0;
			int l = 0;
			while (config[j] != '\n') {
				while (config[j + l] != ' ') {
					key[l] = config[j + l];
					l++;
				}
				key[l] = '\n';
				j += l + 1;
				int k = 0;
				while (config[j + k] != '\r') {
					val[k] = config[j + k];
					k++;
				}
				val[k] = '\n';
				j += k + 1;
			}
			setConfig(key, val);
			i++;
			nl = j + 1;
		}

	}
	else {
		return false;
	}
	return true;
}
bool strCmp(char* st, const char* cm) {
	bool equal = true;
	for (int i = 0; i < strlen(cm); i++)
	{
		if (st[i] != cm[i]) {
			equal = false;
		}
	}
	return equal;
}
void setConfig(char* key, char* val) {

	if (strCmp(key, "aim_fov")) {
		Offsets::DynamicOffsets::aimFOV = (DWORD)strtod(val, NULL); 
	}
	else if (strCmp(key, "aim_smoothness")) {
		Offsets::DynamicOffsets::aimSmoothness = (FLOAT)strtof(val, NULL);
	}
	else if (strCmp(key, "aim_at_teammates")) {
		Offsets::DynamicOffsets::aimTeammates = (strtod(val, NULL) >= 1) ? true : false;
	}
}
DWORD WINAPI GetOffsets(LPVOID lparams) {
	DWORD pid = GetProcessId(Memory::pHandle);
	if (pid != 0) {
		Offsets::Modules::panoramaDll = Memory::GetBaseAddress(pid, "client.dll");
		Offsets::Modules::engineDll = Memory::GetBaseAddress(pid, "engine.dll");
		UINT glowObjectManager = Memory::FindOffsetEx(SIGNATURES.dwGlowObjectManager[0], SIGNATURES.dwGlowObjectManager[1], SIGNATURES.dwGlowObjectManager[2], atoi(SIGNATURES.dwGlowObjectManager[3]), atoi(SIGNATURES.dwGlowObjectManager[4]));
		UINT eListBase = Memory::FindOffsetEx(SIGNATURES.dwEntityList[0], SIGNATURES.dwEntityList[1], SIGNATURES.dwEntityList[2], atoi(SIGNATURES.dwEntityList[3]), atoi(SIGNATURES.dwEntityList[4]));
		UINT localplayer = Memory::FindOffsetEx(SIGNATURES.dwLocalPlayer[0], SIGNATURES.dwLocalPlayer[1], SIGNATURES.dwLocalPlayer[2], atoi(SIGNATURES.dwLocalPlayer[3]), atoi(SIGNATURES.dwLocalPlayer[4]));


		Offsets::ENTITY_LIST = eListBase - Offsets::Modules::panoramaDll;
		Offsets::GLOWOBJECTMANAGER = glowObjectManager - Offsets::Modules::panoramaDll;
		Offsets::LOCALPLAYER_OFFSET = localplayer - Offsets::Modules::panoramaDll;
		
		Offsets::DynamicOffsets::glowObjectManager = Memory::Read<unsigned int>((void*)(Offsets::Modules::panoramaDll + Offsets::GLOWOBJECTMANAGER));
		Offsets::DynamicOffsets::eLISTBASE = Offsets::Modules::panoramaDll + Offsets::ENTITY_LIST;
		Offsets::DynamicOffsets::LocalPlayer = Memory::Read<unsigned int>((void*)(Offsets::Modules::panoramaDll + Offsets::LOCALPLAYER_OFFSET));
		PLAYER::getPlayers();
		offsetsFound = true;
		return 0;
	}
	return 1;
}


DWORD WINAPI hkThread(LPVOID lpParam) {
	while (1) {
		DWORD RandomBones[5] = { 8,6,5 };
		if (GetAsyncKeyState(VK_F8) & 0x8000) {
			if (aimRandom) {
				aimRandom = false;
				cout << "HeadShot Mode" << endl;
				for (int i = 0; i < 3; i++)
				{
					Offsets::BoneIdArray[i] = 8;
				}
			}
			else {
				aimRandom = true;
				cout << "Random Aim Mode " << endl;
				Offsets::BoneIdArray[0] = 8;
				Offsets::BoneIdArray[1] = 6;
				Offsets::BoneIdArray[2] = 5;
			}
			Sleep(100);
		}
		
		PLAYER::getPlayers();
		if (wallhack) {
			FEATURE::glowPlayers();

			
		}
		if (GetAsyncKeyState(VK_MENU) && triggerbot) {
			FEATURE::TriggerBot();
		}
		// START/STOP Aimbot
		if (GetAsyncKeyState(VK_F2) & 0x8000) {
			if (aimbot) {
				cout << "Aimbot Disabled" << endl;
				aimbot = false;
			}
			else {
				cout << "Aimbot Enabled" << endl;
				aimbot = true;
			}
			Sleep(100);
		}
		// START/STOP Wallhack
		if (GetAsyncKeyState(VK_F3) & 0x8000) {
			if (wallhack) {
				cout << "Wallhack Disabled" << endl;
				wallhack = false;
			}
			else {
				cout << "Wallhack Enabled" << endl;
				wallhack = true;
			}
			Sleep(100);
		}
		// START/STOP Triggerbot
		if (GetAsyncKeyState(VK_F4) & 0x8000) {
			if (triggerbot) {
				cout << "Triggerbot Disabled" << endl;
				triggerbot = false;
			}
			else {
				cout << "Triggerbot Enabled" << endl;
				triggerbot = true;
			}
			Sleep(100);
		}
		// START/STOP NoFlash
		if (GetAsyncKeyState(VK_F5) & 0x8000) {
			if (noflash) {
				cout << "NoFlash Disabled" << endl;
				noflash = false;
			}
			else {
				cout << "NoFlash Enabled" << endl;
				noflash = true;
			}
			Sleep(100);
		}
		// START/STOP Bunnyhop
		if (GetAsyncKeyState(VK_F6) & 0x8000) {
			if (bunnyhop) {
				cout << "Bunnyhop Disabled" << endl;
				bunnyhop = false;
			}
			else {
				cout << "Bunnyhop Enabled" << endl;
				bunnyhop = true;
			}
			Sleep(100);
		}
		// START/STOP Radar
		if (GetAsyncKeyState(VK_F7) & 0x8000) {
			if (radar) {
				cout << "Radar Disabled" << endl;
				radar = false;
			}
			else {
				cout << "Radar Enabled" << endl;
				radar = true;
			}
			Sleep(100);
		}
		if (aimbot) {
			FEATURE::Aimbot();
		}
		if (noflash) {
			FEATURE::NoFlash();
		}
		if (noflash) {
			FEATURE::Radar();
		}
		if (bunnyhop) {
			if ((GetAsyncKeyState(VK_SPACE) & 0x8000)) {
				FEATURE::BunnyHop();
			}
			
		}


	}
	return 0;
}

DWORD getPIDFromProcName(const char* pName) {
	PROCESSENTRY32 p32;
	HANDLE tlSnap;
	tlSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	p32.dwSize = sizeof(PROCESSENTRY32);
	BOOL retVal = Process32First(tlSnap, &p32);
	while (retVal) {
		if (strcmp(p32.szExeFile, pName)) {
			return p32.th32ProcessID;
		}
		retVal = Process32Next(tlSnap, &p32);
	}
	return 0;
}