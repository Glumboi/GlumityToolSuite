#pragma once
#include "fstream"
#include "ios"
#include <direct.h>
#include <filesystem>
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
    static HANDLE gameHandle;
    inline static BYTE reloadKey = VK_F9;
    ModLoader();
    void Init();
    void LoadConfig(const std::string fileName);
    void LoadAllPlugins();
    void UnloadAllPlugins();
    void DumpIL2CPPBinary();
    void SatisfyAllPluginRequests();

private:
    std::string workingDirectory;
    std::string pluginsPath;
    std::string appDataPath;
    std::string gameAsmPath;
    std::string assemblyCreationTimeFile;
    std::string gameMetadataPath;
    std::string dumperPath;
    std::string addressGetterPath;
    std::string command;
    std::string dumpOutPath;
    std::string assemblyCreationTime;
    std::string storedAssemblyCreationTime;
    std::vector<HMODULE> loadedPlugins;
    bool skipASI = true;
    bool useConsole = true;
    void LoadPlugin(const std::string& path);
    void UpdateCreationTimeFile();
    bool IsGameANewerVersion();
    void LoadAssemblyCreationTime();

    template <typename T>
    T GetValueFromConfigFile(std::ifstream& cfgFileIn, const std::string& key, bool isBoolOrInt)
    {
        if (cfgFileIn.good())
        {
            std::string sLine;
            while (getline(cfgFileIn, sLine))
            {
                size_t found = sLine.find(key);
                if (found != std::string::npos)
                {
                    size_t lastEq = sLine.find('=') + 1;
                    std::string valueStr = sLine.substr(lastEq, sLine.length() - lastEq);

                    if (isBoolOrInt)
                        return T(std::stoi(valueStr.c_str()));
                    return T(valueStr.c_str());
                }
            }
        }
        // Return a default-constructed value if key not found or file not good
        return T();
    }
};

struct KeyboardHandler
{
    static void KeyBoardLoop(ModLoader* loaderInstance);
};
