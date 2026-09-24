#pragma once

#include <condition_variable>
#include <cstddef>
#include <mutex>
#include <thread>

#include "Route.hpp"
#include "User.hpp"

struct SimulationState {
    double speedKmh;
    double powerWatts;
    double gradientPercent;
    double distanceKm;
    double routeDistanceKm;
    bool running;
    bool finished;
};

class Simulation {
public:
    Simulation(User user, const Route& route);
    ~Simulation();

    Simulation(const Simulation&) = delete;
    Simulation& operator=(const Simulation&) = delete;

    void setPower(double watts);
    void tick(double deltaSeconds);
    void start();
    void pause();
    void stop();
    void join();

    SimulationState getState() const;

private:
    void run();
    void advanceRoute();

    User user;
    const Route& route;
    double powerWatts = 0.0;
    std::size_t currentEdgeId = 0;
    bool paused = true;
    bool stopRequested = false;
    bool finished = false;

    mutable std::mutex mutex;
    std::condition_variable condition;
    std::thread worker;
};
