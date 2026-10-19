#include <doctest/doctest.h>

#include <memory>

#include "Bike.hpp"
#include "Route.hpp"
#include "Simulation.hpp"
#include "User.hpp"

namespace {

/** @brief Creates a simulation-test user assigned to the standard test bike. */
User createSimulationUser() {
    User user("Simulation user", 60.0, 170, "MALE", 0.40);
    user.setBike(std::make_shared<Bike>(
        "Test bike", 7.54, 0.88, 0.03, 1.22601, 9.80665, 0.005));
    return user;
}

}

/** @brief Verifies a fixed simulation tick advances distance without finishing early. */
TEST_CASE("Simulation tick advances deterministic distance") {
    Route route(std::string(HOME_TRAINER_TEST_FIXTURE_DIR) + "/flat-two-points.gpx");
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

/** @brief Verifies simulation distance is clamped at the route end. */
TEST_CASE("Simulation clamps at route completion") {
    Route route(std::string(HOME_TRAINER_TEST_FIXTURE_DIR) + "/flat-two-points.gpx");
    Simulation simulation(createSimulationUser(), route);
    simulation.setPower(500.0);

    simulation.tick(20.0);
    const SimulationState state = simulation.getState();

    CHECK(state.finished);
    CHECK(state.distanceKm == doctest::Approx(state.routeDistanceKm));
    CHECK_FALSE(state.running);
}

/** @brief Pausing clears speed and power while preserving route progress. */
TEST_CASE("Simulation pause keeps distance") {
    Route route(std::string(HOME_TRAINER_TEST_FIXTURE_DIR) + "/flat-two-points.gpx");
    Simulation simulation(createSimulationUser(), route);
    simulation.setPower(500.0);
    simulation.tick(1.0);
    const double distanceBeforePause = simulation.getState().distanceKm;

    simulation.pause();
    const SimulationState state = simulation.getState();

    CHECK(state.distanceKm == doctest::Approx(distanceBeforePause));
}

/** @brief Stopping clears motion and resets route progress. */
TEST_CASE("Simulation stop clears speed power and distance") {
    Route route(std::string(HOME_TRAINER_TEST_FIXTURE_DIR) + "/flat-two-points.gpx");
    Simulation simulation(createSimulationUser(), route);
    simulation.setPower(500.0);
    simulation.tick(1.0);

    simulation.stop();
    const SimulationState state = simulation.getState();

    CHECK(state.speedKmh == doctest::Approx(0.0));
    CHECK(state.powerWatts == doctest::Approx(0.0));
    CHECK(state.distanceKm == doctest::Approx(0.0));
}
