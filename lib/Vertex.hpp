#pragma once

#include <cstdint>

class Vertex {
private:
    uint32_t id;
    double latitude;
    double longitude;
    double altitude;
public:
    /**
     * @brief Creates a vertex with an ID and geographic/elevation coordinates.
     * @param identifier Vertex ID.
     * @param lat Latitude in degrees.
     * @param lon Longitude in degrees.
     * @param alt Elevation in meters.
     */
    Vertex( const uint32_t identifier, 
            const double lat, const double lon, 
            const double alt);
    /** @brief Creates a vertex with zero-initialized coordinate data. */
    Vertex() {}
    /** @brief Destroys the vertex. */
    ~Vertex();

    /// @brief Returns latitude in degrees.
    double getLat() const;
    /// @brief Returns longitude in degrees.
    double getLong() const;
    /// @brief Returns elevation in meters.
    double getAlt() const;
    /// @brief Returns the vertex ID.
    uint32_t getId() const;
};

