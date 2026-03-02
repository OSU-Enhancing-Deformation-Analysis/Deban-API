#include <iostream>
#include <fstream>
#include <limits>
#include <string>
#include "deben_api.h"

namespace {

float readFloatInRange(const std::string& prompt, float minValue, float maxValue) {
    float value = 0.0f;
    while (true) {
        std::cout << prompt;
        std::cin >> value;

        if (std::cin.fail() || value < minValue || value > maxValue) {
            std::cout << "Invalid input. Please enter a valid value.\n\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }
        return value;
    }
}

int readIntChoice(const std::string& prompt, int minValue, int maxValue) {
    int value = 0;
    while (true) {
        std::cout << prompt;
        std::cin >> value;

        if (std::cin.fail() || value < minValue || value > maxValue) {
            std::cout << "Invalid input. Please enter a valid integer.\n\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }
        return value;
    }
}

bool readYesNo(const std::string& prompt) {
    int choice = readIntChoice(prompt, 0, 1);
    return choice == 1;
}

void enforceSafetyLimit(float& load, float safetyLimit) {
    if (load <= safetyLimit) return;

    load = readFloatInRange(
        "The current load exceeds the safety limit, asking for manual load: ",
        0.0f,
        safetyLimit
    );
}

void confirmOrEdit(const std::string& label, float& value, float minValue, float maxValue) {
    std::string prompt = "Does " + std::to_string(value) + " for " + label + " work? (1 for yes and 0 for no): ";
    bool ok = readYesNo(prompt);
    if (ok) return;

    std::string editPrompt = "Please enter the appropriate " + label + ": ";
    value = readFloatInRange(editPrompt, minValue, maxValue);
}

} 

bool DebenAPI::connect() {
    std::cout << "[MOCK] Connect\n";
    return true;
}

void DebenAPI::disconnect() {
    std::cout << "[MOCK] DisConnect\n";
}

bool DebenAPI::startMotor() {
    std::cout << "[MOCK] Motor Start\n";
    return true;
}

void DebenAPI::stopMotor() {
    std::cout << "[MOCK] Motor Stop\n";
}

double DebenAPI::getForce() {
    return 42.8;
}

double DebenAPI::getExtension() {
    return 3.14;
}

double DebenAPI::getPosition() {
    return 0.92;
}

void DebenAPI::gotoload(double load) {
    std::cout << "[MOCK] GoTo Load: " << load << "\n";
}

void DebenAPI::gotoExtension(double mm) {
    std::cout << "[MOCK] GoTo Extension: " << mm << "\n";
}

void DebenAPI::gotoDisplacement(double d) {
    std::cout << "[MOCK] GoTo Displacement: " << d << "\n";
}

bool Bridging(MLData& values, const std::string& path) {
    std::ifstream file(path);
    if (!file) {
        std::cerr << "Error: Could not open file.\n";
        return false;
    }

    if (!(file >> values.confidence >> values.predictedLoad >> values.strainRate)) {
        std::cerr << "Error: Could not parse ML data.\n";
        return false;
    }

    return true;
}

void humanInput(MLData& data) {
    const float safetyLimit = 200.0f;
    const float minValue = 0.0f;
    const float maxValue = 200.0f;

    enforceSafetyLimit(data.predictedLoad, safetyLimit);

    int mode = readIntChoice(
        "Do you wish to have (1 manually add inputs, 2 Automatic, 3 Human Confirming)? ",
        1,
        3
    );

    if (mode == 1) {
        data.predictedLoad = readFloatInRange("Please enter the appropriate load: ", minValue, maxValue);
        data.predictedExtension = readFloatInRange("Please enter the appropriate extension: ", minValue, maxValue);
        data.predictedDisplacement = readFloatInRange("Please enter the appropriate displacement: ", minValue, maxValue);
        return;
    }

    if (mode == 2) {
        data.predictedDisplacement = 10.0f;
        data.predictedLoad = 10.0f;
        data.predictedExtension = 10.0f;
        return;
    }

    data.predictedDisplacement = 10.0f;
    data.predictedLoad = 10.0f;
    data.predictedExtension = 10.0f;

    confirmOrEdit("displacement", data.predictedDisplacement, minValue, maxValue);
    confirmOrEdit("extension", data.predictedExtension, minValue, maxValue);
    confirmOrEdit("load", data.predictedLoad, minValue, safetyLimit);

    enforceSafetyLimit(data.predictedLoad, safetyLimit);
}