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

    void setSafeLoadRange(double minLoad, double maxLoad);
    void setMaxStepDelta(double maxDelta);

    bool validateTargetLoad(double targetLoad, std::string& validationError) const;
    bool emergencyStopIfNeeded(double measuredForce, double emergencyThreshold);

    double getLastCommandedLoad() const;
    std::string getLastErrorMessage() const;

private:
    HMODULE hLib;
    bool loaded;
    bool connected;

    double minSafeLoad;
    double maxSafeLoad;
    double maxStepDelta;
    double lastCommandedLoad;

    std::string lastErrorMessage;

    bool loadFunctions();
    void unloadLibrary();
    void setError(const std::string& message);

    bool isLoadSafe(double targetLoad) const;
    bool isStepDeltaSafe(double targetLoad) const;
};

#endif