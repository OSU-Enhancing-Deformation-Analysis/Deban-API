#include <iostream>
#include "deben_api.h"

int main() {
    DebenAPI api;
    MLData data;

    if (!Bridging(data, "data.txt")) {
        return 1;
    }

    std::cout
        << "Confidence: " << data.confidence << "\n"
        << "Predicted Load: " << data.predictedLoad << "\n"
        << "Strain: " << data.strainRate << "\n\n";

    humanInput(data);

    api.connect();
    api.startMotor();
    api.gotoload(data.predictedLoad);
    api.gotoDisplacement(data.predictedDisplacement);
    api.gotoExtension(data.predictedExtension);
    api.stopMotor();
    api.disconnect();

    return 0;
}