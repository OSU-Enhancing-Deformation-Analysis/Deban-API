#include "MLCommand.h"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <string>

namespace
{
    std::string trim(const std::string& s)
    {
        size_t start = 0;
        while (start < s.size() && std::isspace(static_cast<unsigned char>(s[start])))
        {
            ++start;
        }

        size_t end = s.size();
        while (end > start && std::isspace(static_cast<unsigned char>(s[end - 1])))
        {
            --end;
        }

        return s.substr(start, end - start);
    }

    bool parseBoolValue(const std::string& value, bool& out)
    {
        std::string lower = value;
        std::transform(lower.begin(), lower.end(), lower.begin(),
            [](unsigned char c) { return static_cast<char>(std::tolower(c)); });

        if (lower == "true" || lower == "1" || lower == "yes")
        {
            out = true;
            return true;
        }

        if (lower == "false" || lower == "0" || lower == "no")
        {
            out = false;
            return true;
        }

        return false;
    }
}

bool readMLCommandFromFile(const std::string& filePath, MLCommand& command, std::string& errorMessage)
{
    std::ifstream file(filePath);
    if (!file.is_open())
    {
        errorMessage = "Could not open ML command file: " + filePath;
        return false;
    }

    std::string line;
    bool sawLoad = false;
    bool sawConfidence = false;
    bool sawRisk = false;

    while (std::getline(file, line))
    {
        command.rawText += line + "\n";
        line = trim(line);

        if (line.empty() || line[0] == '#')
        {
            continue;
        }

        size_t eqPos = line.find('=');
        if (eqPos == std::string::npos)
        {
            continue;
        }

        std::string key = trim(line.substr(0, eqPos));
        std::string value = trim(line.substr(eqPos + 1));

        try
        {
            if (key == "recommended_load")
            {
                command.recommendedLoad = std::stod(value);
                sawLoad = true;
            }
            else if (key == "confidence")
            {
                command.confidence = std::stod(value);
                sawConfidence = true;
            }
            else if (key == "risk_flag")
            {
                bool parsed = false;
                if (!parseBoolValue(value, parsed))
                {
                    errorMessage = "Invalid risk_flag value.";
                    return false;
                }
                command.riskFlag = parsed;
                sawRisk = true;
            }
        }
        catch (const std::exception&)
        {
            errorMessage = "Invalid value in ML command file.";
            return false;
        }
    }

    if (!sawLoad || !sawConfidence || !sawRisk)
    {
        errorMessage = "ML command file must contain recommended_load, confidence, and risk_flag.";
        return false;
    }

    if (command.confidence < 0.0 || command.confidence > 1.0)
    {
        errorMessage = "ML confidence must be between 0.0 and 1.0.";
        return false;
    }

    return true;
}