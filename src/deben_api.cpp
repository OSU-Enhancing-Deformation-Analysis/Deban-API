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

void DebenAPI::gotoDisplacement(double d){
    cout << "[MOCK] GoTo Displacement: " << d << endl;
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
    float targetLoad, targetDisplacement, targetExtension;
    float humaninput;
    int extraInput;
    if(TheData.predictedLoad > safetyLimit){
        cout << "The current load exceeds the safety limit, asking for manual load: ";
        cin >> humaninput;
        TheData.predictedLoad = humaninput;
    }
    cout << "Do you wish to have (1-manually add inputs, 2-Automatic, 3-Human Confirming)? ";
    cin >> extraInput;
    if(extraInput == 1){
        cout << "Please enter the appropriate load: ";
        cin >> humaninput;
        TheData.predictedLoad = humaninput;
        cout << "Please enter the appropriate extension: ";
        cin >> humaninput;
        TheData.predictedExtension = humaninput;
        cout << "Please enter the appropriate displacement: ";
        cin >> humaninput;
        TheData.predictedDisplacement = humaninput;
    }
    else if(extraInput == 2){
        TheData.predictedDisplacement = 10;
        TheData.predictedLoad = 10;
        TheData.predictedExtension = 10;
    }
    else{
        TheData.predictedDisplacement = 10;
        TheData.predictedLoad = 10;
        TheData.predictedExtension = 10;

        cout << "Does " << TheData.predictedDisplacement << " for displacement work?"
             << "(1 for yes and 0 for no): ";
        cin >> humaninput;
        if(!humaninput){
            cout << "Please enter the appropriate displacement: ";
            cin >> humaninput;
            TheData.predictedDisplacement = humaninput;
        }
        cout << "Does " << TheData.predictedExtension << " for extension work?"
             << "(1 for yes and 0 for no): ";
        cin >> humaninput;
        if(!humaninput){
            cout << "Please enter the appropriate value: ";
            cin >> humaninput;
            TheData.predictedExtension = humaninput;
        }
        cout << "Does " << TheData.predictedLoad << " for load work?"
             << "(1 for yes and 0 for no): ";
        cin >> humaninput;
        if(!humaninput){
            cout << "Please enter the appropriate value: ";
            cin >> humaninput;
            TheData.predictedLoad = humaninput;
        }
    }
}