#include "x-blades_trainer.h"

#include <iomanip>
#include <TlHelp32.h>
#include <ostream>

int main()
{
    Trainer xbTrainer = Trainer();
    return 0;
}

Trainer::Trainer() : Health_Status(false), Rage_Status(false), NeedsUpdate(true), XBlades(nullptr)
{
    const unsigned int Addr = 0x003E5654;
    const std::vector<unsigned int> Health_Offsets = { 0x44, 0x60, 0x3C, 0x118, 0xE0, 0x90 };
    const std::vector<unsigned int> Rage_Offsets = { 0x44, 0x60, 0xE8, 0x1A4, 0xC0 };
    const std::vector<unsigned int> Souls_Offsets = { 0x14, 0x234 };
    HANDLE GameWindow = nullptr;
    std::cout << "Looking for X-Blades.\n";

    while (dwProcID == 0)
    {
        dwProcID = GetProcID(wGame);
        if(dwProcID == 0)
        {
            DWORD lasterror = GetLastError();
            std::cout << lasterror << "\n";
        }
        Sleep(1000);
    }

    if(dwProcID != 0)
    {
        XBlades = OpenProcess(PROCESS_ALL_ACCESS, false, dwProcID);
        if (XBlades == INVALID_HANDLE_VALUE || XBlades == NULL)
        {
            int x;
            std::cout << "Failed to open process.\n" << XBlades;
            std::cin >> x;
            exit(1);
        }

        uintptr_t moduleBase = GetModuleBaseAddress(dwProcID);
        uintptr_t BaseAddrPtr = moduleBase+Addr;

        while (RunMainLoop)
        {
            HealthAddr = FindDMAAddy(XBlades, BaseAddrPtr, Health_Offsets);
            RageAddr = FindDMAAddy(XBlades, BaseAddrPtr, Rage_Offsets);
            SoulsAddr = FindDMAAddy(XBlades, moduleBase + 0x003E54A4, Souls_Offsets);

            if(NeedsUpdate)
            {
                std::cout << "\033[2J\033[H";
                std::cout << "======================\n   X-Blades Trainer\n======================\n\n";

                ReadProcessMemory(XBlades, (BYTE*)HealthAddr, &Health_Value, sizeof(Health_Value), nullptr);
                std::cout << "Health: " << std::dec << Health_Value << "\n";

                ReadProcessMemory(XBlades, (BYTE*)RageAddr, &Rage_Value, sizeof(Rage_Value), nullptr);
                std::cout << "Rage: " << std::dec << Rage_Value << "\n\n";

                std::cout << "[F1] Health " << (Health_Status ? "(\033[32mon\033[0m)" : "(\033[31moff\033[0m)") << "\n";
                std::cout << "[F2] Rage " << (Rage_Status ? "(\033[32mon\033[0m)" : "(\033[31moff\033[0m)") << "\n";
                std::cout << "[F3] Add 1,000,000 Souls\n";
                std::cout << "[F4] Add 10,000,000 Souls\n";
                std::cout << "[F5] Exit\n";
                NeedsUpdate = false;
            }

            KeyPressCheck();

            if (Health_Status)
            {
                InfiniteHealth(HealthAddr);
            }

            if (Rage_Status)
            {
                InfiniteRage(RageAddr);
            }

            Sleep(100);
        }
    }
    else
    {
        std::string uinpit;
        std::cout << "Process ID not found!\n";
        std::cin >> uinpit;
    }
}

DWORD Trainer::GetProcID(const wchar_t* name)
{
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    DWORD procID = 0;

    if (hSnap != INVALID_HANDLE_VALUE)
    {
        PROCESSENTRY32 procEntry;
        procEntry.dwSize = sizeof(procEntry);

        if (Process32First(hSnap, &procEntry))
        {
            do
            {
                if (!_wcsicmp(procEntry.szExeFile, name))
                {
                    procID = procEntry.th32ProcessID;
                    break;
                }
            } while (Process32Next(hSnap, &procEntry));
        }
    }
    CloseHandle(hSnap);
    return procID;
}

void Trainer::KeyPressCheck()
{
    if (GetKeyState(VK_F1) & 0x8000)
    {
        Health_Status = !Health_Status;
        NeedsUpdate = true;
    }

    if (GetKeyState(VK_F2) & 0x8000)
    {
        Rage_Status = !Rage_Status;
        NeedsUpdate = true;
    }

    if (GetKeyState(VK_F3) & 0x8000)
    {
        AddSouls(SoulsAddr, 1000000);
    }

    if (GetKeyState(VK_F4) & 0x8000)
    {
        AddSouls(SoulsAddr, 10000000);
    }

    if (GetKeyState(VK_F5) & 0x8000)
    {
        CloseHandle(XBlades);
        RunMainLoop = false;
    }
}

void Trainer::InfiniteHealth(unsigned int HealthAddr)
{
    if(ReadProcessMemory(XBlades, (BYTE*)HealthAddr, &Health_Value, sizeof(Health_Value), nullptr))
    {
        WriteProcessMemory(XBlades, (BYTE*)HealthAddr, &MaxHealthValue, sizeof(MaxHealthValue), nullptr);
    }
}

void Trainer::InfiniteRage(unsigned int RageAddr)
{
    if(ReadProcessMemory(XBlades, (BYTE*)RageAddr, &Rage_Value, sizeof(Rage_Value), nullptr))
    {
        WriteProcessMemory(XBlades, (BYTE*)RageAddr, &MaxRageValue, sizeof(MaxRageValue), nullptr);
    }
}

void Trainer::AddSouls(unsigned int Addr, int Amount)
{
    int OldSoulCount;
    if(ReadProcessMemory(XBlades, (BYTE*)Addr, &OldSoulCount, sizeof(OldSoulCount), nullptr))
    {
        Amount = Amount + OldSoulCount;
        WriteProcessMemory(XBlades, (BYTE*)Addr, &Amount, sizeof(Amount), nullptr);
    }
}

uintptr_t Trainer::GetModuleBaseAddress(DWORD procID)
{
    uintptr_t modBaseAddr = 0;
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, procID);
    if (hSnap != INVALID_HANDLE_VALUE)
    {
        MODULEENTRY32 modEntry;
        modEntry.dwSize = sizeof(modEntry);
        if (Module32First(hSnap, &modEntry))
        {
	        do
	        {
                if (!_wcsicmp(modEntry.szModule, wGame))
                {
                    modBaseAddr = (uintptr_t)modEntry.modBaseAddr;
                    break;
                }
            } while (Module32Next(hSnap, &modEntry));
        }
    }
    CloseHandle(hSnap);
    return modBaseAddr;
}

uintptr_t Trainer::FindDMAAddy(HANDLE hProc, uintptr_t ptr, std::vector<unsigned int> offsets)
{
    uintptr_t addr = ptr;
    for (unsigned int i = 0; i < offsets.size(); ++i)
    {
        ReadProcessMemory(hProc, (BYTE*)addr, &addr, sizeof(addr), 0);
        addr += offsets[i];
    }
    return addr;
}