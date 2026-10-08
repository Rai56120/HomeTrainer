#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <cmath>
#include <memory>
#include <cstdint>
#include <algorithm>

#include "Bike.hpp"

/** @brief Gender values accepted by the user profile model. */
enum Gender {
    MALE,
    FEMALE
};

/** @brief Stores rider attributes and current speed/distance simulation state. */
class User {
private:
    std::string name;

    double weight; // in kg
    uint16_t height; // in centimeters
    Gender gender;
    double frontalArea;

    std::shared_ptr<Bike> bike;

    double currentPower; // in W
    double currentSpeed; // in km/h
    double currentGradient; // in %

    double coveredDistance; // in kilometers
public:
    /**
     * @brief Creates a user with default physical attributes and zeroed simulation state.
     * @param username User's display name.
     */
    User(const std::string username);
    /**
     * @brief Creates a user with the supplied physical attributes.
     * @param username User's display name.
     * @param w Body mass in kilograms.
     * @param h Height in centimeters.
     * @param g Gender value; "MALE" selects male and "FEMALE" selects female.
     * @param frontal_area Rider's frontal area in square meters.
     */
    User(const std::string username, const double w, 
         const uint16_t h, const std::string g,
         const double frontal_area);
    /** @brief Destroys the user and releases any owned bike reference. */
    ~User();
    
    /// @brief Returns current speed in kilometers per hour.
    double getCurrentSpeed() const;
    /// @brief Returns distance covered in kilometers.
    double getCoveredDistance() const;
    /// @brief Returns the current road gradient as a percentage.
    double getCurrentGradient() const;
    /**
     * @brief Sets rider power and calculates the resulting speed.
     * @param power Rider power in watts.
     * @details Speed is calculated from the current gradient and bike parameters.
     * Without an assigned bike, current speed is set to zero.
     */
    void setCurrentSpeed(const double power);
    /**
     * @brief Sets the current signed road gradient as a percentage.
     * @param gradient Road gradient, positive uphill and negative downhill.
     */
    void setCurrentGradient(const double gradient);
    /**
     * @brief Adds a distance in kilometers to the user's covered distance.
     * @param d Distance to add in kilometers.
     */
    void addToCoveredDistance(const double d);
    /**
     * @brief Assigns the bike parameters used for speed calculations.
     * @param b Shared bike instance, or an empty pointer to clear the assignment.
     */
    void setBike(std::shared_ptr<Bike> b);

    /** @brief Prints the user's physical attributes to standard output. */
    void printInfos() const;
};