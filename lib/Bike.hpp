#pragma once

#include <iostream>
#include <string>

class Bike {
private:
    std::string model;
    double weight;
    double dragCoef;
    double lDriveTrain;
    double airDensity;
    double headWindSpeed = 0.0; // in m/s
    double gravityConstant;
    double rollingResistCoef;

public:
    Bike(   const std::string modelName, const double bikeWeight, 
            const double dragCoefficient, const double driveTrainLosses,
            const double airDens, const double gravity, const double crr);
    // Getters
    double getWeight() const;
    double getDragCoef() const;
    double getDriveTrainLosses() const;
    double getAirDensity() const;
    double getRollingResistCoef() const;
    double getHeadWind() const;
    double getGravity() const;
};