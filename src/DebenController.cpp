#include "DebenController.h"

#include <iostream>
#include <sstream>

#include "deben_api.h"
#include "External control DLL/MTDDE.h"

DebenController::DebenController()
    : hLib(NULL),
      loaded(false),
      connected(false),
      minSafeLoad(0.0),
      maxSafeLoad(25.0)
{
}

DebenController::~DebenController()
{
    disconnectDevice();
    unloadLibrary();
}

bool DebenController::loadLibrary(const wchar_t* dllPath)
{
    if (loaded)
    {
        return true;
    }

    hLib = LoadLibraryW(dllPath);

    if (hLib == NULL)
    {
        DWORD err = GetLastError();
        std::ostringstream oss;
        oss << "Unable to load DLL. Windows error code: " << err;
        setError(oss.str());
        return false;
    }

    if (!loadFunctions())
    {
        unloadLibrary();
        return false;
    }

    loaded = true;
    return true;
}

bool DebenController::loadFunctions()
{
    MT_Connect    = (DLL_MT_NoParams)    GetProcAddress(hLib, "MT_Connect");
    MT_Disconnect = (DLL_MT_NoParams)    GetProcAddress(hLib, "MT_Disconnect");
    MT_StartMotor = (DLL_MT_NoParams)    GetProcAddress(hLib, "MT_StartMotor");
    MT_StopMotor  = (DLL_MT_NoParams)    GetProcAddress(hLib, "MT_StopMotor");
    MT_GetForce   = (DLL_MT_Double)      GetProcAddress(hLib, "MT_GetForce");
    MT_GotoLoad   = (DLL_MT_SetDouble)   GetProcAddress(hLib, "MT_GotoLoad");

    bool failed =
        MT_Connect == NULL ||
        MT_Disconnect == NULL ||
        MT_StartMotor == NULL ||
        MT_StopMotor == NULL ||
        MT_GetForce == NULL ||
        MT_GotoLoad == NULL;

    if (failed)
    {
        setError("Unable to load one or more function pointers from the DLL.");
        return false;
    }

    return true;
}

bool DebenController::connectDevice()
{
    if (!loaded)
    {
        setError("DLL is not loaded.");
        return false;
    }

    if (connected)
    {
        return true;
    }

    MT_Connect();
    MT_StartMotor();
    connected = true;
    return true;
}

void DebenController::disconnectDevice()
{
    if (connected)
    {
        MT_StopMotor();
        MT_Disconnect();
        connected = false;
    }
}

void DebenController::unloadLibrary()
{
    if (hLib != NULL)
    {
        FreeLibrary(hLib);
        hLib = NULL;
    }

    loaded = false;
}

bool DebenController::isLoaded() const
{
    return loaded;
}

bool DebenController::isConnected() const
{
    return connected;
}

double DebenController::getForce() const
{
    if (!connected || MT_GetForce == NULL)
    {
        return 0.0;
    }

    return MT_GetForce();
}

bool DebenController::gotoLoad(double targetLoad)
{
    if (!connected)
    {
        setError("Device is not connected.");
        return false;
    }

    if (!isLoadSafe(targetLoad))
    {
        std::ostringstream oss;
        oss << "Requested load " << targetLoad
            << " N is outside the safe range ["
            << minSafeLoad << ", " << maxSafeLoad << "] N.";
        setError(oss.str());
        return false;
    }

    MT_GotoLoad(targetLoad);
    return true;
}

void DebenController::stopMotor()
{
    if (connected && MT_StopMotor != NULL)
    {
        MT_StopMotor();
    }
}

bool DebenController::isLoadSafe(double targetLoad) const
{
    return targetLoad >= minSafeLoad && targetLoad <= maxSafeLoad;
}

void DebenController::setSafeLoadRange(double minLoad, double maxLoad)
{
    minSafeLoad = minLoad;
    maxSafeLoad = maxLoad;
}

std::string DebenController::getLastErrorMessage() const
{
    return lastErrorMessage;
}

void DebenController::setError(const std::string& message)
{
    lastErrorMessage = message;
}