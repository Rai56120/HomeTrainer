#include "User.hpp"

namespace {

double largestNonnegativeQuadraticRoot(const double coefficientA,
                                       const double coefficientB,
                                       const double coefficientC) {
    constexpr double epsilon = 1e-12;
    double largestRoot = 0.0;

    if(std::abs(coefficientA) < epsilon) {
        if(std::abs(coefficientB) < epsilon) {
            return 0.0;
        }

        const double root = -coefficientC / coefficientB;
        return root >= 0.0 ? root : 0.0;
    }

    const double discriminant = coefficientB * coefficientB -
                                4.0 * coefficientA * coefficientC;
    if(discriminant < 0.0) {
        return 0.0;
    }

    const double squareRoot = std::sqrt(discriminant);
    const double firstRoot = (-coefficientB + squareRoot) / (2.0 * coefficientA);
    const double secondRoot = (-coefficientB - squareRoot) / (2.0 * coefficientA);

    if(firstRoot >= 0.0) {
        largestRoot = firstRoot;
    }
    if(secondRoot >= 0.0) {
        largestRoot = std::max(largestRoot, secondRoot);
    }

    return largestRoot;
}

double largestNonnegativeCubicRoot(const double coefficientA,
                                   const double coefficientB,
                                   const double coefficientC,
                                   const double coefficientD) {
    constexpr double epsilon = 1e-12;
    if(std::abs(coefficientA) < epsilon) {
        return largestNonnegativeQuadraticRoot(coefficientB, coefficientC, coefficientD);
    }

    const double normalizedB = coefficientB / coefficientA;
    const double normalizedC = coefficientC / coefficientA;
    const double normalizedD = coefficientD / coefficientA;
    const double depressedP = normalizedC - normalizedB * normalizedB / 3.0;
    const double depressedQ = 2.0 * normalizedB * normalizedB * normalizedB / 27.0 -
                              normalizedB * normalizedC / 3.0 + normalizedD;
    const double halfQ = depressedQ / 2.0;
    const double thirdP = depressedP / 3.0;
    const double discriminant = halfQ * halfQ + thirdP * thirdP * thirdP;
    double largestRoot = 0.0;

    if(discriminant >= -epsilon) {
        const double squareRoot = std::sqrt(std::max(0.0, discriminant));
        const double firstTerm = std::cbrt(-halfQ + squareRoot);
        const double secondTerm = std::cbrt(-halfQ - squareRoot);
        const double root = firstTerm + secondTerm - normalizedB / 3.0;
        return root >= 0.0 ? root : 0.0;
    }

    if(std::abs(depressedP) < epsilon) {
        const double root = -normalizedB / 3.0;
        return root >= 0.0 ? root : 0.0;
    }

    const double radius = 2.0 * std::sqrt(-thirdP);
    const double cosineArgument = std::clamp(
        -halfQ / std::sqrt(-(thirdP * thirdP * thirdP)), -1.0, 1.0);
    const double angle = std::acos(cosineArgument);
    constexpr double twoPi = 6.28318530717958647692;

    for(int rootIndex = 0; rootIndex < 3; ++rootIndex) {
        const double root = radius * std::cos((angle + twoPi * rootIndex) / 3.0) -
                            normalizedB / 3.0;
        if(root >= 0.0) {
            largestRoot = std::max(largestRoot, root);
        }
    }

    return largestRoot;
}

}

User::User(const std::string username) : name(username) {
    weight = 60.0;
    height = 170;
    gender = MALE;
    frontalArea = 0.4;

    currentSpeed = 0.0;
    currentPower = 0.0;
    currentGradient = 0.0;

    coveredDistance = 0.0;
}

User::User( const std::string username, const double w, 
            const uint16_t h, const std::string g,
            const double frontArea) 
    : name(username), weight(w), height(h), frontalArea(frontArea) {
    if(g == "MALE") { gender = MALE; }
    else if(g == "FEMALE") { gender = FEMALE; }
    
    currentSpeed = 0.0;
    currentPower = 0.0;
    currentGradient = 0.0;
    coveredDistance = 0.0;
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
    currentPower = power;
    if(!bike) {
        currentSpeed = 0.0;
        return;
    }

    const double CdARho = bike->getDragCoef() * frontalArea * bike->getAirDensity();
    const double angle = atan(currentGradient / 100);
    const double a = CdARho / 2;
    const double b = bike->getHeadWind() * CdARho;
    const double c = (bike->getGravity() * (weight + bike->getWeight()) * 
                (sin(angle) + bike->getRollingResistCoef() * cos(angle))) +
                (a * bike->getHeadWind() * bike->getHeadWind());

    const double d = - (1 - bike->getDriveTrainLosses()) * power;

    const double speedMs = largestNonnegativeCubicRoot(a, b, c, d);
    currentSpeed = speedMs * 3.6;
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
