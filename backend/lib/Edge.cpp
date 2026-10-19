#include "Edge.hpp"

Edge::Edge(const uint32_t stId, const uint32_t enId) 
    :   startId(stId), endId(enId), length(0.0), 
        startKilometer(0.0), gradient(0.0) {}

Edge::~Edge() {
    //cout << "Edge deleted" << endl;
}

double Edge::calculateDistance(const Vertex& v1, const Vertex& v2) const {
    // Haversine for 2D distance (meters)
    constexpr double R = 6371000.0;  // Earth radius (m)
    const double dLat = (v2.getLat() - v1.getLat()) * M_PI / 180.0;
    const double dLon = (v2.getLong() - v1.getLong()) * M_PI / 180.0;
    const double lat1 = v1.getLat() * M_PI / 180.0;
    
    const double haversine = sin(dLat / 2.0) * sin(dLat / 2.0) +
                             cos(lat1) * cos(lat1 + dLat) *
                             sin(dLon / 2.0) * sin(dLon / 2.0);
    const double clampedHaversine = std::clamp(haversine, 0.0, 1.0);
    const double groundDist = 2.0 * R * asin(sqrt(clampedHaversine));
    
    // 3D: Pythagorean with altitude diff
    const double altDiff = v2.getAlt() - v1.getAlt();
    return std::sqrt(groundDist * groundDist + altDiff * altDiff);
}

uint32_t Edge::getStartId() const { return startId; }
uint32_t Edge::getEndId() const { return endId; }
double Edge::getLength() const { return length; }
double Edge::getGradient() const { return gradient; }
double Edge::getStartKilometer() const { return startKilometer; }

void Edge::setLength(const std::map<uint32_t, Vertex>& vertList) { 
    try {
        const Vertex& startVertex = vertList.at(getStartId());
        const Vertex& endVertex = vertList.at(getEndId());

        length = calculateDistance(startVertex, endVertex);
    }
    catch (const std::out_of_range& e) {
        length = 0.0;
        std::cerr << "Error in Edge::setLength: Invalid Vertex ID" << std::endl;
    }
}

void Edge::setGradient(const std::map<uint32_t, Vertex>& vertList) { 
    try {
        const Vertex& startVertex = vertList.at(getStartId());
        const Vertex& endVertex = vertList.at(getEndId());
        if(length > 0.0) {
            gradient = (endVertex.getAlt() - startVertex.getAlt()) / 
                        length * 100; // elevation / distance * 100
        } else {
            gradient = 0.0;
        }
    }
    catch (const std::out_of_range& e) {
        gradient = 0.0;
        std::cerr << "Error in Edge::setLength: Invalid Vertex ID" << std::endl;
    }
}

void Edge::setStartKilometer(const double kilometer) { startKilometer = kilometer; }