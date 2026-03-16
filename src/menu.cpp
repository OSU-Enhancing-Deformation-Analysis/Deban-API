#include "Menu.h"

#include <iostream>
#include <limits>
#include <vector>

#include <windows.h>

Menu::Menu(DebenController& controllerRef)
    : controller(controllerRef)
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
                readForce();
                break;

            case 5:
                emergencyStop();
                break;

            case 6:
                running = false;
                break;

            default:
                std::cout << "Invalid option. Please choose 1 through 6.\n";
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
    std::cout << "4. Read current force\n";
    std::cout << "5. Emergency stop\n";
    std::cout << "6. Exit\n";
}

void Menu::manualMode()
{
    std::cout << "\nManual mode\n";

    double targetLoad = promptDouble("Enter target load in N: ");
    int waitMs = promptInt("Enter wait time in ms: ");

    if (!controller.gotoLoad(targetLoad))
    {
        std::cout << controller.getLastErrorMessage() << "\n";
        return;
    }

    Sleep(waitMs);
    std::cout << "Command sent.\n";
    std::cout << "Current force: " << controller.getForce() << " N\n";
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
        return;
    }

    if (!controller.gotoLoad(targetLoad))
    {
        std::cout << controller.getLastErrorMessage() << "\n";
        return;
    }

    Sleep(waitMs);
    std::cout << "Command sent.\n";
    std::cout << "Current force: " << controller.getForce() << " N\n";
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
        return;
    }

    for (std::size_t i = 0; i < sequence.size(); ++i)
    {
        std::cout << "\nStep " << i + 1 << " of " << sequence.size() << "\n";

        if (!controller.gotoLoad(sequence[i]))
        {
            std::cout << controller.getLastErrorMessage() << "\n";
            return;
        }

        Sleep(waitMs);
        std::cout << "Measured force: " << controller.getForce() << " N\n";

        if (i + 1 < sequence.size())
        {
            if (!promptYesNo("Continue to next step"))
            {
                std::cout << "Automatic sequence stopped by user.\n";
                return;
            }
        }
    }

    std::cout << "Automatic sequence completed.\n";
}

void Menu::readForce()
{
    std::cout << "Current force: " << controller.getForce() << " N\n";
}

void Menu::emergencyStop()
{
    controller.stopMotor();
    std::cout << "Emergency stop command sent.\n";
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