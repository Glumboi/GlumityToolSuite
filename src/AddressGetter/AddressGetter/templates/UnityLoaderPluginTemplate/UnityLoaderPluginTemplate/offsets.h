#pragma once
#include <vector>

uintptr_t gameAsm;

struct GameFunction
{
    uintptr_t offset;
    std::string demangledName;
};
