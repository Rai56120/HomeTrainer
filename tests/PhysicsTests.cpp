#include <cmath>
#include <memory>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include "Bike.hpp"
#include "Route.hpp"
#include "User.hpp"

namespace {

std::shared_ptr<Bike> createBike() {
    return std::make_shared<Bike>(
        "Test bike", 7.54, 0.88, 0.03, 1.22601, 9.80665, 0.005);
}

User createUser() {
    User user("Test user", 60.0, 170, "MALE", 0.40);
    user.setBike(createBike());
    return user;
}

}

TEST_CASE("flat-road speed at 500 watts") {
    User flatUser = createUser();
    flatUser.setCurrentGradient(0.0);
    flatUser.setCurrentSpeed(500.0);
    const double flatSpeed = flatUser.getCurrentSpeed();
    CHECK(std::isfinite(flatSpeed));
    CHECK(flatSpeed > 40.0);
    CHECK(flatSpeed < 50.0);
}

TEST_CASE("uphill speed is lower than flat speed") {
    User flatUser = createUser();
    flatUser.setCurrentGradient(0.0);
    flatUser.setCurrentSpeed(500.0);
    const double flatSpeed = flatUser.getCurrentSpeed();

    User climbUser = createUser();
    climbUser.setCurrentGradient(8.0);
    climbUser.setCurrentSpeed(500.0);
    CHECK(std::isfinite(climbUser.getCurrentSpeed()));
    CHECK(climbUser.getCurrentSpeed() < flatSpeed);
}

TEST_CASE("zero power on a descent remains finite and positive") {
    User descentUser = createUser();
    descentUser.setCurrentGradient(-5.0);
    descentUser.setCurrentSpeed(0.0);
    CHECK(std::isfinite(descentUser.getCurrentSpeed()));
    CHECK(descentUser.getCurrentSpeed() > 0.0);
}

TEST_CASE("zero power on flat ground stops") {
    User stoppedUser = createUser();
    stoppedUser.setCurrentGradient(0.0);
    stoppedUser.setCurrentSpeed(0.0);
    CHECK(stoppedUser.getCurrentSpeed() == 0.0);
}

TEST_CASE("missing bike produces zero speed") {
    User userWithoutBike("No bike", 60.0, 170, "MALE", 0.40);
    userWithoutBike.setCurrentSpeed(500.0);
    CHECK(userWithoutBike.getCurrentSpeed() == 0.0);
}

TEST_CASE("bundled GPX produces finite route geometry") {
    Route route("../resources/ninian_bourg.gpx");
    const auto& segments = route.getAllSegments();
    REQUIRE(segments.size() > 1);
    CHECK(std::isfinite(segments.front().getGradient()));
    CHECK(segments.front().getLength() > 0.0);

    double segmentDistance = 0.0;
    for(const auto& segment : segments) {
        CHECK(std::isfinite(segment.getLength()));
        CHECK(std::isfinite(segment.getGradient()));
        segmentDistance += segment.getLength() / 1000.0;
    }
    CHECK(std::abs(segmentDistance - route.getTotalDistance()) < 1e-9);
}
