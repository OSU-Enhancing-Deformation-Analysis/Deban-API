#include "Logger.h"

#include <chrono>
#include <iomanip>
#include <sstream>

Logger::Logger()
    : headerWritten(false)
{
}

Logger::~Logger()
{
    if (logStream.is_open())
    {
        logStream.close();
    }
}

bool Logger::open(const std::string& path)
{
    logStream.open(path, std::ios::app);
    if (!logStream.is_open())
    {
        return false;
    }

    if (!headerWritten)
    {
        logStream << "timestamp,event_type,detail,requested_load,measured_force,status\n";
        headerWritten = true;
    }

    return true;
}

void Logger::logEvent(const std::string& eventType,
                      const std::string& detail,
                      double requestedLoad,
                      double measuredForce,
                      const std::string& status)
{
    if (!logStream.is_open())
    {
        return;
    }

    logStream
        << escapeCsv(currentTimestamp()) << ','
        << escapeCsv(eventType) << ','
        << escapeCsv(detail) << ','
        << requestedLoad << ','
        << measuredForce << ','
        << escapeCsv(status) << '\n';

    logStream.flush();
}

std::string Logger::currentTimestamp() const
{
    auto now = std::chrono::system_clock::now();
    auto timeT = std::chrono::system_clock::to_time_t(now);

    std::tm localTime{};
#ifdef _WIN32
    localtime_s(&localTime, &timeT);
#else
    localtime_r(&timeT, &localTime);
#endif

    std::ostringstream oss;
    oss << std::put_time(&localTime, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

std::string Logger::escapeCsv(const std::string& value) const
{
    std::string escaped = value;
    size_t pos = 0;
    while ((pos = escaped.find('"', pos)) != std::string::npos)
    {
        escaped.insert(pos, 1, '"');
        pos += 2;
    }
    return '"' + escaped + '"';
}