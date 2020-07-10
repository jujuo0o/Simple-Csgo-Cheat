#pragma once
#include <Windows.h>
#include <TlHelp32.h>
#define PI 3.14159265f

class Memory {

public:
	static HANDLE pHandle;

	static unsigned int GetBaseAddress(DWORD , const char* );
	static MODULEENTRY32 GetModuleEntry(const char* );
	static UINT FindPatternEx(const char*, const char*, const char*, unsigned int);
	static DWORD ScanPatternEx(HANDLE , DWORD , DWORD , BYTE* , const char* , int );
	static DWORD FindOffsetEx(const char*, const char*, const char*, unsigned int, unsigned int);

	template <typename R>
	static R Read(void *location) {
		R data;
		ReadProcessMemory(pHandle, location, &data, sizeof(R), 0);
		return data;
	}

	template <typename R>
	static void Write(void *location, R data) {
		WriteProcessMemory(pHandle, location, &data, sizeof(R), 0);
	}

	template <typename R>
	static void Write(void *location, int size, R *in){
		WriteProcessMemory(pHandle, location, in, size, 0);
	}

};
