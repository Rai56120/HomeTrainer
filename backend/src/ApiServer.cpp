#include "ApiServer.hpp"

#include <cmath>
#include <cstdint>
#include <sstream>

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

bool readJsonObject(const httplib::Request& request,
                    nlohmann::json& payload,
                    std::string& error) {
    try {
        payload = nlohmann::json::parse(request.body);
    } catch(const std::exception&) {
        error = "request body must be valid JSON";
        return false;
    }
    if(!payload.is_object()) {
        error = "request body must be a JSON object";
        return false;
    }
    return true;
}

bool readNumber(const nlohmann::json& payload,
                const char* key,
                const double minimum,
                const double maximum,
                double& value) {
    if(!payload.contains(key) || !payload.at(key).is_number()) {
        return false;
    }
    value = payload.at(key).get<double>();
    return std::isfinite(value) && value >= minimum && value <= maximum;
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
    } catch(const nlohmann::json::out_of_range&) {
        error = "watts must be a finite non-negative number";
        return false;
    } catch(const std::exception&) {
        error = "request body must be valid JSON";
        return false;
    }
}

ApiServer::ApiServer(Simulation& simulationValue)
    : simulation(simulationValue) {
    server.set_payload_max_length(5 * 1024 * 1024);
}

void ApiServer::run(const std::string& host, const int port) {
    configureRoutes();
    std::cout << "[BACKEND] Server run at " << host << ":" << port << std::endl;
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

    server.Post("/api/route", [this](const httplib::Request& request,
                                      httplib::Response& response) {
        if(!request.has_file("file")) {
            respondWithError(response, 400, "multipart field 'file' is required");
            return;
        }
        const auto uploadedFile = request.get_file_value("file");
        if(uploadedFile.content.empty()) {
            respondWithError(response, 400, "uploaded GPX file is empty");
            return;
        }

        std::istringstream gpxStream(uploadedFile.content);
        Route route(gpxStream);
        if(route.getAllSegments().empty()) {
            respondWithError(response, 400, "uploaded GPX file must contain at least two track points");
            return;
        }
        simulation.setRoute(std::move(route));
        respondWithState(response);
    });

    server.Post("/api/config/user", [this](const httplib::Request& request,
                                            httplib::Response& response) {
        nlohmann::json payload;
        std::string error;
        if(!readJsonObject(request, payload, error)) {
            respondWithError(response, 400, error);
            return;
        }

        double weightKg = 0.0;
        double heightCm = 0.0;
        double frontalAreaM2 = 0.0;
        if(!payload.contains("name") || !payload.at("name").is_string() ||
           payload.at("name").get<std::string>().empty() ||
           payload.at("name").get<std::string>().size() > 80 ||
           !readNumber(payload, "weightKg", 1.0, 300.0, weightKg) ||
           !readNumber(payload, "heightCm", 50.0, 250.0, heightCm) ||
           std::floor(heightCm) != heightCm ||
           !readNumber(payload, "frontalAreaM2", 0.0, 2.0, frontalAreaM2) ||
           !payload.contains("gender") || !payload.at("gender").is_string()) {
            respondWithError(response, 400, "invalid user profile parameters");
            return;
        }

        const std::string genderText = payload.at("gender").get<std::string>();
        if(genderText != "MALE" && genderText != "FEMALE") {
            respondWithError(response, 400, "gender must be MALE or FEMALE");
            return;
        }
        simulation.setUserProfile(
            payload.at("name").get<std::string>(), weightKg,
            static_cast<uint16_t>(heightCm),
            genderText == "MALE" ? MALE : FEMALE, frontalAreaM2);
        respondWithState(response);
    });

    server.Post("/api/config/bike", [this](const httplib::Request& request,
                                            httplib::Response& response) {
        nlohmann::json payload;
        std::string error;
        if(!readJsonObject(request, payload, error)) {
            respondWithError(response, 400, error);
            return;
        }

        double weightKg = 0.0;
        double dragCoefficient = 0.0;
        double drivetrainLossPercent = 0.0;
        double airDensity = 0.0;
        double rollingResistance = 0.0;
        double headwindMs = 0.0;
        if(!readNumber(payload, "weightKg", 0.0, 50.0, weightKg) ||
           !readNumber(payload, "dragCoefficient", 0.0, 1.5, dragCoefficient) ||
           !readNumber(payload, "drivetrainLossPercent", 0.0, 100.0,
                       drivetrainLossPercent) ||
           !readNumber(payload, "airDensity", 0.0, 10.0, airDensity) ||
           !readNumber(payload, "rollingResistance", 0.0, 0.5, rollingResistance) ||
           !readNumber(payload, "headwindMs", -100.0, 100.0, headwindMs)) {
            respondWithError(response, 400, "invalid bike parameters");
            return;
        }
        simulation.setBikeParameters(weightKg, dragCoefficient, drivetrainLossPercent,
                                     airDensity, rollingResistance, headwindMs);
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
        simulation.setPower(0);
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
