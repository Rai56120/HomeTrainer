#include "Simulation.hpp"

#include <algorithm>
#include <chrono>
#include <cmath>

Simulation::Simulation(User userValue, const Route& routeValue)
    : user(userValue), initialUser(std::move(userValue)), route(routeValue) {
    if(route.getAllSegments().empty()) {
        finished = true;
    } else {
        user.setCurrentGradient(route.getEdge(0).getGradient());
    }
}

Simulation::~Simulation() {
    stop();
    join();
}

void Simulation::setPower(const double watts) {
    std::lock_guard<std::mutex> lock(mutex);
    powerWatts = std::max(0.0, watts);
}

void Simulation::tick(const double deltaSeconds) {
    if(!std::isfinite(deltaSeconds) || deltaSeconds <= 0.0) {
        return;
    }

    std::lock_guard<std::mutex> lock(mutex);
    if(finished) {
        return;
    }

    user.setCurrentSpeed(powerWatts);
    const double distanceKm = user.getCurrentSpeed() * deltaSeconds / 3600.0;
    const double remainingDistance = route.getTotalDistance() - user.getCoveredDistance();
    user.addToCoveredDistance(std::clamp(distanceKm, 0.0, remainingDistance));
    advanceRoute();
}

void Simulation::start() {
    std::lock_guard<std::mutex> lock(mutex);
    if(finished) {
        return;
    }

    paused = false;
    if(!worker.joinable()) {
        stopRequested = false;
        worker = std::thread(&Simulation::run, this);
    }
    condition.notify_all();
}

void Simulation::pause() {
    std::lock_guard<std::mutex> lock(mutex);
    paused = true;
}

void Simulation::stop() {
    {
        std::lock_guard<std::mutex> lock(mutex);
        stopRequested = true;
        paused = false;
    }
    condition.notify_all();
}

void Simulation::reset() {
    std::lock_guard<std::mutex> lock(mutex);
    user = initialUser;
    currentEdgeId = 0;
    finished = route.getAllSegments().empty();
    paused = true;
    stopRequested = false;

    if(!finished) {
        user.setCurrentGradient(route.getEdge(0).getGradient());
    }
}

void Simulation::join() {
    if(worker.joinable()) {
        worker.join();
    }
}

SimulationState Simulation::getState() const {
    std::lock_guard<std::mutex> lock(mutex);
    return {
        user.getCurrentSpeed(),
        powerWatts,
        user.getCurrentGradient(),
        user.getCoveredDistance(),
        route.getTotalDistance(),
        !paused && !finished && !stopRequested,
        finished
    };
}

void Simulation::run() {
    using Clock = std::chrono::steady_clock;
    auto previous = Clock::now();

    while(true) {
        {
            std::unique_lock<std::mutex> lock(mutex);
            condition.wait(lock, [this] {
                return stopRequested || !paused;
            });
            if(stopRequested) {
                return;
            }
        }

        const auto now = Clock::now();
        const double deltaSeconds =
            std::chrono::duration<double>(now - previous).count();
        previous = now;
        tick(deltaSeconds);
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

void Simulation::advanceRoute() {
    const auto& segments = route.getAllSegments();
    while(currentEdgeId + 1 < segments.size() &&
          user.getCoveredDistance() >= segments[currentEdgeId].getStartKilometer() +
          segments[currentEdgeId].getLength() / 1000.0) {
        ++currentEdgeId;
        user.setCurrentGradient(segments[currentEdgeId].getGradient());
    }

    if(user.getCoveredDistance() >= route.getTotalDistance()) {
        finished = true;
        paused = true;
    }
}
