#include <cmath>
#include <map>

#include <doctest/doctest.h>

#include "Edge.hpp"
#include "Vertex.hpp"

/** @brief Verifies that edge length is calculated from geographic coordinates. */
TEST_CASE("Edge calculates geographic distance") {
    std::map<uint32_t, Vertex> vertices{
        {1, Vertex(1, 48.0, -2.0, 0.0)},
        {2, Vertex(2, 48.001, -2.0, 0.0)}
    };
    Edge edge(1, 2);

    edge.setLength(vertices);

    CHECK(edge.getLength() == doctest::Approx(111.2).epsilon(0.01));
}

/** @brief Verifies edge gradients preserve the sign of elevation changes. */
TEST_CASE("Edge calculates ascent and descent gradients") {
    std::map<uint32_t, Vertex> vertices{
        {1, Vertex(1, 48.0, -2.0, 10.0)},
        {2, Vertex(2, 48.001, -2.0, 20.0)},
        {3, Vertex(3, 48.002, -2.0, 15.0)}
    };
    Edge ascent(1, 2);
    Edge descent(2, 3);

    ascent.setLength(vertices);
    ascent.setGradient(vertices);
    descent.setLength(vertices);
    descent.setGradient(vertices);

    CHECK(std::isfinite(ascent.getGradient()));
    CHECK(std::isfinite(descent.getGradient()));
    CHECK(ascent.getGradient() > 0.0);
    CHECK(descent.getGradient() < 0.0);
}

/** @brief Verifies a zero-length edge has a zero gradient without invalid arithmetic. */
TEST_CASE("Edge handles a zero-length segment") {
    std::map<uint32_t, Vertex> vertices{
        {1, Vertex(1, 48.0, -2.0, 10.0)},
        {2, Vertex(2, 48.0, -2.0, 10.0)}
    };
    Edge edge(1, 2);

    edge.setLength(vertices);
    edge.setGradient(vertices);

    CHECK(edge.getLength() == doctest::Approx(0.0));
    CHECK(edge.getGradient() == doctest::Approx(0.0));
}
