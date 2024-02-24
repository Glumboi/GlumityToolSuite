#pragma once

inline void PutDebug(std::string msg)
{
    std::cout << MODNAME << ": " << msg << std::endl;
}

inline void DebugToFile(std::string szInput)
{
    std::ofstream log("spartacus-proxy-version.log", std::ios_base::app | std::ios_base::out);
    log << szInput;
    log << "\n";
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
}
