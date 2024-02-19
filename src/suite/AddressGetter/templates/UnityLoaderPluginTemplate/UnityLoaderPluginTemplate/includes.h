#pragma once
#define MODNAME "[DirectXHookTest]"
#define JSONFILE "\\Plugins\\DirectXHookTest.offsetRequests.json"
#define QUICKDEBUG(msg) std::cout << MODNAME << ": " <<  msg << "\n"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <MinHook.h>
#include <sstream>

#include "./IL2CppResolver/IL2CPP_Resolver.hpp"

#include "offsets.h"
#include "commonHooks.h"
#include "hooks.h"
#include "debugUtils.h"
