#include "User.hpp"

User::User(const std::string username) : name(username) {
    weight = 60.0;
    height = 170;
    gender = MALE;

    currentSpeed = 0.0;
    currentPower = 0.0;

    coveredDistance = 0.0;
}

User::User( const std::string username, const double w, 
            const uint16_t h, const std::string g,
            const double frontArea) 
    : name(username), weight(w), height(h), frontalArea(frontArea) {
    if(g == "MALE") { gender = MALE; }
    else if(g == "FEMALE") { gender = FEMALE; }
    
    currentSpeed = 20;
}

User::~User() {}

//getters
double User::getCurrentSpeed() const {
    return currentSpeed;
}

double User::getCoveredDistance() const {
    return coveredDistance;
}

double User::getCurrentGradient() const {
    return currentGradient;
}

//setters
void User::setCurrentSpeed(const double power) {
    const double CdARho = bike->getDragCoef() * frontalArea * bike->getAirDensity();
    const double angle = atan(currentGradient / 100);
    const double a = CdARho / 2;
    const double b = bike->getHeadWind() * CdARho;
    const double c = (bike->getGravity() * (weight + bike->getWeight()) * 
                (sin(angle) + bike->getRollingResistCoef() * cos(angle))) +
                (a * bike->getHeadWind() * bike->getHeadWind());

    const double d = - (1 - bike->getDriveTrainLosses()) * power;

    const double q = (3*a*c - b*b) / (9 *a*a);
    const double r = (9*a*b*c - (27 *a*a*d) - (2*b*b*b)) / (54 * a*a*a);
    const double s = cbrt(r + sqrt(q*q*q + r*r));
    const double t = cbrt(r - sqrt(q*q*q + r*r));

    currentSpeed = s + t - (b / (3*a)) * 3.6;
}

void User::setCurrentGradient(const double gradient) {
    currentGradient = gradient;
}

void User::addToCoveredDistance(const double d) {
    coveredDistance += d;
}

void User::setBike(std::shared_ptr<Bike> b) {
    bike = b;
}

void User::printInfos() const {
    std::cout<< "********************" << std::endl
        << "     User infos" << std::endl
        << "name: " << name << std::endl
        << "weight: " << weight << "kg" << std::endl
        << "height: " << height << "cm" << std::endl
        << "gender: " << (gender == MALE ? "MALE" : "female") << std::endl
        << "********************" << std::endl;
}
