#ifndef CONFIG_H
#define CONFIG_H

#include <string>

struct AppConfig
{
    std::wstring dllPath = L"External control DLL/DebenMT64.dll";
    std::string mlCommandFile = "data.txt";
    std::string logFile = "deban_api_log.csv";

    double minSafeLoad = 0.0;
    double maxSafeLoad = 25.0;
    double maxStepDelta = 5.0;
    double emergencyStopForce = 27.0;
    int defaultWaitMs = 1000;
    bool requireOperatorApproval = true;
    double minimumMLConfidence = 0.75;
};

bool loadConfig(const std::string& path, AppConfig& config, std::string& errorMessage);

#endif