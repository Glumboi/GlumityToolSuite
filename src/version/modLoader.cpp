#include "ModLoader.h"
#include "debugUtils.h"

using PluginEntryPoint = void(*)();

ModLoader::ModLoader()
{
    Init();
}

void ModLoader::Init()
{
    char buffer[MAX_PATH];
    GetCurrentDirectory(MAX_PATH, buffer);
    workingDirectory = std::string(buffer);
    TimeStampDebug("Working Directory: " + workingDirectory);
    LoadConfig("./GlummyLoader.cfg");
    DumpIL2CPPBinary();
    CreateThread(0, 0, (LPTHREAD_START_ROUTINE)KeyboardHandler::KeyBoardLoop, this, 0, 0);
}

void ModLoader::LoadConfig(const std::string fileName)
{
    //Create a small config file
    if (!std::filesystem::exists(fileName))
    {
        std::ofstream cfgFile(fileName);
        cfgFile << "LoadASI=0";
        cfgFile.close();
    }

    std::ifstream cfgFileIn(fileName);
    if (cfgFileIn.good())
    {
        std::string sLine;
        getline(cfgFileIn, sLine);
        size_t lastEq = sLine.find('=') + 1;
        std::string toggleSbStr = sLine.substr(lastEq, sLine.length() - lastEq);
        TimeStampDebug("LOAD ASI: " + toggleSbStr);
        skipASI = toggleSbStr == "0";
    }
}

void ModLoader::LoadPlugin(const std::string& path)
{
    HMODULE hModule = LoadLibrary(path.c_str());
    if (hModule == nullptr)
    {
        DWORD error = GetLastError();
        PutDebug("Failed to load library " + path + " with error: " + (char*)error);
    }
    else
    {
        this->loadedPlugins.push_back(hModule);
        PutDebug("Loaded library: " + path);

        // Retrieve and call the entry point function if needed (idk if this will ever be used for something more specific
        // doesnt really do anything)
        PluginEntryPoint entryPoint = reinterpret_cast<PluginEntryPoint>(GetProcAddress(hModule, "PluginEntryPoint"));
        if (entryPoint != nullptr)
        {
            entryPoint(); // Call the entry point function
        }
    }
}

void ModLoader::LoadAllPlugins()
{
    for (const auto& entry : std::filesystem::directory_iterator(pluginsPath))
    {
        std::filesystem::path extension = entry.path().extension();
        if (extension == ".dll")
        {
            LoadPlugin(entry.path().string());
        }
        else if (extension == ".asi")
        {
            if (!skipASI)
            {
                LoadPlugin(entry.path().string());
            }
        }
    }
}

void ModLoader::UnloadAllPlugins()
{
    for (HMODULE hModule : loadedPlugins)
    {
        FreeLibrary(hModule);
    }
    loadedPlugins.clear();
}

void ModLoader::DumpIL2CPPBinary()
{
    //search and assign a folder that ends with _Data in the game directory
    for (const auto& entry : std::filesystem::directory_iterator(workingDirectory))
    {
        //Get last 5 chars of the path
        std::string path = entry.path().string();
        std::string last5 = path.substr(path.length() - 5, 5);
        TimeStampDebug("Currently checking: " + path);
        TimeStampDebug("Checking last 5: " + last5);

        if (last5 == "_Data")
        {
            appDataPath = path;
            TimeStampDebug("Above path is a game data folder, continuing...");
            break;
        }
    }

    if (appDataPath.empty())
    {
        TimeStampDebug("Cannot find a game data folder, are you running a unity game?");
    }

    TimeStampDebug("Game path: " + workingDirectory);
    TimeStampDebug("Data path: " + appDataPath);

    gameAsmPath = workingDirectory + "\\GameAssembly.dll";
    gameMetadataPath = appDataPath + "\\il2cpp_data\\Metadata\\global-metadata.dat";
    pluginsPath = workingDirectory + "\\Plugins";
    dumperPath = workingDirectory + "\\dumper\\Il2CppDumper.exe";
    addressGetterPath = workingDirectory + "\\AddressGetter\\AddressGetter.exe";
    dumpOutPath = workingDirectory + "\\DumpedIL2CPP";
    command = " " + gameAsmPath + " " + gameMetadataPath + " " + dumpOutPath;

    TimeStampDebug("Dumper path: " + dumperPath);
    TimeStampDebug("AddressGetter path: " + addressGetterPath);
    TimeStampDebug("Dumped IL2CPP path: " + dumpOutPath);
    TimeStampDebug("Plugins path: " + pluginsPath);

    //Create the dumper folder if it doesn't exist
    if (!std::filesystem::exists(dumpOutPath))
    {
        std::filesystem::create_directory(dumpOutPath);
    }

    //Start a process to dump the game assembly
    TimeStampDebug("Dumping game assembly...");

    PROCESS_INFORMATION ProcessInfo; //This is what we get as an [out] parameter
    STARTUPINFO StartupInfo; //This is an [in] parameter
    ZeroMemory(&StartupInfo, sizeof(StartupInfo));
    StartupInfo.cb = sizeof StartupInfo; //Only compulsory field


    TimeStampDebug("Starting dumping process with args: " + command);

    if (CreateProcess(dumperPath.c_str(), const_cast<LPSTR>(command.c_str()),
                      NULL,NULL,FALSE, 0,NULL,
                      NULL, &StartupInfo, &ProcessInfo))
    {
        WaitForSingleObject(ProcessInfo.hProcess, INFINITE);
        CloseHandle(ProcessInfo.hThread);
        CloseHandle(ProcessInfo.hProcess);

        TimeStampDebug("Dumped game assembly!");
    }
    else
    {
        TimeStampDebug("Failed to dump game assembly!");
    }

    SatisfyAllPluginRequests();
    LoadAllPlugins();
}

void ModLoader::SatisfyAllPluginRequests()
{
    if (!std::filesystem::exists(pluginsPath))
    {
        PutDebug("Plugins folder does not exist, creating a new one.");
        _mkdir(pluginsPath.c_str());
    }

    // Go through all json files in the plugins folder and iterate over them using the AddressGetter and a new process
    for (const auto& entry : std::filesystem::directory_iterator(pluginsPath))
    {
        // Check if file ends with .json
        std::filesystem::path extension = entry.path().extension();
        if (extension == ".json")
        {
            // Start a new process to satisfy the plugin request
            std::string jsonFile = entry.path().generic_string();
            TimeStampDebug("Satisfying plugin request: " + jsonFile);

            PROCESS_INFORMATION ProcessInfo; // This is what we get as an [out] parameter
            STARTUPINFO StartupInfo; // This is an [in] parameter
            ZeroMemory(&StartupInfo, sizeof(StartupInfo));

            std::string command = " internal " + jsonFile + " useDefault false";
            // Properly format command line arguments

            TimeStampDebug("Starting address getter process with args: " + command);

            if (CreateProcess(
                addressGetterPath.c_str(),
                const_cast<LPSTR>(command.c_str()),
                NULL,
                NULL,
                FALSE,
                0,
                NULL,
                NULL,
                &StartupInfo,
                &ProcessInfo))
            {
                WaitForSingleObject(ProcessInfo.hProcess, INFINITE);
                CloseHandle(ProcessInfo.hThread);
                CloseHandle(ProcessInfo.hProcess);

                TimeStampDebug("Satisfied request!");
            }
        }
    }
}


void KeyboardHandler::KeyBoardLoop(ModLoader* loaderInstance)
{
    while (true)
    {
        Sleep(1);

        if (GetKeyState(VK_F9) KEYISPRESSED)
        {
            TimeStampDebug("F9 pressed, reloading plugins!");
            loaderInstance->UnloadAllPlugins();
            loaderInstance->LoadAllPlugins();
            TimeStampDebug("Plugins reloaded!");
            Sleep(400);
        }
    }
}
