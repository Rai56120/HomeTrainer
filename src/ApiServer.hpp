#pragma once

#include <string>

#include <httplib.h>
#include <nlohmann/json.hpp>

#include "Simulation.hpp"

/**
 * @brief Converts a simulation snapshot to the JSON fields returned by the API.
 * @param state Simulation state to serialize.
 * @return JSON object containing speed, power, gradient, distance, running, and finished values.
 */
nlohmann::json serializeSimulationState(const SimulationState& state);
/**
 * @brief Parses and validates a JSON power command from an HTTP request.
 * @param request Request whose body must contain a numeric "watts" field.
 * @param watts Receives the validated, finite, non-negative power value on success.
 * @param error Receives a validation message on failure.
 * @return True when the request contains a valid power command; otherwise false.
 */
bool parsePowerCommand(const httplib::Request& request, double& watts, std::string& error);

/** @brief Exposes simulation state and controls through a small HTTP JSON API. */
class ApiServer {
public:
    /**
     * @brief Creates an HTTP API server bound to a simulation.
     * @param simulation Simulation controlled and queried by this server.
     */
    explicit ApiServer(Simulation& simulation);

    /**
     * @brief Registers API routes and listens on the requested host and port.
     * @param host Network address on which to listen.
     * @param port TCP port on which to listen.
     */
    void run(const std::string& host, int port);
    /** @brief Stops the HTTP listener. */
    void stop();

private:
    /** @brief Registers health, state, power-control, and simulation-control endpoints. */
    void configureRoutes();
    /**
     * @brief Writes the current simulation snapshot as a JSON HTTP response.
     * @param response HTTP response to populate.
     */
    void respondWithState(httplib::Response& response) const;
    /**
     * @brief Writes a JSON error response with the specified HTTP status and message.
     * @param response HTTP response to populate.
     * @param status HTTP status code.
     * @param message Error text returned in the response body.
     */
    static void respondWithError(httplib::Response& response, int status, const std::string& message);

    Simulation& simulation;
    httplib::Server server;
};
