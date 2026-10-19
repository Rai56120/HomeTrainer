#include <doctest/doctest.h>

#include "Bike.hpp"

/** @brief Verifies that a bike retains all configured physical parameters. */
TEST_CASE("Bike stores physical parameters") {
    Bike bike("Test bike", 7.54, 0.88, 0.03, 1.22601, 9.80665, 0.005);

    CHECK(bike.getWeight() == doctest::Approx(7.54));
    CHECK(bike.getDragCoef() == doctest::Approx(0.88));
    CHECK(bike.getDriveTrainLosses() == doctest::Approx(0.03));
    CHECK(bike.getAirDensity() == doctest::Approx(1.22601));
    CHECK(bike.getGravity() == doctest::Approx(9.80665));
    CHECK(bike.getRollingResistCoef() == doctest::Approx(0.005));
    CHECK(bike.getHeadWind() == doctest::Approx(0.0));
}
