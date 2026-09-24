#include <cassert>
#include <cmath>
#include <memory>

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

int main() {
    User flatUser = createUser();
    flatUser.setCurrentGradient(0.0);
    flatUser.setCurrentSpeed(500.0);
    const double flatSpeed = flatUser.getCurrentSpeed();
    assert(std::isfinite(flatSpeed));
    assert(flatSpeed > 40.0 && flatSpeed < 50.0);

    User climbUser = createUser();
    climbUser.setCurrentGradient(8.0);
    climbUser.setCurrentSpeed(500.0);
    assert(std::isfinite(climbUser.getCurrentSpeed()));
    assert(climbUser.getCurrentSpeed() < flatSpeed);

    User descentUser = createUser();
    descentUser.setCurrentGradient(-5.0);
    descentUser.setCurrentSpeed(0.0);
    assert(std::isfinite(descentUser.getCurrentSpeed()));
    assert(descentUser.getCurrentSpeed() > 0.0);

    User stoppedUser = createUser();
    stoppedUser.setCurrentGradient(0.0);
    stoppedUser.setCurrentSpeed(0.0);
    assert(stoppedUser.getCurrentSpeed() == 0.0);

    User userWithoutBike("No bike", 60.0, 170, "MALE", 0.40);
    userWithoutBike.setCurrentSpeed(500.0);
    assert(userWithoutBike.getCurrentSpeed() == 0.0);

    Route route("../resources/ninian_bourg.gpx");
    const auto& segments = route.getAllSegments();
    assert(segments.size() > 1);
    assert(std::isfinite(segments.front().getGradient()));
    assert(segments.front().getLength() > 0.0);

    double segmentDistance = 0.0;
    for(const auto& segment : segments) {
        assert(std::isfinite(segment.getLength()));
        assert(std::isfinite(segment.getGradient()));
        segmentDistance += segment.getLength() / 1000.0;
    }
    assert(std::abs(segmentDistance - route.getTotalDistance()) < 1e-9);

    return 0;
}
