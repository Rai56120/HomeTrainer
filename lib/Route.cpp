#include "Route.hpp"

Route::Route(const std::string& file) {
    totalDistance = 0.0;
    elevationPlus = 0.0;
    elevationMinus = 0.0;

    setFromGPX(file);
}

Route::~Route() {
    // std::cout << "Route deleted" << std::endl;
}

std::string Route::extractSubstring(const std::string& baseString, 
                                    const std::string& firstDelim, 
                                    const std::string& secondDelim,
                                    size_t& searchPos) {
    
    auto delim_1 = baseString.find(firstDelim, searchPos);
    if(delim_1 == std::string::npos) { return ""; }

    auto startPos = delim_1 + firstDelim.size();
    auto delim_2 = baseString.find(secondDelim, startPos);
    if(delim_2 == std::string::npos) { return ""; }

    searchPos = delim_2 + secondDelim.size();

    std::string substring = baseString.substr(startPos, delim_2 - startPos);

    return substring;
}

std::vector<double> Route::extractCoordinates(const std::string& point) {
    double lat = 0.0;
    double lon = 0.0;
    double alt = 0.0;
    size_t pos = 0;

    try {
        std::string latStr = extractSubstring(point, "lat=\"", "\"", pos);
        if(!latStr.empty()) { lat = std::stod(latStr); }

        std::string lonStr = extractSubstring(point, " lon=\"", "\"", pos);
        if(!lonStr.empty()) { lon = std::stod(lonStr); }

        pos = 0;
        std::string altStr = extractSubstring(point, "<ele>", "</ele>", pos);
        if(!altStr.empty()) { alt = std::stod(altStr); }
    } 
    catch (const std::exception& e) {
        std::cerr << "Error parsing coordinates: " << e.what() << std::endl;
    }

    return {lat, lon, alt};
}

void Route::calculateElevation(const Edge& e) {
    try {
        const Vertex& startVertex = vertList.at(e.getStartId());
        const Vertex& endVertex = vertList.at(e.getEndId());

        double elevation = endVertex.getAlt() - startVertex.getAlt();

        if(elevation >= 0) { elevationPlus += elevation; }
        else { elevationMinus += elevation; }
    }
    catch (const std::out_of_range& err) {
        std::cerr << "Vertex ID out of bounds during elevation calculation" << std::endl;
    }
}

void Route::setFromGPX(const std::string& file) {
    std::ifstream myGPX(file, std::ifstream::in);

    if(!myGPX.is_open()) {
        std::cerr << "Error: cannot open file: " << file << std::endl;
        return;
    }

    std::string line;
    std::string rte;
    rte.reserve(1024);
    bool rteFlag = false;

    while(getline(myGPX, line)) {
        if      (line.find("<trk>") != std::string::npos)  { rteFlag = true; continue; }
        else if (line.find("</trk>") != std::string::npos) { break; }
        else if (rteFlag)                             { rte.append(line); rte.push_back('\n'); }
    }
    
    myGPX.close();

    size_t searchPos = 0;
    name = extractSubstring(rte, "<name>", "</name>", searchPos);
    searchPos = 0;
    description = extractSubstring(rte, "<desc>", "</desc>", searchPos);

    uint32_t currentId = 0;
    searchPos = 0;

    while(true) {
        std::string point = extractSubstring(rte, "<trkpt", "</trkpt>", searchPos);
        if(point.empty()) { break; }

        std::vector<double> coordinates = extractCoordinates(point);
        Vertex v(currentId, coordinates[0], coordinates[1], coordinates[2]);
        
        vertList.insert({ currentId, v });
        ++currentId;
    }

    if(vertList.size() < 2) { return; }
    
    for(uint32_t i = 0; i < vertList.size() - 1; ++i) {
        Edge e(i, i + 1);

        e.setLength(vertList);
        e.setGradient(vertList);
        e.setStartKilometer(totalDistance);

        totalDistance += e.getLength() / 1000.0;

        calculateElevation(e);

        edgeList.push_back(e);
    }

    // edgeList.pop_back();
}

const std::vector<Edge>& Route::getAllSegments() const {
    return edgeList;
}

double Route::getTotalDistance() const {
    return totalDistance;
}

const Edge& Route::getEdge(const size_t id) const {
    return edgeList.at(id);
}

void Route::printRoute() const {
    std::cout<< "********************" << std::endl
        << "Name: " << name << std::endl
        << "Description: " << description << std::endl    
        << "Distance: " << std::setprecision(4) << totalDistance << "km" << std::endl
        << "D+: " << elevationPlus << "m" << std::endl
        << "D-: " << elevationMinus << "m" << std::endl
        << "********************" << std::endl;
}

void Route::printEdges() const {
    int i = 0;

    for(const auto& e: edgeList) {
        std::cout<< "Edge #" << i << std::endl
            << "startId: " << std::setprecision(4) << e.getStartId() << std::endl
            << "endId: " << e.getEndId() << std::endl
            << "startMeter: " << e.getStartKilometer() << std::endl
            << "length: " << e.getLength() << "m" << std::endl
            << "gradient: " << e.getGradient() << "%" << std::endl;
        
        ++i;
    }
}