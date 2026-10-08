#include <doctest/doctest.h>

#include "Vertex.hpp"

/** @brief Verifies that vertex getters return the ID and stored coordinates. */
TEST_CASE("Vertex exposes its coordinate data") {
    Vertex vertex(42, 48.001, -2.486, 45.5);

    CHECK(vertex.getId() == 42);
    CHECK(vertex.getLat() == doctest::Approx(48.001));
    CHECK(vertex.getLong() == doctest::Approx(-2.486));
    CHECK(vertex.getAlt() == doctest::Approx(45.5));
}
