#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <iostream>
#include <string>

#include "Cheat.h"

DWORD WINAPI Main()
{
    AllocConsole();
    SetConsoleTitleA("alt:V Script Executor & Dumper");

    freopen_s((FILE**)stdin, "conin$", "r", stdin);
    freopen_s((FILE**)stdout, "conout$", "w", stdout);

    Executor::Initialize();

    std::string buffer;
    std::size_t split;

    while (true)
    {
        std::getline(std::cin, buffer);

        split = buffer.find(" ");

        if (split == std::string::npos)
        {
            if (buffer.find("dump") != std::string::npos)
                Executor::Dump();
            else if (buffer.find("weapon") != std::string::npos)
                Executor::Weapon();

            continue;
        }

        if (buffer.find("exec") != std::string::npos)
            Executor::File(buffer.substr(split + 1));
        else if (buffer.find("find") != std::string::npos)
            Executor::Find(buffer.substr(split + 1));
        else if (buffer.find("remove") != std::string::npos)
            Executor::Remove(buffer.substr(split + 1));
        else if (buffer.find("serial") != std::string::npos)
            Executor::Serial(buffer.substr(split + 1));
    }
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    if (ul_reason_for_call == DLL_PROCESS_ATTACH)
    {
        HANDLE hThread = CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)Main, hModule, 0, nullptr);
        if (hThread)
        {
            CloseHandle(hThread);
            DisableThreadLibraryCalls(hModule);
        }
    }
    return TRUE;
}
