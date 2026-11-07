#include <ctime>
#include <sstream>
#include <iostream>
#include <chrono>
#include <iomanip>
#include "../../incl/System.hpp"
#include "../../incl/Elevator.hpp"

System::System(int floorCount, int elevatorCount) : floorCount_(floorCount) {
    Elevator elevatorDefault = Elevator();
    elevators_ = std::vector<Elevator>(elevatorCount, elevatorDefault); 
}

void System::callElevator(int from, int to){
    int idx = rand() % elevators_.size();

    if (elevators_[idx].getState().state == Status::IDLE) {
        elevators_[idx].addDestination(from, to);
    } else {
        std::cout << "Elevator " << idx << " is busy. Please wait." << std::endl;
    }
}

void System::update(std::string input){
    static std::chrono::steady_clock::time_point lastTime = std::chrono::steady_clock::now();

    auto currentTime = std::chrono::steady_clock::now();

    auto elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastTime).count();

    if (elapsedMs >= TICK_RATE) {
        for (int i  = 0; i < elevators_.size(); i++){
            elevators_[i].update(elapsedMs);
        }
        display(input);
        lastTime = currentTime;
    }

}


constexpr int ELEVATOR_WIN_WIDTH = 20;
constexpr int ELEVATOR_WIN_HEIGHT = 3;

void System::_displayOne(std::ostringstream& screenBuff, Elevator &elevator, int index, int startRow, int startCol) {


    int boxWidth = 20;

    screenBuff << "\033[" << startRow << ";" << startCol << "H" << "--------------------";

    for (int n = 0; n < ELEVATOR_WIN_HEIGHT; n++) {
        screenBuff << "\033[" << (startRow + 1 + n) << ";" << startCol << "H" << "|                  |";
    }

    screenBuff << "\033[" << (startRow + ELEVATOR_WIN_HEIGHT + 1) << ";" << startCol << "H" << "--------------------";

    std::ostringstream L1, L2, L3;
    L1 << "ELEVATOR " << std::right << std::setw(3) << index;
    L2 << "Floor " << std::right << std::setw(3) << elevator.getState().currentFloor; 
    L3 << statusToString(elevator.getState().state);

    int boxInnerStartCol = startCol + 1;
    int boxInnerWidth = 18;

    int c1 = boxInnerStartCol + (boxInnerWidth - static_cast<int>(L1.str().size())) / 2;
    int c2 = boxInnerStartCol + (boxInnerWidth - static_cast<int>(L2.str().size())) / 2;
    int c3 = boxInnerStartCol + (boxInnerWidth - static_cast<int>(L3.str().size())) / 2;

    screenBuff << "\033[" << (startRow + 1) << ";" << c1 << "H" << L1.str();
    screenBuff << "\033[" << (startRow + 2) << ";" << c2 << "H" << L2.str();
    screenBuff << "\033[" << (startRow + 3) << ";" << c3 << "H" << L3.str();
}


void System::display(std::string input) {
     std::ostringstream screenBuff;
    
    screenBuff << "\033[H\033[?25l";

    int boxWidth = 20;     
    int boxSpacing = 3;    
    int boxHeight = ELEVATOR_WIN_HEIGHT + 2; 
    int rowSpacing = 1;    

    int elevatorsPerRow = TERMIN_W / (boxWidth + boxSpacing);
    if (elevatorsPerRow < 1) elevatorsPerRow = 1;

    for (size_t i = 0; i < elevators_.size(); ++i) {
        int gridRow = i / elevatorsPerRow;
        int gridCol = i % elevatorsPerRow;

        int startRow = 1 + gridRow * (boxHeight + rowSpacing);
        int startCol = 1 + gridCol * (boxWidth + boxSpacing);

        _displayOne(screenBuff, elevators_[i], i, startRow, startCol);
    }

    screenBuff << "\n\n\nINPUT: $ ";
    for (char character : input) {
        screenBuff << character;
        if (character == '\n') {
            screenBuff << "INPUT: $ ";
        }
    }

    std::cout << screenBuff.str() << std::flush;
}

void System::processInput(std::string input) {
    std::stringstream ss(input);
    int floor, dest;
    char comma;

    ss >> floor >> comma >> dest;
    if (ss.fail() || comma != ',') {
        return;
    }
    if (
        floor == dest ||
        floor < 0 ||
        dest < 0 ||
        floor > MAX_FLOORS ||
        dest > MAX_FLOORS
    ) {
        return;
    }
    callElevator(floor, dest);
}

System::~System() {
}
