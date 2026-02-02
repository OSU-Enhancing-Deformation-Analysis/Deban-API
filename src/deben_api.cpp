#include <iostream>
#include <fstream>
#include <limits>
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
        while(true){
            cout << "The current load exceeds the safety limit, asking for manual load: ";
            cin >> humaninput;
            if(cin.fail() || humaninput > 200){
                cout << "Invalid input. Please enter a valid integer.\n" << endl;
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
            } else break;
        }
        TheData.predictedLoad = humaninput;
    }
    while(true){
        cout << "Do you wish to have (1-manually add inputs, 2-Automatic, 3-Human Confirming)? ";
        cin >> extraInput;
        if(cin.fail()||extraInput > 3||extraInput < 1){
            cout << "Invalid input. Please enter a valid integer.\n" << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        } else break;

    }
    if(extraInput == 1){
        while(true){
            cout << "Please enter the appropriate load: ";
            cin >> humaninput;
            if(cin.fail()|humaninput > 200){
                cout << "Invalid input. Please enter a valid integer.\n" << endl;
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
            } else break;
        }
        TheData.predictedLoad = humaninput;
        while(true){
            cout << "Please enter the appropriate extension: ";
            cin >> humaninput;
            if(cin.fail() || humaninput > 200){
                cout << "Invalid input. Please enter a valid integer.\n" << endl;
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
            } else break;
        }
        TheData.predictedExtension = humaninput;
        while(true){
            cout << "Please enter the appropriate displacement: ";
            cin >> humaninput;
            if(cin.fail() || humaninput > 200){
                cout << "Invalid input. Please enter a valid integer.\n" << endl;
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
            } else break;
        }
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

        while(true){
            cout << "Does " << TheData.predictedDisplacement << " for displacement work?"
                 << "(1 for yes and 0 for no): ";
            cin >> humaninput;
            if(cin.fail() && (humaninput == 1 || humaninput == 0)){
                cout << "Invalid input. Please enter a valid integer.\n" << endl;
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
            } else break;
        }
        if(!humaninput){
            while(true){
                cout << "Please enter the appropriate displacement: ";
                cin >> humaninput;
                if(cin.fail() || humaninput > 200){
                    cout << "Invalid input. Please enter a valid integer.\n" << endl;
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                } else break;
            }

            TheData.predictedDisplacement = humaninput;
        }
            while(true){
                cout << "Does " << TheData.predictedExtension << " for extension work?"
                     << "(1 for yes and 0 for no): ";
                cin >> humaninput;
                if(cin.fail() && (humaninput == 1 || humaninput == 0)){
                    cout << "Invalid input. Please enter a valid integer.\n" << endl;
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                } else break;
            }
        if(!humaninput){
            while(true){
                cout << "Please enter the appropriate value: ";
                cin >> humaninput;
                if(cin.fail() || humaninput > 200){
                    cout << "Invalid input. Please enter a valid integer.\n" << endl;
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                } else break;
            }
            TheData.predictedExtension = humaninput;
        }
        while(true){
            cout << "Does " << TheData.predictedLoad << " for load work?"
                << "(1 for yes and 0 for no): ";
            cin >> humaninput;
            if(cin.fail() && (humaninput == 1 || humaninput == 0)){
                cout << "Invalid input. Please enter a valid integer.\n" << endl;
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
            } else break;
        }
        if(!humaninput){
            while(true){
                cout << "Please enter the appropriate value: ";
                cin >> humaninput;
                if(cin.fail() || humaninput > 200){
                    cout << "Invalid input. Please enter a valid integer.\n" << endl;
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                } else break;
            }
            TheData.predictedLoad = humaninput;
        }
    }
}