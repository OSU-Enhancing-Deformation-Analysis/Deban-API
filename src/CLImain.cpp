#include <iostream>

#include "Config.h"
#include "DebenController.h"
#include "Logger.h"
#include "Menu.h"

int main()
{
    AppConfig config;
    std::string configError;

    if (!loadConfig("config.txt", config, configError))
    {
        std::cout << "Config warning: " << configError << "\n";
        std::cout << "Using built in defaults.\n";
    }

    Logger logger;
    if (!logger.open(config.logFile))
    {
        std::cout << "Warning: could not open log file: " << config.logFile << "\n";
    }

    DebenController controller;
    controller.setSafeLoadRange(config.minSafeLoad, config.maxSafeLoad);
    controller.setMaxStepDelta(config.maxStepDelta);

    std::wcout << L"Attempting to load DLL from: " << config.dllPath << L"\n";

    if (!controller.loadLibrary(config.dllPath.c_str()))
    {
        std::cout << controller.getLastErrorMessage() << "\n";
        logger.logEvent("startup", controller.getLastErrorMessage(), 0.0, -9999.0, "failure");
        return 1;
    }

    logger.logEvent("startup", "DLL loaded successfully", 0.0, -9999.0, "success");

    if (!controller.connectDevice())
    {
        std::cout << controller.getLastErrorMessage() << "\n";
        logger.logEvent("connect", controller.getLastErrorMessage(), 0.0, -9999.0, "failure");
        return 1;
    }

    std::cout << "Connected to Deben controller.\n";
    logger.logEvent("connect", "Connected to Deben controller", 0.0, controller.getForce(), "success");

    Menu menu(controller, config, logger);
    menu.run();

    controller.disconnectDevice();
    return 0;
}