#ifndef MENU_H
#define MENU_H

#include "Config.h"
#include "DebenController.h"
#include "Logger.h"
#include "MLCommand.h"

class Menu
{
public:
    Menu(DebenController& controller, AppConfig& config, Logger& logger);
    void run();

private:
    DebenController& controller;
    AppConfig& config;
    Logger& logger;

    void showMenu() const;

    void manualMode();
    void semiAutomaticMode();
    void automaticMode();
    void mlDrivenMode();
    void readForce();
    void emergencyStop();

    int promptInt(const char* prompt) const;
    double promptDouble(const char* prompt) const;
    bool promptYesNo(const char* prompt) const;

    void logAndCheckForce(const std::string& eventType,
                          const std::string& detail,
                          double requestedLoad);
};

#endif