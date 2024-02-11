#pragma once

#include "exports.h"

#include "windows.h"
#include "offsets.h"
#include "modLoader.h"
#include "debugUtils.h"

// Remove this line if you aren't proxying any functions.
HMODULE hModule = LoadLibrary("C:\\Windows\\System32\\version.dll");
HANDLE prHandle = GetCurrentProcess();

ModLoader* modLoaderInstance = nullptr;

void Init()
{
    InitConsole();

    //Load base address of GameAssembly.dll
    gameAssembly = reinterpret_cast<uintptr_t>(GetModuleHandle("GameAssembly.dll"));

    if (gameAssembly != NULL)
    {
        TimeStampDebug("GameAssembly.dll base address: " + std::to_string(gameAssembly));
    }
    else // Notify user that we are in "universal" mode,
    //this loader can be used on any process actually, no need for specific modes or tweaks
    {
        TimeStampDebug("Could not load GameAssembly.dll, universal mode is risky!");
    }

    TimeStampDebug("Initializing ModLoader instance...");
    modLoaderInstance = new ModLoader();
}

void Main()
{
    Init();
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        CreateThread(0, 0, (LPTHREAD_START_ROUTINE)Main, 0, 0, 0);
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
