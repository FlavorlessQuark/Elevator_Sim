#include <ctime>
#include <sstream>
#include <iostream>
#include <chrono>
#include "../../incl/System.hpp"
#include "../../incl/Elevator.hpp"

System::System(int floorCount, int elevatorCount) : floorCount_(floorCount) {
    Elevator elevatorDefault = Elevator();
    // Init with "count" copies of _elevatorDefault, most efficient way to init, single allocation isntead of multiple
    elevators_ = std::vector<Elevator>(elevatorCount, elevatorDefault); 
}

void System::callElevator(int from, int to){
    int idx = rand() % elevators_.size();

    if (elevators_[idx].getState().state == Status::IDLE) {
        elevators_[idx].addDestination(from, to);
    } else {
        // If the selected elevator is not idle, you can implement logic to find the best elevator
        // For simplicity, we just print a message here
        std::cout << "Elevator " << idx << " is busy. Please wait." << std::endl;
    }
}

void System::update(){
    static std::chrono::steady_clock::time_point lastTime = std::chrono::steady_clock::now();

    auto currentTime = std::chrono::steady_clock::now();

    auto elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastTime).count();

    // std::cout << "elapsed: " << lastTime << std::endl;
    // std::cout << "elapsed: " << currentTime << std::endl;
    
    if (elapsedMs >= TICK_RATE) {
        // for (int i  = 0; i < elevators_.size(); i++){
        //     elevators_[i].update(elapsedMs);
        // }
        lastTime = currentTime; // Reset the last time to the current time
        display();
    }

}


/* 
 -------------
| ELEVATOR 01 |
|   FLoor 01  |
|   Waiting.. |
 -------------
*/

constexpr int ELEVATOR_WIN_WIDTH = 20;
constexpr int ELEVATOR_WIN_HEIGHT = 3;

void System::display() {
    std::ostringstream screenBuff;
    // screenBuff<< "\033[2J\033[H\033[?25l"; // Clear screen and move cursor to home position
    screenBuff<< "\033[H\033[?25l"; // Clear screen and move cursor to home position

    static int i = 0;

    screenBuff<< "\033[0;2H";
    screenBuff<< "--------------------";
    for (int n = 0; n < ELEVATOR_WIN_HEIGHT; n++) {
        screenBuff<< "\033[" << (n + 2) << ";2H";
        screenBuff<< "|                  |";
    }
    screenBuff<< "\033[" << (ELEVATOR_WIN_HEIGHT + 2) << ";2H";
    screenBuff<< "--------------------";

    std::cout << screenBuff.str();
}

System::~System() {
    // Destructor implementation (if needed)
}
