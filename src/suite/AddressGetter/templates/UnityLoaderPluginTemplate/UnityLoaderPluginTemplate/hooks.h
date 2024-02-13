#pragma once
#include <iostream>
#include "offsets.h"
#define MODNAME "[ModName]"
#define JSONFILE "\\Plugins\\ModName.offsetRequests.json"
#define QUICKDEBUG(msg) std::cout << MODNAME << ": " <<  msg << "\n"

//Define your hooks here, or somewhere else and include them here
//Example from my infinite money plugin:
/*
int32_t(__fastcall* CareerStatus__GetCash_o)(DWORD*, DWORD*);

int32_t __stdcall CareerStatus__GetCash_hook(DWORD* __tis, DWORD* method)
{
    return 1000000;
}
*/

inline void CreateAndLoadHooks()
{
    //Load addresses from json file
    QUICKDEBUG("Loading offsets from " << JSONFILE);
    //Append json path with dll location
    char buffer[MAX_PATH];
    GetCurrentDirectory(MAX_PATH, buffer);
    std::string dllJsonPath = std::string(buffer) + JSONFILE;

    //Load offsets
    functionOffsets = readJsonFromFile(dllJsonPath).offsetRequests;

    //Print all offsets for debugging, remove when you are done testing your plugin!
    for (auto& offset : functionOffsets)
    {
        QUICKDEBUG("Offset loaded: " << offset.searchName << " at " << offset.value);
    }

    //Instantiate your hooks here using MH_CreateHook

    //Example from my infinite money plugin:
    /*
    MH_CreateHook(
        reinterpret_cast<LPVOID*>(gameAsm + getCashOffset),
        &CareerStatus__GetCash_hook,
     */

    MH_STATUS status = MH_EnableHook(MH_ALL_HOOKS); //Get the hook status, 0 = ALl good

    QUICKDEBUG("Hooks loaded with status: " << status);
}
