// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include <iostream>
#include <MinHook.h>
#include "debugUtils.h"

VOID Init()
{
    InitConsole();
    PutDebug("Loading");
    gameAsm = reinterpret_cast<uintptr_t>(GetModuleHandle(L"GameAssembly.dll"));

    MH_Initialize();
    CreateAndLoadHooks();
}

//Actual entry point
VOID Main()
{
    //init
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
