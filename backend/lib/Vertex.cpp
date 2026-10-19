#include "Vertex.hpp"

Vertex::Vertex(const uint32_t identifier, const double lat, const double lon, const double alt) 
    : id(identifier), latitude(lat), longitude(lon), altitude(alt) {}

Vertex::~Vertex() {
    // cout << "Vertex #" << id << " deleted" << endl;
}

double Vertex::getLat() const { return latitude; }

double Vertex::getLong() const { return longitude; }

double Vertex::getAlt() const { return altitude; }

uint32_t Vertex::getId() const { return id;}