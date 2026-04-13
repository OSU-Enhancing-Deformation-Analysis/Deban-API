#ifndef ML_COMMAND_H
#define ML_COMMAND_H

#include <string>

struct MLCommand
{
    double recommendedLoad = 0.0;
    double confidence = 0.0;
    bool riskFlag = false;
    std::string rawText;
};

bool readMLCommandFromFile(const std::string& filePath, MLCommand& command, std::string& errorMessage);

#endif