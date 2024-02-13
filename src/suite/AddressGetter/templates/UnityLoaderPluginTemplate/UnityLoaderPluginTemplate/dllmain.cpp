// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include <MinHook.h>
#include "debugUtils.h"

VOID Init()
{
    InitConsole();
    gameAsm = reinterpret_cast<uintptr_t>(GetModuleHandle("GameAssembly.dll"));
    MH_Initialize();
    CreateAndLoadHooks();
}

VOID Main()
{
    Init();
}

BOOL APIENTRY DllMain(HMODULE hModule,
                      DWORD ul_reason_for_call,
                      LPVOID lpReserved
)
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
