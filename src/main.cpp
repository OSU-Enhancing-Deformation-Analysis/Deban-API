#include <iostream>

#include "DebenController.h"
#include "Menu.h"

int main()
{
    std::cout << "Attempting to load DLL...\n";

    DebenController controller;
    controller.setSafeLoadRange(0.0, 25.0);

    if (!controller.loadLibrary(L"External control DLL/DebenMT64.dll"))
    {
        std::cout << controller.getLastErrorMessage() << "\n";
        return 1;
    }

    std::cout << "Library loaded successfully.\n";

    if (!controller.connectDevice())
    {
        std::cout << controller.getLastErrorMessage() << "\n";
        return 1;
    }

    std::cout << "Connected to Deben controller.\n";
    std::cout << "Initial force: " << controller.getForce() << " N\n";

    Menu menu(controller);
    menu.run();

    std::cout << "Shutting down cleanly.\n";
    return 0;
}