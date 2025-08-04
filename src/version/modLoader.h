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
	static HANDLE m_gameHandle;
	inline static BYTE m_reloadKey = VK_F9;
	
	ModLoader();
	void LaunchModLoaderThread();

	void LoadConfig(const std::string& fileName);
	
	void LoadAllPlugins();
	void UnloadAllPlugins();
	
	void DumpIL2CPPBinary();
	
	void SatisfyAllPluginRequests();

private:
	inline static std::string const DEFAULT_CONFIG = 
		"LoadASI=0\nUseConsole=1\nReloadKey=120";

	std::string m_workingDirectory;
	std::string m_pluginsPath;
	std::string m_appDataPath;
	std::string m_gameAsmPath;
	std::string m_assemblyCreationTimeFile;
	std::string m_gameMetadataPath;
	std::string m_dumperPath;
	std::string m_addressGetterPath;
	std::string m_command;
	std::string m_dumpOutPath;
	std::string m_assemblyCreationTime;
	std::string m_storedAssemblyCreationTime;

	std::vector<HMODULE> m_loadedPlugins;

	bool m_skipASI = true;
	bool m_useConsole = true;

	void LoadPluginsFromPath(std::string const& path);
	void LoadPlugin(const std::string& path);

	void UpdateCreationTimeFile();
	bool IsGameANewerVersion();
	void LoadAssemblyCreationTime();

	template <typename T>
	static T GetValueFromConfigFile(std::ifstream& cfgFileIn, const std::string& key, bool isBoolOrInt)
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
