#include "Bike.hpp"

Bike::Bike( const std::string modelName, const double bikeWeight, 
            const double dragCoefficient, const double driveTrainLosses,
            const double airDens, const double gravity, const double crr)
         :  model(modelName), weight(bikeWeight), 
            dragCoef(dragCoefficient), lDriveTrain(driveTrainLosses),
            airDensity(airDens), gravityConstant(gravity), rollingResistCoef(crr) {}

double Bike::getWeight() const {
    return weight;
}

double Bike::getDragCoef() const {
    return dragCoef;
}
 
double Bike::getDriveTrainLosses() const {
    return lDriveTrain;
}

double Bike::getAirDensity() const {
    return airDensity;
}

double Bike::getRollingResistCoef() const {
    return rollingResistCoef;
}

double Bike::getHeadWind() const {
    return headWindSpeed;
}

double Bike::getGravity() const {
    return gravityConstant;
}