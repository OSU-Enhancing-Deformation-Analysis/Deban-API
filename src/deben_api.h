#ifndef DEBEN_API_H
#define DEBEN_API_H
#include <string>
#include <vector>

//Struct to make the data more apparent.
struct MLData{
    float confidence;
    float predictedLoad;
    float strainRate;
    float predictedExtension;
    float predictedDisplacement;
};

//This struct is currently unused, but will likely be implemented in further iterations.
struct HumanInput{
    int InputAutomation;
};

//Class for the DebenAPI
//TODO: Actually acquire the DebenAPI at some point, so that we can then directly implement in this class.
class DebenAPI{
    public:
    bool connect();
    void disconnect();
    bool startMotor();
    void stopMotor();
    
    double getForce();
    double getExtension();
    double getPosition();

    void gotoload(double);
    void gotoExtension(double);
    void gotoDisplacement(double);
};

void Bridging(MLData&);
void humanInput(MLData&);
#endif