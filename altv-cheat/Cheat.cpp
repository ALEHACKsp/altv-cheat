#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <random>

#include "Cheat.h"

typedef __int64(__fastcall* _CreateHook)(__int64 a1, __int64 a2, unsigned __int64* a3);
typedef __int64(__fastcall* _EnableHook)(__int64 a1);
typedef unsigned __int64* (__fastcall* _ConsoleMessage)(unsigned __int64* a1, __int64 a2, __int64 a3);
typedef __int64(__fastcall* _NewString)(__int64 a1, __int64 a2, __int64 a3, __int64 a4, int a5);
typedef __int64(__fastcall* _Serial)();
typedef __int64(__fastcall* _LicenseHash)(__int64 a1, __int64 a2);

_ConsoleMessage ConsoleMessagePtr = nullptr;
_NewString NewStringPtr = nullptr;
_LicenseHash LicenseHashPtr = nullptr;

const uintptr_t gameBase = (uintptr_t)GetModuleHandleA(NULL);
const uintptr_t clientBase = (uintptr_t)GetModuleHandleA("altv-client.dll");
const _CreateHook CreateHook = (_CreateHook)(clientBase + 0x6AAC0);
const _EnableHook EnableHook = (_EnableHook)(clientBase + 0x6AD40);

class Script
{
public:
    std::string Path;
    std::string Code;

    Script(const char* path, const char* code)
    {
        Path = path;
        Code = code;
    }
};

class Resource
{
public:
    std::string Name;
    std::vector<Script> Scripts;

    Resource(const char* name)
    {
        Name = name;
    }
};

std::vector<Script> scripts;
std::vector<Resource> resources;

std::string findstr = "import";
std::string tmpstr;
std::string codestr;
std::string rmvstr;
std::string execstr;

bool found = false;
bool active = false;

__int64 serial = 0;

void remove_all(std::string& from, std::string const& to)
{
    size_t start_pos = 0;
    while ((start_pos = from.find(to, start_pos)) != std::string::npos) {
        from.replace(start_pos, to.length(), "");
        start_pos += 1;
    }
}

bool ends_with(std::string const& str, std::string const& ending)
{
    if (ending.size() > str.size()) return false;
    return std::equal(ending.rbegin(), ending.rend(), str.rbegin());
}

std::string random_string(size_t len)
{
    std::string str("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");

    std::random_device rd;
    std::mt19937 generator(rd());

    std::shuffle(str.begin(), str.end(), generator);

    return str.substr(0, len);
}

__int64 __fastcall DetourNewString(__int64 a1, __int64 a2, __int64 a3, __int64 a4, int a5)
{
	tmpstr = (const char*)a3;

    if (ends_with(tmpstr, ".js") || ends_with(tmpstr, ".mjs"))
        scripts.emplace_back(tmpstr.c_str(), codestr.c_str());
    else
    {
        codestr = (const char*)a3;
        remove_all(codestr, "\r");
    }

	if (!rmvstr.empty())
        remove_all(tmpstr, rmvstr);

	if (!found)
		if (tmpstr.find(findstr) != std::string::npos)
		{
			found = true;
			tmpstr += execstr;
		}

	return NewStringPtr(a1, a2, (__int64)tmpstr.c_str(), a4, -1);
}

unsigned __int64* __fastcall DetourConsoleMessage(unsigned __int64* a1, __int64 a2, __int64 a3)
{
    if (active)
    {
        resources.emplace_back((const char*)a2);

        for (auto it = scripts.begin(); it != scripts.end(); ++it)
            resources.back().Scripts.push_back(*it);

        scripts.clear();
        active = false;
    }

    if (std::string((const char*)a2) == " Loaded resource ")
        active = true;

    return ConsoleMessagePtr(a1, a2, a3);
}

__int64 __fastcall DetourSerial() { return serial; }

__int64 __fastcall DetourLicenseHash(__int64 a1, __int64 a2)
{
    __int64 result = LicenseHashPtr(a1, a2);

    strcpy_s((char*)(*(__int64*)a2), 65, random_string(64).c_str());

    return result;
}

void Executor::Initialize()
{
    const _ConsoleMessage ConsoleMessage = (_ConsoleMessage)(clientBase + 0x125A0);
    const _NewString NewString = (_NewString)(clientBase + 0x1AEEBE);

    CreateHook((__int64)ConsoleMessage, (__int64)DetourConsoleMessage, (unsigned __int64*)(&ConsoleMessagePtr));
    CreateHook((__int64)NewString, (__int64)DetourNewString, (unsigned __int64*)(&NewStringPtr));

    EnableHook((__int64)ConsoleMessage);
    EnableHook((__int64)NewString);

    CreateDirectoryA("C:\\AltCheat", NULL);
}

void Executor::File(std::string const& str)
{
    std::ifstream script;

	script.open("C:/AltCheat/" + str);
	if (script)
	{
		std::getline(script, execstr, '\0');
		std::cout << "Successfully loaded file!" << std::endl;
	}
	else
		std::cout << "Couldn't find file!" << std::endl;

	script.close();
}

void Executor::Find(std::string const& str)
{
	findstr = str;
	std::cout << "Successfully set the find string!" << std::endl;
}

void Executor::Remove(std::string const& str)
{
	rmvstr = str;
	std::cout << "Successfully set the remove string!" << std::endl;
}

void Executor::Serial(std::string const& str)
{
    const _Serial SocialId = (_Serial)(clientBase + 0x21630);
    const _Serial MachineGuid = (_Serial)(clientBase + 0x209E0);
    const _Serial ProductId = (_Serial)(clientBase + 0x208A0);
    const _Serial LicenseHash = (_Serial)(clientBase + 0x21410);

    CreateHook((__int64)SocialId, (__int64)DetourSerial, nullptr);
    CreateHook((__int64)MachineGuid, (__int64)DetourSerial, nullptr);
    CreateHook((__int64)ProductId, (__int64)DetourSerial, nullptr);
    CreateHook((__int64)LicenseHash, (__int64)DetourLicenseHash, (unsigned __int64*)(&LicenseHashPtr));

    EnableHook((__int64)SocialId);
    EnableHook((__int64)ProductId);
    EnableHook((__int64)MachineGuid);
    EnableHook((__int64)LicenseHash);

    strcpy_s((char*)(gameBase + 0x2D3229F), 13, random_string(12).c_str());

    serial = std::stoi(str);
    std::cout << "Successfully set the serial!" << std::endl;
}

void Executor::Weapon()
{
    memset((void*)(gameBase + 0x10FB898), 0x90, 6);
    std::cout << "Successfully blocked weapon checks!" << std::endl;
}

void Executor::Dump()
{
    std::cout << "Started dumping resources..." << std::endl;

    CreateDirectoryA("C:\\AltCheat", NULL);
    CreateDirectoryA("C:\\AltCheat\\dumper", NULL);

    for (auto res = resources.begin(); res != resources.end(); ++res)
    {
        CreateDirectoryA(std::string("C:\\AltCheat\\dumper\\" + res->Name).c_str(), NULL);
        std::cout << "Created directory: " << res->Name << std::endl;

        for (auto script = res->Scripts.begin(); script != res->Scripts.end(); ++script)
        {
            std::string dirPath = "C:\\AltCheat\\dumper\\" + res->Name;
            std::string scriptPath = script->Path;

            while (scriptPath.find("/") != std::string::npos)
            {
                std::size_t pos = scriptPath.find("/");
                dirPath += "\\" + scriptPath.substr(0, pos);
                scriptPath = scriptPath.substr(pos + 1);

                CreateDirectoryA(dirPath.c_str(), NULL);
            }

            std::ofstream file("C:/AltCheat/dumper/" + res->Name + "/" + script->Path);
            file << script->Code;

            std::cout << "Created file: " << script->Path << std::endl;
        }
    }

    std::cout << "Successfully dumped resources!" << std::endl;
}