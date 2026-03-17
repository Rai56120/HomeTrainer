#pragma once

#include <cstdlib>
#include <string>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <map>
#include <vector>

#include "Edge.hpp"
#include "Vertex.hpp"

class Route {
private:
    std::map<uint32_t, Vertex> vertList;
    std::vector<Edge> edgeList;

    std::string name;
    std::string description;

    double elevationPlus;
    double elevationMinus;
    double totalDistance;

    std::string extractSubstring(   const std::string& baseString, 
                                    const std::string& firstDelim, 
                                    const std::string& secondDelim,
                                    size_t& searchPos);

    std::vector<double> extractCoordinates(const std::string& point);

    void calculateElevation(const Edge& e);
    void setFromGPX(const std::string& file);

public:
    //Constructor
    Route(const std::string& file);
    //Destructor
    ~Route();

    //Getters
    const std::vector<Edge>& getAllSegments() const;
    const Edge& getEdge(const size_t id) const;
    double getTotalDistance() const;

    void printRoute() const;
    void printEdges() const;
};