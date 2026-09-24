#include <doctest/doctest.h>

#include <memory>

#include "Bike.hpp"
#include "Route.hpp"
#include "Simulation.hpp"
#include "User.hpp"

namespace {

User createSimulationUser() {
    User user("Simulation user", 60.0, 170, "MALE", 0.40);
    user.setBike(std::make_shared<Bike>(
        "Test bike", 7.54, 0.88, 0.03, 1.22601, 9.80665, 0.005));
    return user;
}

}

TEST_CASE("Simulation tick advances deterministic distance") {
    Route route("../tests/fixtures/flat-two-points.gpx");
    Simulation simulation(createSimulationUser(), route);
    simulation.setPower(500.0);

    const SimulationState initial = simulation.getState();
    simulation.tick(1.0);
    const SimulationState afterTick = simulation.getState();

    CHECK(initial.distanceKm == doctest::Approx(0.0));
    CHECK(initial.gradientPercent == doctest::Approx(0.0));
    CHECK(afterTick.speedKmh > 0.0);
    CHECK(afterTick.distanceKm > initial.distanceKm);
    CHECK(afterTick.distanceKm < afterTick.routeDistanceKm);
    CHECK_FALSE(afterTick.finished);
}

TEST_CASE("Simulation clamps at route completion") {
    Route route("../tests/fixtures/flat-two-points.gpx");
    Simulation simulation(createSimulationUser(), route);
    simulation.setPower(500.0);

    simulation.tick(20.0);
    const SimulationState state = simulation.getState();

    CHECK(state.finished);
    CHECK(state.distanceKm == doctest::Approx(state.routeDistanceKm));
    CHECK_FALSE(state.running);
}
