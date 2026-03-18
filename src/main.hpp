#include <thread>
#include <chrono>
#include <mutex>
#include <atomic>
#include <iostream>
#include <memory>
#include <iomanip>
#include <limits>

#include "Route.hpp"
#include "User.hpp"

using namespace std;

/*******************************************************/
/*                  GLOBAL CONSTANTS                   */
/*******************************************************/
static constexpr uint32_t TICK_INTERVAL_MS = 500U;
static constexpr double   MS_PER_HOUR      = 3'600'000.0;
static constexpr double   TIME_FACTOR      = TICK_INTERVAL_MS / MS_PER_HOUR;

static constexpr char UPDATE_DISTANCE_STRING[] = "[UPDATE DISTANCE TASK] ";
static constexpr char UPDATE_SPEED_STRING[] = "[UPDATE SPEED TASK] ";
static constexpr char MAIN_STRING[] = "[MAIN] ";

static constexpr double     BIKE_WEIGHT             = 7.54;
static constexpr double     DRAG_COEFFICIENT        = 0.88;
static constexpr double     FRONTAL_AREA            = 0.40;
static constexpr double     DRIVE_TRAIN_LOSSES      = 0.03;
static constexpr double     AIR_DENSITY             = 1.22601;
static constexpr double     GRAVITY_CONSTANT        = 9.80665;
static constexpr double     ROLLING_RESISTANCE_COEF = 0.005;

static constexpr uint32_t   CURRENT_POWER = 500000U;

/*******************************************************/
/*                   GLOBAL VARIABLES                  */
/*******************************************************/
mutex userMutex;
atomic<bool> running = true;


/*******************************************************/
/*                FUNCTIONS PROTOTYPES                 */
/*******************************************************/
void updateDistanceTask(User& u, const Route& circuit);
void updateSpeedTask(User& u);
User createUser();