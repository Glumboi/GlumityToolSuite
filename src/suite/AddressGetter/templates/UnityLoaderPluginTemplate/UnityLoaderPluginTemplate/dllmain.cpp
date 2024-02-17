// dllmain.cpp : Defines the entry point for the DLL application.
#include "includes.h"

int InitIL2CPPResolver()
{
    //init IL2CPP
    if (IL2CPP::Initialize(true) && IL2CPP::Thread::Attach(IL2CPP::Domain::Get()))
    {
        return 1;
    }

    return 0;
}

VOID Init()
{
    InitConsole();
    gameAsm = reinterpret_cast<uintptr_t>(GetModuleHandle("GameAssembly.dll"));
    MH_Initialize();

    if (InitIL2CPPResolver())
    {
        TimeStampDebug("Initialized Il2CPP, loading hooks");
        CreateAndLoadHooks();
        return;
    }

    TimeStampDebug("Failed to initialize IL2CPP, aborting hook initialization...");
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
