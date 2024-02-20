#pragma once

//Define your hooks here, or somewhere else and include them here
//Example from my infinite money plugin:
/*
int32_t(__fastcall* CareerStatus_GetCash_o)(DWORD*, const DWORD*);
int32_t __stdcall CareerStatus_GetCash_hook(DWORD* __this, const DWORD* method)
{
    int32_t returnResult = 1000000;//CareerStatus_GetCash_o(__this, method);
    Unity::CComponent* caller = (Unity::CComponent*)__this; //Gets the instance of the current unity gameobject component
    QUICKDEBUG("CareerStatus_GetCash called with result: " << returnResult);
    return returnResult;
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
    uintptr_t CareerStatus_GetCashOffset = std::stoull(functionOffsets[0].value, nullptr, 16);
    MH_CreateHook(
        reinterpret_cast<LPVOID*>(gameAsm + CareerStatus_GetCashOffset),
        &CareerStatus_GetCash_hook,
        (LPVOID*)&CareerStatus_GetCash_o);
     */

    //Common hooks aka hooks to windows API and more will be created here
    //CreateCommonHooks();
    
    MH_STATUS status = MH_EnableHook(MH_ALL_HOOKS); //Get the hook status, 0 = ALl good

    QUICKDEBUG("Hooks loaded with status: " << status);
}
