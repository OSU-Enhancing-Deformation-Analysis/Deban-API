#ifndef MENU_H
#define MENU_H

#include "DebenController.h"

class Menu
{
public:
    explicit Menu(DebenController& controller);
    void run();

private:
    DebenController& controller;

    void showMenu() const;

    void manualMode();
    void semiAutomaticMode();
    void automaticMode();
    void readForce();
    void emergencyStop();

    int promptInt(const char* prompt) const;
    double promptDouble(const char* prompt) const;
    bool promptYesNo(const char* prompt) const;
};

#endif