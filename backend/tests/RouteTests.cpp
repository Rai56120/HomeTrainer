#include <cmath>

#include <doctest/doctest.h>

#include "Route.hpp"

/** @brief Verifies that an unavailable GPX file produces an empty route. */
TEST_CASE("missing route file produces an empty route") {
    const std::string fixtureDir = HOME_TRAINER_TEST_FIXTURE_DIR;
    Route route(fixtureDir + "/does-not-exist.gpx");

    CHECK(route.getAllSegments().empty());
    CHECK(route.getTotalDistance() == doctest::Approx(0.0));
}

/** @brief Verifies GPX tracks with fewer than two points produce no segments. */
TEST_CASE("empty and one-point GPX files produce no edges") {
    const std::string fixtureDir = HOME_TRAINER_TEST_FIXTURE_DIR;
    Route emptyRoute(fixtureDir + "/empty.gpx");
    Route onePointRoute(fixtureDir + "/one-point.gpx");

    CHECK(emptyRoute.getAllSegments().empty());
    CHECK(onePointRoute.getAllSegments().empty());
    CHECK(emptyRoute.getTotalDistance() == doctest::Approx(0.0));
    CHECK(onePointRoute.getTotalDistance() == doctest::Approx(0.0));
}

/** @brief Verifies a two-point GPX track produces one valid flat segment. */
TEST_CASE("two-point GPX produces one finite flat edge") {
    Route route(std::string(HOME_TRAINER_TEST_FIXTURE_DIR) + "/flat-two-points.gpx");

    REQUIRE(route.getAllSegments().size() == 1);
    const Edge& edge = route.getAllSegments().front();
    CHECK(edge.getLength() > 0.0);
    CHECK(std::isfinite(edge.getLength()));
    CHECK(edge.getGradient() == doctest::Approx(0.0));
    CHECK(route.getTotalDistance() == doctest::Approx(edge.getLength() / 1000.0));
}
