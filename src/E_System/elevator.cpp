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
    if (state_.routes[0].any() == true) {
        return true;
    }
    _roll();
    state_.nextDir =  static_cast<Status>(static_cast<char>(state_.nextDir) ^ DIR_MASK);
    if (state_.routes[0].any() == true) {
        return true;
    }
    _roll();
    state_.nextDir =  static_cast<Status>(static_cast<char>(state_.nextDir) ^ DIR_MASK);
    if (state_.routes[0].any() == true) {
        return true;
    }
    _roll();
    state_.nextDir =  static_cast<Status>(static_cast<char>(state_.nextDir) ^ DIR_MASK);
    return false;
}

ElevatorState Elevator::update(int elapsed) {
    if (state_.state == Status:: IDLE && state_.routes[0].none()) return state_;
    elapsed_ += elapsed; 
    switch (state_.state)
    {
        case Status::IDLE:
            if (state_.routes[0].any()) {
                state_.state = state_.nextDir;
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
                state_.state = Status::D_CLOSE;
                elapsed_ = 0;
            }
            break ;
        case Status::D_CLOSE:
            if (elapsed_ >= TICK_RATE * D_CLOSE_TIME){
                if (_CheckAndRoll() == false) {
                    state_.state = Status::IDLE;
                }
                else {
                    state_.state = state_.nextDir;
                }
                elapsed_ = 0;
            }
            break;
        default:
            int travelled = elapsed_ / (TICK_RATE * FLOOR_DIST);
            if (travelled == 0) break;
            elapsed_ = elapsed_ % (TICK_RATE * FLOOR_DIST);

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
            bitsetFrom = 0;
            if (to >= from) {
                bitsetTo = 0;
            } else {
                bitsetTo = 1;
            }
        } else {
            bitsetFrom = 1;
            if (to > from) {
                bitsetTo = 2;
            } else {
                bitsetTo = 1;
            }
        }
    } 
    else {
        if (from <= state_.currentFloor) {
            bitsetFrom = 0;
            if (to <= from) {
                bitsetTo = 0;
            } else {
                bitsetTo = 1;
            }
        } else {
            bitsetFrom = 1;
            if (to < from) {
                bitsetTo = 2;
            } else {
                bitsetTo = 1;
            }
        }
    }

    _set(bitsetFrom, from);
    _set(bitsetTo, to);
}

void Elevator::setDirection(Status dir) {
    state_.nextDir = dir;
}
