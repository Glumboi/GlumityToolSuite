#pragma once
#include "fstream"
#include "ios"
#include <direct.h>
#include <filesystem>
#include <iomanip>  // For std::setw
#include <iostream>
#include <string>
#include <vector>
#include <windows.h>
#include <winnt.h>
#include <thread>

#define KEYISPRESSED < 0
#define KEYISNOTPRESSED > 0

enum ModLoaderInitState
{
    //--> implement at some point
    UNDEFINED,
    SUCC,
    ERR,
    WARN
};

class ModLoader
{
public:
    bool skipASI = true;
    static HANDLE gameHandle;
    ModLoader();
    void Init();
    void LoadConfig(const std::string fileName);
    void LoadAllPlugins();
    void UnloadAllPlugins();
    void DumpIL2CPPBinary();

private:
    std::string pluginsPath;
    std::string appPath;
    std::string appDataPath;
    std::string gameAsmPath;
    std::string gameMetadataPath;
    std::string dumperPath;
    std::string command;
    std::string dumpOutPath;
    std::vector<HMODULE> loadedPlugins;
    void LoadPlugin(const std::string& path);
};

struct KeyboardHandler
{
    static void KeyBoardLoop(ModLoader* loaderInstance);
};
