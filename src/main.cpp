#include <iostream>
#include <fstream>
#include "deben_api.h"
#include <vector>
#include <windows.h>

using namespace std;

int main() {
    DebenAPI api;
    MLData TheData;
    Bridging(TheData);

    //Print out the current confidence, load and strain of ML
    cout << "Confidence: " << TheData.confidence << "\n"
    << "Predicted Load: " << TheData.predictedLoad << "\n"
    << "Strain: " << TheData.strainRate << "\n" << endl;

    humanInput(TheData);

    api.connect();
    api.startMotor();
    api.gotoload(TheData.predictedLoad);
    api.stopMotor();
    api.disconnect();
    return 0;
}