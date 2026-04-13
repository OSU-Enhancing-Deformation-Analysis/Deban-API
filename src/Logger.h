#ifndef LOGGER_H
#define LOGGER_H

#include <fstream>
#include <string>

class Logger
{
public:
    Logger();
    ~Logger();

    bool open(const std::string& path);
    void logEvent(const std::string& eventType,
                  const std::string& detail,
                  double requestedLoad,
                  double measuredForce,
                  const std::string& status);

private:
    std::ofstream logStream;
    bool headerWritten;

    std::string currentTimestamp() const;
    std::string escapeCsv(const std::string& value) const;
};

#endif