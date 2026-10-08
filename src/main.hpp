#include <iostream>
#include <memory>
#include <iomanip>

#include "Route.hpp"
#include "ApiServer.hpp"
#include "Simulation.hpp"
#include "User.hpp"

using namespace std;

/*******************************************************/
/*                  GLOBAL CONSTANTS                   */
/*******************************************************/
static constexpr double     BIKE_WEIGHT             = 7.54;
static constexpr double     DRAG_COEFFICIENT        = 0.88;
static constexpr double     FRONTAL_AREA            = 0.40;
static constexpr double     DRIVE_TRAIN_LOSSES      = 0.03;
static constexpr double     AIR_DENSITY             = 1.22601;
static constexpr double     GRAVITY_CONSTANT        = 9.80665;
static constexpr double     ROLLING_RESISTANCE_COEF = 0.005;

static constexpr uint32_t   CURRENT_POWER = 500U;
User createUser();