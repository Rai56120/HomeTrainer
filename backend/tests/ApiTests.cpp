#include <doctest/doctest.h>

#include <chrono>
#include <memory>
#include <thread>

#include <httplib.h>
#include <nlohmann/json.hpp>

#include "ApiServer.hpp"
#include "Bike.hpp"
#include "Route.hpp"
#include "Simulation.hpp"
#include "User.hpp"

namespace {

/** @brief Creates a user with the standard test bike for API integration tests. */
User createApiUser() {
    User user("API user", 60.0, 170, "MALE", 0.40);
    user.setBike(std::make_shared<Bike>(
        "Test bike", 7.54, 0.88, 0.03, 1.22601, 9.80665, 0.005));
    return user;
}

}

/** @brief Verifies that simulation snapshots serialize with stable API field names. */
TEST_CASE("simulation state serializes to stable JSON fields") {
    const SimulationState state{25.0, 250.0, 2.5, 1.2, 10.0, true, false};
    const nlohmann::json payload = serializeSimulationState(state);

    CHECK(payload.at("speedKmh") == doctest::Approx(25.0));
    CHECK(payload.at("powerWatts") == doctest::Approx(250.0));
    CHECK(payload.at("gradientPercent") == doctest::Approx(2.5));
    CHECK(payload.at("distanceKm") == doctest::Approx(1.2));
    CHECK(payload.at("routeDistanceKm") == doctest::Approx(10.0));
    CHECK(payload.at("running") == true);
    CHECK(payload.at("finished") == false);
}

/** @brief Verifies power-command parsing accepts valid values and rejects invalid bodies. */
TEST_CASE("power command validates JSON and bounds") {
    httplib::Request request;
    double watts = 0.0;
    std::string error;

    request.body = R"({"watts":250})";
    CHECK(parsePowerCommand(request, watts, error));
    CHECK(watts == doctest::Approx(250.0));

    request.body = R"({"watts":-1})";
    CHECK_FALSE(parsePowerCommand(request, watts, error));
    CHECK_FALSE(error.empty());

    request.body = R"({"power":250})";
    CHECK_FALSE(parsePowerCommand(request, watts, error));

    request.body = R"({"watts":1e999})";
    CHECK_FALSE(parsePowerCommand(request, watts, error));
    CHECK(error == "watts must be a finite non-negative number");

    request.body = "not json";
    CHECK_FALSE(parsePowerCommand(request, watts, error));
}

/** @brief Exercises the health, state, and power-control HTTP endpoints end to end. */
TEST_CASE("REST server exposes health, state, and power control") {
    Route route(std::string(HOME_TRAINER_TEST_FIXTURE_DIR) + "/flat-two-points.gpx");
    Simulation simulation(createApiUser(), route);
    simulation.setPower(175.0);
    ApiServer apiServer(simulation);
    std::thread serverThread([&apiServer] {
        apiServer.run("127.0.0.1", 18080);
    });

    httplib::Client client("127.0.0.1", 18080);
    httplib::Result health;
    for(int attempt = 0; attempt < 20 && !health; ++attempt) {
        health = client.Get("/api/health");
        if(!health) {
            std::this_thread::sleep_for(std::chrono::milliseconds(25));
        }
    }

    REQUIRE(health);
    REQUIRE(health->status == 200);
    CHECK(nlohmann::json::parse(health->body).at("status") == "ok");

    const auto state = client.Get("/api/state");
    REQUIRE(state);
    REQUIRE(state->status == 200);
    CHECK(nlohmann::json::parse(state->body).at("running") == false);

    const auto power = client.Post(
        "/api/control/power", R"({"watts":300})", "application/json");
    REQUIRE(power);
    REQUIRE(power->status == 200);
    CHECK(nlohmann::json::parse(power->body).at("powerWatts") == doctest::Approx(300.0));

    const auto invalid = client.Post(
        "/api/control/power", R"({"watts":-1})", "application/json");
    REQUIRE(invalid);
    CHECK(invalid->status == 400);

    const auto overflow = client.Post(
        "/api/control/power", R"({"watts":1e999})", "application/json");
    REQUIRE(overflow);
    CHECK(overflow->status == 400);
    CHECK(nlohmann::json::parse(overflow->body).at("error") ==
          "watts must be a finite non-negative number");

    const auto userUpdate = client.Post(
        "/api/config/user",
        R"({"name":"Updated rider","weightKg":72.5,"heightCm":182,"gender":"FEMALE","frontalAreaM2":0.44})",
        "application/json");
    REQUIRE(userUpdate);
    CHECK(userUpdate->status == 200);

    const auto invalidUser = client.Post(
        "/api/config/user",
        R"({"name":"Rider","weightKg":72.5,"heightCm":182.5,"gender":"FEMALE","frontalAreaM2":0.44})",
        "application/json");
    REQUIRE(invalidUser);
    CHECK(invalidUser->status == 400);

    const auto bikeUpdate = client.Post(
        "/api/config/bike",
        R"({"weightKg":9.1,"dragCoefficient":0.35,"drivetrainLossPercent":4,"airDensity":1.2,"rollingResistance":0.005,"headwindMs":-1.5})",
        "application/json");
    REQUIRE(bikeUpdate);
    CHECK(bikeUpdate->status == 200);

    const std::string gpx =
        "<gpx version=\"1.1\">\n"
        "<trk>\n"
        "<name>Uploaded route</name>\n"
        "<trkseg>\n"
        "<trkpt lat=\"48.000000\" lon=\"-2.000000\"><ele>10.0</ele></trkpt>\n"
        "<trkpt lat=\"48.002000\" lon=\"-2.000000\"><ele>10.0</ele></trkpt>\n"
        "</trkseg>\n"
        "</trk>\n"
        "</gpx>\n";
    const httplib::MultipartFormDataItems routeUpload = {
        {"file", gpx, "uploaded.gpx", "application/gpx+xml"}
    };
    const auto routeUpdate = client.Post("/api/route", routeUpload);
    REQUIRE(routeUpdate);
    REQUIRE(routeUpdate->status == 200);
    CHECK(nlohmann::json::parse(routeUpdate->body).at("routeDistanceKm").get<double>() > 0.2);

    const httplib::MultipartFormDataItems invalidRouteUpload = {
        {"file", "<gpx><trk></trk></gpx>", "empty.gpx", "application/gpx+xml"}
    };
    const auto invalidRoute = client.Post("/api/route", invalidRouteUpload);
    REQUIRE(invalidRoute);
    CHECK(invalidRoute->status == 400);

    const auto start = client.Post("/api/simulation/start", "", "application/json");
    REQUIRE(start);
    REQUIRE(start->status == 200);
    CHECK(nlohmann::json::parse(start->body).at("running") == true);
    const auto stopped = client.Post("/api/simulation/stop", "", "application/json");
    REQUIRE(stopped);
    REQUIRE(stopped->status == 200);
    const auto stoppedState = nlohmann::json::parse(stopped->body);
    CHECK(stoppedState.at("speedKmh") == doctest::Approx(0.0));
    CHECK(stoppedState.at("powerWatts") == doctest::Approx(0.0));
    CHECK(stoppedState.at("distanceKm") == doctest::Approx(0.0));

    const auto restart = client.Post("/api/simulation/start", "", "application/json");
    REQUIRE(restart);
    CHECK(nlohmann::json::parse(restart->body).at("running") == true);
    const auto stopAgain = client.Post("/api/simulation/stop", "", "application/json");
    REQUIRE(stopAgain);
    CHECK(stopAgain->status == 200);

    const auto reset = client.Post("/api/simulation/reset", "", "application/json");
    REQUIRE(reset);
    REQUIRE(reset->status == 200);
    CHECK(nlohmann::json::parse(reset->body).at("powerWatts") == doctest::Approx(0.0));

    apiServer.stop();
    serverThread.join();
}
