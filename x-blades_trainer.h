#pragma once

#include <Windows.h>
#include <iostream>
#include <vector>

class Trainer
{
public:
	Trainer();
	DWORD GetProcID(const wchar_t* name);
	uintptr_t GetModuleBaseAddress(DWORD procID);
	uintptr_t FindDMAAddy(HANDLE hProc, uintptr_t ptr, std::vector<unsigned int> offsets);
	uintptr_t HealthAddr;
	uintptr_t RageAddr;
	uintptr_t SoulsAddr;

	void KeyPressCheck();
	void InfiniteHealth(unsigned int HealthAddr);
	void InfiniteRage(unsigned int RageAddr);
	void AddSouls(unsigned int Addr, int Amount);
	const LPCSTR Game = "X-Blades";
	const wchar_t* wGame = L"xblades.exe";
	const float MaxHealthValue = 600.0f;
	const float MaxRageValue = 100.0f;
	float Health_Value = 0;
	float Rage_Value = 0;

	bool RunMainLoop = true;
	HANDLE XBlades;
	DWORD dwProcID;
	bool NeedsUpdate;
	bool Health_Status;
	bool Rage_Status;
};