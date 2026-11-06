#pragma once

#include <vector>

#include "Elevator.hpp"


class System {
    private:
        int floorCount_;
        std::vector<Elevator> elevators_;

        int findBestElevator(int from, int to);
    public:
        System (int floorCount, int ElevatorCount);
        void callElevator(int from, int to);
        void update();
        void display();
        ~System();

};