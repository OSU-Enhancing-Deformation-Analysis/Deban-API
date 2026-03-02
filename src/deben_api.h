#ifndef DEBEN_API_H
#define DEBEN_API_H

#include <string>

struct MLData {
    float confidence = 0.0f;
    float predictedLoad = 0.0f;
    float strainRate = 0.0f;
    float predictedExtension = 0.0f;
    float predictedDisplacement = 0.0f;
};

struct HumanInput {
    int InputAutomation = 0;
};

class DebenAPI {
public:
    bool connect();
    void disconnect();
    bool startMotor();
    void stopMotor();

    double getForce();
    double getExtension();
    double getPosition();

    void gotoload(double load);
    void gotoExtension(double mm);
    void gotoDisplacement(double d);
};

bool Bridging(MLData& values, const std::string& path);
void humanInput(MLData& data);

#endif