#pragma once
#include "debugUtils.h"
#include "offsets.h"
#define MODNAME "[ModName]"

inline void CreateAndLoadHooks()
{
    MH_STATUS status = MH_EnableHook(MH_ALL_HOOKS); //Get the hook status, 0 = ALl good

    //Proof of concept, this dll will only have 1 hook, thus hard coded, don't do that in 'real' mods!
    std::cout << MODNAME << ": has loaded hooks with status: " << status << '\n';
}
