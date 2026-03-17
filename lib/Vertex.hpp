#pragma once

#include <cstdint>

class Vertex {
private:
    uint32_t id;
    double latitude;
    double longitude;
    double altitude;
public:
    Vertex( const uint32_t identifier, 
            const double lat, const double lon, 
            const double alt);
    Vertex() {}
    ~Vertex();

    double getLat() const;
    double getLong() const;
    double getAlt() const;
    uint32_t getId() const;
};

