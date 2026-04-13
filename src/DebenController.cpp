#include "DebenController.h"

#include <cmath>
#include <sstream>

#include "External control DLL/MTDDE.h"

DebenController::DebenController()
    : hLib(NULL),
      loaded(false),
      connected(false),
      minSafeLoad(0.0),
      maxSafeLoad(25.0),
      maxStepDelta(5.0),
      lastCommandedLoad(0.0)
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
        DWORD errorCode = GetLastError();
        std::ostringstream oss;
        oss << "Failed to load Deben DLL. Windows error code: " << errorCode;
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
    MT_Connect = reinterpret_cast<DLL_MT_NoParams>(GetProcAddress(hLib, "MT_Connect"));
    MT_Disconnect = reinterpret_cast<DLL_MT_NoParams>(GetProcAddress(hLib, "MT_Disconnect"));
    MT_StartMotor = reinterpret_cast<DLL_MT_NoParams>(GetProcAddress(hLib, "MT_StartMotor"));
    MT_StopMotor = reinterpret_cast<DLL_MT_NoParams>(GetProcAddress(hLib, "MT_StopMotor"));
    MT_GetForce = reinterpret_cast<DLL_MT_Double>(GetProcAddress(hLib, "MT_GetForce"));
    MT_GotoLoad = reinterpret_cast<DLL_MT_SetDouble>(GetProcAddress(hLib, "MT_GotoLoad"));
    MT_IsConnected = reinterpret_cast<DLL_MT_Bool>(GetProcAddress(hLib, "MT_IsConnected"));
    MT_GetErrorStr = reinterpret_cast<DLL_MT_Err_Str>(GetProcAddress(hLib, "MT_GetErrorStr"));

    if (MT_Connect == NULL ||
        MT_Disconnect == NULL ||
        MT_StartMotor == NULL ||
        MT_StopMotor == NULL ||
        MT_GetForce == NULL ||
        MT_GotoLoad == NULL)
    {
        setError("Failed to load one or more required DLL function pointers.");
        return false;
    }

    return true;
}

bool DebenController::connectDevice()
{
    if (!loaded)
    {
        setError("Cannot connect because the DLL is not loaded.");
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
        if (MT_StopMotor != NULL)
        {
            MT_StopMotor();
        }

        if (MT_Disconnect != NULL)
        {
            MT_Disconnect();
        }

        connected = false;
    }
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
        return -9999.0;
    }

    return MT_GetForce();
}

bool DebenController::gotoLoad(double targetLoad)
{
    std::string validationError;
    if (!validateTargetLoad(targetLoad, validationError))
    {
        const_cast<DebenController*>(this)->setError(validationError);
        return false;
    }

    MT_GotoLoad(targetLoad);
    lastCommandedLoad = targetLoad;
    return true;
}

void DebenController::stopMotor()
{
    if (connected && MT_StopMotor != NULL)
    {
        MT_StopMotor();
    }
}

void DebenController::setSafeLoadRange(double minLoad, double maxLoad)
{
    minSafeLoad = minLoad;
    maxSafeLoad = maxLoad;
}

void DebenController::setMaxStepDelta(double maxDelta)
{
    maxStepDelta = maxDelta;
}

bool DebenController::validateTargetLoad(double targetLoad, std::string& validationError) const
{
    if (!connected)
    {
        validationError = "Device is not connected.";
        return false;
    }

    if (!std::isfinite(targetLoad))
    {
        validationError = "Target load is not a finite numeric value.";
        return false;
    }

    if (!isLoadSafe(targetLoad))
    {
        std::ostringstream oss;
        oss << "Target load " << targetLoad << " N is outside safe range ["
            << minSafeLoad << ", " << maxSafeLoad << "] N.";
        validationError = oss.str();
        return false;
    }

    if (!isStepDeltaSafe(targetLoad))
    {
        std::ostringstream oss;
        oss << "Requested jump from " << lastCommandedLoad << " N to "
            << targetLoad << " N exceeds max step delta of " << maxStepDelta << " N.";
        validationError = oss.str();
        return false;
    }

    return true;
}

bool DebenController::emergencyStopIfNeeded(double measuredForce, double emergencyThreshold)
{
    if (measuredForce > emergencyThreshold)
    {
        stopMotor();

        std::ostringstream oss;
        oss << "Emergency stop triggered: measured force " << measuredForce
            << " N exceeded threshold " << emergencyThreshold << " N.";
        setError(oss.str());
        return true;
    }

    return false;
}

double DebenController::getLastCommandedLoad() const
{
    return lastCommandedLoad;
}

std::string DebenController::getLastErrorMessage() const
{
    return lastErrorMessage;
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

void DebenController::setError(const std::string& message)
{
    lastErrorMessage = message;
}

bool DebenController::isLoadSafe(double targetLoad) const
{
    return targetLoad >= minSafeLoad && targetLoad <= maxSafeLoad;
}

bool DebenController::isStepDeltaSafe(double targetLoad) const
{
    return std::fabs(targetLoad - lastCommandedLoad) <= maxStepDelta;
}