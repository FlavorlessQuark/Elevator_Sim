#pragma once

#include <vector>

#include "Elevator.hpp"


class System {
    private:
        int floorCount_;
        std::vector<Elevator> elevators_;

        int _findBestElevator(int from, int to);
        void _displayOne(std::ostringstream& screenBuff, Elevator &elevator, int index, int startRow, int startCol);
        
    public:
        System (int floorCount, int ElevatorCount);
        void callElevator(int from, int to);
        void update(std::string input);
        void display(std::string input);
        void processInput(std::string input);
        ~System();

};