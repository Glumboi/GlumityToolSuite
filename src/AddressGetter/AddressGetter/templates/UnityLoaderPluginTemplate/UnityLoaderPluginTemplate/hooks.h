#pragma once

//Define your hooks here, or somewhere else and include them here
//Example from my infinite money plugin:

/*
using CareerStatus__GetCash_t = int32_t(__fastcall*)(DWORD*, DWORD*);
CareerStatus__GetCash_t CareerStatus__GetCash_o; 

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
    uintptr_t getCashOffset = std::stoull(functionOffsets[0].value, nullptr, 16);
    MH_CreateHook(
        reinterpret_cast<LPVOID*>(gameAsm + getCashOffset),
        &CareerStatus__GetCash_hook,
        (LPVOID*)&CareerStatus__GetCash_o);
     */


    //Common hooks aka hooks to windows API and more will be created here
    //CreateCommonHooks();
    
    MH_STATUS status = MH_EnableHook(MH_ALL_HOOKS); //Get the hook status, 0 = ALl good

    QUICKDEBUG("Hooks loaded with status: " << status);
}
