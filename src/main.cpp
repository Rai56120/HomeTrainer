#include "main.hpp"

int main() {
    cout << fixed << setprecision(4);

    Route circuit("./resources/ninian_bourg.gpx");
    std::shared_ptr<Bike> bike = std::make_shared<Bike>(
        "Colnago Y1RS", BIKE_WEIGHT, DRAG_COEFFICIENT, DRIVE_TRAIN_LOSSES,
        AIR_DENSITY, GRAVITY_CONSTANT, ROLLING_RESISTANCE_COEF);

    User user("Yoann", 60.0, 170, "MALE", FRONTAL_AREA);
    user.setBike(bike);

    circuit.printRoute();
    user.printInfos();

    Simulation simulation(user, circuit);
    simulation.setPower(CURRENT_POWER);
    simulation.start();
    simulation.join();

    return EXIT_SUCCESS;
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