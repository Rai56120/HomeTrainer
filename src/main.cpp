#include <thread>
#include <chrono>
#include <mutex>
#include <atomic>
#include <iostream>
#include <memory>
#include <iomanip>
#include <limits>

#include "../lib/Route.hpp"
#include "../lib/User.hpp"

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

/*******************************************************/
/*                        MAIN                         */
/*******************************************************/
int main() {
    cout << fixed << setprecision(4);

    Route circuit("../ressources/ninian_bourg.gpx");
    std::shared_ptr<Bike> b = std::make_shared<Bike>(   "Colnago Y1RS", BIKE_WEIGHT, DRAG_COEFFICIENT, 
                                                        DRIVE_TRAIN_LOSSES, AIR_DENSITY, 
                                                        GRAVITY_CONSTANT, ROLLING_RESISTANCE_COEF);

    // User u = createUser();
    User u("Yoann", 60.0, 170, "MALE", FRONTAL_AREA);

    u.setBike(b);

    circuit.printRoute();
    u.printInfos();


    thread tDistance(updateDistanceTask, ref(u), cref(circuit));
    thread tSpeed(updateSpeedTask, ref(u));

    cout << MAIN_STRING << "Starting threads" << endl;

    if(tDistance.joinable()) { 
        cout<< MAIN_STRING << "thread tDistance started at id #" 
            << tDistance.get_id() << endl;
        
        tDistance.join();
    }
    if(tSpeed.joinable()) { 
        cout<< MAIN_STRING<< "thread tSpeed started at id #" 
            << tSpeed.get_id() << endl;
        
        tSpeed.join();
    }

    return EXIT_SUCCESS;
}

/*******************************************************/
/*                     FUNCTIONS                       */
/*******************************************************/
void updateDistanceTask(User& u, const Route& circuit) {
    auto nextTick = chrono::steady_clock::now();
    size_t currentEdgeId = 0;

    if(circuit.getAllSegments().empty()) {
        cerr << UPDATE_DISTANCE_STRING << "Route is empty" << endl;
        running.store(false);
        return;
    }

    Edge currentEdge = circuit.getEdge(currentEdgeId);

    while(running.load()) {
        nextTick += chrono::milliseconds(TICK_INTERVAL_MS);
        
        {
            lock_guard<mutex> lock(userMutex);
            
            //Calculate duration
            const double delta = u.getCurrentSpeed() * TIME_FACTOR;
            
            //Calculate coveredDistance
            u.addToCoveredDistance(delta);

            //Get current gradient and Edge
            if(u.getCoveredDistance() >= currentEdge.getStartKilometer() + (currentEdge.getLength() / 1000.0)) {
                currentEdgeId++;

                if(currentEdgeId < circuit.getAllSegments().size()) {
                    currentEdge = circuit.getEdge(currentEdgeId);
                    u.setCurrentGradient(currentEdge.getGradient());
                    cout << UPDATE_DISTANCE_STRING << "Gradient: " << u.getCurrentGradient() << "%" << endl;
                } 
                else if(u.getCoveredDistance() >= circuit.getTotalDistance()){
                    running.store(false);
                    cout << UPDATE_DISTANCE_STRING << "Circuit finished !" << endl;
                }
            }

            cout << UPDATE_DISTANCE_STRING << "Covered distance: " << u.getCoveredDistance() << "km" << endl;
        }
        //attendre 500ms
        this_thread::sleep_until(nextTick);
    }
}

void updateSpeedTask(User& u) {
    double power = CURRENT_POWER;

    while(running.load()) {
        this_thread::sleep_for(chrono::milliseconds(TICK_INTERVAL_MS));
        
        {
            lock_guard<mutex> lock(userMutex);
            u.setCurrentSpeed(power);

            cout << UPDATE_SPEED_STRING << "Speed: " << u.getCurrentSpeed() << " km/h" << endl;
        }
    }
}

User createUser() {
    string name;
    uint16_t weight;
    uint16_t height;
    int gender;

    do {
        cout << "Enter your name: ";
        getline(cin, name);
    } while(name.empty());
    do {
        cout << "Enter your weight (in kg): ";
        cin >> weight;
    } while(weight < 1 || weight > 730);
    do {
        cout << "Enter your height (in cm): ";
        cin >> height;
    } while(height < 50 || height > 300);
    do {
        cout<< "Enter your gender:" << endl
            << "0. Male" << endl
            << "1. Female" << endl;
        cin >> gender;
    } while (gender != 0 && gender != 1);

    return User(name, weight, height, gender == 1 ? "FEMALE": "MALE", FRONTAL_AREA); 
}