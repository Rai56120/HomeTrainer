#pragma once

#include <cmath>
#include <map>
#include <cstdint>
#include <stdexcept>
#include <iostream>

#include "Vertex.hpp"

class Edge {
private:
    uint32_t startId;
    uint32_t endId;
    double length;
    double startKilometer;
    double gradient;

    double calculateDistance(const Vertex& v1, const Vertex& v2) const;
public:
    Edge(const uint32_t stId, const uint32_t enId);
    ~Edge();

    uint32_t getStartId() const;
    uint32_t getEndId() const;
    double getLength() const;
    double getGradient() const;
    double getStartKilometer() const;

    void setLength(const std::map<uint32_t, Vertex>& vertList);
    void setGradient(const std::map<uint32_t, Vertex>& vertList);
    void setStartKilometer(const double kilometer);
};