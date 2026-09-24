#include <cmath>

#include <doctest/doctest.h>

#include "Route.hpp"

TEST_CASE("missing route file produces an empty route") {
    Route route("../tests/fixtures/does-not-exist.gpx");

    CHECK(route.getAllSegments().empty());
    CHECK(route.getTotalDistance() == doctest::Approx(0.0));
}

TEST_CASE("empty and one-point GPX files produce no edges") {
    Route emptyRoute("../tests/fixtures/empty.gpx");
    Route onePointRoute("../tests/fixtures/one-point.gpx");

    CHECK(emptyRoute.getAllSegments().empty());
    CHECK(onePointRoute.getAllSegments().empty());
    CHECK(emptyRoute.getTotalDistance() == doctest::Approx(0.0));
    CHECK(onePointRoute.getTotalDistance() == doctest::Approx(0.0));
}

TEST_CASE("two-point GPX produces one finite flat edge") {
    Route route("../tests/fixtures/flat-two-points.gpx");

    REQUIRE(route.getAllSegments().size() == 1);
    const Edge& edge = route.getAllSegments().front();
    CHECK(edge.getLength() > 0.0);
    CHECK(std::isfinite(edge.getLength()));
    CHECK(edge.getGradient() == doctest::Approx(0.0));
    CHECK(route.getTotalDistance() == doctest::Approx(edge.getLength() / 1000.0));
}
