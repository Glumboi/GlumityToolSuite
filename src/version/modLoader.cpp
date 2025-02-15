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

    LoadConfig("./GlummyLoader.cfg");
    if (useConsole)
    {
        InitConsole();
        TimeStampDebug("Initializing ModLoader instance...");
        TimeStampDebug("Working Directory: " + workingDirectory);
        TimeStampDebug(skipASI ? "LoadASI: true" : "LoadASI: false");
    }

    DumpIL2CPPBinary();
    CreateThread(0, 0, (LPTHREAD_START_ROUTINE)KeyboardHandler::KeyBoardLoop, this, 0, 0);
}

void ModLoader::LoadConfig(const std::string& fileName)
{
    //Create a small config file
    if (!std::filesystem::exists(fileName))
    {
        std::ofstream cfgFile(fileName);
        cfgFile << "LoadASI=0\n";
        cfgFile << "UseConsole=1\n";
        cfgFile.close();
    }

    std::ifstream cfgFileIn(fileName);

    skipASI = GetValueFromConfigFile<bool>(cfgFileIn, "LoadASI", true);
    useConsole = GetValueFromConfigFile<bool>(cfgFileIn, "UseConsole", true);
    cfgFileIn.close();
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
        loadedPlugins.push_back(hModule);
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

void ModLoader::UpdateCreationTimeFile()
{
    std::ofstream creationTimeFile(assemblyCreationTimeFile);
    creationTimeFile << "assemblyCreationTime=" + assemblyCreationTime;
    creationTimeFile.close();
}

bool ModLoader::IsGameANewerVersion()
{
    std::ifstream creationFileIn(assemblyCreationTimeFile);
    if (creationFileIn.good())
    {
        std::string sLine;
        getline(creationFileIn, sLine);
        size_t lastEq = sLine.find('=') + 1;
        storedAssemblyCreationTime = sLine.substr(lastEq, sLine.length() - lastEq);
        TimeStampDebug(
            "Stored creation time: " + storedAssemblyCreationTime + " | GameAssembly creation time: " +
            assemblyCreationTime);
        return storedAssemblyCreationTime != assemblyCreationTime;
    }
    creationFileIn.close();
    return true;
}

void ModLoader::LoadAssemblyCreationTime()
{
    WIN32_FILE_ATTRIBUTE_DATA fInfo;
    GetFileAttributesEx(gameAsmPath.c_str(), GetFileExInfoStandard, &fInfo);

    assemblyCreationTime = std::to_string(
        fInfo.ftCreationTime.dwHighDateTime + fInfo.ftCreationTime.dwLowDateTime);
}

void ModLoader::LoadAllPlugins()
{
    //Create a folder called plugins in the working directory if it doesnt exist
    if (!std::filesystem::exists(pluginsPath))
    {
        TimeStampDebug("Creating Plugins Directory...");
        std::filesystem::create_directory(pluginsPath);
    }

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
        if (last5 == "_Data")
        {
            appDataPath = path;
            TimeStampDebug("Found Unity game data at: " + path);
            TimeStampDebug("Continuing in unity IL2CPP mode!");
            break;
        }
    }

    if (appDataPath.empty())
    {
        TimeStampDebug("Running in universal ASI loader mode!");
    }

    TimeStampDebug("Game path: " + workingDirectory);

    gameAsmPath = workingDirectory + "\\GameAssembly.dll";
    assemblyCreationTimeFile = workingDirectory + "\\AssemblyCreationTime";
    gameMetadataPath = appDataPath + "\\il2cpp_data\\Metadata\\global-metadata.dat";
    pluginsPath = workingDirectory + "\\Plugins";
    dumperPath = workingDirectory + "\\dumper\\Il2CppDumper.exe";
    addressGetterPath = workingDirectory + "\\AddressGetter\\AddressGetter.exe";
    dumpOutPath = workingDirectory + "\\DumpedIL2CPP";
    command = " " + std::string("\"" + gameAsmPath + "\"") + " " + std::string("\"" + gameMetadataPath + "\"") + " " +
        std::string("\"" + dumpOutPath + "\"");

    TimeStampDebug("Assumed Dumper path: " + dumperPath);
    TimeStampDebug("Assumed AddressGetter path: " + addressGetterPath);
    TimeStampDebug("Assumed Dumped IL2CPP path: " + dumpOutPath);
    TimeStampDebug("Assumed Plugins path: " + pluginsPath);

    if (!appDataPath.empty()) // we are in a unity IL2CPP game, proceed with dumping
    {
        //Create the dumper folder if it doesn't exist
        if (!std::filesystem::exists(dumpOutPath))
        {
            std::filesystem::create_directory(dumpOutPath);
        }

        LoadAssemblyCreationTime();

        if (IsGameANewerVersion())
        {
            //Start a process to dump the game assembly
            TimeStampDebug("New game version detected, dumping game assembly...");

            PROCESS_INFORMATION ProcessInfo; //This is what we get as an [out] parameter
            STARTUPINFO StartupInfo; //This is an [in] parameter
            ZeroMemory(&StartupInfo, sizeof(StartupInfo));
            StartupInfo.cb = sizeof StartupInfo; //Only compulsory field

            TimeStampDebug("Starting dumping process with args: " + command);

            bool success = CreateProcess(dumperPath.c_str(), const_cast<LPSTR>(command.c_str()),
                                         NULL,NULL,FALSE, 0,NULL,
                                         NULL, &StartupInfo, &ProcessInfo);
            WaitForSingleObject(ProcessInfo.hProcess, INFINITE);
            CloseHandle(ProcessInfo.hThread);
            CloseHandle(ProcessInfo.hProcess);


            if (success)
                TimeStampDebug("Dumped game assembly!");
            else
                TimeStampDebug("Failed to dump game assembly!");

            std::filesystem::remove(assemblyCreationTimeFile);
        }

        UpdateCreationTimeFile();
        SatisfyAllPluginRequests();
    }
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
            std::string jsonFile = std::string("\"" + entry.path().generic_string() + "\"");
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

        if (GetKeyState(ModLoader::reloadKey) KEYISPRESSED)
        {
            TimeStampDebug("F9 pressed, reloading plugins!");
            loaderInstance->UnloadAllPlugins();
            loaderInstance->LoadAllPlugins();
            TimeStampDebug("Plugins reloaded!");
            Sleep(400);
        }
    }
}
