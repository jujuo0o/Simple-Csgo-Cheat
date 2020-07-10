#include <Windows.h>
#include <TlHelp32.h>


#include <dwmapi.h>
//#include "Overlay.h"
//#include "Direct3D/Renderer.h"

///////////////// DIRECTX OVERLAY////////////////////////////////
//Renderer* gRenderer = new Renderer();
//Overlay* gOverlay = new Overlay();

//HFont hFont = NULL;
//void RenderFrame();
//void OnFrame();
//////////// END //////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
#include "FEATURE.h"
#include "Memory.h"
#include "About.h"
#include "OffsetManager.h"
#include "PLAYER.h"
#include <iostream>
#include <thread>
#include <commctrl.h>
#include <wchar.h>

/*
#include "imgui.h"
#include "imgui_impl_glut.h"
#include "imgui_impl_opengl2.h"
#include "Dependencies/glut_freeglut/glew.h"
#include "Dependencies/glut_freeglut/freeglut.h"

#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")
*/
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
//static ImVec4 back_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
static const char* StartGui = "Start";
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
///////////////// IMGUI //////////////////////////////////////////
//void MainGui();
//void MainUI();
//void StyleColorHacky();
//////////// END //////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

//This disables the console windows popping up
//#pragma comment(linker,"/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
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
		//OVERLAPPED ov = { 0,0,0,0,NULL };
		//ov.Offset = 0xFFFFFFFF;
		//ov.OffsetHigh = 0xFFFFFFFF;
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

			// work threads goes here //
			//tArray = CreateThread(NULL, 0, hkThread, NULL, 0, NULL);

			//tAimbot = CreateThread(NULL, 0, AimBotThread, NULL, 0, NULL);
			//tBunnyHop = CreateThread(NULL, 0, BunnyHopThread, NULL, 0, NULL);
			start();
			//gOverlay->OnFrame();
		}
		if (GetAsyncKeyState(VK_DELETE)) {
			exit(0);
		}
		Sleep(100);
	}
	

	// Create GLUT window
	/*Initialize();
	glutInit(&argc, argv);
#ifdef __FREEGLUT_EXT_H__
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
#endif
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_MULTISAMPLE);
	glutInitWindowSize(365, 490);
	//glutWind
	glutCreateWindow("PnkBstrA");
	
	glutDisplayFunc(MainGui);
	// Setup Dear ImGui context
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags = ImGuiWindowFlags_NoMove;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.Fonts->AddFontFromFileTTF("Fonts\\Ruda-Bold.ttf", 12);
	io.Fonts->AddFontFromFileTTF("Fonts\\Ruda-Bold.ttf", 10);
	io.Fonts->AddFontFromFileTTF("Fonts\\Ruda-Bold.ttf", 14);
	io.Fonts->AddFontFromFileTTF("Fonts\\Ruda-Bold.ttf", 18);
	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	
	//ImGui::StyleColorsClassic();
	StyleColorHacky();
	// Setup Platform/Renderer bindings
	ImGui_ImplGLUT_Init();
	ImGui_ImplGLUT_InstallFuncs();
	ImGui_ImplOpenGL2_Init();


	glutMainLoop();

	// Cleanup
	ImGui_ImplOpenGL2_Shutdown();
	ImGui_ImplGLUT_Shutdown();
	ImGui::DestroyContext();
	*/
	return 0;
	
	
}
/*
void StyleColorHacky() {
	ImGuiStyle* style = &ImGui::GetStyle();

	style->WindowPadding = ImVec2(15, 15);
	style->WindowRounding = 5.0f;
	style->FramePadding = ImVec2(5, 5);
	style->FrameRounding = 4.0f;
	style->ItemSpacing = ImVec2(12, 8);
	style->ItemInnerSpacing = ImVec2(8, 6);
	style->IndentSpacing = 25.0f;
	style->ScrollbarSize = 15.0f;
	style->ScrollbarRounding = 9.0f;
	style->GrabMinSize = 5.0f;
	style->GrabRounding = 3.0f;

	style->Colors[ImGuiCol_Text] = ImVec4(0.80f, 0.80f, 0.83f, 1.00f);
	style->Colors[ImGuiCol_TextDisabled] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style->Colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style->Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	style->Colors[ImGuiCol_PopupBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	style->Colors[ImGuiCol_Border] = ImVec4(0.80f, 0.80f, 0.83f, 0.88f);
	style->Colors[ImGuiCol_BorderShadow] = ImVec4(0.92f, 0.91f, 0.88f, 0.00f);
	style->Colors[ImGuiCol_FrameBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style->Colors[ImGuiCol_FrameBgActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 0.98f, 0.95f, 0.75f);
	style->Colors[ImGuiCol_TitleBgActive] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	style->Colors[ImGuiCol_MenuBarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	style->Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	//style->Colors[ImGuiCol_ComboBg] = ImVec4(0.19f, 0.18f, 0.21f, 1.00f);
	style->Colors[ImGuiCol_CheckMark] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	style->Colors[ImGuiCol_SliderGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	style->Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style->Colors[ImGuiCol_Button] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_ButtonHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style->Colors[ImGuiCol_ButtonActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_Header] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_HeaderHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_HeaderActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style->Colors[ImGuiCol_Column] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_ColumnHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style->Colors[ImGuiCol_ColumnActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style->Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	//style->Colors[ImGuiCol_CloseButton] = ImVec4(0.40f, 0.39f, 0.38f, 0.16f);
	//style->Colors[ImGuiCol_CloseButtonHovered] = ImVec4(0.40f, 0.39f, 0.38f, 0.39f);
	//style->Colors[ImGuiCol_CloseButtonActive] = ImVec4(0.40f, 0.39f, 0.38f, 1.00f);
	style->Colors[ImGuiCol_PlotLines] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
	style->Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
	style->Colors[ImGuiCol_PlotHistogram] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
	style->Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
	style->Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.25f, 1.00f, 0.00f, 0.43f);
	style->Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(1.00f, 0.98f, 0.95f, 0.73f);
}
////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////	 IMGUI WORK HERE  /////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
void MainGui() {
	// Start the Dear ImGui frame
	ImGui_ImplOpenGL2_NewFrame();
	ImGui_ImplGLUT_NewFrame();

	MainUI();
	// Rendering
	ImGui::Render();
	ImGuiIO& io = ImGui::GetIO();
	glViewport(0, 0, (GLsizei)io.DisplaySize.x, (GLsizei)io.DisplaySize.y);
	glClearColor(back_color.x, back_color.y, back_color.z, back_color.w);
	glClear(GL_COLOR_BUFFER_BIT);
	//glUseProgram(0); // You may want this if using this code in an OpenGL 3+ context where shaders may be bound, but prefer using the GL3+ code.
	ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());

	glutSwapBuffers();
	glutPostRedisplay();
}

void MainUI() {
	// MY UI IS HERE 
	//WALLHACK BOX
	SetProcessDPIAware();
	//SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);
	ImGui::Begin(" ",NULL,ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoCollapse|ImGuiWindowFlags_NoTitleBar);

	ImVec4 ob = { 14025,14025,0,179 };
	ImFont font = *ImGui::GetFont();
	font.FontSize = 18;
	//font.Scale
	font.Scale =1.5f;
	ImGui::PushFont(&font);
	ImGui::TextColored(ob, "PunkBuster");
	ImGui::PopFont();
	//ImFont font = *ImGui::GetFont();
	ImVec4 c = { 36,38,40,179 };
	ImGui::SetWindowFontScale(1.1f);
	ImGui::SameLine();
	ImGui::TextColored(c, "v0.1");
	ImGui::Text("Hack For CSGO By Juju");
	if (ImGui::CollapsingHeader("Wallhack/Esp")) {
		ImGui::Text("Setup You Wallhack/Esp!");
		ImGui::ColorEdit4("Glow Color Team", Offsets::colorTeam);
		ImGui::ColorEdit4("Glow Color Enemy", Offsets::colorEnemy);
		ImGui::Checkbox("Enable Glow", &wallhack);
	}
	if (ImGui::CollapsingHeader("Extras")) {
		ImGui::Checkbox("Enable NoFlash", &noflash);
		ImGui::Checkbox("Enable NoSmoke", &nosmoke);
	}
	if (ImGui::CollapsingHeader("Aim")) {
		ImGui::Text("Setup You Aimbot/Spinbot/Triggerbot");
		ImGui::Text("#Aimbot");
		ImGui::Checkbox("Enable Aimbot", &aimbot);
		ImGui::Text("#Triggerbot");
		ImGui::SliderInt("Fire Delay(ms)", &Offsets::TriggerBotTime,5, 100);
		ImGui::Checkbox("Enable Triggerbot", &triggerbot);
	}

	ImGui::TextColored(c, "FRAMERATE: %f", ImGui::GetIO().Framerate);
	ImGui::TextColored(c, "AVERAGE:   %f", 1000.0f / ImGui::GetIO().Framerate);
	if (ImGui::Button(StartGui, { 150,25 })) {
		if (started) {
			StartGui = "Stop";
		}
		else {
			StartGui = "Start";
		}
		start();
	}
	ImGui::End();zz
	//ImGui::ShowDemoWindow();
}
*/
////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////// THREAD FUNCTION'S /////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

void writeConfigFile() {
	

}

void start() {
	if (started) {
		started = false;

		//SuspendThread(tGetPlayers);
		SuspendThread(tArray);
		SuspendThread(hTriggerBot);
		SuspendThread(tAimbot);
		//WaitForSingleObject(tGetPlayers, WAIT_TIMEOUT);
		WaitForSingleObject(tArray, WAIT_TIMEOUT);
		WaitForSingleObject(hTriggerBot, WAIT_TIMEOUT);
		WaitForSingleObject(tAimbot, WAIT_TIMEOUT);
		CloseHandle(tArray);
		CloseHandle(tAimbot);
		CloseHandle(hTriggerBot);
	}
	else {
		started = true;
		// work threads goes here //
		//tGetPlayers = CreateThread(NULL, 0, GetPlayer, NULL, 0, NULL);
		tArray = CreateThread(NULL, 0, hkThread, NULL, 0, NULL);
		//tAimbot = CreateThread(NULL, 0, AimBotThread, NULL, 0, NULL);
		//hTriggerBot = CreateThread(NULL, 0, TriggerBotThread, NULL, 0, NULL);

	}
}

int Initialize() {

	HWND wnd = FindWindowA(0, "Counter-Strike: Global Offensive");
	//if (!gOverlay->Attach(wnd))
	//	return 2;
	GetWindowThreadProcessId(wnd, &pid);
	//gRenderer->OnSetup(gOverlay->GetDevice());
	//hFont = gRenderer->CreateFont("Verdana", 12, FONT_CREATE_SPRITE | FONT_CREATE_OUTLINE | FONT_CREATE_BOLD);
	//gOverlay->AddOnFrame(PLAYER::getPlayers);
	//gOverlay->AddOnFrame(OnFrame);
	// Get Process Handle Using Pid
	//cout << "Pid: " << pid << endl;
	if (pid == 0 || pid ==NULL) {
		MessageBox(hwnd, "Process Not Found", "Error", NULL);
		started = false;
		return -1;
	}
	//cout << "opid: " << pid << endl;
	Memory::pHandle = OpenProcess(PROCESS_ALL_ACCESS, 0, pid);
	if (Memory::pHandle == INVALID_HANDLE_VALUE) {
		MessageBox(hwnd, "Unable To Open Process", "Error", NULL);
		started = false;
		return -1;
	}
	//cout << "opid: " << GetProcessId(Memory::pHandle) << endl;
	// Create Thread To get offsets //
	HANDLE offsets = CreateThread(NULL, 0, GetOffsets, NULL, 0, NULL);
	//OffsetManager::SetupOffsets(pid);
	return 0;
}


/*void RenderFrame() {
	if (offsetsFound) {
		for (int i = 0; i < Offsets::DynamicOffsets::eLIST.size(); i++) {
			DWORD alive = Memory::Read<DWORD>((void*)(Offsets::DynamicOffsets::eLIST[i] + Offsets::Player::lifeState));
			DWORD dormant = Memory::Read<DWORD>((void*)(Offsets::DynamicOffsets::eLIST[i] + Offsets::Player::dormant));
			if (alive<0 || dormant) {
				continue;
			}
			Color color = Color::White();
			if (Offsets::DynamicOffsets::eTeam[i] == 2) {
				color = Color::Red();
			}
			else if (Offsets::DynamicOffsets::eTeam[i] == 3) {
				color = Color::Cyan();
			}
			Vector3 head = FEATURE::GetPlayerBonePos(Offsets::DynamicOffsets::eLIST[i], 8);
			head.z +=9;
			Vector3 feet = { head.x - 0,head.y - 0,head.z - 60 };
			
			Vector3 top = { 0,0,0 }, bottom = { 0,0,0 };
			int width, height;
			gOverlay->GetScreenSize(&width, &height);
			if (FEATURE::WorldToScreen(head, &top,width,height) && FEATURE::WorldToScreen(feet, &bottom,width,height)) {
				float h = bottom.y - top.y;
				float w = h / 5.0f;
				Vector3 body = FEATURE::GetPlayerBonePos(Offsets::DynamicOffsets::eLIST[i], 5);
				Vector3 pBody = FEATURE::GetPlayerBonePos(Offsets::DynamicOffsets::LocalPlayer, 5);
				Vector3 c1 = { 0,0,0 }, c2 = { 0,0,0 };
				FEATURE::WorldToScreen(body, &c1, width, height);
				FEATURE::WorldToScreen(pBody, &c2, width, height);
				gRenderer->DrawBorderBoxOut(top.x - w, top.y, w * 2, h, 1, color, Color::Red());
				gRenderer->DrawLine(c2.x, 0, c1.x, 0, Color::White());
				//gRenderer->DrawText(hFont, top.x, top.y + h, FONT_RENDER_CENTER_H, Color::White(), "JuJu");
			}
		}
	}
}



void OnFrame() {
	gRenderer->PreFrame();

	RenderFrame();

	int x = 0, y = 0;

	gOverlay->GetScreenSize(&x, &y);

	x -= 2;
	y = 0;

	//gRenderer->DrawText(hFont, x, y, FONT_RENDER_RIGHT, Color::White(), "External");
	gRenderer->PostFrame();
}
*/
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
		//cout << "client_panorama.dll " << Offsets::Modules::panoramaDll << endl;
		Offsets::Modules::engineDll = Memory::GetBaseAddress(pid, "engine.dll");
		//cout << "engine.dll " << Offsets::Modules::engineDll << endl;
		UINT glowObjectManager = Memory::FindOffsetEx(SIGNATURES.dwGlowObjectManager[0], SIGNATURES.dwGlowObjectManager[1], SIGNATURES.dwGlowObjectManager[2], atoi(SIGNATURES.dwGlowObjectManager[3]), atoi(SIGNATURES.dwGlowObjectManager[4]));
		//cout << "PglowObjectManager " << glowObjectManager << endl;
		UINT eListBase = Memory::FindOffsetEx(SIGNATURES.dwEntityList[0], SIGNATURES.dwEntityList[1], SIGNATURES.dwEntityList[2], atoi(SIGNATURES.dwEntityList[3]), atoi(SIGNATURES.dwEntityList[4]));
		//cout << "PeListBase " << eListBase << endl;
		UINT localplayer = Memory::FindOffsetEx(SIGNATURES.dwLocalPlayer[0], SIGNATURES.dwLocalPlayer[1], SIGNATURES.dwLocalPlayer[2], atoi(SIGNATURES.dwLocalPlayer[3]), atoi(SIGNATURES.dwLocalPlayer[4]));
		//cout << "plocalplayer " << localplayer << endl;
		//UINT dwClntState = Memory::FindOffsetEx(SIGNATURES.dwClientState[0], SIGNATURES.dwClientState[1], SIGNATURES.dwClientState[2], atoi(SIGNATURES.dwClientState[3]), atoi(SIGNATURES.dwClientState[4]));
		
		//UINT dwClntState_ViewAngles = Memory::FindOffsetEx(SIGNATURES.dwClientState_ViewAngles[0], SIGNATURES.dwClientState_ViewAngles[1], SIGNATURES.dwClientState_ViewAngles[2], atoi(SIGNATURES.dwClientState_ViewAngles[3]), atoi(SIGNATURES.dwClientState_ViewAngles[4]));


		Offsets::ENTITY_LIST = eListBase - Offsets::Modules::panoramaDll;
		Offsets::GLOWOBJECTMANAGER = glowObjectManager - Offsets::Modules::panoramaDll;
		Offsets::LOCALPLAYER_OFFSET = localplayer - Offsets::Modules::panoramaDll;
		
		//cout << " Offsets::ENTITY_LIST: " << hex << Offsets::ENTITY_LIST << endl;
		//cout << " Offsets::GLOWOBJECTMANAGER: " << hex << Offsets::GLOWOBJECTMANAGER << endl;
		//cout << " Offsets::LOCALPLAYER_OFFSET: " << hex << Offsets::LOCALPLAYER_OFFSET << endl;
		Offsets::DynamicOffsets::glowObjectManager = Memory::Read<unsigned int>((void*)(Offsets::Modules::panoramaDll + Offsets::GLOWOBJECTMANAGER));
		//cout << "glowObjectManager " << Offsets::DynamicOffsets::glowObjectManager << endl;
		Offsets::DynamicOffsets::eLISTBASE = Offsets::Modules::panoramaDll + Offsets::ENTITY_LIST;
		//cout << "eListBase " << Offsets::DynamicOffsets::eLISTBASE << endl;
		Offsets::DynamicOffsets::LocalPlayer = Memory::Read<unsigned int>((void*)(Offsets::Modules::panoramaDll + Offsets::LOCALPLAYER_OFFSET));
		//cout << "localplayer " << Offsets::DynamicOffsets::LocalPlayer << endl;
		/* */
		PLAYER::getPlayers();
		offsetsFound = true;
		return 0;
	}
	return 1;
}

DWORD WINAPI TriggerBotThread(LPVOID lparams) {

	while (1) {
		
		if (GetAsyncKeyState(VK_MENU)) {
			FEATURE::TriggerBot();
		}
	}
	return 0;
}
DWORD WINAPI AimBotThread(LPVOID lparams) {

	while (1) {
		FEATURE::Aimbot();
		
	}
	return 0;
}

DWORD WINAPI BunnyHopThread(LPVOID lparams) {

	while (1) {
		FEATURE::BunnyHop();

	}
	return 0;
}

DWORD WINAPI GetPlayer(LPVOID lpParam) {
	while (1) {
		PLAYER::getPlayers();
	
		Sleep(3000);
	}
}

DWORD WINAPI hkThread(LPVOID lpParam) {
	while (1) {
		//FEATURE::NoRecoil();
		//OffsetManager::SetupOffsets(pid);
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
bool dothis() {
	return threadSwitch;
}
