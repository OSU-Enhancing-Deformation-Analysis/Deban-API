#include <iostream>
#include <fstream>
#include "deben_api.h"

using namespace std;


//Class methods - Deben
bool DebenAPI::connect(){
    cout << "[MOCK] Connect\n"; 
    return true;
}

void DebenAPI::disconnect(){
    cout << "[MOCK] DisConnect\n";
}

bool DebenAPI::startMotor(){
    cout << "[MOCK] Motor Start\n";
    return true;
}

void DebenAPI::stopMotor(){
    cout << "[MOCK] Motor Stop\n";
}

double DebenAPI::getForce(){
    return 42.8;
}

double DebenAPI::getExtension(){
    return 3.14;
}

double DebenAPI::getPosition(){
    return 0.92;
}

void DebenAPI::gotoload(double load){
    cout << "[MOCK] GoTo Load: " << load << endl;
}

void DebenAPI::gotoExtension(double mm){
    cout << "[MOCK] GoTo Extension: " << mm << endl;
}

//Function to get the data from the python script, which will contain the ML data output. 
void Bridging(MLData& values){
    ifstream file("data.txt");
    if (!file) {
        cerr << "Error: Could not open file.\n";
    }
    file >> values.confidence >> values.predictedLoad >> values.strainRate;
}

void humanInput(MLData& TheData){
    float safetyLimit = 200;
    if(TheData.predictedLoad > safetyLimit){
        cout << "The current load exceeds the safety limit, asking for manual load: ";
        float humaninput;
        cin >> humaninput;
        TheData.predictedLoad = humaninput;
    }
}