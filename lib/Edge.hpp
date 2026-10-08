#pragma once

#include <cmath>
#include <map>
#include <cstdint>
#include <stdexcept>
#include <iostream>
#include <algorithm>

#include "Vertex.hpp"

/** @brief Represents a measured segment between two route vertices. */
class Edge {
private:
    uint32_t startId;
    uint32_t endId;
    double length;
    double startKilometer;
    double gradient;

    /**
     * @brief Calculates the three-dimensional distance between two vertices.
     * @param v1 Start vertex.
     * @param v2 End vertex.
     * @return Distance in meters, combining horizontal and altitude differences.
     */
    double calculateDistance(const Vertex& v1, const Vertex& v2) const;
public:
    /**
     * @brief Creates a segment connecting the vertices with the given IDs.
     * @param stId Start vertex ID.
     * @param enId End vertex ID.
     */
    Edge(const uint32_t stId, const uint32_t enId);
    /** @brief Destroys the segment. */
    ~Edge();

    /// @brief Returns the ID of the segment's start vertex.
    uint32_t getStartId() const;
    /// @brief Returns the ID of the segment's end vertex.
    uint32_t getEndId() const;
    /// @brief Returns the segment length in meters.
    double getLength() const;
    /// @brief Returns the signed road gradient as a percentage.
    double getGradient() const;
    /// @brief Returns the segment's starting distance along the route in kilometers.
    double getStartKilometer() const;

    /**
     * @brief Calculates and stores the distance using vertices from a lookup map.
     * @param vertList Vertices indexed by ID.
     * @details If either vertex ID is absent, the stored length is set to zero and
     * an error is written to standard error.
     */
    void setLength(const std::map<uint32_t, Vertex>& vertList);
    /**
     * @brief Calculates and stores the signed elevation gradient.
     * @param vertList Vertices indexed by ID.
     * @details A zero-length segment or missing vertex IDs produce a zero gradient.
     */
    void setGradient(const std::map<uint32_t, Vertex>& vertList);
    /**
     * @brief Sets the segment's start distance along the route in kilometers.
     * @param kilometer Distance from the beginning of the route.
     */
    void setStartKilometer(const double kilometer);
};