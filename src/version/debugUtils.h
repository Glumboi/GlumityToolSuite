#pragma once
#include <sstream>
#include <iomanip>  // For std::setw
#include <iostream>

#define LOGFILE "GlummyLoaderLog.txt"

#define CLEAR_LOG() if (std::filesystem::exists(LOGFILE)) \
    std::filesystem::remove(LOGFILE);

inline void DebugToFile(const std::string& szInput)
{
	std::ofstream log(LOGFILE, std::ios_base::app | std::ios_base::out);
	log << szInput;
	log << "\n";
}

inline void PutDebug(const std::string& msg)
{
	std::cout << "[GlummyLoader]: " << msg << "\n";

	//Also log to file
	DebugToFile("[GlummyLoader]: " + msg);
}

inline void TimeStampDebug(const std::string& msg)
{
	std::time_t t = std::time(0); // get time now
	std::tm* now = std::localtime(&t);

	std::stringstream timeStampStream;
	timeStampStream << std::setw(2) << std::setfill('0') << now->tm_hour << ':'
		<< std::setw(2) << std::setfill('0') << now->tm_min << ':'
		<< std::setw(2) << std::setfill('0') << now->tm_sec;

	std::string timeStamp = timeStampStream.str();
	PutDebug(timeStamp + " -> " + msg);
}

inline void InitConsole()
{
	AllocConsole();
	FILE* f;
	freopen_s(&f, "CONOUT$", "w", stdout);
	PutDebug("Console initialized");
	SetConsoleTitle("GlummyLoader");
}
