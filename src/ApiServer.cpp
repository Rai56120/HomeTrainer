#include "ApiServer.hpp"

#include <cmath>

namespace {

constexpr const char* jsonContentType = "application/json";

/**
 * @brief Serializes a JSON value into an HTTP response with the JSON content type.
 * @param response HTTP response to populate.
 * @param value JSON value to serialize.
 */
void setJson(httplib::Response& response, const nlohmann::json& value) {
    response.set_content(value.dump(), jsonContentType);
}

}

nlohmann::json serializeSimulationState(const SimulationState& state) {
    return {
        {"speedKmh", state.speedKmh},
        {"powerWatts", state.powerWatts},
        {"gradientPercent", state.gradientPercent},
        {"distanceKm", state.distanceKm},
        {"routeDistanceKm", state.routeDistanceKm},
        {"running", state.running},
        {"finished", state.finished}
    };
}

bool parsePowerCommand(const httplib::Request& request, double& watts, std::string& error) {
    try {
        const auto payload = nlohmann::json::parse(request.body);
        if(!payload.is_object() || !payload.contains("watts") ||
           !payload.at("watts").is_number()) {
            error = "watts must be a number";
            return false;
        }

        watts = payload.at("watts").get<double>();
        if(!std::isfinite(watts) || watts < 0.0) {
            error = "watts must be a finite non-negative number";
            return false;
        }
        return true;
    } catch(const std::exception&) {
        error = "request body must be valid JSON";
        return false;
    }
}

ApiServer::ApiServer(Simulation& simulationValue)
    : simulation(simulationValue) {}

void ApiServer::run(const std::string& host, const int port) {
    configureRoutes();
    server.listen(host, port);
}

void ApiServer::stop() {
    server.stop();
}

void ApiServer::configureRoutes() {
    server.Get("/api/health", [](const httplib::Request&, httplib::Response& response) {
        setJson(response, {{"status", "ok"}, {"apiVersion", 1}});
    });

    server.Get("/api/state", [this](const httplib::Request&, httplib::Response& response) {
        respondWithState(response);
    });

    server.Post("/api/control/power", [this](const httplib::Request& request,
                                               httplib::Response& response) {
        double watts = 0.0;
        std::string error;
        if(!parsePowerCommand(request, watts, error)) {
            respondWithError(response, 400, error);
            return;
        }
        simulation.setPower(watts);
        respondWithState(response);
    });

    server.Post("/api/simulation/start", [this](const httplib::Request&, httplib::Response& response) {
        simulation.start();
        respondWithState(response);
    });

    server.Post("/api/simulation/pause", [this](const httplib::Request&, httplib::Response& response) {
        simulation.pause();
        respondWithState(response);
    });

    server.Post("/api/simulation/stop", [this](const httplib::Request&, httplib::Response& response) {
        simulation.stop();
        respondWithState(response);
    });

    server.Post("/api/simulation/reset", [this](const httplib::Request&, httplib::Response& response) {
        simulation.reset();
        respondWithState(response);
    });
}

void ApiServer::respondWithState(httplib::Response& response) const {
    setJson(response, serializeSimulationState(simulation.getState()));
}

void ApiServer::respondWithError(httplib::Response& response,
                                 const int status,
                                 const std::string& message) {
    response.status = status;
    setJson(response, {{"error", message}});
}
