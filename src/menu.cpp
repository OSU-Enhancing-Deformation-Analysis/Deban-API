#include "Menu.h"

#include <iostream>
#include <limits>
#include <vector>

#include <windows.h>

Menu::Menu(DebenController& controllerRef, AppConfig& configRef, Logger& loggerRef)
    : controller(controllerRef),
      config(configRef),
      logger(loggerRef)
{
}

void Menu::run()
{
    bool running = true;

    while (running)
    {
        showMenu();
        int choice = promptInt("Select an option: ");

        switch (choice)
        {
            case 1:
                manualMode();
                break;

            case 2:
                semiAutomaticMode();
                break;

            case 3:
                automaticMode();
                break;

            case 4:
                mlDrivenMode();
                break;

            case 5:
                readForce();
                break;

            case 6:
                emergencyStop();
                break;

            case 7:
                running = false;
                logger.logEvent("shutdown", "Operator exited menu", controller.getLastCommandedLoad(), controller.getForce(), "success");
                break;

            default:
                std::cout << "Invalid option. Please choose 1 through 7.\n";
                break;
        }
    }
}

void Menu::showMenu() const
{
    std::cout << "\n===== Deben Control Menu =====\n";
    std::cout << "1. Manual mode\n";
    std::cout << "2. Semi automatic mode\n";
    std::cout << "3. Automatic mode\n";
    std::cout << "4. Execute ML command from file\n";
    std::cout << "5. Read current force\n";
    std::cout << "6. Emergency stop\n";
    std::cout << "7. Exit\n";
}

void Menu::manualMode()
{
    std::cout << "\nManual mode\n";

    double targetLoad = promptDouble("Enter target load in N: ");
    int waitMs = promptInt("Enter wait time in ms: ");

    if (!controller.gotoLoad(targetLoad))
    {
        std::cout << controller.getLastErrorMessage() << "\n";
        logger.logEvent("manual_mode", controller.getLastErrorMessage(), targetLoad, controller.getForce(), "rejected");
        return;
    }

    Sleep(waitMs);
    logAndCheckForce("manual_mode", "Manual gotoLoad executed", targetLoad);
}

void Menu::semiAutomaticMode()
{
    std::cout << "\nSemi automatic mode\n";

    double targetLoad = promptDouble("Enter target load in N: ");
    int waitMs = promptInt("Enter wait time in ms: ");

    std::cout << "\nProposed command:\n";
    std::cout << "Target load: " << targetLoad << " N\n";
    std::cout << "Wait time: " << waitMs << " ms\n";

    if (!promptYesNo("Proceed"))
    {
        std::cout << "Operation cancelled.\n";
        logger.logEvent("semi_automatic_mode", "Operator cancelled command", targetLoad, controller.getForce(), "cancelled");
        return;
    }

    if (!controller.gotoLoad(targetLoad))
    {
        std::cout << controller.getLastErrorMessage() << "\n";
        logger.logEvent("semi_automatic_mode", controller.getLastErrorMessage(), targetLoad, controller.getForce(), "rejected");
        return;
    }

    Sleep(waitMs);
    logAndCheckForce("semi_automatic_mode", "Semi automatic gotoLoad executed", targetLoad);
}

void Menu::automaticMode()
{
    std::cout << "\nAutomatic mode\n";

    double startLoad = promptDouble("Enter start load in N: ");
    double endLoad = promptDouble("Enter end load in N: ");
    double stepLoad = promptDouble("Enter step size in N: ");
    int waitMs = promptInt("Enter wait time in ms between steps: ");

    if (stepLoad <= 0.0)
    {
        std::cout << "Step size must be positive.\n";
        logger.logEvent("automatic_mode", "Invalid non-positive step size", stepLoad, controller.getForce(), "failure");
        return;
    }

    std::vector<double> sequence;

    if (startLoad <= endLoad)
    {
        for (double load = startLoad; load <= endLoad; load += stepLoad)
        {
            sequence.push_back(load);
        }
    }
    else
    {
        for (double load = startLoad; load >= endLoad; load -= stepLoad)
        {
            sequence.push_back(load);
        }
    }

    std::cout << "\nGenerated sequence:\n";
    for (std::size_t i = 0; i < sequence.size(); ++i)
    {
        std::cout << i + 1 << ". " << sequence[i] << " N\n";
    }

    if (!promptYesNo("Start automatic sequence"))
    {
        std::cout << "Automatic mode cancelled.\n";
        logger.logEvent("automatic_mode", "Operator cancelled automatic sequence", 0.0, controller.getForce(), "cancelled");
        return;
    }

    for (std::size_t i = 0; i < sequence.size(); ++i)
    {
        std::cout << "\nStep " << i + 1 << " of " << sequence.size() << "\n";

        if (!controller.gotoLoad(sequence[i]))
        {
            std::cout << controller.getLastErrorMessage() << "\n";
            logger.logEvent("automatic_mode", controller.getLastErrorMessage(), sequence[i], controller.getForce(), "rejected");
            return;
        }

        Sleep(waitMs);

        double measuredForce = controller.getForce();
        logger.logEvent("automatic_mode", "Automatic step executed", sequence[i], measuredForce, "success");
        std::cout << "Measured force: " << measuredForce << " N\n";

        if (controller.emergencyStopIfNeeded(measuredForce, config.emergencyStopForce))
        {
            std::cout << controller.getLastErrorMessage() << "\n";
            logger.logEvent("automatic_mode", controller.getLastErrorMessage(), sequence[i], measuredForce, "emergency_stop");
            return;
        }

        if (i + 1 < sequence.size())
        {
            if (!promptYesNo("Continue to next step"))
            {
                std::cout << "Automatic sequence stopped by user.\n";
                logger.logEvent("automatic_mode", "Operator stopped automatic sequence", sequence[i], measuredForce, "stopped");
                return;
            }
        }
    }

    std::cout << "Automatic sequence completed.\n";
    logger.logEvent("automatic_mode", "Automatic sequence completed", controller.getLastCommandedLoad(), controller.getForce(), "success");
}

void Menu::mlDrivenMode()
{
    std::cout << "\nML driven mode\n";

    MLCommand command;
    std::string errorMessage;

    if (!readMLCommandFromFile(config.mlCommandFile, command, errorMessage))
    {
        std::cout << errorMessage << "\n";
        logger.logEvent("ml_mode", errorMessage, 0.0, controller.getForce(), "failure");
        return;
    }

    std::cout << "Recommended load: " << command.recommendedLoad << " N\n";
    std::cout << "Confidence: " << command.confidence << "\n";
    std::cout << "Risk flag: " << (command.riskFlag ? "true" : "false") << "\n";

    logger.logEvent("ml_mode", "Parsed ML command file", command.recommendedLoad, controller.getForce(), "parsed");

    if (command.riskFlag)
    {
        std::cout << "ML command rejected because risk_flag is true.\n";
        logger.logEvent("ml_mode", "Rejected ML command because risk_flag=true", command.recommendedLoad, controller.getForce(), "rejected");
        return;
    }

    if (command.confidence < config.minimumMLConfidence)
    {
        std::cout << "ML command rejected because confidence is below threshold.\n";
        logger.logEvent("ml_mode", "Rejected ML command because confidence below threshold", command.recommendedLoad, controller.getForce(), "rejected");
        return;
    }

    if (config.requireOperatorApproval)
    {
        if (!promptYesNo("Approve ML command"))
        {
            std::cout << "ML command cancelled by operator.\n";
            logger.logEvent("ml_mode", "Operator declined ML command", command.recommendedLoad, controller.getForce(), "cancelled");
            return;
        }
    }

    if (!controller.gotoLoad(command.recommendedLoad))
    {
        std::cout << controller.getLastErrorMessage() << "\n";
        logger.logEvent("ml_mode", controller.getLastErrorMessage(), command.recommendedLoad, controller.getForce(), "rejected");
        return;
    }

    Sleep(config.defaultWaitMs);
    logAndCheckForce("ml_mode", "ML-driven gotoLoad executed", command.recommendedLoad);
}
void Menu::readForce()
{
    double measuredForce = controller.getForce();
    std::cout << "Current force: " << measuredForce << " N\n";
    logger.logEvent("read_force", "Read current force", controller.getLastCommandedLoad(), measuredForce, "success");
}

void Menu::emergencyStop()
{
    controller.stopMotor();
    double measuredForce = controller.getForce();
    std::cout << "Emergency stop command sent.\n";
    logger.logEvent("emergency_stop", "Operator requested emergency stop", controller.getLastCommandedLoad(), measuredForce, "success");
}

int Menu::promptInt(const char* prompt) const
{
    int value = 0;

    while (true)
    {
        std::cout << prompt;
        std::cin >> value;

        if (!std::cin.fail())
        {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return value;
        }

        std::cout << "Invalid integer input.\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}

double Menu::promptDouble(const char* prompt) const
{
    double value = 0.0;

    while (true)
    {
        std::cout << prompt;
        std::cin >> value;

        if (!std::cin.fail())
        {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return value;
        }

        std::cout << "Invalid numeric input.\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}

bool Menu::promptYesNo(const char* prompt) const
{
    char choice = '\0';

    while (true)
    {
        std::cout << prompt << " (y/n): ";
        std::cin >> choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if (choice == 'y' || choice == 'Y')
        {
            return true;
        }

        if (choice == 'n' || choice == 'N')
        {
            return false;
        }

        std::cout << "Please enter y or n.\n";
    }
}

void Menu::logAndCheckForce(const std::string& eventType,
                            const std::string& detail,
                            double requestedLoad)
{
    double measuredForce = controller.getForce();
    logger.logEvent(eventType, detail, requestedLoad, measuredForce, "success");

    std::cout << "Command sent.\n";
    std::cout << "Current force: " << measuredForce << " N\n";

    if (controller.emergencyStopIfNeeded(measuredForce, config.emergencyStopForce))
    {
        std::cout << controller.getLastErrorMessage() << "\n";
        logger.logEvent(eventType, controller.getLastErrorMessage(), requestedLoad, measuredForce, "emergency_stop");
    }
}