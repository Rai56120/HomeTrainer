#pragma once

#include <iostream>
#include <string>

/** @brief Stores the equipment parameters used by the rider speed model. */
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
    /**
     * @brief Creates a bike with the physical parameters used by the speed model.
     * @param modelName Display name of the bike.
     * @param bikeWeight Bike mass in kilograms.
     * @param dragCoefficient Aerodynamic drag coefficient.
     * @param driveTrainLosses Fraction of rider power lost in the drivetrain.
     * @param airDens Air density in kilograms per cubic meter.
     * @param gravity Gravitational acceleration in meters per second squared.
     * @param crr Rolling-resistance coefficient.
     */
    Bike(   const std::string modelName, const double bikeWeight, 
            const double dragCoefficient, const double driveTrainLosses,
            const double airDens, const double gravity, const double crr);
    /// @brief Returns the bike mass in kilograms.
    double getWeight() const;
    /// @brief Returns the aerodynamic drag coefficient.
    double getDragCoef() const;
    /// @brief Returns the fraction of power lost in the drivetrain.
    double getDriveTrainLosses() const;
    /// @brief Returns air density in kilograms per cubic meter.
    double getAirDensity() const;
    /// @brief Returns the rolling-resistance coefficient.
    double getRollingResistCoef() const;
    /// @brief Returns the headwind speed in meters per second.
    double getHeadWind() const;
    /// @brief Returns gravitational acceleration in meters per second squared.
    double getGravity() const;
};