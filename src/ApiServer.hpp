#pragma once

#include <string>

#include <httplib.h>
#include <nlohmann/json.hpp>

#include "Simulation.hpp"

nlohmann::json serializeSimulationState(const SimulationState& state);
bool parsePowerCommand(const httplib::Request& request, double& watts, std::string& error);

class ApiServer {
public:
    explicit ApiServer(Simulation& simulation);

    void run(const std::string& host, int port);
    void stop();

private:
    void configureRoutes();
    void respondWithState(httplib::Response& response) const;
    static void respondWithError(httplib::Response& response, int status, const std::string& message);

    Simulation& simulation;
    httplib::Server server;
};
