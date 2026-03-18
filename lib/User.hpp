#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <cmath>
#include <memory>
#include <cstdint>

#include "Bike.hpp"

enum Gender {
    MALE,
    FEMALE
};

class User {
private:
    std::string name;

    double weight; // in kg
    uint16_t height; // in centimeters
    Gender gender;
    double frontalArea;

    std::shared_ptr<Bike> bike;

    double currentSpeed; // in km/h
    double currentGradient; // in %

    double coveredDistance; // in kilometers

    double calculateInstantSpeed(const double power);
    double calculateAcceleration(const double power);
public:
    User(const std::string username);
    User(const std::string username, const double w, 
         const uint16_t h, const std::string g,
         const double frontal_area);
    ~User();
    
    //getters
    double getCurrentSpeed() const;
    double getCoveredDistance() const;
    double getCurrentGradient() const;
    //setters
    void setCurrentSpeed(const double power, const uint32_t time_interval_ms);
    void setCurrentGradient(const double gradient);
    void addToCoveredDistance(const double d);
    void setBike(std::shared_ptr<Bike> b);

    void printInfos() const;
};