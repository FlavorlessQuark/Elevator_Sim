#include "System.hpp"
#include <iostream>
#include <functional>
#include <chrono>


int count = 3;
int floors = 10;


void processInput(System& system) {
static std::chrono::steady_clock::time_point lastTime = std::chrono::steady_clock::now();
static int first = 1;
// Inside your update loop:
auto currentTime = std::chrono::steady_clock::now();

// Calculate exact elapsed time in milliseconds
auto elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastTime).count();

// Define how long a single tick lasts (e.g., 50 ms per tick)
const long long msPerTick = 50; 
long long ticksToProcess = elapsedMs / msPerTick;

// Update the timestamp for the next cycle
    int from = rand() % floors;
    srand(currentTime.time_since_epoch().count()); // Seed the random number generator with elapsed time;
    int to = rand() % floors;
    if (first ||ticksToProcess >= 50) {
        lastTime = currentTime; // Reset the last time to the current time
        std::cout << "From " << from << " called to floor " << to << std::endl;
        system.callElevator(from, to);
        first = 0;
    } 

}

void render(std::vector<Elevator>& elevators) {
    std::cout << "\033[2J\033[H"; // Clear screen and move cursor to home position
    for (size_t i = 0; i < elevators.size(); ++i) {
        const Elevator& elevator = elevators[i];
        std::cout << "Elevator " << i + 1 << ": Floor " << elevators[i].getState().currentFloor
                    << " | State: " << static_cast<int>(elevators[i].getState().state) << std::endl;
        }

}

int main(int argc, char* argv[]){
    System system(floors, count);
    int to = 0, from = 0;
    // int count;
    // if (argc != 2) {
    //     std::cout << "Usage: " << argv[0] << " <number_of_elevators>" << std::endl;
    //     return 1;
    // }

    // count = std::stoi(argv[1]);

    std::cout << "\033[2J\033[H\033[?25l";
    while (true) {
        // processInput(system);
        // system.update();
        system.display();
    }
    return 0;
}