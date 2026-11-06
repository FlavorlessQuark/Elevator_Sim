#pragma once
#include <bitset>
#include <string>
#include <vector>

constexpr int FLOOR_DIST = 10;
constexpr int MAX_FLOORS = 20;
constexpr int D_OPEN_TIME = 4;
constexpr int D_CLOSE_TIME = 2;
constexpr int TICK_RATE = 50; // 50 ms per tick

enum Status {
    IDLE,
    UP,
    DOWN,
    D_OPEN,
    D_CLOSE
};

static inline std::string statusToString(Status s) {
    switch (s) {
        case Status::IDLE:    return "Waiting...  ";
        case Status::UP:      return "UP ^        ";
        case Status::DOWN:    return "DOWN V      ";
        case Status::D_OPEN:  return "Doors Open  ";
        case Status::D_CLOSE: return "Doors Closed";
    }
    return "IDLE   ";
}

struct ElevatorState
{
    int currentFloor;
    Status nextDir;
    Status state;
    std::vector<std::bitset<MAX_FLOORS>> routes;
};

class Elevator {
    private:
        char DIR_MASK = (char)(Status::UP) | (char)(Status::DOWN);
        ElevatorState state_;
        int elapsed_ = 0;

        void _set(int setIdx, int floor);
        void _unset(int setIdx, int floor);
        void _reset(int setIdx);
        void _roll();
        int _Find_last(std::bitset<MAX_FLOORS>& route);
        bool _CheckAndRoll();
            
    public:
        Elevator();
        ElevatorState getState();
        ElevatorState update(int elapsedTime);
        void addDestination(int from, int to);
        void setDirection(Status dir);

};