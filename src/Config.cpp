#include "Config.h"

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

    bool parseBool(const std::string& value, bool& out)
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


bool loadConfig(const std::string& path, AppConfig& config, std::string& errorMessage)
{
    std::ifstream file(path);
    if (!file.is_open())
    {
        errorMessage = "Could not open config file: " + path;
        return false;
    }

    std::string line;
    while (std::getline(file, line))
    {
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
            if (key == "dll_path")
            {
                config.dllPath = std::wstring(value.begin(), value.end());
            }
            else if (key == "ml_command_file")
            {
                config.mlCommandFile = value;
            }
            else if (key == "log_file")
            {
                config.logFile = value;
            }
            else if (key == "min_safe_load")
            {
                config.minSafeLoad = std::stod(value);
            }
            else if (key == "max_safe_load")
            {
                config.maxSafeLoad = std::stod(value);
            }
            else if (key == "max_step_delta")
            {
                config.maxStepDelta = std::stod(value);
            }
            else if (key == "emergency_stop_force")
            {
                config.emergencyStopForce = std::stod(value);
            }
            else if (key == "default_wait_ms")
            {
                config.defaultWaitMs = std::stoi(value);
            }
            else if (key == "require_operator_approval")
            {
                bool parsed = false;
                if (!parseBool(value, parsed))
                {
                    errorMessage = "Invalid boolean value for require_operator_approval.";
                    return false;
                }
                config.requireOperatorApproval = parsed;
            }
            else if (key == "minimum_ml_confidence")
            {
                config.minimumMLConfidence = std::stod(value);
            }
        }
        catch (const std::exception&)
        {
            errorMessage = "Invalid config value for key: " + key;
            return false;
        }
    }

    if (config.minSafeLoad > config.maxSafeLoad)
    {
        errorMessage = "min_safe_load cannot be greater than max_safe_load.";
        return false;
    }

    if (config.maxStepDelta <= 0.0)
    {
        errorMessage = "max_step_delta must be positive.";
        return false;
    }

    if (config.defaultWaitMs < 0)
    {
        errorMessage = "default_wait_ms cannot be negative.";
        return false;
    }

    if (config.minimumMLConfidence < 0.0 || config.minimumMLConfidence > 1.0)
    {
        errorMessage = "minimum_ml_confidence must be in [0.0, 1.0].";
        return false;
    }

    return true;
}