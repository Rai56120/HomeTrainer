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

/** @brief Loads GPX track geometry and provides ordered route segments and distance. */
class Route {
private:
    std::map<uint32_t, Vertex> vertList;
    std::vector<Edge> edgeList;

    std::string name;
    std::string description;

    double elevationPlus;
    double elevationMinus;
    double totalDistance;

    /**
     * @brief Extracts text between delimiters and advances the search position.
     * @param baseString Text to search.
     * @param firstDelim Opening delimiter.
     * @param secondDelim Closing delimiter.
     * @param searchPos Search offset; updated to the end of the extracted text.
     * @return The text between the delimiters, or an empty string if not found.
     */
    std::string extractSubstring(   const std::string& baseString, 
                                    const std::string& firstDelim, 
                                    const std::string& secondDelim,
                                    size_t& searchPos);

    /**
     * @brief Reads latitude, longitude, and elevation values from a GPX track point.
     * @param point Track-point XML text.
     * @return Coordinate values in the order latitude, longitude, and meters altitude.
     * @details Invalid or missing values remain zero; parsing errors are reported to
     * standard error.
     */
    std::vector<double> extractCoordinates(const std::string& point);

    /**
     * @brief Adds this segment's elevation change to the route's ascent or descent.
     * @param e Segment whose endpoint elevations determine the change.
     */
    void calculateElevation(const Edge& e);
    /**
     * @brief Loads track metadata and creates route vertices and segments from a GPX file.
     * @param file Path to the GPX file.
     */
    void setFromGPX(const std::string& file);

public:
    /**
     * @brief Loads a route from a GPX file.
     * @param file Path to the GPX file.
     * @details If the file cannot be opened or contains fewer than two track points,
     * the route has no segments.
     */
    Route(const std::string& file);
    /** @brief Destroys the route and its stored geometry. */
    ~Route();

    /// @brief Returns all route segments in traversal order.
    const std::vector<Edge>& getAllSegments() const;
    /**
     * @brief Returns a route segment by its zero-based index.
     * @param id Zero-based segment index.
     * @return The requested segment.
     * @throws std::out_of_range if the index is outside the route.
     */
    const Edge& getEdge(const size_t id) const;
    /// @brief Returns the total route distance in kilometers.
    double getTotalDistance() const;

    /** @brief Prints route name, description, distance, ascent, and descent to standard output. */
    void printRoute() const;
    /** @brief Prints each segment's geometry and gradient to standard output. */
    void printEdges() const;
};