#ifndef DEBEN_CONTROLLER_H
#define DEBEN_CONTROLLER_H

#include <string>
#include <windows.h>

class DebenController
{
public:
    DebenController();
    ~DebenController();

    bool loadLibrary(const wchar_t* dllPath);
    bool connectDevice();
    void disconnectDevice();

    bool isLoaded() const;
    bool isConnected() const;

    double getForce() const;
    bool gotoLoad(double targetLoad);
    void stopMotor();

    bool isLoadSafe(double targetLoad) const;
    void setSafeLoadRange(double minLoad, double maxLoad);

    std::string getLastErrorMessage() const;

private:
    HMODULE hLib;
    bool loaded;
    bool connected;
    double minSafeLoad;
    double maxSafeLoad;
    std::string lastErrorMessage;

    bool loadFunctions();
    void unloadLibrary();
    void setError(const std::string& message);
};

#endif