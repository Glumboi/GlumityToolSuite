#include "ModLoader.h"
#include "debugUtils.h"

using PluginEntryPoint = void(*)();

ModLoader::ModLoader()
{
	char buffer[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, buffer);
	m_workingDirectory = std::string(buffer);

	LoadConfig("./GlummyLoader.cfg");
}

void ModLoader::LaunchModLoaderThread()
{
	if (m_useConsole)
	{
		InitConsole();
		TimeStampDebug("Initializing ModLoader instance...");
		TimeStampDebug("Working Directory: " + m_workingDirectory);
		TimeStampDebug(m_skipASI ? "LoadASI: true" : "LoadASI: false");
	}

	DumpIL2CPPBinary();
	auto hThread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)KeyboardHandler::KeyBoardLoop, this, 0, 0);
	CloseHandle(hThread);
}

void ModLoader::LoadConfig(const std::string& fileName)
{
	//Create a small config file
	if (!std::filesystem::exists(fileName))
	{
		std::ofstream cfgFile(fileName);
		cfgFile << DEFAULT_CONFIG;
		cfgFile.close();
	}

	std::ifstream cfgFileIn(fileName);

	m_skipASI = GetValueFromConfigFile<bool>(cfgFileIn, "LoadASI", true);
	m_useConsole = GetValueFromConfigFile<bool>(cfgFileIn, "UseConsole", true);
	m_reloadKey = GetValueFromConfigFile<int>(cfgFileIn, "ReloadKey", true);
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
		m_loadedPlugins.push_back(hModule);
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
	std::ofstream creationTimeFile(m_assemblyCreationTimeFile);
	creationTimeFile << "m_assemblyCreationTime=" + m_assemblyCreationTime;
	creationTimeFile.close();
}

bool ModLoader::IsGameANewerVersion()
{
	std::ifstream creationFileIn(m_assemblyCreationTimeFile);
	if (creationFileIn.good())
	{
		std::string sLine;
		getline(creationFileIn, sLine);
		size_t lastEq = sLine.find('=') + 1;
		m_storedAssemblyCreationTime = sLine.substr(lastEq, sLine.length() - lastEq);
		TimeStampDebug(
			"Stored creation time: " + m_storedAssemblyCreationTime + " | GameAssembly creation time: " +
			m_assemblyCreationTime);
		return m_storedAssemblyCreationTime != m_assemblyCreationTime;
	}
	creationFileIn.close();
	return true;
}

void ModLoader::LoadAssemblyCreationTime()
{
	WIN32_FILE_ATTRIBUTE_DATA fInfo;
	GetFileAttributesEx(m_gameAsmPath.c_str(), GetFileExInfoStandard, &fInfo);

	m_assemblyCreationTime = std::to_string(
		fInfo.ftCreationTime.dwHighDateTime + fInfo.ftCreationTime.dwLowDateTime);
}

void ModLoader::LoadPluginsFromPath(std::string const& path)
{
	if (!std::filesystem::exists(path) || !std::filesystem::is_directory(path))
	{
		PutDebug(std::format("Invalid path or not a directory: {}", path));
		return;
	}

	for (const auto& entry : std::filesystem::directory_iterator(path))
	{
		std::filesystem::path extension = entry.path().extension();
		if (extension == ".dll")
		{
			LoadPlugin(entry.path().string());
		}
		else if (extension == ".asi" && !m_skipASI)
		{
			LoadPlugin(entry.path().string());
		}
		/*else if (std::filesystem::is_directory(entry))
		{
			PutDebug(std::format("{} is a directory, iterating over it's files...", entry.path().string()));
			LoadPluginsFromPath(entry.path().string());
		}*/
	}
}

void ModLoader::LoadAllPlugins()
{
	//Create a folder called plugins in the working directory if it doesnt exist
	if (!std::filesystem::exists(m_pluginsPath))
	{
		TimeStampDebug("Creating Plugins Directory...");
		std::filesystem::create_directory(m_pluginsPath);
	}

	LoadPluginsFromPath(m_pluginsPath);
}

void ModLoader::UnloadAllPlugins()
{
	for (HMODULE hModule : m_loadedPlugins)
	{
		FreeLibrary(hModule);
	}
	m_loadedPlugins.clear();
}

void ModLoader::DumpIL2CPPBinary()
{
	//search and assign a folder that ends with _Data in the game directory
	for (const auto& entry : std::filesystem::directory_iterator(m_workingDirectory))
	{
		//Get last 5 chars of the path
		std::string path = entry.path().string();
		std::string last5 = path.substr(path.length() - 5, 5);
		if (last5 == "_Data")
		{
			m_appDataPath = path;
			TimeStampDebug("Found Unity game data at: " + path);
			TimeStampDebug("Continuing in unity IL2CPP mode!");
			break;
		}
	}

	if (m_appDataPath.empty())
	{
		TimeStampDebug("Running in universal ASI loader mode!");
	}

	TimeStampDebug("Game path: " + m_workingDirectory);

	m_gameAsmPath = m_workingDirectory + "\\GameAssembly.dll";
	m_assemblyCreationTimeFile = m_workingDirectory + "\\m_assemblyCreationTime";
	m_gameMetadataPath = m_appDataPath + "\\il2cpp_data\\Metadata\\global-metadata.dat";
	m_pluginsPath = m_workingDirectory + "\\Plugins";
	m_dumperPath = m_workingDirectory + "\\dumper\\Il2CppDumper.exe";
	m_addressGetterPath = m_workingDirectory + "\\AddressGetter\\AddressGetter.exe";
	m_dumpOutPath = m_workingDirectory + "\\DumpedIL2CPP";
	m_command = " " + std::string("\"" + m_gameAsmPath + "\"") + " " + std::string("\"" + m_gameMetadataPath + "\"") + " " +
		std::string("\"" + m_dumpOutPath + "\"");

	TimeStampDebug("Assumed Dumper path: " + m_dumperPath);
	TimeStampDebug("Assumed AddressGetter path: " + m_addressGetterPath);
	TimeStampDebug("Assumed Dumped IL2CPP path: " + m_dumpOutPath);
	TimeStampDebug("Assumed Plugins path: " + m_pluginsPath);

	if (!m_appDataPath.empty()) // we are in a unity IL2CPP game, proceed with dumping
	{
		//Create the dumper folder if it doesn't exist
		if (!std::filesystem::exists(m_dumpOutPath))
		{
			std::filesystem::create_directory(m_dumpOutPath);
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

			TimeStampDebug("Starting dumping process with args: " + m_command);

			bool success = CreateProcess(m_dumperPath.c_str(), const_cast<LPSTR>(m_command.c_str()),
				NULL, NULL, FALSE, 0, NULL,
				NULL, &StartupInfo, &ProcessInfo);
			WaitForSingleObject(ProcessInfo.hProcess, INFINITE);
			CloseHandle(ProcessInfo.hThread);
			CloseHandle(ProcessInfo.hProcess);


			if (success)
				TimeStampDebug("Dumped game assembly!");
			else
				TimeStampDebug("Failed to dump game assembly!");

			std::filesystem::remove(m_assemblyCreationTimeFile);
		}

		UpdateCreationTimeFile();
		SatisfyAllPluginRequests();
	}
	LoadAllPlugins();
}

void ModLoader::SatisfyAllPluginRequests()
{
	// Go through all json files in the plugins folder and iterate over them using the AddressGetter and a new process
	for (const auto& entry : std::filesystem::directory_iterator(m_pluginsPath))
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

			std::string m_command = " internal " + jsonFile + " useDefault false";
			// Properly format m_command line arguments

			TimeStampDebug("Starting address getter process with args: " + m_command);

			if (CreateProcess(
				m_addressGetterPath.c_str(),
				const_cast<LPSTR>(m_command.c_str()),
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
	TimeStampDebug("Started Keyboard loop...");

	while (true)
	{
		Sleep(1);

		if (GetKeyState(VK_F9))//ModLoader::m_reloadKey))
		{
			TimeStampDebug("F9 pressed, reloading plugins!");
			loaderInstance->UnloadAllPlugins();
			loaderInstance->LoadAllPlugins();
			TimeStampDebug("Plugins reloaded!");
			Sleep(400);
		}
	}
}
