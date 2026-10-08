#pragma once

#include <condition_variable>
#include <cstddef>
#include <mutex>
#include <thread>

#include "Route.hpp"
#include "User.hpp"

struct SimulationState {
    /// Current rider speed in kilometers per hour.
    double speedKmh;
    /// Current rider power in watts.
    double powerWatts;
    /// Current road gradient as a percentage.
    double gradientPercent;
    /// Distance traveled along the route in kilometers.
    double distanceKm;
    /// Total route length in kilometers.
    double routeDistanceKm;
    /// True when the worker is active and the simulation is not paused or stopped.
    bool running;
    /// True when the rider has reached the end of the route.
    bool finished;
};

/** @brief Controls rider progress along a route with optional threaded updates. */
class Simulation {
public:
    /**
     * @brief Creates a simulation for a user and route.
     * @param user Initial user state, including any assigned bike.
     * @param route Route whose segments determine gradient and completion.
     */
    Simulation(User user, const Route& route);
    /** @brief Stops and joins the simulation worker before destruction. */
    ~Simulation();

    Simulation(const Simulation&) = delete;
    Simulation& operator=(const Simulation&) = delete;

    /**
     * @brief Sets rider power in watts; negative values are clamped to zero.
     * @param watts Requested rider power.
     */
    void setPower(double watts);
    /**
     * @brief Advances simulation state by a fixed time interval.
     * @param deltaSeconds Elapsed time in seconds; non-finite and non-positive values are ignored.
     */
    void tick(double deltaSeconds);
    /** @brief Resumes the simulation and starts its worker thread if needed. */
    void start();
    /** @brief Pauses simulation updates without terminating the worker thread. */
    void pause();
    /** @brief Requests that the worker stop and wakes it if it is paused. */
    void stop();
    /** @brief Restores the initial user, route position, and paused state. */
    void reset();
    /** @brief Waits for the worker thread to exit, if one was started. */
    void join();

    /** @brief Returns a thread-safe snapshot of the current simulation state. */
    SimulationState getState() const;

private:
    /** @brief Runs timed simulation updates until stop is requested. */
    void run();
    /** @brief Updates the current route segment and marks the simulation finished at the end. */
    void advanceRoute();

    User user;
    User initialUser;
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
