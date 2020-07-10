#include <Windows.h>
#include <TlHelp32.h>
#include "Memory.h"
#include <iostream>
#include "OffsetManager.h"
HANDLE Memory::pHandle;

unsigned int Memory::GetBaseAddress(DWORD pid, const char * module) {
	unsigned int address = 0;
	HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, pid);
	if (snap != INVALID_HANDLE_VALUE) {
		MODULEENTRY32 mEntry;
		mEntry.dwSize = sizeof(mEntry);
		if (Module32First(snap, &mEntry)) {
			do {
				if (!strcmp(mEntry.szModule,module))
				{
					address = (unsigned int)mEntry.modBaseAddr;
					break;
				}
			} while (Module32Next(snap, &mEntry));
		}
	}
	CloseHandle(snap);
	return address;
}

MODULEENTRY32 Memory::GetModuleEntry(const char * Module) {
	MODULEENTRY32 mod;
	if (pHandle == INVALID_HANDLE_VALUE) {
		std::cout << "Invalid";
	}
	HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE32 | TH32CS_SNAPMODULE, GetProcessId(pHandle));
	if (snap != INVALID_HANDLE_VALUE) {
		MODULEENTRY32 rEntry;
		rEntry.dwSize = sizeof(rEntry);
		bool mod32 = Module32First(snap, &rEntry);
		if (mod32) {
			do {
				if (!strcmp(rEntry.szModule, Module)) {			// Strcmp return 0 if strings are equal and any other number depending upon the difference b/w characters
					mod = rEntry;
				}
			} while (Module32Next(snap, &rEntry));
		}
	}

	CloseHandle(snap);
	return mod;
}

DWORD Memory::ScanPatternEx(HANDLE hProc, DWORD base, DWORD len, BYTE* sig, const char* mask, int offset)
{
	BYTE* buf = (BYTE*)VirtualAlloc(0, len, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (ReadProcessMemory(hProc, (LPCVOID)base, buf, len, NULL) == false)
	{
		char buf2[64]; 
		return NULL;
	}
	for (int i = 0; i <= (len - strlen(mask)) + 1; i++)
	{
		if ((buf[i] == sig[0] && mask[0] == 'x') || (mask[0] == '?'))
		{
			for (int x = 0; ; x++)
			{
				if (mask[x] == 'x')
				{
					if (buf[i + x] == sig[x])
						continue;
					else
						break;
				}
				else if (mask[x] == 0x00)
				{
					return (DWORD)(base + i + offset);
				}
			}
		}
	}
	return NULL;
}

UINT Memory::FindPatternEx(const char* Module, const char* pattern, const char* mask, unsigned int offset)
{
	MODULEENTRY32 mod32 = Memory::GetModuleEntry(Module);
	UINT baseOfModule = (UINT)mod32.modBaseAddr;
	UINT endOfModule = (UINT)mod32.modBaseSize + baseOfModule;

	UINT readLocation = baseOfModule;
	UINT lpSigAddress = 0;
	while (readLocation < endOfModule)
	{
		
		lpSigAddress = Memory::ScanPatternEx(Memory::pHandle, readLocation, 4096, (BYTE*)pattern, mask, offset);
		if (lpSigAddress != 0)
		{
			return lpSigAddress;
		}
		readLocation += 4096;
	}
	return 0;
}


DWORD Memory::FindOffsetEx(const char* Module, const char* pattern, const char* mask, unsigned int offsets, unsigned int size)
{
	UINT lpAddr = Memory::FindPatternEx(Module, pattern, mask, offsets);
	DWORD offset = Memory::Read<DWORD>((void*)lpAddr);
	if (offset == 0)
	{
		return 0;
	}
	return offset + size;
}