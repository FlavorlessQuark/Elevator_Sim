#include "Elevator.hpp"
#include <vector>
#include <iostream>

Elevator::Elevator() {
    state_ = {
        .currentFloor = 0, 
        .nextDir = Status::UP,
        .state = Status::IDLE,
        .routes = std::vector<std::bitset<MAX_FLOORS>>(3, std::bitset<MAX_FLOORS>())
    };
}

ElevatorState Elevator::getState() {
    return state_;
}

void Elevator::_set(int setIdx, int floor) {
    if (floor < 0 || floor >= MAX_FLOORS) return;
    std::cout << "Setting bit for floor " << floor << " in set " << setIdx << std::endl;
    state_.routes[setIdx].set(floor);
}

void Elevator::_unset(int setIdx, int floor) {
    if (floor < 0 || floor >= MAX_FLOORS) return;
    state_.routes[setIdx].reset(floor);
}

void Elevator::_reset(int setIdx) {
    state_.routes[setIdx].reset();
}

void Elevator::_roll() {
    state_.routes[0] = state_.routes[1];
    state_.routes[1] = state_.routes[2];
    state_.routes[2].reset();
}

int Elevator::_Find_last(std::bitset<MAX_FLOORS>& route) {
    for (int i = MAX_FLOORS - 1; i >= 0; --i) {
        if (route.test(i)) {
            return i;
        }
    }
    return -1;
}

bool Elevator::_CheckAndRoll() {
    std::cout << "Checking and rolling routes..." 
    << " r 0: " << state_.routes[0].count() << ", r 1: " << state_.routes[1].count() << ", r 2: " << state_.routes[2].count()
    << std::endl;
    if (state_.routes[0].any() == true) {
        std::cout << "Exit on 1" << std::endl;
        return true;
    }
    _roll();
    state_.nextDir =  static_cast<Status>(static_cast<char>(state_.nextDir) ^ DIR_MASK);
    if (state_.routes[0].any() == true) {
        std::cout << "Exit on 2" << std::endl;
        return true;
    }
    _roll();
    state_.nextDir =  static_cast<Status>(static_cast<char>(state_.nextDir) ^ DIR_MASK);
    if (state_.routes[0].any() == true) {
        std::cout << "Exit on 3" << std::endl;
        return true;
    }
    _roll();
    state_.nextDir =  static_cast<Status>(static_cast<char>(state_.nextDir) ^ DIR_MASK);
    std::cout << "No more routes to roll." << std::endl;
    return false;
}

ElevatorState Elevator::update(int elapsed) {
    if (state_.state == Status:: IDLE && state_.routes[0].none()) return state_;
    elapsed_ += elapsed; 
    switch (state_.state)
    {
        case Status::IDLE:
            std::cout << "IDLE" << std::endl;
            if (state_.routes[0].any()) {
                state_.state = state_.nextDir;
                std::cout << "Elevator starting to move in direction: " << statusToString(state_.state) << std::endl;
                std::cout<< "GOING TO FLOOR: ";
                if (state_.nextDir == Status::UP) {
                    std::cout << state_.routes[0]._Find_first() << std::endl;
                } else {
                    std::cout << _Find_last(state_.routes[0]) << std::endl;
                }
            }
            elapsed_  = 0;

            break;
        case Status::D_OPEN:
            if (elapsed_ >= TICK_RATE * D_OPEN_TIME) {
                // std::cout << "Elevator closing doors at floor: " << state_.currentFloor << std::endl;
                state_.state = Status::D_CLOSE;
                elapsed_ = 0; // Not sure about this, could be elapsed_ -= D_OPENITME, but error accumulation could lead to state skips
            }
            break ;
        case Status::D_CLOSE:
            if (elapsed_ >= TICK_RATE * D_CLOSE_TIME){
                // std::cout << "Elevator doors closed at floor: " << state_.currentFloor << std::endl;
                if (_CheckAndRoll() == false) {
                    state_.state = Status::IDLE;
                }
                else {
                    state_.state = state_.nextDir;
                    // std::cout << "Elevator starting to move in direction: " << statusToString(state_.state) << std::endl;
                }
                elapsed_ = 0;
            }
            break;
        default:
            int travelled = elapsed_ / (TICK_RATE * FLOOR_DIST);
            if (travelled == 0) break; // Not enough time has passed to move a floor
            elapsed_ = elapsed_ % (TICK_RATE * FLOOR_DIST);

            // std::cout << "Elevator moving in direction: " << statusToString(state_.state) << " | Current floor: " << state_.currentFloor << std::endl;
            if (state_.state == Status::UP) {
                state_.currentFloor += travelled;
                if (state_.nextDir == Status::UP && state_.currentFloor >= state_.routes[0]._Find_first()) {
                    state_.currentFloor = state_.routes[0]._Find_first();
                    state_.state = Status::D_OPEN;
                    elapsed_ = 0;
                    _unset(0, state_.currentFloor);
                }   
            } else if (state_.state == Status::DOWN) {
                state_.currentFloor -= travelled;
                if (state_.nextDir == Status::DOWN  && state_.currentFloor <= _Find_last(state_.routes[0])) {
                    state_.currentFloor = _Find_last(state_.routes[0]);
                    state_.state = Status::D_OPEN;
                    elapsed_ = 0;
                    _unset(0, state_.currentFloor);
                }
            }
            break;
    }
    return state_;
}

void Elevator::addDestination(int from, int to) {
    if (state_.state == Status::IDLE) {
        state_.nextDir = (from >= state_.currentFloor) ? Status::UP : Status::DOWN;
        state_.state = state_.nextDir;
    }

    int bitsetFrom = 0;
    int bitsetTo = 0;

    if (state_.nextDir == Status::UP) {
        if (from >= state_.currentFloor) {
            // Case 1: 'from' is ahead in the UP direction
            bitsetFrom = 0;
            if (to >= from) {
                bitsetTo = 0; // 'to' is further up (same sweep)
            } else {
                bitsetTo = 1; // 'to' is below 'from' (serviced on the way down)
            }
        } else {
            // Case 2: 'from' is behind us (below current floor)
            bitsetFrom = 1; // Reached on the downward return trip
            if (to > from) {
                bitsetTo = 2; // 'to' is above 'from', requiring a final upward pass
            } else {
                bitsetTo = 1; // 'to' is even lower, serviced on the downward trip
            }
        }
    } 
    else { // Status::DOWN
        if (from <= state_.currentFloor) {
            // Case 1: 'from' is ahead in the DOWN direction
            bitsetFrom = 0;
            if (to <= from) {
                bitsetTo = 0; // 'to' is further down (same sweep)
            } else {
                bitsetTo = 1; // 'to' is above 'from' (serviced on the way up)
            }
        } else {
            // Case 2: 'from' is behind us (above current floor)
            bitsetFrom = 1; // Reached on the upward return trip
            if (to < from) {
                bitsetTo = 2; // 'to' is below 'from', requiring a final downward pass
            } else {
                bitsetTo = 1; // 'to' is even higher, serviced on the upward trip
            }
        }
    }

    // Set the bits for both pickup and destination floors
    _set(bitsetFrom, from);
    _set(bitsetTo, to);
}

void Elevator::setDirection(Status dir) {
    state_.nextDir = dir;
}
