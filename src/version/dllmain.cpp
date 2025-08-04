#pragma once

#include "exports.h"

#include "windows.h"
#include "modLoader.h"
#include "debugUtils.h"

// Remove this line if you aren't proxying any functions.
HMODULE hModule = LoadLibrary("C:\\Windows\\System32\\version.dll");
HANDLE prHandle = GetCurrentProcess();

// This is global because the keyboard loop wouldnt work otherwise
ModLoader modLoader{};

void Setup()
{
	CLEAR_LOG();
	modLoader.LaunchModLoaderThread();
}

void Main()
{
	//Check if we are already running
	HANDLE hMutex = CreateMutex(NULL, TRUE, "GlummysPCBS2ModdingToolsuite");
	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		//We are already running, stop
		CloseHandle(hMutex);
		TimeStampDebug("An instance is already running, stopping duplicated instance...");
		return;
	}
	Setup();
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
